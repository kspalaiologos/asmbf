
$(

function slurp(file)
    local f = assert(io.open(file, "rb"))
    local content = f:read("*all")
    f:close()
    return content
end

function include(path)
	-- so essentially we'd like to pipe the output
	-- of /bin/bfpp and return it
	
	local file = assert(io.popen('/bin/bfpp', 'rw'))
	file:write(slurp(path))
	local output = file:read('*all')
	file:close()
	return output
end

)
