/** File:		count.c
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
	fp=fopen(argv[1], "r");

	char ch;
	ch=fgetc(fp);

	int num=0;

	while(ch!=EOF)
	{
		if(ch=='>')
		{
			num++;
			putchar(ch);

			ch=fgetc(fp);
			while(ch!='\n')
			{
				putchar(ch);
				ch=fgetc(fp);
			}

			printf("\n\n");
		}
		ch=fgetc(fp);
	}

	printf("#####Total Number: %d#####\n\n", num);
}
