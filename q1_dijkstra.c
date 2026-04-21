/*
 * Question 1 - Decrease and Conquer: Dijkstra's Shortest Path Algorithm
 * Implements both Adjacency Matrix and Binary Heap (Min-Priority Queue) versions
 * and experimentally compares them on dense and sparse graphs.
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <string.h>

#define INF INT_MAX
#define MAX_VERTICES 100

/* ============================================================
   PART 1: ADJACENCY MATRIX IMPLEMENTATION  O(V^2)
   ============================================================ */

/* Find vertex with minimum distance not yet finalized */
int minDistance(int dist[], int visited[], int V) {
    int min = INF, min_idx = -1;
    for (int v = 0; v < V; v++) {
        if (!visited[v] && dist[v] <= min) {
            min = dist[v];
            min_idx = v;
        }
    }
    return min_idx;
}

/*
 * Dijkstra using Adjacency Matrix
 * graph[i][j] = weight of edge from i to j (0 means no edge)
 * dist[]      = shortest distance from src to each vertex
 * parent[]    = to reconstruct path
 *
 * HOW PROBLEM SIZE DECREASES (Decrease and Conquer):
 *   At each iteration, we "finalize" one vertex (add it to the
 *   visited/settled set S). The unvisited set shrinks by 1 each step.
 *   Problem reduces from |V| unknowns to |V|-1, |V|-2, ... 0.
 */
void dijkstraMatrix(int graph[MAX_VERTICES][MAX_VERTICES], int V, int src,
                    int dist[], int parent[]) {
    int visited[MAX_VERTICES] = {0};

    /* Initialize distances to INF, source distance to 0 */
    for (int i = 0; i < V; i++) {
        dist[i]   = INF;
        parent[i] = -1;
    }
    dist[src] = 0;

    /* Process V vertices */
    for (int count = 0; count < V - 1; count++) {
        /* DECREASE step: pick the closest unvisited vertex */
        int u = minDistance(dist, visited, V);
        if (u == -1) break; /* all remaining vertices are unreachable */

        visited[u] = 1; /* finalize vertex u */

        /* Relax edges from u */
        for (int v = 0; v < V; v++) {
            if (!visited[v] && graph[u][v] != 0 &&
                dist[u] != INF &&
                dist[u] + graph[u][v] < dist[v]) {
                dist[v]   = dist[u] + graph[u][v];
                parent[v] = u;
            }
        }
    }
}

/* ============================================================
   PART 2: BINARY MIN-HEAP (PRIORITY QUEUE) IMPLEMENTATION  O((V+E) log V)
   ============================================================ */

typedef struct {
    int vertex;
    int dist;
} HeapNode;

typedef struct {
    HeapNode *arr;
    int *pos;   /* pos[v] = index of vertex v in heap array */
    int size;
    int capacity;
} MinHeap;

MinHeap* createHeap(int capacity) {
    MinHeap *h  = (MinHeap*)malloc(sizeof(MinHeap));
    h->arr      = (HeapNode*)malloc(capacity * sizeof(HeapNode));
    h->pos      = (int*)malloc(capacity * sizeof(int));
    h->size     = 0;
    h->capacity = capacity;
    return h;
}

void freeHeap(MinHeap *h) {
    free(h->arr);
    free(h->pos);
    free(h);
}

void swapNodes(MinHeap *h, int i, int j) {
    h->pos[h->arr[i].vertex] = j;
    h->pos[h->arr[j].vertex] = i;
    HeapNode tmp = h->arr[i];
    h->arr[i]    = h->arr[j];
    h->arr[j]    = tmp;
}

void minHeapify(MinHeap *h, int idx) {
    int smallest = idx;
    int left     = 2 * idx + 1;
    int right    = 2 * idx + 2;
    if (left  < h->size && h->arr[left].dist  < h->arr[smallest].dist) smallest = left;
    if (right < h->size && h->arr[right].dist < h->arr[smallest].dist) smallest = right;
    if (smallest != idx) {
        swapNodes(h, smallest, idx);
        minHeapify(h, smallest);
    }
}

