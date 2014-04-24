/*
 BLOG.C - Binkley-Style Log Analyzer by JBB v1.0a
 15.01.95 10.02.96 21.03.96 22.03.96 23.03.96 25.03.96
 C source code for GNU/EMX
*/

#ifdef DOS
	#include <djgppstd.h>
#endif
#include <stdio.h>

#define NumChars 256 /* number of characters in a line */

struct StructNode
{
	char NodeNumber[20];
	unsigned long times;
	struct StructNode *next;
};

struct StructBink
{
	unsigned int Connects; /* number of connect's */
	unsigned int BBS; 		 /* number of calls passed to BBS */
	unsigned int Nulls; 	 /* number of null calls */
	unsigned int FReqs; 	 /* number of freq's */
	unsigned long KbSent;  /* total of kilobytes sent */
	unsigned long KbRec;	 /* total of kilobytes received */
};

/* body */
int analyze(char *FileName)
{
	int ErrorLevel=0,i,k,l;
	FILE *FileLog;
	char *string,*joker,*dalmuti,*jyhad,*NameFile,*BaudRate,*string2,*node;
	char *date1,*date2;
	char byte,soft,Ring,j;
    int final;
	struct StructBink Binkley;
	struct StructNode *NodeMemo=NULL,*nm1=NULL,*nm2=NULL;

	Binkley.Connects=0; Binkley.FReqs=0; Binkley.KbSent=0; Binkley.KbRec=0;
	Binkley.BBS=0; Binkley.Nulls=0;
	printf("\nPlease wait...\n");
	string=(char *)malloc(sizeof(char)*NumChars);
	string2=(char *)malloc(sizeof(char)*NumChars);
	node=(char *)malloc(sizeof(char)*NumChars);
	NameFile=(char *)malloc(sizeof(char)*NumChars);
	BaudRate=(char *)malloc(sizeof(char)*NumChars);
	date1=(char *)malloc(sizeof(char)*8);
	date2=(char *)malloc(sizeof(char)*8);
	NodeMemo=(struct StructNode *)malloc(sizeof(struct StructNode));
	NodeMemo->next=NULL;
	NodeMemo->times=0;
	nm1=NodeMemo;
	FileLog=fopen(FileName,"rt");
	rewind(FileLog);
    i=0; byte=0; j=0; final=0;
    while (final!=EOF)
	{
        final=fgetc(FileLog);
        byte=(char)final;
		if ((byte!='\n') && (i<NumChars)) string[i++]=byte;
			else
			{
				string[i]=0; i=0;
				if ((char *)strstr(string,":"))
					if (!j)
					{
						j=1;
						for (l=2;l<8;l++) date1[l-2]=string[l];
						date1[7]=0;
					} else
					{
						for (l=2;l<8;l++) date2[l-2]=string[l];
						date2[7]=0;
					}
				soft=0;
				if ((string[18]='B') && (string[19]='I') && (string[20]='N')
				&& (string[21]='K')) soft=1;
				else
				 if ((string[18]='M') && (string[19]='A') && (string[20]='X')) soft=2;
				switch (soft)
				{
					case 1: /* Binkley */
						if (Ring==1) /* we've obtained a connect */
						{
							joker=(char *)strstr(string,"CRC");
							dalmuti=(char *)strstr(string,"Modem protocol negotiation filtered");
							if ((joker==NULL) && (dalmuti==NULL))
							{
								Ring=0;
								jyhad=(char *)strstr(string,"Received: ");
								joker=(char *)strstr(string,"Seconds: ");
								if ((jyhad!=NULL) || (joker!=NULL)) Binkley.Nulls++;
								else
								{
									joker=(char *)strstr(string,"Spawning BBS");
									jyhad=(char *)strstr(string,"End of connection attempt");
									if ((joker==NULL) && (jyhad==NULL))
									{
										printf("\n[BINK] Connection from ");
										strcpy(string2,string);
										l=23; while (string[l]!=0) printf("%c",string[l++]);
										printf(" at %s",BaudRate);
										l=0; while ((string2[l]!='(') && (string2[l]!=0)) l++;
										k=0; l++;
										while ((string2[l]!='@') && (string2[l]!=0))
										{
											node[k++]=string2[l++];
										}
										node[k]=0;
										nm2=NodeMemo; byte=0;
										while ((nm2!=NULL) && (!byte))
										{
											joker=(char *)strstr(nm2->NodeNumber,node);
											if (joker)
											{
												nm2->times=nm2->times+1;
												byte=1;
											}
											nm2=nm2->next;
										}
										if (!byte)
										{
											nm2=(struct StructNode *)malloc(sizeof(struct StructNode));
											nm2->times=0;
											nm2->next=NULL;
											nm1->next=nm2;
											nm1->times=1;
											k=0;
											while ((k<20) && (k<=strlen(node))) nm1->NodeNumber[k]=node[k++];
											nm1->NodeNumber[k]=0; nm1->NodeNumber[19]=0;
											nm1=nm2;
										}
									}
								}
							}
						}
						joker=(char *)strstr(string,"Connect");
						if (joker!=NULL)
						{
							Ring=1; Binkley.Connects++;
							for (l=31;l<strlen(string);l++) BaudRate[l-31]=string[l];
							BaudRate[l-31]=0;
						}
						joker=(char *)strstr(string,"File Request");
						if (joker!=NULL)
						{
							Binkley.FReqs++;
							if (string[36]='(')
							{
								l=37; k=0;
								while ((string[l]!=')') && (string[l]!=0) && (k<NumChars)) NameFile[k++]=string[l++];
								NameFile[k]=0;
								printf("\n[BINK] File Request of %s",NameFile);
							} else printf("%s",string);
						}
						joker=(char *)strstr(string,"Received:");
						if (joker!=NULL)
						{
							joker=(char *)strstr(string,"Sent:");
							l=0; k=1;
							while ((l<strlen(string)) && (k)) if (string[l++]=='/') k=0;
							if (!k)
							while ((string[l]!=' ') && (l<strlen(string)))
								NameFile[k++]=string[l++];
							NameFile[k]=0;
							Binkley.KbRec+=atoi(NameFile);
							if (joker!=NULL)
							{
								k=1;
								while ((l<strlen(string)) && (k)) if (string[l++]=='/') k=0;
								if (!k)
								while ((string[l]!=' ') && (l<strlen(string)))
									NameFile[k++]=string[l++];
								NameFile[k]=0;
								Binkley.KbSent+=atoi(NameFile);
							}
						}
						joker=(char *)strstr(string,"Spawning BBS");
						if (joker!=NULL) Binkley.BBS++;
						break;
					case 2: /* Maximus */
						break;
				}
			}
	}
    printf("\n\n           Connects  Freq's  KB/Sent  KB/Rec.  (from %s to %s)",date1,date2);
	printf("\nBinkley      %6d  %6u%8uk%8uk",Binkley.Connects-Binkley.BBS,Binkley.FReqs,Binkley.KbSent/1024,Binkley.KbRec/1024);
	printf("\nMaximus      %6d",Binkley.BBS);
	printf("\nNulls        %6d",Binkley.Nulls);
	printf("\nTotal calls  %6d",Binkley.Connects);
	fclose(FileLog);
	free(string);
	free(node);
	free(string2);
	free(NameFile);
	free(BaudRate);
	free(date1);
	free(date2);
    printf("\n\nAddress          Calls (%%)");
	nm1=NodeMemo;
	while (nm1!=NULL)
	{
		if (nm1->times>0)
		{
			printf("\n%s",nm1->NodeNumber);
			for (l=0;(l<(15-strlen(nm1->NodeNumber))&&(l<20));l++) printf(" ");
            printf(" - %4d (%4.1f%%)",nm1->times,((float)nm1->times)*100/((float)Binkley.Connects-Binkley.BBS));
		}
		nm2=nm1->next;
		free(nm1);
		nm1=nm2;
	}
	return(ErrorLevel);
}

