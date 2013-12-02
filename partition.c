/** File:		partition.c
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

int main(int argc, char **argv)
{
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

	system("mkdir out");

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

			sprintf(filename, "%C%C%C/%s", 'o', 'u', 't', genoname);

			FILE *out_fp;
			out_fp=fopen(filename, "w");

			fputc('>', out_fp);
			fputs(genoname, out_fp);
			fputc('\n', out_fp);

			ch=fgetc(fp);
			while(ch!='>' && ch!=EOF)
			{
				fputc(ch, out_fp);
				ch=fgetc(fp);
			}

			fclose(out_fp);

			printf("\n\n");
		}
	}

	fclose(fp);

	printf("#####Total Number: %d#####\n\n", num_file);
}
