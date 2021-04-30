int fib[256];

int
main()
{
	int n, i;
	n = 30;
	fib[0] = 0;
	fib[1] = 1;
	if (n < 0 || n >= 256) {
		write -1;
	} else {
		i = 1;
		while (i < n) {
			fib[i + 1] = fib[i] + fib[i - 1];
			i = i + 1;
		}
		write fib[n];
	}
	return 0;
}
