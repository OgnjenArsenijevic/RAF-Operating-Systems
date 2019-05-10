#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define UTIL_IMPLEMENTATION
#include "utils.h"

#define BUF_SIZE 129

int main(int argc, char *argv[])
{
	/* ucitavanje podesavanja */
	char scancodeFile[BUF_SIZE];
	char mnemonicsFile[BUF_SIZE]="ctrl.map";
	printstr("Unesite ime datoteke koja sadrzi scancode vrednosti:\n");
	read(0,scancodeFile,BUF_SIZE);
	if(scancodeFile[strlen(scancodeFile)-1]=='\n')
		scancodeFile[strlen(scancodeFile)-1]='\0';
	load_config(scancodeFile,mnemonicsFile);
	/* ponavljamo: */
		/* ucitavanje i otvaranje test fajla */
		/* parsiranje fajla, obrada scanecodova, ispis */
	char testFile[BUF_SIZE];
	int flag=1;
	while(flag==1)
	{
		int rr;
		for(rr=0;rr<BUF_SIZE;rr++)
			testFile[rr]='\0';
		printstr("Unesite ime test datoteke:\n");
		read(0,testFile,BUF_SIZE);
		if(testFile[strlen(testFile)-1]=='\n')
			testFile[strlen(testFile)-1]='\0';
		if(strcmp(testFile,"exit")==0)
			break;	
		int fd=open(testFile, O_RDONLY);
		if(fd==-1)
		{
			printerr("Test fajl neuspesno otvoren!\n");
			printstr("---------------------------- \n");
			continue;
		}
		char buffer[BUF_SIZE];
		fgets(buffer,BUF_SIZE,fd);
		int num=atoi(buffer);
		while(num!=400)
		{
			char text[65];
			int i;
			for(i=0;i<65;i++)
				text[i]='\0';
			int len=process_scancode(num, text);
			if(len>0)
				printstr(text);
			fgets(buffer,BUF_SIZE,fd);
			num=atoi(buffer); 
		}

		printstr("\n---------------------------- \n");
	}	
	_exit(0);
}
