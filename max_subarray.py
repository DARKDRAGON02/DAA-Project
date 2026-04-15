def max_crossing_sum(arr, left, mid, right):
    left_sum = float('-inf')
    sum = 0
    for i in range(mid, left - 1, -1):
        sum += arr[i]
        left_sum = max(left_sum, sum)

    right_sum = float('-inf')
    sum = 0
    for i in range(mid + 1, right + 1):
        sum += arr[i]
        right_sum = max(right_sum, sum)

    return left_sum + right_sum


def max_subarray(arr, left, right):
    print(f"Divide: left={left}, right={right}")

    if left == right:
        print(f"Base case at index {left} value {arr[left]}")
        return arr[left]

    mid = (left + right) // 2

    left_max = max_subarray(arr, left, mid)
    right_max = max_subarray(arr, mid + 1, right)
    cross_max = max_crossing_sum(arr, left, mid, right)

    result = max(left_max, right_max, cross_max)

    print(f"Combine: left={left_max}, right={right_max}, cross={cross_max} => max={result}")

    return result


arr = [2, -4, 3, -1, 5, -6, 2, 4]

result = max_subarray(arr, 0, len(arr) - 1)

print("\nMaximum Subarray Sum:", result)