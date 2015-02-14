/* The dice */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>

typedef void (*sighandler) (int);
sigjmp_buf pos;
void brake (int);
static char digit[3];
static char* who[] = { "Man: ", "Com: "};
static int res[2];

int main (int argc, char** argv){
	int timeout, timestart;
	int j = 0;
	if (argc < 3){
		write (2, "Usage: dice timeout\n", 20);
		exit (0);
	} /* if */
	char* temp = malloc(sizeof(char)*5);
	if ((timestart = atoi(argv[1])) <= 0)
		timestart = 0;
	if ((timeout = atoi(argv[2])) < 1)
		timeout = 1;

	digit[0] = '1' + rand() % 6;
	digit[1] = '\b';
	digit[2] = '\n';
	if (((j = sigsetjmp(pos,1)) == 0 || j == 2))
	{
		signal (SIGINT, SIG_IGN);
		signal (SIGALRM, (sighandler) brake);
		alarm(timestart);
	}
	else
	{
		if ( j == 1)
			signal (SIGINT, (sighandler) brake);
		signal (SIGALRM, (sighandler) brake);
		alarm(timeout);	
	}
	if (j == 0 || j == 2 )
	{
		write(1, who[j/2], 5);
		if (timestart == 0)
			brake(1);
	}
	while (j < 4){
		write (1, digit, 2);
		digit[0] = (digit[0] < '6') ? digit[0] + 1 : '1';
		usleep(50000);
	} /* while-j */
	if (res[0] > res[1])
		printf("Winner: Man\n");
	if (res[0] < res[1])
		printf("Winner: Com\n");
	if (res[0] == res[1])
		printf("Draw\n");
	
	signal (SIGINT, SIG_DFL);
	exit(0);
}

void brake (int c){
	static int j = 0;
	signal(SIGINT, SIG_IGN);
	if ( j % 2 == 1 ) write (1, digit, 3);
	res[j / 2] = *digit;
	siglongjmp (pos, ++j);
	return;
}
