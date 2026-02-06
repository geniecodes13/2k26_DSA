//3013

#define MIN_QUEUE_SIZE 64

typedef struct Element {
    int data[2];
} Element;

typedef bool (*compare)(const void*, const void*);

typedef struct PriorityQueue {
    Element* arr;
    int capacity;
    int queueSize;
    compare cmpFunc;
} PriorityQueue;

typedef struct HashItem {
    int key;
    int val;
    UT_hash_handle hh;
} HashItem;

// comparison function: min-heap
static bool minHeapCmp(const void* a, const void* b) {
    Element* e1 = (Element*)a;
    Element* e2 = (Element*)b;
    return e1->data[0] > e2->data[0];
}

// comparison function: max-heap
static bool maxHeapCmp(const void* a, const void* b) {
    Element* e1 = (Element*)a;
    Element* e2 = (Element*)b;
    return e1->data[0] < e2->data[0];
}

static void memswap(void* m1, void* m2, size_t size) {
    unsigned char* a = (unsigned char*)m1;
    unsigned char* b = (unsigned char*)m2;
    while (size--) {
        *b ^= *a ^= *b ^= *a;
        a++;
        b++;
    }
}

static void swap(Element* arr, int i, int j) {
    memswap(&arr[i], &arr[j], sizeof(Element));
}

static void down(Element* arr, int size, int i, compare cmpFunc) {
    for (int k = 2 * i + 1; k < size; k = 2 * k + 1) {
        if (k + 1 < size && cmpFunc(&arr[k], &arr[k + 1])) {
            k++;
        }
        if (cmpFunc(&arr[k], &arr[(k - 1) / 2])) {
            break;
        }
        swap(arr, k, (k - 1) / 2);
    }
}

PriorityQueue* createPriorityQueue(compare cmpFunc) {
    PriorityQueue* obj = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    obj->capacity = MIN_QUEUE_SIZE;
    obj->arr = (Element*)malloc(sizeof(Element) * obj->capacity);
    obj->queueSize = 0;
    obj->cmpFunc = cmpFunc;
    return obj;
}

void heapify(PriorityQueue* obj) {
    for (int i = obj->queueSize / 2 - 1; i >= 0; i--) {
        down(obj->arr, obj->queueSize, i, obj->cmpFunc);
    }
}

void enQueue(PriorityQueue* obj, Element* e) {
    if (obj->queueSize == obj->capacity) {
        obj->capacity *= 2;
        obj->arr = realloc(obj->arr, sizeof(Element) * obj->capacity);
    }
    memcpy(&obj->arr[obj->queueSize], e, sizeof(Element));
    for (int i = obj->queueSize;
         i > 0 && obj->cmpFunc(&obj->arr[(i - 1) / 2], &obj->arr[i]);
         i = (i - 1) / 2) {
        swap(obj->arr, i, (i - 1) / 2);
    }
    obj->queueSize++;
}

Element* deQueue(PriorityQueue* obj) {
    if (obj->queueSize == 0) return NULL;
    swap(obj->arr, 0, obj->queueSize - 1);
    down(obj->arr, obj->queueSize - 1, 0, obj->cmpFunc);
    Element* e = &obj->arr[obj->queueSize - 1];
    obj->queueSize--;
    return e;
}

bool isEmpty(const PriorityQueue* obj) { return obj->queueSize == 0; }

Element* top(const PriorityQueue* obj) {
    if (obj->queueSize == 0) {
        return NULL;
    } else {
        return &obj->arr[0];
    }
}

int size(const PriorityQueue* obj) { return obj->queueSize; }

void clear(PriorityQueue* obj) { obj->queueSize = 0; }

void freeQueue(PriorityQueue* obj) {
    free(obj->arr);
    free(obj);
}

HashItem* hashFindItem(HashItem** obj, int key) {
    HashItem* pEntry = NULL;
    HASH_FIND_INT(*obj, &key, pEntry);
    return pEntry;
}

bool hashAddItem(HashItem** obj, int key, int val) {
    if (hashFindItem(obj, key)) {
        return false;
    }
    HashItem* pEntry = (HashItem*)malloc(sizeof(HashItem));
    pEntry->key = key;
    pEntry->val = val;
    HASH_ADD_INT(*obj, key, pEntry);
    return true;
}

bool hashSetItem(HashItem** obj, int key, int val) {
    HashItem* pEntry = hashFindItem(obj, key);
    if (!pEntry) {
        hashAddItem(obj, key, val);
    } else {
        pEntry->val = val;
    }
    return true;
}

int hashGetItem(HashItem** obj, int key, int defaultVal) {
    HashItem* pEntry = hashFindItem(obj, key);
    if (!pEntry) {
        return defaultVal;
    }
    return pEntry->val;
}

void hashEraseItem(HashItem** obj, int key) {
    HashItem* pEntry = NULL;
    HASH_FIND_INT(*obj, &key, pEntry);
    if (pEntry) {
        HASH_DEL(*obj, pEntry);
        free(pEntry);
    }
}

