#include <stdio.h> 
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>

static unsigned char pattern[80], PatternEdited[80];//буфер для образца
int textmode(int);//установка териминального режима
int getch1();//прозрачный ввод символов
int random1(int);//заполнение образца
void PatternWithMisses (int, int, int*);
int move(char, int, int );

/*основная функция тренажера клавиатуры*/
int main (int argc, char* argv[])
{
	if( argc < 3 )
	{
			printf("Error. Lack of arguments\n");
			return 0;
	}
	int len;//длина образца
	int amount;//количествоо пропусков
	int err=0;//счетчик ошибок
	int i=0;//счетчик символов образца
	char c;//код символа ввода
	/*фиксировать длину образца*/
	len=atoi(argv[1]);
	amount = atoi(argv[2]);
	if( len < 0 || amount < 0 || len < amount)
	{
		printf("Error. Wrong Arguments.\n");
		return 0;
	}
	/*заполнить образец случайными символами*/
	random1(len);
	/*печать образца*/
	write(1,pattern,len);
	write(1,"\n",1);
	int mas[amount];
	PatternWithMisses( len, amount, mas );
	write(1, PatternEdited, len);
	/*установить прозрачный режим ввода-вывода*/
	textmode (0);
	/*цикл копирования образца*/
	unsigned char ch[3];
	int delta = len;
	while (amount){
		ch[0]=ch[1]=ch[2]='\0';
		if ( read(0, ch, 3) < 2)
		{
			if( ch[0] == 27 )
			{
				break;
			}
			if( PatternEdited[delta]==' ' && ch[0]==pattern[delta] )//проверка совпадения
			{
				PatternEdited[delta]=ch[0];
				write(1,&ch[0],1);
				delta++;
				amount--;
			}
		}
		else
			delta = move(ch[2],delta, len);
		tcflush (0,TCSAFLUSH);	
	}//while
	write(1,"\n",1);
	//восстановить исходный режим ввода-вывода
	textmode(1);
	return(err);//возврат числа ошибок
}//main


int move(char ch, int delta, int len)
{
	switch (ch) 
	{
		case 67:
			printf ("%c[1C", 27);
			fflush (stdout);
			if ( delta < len - 1 ) 
			{
				delta += 1;
			}
			break;
		case 68:
			printf ("%c[1D", 27);
			fflush (stdout);
			if ( delta > 0 )
			{
				delta -= 1;
			}
			break;
	};
	return delta;
}

void PatternWithMisses(int len, int amount, int* mas)
{
	int i = 0, j;
	srand( getpid() );
	while ( i < amount )
	{
		mas[i] = rand()%len;
		for ( j = 0; ; ++j )
		{
			if ( mas[j] == mas[i] )
			{
				break;
			}
		}
		if ( i == j )
		{
			++i;
		}
	}
	for ( i = 0; i < amount; ++i )
	{
		PatternEdited[ mas[i] ] = ' ';
	}
}

//настройка режима терминального ввода-вывода
int textmode(int mode){
	static struct termios con[2];
	//восстановить исходный режим	терминального ввода-вывода
	if (mode>0)
		return (tcsetattr(0,TCSAFLUSH,&con[1]));
	//получить и сохранить текущие настройки терминала
	tcgetattr(0,&con[0]);
	tcgetattr(0,&con[1]);
	//установить неканонический режим ввода
	//отключить эхо-печать и обработку клавиатурных сигналов
	con[0].c_lflag &=~(ICANON|ECHO|ISIG);
	//отключить усечение входных символов до 7 бит и старт-стопное упр. i/o
	con[0].c_lflag &=~(ISTRIP|IXON|IXOFF);
	//установить 8-ми битный режим вывода
	con[0].c_lflag|=CS8;
	/*установить минимальное число ожидаемых символов и максимальное время ожидания ввода*/
	con[0].c_cc[VMIN]=3;//запрос ввода завершается при получении
	con[0].c_cc[VTIME]=1;//2-х символов или одного символа за 0.1 сек
	/*активировать режим терминального ввода-вывода по установленным атрибутам*/
	return tcsetattr(0,TCSAFLUSH,&con[0]);
}//textmode


//случайное заполнение образца
int random1 (int len){
	int i=0;//счетчик образца
	int r;//случайный код
	/*установить зерно случайной последовательности*/
	srand (getpid ());
	/*цикл заполнения образца случайными байтами*/
	while (i<len){
		r=rand()%(127-33);
		PatternEdited[i]=pattern[i]=r+33;
		++i;
	}//while
	return (0);
}//random
