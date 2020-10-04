
getnumb() {
    auto a, c, x, z 1, s;
    s = &s + 1;
    while (isdigit(c = getchar()) && a < 10) s[a++] = c;
    for (0; a--; z =* 10) x =+ z * (s[a] - 48);
    return x;
}

putchar(c) {
    asm("rcl r1, r4");
    asm("out r1");
}

main(void) {
    putchar(getnumb());
}
