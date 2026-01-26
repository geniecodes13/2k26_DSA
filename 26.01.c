/**
 * Return an array of arrays of size *returnSize.
 * The sizes of the arrays are returned as *returnColumnSizes array.
 * Note: Both returned array and *columnSizes array must be malloced, assume caller calls free().
 */
// comparator for qsort
int compare(const void*a, const void*b){
    return (*(int*)a-*(int *)b);
}



int** minimumAbsDifference(int* arr, int arrSize, int* returnSize, int** returnColumnSizes) {
    qsort(arr, arrSize, sizeof(int),compare);

    int minDiff=INT_MAX;

    // first pass: find min difference

for(int i=0; i<arrSize-1; i++){
    int diff=arr[i+1]-arr[i];
    if(diff<minDiff){
        minDiff=diff;
    }
    //second pass: count how many pairs have mindiff
    }
    int count=0;
    for (int i=0;i<arrSize-1;i++)
    {
        if(arr[i+1]-arr[i]==minDiff){
            count++;
        }
    }
    //allocate result
    int **result=(int**)malloc(count*sizeof(int*));
    *returnColumnSizes=(int*)malloc(count* sizeof(int));
    *returnSize=count;

    int idx=0;
    for(int i=0;i<arrSize-1;i++){
        if(arr[i+1]-arr[i]==minDiff){
            result[idx]=(int*)malloc(2*sizeof(int));
            result[idx][0]=arr[i];
            result[idx][1]=arr[i+1];
            (*returnColumnSizes)[idx]=2;
            idx++;
        }
    }
    return result;
}
