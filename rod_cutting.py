def rod_cutting(prices, n):
    dp = [0] * (n + 1)
    cuts = [0] * (n + 1)

    for i in range(1, n + 1):
        max_val = -1
        print(f"\nCalculating for rod length {i}")
        for j in range(1, i + 1):
            value = prices[j] + dp[i - j]
            print(f"Try cut {j} -> value = {prices[j]} + {dp[i-j]} = {value}")

            if value > max_val:
                max_val = value
                cuts[i] = j

        dp[i] = max_val
        print(f"Best for length {i} = {dp[i]}")

    return dp, cuts


prices = [0, 2, 5, 7, 8, 10, 17]
n = 6

dp, cuts = rod_cutting(prices, n)

print("\nMaximum Revenue:", dp[n])

print("Cuts used:")
length = n
while length > 0:
    print(cuts[length], end=" ")
    length -= cuts[length]

print("\nDP Table:", dp)