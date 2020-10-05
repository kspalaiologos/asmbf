$(
    function test(x)
        print("mov r1, " .. signed(x))
        print("s02 r1")
        print("eq r1, " .. signed(-x))
        print("add r1, .0")
        print("out r1")
    end
)

#test(0)
#test(1)
#test(2)
#test(3)
#test(4)
#test(5)
#test(6)
#test(7)
