/*
 * Question 3 - Divide and Conquer: Maximum Subarray Problem
 *
 * Given array A[0..n-1] of positive and negative integers,
 * find the contiguous subarray with the maximum sum.
 *
 * Three-case approach:
 *  1. Max subarray entirely in left half
 *  2. Max subarray entirely in right half
 *  3. Max subarray crosses the midpoint
 *
 * Tasks covered:
 *  1. Algorithm design
 *  2. Implementation
 *  3. Recurrence relation: T(n) = 2T(n/2) + O(n)
 *  4. Master Theorem => T(n) = O(n log n)
 *  5. Comparison with brute-force O(n^2)
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#define NEG_INF INT_MIN

/* ============================================================
   RESULT STRUCTURE: holds indices and sum
   ============================================================ */
typedef struct {
    int left;   /* start index of max subarray */
    int right;  /* end   index of max subarray (inclusive) */
    int sum;    /* sum of elements in [left..right] */
} SubarrayResult;

/* ============================================================
   TASK 1 & 2: DIVIDE AND CONQUER ALGORITHM
   ============================================================

   Algorithm design:
   - Base case: single element, return it (even if negative)
   - Divide: split at mid = (low + high) / 2
   - Conquer: recurse on left [low..mid] and right [mid+1..high]
   - Combine: find max crossing subarray through mid
   - Return maximum of three results

   Recurrence: T(n) = 2T(n/2) + cn  (crossing subarray takes O(n))
*/

/*
 * findMaxCrossing: finds the max subarray that crosses the midpoint.
 * It scans left from mid and right from mid+1, finding the best
 * extension in each direction independently.
 * Time: O(n) for the crossing step.
 */
SubarrayResult findMaxCrossing(int A[], int low, int mid, int high) {
    /* Extend LEFT from mid */
    int leftSum = NEG_INF;
    int sum     = 0;
    int maxLeft = mid;
    for (int i = mid; i >= low; i--) {
        sum += A[i];
        if (sum > leftSum) {
            leftSum = sum;
            maxLeft = i;
        }
    }

    /* Extend RIGHT from mid+1 */
    int rightSum = NEG_INF;
    sum          = 0;
    int maxRight = mid + 1;
    for (int j = mid + 1; j <= high; j++) {
        sum += A[j];
        if (sum > rightSum) {
            rightSum = sum;
            maxRight = j;
        }
    }

    SubarrayResult res;
    res.left  = maxLeft;
    res.right = maxRight;
    res.sum   = leftSum + rightSum;
    return res;
}

/*
 * findMaxSubarray: main recursive function.
 * Returns the SubarrayResult for A[low..high].
 */
SubarrayResult findMaxSubarray(int A[], int low, int high) {
    /* BASE CASE: single element */
    if (low == high) {
        SubarrayResult res = {low, high, A[low]};
        return res;
    }

    int mid = (low + high) / 2;

    /* DIVIDE and CONQUER */
    SubarrayResult leftRes   = findMaxSubarray(A, low, mid);
    SubarrayResult rightRes  = findMaxSubarray(A, mid + 1, high);
    SubarrayResult crossRes  = findMaxCrossing(A, low, mid, high);

    /* COMBINE: return maximum of three */
    if (leftRes.sum >= rightRes.sum && leftRes.sum >= crossRes.sum)
        return leftRes;
    else if (rightRes.sum >= leftRes.sum && rightRes.sum >= crossRes.sum)
        return rightRes;
    else
        return crossRes;
}

/* ============================================================
   TASK 5: BRUTE-FORCE O(n^2) FOR COMPARISON
   ============================================================ */
SubarrayResult bruteForce(int A[], int n) {
    SubarrayResult best = {0, 0, NEG_INF};
    for (int i = 0; i < n; i++) {
        int sum = 0;
        for (int j = i; j < n; j++) {
            sum += A[j];
            if (sum > best.sum) {
                best.sum   = sum;
                best.left  = i;
                best.right = j;
            }
        }
    }
    return best;
}

/* ============================================================
   BONUS: Kadane's O(n) algorithm (linear DP) for reference
   ============================================================ */
SubarrayResult kadane(int A[], int n) {
    SubarrayResult best    = {0, 0, A[0]};
    int curSum = A[0], curStart = 0;

    for (int i = 1; i < n; i++) {
        if (curSum + A[i] < A[i]) {
            curSum    = A[i];
            curStart  = i;
        } else {
            curSum += A[i];
        }
        if (curSum > best.sum) {
            best.sum   = curSum;
            best.left  = curStart;
            best.right = i;
        }
    }
    return best;
}

/* ============================================================
   UTILITY
   ============================================================ */
void printArray(int A[], int n) {
    printf("Array: [");
    for (int i = 0; i < n; i++) {
        printf("%d", A[i]);
        if (i < n - 1) printf(", ");
    }
    printf("]\n");
}

void printResult(SubarrayResult r, const char *method) {
    printf("%-20s -> Sum = %-5d  Indices [%d, %d]\n",
           method, r.sum, r.left, r.right);
}

/* ============================================================
   DEMO: trace divide and conquer steps for small array
   ============================================================ */
