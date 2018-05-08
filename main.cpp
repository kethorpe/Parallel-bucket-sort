#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <vector>
#include "mpi.h"
#include "quickSort.h"

using namespace std;

int MAX = 640;

void Write(int *sortedArray){
   ofstream myfile;

   myfile.open("out.txt");
   if(myfile.fail()){
        cout << "Unable to open file to write";
        return;
   }

   for(int i = 0; i < MAX; i++){
       myfile << sortedArray[i] << " ";
   }

   myfile.close();
}

void PrintArray(int *arr){
  for (int i = 0; i < MAX; i++)
    {
        cout << arr[i] << ' ';
    }

   cout << endl << endl;
}
 
void RandomWrite(){
   /* initialize random seed: */
   srand(time(NULL));

   int random;
   ofstream myfile;

   myfile.open("in.txt");
   if(myfile.fail()){
        cout << "Unable to open file to write";
        return;
   }

   //random evenly distributed integers
   /*for(int i = 0; i < MAX; i++){
      double myRand = rand()/(1.0 + MAX);
      int range = MAX - 0 + 1;
      int myRand_scaled = (myRand * range);
        myfile << myRand_scaled << " ";
   }*/

   for(int i = MAX - 1; i >= 0; --i){
      myfile << i << " ";
   }

   myfile.close();
}

void PutInBuckets(int *unsortedArray){
	cout << "putting into buckets" << endl;
  
  int minValue = unsortedArray[0];
	int maxValue = unsortedArray[0];

  //find largest and smallest value of array
	for (int i = 1; i < MAX; i++)
	{
		if (unsortedArray[i] > maxValue)
			maxValue = unsortedArray[i];
		if (unsortedArray[i] < minValue)
			minValue = unsortedArray[i];
	}

        cout << "Max Val = " << maxValue << endl;
        cout << "Min Val = " << minValue << endl;
        
        int bucketNum = MAX / 64; 
	int bucketLength = maxValue - minValue + 1;
	vector<int>* bucket = new vector<int>[bucketLength];

  //puts elements into correct buckets
  for (int i=0; i<MAX; i++)
  {
     int bi = unsortedArray[i] / bucketNum; // Index in bucket
     bucket[bi].push_back(unsortedArray[i]);
  }

  //concatenate buckets back into array
  int index = 0;
  for (int i = 0; i < MAX; i++) {
      for (int j = 0; j < bucket[i].size(); j++){
        unsortedArray[index++] = bucket[i][j];
      }
  }

  //PrintArray(unsortedArray);
}

void ReadInFile(int *arr){
   int num;
   ifstream din;
 
   din.open("in.txt");
   if (din.fail()) {
        cout << "Unable to open file to read";
        return;
    }

   for(int i = 0; i < MAX; i++){
        din >> num;
        arr[i] = num;
   }

  din.close();
}

main(int argc, char* argv[])
{
  int my_rank;       /* rank of process      */
  int p;             /* number of processes  */
  int source;        /* rank of sender       */
  int dest;          /* rank of receiver     */
  int tag = 0;       /* tag for messages     */
  MPI_Status status;        /* return status for receive */
  int elementsPerProc; //length of local array
  int *bucket, *sortedArray, *unsortedArray;   //local array for each processor
  double startTime, endTime;

  /* Start up MPI */
  MPI_Init(&argc, &argv);

  /* Find out process rank  */
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  
  /* Find out number of processes */
  MPI_Comm_size(MPI_COMM_WORLD, &p);

    elementsPerProc = MAX/p;

    if(my_rank == 0){

       unsortedArray = (int*)malloc(sizeof(int) * MAX);
       sortedArray = (int*)malloc(sizeof(int) * MAX);

       RandomWrite();

       startTime = MPI_Wtime();

       ReadInFile(unsortedArray);

       PrintArray(unsortedArray);

        PutInBuckets(unsortedArray);

        endTime = MPI_Wtime();

        cout << "Finished Read and fill buckets in " << endTime - startTime << " seconds" << endl;
       
	PrintArray(unsortedArray);

        startTime = MPI_Wtime();
     }

    

    //Scatter the data w/ Process 0
    bucket = (int*)malloc(sizeof(int) * elementsPerProc);
    MPI_Scatter(unsortedArray, elementsPerProc, MPI_INT, bucket, elementsPerProc, MPI_INT, 0, MPI_COMM_WORLD);
    //cout <<"inside scatter" << endl;

    if(my_rank == 0){
        cout << "rank " << my_rank << " has array";
        for(int i = 0; i < elementsPerProc; i++)
           cout << bucket[i] << " ";

        cout << endl << endl;
    }

    

    quickSort(bucket, 0, elementsPerProc - 1);

   //Gather the data w/ Process 0
   MPI_Gather(bucket, elementsPerProc, MPI_INT, sortedArray, elementsPerProc, MPI_INT, 0, MPI_COMM_WORLD);

  if(my_rank == 0){
    endTime = MPI_Wtime();
    cout << "Scattered/Gather and sorted arrays in " << endTime - startTime << " seconds" << endl;
    cout << "After MPI Gather" << endl;
    PrintArray(sortedArray);
    Write(sortedArray);
  }

  /* Shut down MPI */
   MPI_Finalize();
}
