#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/* Variables to calculate cache hit and miss rate */
int miss = 0, hit = 0 ,fetches = 0;
int imiss = 0,dmiss = 0;

/* pointer to file */
FILE *fp; 

/* Elements of Cache */
long long int address, indexVal, tag, line; 

/* Variables to calculate bits for eache element of cache*/
int cacheSize, no_Of_Blocks;
int blockSize, offstBits, indexBits;

int flag = 0, opflag;
char *token; 
char buf[12];
char add[10];
int count = 0, previous;
int replace, counter;
long long int indx;


/************************************************************************
* Function name: unifiedDM
* Argument: Filename of memory trace file
* Return Type: void
* Description: Calculates hit and miss rate of unified cache (I + D)using
			   direct-map or 1-way associativity 
************************************************************************/
void unifiedDM (char fn[])
{
	fp = fopen(fn, "r");
	long long int *valid = (long long int*) malloc(no_Of_Blocks * sizeof(long long int));
	long long int *tags = (long long int*) malloc(no_Of_Blocks * sizeof(long long int));

	memset(valid, 0 , no_Of_Blocks * sizeof(long long int));

	if (fp)
	{
		while (fgets(buf, 20, (FILE*)fp) != NULL)
		{
			token = strtok(buf, " ");
			 
			while (token != NULL) 
    		{ 
    			if (flag == 0)
    			{
    				fetches++;
        			flag = 1;
    			}
    			else
    			{
    				strcpy(add,token);
    				flag = 0;
    			}
    			token = strtok(NULL, " "); 
    		}
    		memset(buf, 0 , 20);

 			sscanf(add, "%llx", &address);
    		indexVal = getIndex(offstBits, indexBits, address);
			tag = getTag((offstBits + indexBits), address);
			if ((valid[indexVal] == 0) || (tags[indexVal] != tag))
			{
				miss++;
				valid[indexVal] = 1;
				tags[indexVal] = tag;
			}
			else
			{
				hit++;
			}
    	}	
	}
	fclose(fp);
	free(valid);
	free(tags);
} 


/************************************************************************
* Function name: splitDM
* Argument: Filename of memory trace file
* Return Type: void
* Description: Calculates hit and miss rate of Icache and Dcache
			   separately using direct-map or 1-way associativity
************************************************************************/
void splitDM (char fn[])
{
	fp = fopen(fn, "r");
	long long int *ivalid = (long long int*) malloc(no_Of_Blocks * sizeof(long long int));
	long long int *itags = (long long int*) malloc(no_Of_Blocks * sizeof(long long int));
	long long int *dvalid = (long long int*) malloc(no_Of_Blocks * sizeof(long long int));
	long long int *dtags = (long long int*) malloc(no_Of_Blocks * sizeof(long long int));

	memset(ivalid, 0 , no_Of_Blocks * sizeof(long long int));
	memset(dvalid, 0 , no_Of_Blocks * sizeof(long long int));

	if (fp)
	{
		while (fgets(buf, 20, (FILE*)fp) != NULL)
		{
			token = strtok(buf, " ");

			if (!strcmp(token,"2"))
        		opflag = 0;
        	else
        		opflag = 1;
    		while (token != NULL) 
    		{ 
    			if (flag == 0)
    			{
    				fetches++;
        			flag = 1;
    			}
    			else
    			{
    				strcpy(add,token);
    				flag = 0;
    			}
    			token = strtok(NULL, " "); 
   			}

   			memset(buf, 0 , 20);

    		sscanf(add, "%llx", &address);
    		indexVal = getIndex(offstBits, indexBits, address);
			tag = getTag((offstBits + indexBits), address);
			if (opflag == 0)
			{
				if ((ivalid[indexVal] == 0) || (itags[indexVal] != tag))
				{
					miss++;
					imiss++;
					ivalid[indexVal] = 1;
					itags[indexVal] = tag;
				}
				else
				{
					hit++;
				}
			}
			else
			{
				if ((dvalid[indexVal] == 0) || (dtags[indexVal] != tag))
				{
					miss++;
					dmiss++;
					dvalid[indexVal] = 1;
					dtags[indexVal] = tag;
				}
				else
				{
					hit++;
				}
			} 
		}
	}
	fclose(fp);
	free(ivalid);
	free(itags);
	free(dvalid);
	free(dtags);
}



