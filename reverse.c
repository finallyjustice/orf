/** File:		reverse.c
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

char getReverse(char base);

int main(int argc, char **argv)
{
	int count_num;

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
			
			char outFileName[MAXLINE];

			sprintf(outFileName, "%s", outDirName);
			strcat(outFileName, inDirEnt->d_name);
			strcat(outFileName, "-reverse");

			FILE *preInFp;
			preInFp=fopen(inFileName, "r");

			FILE *outFp;
			outFp=fopen(outFileName, "w");

			int num_base=0;
			char ch;

			while(ch!='\n')
			{
				ch=fgetc(preInFp);
				fputc('\n', outFp);
			}

			fputc('\n', outFp);

			ch=fgetc(preInFp);
			while(ch!=EOF)
			{
				fputc('\n',outFp);
				ch=fgetc(preInFp);
			}
			fclose(preInFp);

			FILE *inFp;
			inFp=fopen(inFileName, "r");

			int num_header=0;
			char header[MAXLINE];
			
			ch=fgetc(inFp);
			while(ch!='\n')
			{
				header[num_header]=ch;
				num_header++;
				ch=fgetc(inFp);
			}
			
			for( ; ; )
			{
				ch=fgetc(inFp);
				
				if(ch==EOF)
				{
					break;
				}
				
				ch=getReverse(ch);
				fputc(ch, outFp);
				fseek(outFp, -2L, 1);
			}

			rewind(outFp);

			for(count_num=0;count_num<num_header;count_num++)
			{
				ch=header[count_num];
				fputc(ch, outFp);
			}

			fclose(inFp);
			fclose(outFp);
			
			printf("End Reversing %s\n", outFileName);

			printf("\n\n");
		}
	}

/*
	printf("\n");

	if(argv[1]==NULL)
	{
		printf("#####Please input file name!#####\n\n");
		return 1;
	}

	FILE *fp;

	//precomputation
	fp=fopen(argv[1], "r");

	char ch=fgetc(fp);

	int num_file=0;

	while(ch!=EOF)
	{
		if(ch=='>')
		{
			num_file++;
		}

		ch=fgetc(fp);
	}

	printf("#####Totally %d files.#####\n\n", num_file);

	fclose(fp);

	//Main Procedure
	fp=fopen(argv[1], "r");

	ch=fgetc(fp);
	num_file=0;

	system("mkdir reverse");

	while(ch!=EOF)
	{
		//start a new file
		char filename[MAXLINE];
		char genoname[MAXLINE];
		
		if(ch=='>')
		{
			num_file++;
			putchar(ch);

			ch=fgetc(fp);
			int index_name=0;

			while(ch!='\n')
			{
				if(ch!='\\' && ch!='/')
				{
					putchar(ch);
					genoname[index_name]=ch;
				}

				index_name++;

				ch=fgetc(fp);
			}
			genoname[index_name]='\0';

			sprintf(filename, "%c%c%c%c%c%c%c/%s", 'r', 'e', 'v', 'e', 'r', 's', 'e', genoname);

			FILE *out_fp;
			out_fp=fopen(filename, "w");

			//fputc('>', out_fp);
			//fputs(genoname, out_fp);
			//fputc('\n', out_fp);

			ch=fgetc(fp);
			while(ch!='>' && ch!=EOF)
			{
				rewind(out_fp);
				fputc(ch, out_fp);
				ch=fgetc(fp);
			}

			fclose(out_fp);

			printf("\n\n");
		}
	}

	fclose(fp);

	printf("#####Total Number: %d#####\n\n", num_file);
*/
}

char getReverse(char base)
{
	char r_base;
	r_base=base;

	if(base=='A' || base=='a')
	{
		r_base='T';
		return r_base;
	}
	if(base=='T' || base=='t')
	{
		r_base='A';
		return r_base;
	}
	if(base=='C' || base=='c')
	{
		r_base='G';
		return r_base;
	}
	if(base=='G' || base=='g')
	{
		r_base='C';
		return r_base;
	}

	r_base;
}