int isEmpty(MinHeap *h) { return h->size == 0; }

HeapNode extractMin(MinHeap *h) {
    HeapNode root      = h->arr[0];
    HeapNode lastNode  = h->arr[h->size - 1];
    h->arr[0]          = lastNode;
    h->pos[lastNode.vertex] = 0;
    h->pos[root.vertex]     = h->size - 1;
    h->size--;
    minHeapify(h, 0);
    return root;
}

/* Decrease key: update distance of vertex v to dist */
void decreaseKey(MinHeap *h, int v, int dist) {
    int i      = h->pos[v];
    h->arr[i].dist = dist;
    /* Bubble up */
    while (i > 0 && h->arr[i].dist < h->arr[(i - 1) / 2].dist) {
        swapNodes(h, i, (i - 1) / 2);
        i = (i - 1) / 2;
    }
}

int inHeap(MinHeap *h, int v) {
    return h->pos[v] < h->size;
}

/* Adjacency list node */
typedef struct AdjNode {
    int dest, weight;
    struct AdjNode *next;
} AdjNode;

typedef struct {
    AdjNode **head;
    int V;
} AdjList;

AdjList* createAdjList(int V) {
    AdjList *g = (AdjList*)malloc(sizeof(AdjList));
    g->V       = V;
    g->head    = (AdjNode**)calloc(V, sizeof(AdjNode*));
    return g;
}

void addEdge(AdjList *g, int u, int v, int w) {
    AdjNode *node  = (AdjNode*)malloc(sizeof(AdjNode));
    node->dest     = v;
    node->weight   = w;
    node->next     = g->head[u];
    g->head[u]     = node;
}

void freeAdjList(AdjList *g) {
    for (int i = 0; i < g->V; i++) {
        AdjNode *cur = g->head[i];
        while (cur) { AdjNode *tmp = cur->next; free(cur); cur = tmp; }
    }
    free(g->head);
    free(g);
}

/*
 * Dijkstra using Min-Heap + Adjacency List
 */
void dijkstraHeap(AdjList *g, int src, int dist[], int parent[]) {
    int V = g->V;
    MinHeap *h = createHeap(V);

    for (int v = 0; v < V; v++) {
        dist[v]   = INF;
        parent[v] = -1;
        h->arr[v].vertex = v;
        h->arr[v].dist   = INF;
        h->pos[v]        = v;
    }
    dist[src]       = 0;
    h->arr[src].dist = 0;
    h->size          = V;

    while (!isEmpty(h)) {
        HeapNode minNode = extractMin(h);  /* DECREASE step: extract closest */
        int u = minNode.vertex;

        AdjNode *adj = g->head[u];
        while (adj) {
            int v = adj->dest;
            if (inHeap(h, v) && dist[u] != INF &&
                dist[u] + adj->weight < dist[v]) {
                dist[v]   = dist[u] + adj->weight;
                parent[v] = u;
                decreaseKey(h, v, dist[v]);
            }
            adj = adj->next;
        }
    }
    freeHeap(h);
}

/* ============================================================
   UTILITY FUNCTIONS
   ============================================================ */

void printPath(int parent[], int j) {
    if (parent[j] == -1) { printf("%d", j); return; }
    printPath(parent, parent[j]);
    printf(" -> %d", j);
}

void printDistances(int dist[], int parent[], int V, int src) {
    printf("\n%-10s %-15s %s\n", "Vertex", "Distance", "Path");
    printf("------------------------------------------\n");
    for (int i = 0; i < V; i++) {
        printf("%-10d ", i);
        if (dist[i] == INF) printf("%-15s", "INF");
        else                printf("%-15d", dist[i]);
        if (dist[i] == INF) printf("No path");
        else                printPath(parent, i);
        printf("\n");
    }
}

/* ============================================================
   TASK 3: TRACE ON SAMPLE 6-VERTEX GRAPH
   ============================================================ */

