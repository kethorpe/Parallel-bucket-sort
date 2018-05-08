#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "quickSort.h"

using namespace std;

quickSort::quickSort(int *arr, int low, int high)
{
   int left = low, right = high;
   int tmp;
   int pivot = arr[(left + right) / 2];

   /* partition */
   while (left <= right) {
     while(arr[left] < pivot){
         left++;
     }
     while (arr[right] > pivot){
         right--;
     }
     if (left <= right) {
         tmp = arr[left];
         arr[left] = arr[right];
         arr[right] = tmp;
         left++;
         right--;
     }
   };

   /* recursion */
   if (low < right)
      quickSort(arr, low, right);
   if (left < high)
      quickSort(arr, left, high);
}
