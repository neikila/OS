//программа перекодировки текста кои8 в windows файл
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

int diag (char*,const char*,int);//функция диагностики
unsigned char* recode (int);//функция перекодировки
static unsigned char buf [BUFSIZ];//буфер чтения_записи
int main (int argc, char* argv[])
{//основная функция
  	int fds;//файловый дескриптор исходного кои_8 
  	int fdt;//файловый дескриптор результирующего windows текста
  	int num;
  //проверка числа аргументов в командной строке
  	if (argc<2)
    		exit (diag (argv [0], "Source target file name?",127));
	if (argc<3)
   		exit (diag (argv [0], "target file name?",63));

	

	if((fds=open(argv[1],0))<0)//открыть файл исх. кои
  		exit (diag(argv[1],sys_errlist[errno],errno));
	if(argc==4 && argv[3][0]=='-' && argv[3][1]=='f' && access(argv[2],0)==0)
		unlink(argv[2]);
	if(access(argv[2],0)==0)
  		exit(diag(argv[2],"target file already exists",255));
  	errno=0;
 	if((fdt=creat(argv[2],0644))<0)
   		exit (diag(argv[2],sys_errlist[errno],errno));
  	while ((num=read(fds,buf,BUFSIZ))>0)
	{
    		write (fdt,recode(num),num);
  	}
    	close (fdt);
    	close (fds);
    	exit (0);
} 
  
unsigned char* recode (int n){
	unsigned char c;
	int i=0;
    	static char tab [32]={
      		30,0,1,22,4,5,20,3,21,8,9,10,11,12,13,14,
      		15,31,16,17,18,19,6,2,28,27,7,24,29,25,23,26
    		};
    
    	for(i=0; i<n; i++)
	{
      		c=buf[i];
      		if (c<192)
			continue;
      		if (c<224)
			buf[i]=tab[c-192]+224;
      		else
			buf[i]=tab[c-224]+192;
    	}
    	return (buf);
}

 
int diag(char* name,const char* mes, int code)
{
    	int len;
    	strcpy(buf,name);
    	strncat(buf,":",2);
    	strcat(buf,(const char*) mes);
   	len=strlen((const char*) buf);
    	buf[len]='\n';
    	write(2,buf,len+1);
    	return(code);
}
