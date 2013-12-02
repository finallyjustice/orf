/** File:		orf.c
 ** Author:		Dongli Zhang
 ** Contact:	dongli.zhang0129@gmail.com
 **
 ** Copyright (C) Dongli Zhang 2013
 **
 ** This program is free software;  you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 2 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY;  without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 ** the GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program;  if not, write to the Free Software 
 ** Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "orflib.h"

char nobase[4];  //store base in lower case
char base[4];  //store base in upper case
char codon[64][3]; //store all 64 codons

struct codonStat *codonArray;//struct codonStat in orflib.h
struct pairStat *statArray; //struct pairStat in orflib.h

void createBase(char *base);
void createNoBase(char *nobase);
void createCodon(char codon[][3]);
void createCodonStat(struct codonStat *statArray);
void createCodonPair(struct pairStat *statArray);
int cmpSCodon(char *sCodonA, char *sCodonB);
int cmpDCodon(char *dCodonA, char *dCodonB);
void processCodon(FILE *inFp, FILE *outFp, FILE *codonFp);
int getBondValue(char base);
void writeToFile(FILE *fp, struct pairStat *statArray);
void writeCodonFile(FILE *fp, struct codonStat *codonArray);
int readSinFrom(FILE *fp, char *tempPair);
int readTriFrom(FILE *fp, char *tempPair, int *drop);
char readVerify(char ch);
void updateFrequency(char *tempPair,struct pairStat *statArrayCopy);
void updateCodonFFrequency(char *tempPair, struct codonStat *codonArrayCopy);
void updateCodonLFrequency(char *tempPair, struct codonStat *codonArrayCopy);
void copyStatArray(struct pairStat *statArray, struct pairStat *statArrayCopy);
void copyCodonArray(struct codonStat *codonArray, struct codonStat *codonArrayCopy);

int main(int argc, char **argv)
{
	//check the constraints on path with slash "/"	
	int len_in_path=strlen(argv[1]);
	int len_out_path=strlen(argv[2]);

	if(argv[1][len_in_path-1]!='/')
	{
		printf("\nInput Folder Path Name Without the last slash!\n\n");
		return 1;
	}

	if(argv[2][len_out_path-1]!='/')
	{
		printf("\nOutput Folder Path Name Without the last slash!\n\n");
		return 1;
	}

	printf("\n");

	int count;

	createNoBase(nobase);
	createBase(base);
	createCodon(codon);
	
	//input file name
	char *inDirName;
	inDirName=argv[1];

	//output file name
	char *outDirName;
	outDirName=argv[2];

	//Precompute the information required!
	struct dirent *pre_inDirEnt;
	DIR *pre_pInDir;

	pre_pInDir=opendir(inDirName);
	
	if(pre_pInDir==NULL)
	{
		printf("Input Folder Does Not Exist!\n\n");
		
		closedir(pre_pInDir);
		return 1;
	}

	closedir(pre_pInDir);

	DIR *pre_pOutDir;

	pre_pOutDir=opendir(outDirName);

	if(pre_pOutDir==NULL)
	{
		printf("Output Folder Does Not Exist!\n\n");

		closedir(pre_pOutDir);
		return 1;
	}

	closedir(pre_pOutDir);

	//computer file numbers in input folder
	int num_file=0;

	pre_pInDir=opendir(inDirName);

	while((pre_inDirEnt=readdir(pre_pInDir)) != NULL)
	{
		if(pre_inDirEnt->d_type==8)
		{
			num_file++;
		}
	}

	if(num_file==0)
	{
		printf("No File In Input Folder!\n\n");

		closedir(pre_pInDir);
		return 1;
	}

	closedir(pre_pInDir);

	printf("There are totally %d files in the folder.\n\n", num_file);
	
	//structure of directory in C
	struct dirent *inDirEnt;
	DIR *pInDir;

	pInDir=opendir(inDirName);

	//traverse all files in directory
	while((inDirEnt=readdir(pInDir)) != NULL)
	{
		//type 8 means it is a file
		if(inDirEnt->d_type==8)
		{
			//create codon statistics
			struct codonStat codonArray[64];

			//create statistics
			struct pairStat statArray[64*64]; 

			char inFileName[MAXLINE];
			sprintf(inFileName, "%s", inDirName);
			strcat(inFileName, inDirEnt->d_name);

			printf("Handling %s ... ...\n", inFileName);

			FILE *inFp;
			inFp=fopen(inFileName, "r");
			
			char outFileName[MAXLINE];

			sprintf(outFileName, "%s", outDirName);
			strcat(outFileName, inDirEnt->d_name);
			strcat(outFileName, ".csv");

			char outCodonName[MAXLINE];

			sprintf(outCodonName, "%s", outDirName);
			strcat(outCodonName, inDirEnt->d_name);
			strcat(outCodonName, "-codon.csv");

			FILE *outFp;
			outFp=fopen(outFileName, "w");

			FILE *outCodonFp;
			outCodonFp=fopen(outCodonName, "w");

			//the main function to process the data
			processCodon(inFp, outFp, outCodonFp);

			fclose(inFp);
			fclose(outFp);
			fclose(outCodonFp);
			
			printf("End Processing %s\n", outFileName);

			printf("\n\n");
		}
	}

}

//store bases in lower case
void createNoBase(char *nobase)
{
	nobase[0]='a';
	nobase[1]='t';
	nobase[2]='c';
	nobase[3]='g';
}

//store bases in upper case
void createBase(char *base)
{
	base[0]='A';
	base[1]='T';
	base[2]='C';
	base[3]='G';
		
}

//generate all codons
void createCodon(char codon[][3])
{
	int count1;
	int count2;
	int count3;

	int count=-1;

	//3 loops ---- 4*4*4=64
	for(count1=0;count1<4;count1++)
	{
		for(count2=0;count2<4;count2++)
		{
			for(count3=0;count3<4;count3++)
			{
				count++;

				codon[count][0]=base[count1];
				codon[count][1]=base[count2];
				codon[count][2]=base[count3];
			}
		}
	}
}

//generate all codon
void createCodonStat(struct codonStat *statArray)
{
	int count_codon;

	for(count_codon=0;count_codon<64;count_codon++)
	{
		statArray[count_codon].codonStat_id=count_codon+1;
		memcpy(statArray[count_codon].codon, codon[count_codon], 3);
		statArray[count_codon].frequency=0;
	}
}

//generate all codon-pairs
void createCodonPair(struct pairStat *statArray)
{
	int count=-1;

	int count1;
	int count2;

	int size=64*64;

	//2 loops ----- 64*64
	//see struct pairStat in orflib.h
	for(count1=0;count1<64;count1++)
	{
		for(count2=0;count2<64;count2++)
		{
			count++;

			statArray[count].pairStat_id=count+1;
			memcpy(statArray[count].first_codon, codon[count1], 3);
			memcpy(statArray[count].second_codon, codon[count2], 3);
			statArray[count].frequency=0;

			statArray[count].codonpair[0]=statArray[count].first_codon[0];
			statArray[count].codonpair[1]=statArray[count].first_codon[1];
			statArray[count].codonpair[2]=statArray[count].first_codon[2];
			statArray[count].codonpair[3]=statArray[count].second_codon[0];
			statArray[count].codonpair[4]=statArray[count].second_codon[1];
			statArray[count].codonpair[5]=statArray[count].second_codon[2];
		}
	}
}

//compare 2 codon 1-equal 0-unequal
int cmpSCodon(char *sCodonA, char *sCodonB)
{
	int count;

	for(count=0;count<3;count++)
	{
		if(sCodonA[count]!=sCodonB[count])
		{
			return 0;
		}
	}

	return 1;
}

//compare 2 codon-pairs 1-equal 0-unequal
int cmpDCodon(char *dCodonA, char *dCodonB)
{
	int count;

	for(count=0;count<6;count++)
	{
		if(dCodonA[count]!=dCodonB[count])
		{
			return 0;
		}
	}

	return 1;
}

//main procedure of this program
void processCodon(FILE *inFp, FILE *outFp, FILE *codonFp)
{
	int count;

	char ch;

	codonArray=(struct codonStat *)malloc(sizeof(struct codonStat)*64);
	createCodonStat(codonArray);

	statArray=(struct pairStat *)malloc(sizeof(struct pairStat)*64*64);
	createCodonPair(statArray);

	int count_read;
	int count_base;

	//whether read EOF from file
	int is_read_end=0;
	//whether begin next fasta
	int is_next_begin=0;
	//whether find TAA or TAG or TGA
	int is_end_position;

	int result;

	//0-no 1-find start 2-find end
	int codon_status;

	int orf_len;
	int drop=0;

	char tempPair[6];
	char tempRead[MAXLINE];

	ch=fgetc(inFp);

	if(ch!='>')
	{
		return;
	}
	
	fgets(tempRead, MAXLINE, inFp);

	//############################
	printf("Processing New Genome.....\n");

	//main part of this program
	is_read_end=0;

	for( ; ; )
	{
		is_next_begin=0;
		is_end_position=0;
	
		//temp file for codon. If end position is found, copy it to final codonArray
		struct codonStat *codonArrayCopy=(struct codonStat *)malloc(sizeof(struct codonStat)*64);
		createCodonStat(codonArrayCopy);

		//temp file for statistics. If end position is found, copy it to final statArray
		struct pairStat *statArrayCopy=(struct pairStat *)malloc(sizeof(struct pairStat)*64*64);
		createCodonPair(statArrayCopy);
	
		//count_base should be 6
		count_base=0;

		for( ; ; )
		{
			ch=fgetc(inFp);
		
			if((ch=='\n') || (ch==' '))
			{
				continue;
			}

			if(ch==EOF)
			{
				//#####################################
				printf("Genome End\n");

				is_read_end=1;
				break;
			}

			if(ch=='>')
			{
				//#####################################
				printf("Genome End\n");
				//#####################################
				printf("Processing New Genome.....\n");

				is_next_begin=1;

				fgets(tempRead, MAXLINE, inFp);

				count_base=0;

				break;
			}
			
			tempPair[count_base]=readVerify(ch);
			count_base++;

			if(count_base==6)
			{
				break;
			}
		}

		if(is_read_end==1)
		{
			break;
		}

		if(is_next_begin==1)
		{
			continue;
		}

		//Find start Position
		for( ; ; )
		{
			//tempPair[0-2] is ATG
			if(isBeginCodon(tempPair)!=1)
			{
				result=readSinFrom(inFp, tempPair);

				if(result==3)
				{
					//############################
					printf("Genome End\n");

					is_read_end=1;
					break;
				}

				if(result==2)
				{
					//############################
					printf("Genome End\n");
					//############################
					printf("Processing New Genome.....\n");
					
					is_next_begin=1;
					break;
				}
			}
			else
			{
				orf_len=1;

				break;
			}
		}

		if(is_read_end==1)
		{
			break;
		}

		if(is_next_begin==1)
		{
			continue;
		}

		//Record Statistics of Codon Pairs && Find end Position 
		for( ; ; )
		{
			updateFrequency(tempPair, statArrayCopy);
			updateCodonFFrequency(tempPair, codonArrayCopy);

			//Find End Position tempPair[3-5] is end Position
			if(isEndCodon(tempPair)==1)
			{
				if(orf_len<ORF_LENGTH)
				{
					is_end_position=1;
					free(statArrayCopy);

					break;
				}

				if(orf_len>=ORF_LENGTH)
				{
					updateCodonLFrequency(tempPair, codonArrayCopy);

					is_end_position=1;
					
					copyCodonArray(codonArray, codonArrayCopy);
					copyStatArray(statArray, statArrayCopy);

					free(statArrayCopy);

					break;
				}
			}

			result=readTriFrom(inFp, tempPair, &drop);
			
			if(result==3)
			{
				//############################
				printf("Genome End\n");

				is_read_end=1;
				
				break;
			}

			if(result==2)
			{
				//############################
				printf("Genome End\n");
				//############################
				printf("Processing New Genome.....\n");
				is_next_begin=1;
				
				break;
			}

			orf_len++;
			orf_len=orf_len+drop;
		}

		if(is_end_position==1)
		{
			continue;
		}

		if(is_read_end==1)
		{
			break;
		}

		if(is_next_begin==1)
		{
			continue;
		}

	}

	//write the final codon file to output file in csv format
	writeCodonFile(codonFp, codonArray);

	//write the final statistics to output file in csv format
	writeToFile(outFp, statArray);

	free(statArray);

}

int getBondValue(char base)
{
	if(base=='A' || base=='T')
	{
		return 2;
	}
	if(base=='C' || base=='G')
	{
		return 3;
	}
}

//write the final statistics to output file in csv format
void writeToFile(FILE *fp, struct pairStat *statArray)
{
	int count;
	int count_ln;

	char writeline[MAXLINE];

	sprintf(writeline, "%s,%s,%s,%s,%s,%s,%s\n", "id", "first_codon", "second_codon", "first_hbond", "second_hbond", "total_hbond", "frequency");
	fputs(writeline, fp);

	for(count=0;count<64*64;count++)
	{
		for(count_ln=0;count_ln<MAXLINE;count_ln++)
		{
			writeline[count_ln]='\0';
		}
		
		//every line in csv is in this format
		sprintf(writeline, "%d,%c%c%c,%c%c%c,%d,%d,%d,%d\n", statArray[count].pairStat_id, statArray[count].first_codon[0], statArray[count].first_codon[1], statArray[count].first_codon[2], statArray[count].second_codon[0], statArray[count].second_codon[1], statArray[count].second_codon[2], getBondValue(statArray[count].first_codon[0])+getBondValue(statArray[count].first_codon[1])+getBondValue(statArray[count].first_codon[2]), getBondValue(statArray[count].second_codon[0])+getBondValue(statArray[count].second_codon[1])+getBondValue(statArray[count].second_codon[2]), getBondValue(statArray[count].first_codon[0])+getBondValue(statArray[count].first_codon[1])+getBondValue(statArray[count].first_codon[2])+getBondValue(statArray[count].second_codon[0])+getBondValue(statArray[count].second_codon[1])+getBondValue(statArray[count].second_codon[2]),statArray[count].frequency);
		fputs(writeline, fp);
	}

}

void writeCodonFile(FILE *fp, struct codonStat *codonArray)
{
	int count;
	int count_ln;

	char writeline[MAXLINE];

	sprintf(writeline, "%s,%s,%s\n", "id", "codon", "frequency");
	fputs(writeline, fp);

	for(count=0;count<64;count++)
	{
		for(count_ln=0;count_ln<MAXLINE;count_ln++)
		{
			writeline[count_ln]='\0';
		}

		//every line in csv is in this format
		sprintf(writeline, "%d,%c%c%c,%d\n", codonArray[count].codonStat_id, codonArray[count].codon[0], codonArray[count].codon[1], codonArray[count].codon[2], codonArray[count].frequency);
		fputs(writeline, fp);
	}
}

//Read 1 base from input file
//1-normal 2-'>' 3-EOF
int readSinFrom(FILE *fp, char *tempPair)
{
	int count;
	char ch;

	char tempRead[MAXLINE];

	for(count=0;count<5;count++)
	{
		tempPair[count]=tempPair[count+1];
	}

	for( ; ; )
	{
		ch=fgetc(fp);

		if((ch=='\n') || (ch==' ') || (ch=='N') || (ch=='n'))
		{
			continue;
		}

		else if(ch==EOF)
		{
			return 3;
		}

		else if(ch=='>')
		{	
			fgets(tempRead, MAXLINE, fp);
			return 2;
		}

		else
		{
			ch=readVerify(ch);
			tempPair[5]=ch;
			return 1;
		}
	}
}

//Read 3 bases(1 codon) from input file
//1-normal 2-'>' 3-EOF
int readTriFrom(FILE *fp, char *tempPair, int *drop)
{
	*drop=0;

	int count;

	int num_read=0;

	char ch;
	char tempRead[MAXLINE];

	for(count=0;count<3;count++)
	{
		tempPair[count]=tempPair[count+3];
	}
	for( ; ; )
	{
		ch=fgetc(fp);

		if((ch=='\n') || (ch==' '))
		{
			continue;
		}

		else if(ch==EOF)
		{
			return 3;
		}

		else if(ch=='>')
		{
			fgets(tempRead, MAXLINE, fp);

			return 2;
		}

		else
		{
			ch=readVerify(ch);
			tempPair[3+num_read]=ch;

			num_read++;

			if(num_read==3)
			{
				if((tempPair[3]!='N') && (tempPair[3]!='n'))
				{
					if((tempPair[4]!='N') && (tempPair[4]!='n'))
					{
						if((tempPair[5]!='N') && (tempPair[5]!='n'))
						{
							return 1;
						}
					}
				}

				drop++;

				num_read=0;
			}
		}
	}
}

//if ch is in lower case, convert it to upper case
char readVerify(char ch)
{
	int count_base;

	for(count_base=0;count_base<4;count_base++)
	{
		if(ch==base[count_base])
		{
			return base[count_base];
		}
	}

	for(count_base=0;count_base<4;count_base++)
	{
		if(ch==nobase[count_base])
		{
			return base[count_base];
		}
	}
}

//test whether tempPair[0-2] is ATG
int isBeginCodon(char *tempPair)
{
	if(tempPair[0]=='A')
	{
		if(tempPair[1]=='T')
		{
			if(tempPair[2]=='G')
			{
				return 1;
			}
		}
	}

	return 0;
}

//test whether tempPair[3-5] is from TAA TAG or TGA
int isEndCodon(char *tempPair)
{
	if(tempPair[3]=='T')
	{
		if(tempPair[4]=='A')
		{
			if(tempPair[5]=='A')
			{
				return 1;
			}
		}
	}

	if(tempPair[3]=='T')
	{
		if(tempPair[4]=='A')
		{
			if(tempPair[5]=='G')
			{
				return 1;
			}
		}
	}

	if(tempPair[3]=='T')
	{
		if(tempPair[4]=='G')
		{
			if(tempPair[5]=='A')
			{
				return 1;
			}
		}
	}

	return 0;
}

//when we find end codon we should update statistics
void updateFrequency(char *tempPair,struct pairStat *statArrayCopy)
{
	int count;

	for(count=0;count<64*64;count++)
	{
		if(cmpDCodon(tempPair, statArrayCopy[count].codonpair)==1)
		{
			statArrayCopy[count].frequency++;

			break;
		}
	}
}

void updateCodonFFrequency(char *tempPair, struct codonStat *codonArrayCopy)
{
	int count;

	for(count=0;count<64;count++)
	{
		if(tempPair[0]==codonArrayCopy[count].codon[0])
		{
			if(tempPair[1]==codonArrayCopy[count].codon[1])
			{
				if(tempPair[2]==codonArrayCopy[count].codon[2])
				{
					codonArrayCopy[count].frequency++;

					break;
				}
			}
		}
	}
}

void updateCodonLFrequency(char *tempPair, struct codonStat *codonArrayCopy)
{
	int count;

	for(count=0;count<64;count++)
	{
		if(tempPair[3]==codonArrayCopy[count].codon[0])
		{
			if(tempPair[4]==codonArrayCopy[count].codon[1])
			{
				if(tempPair[5]==codonArrayCopy[count].codon[2])
				{
					codonArrayCopy[count].frequency++;

					break;
				}
			}
		}
	}
}

//copy temp statArray to final statArray
void copyStatArray(struct pairStat *statArray, struct pairStat *statArrayCopy)
{
	int count=0;

	for(count=0;count<64*64;count++)
	{
		statArray[count].frequency=statArray[count].frequency+statArrayCopy[count].frequency;
	}
}

//copy temp codonArray to final array
void copyCodonArray(struct codonStat *codonArray, struct codonStat *codonArrayCopy)
{
	int count=0;

	for(count=0;count<64;count++)
	{
		codonArray[count].frequency=codonArray[count].frequency+codonArrayCopy[count].frequency;
	}
}
