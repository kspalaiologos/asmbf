
$(
    function auto(a, b, op, c)
        print("mov r1, " .. signed(a))
        print("mov r2, " .. signed(b))
        print("sg" .. op .. " r1, r2")
        print("eq r1, " .. signed(c))
        print("eq r2, " .. signed(b))
        print("and r1, r2")
        print("add r1, .0")
        print("out r1")
    end
)

#auto(5, 5, "mul", 25)
#auto(-5, 5, "mul", -25)
#auto(5, -5, "mul", -25)
#auto(-5, -5, "mul", 25)

#auto(-1, 0, "mul", 0)
#auto(0, -1, "mul", 0)
#auto(0, 0, "mul", 0)

#auto(1, 1, "mul", 1)
#auto(-1, 1, "mul", -1)
#auto(1, -1, "mul", -1)
#auto(-1, -1, "mul", 1)

#auto(2, 3, "mul", 6)
#auto(-2, 3, "mul", -6)
#auto(2, -3, "mul", -6)
#auto(-2, -3, "mul", 6)

#auto(8, 2, "div", 4)
#auto(8, -2, "div", -4)
#auto(-8, 2, "div", -4)
#auto(-8, -2, "div", 4)

#auto(2, 2, "div", 1)
#auto(2, -2, "div", -1)
#auto(-2, 2, "div", -1)
#auto(-2, -2, "div", 1)

#auto(0, -2, "div", 0)
#auto(0, 2, "div", 0)

#auto(8, 2, "div", 4)
#auto(8, -2, "div", -4)
#auto(-8, 2, "div", -4)
#auto(-8, -2, "div", 4)

#auto(2, 2, "div", 1)
#auto(2, -2, "div", -1)
#auto(-2, 2, "div", -1)
#auto(-2, -2, "div", 1)

#auto(8, 2, "mod", 0)
#auto(9, 2, "mod", 1)

#auto(-9, 2, "mod", -1)
#auto(-9, -2, "mod", 1)