void hashFree(HashItem** obj) {
    HashItem *curr = NULL, *tmp = NULL;
    HASH_ITER(hh, *obj, curr, tmp) {
        HASH_DEL(*obj, curr);
        free(curr);
    }
}

typedef struct {
    int k;
    HashItem* st1;
    HashItem* st2;
    PriorityQueue* tree1;
    PriorityQueue* tree2;
    int st1Size;
    int st2Size;
    long long sm;
} Container;

Container* createContainer(int k) {
    Container* cnt = (Container*)malloc(sizeof(Container));
    cnt->k = k;
    cnt->st1 = NULL;
    cnt->st2 = NULL;
    cnt->tree1 = createPriorityQueue(maxHeapCmp);
    cnt->tree2 = createPriorityQueue(minHeapCmp);
    cnt->st1Size = 0;
    cnt->st2Size = 0;
    cnt->sm = 0;
    return cnt;
}

void addOne(HashItem** map, PriorityQueue* heap, int key) {
    int count = hashGetItem(map, key, 0);
    hashSetItem(map, key, count + 1);

    if (count == 0) {
        Element e;
        e.data[0] = key;
        e.data[1] = 0;
        enQueue(heap, &e);
    }
}

void removeOne(HashItem** map, PriorityQueue* heap, int key) {
    HashItem* entry = hashFindItem(map, key);
    if (!entry) {
        return;
    }
    entry->val--;
    if (entry->val == 0) {
        hashEraseItem(map, key);
    }
}

void adjust(Container* cnt) {
    while (cnt->st1Size < cnt->k && cnt->st2Size > 0) {
        while (!isEmpty(cnt->tree2)) {
            Element* topElem = top(cnt->tree2);
            if (!topElem || hashGetItem(&cnt->st2, topElem->data[0], 0) == 0) {
                deQueue(cnt->tree2);
            } else {
                break;
            }
        }

        if (isEmpty(cnt->tree2)) {
            break;
        }

        int x = top(cnt->tree2)->data[0];
        removeOne(&cnt->st2, cnt->tree2, x);
        cnt->st2Size--;
        addOne(&cnt->st1, cnt->tree1, x);
        cnt->st1Size++;
        cnt->sm += x;
    }

    while (cnt->st1Size > cnt->k) {
        while (!isEmpty(cnt->tree1)) {
            Element* topElem = top(cnt->tree1);
            if (!topElem || hashGetItem(&cnt->st1, topElem->data[0], 0) == 0) {
                deQueue(cnt->tree1);
            } else {
                break;
            }
        }

        if (isEmpty(cnt->tree1)) {
            break;
        }
        int x = top(cnt->tree1)->data[0];
        removeOne(&cnt->st1, cnt->tree1, x);
        cnt->st1Size--;
        cnt->sm -= x;

        addOne(&cnt->st2, cnt->tree2, x);
        cnt->st2Size++;
    }
}

void containerAdd(Container* cnt, int x) {
    while (!isEmpty(cnt->tree2)) {
        Element* topElem = top(cnt->tree2);
        if (!topElem || hashGetItem(&cnt->st2, topElem->data[0], 0) == 0) {
            deQueue(cnt->tree2);
        } else {
            break;
        }
    }

    if (cnt->st2Size > 0 && !isEmpty(cnt->tree2) &&
        x >= top(cnt->tree2)->data[0]) {
        addOne(&cnt->st2, cnt->tree2, x);
        cnt->st2Size++;
    } else {
        addOne(&cnt->st1, cnt->tree1, x);
        cnt->st1Size++;
        cnt->sm += x;
    }
    adjust(cnt);
}

void containerErase(Container* cnt, int x) {
    if (hashGetItem(&cnt->st1, x, 0) > 0) {
        removeOne(&cnt->st1, cnt->tree1, x);
        cnt->st1Size--;
        cnt->sm -= x;
    } else if (hashGetItem(&cnt->st2, x, 0) > 0) {
        removeOne(&cnt->st2, cnt->tree2, x);
        cnt->st2Size--;
    }
    adjust(cnt);
}

long long containerSum(Container* cnt) { return cnt->sm; }

void freeContainer(Container* cnt) {
    hashFree(&cnt->st1);
    hashFree(&cnt->st2);
    freeQueue(cnt->tree1);
    freeQueue(cnt->tree2);
    free(cnt);
}

long long minimumCost(int* nums, int numsSize, int k, int dist) {
    Container* cnt = createContainer(k - 2);
    for (int i = 1; i < k - 1; i++) {
        containerAdd(cnt, nums[i]);
    }

    long long ans = containerSum(cnt) + nums[k - 1];
    for (int i = k; i < numsSize; i++) {
        int j = i - dist - 1;
        if (j > 0) {
            containerErase(cnt, nums[j]);
        }
        containerAdd(cnt, nums[i - 1]);
        long long current = containerSum(cnt) + nums[i];
        if (current < ans) {
            ans = current;
        }
    }

    long long result = ans + nums[0];
    freeContainer(cnt);
    return result;
}
