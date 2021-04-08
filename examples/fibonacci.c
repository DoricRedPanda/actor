int n, i, foo, bar, baz, res;

int
main()
{
	n = 30;
	foo = 0;
	bar = 1;
	if (n < 0) {
		res = -1;
		goto FINISH;
	}
	if (n == 0) {
		res = foo;
		goto FINISH;
	}
	if (n == 1) {
		res = bar;
		goto FINISH;
	}
	n = n - 1;
	i = 0;
	while (i < n) {
		baz = bar;
		bar = foo + bar;
		foo = baz;
		i = i + 1;
	}
	res = bar;
FINISH:
	write res;
}