void traceSampleGraph() {
    printf("\n========================================\n");
    printf("  TASK 3: Trace on 6-Vertex Sample Graph\n");
    printf("========================================\n");

    /*
     * Sample graph (directed, weighted):
     *
     *      2       3
     *  0 -----> 1 -----> 3
     *  |        |        |
     *  6        8        9
     *  |        |        |
     *  v        v        v
     *  2 -----> 4 -----> 5
     *      7        4
     *
     * Also: 0->2=6, 1->4=8, 2->4=7, 3->5=9, 4->5=4, 1->3=3, 0->1=2
     */
    int V = 6;
    int graph[MAX_VERTICES][MAX_VERTICES] = {0};

    /* Add directed edges */
    graph[0][1] = 2;
    graph[0][2] = 6;
    graph[1][3] = 3;
    graph[1][4] = 8;
    graph[2][4] = 7;
    graph[3][5] = 9;
    graph[4][5] = 4;

    printf("\nGraph edges (directed):\n");
    printf("  0->1 (2),  0->2 (6)\n");
    printf("  1->3 (3),  1->4 (8)\n");
    printf("  2->4 (7)\n");
    printf("  3->5 (9)\n");
    printf("  4->5 (4)\n");
    printf("\nSource vertex: 0\n");

    /* Step-by-step trace */
    int dist[MAX_VERTICES], parent[MAX_VERTICES];
    int visited[MAX_VERTICES] = {0};

    for (int i = 0; i < V; i++) { dist[i] = INF; parent[i] = -1; }
    dist[0] = 0;

    printf("\nStep-by-step distance evolution:\n");
    printf("%-6s | %-5s %-5s %-5s %-5s %-5s %-5s | %-10s\n",
           "Step","d[0]","d[1]","d[2]","d[3]","d[4]","d[5]","Finalized");
    printf("-------+----------------------------------+-----------\n");

    for (int step = 0; step < V; step++) {
        /* Print current distances */
        printf("%-6d | ", step);
        for (int i = 0; i < V; i++) {
            if (dist[i] == INF) printf("%-5s ", "INF");
            else                printf("%-5d ", dist[i]);
        }

        int u = minDistance(dist, visited, V);
        if (u == -1) { printf("(none)\n"); break; }
        visited[u] = 1;
        printf("| v%d\n", u);

        /* Relax */
        for (int v = 0; v < V; v++) {
            if (!visited[v] && graph[u][v] != 0 &&
                dist[u] != INF && dist[u] + graph[u][v] < dist[v]) {
                dist[v]   = dist[u] + graph[u][v];
                parent[v] = u;
            }
        }
    }

    printf("\nFinal shortest distances from vertex 0:\n");
    printDistances(dist, parent, V, 0);
}

/* ============================================================
   TASK 5: EXPERIMENTAL COMPARISON
   ============================================================ */

/* Generate random dense graph (adjacency matrix) */
void generateDenseGraph(int graph[MAX_VERTICES][MAX_VERTICES], int V, int maxW) {
    for (int i = 0; i < V; i++)
        for (int j = 0; j < V; j++)
            graph[i][j] = (i != j && rand() % 2) ? (rand() % maxW + 1) : 0;
}

/* Generate random sparse graph (adjacency list) */
void generateSparseGraph(AdjList *g, int V, int maxW) {
    /* Each vertex gets ~2-3 edges */
    for (int i = 0; i < V; i++) {
        int edges = rand() % 3 + 1;
        for (int e = 0; e < edges; e++) {
            int j = rand() % V;
            if (j != i) addEdge(g, i, j, rand() % maxW + 1);
        }
    }
}

/* Convert matrix to adj list */
AdjList* matrixToList(int graph[MAX_VERTICES][MAX_VERTICES], int V) {
    AdjList *g = createAdjList(V);
    for (int i = 0; i < V; i++)
        for (int j = 0; j < V; j++)
            if (graph[i][j] != 0) addEdge(g, i, j, graph[i][j]);
    return g;
}

