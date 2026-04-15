import heapq

def dijkstra(graph, start):
    n = len(graph)
    distances = [float('inf')] * n
    distances[start] = 0

    pq = []
    heapq.heappush(pq, (0, start))

    while pq:
        current_distance, u = heapq.heappop(pq)

        if current_distance > distances[u]:
            continue

        for v, weight in graph[u]:
            distance = current_distance + weight

            if distance < distances[v]:
                distances[v] = distance
                heapq.heappush(pq, (distance, v))

    return distances


# Sample graph with 6 vertices (0 to 5)
graph = {
    0: [(1, 4), (2, 1)],
    1: [(3, 1)],
    2: [(1, 2), (3, 5)],
    3: [(4, 3)],
    4: [(5, 2)],
    5: []
}

start_vertex = 0
distances = dijkstra(graph, start_vertex)

print("Shortest distances from vertex", start_vertex)
for i in range(len(distances)):
    print(f"To {i}: {distances[i]}")