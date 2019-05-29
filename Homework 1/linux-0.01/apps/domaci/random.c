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
	printstr(buffer);
	int i;	
	for(i=0;i<scancodeSz;i++)
		scancodes[i]=buffer[i];
	fgets(buffer,BUF_SIZE,fd);
	for(i=0;i<scancodeSz;i++)
		scancodeShift[i]=buffer[i];
	/*for(i=0;i<scancodeSz;i++)
	{
		char test[TEST_SIZE];
		test[0]=scancodes[i];
		test[1]='\0';
		write(1,test,strlen(test));
	}
	write(1,"\n",strlen("\n"));
	for(i=0;i<scancodeSz;i++)
	{
		char test[TEST_SIZE];
		test[0]=scancodeShift[i];
		test[1]='\0';
		write(1,test,strlen(test));
	}
	write(1,"\n",strlen("\n"));*/
	fd=open(mnemonic_filename, O_RDONLY);
	if(fd==-1)
	{
		printerr("Fajl sa mnemonicima neuspesno otvoren!\n");
		_exit(1);
	}
	fgets(buffer,BUF_SIZE,fd);
	int numOfMnemonics=atoi(buffer);
	memset(scancodeIdxInMnemonics,-1,sizeof(scancodeIdxInMnemonics));
	memset(scancodeShiftIdxInMnemonics,-1,sizeof(scancodeShiftIdxInMnemonics));	
	int row;	
	for(row=0;row<numOfMnemonics;row++)
	{
		fgets(buffer,BUF_SIZE,fd);
		char toSearch=buffer[0]; //karakter koji trazimo da li postoji medju scancodovima, ili siftovanim scancodovima
		int idx=0;		
		for(i=2;i<strlen(buffer);i++)
			mnemonics[row][idx++]=buffer[i];
		if(mnemonics[row][idx-1]=='\n')
			idx--;
		mnemonics[row][idx]='\0';
		//printstr(mnemonics[row]);
		//printstr("\n");
		char found='F';
		for(i=0;i<scancodeSz;i++)
		{
			if(scancodes[i]==toSearch)
			{
				scancodeIdxInMnemonics[i]=row;
				found='T';
				break;
			}
		}
		if(found=='F')
		{
			for(i=0;i<scancodeSz;i++)
			{
				if(scancodeShift[i]==toSearch)
				{
					scancodeShiftIdxInMnemonics[i]=row;
					break;
				}
			}
		}
	}
	/*for(i=0;i<numOfMnemonics;i++)
	{
		printstr(mnemonics[i]);
		printstr("\n");
	}*/
	/*for(i=0;i<scancodeSz;i++)
	{
		itoa(scancodeIdxInMnemonics[i],bu);
		printstr(bu);
		printstr(" ");
	}
	printstr("\n");
	for(i=0;i<scancodeSz;i++)
	{
		itoa(scancodeShiftIdxInMnemonics[i],bu);
		printstr(bu);
		printstr(" ");
	}*/
}