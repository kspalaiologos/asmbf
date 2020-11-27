
function include(path)
    local M = {}

    -- Lua 5.1 and 5.2 compat
    local load = pcall(load, '') and load or function(ld, source, mode_, env)
      local f, err = loadstring(ld, source)
      if not f then return f, err end
      return setfenv(f, env or _G)
    end

    -- Count number of chars c in string s.
    local function countchar(s, c)
      local count = 0
      local i = 1
      while true do
        i = string.find(s, c, i)
        if i then count = count + 1; i = i + 1 else break end
      end
      return count
    end

    -- In error message string, translate line numbers from
    -- processed file to source file.
    -- linenums is translation array (processed line number ->
    -- source line number) or source line number.
    local function fix_linenums(message, linenums)
      message = message:gsub("(%b[]:)(%d+)", function(a,n)
        n = tonumber(n)
        local source_linenum =
          type(linenums) == "table" and (linenums[n] or '?') or
          type(linenums) == "number" and linenums + n - 1 or
          '?'
        return a .. source_linenum
      end)
      return message
    end


    -- Expands $(...) syntax.
    local function parse_dollar_paren(pieces, chunk, name, linenum)
      local is = 1
      for ibegin, iend in chunk:gmatch("()$%b()()") do
        local text = chunk:sub(is, ibegin - 1)
        local executed = chunk:sub(ibegin+2, iend-2) -- remove parens

        local name2 = name .. ":" .. executed
        linenum = linenum + countchar(text, '\n')
        local may_have_comment = executed:find("%-%-")
        local nl = may_have_comment and "\n" or ""

        pieces[#pieces+1] = ("_put(%q)"):format(text)
        if load("return " .. executed, name2) then -- is expression list
          pieces[#pieces+1] = "_put(" .. executed .. nl .. ")"
        else -- assume chunk
          local status, message = load(executed, name2)
          if not status then -- unrecognized
            if message then
              message = fix_linenums(message, linenum)
            end
            return status, message
          end
          pieces[#pieces+1] = " " .. executed .. nl .. " "
          linenum = linenum + countchar(executed, '\n')
        end
        is = iend
      end
      pieces[#pieces+1] = ("_put(%q)"):format(chunk:sub(is))
      return true
    end

    -- Expands #... syntax.
    local function parse_hash_lines(chunk, name, env)
      local pieces = {}

      local luas = {} -- for improved error reporting
      local linenums = {}
      local linenum = 1

      pieces[#pieces+1] = "local _put = ... "

      local is = 1
      while true do
        local _, ie, lua = chunk:find("^#+([^\n]*\n?)", is)
        if not ie then
          local iss; iss, ie, lua = chunk:find("\n#+([^\n]*\n?)", is)
          local text = chunk:sub(is, iss)
          local status, message = parse_dollar_paren(pieces, text, name, linenum)
          if not status then return status, message end
          if not ie then break end
          linenum = linenum + countchar(text, '\n')
        end

        luas[#luas+1] = lua
        linenums[#linenums+1] = linenum
        linenum = linenum + 1

        pieces[#pieces+1] = ' ' .. lua .. ' '

        is = ie + 1
      end
     
      local code = table.concat(pieces, ' ')

      -- Attempt to compile.
      local f, message = load(code, name, 't', env)
      if not f then
        -- Attempt to compile only user-written Lua
        -- (for cleaner error message)
        local lua = table.concat(luas)
        local f2, message2 = load(lua, name, 't', env)
        if not f2 then
          message = fix_linenums(message2, linenums)
        else -- unexpected
          message = fix_linenums(message, nil)
        end
      end

      return f, message
    end

    -- Abstraction of string output stream.
    local function string_writer()
      local t = {}
      local function write(...)
        local n = select('#', ...)
        if n > 0 then
          t[#t+1] = tostring((...))
          write(select(2, ...))
        end
      end
      local function close()
        return table.concat(t)
      end
      return {write=write, close=close}
    end

    -- Abstraction of file output stream.
    local function file_writer(fh, is_close)
      local function write(...)
        local n = select('#', ...)
        if n > 0 then
          fh:write(tostring((...)))
          write(select(2, ...))
        end
      end
      local function close()
        if is_close then fh:close() end
      end
      return {write=write, close=close}
    end

    -- Convert output specification to output stream.
    -- A helper function for C<preprocess>.
    local function make_output(output)
      if type(output) == 'string' then
        output = string_writer()
      elseif type(output) == 'table' then
        assert(#output == 1, 'table size must be 1')
        local filename = output[1]
        local fh, message = io.open(filename, 'w')
        if not fh then return false, message end
        output = file_writer(fh, true)
      elseif io.type(output) == 'file' then
        output = file_writer(output, false)
      else
        error('unrecognized', 2)
      end
      return output
    end

    -- Convert input specification to input stream.
    -- A helper function for C<preprocess>.
    local function make_input(input)
      if type(input) == 'string' then
        input = {text = input, name = 'source'}
      elseif type(input) == 'table' then
        assert(#input == 1, 'table size must be 1')
        local filename = input[1]
        local fh, message = io.open(filename)
        if not fh then return false, message end
        input = {text = fh:read'*a', name = filename}
        fh:close()
      elseif io.type(input) == 'file' then
        input = {text = input:read'*a', name = nil}
      else
        error('unrecognized', 2)
      end
      return input
    end

    function M.preprocess(t)
      if type(t) == 'string' then t = {input = t} end
      local input = t.input or io.stdin
      local output = t.output or
                 (type(input) == 'string' and 'string') or io.stdout
      local lookup = t.lookup or _G
      local strict = t.strict; if strict == nil then strict = true end

      local err; input, err = make_input(input)
      if not input then error(err, 2) end
      
      local name = input.name or "<source>"

      local mt = {}
      if strict then
        function mt.__index(t,k)
          local v = lookup[k]
          if v == nil then
            error("Undefined global variable " .. tostring(k), 2)
          end
          return v
        end
      else
        mt.__index = lookup
      end
      
      local env = {}
      setmetatable(env, mt)
      
      local f, message = parse_hash_lines(input.text, name, env)
      if not f then return f, message end

      output = make_output(output)

      local status, message = pcall(f, output.write)

      local result = output.close()
      if not result then result = true end

      if not status then
        return false, message
      else
        return result
      end
    end

    local input, output
        input = io.open(path, "r")
        output = io.stdout
      local status, message = M.preprocess{input=input, output=output, lookup=_G}
      if not status then
        io.stderr:write(message .. "\n")
        os.exit(1)
      end
end

RET_ID = 0
function call(x)
    print("psh %_return_" .. RET_ID)
    print("jmp %" .. x)
    print("@_return_" .. RET_ID)
    RET_ID = RET_ID + 1
end

MM_BASE = 0
PAGE_SIZE = 16

function alloc(target_register, clear)
    print("psh " .. clear)
    print("mov " .. target_register .. ", " .. tostring(MM_BASE))
    print("mov " .. clear .. ", 1")
    print("nav " .. clear)
    print("raw .[")
    print("rcl " .. clear .. ", " .. target_register)
    print("add " .. target_register .. ", " .. tostring(PAGE_SIZE))
    print("inc " .. target_register)
    print("nav " .. clear)
    print("raw .]")
    print("sub " .. target_register .. ", " .. tostring(PAGE_SIZE))
    print("dec " .. target_register)
    print("sto " .. target_register .. ", 1")
    print("inc " .. target_register)
    print("pop " .. clear)
end

function emit(code)
    code:gsub(".", function(c)
        print("raw ." .. c)
    end)
end

function free(target_register)
    print("dec " .. target_register)
    print("sto " .. target_register .. ", 0")
end

function times(str, n)
    for i=1,n,1 do print(str) end
end

function signed(x)
    if x < 0 then
        return (-x) * 2 + 1
    elseif x == 0 then
        return 0
    else
        return x * 2
    end
end

function gen_text(str)
    local function grade(n, base)
        local sp = 0
        local norm = 0
      
        while n > 0 do
            sp = sp + 1
            norm = norm + n % base
            n = math.floor(n / base)
        end
      
        return norm + (6 + base) * sp + (((sp % 2 == 1) and 4) or 0)
    end
      
    local function best_base(n)
        local v = 0
        local b = 0
      
        for i = 2, 60, 1 do
            local cv = grade(n, i)
    
            if v == 0 or v > cv then
                v = cv
                b = i
            end
        end
      
        return b
    end
    
    local function translate(n)
        local stack = {}
        local sp = 0
        local flip = 1
        local out = '>'
    
        if n < 12 then
            out = ''
            
            for i = 1, n, 1 do
                out = out .. '+'
            end
    
            return out
        end

        local base = best_base(n)
    
        while n > 0 do
            stack[sp] = n % base
            sp = sp + 1
            n = math.floor(n / base)
        end
    
        while sp > 0 do
            sp = sp - 1
            local bc = base
            
            while stack[sp] > 0 do
                out = out .. '+'
                stack[sp] = stack[sp] - 1
            end
    
            if sp > 0 then
                if not flip then
                    out = out .. '[>'
                    while bc > 0 do
                        out = out .. '+'
                        bc = bc -1
                    end
                    out = out .. '<-]>'
                else
                    out = out .. '[<'
                    while bc > 0 do
                        out = out .. '+'
                        bc = bc -1
                    end
                    out = out .. '>-]<'
                end
            end
    
            flip = not flip
        end
    
        if not flip then
            out = out .. "[-<+>]<"
        end
    
        return out
    end

    -- move to r1
    print("nav r1")
    -- move to `c'
    emit("<<<")
    
    -- The compiler now thinks we're in r1, so if we
    -- mov something to r1, it ends up in `c'.

    local last = 0

    str:gsub(".", function(c)
        local h = string.byte(c)

        if h == last then
            emit(".")
        else
            if h > last and h - last < 10 then
                for i = 1, h - last, 1 do
                    emit('+')
                end
            elseif last > h and last - h < 10 then
                for i = 1, last - h, 1 do
                    emit('-')
                end
            else
                if last ~= 0 then
                    emit("[-]")
                end

                emit(translate(h))
            end

            emit(".")
            last = h
        end
    end)

    emit("[-]")

    -- move back to r1.
    emit(">>>")
end

include(os.getenv("HOME") .. '/.asmbf/lib/lib-def.lua')