/************************************************************************
* Function name: unified4w
* Argument: Filename of memory trace file
* Return Type: void
* Description: Calculates hit mis rate of unified cache (I + D) using
			   4-way associativity with LRU policy
************************************************************************/
void unified4w(char fn[])
{
	fp = fopen(fn, "r");
	long long int *valid = (long long int*) malloc(no_Of_Blocks * sizeof(long long int));
	long long int *tags = (long long int*) malloc(no_Of_Blocks * sizeof(long long int));
	long long int *lru = (long long int*) malloc(no_Of_Blocks * sizeof(long long int));

	memset(valid, 0 , no_Of_Blocks * sizeof(long long int));

	for (int i = 0; i < no_Of_Blocks; i++)
	{
		lru[i] = count;
		count++;
		if (count == 4) {
			count = 0;
		}
	}

	if (fp)
	{
		while (fgets(buf, 20, (FILE*)fp) != NULL)
		{
			token = strtok(buf, " ");
			 
			while (token != NULL) 
    		{ 
    			if (flag == 0)
    			{
    				fetches++;
        			flag = 1;
    			}
    			else
    			{
    				strcpy(add,token);
    				flag = 0;
    			}
    			token = strtok(NULL, " "); 
    		}
    				
    		memset(buf, 0 , 20);
    		sscanf(add, "%llx", &address);
    		indexVal = getIndex(offstBits, indexBits, address);
			tag = getTag((offstBits + indexBits), address);
			line = indexVal * 4;
			counter = 0;
			replace = 0;
			previous = 0;
			for (int j = 0 ; j < 4; j++) 
			{
				if(valid[line + j] == 0)
				{
					counter++;
				}
			    else if((valid[line + j] == 1) && (tags[line + j] != tag))
				{
					counter++;
				}
				else if(((valid[line + j] == 1) && (tags[line + j] == tag)))
				{
					hit++;
					indx = line + j;
					previous = lru[line + j];
					counter = 0;
					replace = 1;
					break;
				}
			}
					
			if (counter)
			{
				miss++;
				for(int k = 0 ; k < 4; k++)
			    {
					if (previous >= lru[line + k])
					{
						previous = lru[line + k];
						indx = line + k;
					}
				}
						replace = 1;
			}
				
			if (replace == 1)
			{
 				valid[indx] = 1;
 				tags[indx] = tag;
				lru[indx] = 3;
				for (int l = 0; l < 4; l++)
				{
					if(((line + l) != indx) && (lru[line + l] > previous))
					{
						lru[line + l] = lru[line + l] - 1;
					}
				}
			}
 
    	} 				
	}
	fclose(fp);
	free(valid);
	free(tags);
	free(lru);
}



/************************************************************************
* Function name: split4w
* Argument: Filename of memory trace file
* Return Type: void
* Description: Calculates hit and miss rate of Icache and Dcache
			   separately using 4-way associativity with LRU policy
************************************************************************/
void split4w(char fn[])
{
	fp = fopen(fn, "r");
	long long int *ivalid = (long long int*) malloc(no_Of_Blocks * sizeof(long long int));
	long long int *itags = (long long int*) malloc(no_Of_Blocks * sizeof(long long int));
	long long int *ilru = (long long int*) malloc(no_Of_Blocks * sizeof(long long int));
	long long int *dvalid = (long long int*) malloc(no_Of_Blocks * sizeof(long long int));
	long long int *dtags = (long long int*) malloc(no_Of_Blocks * sizeof(long long int));
	long long int *dlru = (long long int*) malloc(no_Of_Blocks * sizeof(long long int));

	memset(ivalid, 0 , no_Of_Blocks * sizeof(long long int));
	memset(dvalid, 0 , no_Of_Blocks * sizeof(long long int));

	for (int i = 0; i < no_Of_Blocks; i++)
	{
		ilru[i] = count;
		dlru[i] = count;
		count++;
		if (count == 4) {
			count = 0;
		}
	}

	if (fp)
	{
		while (fgets(buf, 20, (FILE*)fp) != NULL)
		{
			token = strtok(buf, " ");

			if(!strcmp(token,"2"))
				opflag = 0;
			else
				opflag = 1;
			while (token != NULL) 
    		{ 
    			if (flag == 0)
    			{
    				fetches++;
        			flag = 1;
    			}
    			else
    			{
    				strcpy(add,token);
    				flag = 0;
    			}
    			token = strtok(NULL, " "); 
   			}
			
    				
    		memset(buf, 0 , 20);
    		sscanf(add, "%llx", &address);
    		indexVal = getIndex(offstBits, indexBits, address);
			tag = getTag((offstBits + indexBits), address);
			line = indexVal * 4;
			counter = 0;
			replace = 0;
			previous = 0;
			if(opflag == 0)
			{
				for (int j = 0 ; j < 4; j++) 
				{
					if(ivalid[line + j] == 0)
					{
						counter++;
					}
			    	else if((ivalid[line + j] == 1) && (itags[line + j] != tag))
					{
						counter++;
					}
					else if(((ivalid[line + j] == 1) && (itags[line + j] == tag)))
					{
						hit++;
						indx = line + j;
						previous = ilru[line + j];
						counter = 0;
						replace = 1;
						break;
					}
				}
					
				if (counter)
				{
					miss++;
					imiss++;
					for(int k = 0 ; k < 4; k++)
			    	{
						if (previous >= ilru[line + k])
						{
							previous = ilru[line + k];
							indx = line + k;
						}
					}	
					replace = 1;
				}
				
				if (replace == 1)
				{
 					ivalid[indx] = 1;
 					itags[indx] = tag;
					ilru[indx] = 3;
					for (int l = 0; l < 4; l++)
					{
						if(((line + l) != indx) && (ilru[line + l] > previous))
						{
							ilru[line + l] = ilru[line + l] - 1;
						}
					}
				}
			}

			else
			{
				for (int j = 0 ; j < 4; j++) 
				{
					if(dvalid[line + j] == 0)
					{
						counter++;
					}
			    	else if((dvalid[line + j] == 1) && (dtags[line + j] != tag))
					{
						counter++;
					}
					else if(((dvalid[line + j] == 1) && (dtags[line + j] == tag)))
					{
						hit++;
						indx = line + j;
						previous = dlru[line + j];
						counter = 0;
						replace = 1;
						break;
					}
				}
					
				if (counter)
				{
					miss++;
					dmiss++;
					for(int k = 0 ; k < 4; k++)
			    	{
						if (previous >= dlru[line + k])
						{
							previous = dlru[line + k];
							indx = line + k;
						}
					}
					replace = 1;
				}
				
				if (replace == 1)
				{
 					dvalid[indx] = 1;
 					dtags[indx] = tag;
					dlru[indx] = 3;
					for (int l = 0; l < 4; l++)
					{
						if(((line + l) != indx) && (dlru[line + l] > previous))
						{
							dlru[line + l] = dlru[line + l] - 1;
						}
					}
				}
			}
    	} 				
	}
	fclose(fp);
	free(ivalid);
	free(itags);
	free(ilru);
	free(dvalid);
	free(dtags);
	free(dlru);
}


