//3651 minimun cost path with teleportation

static int** gridPtr = NULL;

static int cmp(const void* a, const void* b) {
    int* pa = (int*)a;
    int* pb = (int*)b;
    return gridPtr[pa[0]][pa[1]] - gridPtr[pb[0]][pb[1]];
}

int minCost(int** grid, int gridSize, int* gridColSize, int k) {
    int m = gridSize, n = gridColSize[0];
    gridPtr = grid;
    int points[m * n][2], idx = 0;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            points[idx][0] = i, points[idx++][1] = j;
        }
    }
    qsort(points, m * n, sizeof(points[0]), cmp);
    int costs[m][n];
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            costs[i][j] = INT_MAX;
        }
    }
    for (int t = 0; t <= k; t++) {
        int minCost = INT_MAX;
        for (int i = 0, j = 0; i < m * n; i++) {
            minCost = fmin(minCost, costs[points[i][0]][points[i][1]]);
            if (i + 1 < m * n && grid[points[i][0]][points[i][1]] ==
                                     grid[points[i + 1][0]][points[i + 1][1]]) {
                continue;
            }
            for (int r = j; r <= i; r++) {
                costs[points[r][0]][points[r][1]] = minCost;
            }
            j = i + 1;
        }
        for (int i = m - 1; i >= 0; i--) {
            for (int j = n - 1; j >= 0; j--) {
                if (i == m - 1 && j == n - 1) {
                    costs[i][j] = 0;
                    continue;
                }
                if (i != m - 1) {
                    costs[i][j] =
                        fmin(costs[i][j], costs[i + 1][j] + grid[i + 1][j]);
                }
                if (j != n - 1) {
                    costs[i][j] =
                        fmin(costs[i][j], costs[i][j + 1] + grid[i][j + 1]);
                }
            }
        }
    }
    return costs[0][0];
}
