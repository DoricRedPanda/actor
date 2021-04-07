int counter;

int main()
{
	counter = 0;
	goto SKIP;
	counter = counter + 1;
SKIP:
	counter = counter + 1;
}