/************************************************************************
* Function name: getIndex
* Argument: Number of offset bits, Number of index bits, PC address
* Return Type: long long int
* Description: Calculates index of line in Cache for given PC address
************************************************************************/
long long int getIndex(int offB, int indB, long long int address)
{
	long long int temp = address;
	for (int i = 0; i < indB; i++)
	{
		temp &= ~(1 << (offB + i));
	}
	return (long long)((address & (~temp)) >> offB);
}


/************************************************************************
* Function name: getTag
* Argument: Number of mask bits, PC address
* Return Type: long long int
* Description: Calculates tag from given PC address
************************************************************************/
long long int getTag(int maskBits, long long int address)
{
	return (long long)(address >> maskBits);
}


/************************************************************************
* Function name: main
* Argument: Cachesize, Blocksize, filename, Cache type, associativty
* Return Type: int
* Description: Design the cache accordings to arguments passed
 			   and calls respective functions to calculate hit and miss
 			   rate  
************************************************************************/
int main(int argc, char const *argv[])
{
	char filename[100];

	if(argc < 5)
	{
		printf("Please enter arguments in following format \n <cacheSize> 
			<blockSize> <filename> <unified: u or split: s> <associativity: 1 or 4>");
	}
	else
	{
		strcpy(filename,argv[3]);
		blockSize = atoi(argv[2]);
		offstBits = log2(blockSize);
		if(!(strcmp(argv[4],"u")))
		{
			cacheSize = atoi(argv[1]);
			indexBits = log2(cacheSize / blockSize);
			no_Of_Blocks = pow(2,indexBits);

			if(!(strcmp(argv[5],"1")))
			{
				unifiedDM(filename);
				printf("*********** RESULT OF DIRECT MAP UNIFIED CACHE WITH
				 BLOCKSIZE %d ***********\n",blockSize);
			}
			else if(!(strcmp(argv[5],"4")))
			{
				indexBits = log2((cacheSize / blockSize)/4);
				unified4w(filename);
				printf("*********** RESULT OF 4-WAY ASSOCIATIVITY UNIFIED
				 CACHE WITH BLOCKSIZE %d ***********\n",blockSize);	
			}
		}
		else if(!(strcmp(argv[4],"s")))
		{
			cacheSize = (atoi(argv[1])) / 2;
			indexBits = log2(cacheSize / blockSize);
			no_Of_Blocks = pow(2,indexBits);

			if(!(strcmp(argv[5],"1")))
			{
				splitDM(filename);
				printf("*********** RESULT OF DIRECT MAP SPLIT CACHE WITH
				 BLOCKSIZE %d ***********\n",blockSize);
			}
			else if(!(strcmp(argv[5],"4")))
			{
				indexBits = log2((cacheSize / blockSize)/4);
				split4w(filename);
				printf("*********** RESULT OF 4-WAY ASSOCIATIVITY SPLIT 
					CACHE WITH BLOCKSIZE %d ***********\n",blockSize);
			}	
			printf("Instruction miss = %d\n",imiss);
			printf("Data miss = %d\n",dmiss);
		}
		else
		{
			printf("Wrong arguments");
			exit(0);
		}

		printf("Total miss = %d\n",miss);
		printf("Total hit = %d\n",hit);
		printf("Total fetches = %d\n",fetches);
		printf("Hit rate = %.5f\n", (float) (((float) hit / fetches) * 100));
		printf("Miss rate = %.5f\n", (float) (((float) miss / fetches) * 100));
	}
	
	return 0;
}
