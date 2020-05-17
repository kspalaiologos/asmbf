getchar() {
    asm("in_ r1");
}

putchar(c) {
    asm("rcl r1, r4");
    asm("out r1");
}

s 0; x 0;
y 0; i 0;

main() {
    auto c 1, b 0, q;
    s = 1 << (getchar() - 48);
    y = s - 1;

    while (c) {
        i = 0;
        while (i < y) {
            putchar(' ');
            i++;
        }

        x = 0;
        while (x + y < s) {
            if (x & y) {
                putchar(' ');
            } else {
                putchar('*');
            }
            x++;
            putchar(' ');
        }
        putchar(10);
        if (b) {
            c = 0;
        }
        if (--y == 0) {
            b = 1;
        }
    }
}