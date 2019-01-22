#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#define NUM_THREADS 5
#define COLUMNS 4
#define ROWS 4

using namespace std;

int c = 0; //row, column switch.
pthread_t threads[NUM_THREADS];
pthread_mutex_t mutexsort;
int matrix[4][4];


void *bubbleSort(void *nothing){
	pthread_mutex_lock (&mutexsort);

	int temp;
	int n;
	bool swapped;
	long t = (long)nothing;
	printf("Thread: %d running\n", t);

	if (c == 0){
		for (int i = 0; i < ROWS; i++){
			if(i % 2 == 0){
				n=0;
				swapped = true;
				while (swapped){
				swapped = false;
				n++;
				for (int j = 0; j < COLUMNS - n; j++){

					if (matrix[i][j] > matrix[i][j+1]){
					temp = matrix[i][j];
					matrix[i][j] = matrix[i][j+1];
					matrix[i][j+1] = temp;
					swapped = true;
					}

					else{}
				}//end for
				}//end while
			}
		
	

			else if(i % 2 == 1){
				n=0;
				swapped = true;
				while (swapped){
				swapped = false;
				n++;
				for (int j = 0; j < COLUMNS - n; j++){
					
					if (matrix[i][j] < matrix[i][j+1]){
					temp = matrix[i][j];
					matrix[i][j] = matrix[i][j+1];
					matrix[i][j+1] = temp;
					swapped = true;
					}

					else{}
				}//end for
				}//end while
			}
		}
	}

	else if (c == 1){
		for (int j = 0; j < COLUMNS; j++){
			n = 0;
			swapped = true;
			while (swapped){
			swapped = false;
			n++;
			for (int i = 0; i < ROWS - n; i++){
				
				if(matrix[i][j] > matrix[i+1][j]){
				temp = matrix[i][j];
				matrix[i][j] = matrix[i+1][j];
				matrix[i+1][j] = temp;
				swapped = true;
				}
				else{}
						
			}//end for
			}//end while
		}	
	}
	else{}

	c = (c == 0) ? 1:0;

	//Remember to take out
	for (int i=0; i<ROWS; i++){
    	for(int j=0; j<COLUMNS; j++){
         	printf("%d     ", matrix[i][j]);
		}
	printf("\n");
	}
	pthread_mutex_unlock (&mutexsort);
	pthread_exit(NULL);
}



int main ()
{
	pthread_mutex_init(&mutexsort, NULL);
	pthread_t threads[NUM_THREADS];


	// Read numbers from .txt file 
	FILE *in_data;
	in_data = fopen("input.txt", "r");

	for (int i=0; i<4; i++)
	{
		for (int j=0; j<4; j++)
		{
			fscanf(in_data,"%d",&matrix[i][j]);
		}
	}

	for (int i=0; i<ROWS; i++){
    	for(int j=0; j<COLUMNS; j++){
         	printf("%d     ", matrix[i][j]);
		}
	printf("\n");
	}

	//Initialize threads?

	for(long t=0; t<NUM_THREADS; t++){
      printf("Creating thread %ld\n", t);

      int rc = pthread_create(&threads[t], NULL, bubbleSort, (void *)t);
      
      if (rc){
         printf("Code returned, pthread_create() is %d\n", rc);
         exit(-1);
    	}
    }

/*	for (int i=0; i<ROWS; i++){
    	for(int j=0; j<COLUMNS; j++){
         	printf("%d     ", matrix[i][j]);
		}
	printf("\n");
	}*/
    pthread_mutex_destroy(&mutexsort);
    pthread_exit(NULL);
}