void experimentalComparison() {
    printf("\n========================================\n");
    printf("  TASK 5: Experimental Comparison\n");
    printf("========================================\n");

    srand(42);
    int sizes[] = {20, 50, 80};
    int numSizes = 3;

    printf("\n%-6s | %-20s | %-20s\n", "V", "Matrix O(V^2) (us)", "Heap O((V+E)logV) (us)");
    printf("-------+----------------------+----------------------\n");

    int distM[MAX_VERTICES], parentM[MAX_VERTICES];
    int distH[MAX_VERTICES], parentH[MAX_VERTICES];
    int graph[MAX_VERTICES][MAX_VERTICES];

    for (int s = 0; s < numSizes; s++) {
        int V = sizes[s];

        /* Dense graph */
        memset(graph, 0, sizeof(graph));
        generateDenseGraph(graph, V, 20);

        clock_t start = clock();
        dijkstraMatrix(graph, V, 0, distM, parentM);
        clock_t endM = clock();
        long timeMatrix = (long)((endM - start) * 1000000 / CLOCKS_PER_SEC);

        AdjList *gl = matrixToList(graph, V);
        start = clock();
        dijkstraHeap(gl, 0, distH, parentH);
        clock_t endH = clock();
        long timeHeap = (long)((endH - start) * 1000000 / CLOCKS_PER_SEC);
        freeAdjList(gl);

        printf("%-6d | %-20ld | %-20ld  (Dense)\n", V, timeMatrix, timeHeap);

        /* Sparse graph */
        AdjList *gs = createAdjList(V);
        generateSparseGraph(gs, V, 20);

        start = clock();
        dijkstraHeap(gs, 0, distH, parentH);
        endH = clock();
        long timeHeapSparse = (long)((endH - start) * 1000000 / CLOCKS_PER_SEC);
        freeAdjList(gs);

        printf("%-6d | %-20s | %-20ld  (Sparse, heap only)\n", V, "N/A", timeHeapSparse);
    }
    printf("\nConclusion: Matrix is better for dense graphs (low constant),\n");
    printf("Heap is better for sparse graphs (fewer edges to relax).\n");
}

/* ============================================================
   MAIN
   ============================================================ */

int main() {
    printf("=========================================\n");
    printf("  Q1 - Dijkstra's Algorithm (C)\n");
    printf("=========================================\n");

    /* --- Simple demo with adjacency matrix --- */
    printf("\n--- Matrix-based Dijkstra Demo ---\n");
    int V = 5;
    int graph[MAX_VERTICES][MAX_VERTICES] = {0};
    graph[0][1] = 10; graph[0][2] = 3;
    graph[1][2] = 1;  graph[1][3] = 2;
    graph[2][1] = 4;  graph[2][3] = 8; graph[2][4] = 2;
    graph[3][4] = 5;
    graph[4][3] = 1;

    int dist[MAX_VERTICES], parent[MAX_VERTICES];
    dijkstraMatrix(graph, V, 0, dist, parent);
    printf("Source: 0, Vertices: %d\n", V);
    printDistances(dist, parent, V, 0);

    /* --- Simple demo with heap --- */
    printf("\n--- Heap-based Dijkstra Demo ---\n");
    AdjList *g = createAdjList(V);
    addEdge(g, 0, 1, 10); addEdge(g, 0, 2, 3);
    addEdge(g, 1, 2, 1);  addEdge(g, 1, 3, 2);
    addEdge(g, 2, 1, 4);  addEdge(g, 2, 3, 8); addEdge(g, 2, 4, 2);
    addEdge(g, 3, 4, 5);  addEdge(g, 4, 3, 1);

    dijkstraHeap(g, 0, dist, parent);
    printDistances(dist, parent, V, 0);
    freeAdjList(g);

    /* --- Task 3 trace --- */
    traceSampleGraph();

    /* --- Task 5 comparison --- */
    experimentalComparison();

    printf("\n========================================\n");
    printf("  COMPLEXITY SUMMARY\n");
    printf("========================================\n");
    printf("Matrix version : O(V^2) time, O(V^2) space\n");
    printf("Heap   version : O((V+E) log V) time, O(V+E) space\n");
    printf("For dense graph (E~V^2): Heap = O(V^2 logV) > Matrix = O(V^2)\n");
    printf("For sparse graph (E~V) : Heap = O(V logV)  < Matrix = O(V^2)\n");

    return 0;
}
