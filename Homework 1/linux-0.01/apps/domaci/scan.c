#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "utils.h"

#define BUF_SIZE 129
#define MNEM_SIZE 65
#define TEST_SIZE 2
/*
	Deklaracije za nizove i tabele ovde
	tabelu za prevodjenje scancodeova i tabelu
	za cuvanje mnemonika cuvati kao staticke
	varijable unutar ovog fajla i ucitavati
	ih uz pomoc funkcije load_config

*/

static int ctrl=0;
static int alt=0;
static int shift=0;
static char scancodes[BUF_SIZE];
static int scancodeSz;
static int scancodeShiftSz;
static char scancodeShift[BUF_SIZE];
static char scancodeMnemonics[BUF_SIZE][MNEM_SIZE];
static char scancodeShiftMnemonics[BUF_SIZE][MNEM_SIZE];
static int altCode[BUF_SIZE];
static int altCodeShift[BUF_SIZE];
static int altNum=0;

int convertToDigit(char c)
{
	int num=(int)(c-'0');
	return ((num>=0 && num<=9) ? num : -1 );
}

void load_config(const char *scancodes_filename, const char *mnemonic_filename)
{
	int fd=open(scancodes_filename, O_RDONLY);
	char buffer[BUF_SIZE];
	char bu[BUF_SIZE];
	if(fd==-1)
	{
		printerr("Fajl sa scan-kodovima neuspesno otvoren!\n");
		_exit(1);
	}
	fgets(buffer,BUF_SIZE,fd);
	scancodeSz=strlen(buffer);
	scancodeSz--; //smanjujemo za 1 da ne bi smo racunali karakter za novi red
	int i,j;	
	for(i=0;i<scancodeSz;i++)
		scancodes[i]=buffer[i];
	fgets(buffer,BUF_SIZE,fd);
	scancodeShiftSz=strlen(buffer);
	if(buffer[scancodeShiftSz-1]=='\n')
		buffer[scancodeShiftSz--]='\0';
	for(i=0;i<scancodeShiftSz;i++)
		scancodeShift[i]=buffer[i];
	for(i=0;i<scancodeSz;i++)
		altCode[i]=convertToDigit(scancodes[i]);
	for(i=0;i<scancodeShiftSz;i++)
		altCodeShift[i]=convertToDigit(scancodeShift[i]);

	//ucitavanje memonika
	fd=open(mnemonic_filename, O_RDONLY);
	if(fd==-1)
	{
		printerr("Fajl sa mnemonicima neuspesno otvoren!\n");
		_exit(1);
	}
	fgets(buffer,BUF_SIZE,fd);
	int numOfMnemonics=atoi(buffer);
	for(i=0;i<129;i++)
	{
		for(j=0;j<65;j++)
		{
			scancodeMnemonics[i][j]='\0';
			scancodeShiftMnemonics[i][j]='\0';
		}
	}
	int row;	
	for(row=0;row<numOfMnemonics;row++)
	{
		fgets(buffer,BUF_SIZE,fd);
		char toSearch=buffer[0]; //karakter koji trazimo da li postoji medju scancodovima, ili siftovanim scancodovima
		for(i=0;i<scancodeSz;i++)
		{
			if(scancodes[i]==toSearch)
			{
				int idx=0;
				for(j=2;j<strlen(buffer);j++)
					scancodeMnemonics[i][idx++]=buffer[j];
				if(scancodeMnemonics[i][idx-1]=='\n')
					idx--;
				scancodeMnemonics[i][idx]='\0';
			}
		}
		for(i=0;i<scancodeShiftSz;i++)
		{
			if(scancodeShift[i]==toSearch)
			{
				int idx=0;
				for(j=2;j<strlen(buffer);j++)
					scancodeShiftMnemonics[i][idx++]=buffer[j];
				if(scancodeShiftMnemonics[i][idx-1]=='\n')
					idx--;
				scancodeShiftMnemonics[i][idx]='\0';
			}
		}
	}
}


