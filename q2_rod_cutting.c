/*
 * Question 2 - Dynamic Programming: Rod Cutting Problem
 *
 * Given a rod of length n and a price table p[1..n],
 * find the maximum revenue by cutting the rod optimally.
 *
 * Tasks covered:
 *  1. Optimal substructure + recurrence relation
 *  2. Bottom-up DP solution
 *  3. Track actual cuts (path reconstruction)
 *  4. Time and space complexity analysis
 *  5. Extension: fixed cutting cost c per cut
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================
   RECURRENCE RELATION:
   r(n) = max over 1<=i<=n of { p[i] + r(n-i) }
   r(0) = 0  (base case: rod of length 0 has revenue 0)

   Optimal substructure: after making first cut of length i,
   the remaining rod of length n-i is solved optimally.
   ============================================================ */

/* ============================================================
   TASK 2: BOTTOM-UP DYNAMIC PROGRAMMING
   ============================================================ */

/*
 * rodCutting: fills dp[0..n] and cuts[0..n]
 * dp[i]   = max revenue for rod of length i
 * cuts[i] = best first-cut length for rod of length i
 *
 * Time:  O(n^2)  - two nested loops
 * Space: O(n)    - two arrays of size n+1
 */
void rodCutting(int p[], int n, int dp[], int cuts[]) {
    dp[0]   = 0;
    cuts[0] = 0;

    for (int j = 1; j <= n; j++) {
        int maxVal = -1;
        for (int i = 1; i <= j; i++) {
            int val = p[i] + dp[j - i];
            if (val > maxVal) {
                maxVal   = val;
                cuts[j]  = i;  /* best first-cut to achieve maxVal */
            }
        }
        dp[j] = maxVal;
    }
}

/* ============================================================
   TASK 3: PRINT ACTUAL CUTS
   ============================================================ */

void printCuts(int cuts[], int n) {
    printf("Cuts: ");
    int remaining = n;
    int first = 1;
    while (remaining > 0) {
        if (!first) printf(" + ");
        printf("%d", cuts[remaining]);
        first = 0;
        remaining -= cuts[remaining];
    }
    printf("\n");
}

/* ============================================================
   TASK 5: EXTENDED VERSION — FIXED COST c PER CUT
   ============================================================
   New recurrence:
   r_c(0) = 0
   r_c(n) = max over 1<=i<=n of:
             p[i]                          if i == n  (no cut needed)
             p[i] + r_c(n-i) - c          if i <  n  (one cut costs c)

   Equivalently:
   r_c(n) = max(
               p[n],                              // sell whole rod
               max_{1<=i<n} { p[i] + r_c(n-i) - c }  // cut
            )
*/
void rodCuttingWithCost(int p[], int n, int c, int dp[], int cuts[]) {
    dp[0]   = 0;
    cuts[0] = 0;

    for (int j = 1; j <= n; j++) {
        /* Option 1: don't cut - sell whole rod of length j */
        int maxVal = p[j];
        cuts[j] = j;  /* no cut */

        /* Option 2: cut at position i (1 <= i < j), subtract cost c */
        for (int i = 1; i < j; i++) {
            int val = p[i] + dp[j - i] - c;
            if (val > maxVal) {
                maxVal  = val;
                cuts[j] = i;
            }
        }
        dp[j] = maxVal;
    }
}

/* ============================================================
   HELPER: Print the full DP table
   ============================================================ */
void printDPTable(int p[], int dp[], int cuts[], int n) {
    printf("\n%-8s %-10s %-10s %-10s\n", "Length", "Price p[]", "MaxRev r[]", "Best Cut");
    printf("------------------------------------------\n");
    for (int i = 0; i <= n; i++) {
        int price = (i > 0) ? p[i] : 0;
        printf("%-8d %-10d %-10d %-10d\n", i, price, dp[i], cuts[i]);
    }
}

/* ============================================================
   DEMO: Run on a sample price table
   ============================================================ */
void runDemo(int p[], int n, const char *label) {
    printf("\n==========================================\n");
    printf("  %s  (Rod length = %d)\n", label, n);
    printf("==========================================\n");

    printf("Price table:\n");
    printf("  Length: ");
    for (int i = 1; i <= n; i++) printf("%4d", i);
    printf("\n  Price:  ");
    for (int i = 1; i <= n; i++) printf("%4d", p[i]);
    printf("\n");

    int *dp   = (int*)malloc((n + 1) * sizeof(int));
    int *cuts = (int*)malloc((n + 1) * sizeof(int));

    /* Basic rod cutting */
    rodCutting(p, n, dp, cuts);

    printf("\nMax revenue for rod of length %d: %d\n", n, dp[n]);
    printCuts(cuts, n);
    printDPTable(p, dp, cuts, n);

    free(dp);
    free(cuts);
}

void runDemoWithCost(int p[], int n, int c) {
    printf("\n==========================================\n");
    printf("  With Cutting Cost c = %d  (Rod length = %d)\n", c, n);
    printf("==========================================\n");

    int *dp   = (int*)malloc((n + 1) * sizeof(int));
    int *cuts = (int*)malloc((n + 1) * sizeof(int));

    rodCuttingWithCost(p, n, c, dp, cuts);

    printf("Max revenue (after cutting costs): %d\n", dp[n]);
    printCuts(cuts, n);
    printDPTable(p, dp, cuts, n);

    free(dp);
    free(cuts);
}

/* ============================================================
   MAIN
   ============================================================ */
int main() {
    printf("=========================================\n");
    printf("  Q2 - Rod Cutting Problem (C)\n");
    printf("=========================================\n");

    /* Classic CLRS example (1-indexed, p[0] unused) */
    /* Length: 1  2  3  4  5   6   7   8   9  10 */
    int p1[] = {0, 1, 5, 8, 9, 10, 17, 17, 20, 24, 30};
    int n1   = 10;
    runDemo(p1, n1, "CLRS Example");

    /* Smaller example to verify manually */
    /* p[1]=2, p[2]=5, p[3]=7, p[4]=8 */
    int p2[] = {0, 2, 5, 7, 8};
    int n2   = 4;
    runDemo(p2, n2, "Small Example (n=4)");

    /* Task 5: cutting cost extension */
    printf("\n==========================================\n");
    printf("  TASK 5: Extension - Cutting Cost\n");
    printf("==========================================\n");
    int c = 2;  /* cost per cut */
    runDemoWithCost(p1, n1, c);

    /* Try c=0 to verify it matches original */
    printf("\nVerification: c=0 should match original result\n");
    runDemoWithCost(p1, n1, 0);

    printf("\n==========================================\n");
    printf("  COMPLEXITY ANALYSIS\n");
    printf("==========================================\n");
    printf("Time  complexity: O(n^2)  - two nested loops over n\n");
    printf("Space complexity: O(n)    - dp[] and cuts[] arrays\n");
    printf("With cutting cost: same complexity, new recurrence\n");
    printf("\nKey insight (Optimal Substructure):\n");
    printf("  r(n) = max{ p[i] + r(n-i) }  for i in 1..n\n");
    printf("  The sub-problem r(n-i) is solved optimally => global optimum.\n");

    return 0;
}
