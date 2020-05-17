
fib(n) {
	if(n <= 1)
		return n;
	
	return fib(n - 1) + fib(n - 2);
}

putchar(x) {
	asm("rcl r1, r4");
	asm("out r1");
}

main() {
	putchar(fib(4));
}