int process_scancode(int scancode, char *buffer)
{
	int result=0;

	/*
		Your code goes here!
		Remember, only inline assembly.
		Good luck!
	*/

	//Setovanje flag-a koji nam oznacava da li je shift trenutno pritisnut
	__asm__ __volatile__ 
	(
		"cmpl $200, %%eax;"
		"je CON1;"
		"cmpl $300, %%eax;"
		"jne CON2;"
		"CON1:;"
		"xorl $1, %%edx;"
		"CON2:;"
		: "=d" (shift)
		: "a" (scancode), "d" (shift)
		:		
	);

	//Setovanje flag-a koji nam oznacava da li je ctrl trenutno pritisnut 
	__asm__ __volatile__ 
	(
		"cmpl $201, %%eax;"
		"je CON3;"
		"cmpl $301, %%eax;"
		"jne CON4;"
		"CON3:;"
		"xorl $1, %%edx;"
		"CON4:;"
		: "=d" (ctrl)
		: "a" (scancode), "d" (ctrl)
		:		
	);	
	
	//Setovanje flag-a koji nam oznacava da li je alt trenutno pritisnut
	//i ubacivanje ascii karaktera u buffer ako je scancode 302
	__asm__ __volatile__ 
	(
		"xorl %%ecx, %%ecx;"
		"cmpl $202, %%eax;"
		"je CON5;"
		"cmpl $302, %%eax;"
		"jne CON6;"

		"movl (altNum), %%eax;"
		"stosb;"
		"movl $0, (altNum);"
		"movl $1, %%ecx;"

		"CON5:;"
		"xorl $1, %%edx;"
		"CON6:;"
		: "=d" (alt), "=c" (result)
		: "a" (scancode), "d" (alt), "D" (buffer)
		:		
	);

	//Proveravanje da li postoji karakter koji odgovara datom scancodu
	//a da nije shift, ctrl i alt
	__asm__ __volatile__ 
	(
		"cmpl %%edx, %%ecx;"
		"jge END;"
		: 
		: "d" (scancodeSz), "c" (scancode)
		: 
	);

	//Kada nije pritisnut ni shift, ni ctrl, ni alt
	__asm__ __volatile__ 
	(
		"addl (alt), %%eax;"
		"addl (ctrl), %%eax;"

		"cmpl $0, %%eax;"
		"jne CON7;"
		"xorl %%eax, %%eax;" // ..
		"addl %%ecx, %%esi;"
		"lodsb;"
		"stosb;"
		"movl $1, %%ecx;"
		"jmp CON8;"
		"CON7:;"
		"xorl %%ecx, %%ecx;"
		"CON8:;"
		: "=c" (result)
		: "S" (scancodes), "a" (shift), "c" (scancode), "D" (buffer)
		: 
	);

	//Kada je samo shift pritisnut
	__asm__ __volatile__ 
	(
		"addl (alt), %%eax;"
		"addl (ctrl), %%eax;"

		"cmpl $1, %%eax;"
		"jne CON9;"
		"cmpl $1, (shift);"
		"jne CON9;"
		"xorl %%eax, %%eax;" // ..
		"addl %%ecx, %%esi;"
		"lodsb;"
		"stosb;"
		"movl $1, %%ecx;"
		"jmp CON10;"
		"CON9:;"
		"xorl %%ecx, %%ecx;"
		"orl %%edx, %%ecx;"
		"CON10:;"
		: "=c" (result)
		: "S" (scancodeShift), "a" (shift), "c" (scancode), "d" (result), "D" (buffer)
		:		
	);

	//Kada je samo ctrl pritisnut
	__asm__ __volatile__ 
	(
		"addl (alt), %%eax;"
		"addl (ctrl), %%eax;"
		"imul $65, %%ecx;"

		"cmpl $1, %%eax;"
		"jne CON11;"
		"cmpl $1, (ctrl);"
		"jne CON11;"
		"xorl %%eax, %%eax;"
		"addl %%ecx, %%esi;"
		"movl $65, %%ecx;"
		"MORE: lodsb;" 
		"stosb;"
		"loop MORE;"
		"movl $1, %%ecx;"
		"jmp CON12;"
		"CON11:;"
		"xorl %%ecx, %%ecx;"
		"orl %%edx, %%ecx;"
		"CON12:;"
		: "=c" (result)
		: "S" (scancodeMnemonics), "a" (shift), "c" (scancode), "d" (result), "D" (buffer)
		:		
	);

	//Kada je shift i ctrl pritisnut
	__asm__ __volatile__ 
	(
		"addl (alt), %%eax;"
		"addl (ctrl), %%eax;"
		"imul $65, %%ecx;"

		"cmpl $2, %%eax;"
		"jne CON13;"
		"cmpl $1, (ctrl);"
		"jne CON13;"
		"xorl %%eax, %%eax;"
		"addl %%ecx, %%esi;"
		"movl $65, %%ecx;"
		"MORE1: lodsb;" 
		"stosb;"
		"loop MORE1;"
		"movl $1, %%ecx;"
		"jmp CON14;"
		"CON13:;"
		"xorl %%ecx, %%ecx;"
		"orl %%edx, %%ecx;"
		"CON14:;"
		: "=c" (result)
		: "S" (scancodeShiftMnemonics), "a" (shift), "c" (scancode), "d" (result), "D" (buffer)
		:		
	);

	//Kada je samo alt pritisnut
	__asm__ __volatile__ 
	(
		"addl (alt), %%eax;"
		"addl (ctrl), %%eax;"
		"imul $4, %%ecx;"

		"cmpl $1, %%eax;"
		"jne CON15;"
		"cmpl $1, (alt);"
		"jne CON15;"
		"xorl %%eax, %%eax;" // ..
		"addl %%ecx, %%esi;"
		"lodsl;"
		"cmpl $-1, %%eax;"
		"je CON15;"
		"movl (altNum), %%ecx;"
		"imul $10, %%ecx;"
		"addl %%eax, %%ecx;"
		"movl %%ecx, (altNum);"
		"xorl %%ecx, %%ecx;"
		"jmp CON16;"
		"CON15:;"
		"xorl %%ecx, %%ecx;"
		"orl %%edx, %%ecx;"
		"CON16:;"
		: "=c" (result)
		: "S" (altCode), "a" (shift), "c" (scancode), "d" (result)
		:		
	);

	//Kada je alt i shift pritisnuto
	__asm__ __volatile__ 
	(
		"addl (alt), %%eax;"
		"addl (ctrl), %%eax;"
		"imul $4, %%ecx;"

		"cmpl $2, %%eax;"
		"jne CON17;"
		"cmpl $1, (alt);"
		"jne CON17;"
		"xorl %%eax, %%eax;" // ..
		"addl %%ecx, %%esi;"
		"lodsl;"
		"cmpl $-1, %%eax;"
		"je CON17;"
		"movl (altNum), %%ecx;"
		"imul $10, %%ecx;"
		"addl %%eax, %%ecx;"
		"movl %%ecx, (altNum);"
		"xorl %%ecx, %%ecx;"
		"jmp CON18;"
		"CON17:;"
		"xorl %%ecx, %%ecx;"
		"orl %%edx, %%ecx;"
		"CON18:;"
		"END:;"
		: "=c" (result)
		: "S" (altCodeShift), "a" (shift), "c" (scancode), "d" (result)
		:		
	);

	return result;
}
