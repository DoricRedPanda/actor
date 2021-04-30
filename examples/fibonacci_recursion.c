int
fib(int n)
{
	if (n <= 1)
		return n;
	return fib(n - 1) + fib(n - 2);
}

int
main()
{
	int n;
	n = 34;
	n = fib(n);
	return 0;
}
