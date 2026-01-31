#define INF (INT_MAX / 2)

typedef struct Trie {
    struct Trie* child[26];
    int id;
} Trie;

Trie* createTrie() {
    Trie* node = (Trie*)malloc(sizeof(Trie));
    for (int i = 0; i < 26; i++) {
        node->child[i] = NULL;
    }
    node->id = -1;
    return node;
}

int add(Trie* node, const char* word, int* index) {
    for (int i = 0; word[i]; i++) {
        int ch = word[i] - 'a';
        if (!node->child[ch]) {
            node->child[ch] = createTrie();
        }
        node = node->child[ch];
    }
    if (node->id == -1) {
        node->id = ++(*index);
    }
    return node->id;
}

void update(long long* x, long long y) {
    if (*x == -1 || y < *x) {
        *x = y;
    }
}

long long minimumCost(char* source, char* target, char** original,
                      int originalSize, char** changed, int changedSize,
                      int* cost, int costSize) {
    int n = strlen(source);
    int m = originalSize;
    Trie* root = createTrie();

    int p = -1;
    int nodeCount = m * 2;
    int** G = (int**)malloc(nodeCount * sizeof(int*));
    for (int i = 0; i < nodeCount; i++) {
        G[i] = (int*)malloc(nodeCount * sizeof(int));
        for (int j = 0; j < nodeCount; j++) {
            G[i][j] = INF;
        }
        G[i][i] = 0;
    }

    for (int i = 0; i < m; i++) {
        int x = add(root, original[i], &p);
        int y = add(root, changed[i], &p);
        G[x][y] = fmin(G[x][y], cost[i]);
    }

    for (int k = 0; k <= p; k++) {
        for (int i = 0; i <= p; i++) {
            for (int j = 0; j <= p; j++) {
                G[i][j] = fmin(G[i][j], G[i][k] + G[k][j]);
            }
        }
    }

    long long* f = (long long*)malloc(n * sizeof(long long));
    for (int i = 0; i < n; i++) {
        f[i] = -1;
    }
    for (int j = 0; j < n; j++) {
        if (j > 0 && f[j - 1] == -1) {
            continue;
        }

        long long base = (j == 0 ? 0 : f[j - 1]);
        if (source[j] == target[j]) {
            update(&f[j], base);
        }

        Trie* u = root;
        Trie* v = root;
        for (int i = j; i < n; i++) {
            u = u->child[source[i] - 'a'];
            v = v->child[target[i] - 'a'];
            if (!u || !v) {
                break;
            }
            if (u->id != -1 && v->id != -1 && G[u->id][v->id] != INF) {
                update(&f[i], base + G[u->id][v->id]);
            }
        }
    }

    long long result = f[n - 1];
    for (int i = 0; i < nodeCount; i++) {
        free(G[i]);
    }
    free(G);
    free(f);

    return result;
}
