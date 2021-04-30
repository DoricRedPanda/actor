int
fib(int n)
{
	int foo, bar;

	if (n <= 1)
		return n;
	foo = fib(n - 1);
	bar = fib(n - 2);
	foo = foo + bar;
	return foo;
}

int
main()
{
	int n;
	n = 30;
	write fib(n);
	return 0;
}
