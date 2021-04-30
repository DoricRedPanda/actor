int counter;

int main()
{
	counter = 0;
	if (!counter)
		counter = 1;
	write counter;
	if (counter)
		counter = 42;
	write counter;
	if (!counter)
		;
	else
		write counter;
	return 0;
}