/* returns 0 if file doesn't exist, 1 otherwise */
int f_exist(char *FileName)
{
	FILE *FileHandle;
	int i=0;

	printf("\nChecking file %s",FileName);
	FileHandle=fopen(FileName,"rt");
	if (FileHandle) i=1;
	fclose(FileHandle);

	return(i);
}

int main(int argc, char *argv[])
{
	int ErrorLevel=0,i;

    printf("\nBinkley-Style Log Analyzer v1.0a");
	printf("\nAuthor: joanmi@bbs-ce.uab.es - Joanmi Bardera 2:343/143.2");
	printf("\n\nCommand Line = ");

	for (i=0;i<argc;i++) printf("%s ",argv[i]);
	if (argc<2)
	{
		printf("\nToo few parameters.");
		printf("\nUsage: blog <file_log>");
		ErrorLevel=1;
	} else
	{
		if (f_exist(argv[1])) ErrorLevel=analyze(argv[1]);
		else
		{
			printf("\nFile '%s' does not exist.",argv[1]);
			ErrorLevel=-1;
		}
	}
	printf("\n\nErrorLevel = %i\n",ErrorLevel);
	return(ErrorLevel);
}

/*
 ErrorLevels:

 -1 file doesn't exist
	0 no error
	1 too few parameters
*/
