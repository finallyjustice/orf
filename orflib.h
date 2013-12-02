/** File:		orflib.h
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

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE 2048
#define ORF_LENGTH 150

//represent 1 line in csv file.
struct pairStat
{
	int pairStat_id;       //ID for codon pair
	char codonpair[6];     //Codon Pair. For convenience. Not show up in csv
	char first_codon[3];   //first codon in codon pair
	char second_codon[3];  //second codon in codon pair
	int frequency;         //frequency
};

struct codonStat
{
	int codonStat_id; //ID for codon
	char codon[3];	  //Codon
	unsigned int frequency;    //frequency
};
