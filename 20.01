int* minBitwiseArray(int* nums, int numsSize, int* returnSize) {
    int* result = (int*)malloc(numsSize * sizeof(int));
    *returnSize = numsSize;

    for (int i = 0; i < numsSize; i++) {
        int original = nums[i];
        int candidate = -1;

        for (int j = 1; j < original; j++) {
            if ((j | (j + 1)) == original) {
                candidate = j;
                break;
            }
        }

        result[i] = candidate;
    }

    return result;
}