int traceDepth = 0;
SubarrayResult findMaxSubarrayVerbose(int A[], int low, int high) {
    for (int i = 0; i < traceDepth; i++) printf("  ");
    printf("Call: [%d..%d]\n", low, high);
    traceDepth++;

    SubarrayResult res;
    if (low == high) {
        traceDepth--;
        for (int i = 0; i < traceDepth; i++) printf("  ");
        printf("Base: A[%d]=%d\n", low, A[low]);
        res.left = low; res.right = high; res.sum = A[low];
        return res;
    }

    int mid = (low + high) / 2;
    SubarrayResult leftRes  = findMaxSubarrayVerbose(A, low,    mid);
    SubarrayResult rightRes = findMaxSubarrayVerbose(A, mid+1,  high);
    SubarrayResult crossRes = findMaxCrossing(A, low, mid, high);

    traceDepth--;
    for (int i = 0; i < traceDepth; i++) printf("  ");
    printf("Combine [%d..%d]: L=%d R=%d X=%d\n",
           low, high, leftRes.sum, rightRes.sum, crossRes.sum);

    if (leftRes.sum >= rightRes.sum && leftRes.sum >= crossRes.sum) return leftRes;
    if (rightRes.sum >= leftRes.sum && rightRes.sum >= crossRes.sum) return rightRes;
    return crossRes;
}

/* ============================================================
   TASK 5: EXPERIMENTAL PERFORMANCE COMPARISON
   ============================================================ */
void experimentalComparison() {
    printf("\n==========================================\n");
    printf("  TASK 5: Experimental Comparison\n");
    printf("==========================================\n");

    int sizes[] = {1000, 5000, 20000, 100000};
    int numSizes = 4;

    printf("\n%-10s | %-20s | %-20s\n",
           "n", "D&C O(nlogn) (us)", "Brute O(n^2) (us)");
    printf("--------+----------------------+----------------------\n");

    srand(42);
    for (int s = 0; s < numSizes; s++) {
        int n = sizes[s];
        int *A = (int*)malloc(n * sizeof(int));
        for (int i = 0; i < n; i++)
            A[i] = (rand() % 201) - 100;  /* -100 to 100 */

        /* D&C */
        clock_t start = clock();
        findMaxSubarray(A, 0, n - 1);
        clock_t end = clock();
        long timeDC = (long)((end - start) * 1000000 / CLOCKS_PER_SEC);

        /* Brute force (skip for very large n) */
        long timeBF = -1;
        if (n <= 20000) {
            start  = clock();
            bruteForce(A, n);
            end    = clock();
            timeBF = (long)((end - start) * 1000000 / CLOCKS_PER_SEC);
        }

        if (timeBF == -1)
            printf("%-10d | %-20ld | %-20s\n", n, timeDC, "skipped (too slow)");
        else
            printf("%-10d | %-20ld | %-20ld\n", n, timeDC, timeBF);

        free(A);
    }
    printf("\nConclusion: D&C O(n log n) significantly faster for large n.\n");
}

/* ============================================================
   MAIN
   ============================================================ */
int main() {
    printf("=========================================\n");
    printf("  Q3 - Maximum Subarray (Divide & Conquer) (C)\n");
    printf("=========================================\n");

    /* --- Example 1: Classic CLRS example --- */
    int A1[] = {13, -3, -25, 20, -3, -16, -23, 18, 20, -7, 12, -5, -22, 15, -4, 7};
    int n1   = sizeof(A1) / sizeof(A1[0]);

    printf("\n--- Example 1 (CLRS) ---\n");
    printArray(A1, n1);
    SubarrayResult r1a = findMaxSubarray(A1, 0, n1 - 1);
    SubarrayResult r1b = bruteForce(A1, n1);
    SubarrayResult r1c = kadane(A1, n1);
    printResult(r1a, "Divide & Conquer");
    printResult(r1b, "Brute Force");
    printResult(r1c, "Kadane O(n)");

    /* --- Example 2: All negative --- */
    int A2[] = {-5, -2, -8, -1, -9};
    int n2   = sizeof(A2) / sizeof(A2[0]);
    printf("\n--- Example 2 (All Negative) ---\n");
    printArray(A2, n2);
    SubarrayResult r2 = findMaxSubarray(A2, 0, n2 - 1);
    printResult(r2, "Divide & Conquer");

    /* --- Example 3: All positive --- */
    int A3[] = {3, 5, 1, 2, 4};
    int n3   = sizeof(A3) / sizeof(A3[0]);
    printf("\n--- Example 3 (All Positive) ---\n");
    printArray(A3, n3);
    SubarrayResult r3 = findMaxSubarray(A3, 0, n3 - 1);
    printResult(r3, "Divide & Conquer");

    /* --- Verbose trace for small array --- */
    int A4[] = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
    int n4   = sizeof(A4) / sizeof(A4[0]);
    printf("\n--- Verbose Trace (n=9) ---\n");
    printArray(A4, n4);
    traceDepth = 0;
    SubarrayResult r4 = findMaxSubarrayVerbose(A4, 0, n4 - 1);
    printf("Final: ");
    printResult(r4, "Divide & Conquer");

    /* --- Experimental comparison --- */
    experimentalComparison();

    /* --- Complexity summary --- */
    printf("\n==========================================\n");
    printf("  COMPLEXITY ANALYSIS\n");
    printf("==========================================\n");
    printf("Recurrence:  T(n) = 2T(n/2) + Theta(n)\n");
    printf("             a=2, b=2, f(n)=n\n");
    printf("Master Thm:  n^(log_b a) = n^(log_2 2) = n^1 = n\n");
    printf("             f(n) = Theta(n) => Case 2 of Master Theorem\n");
    printf("             T(n) = Theta(n log n)\n\n");
    printf("Brute Force: O(n^2)\n");
    printf("Kadane DP:   O(n)  -- linear optimal\n");

    return 0;
}
