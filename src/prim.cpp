#include "prim.hpp"

#include "binomial_heap.hpp"
#include "fibonacci_heap.hpp"

#include <chrono>
#include <limits>
#include <stdexcept>
#include <vector>

namespace prim {

namespace {

template <typename Heap>
PrimResult runPrim(const Graph &graph, bool measureDecreaseKey) {
  if (graph.vertexCount() == 0) {
    throw std::runtime_error("Prim requiere un grafo no vacio");
  }
  const double infinity = std::numeric_limits<double>::infinity();
  std::vector<double> costs(graph.vertexCount(), infinity);
  std::vector<Vertex> parents(graph.vertexCount(), 0);
  std::vector<bool> hasParent(graph.vertexCount(), false);
  std::vector<bool> inQueue(graph.vertexCount(), true);
  costs[0] = 0.0;

  Heap queue;
  queue.build(costs);
  PrimResult result;
  result.edges.reserve(graph.vertexCount() - 1);
  while (!queue.empty()) {
    const QueueEntry entry = queue.extractMin();
    const Vertex u = entry.vertex;
    inQueue[u] = false;
    if (entry.key == infinity) {
      throw std::runtime_error("Prim recibio un grafo no conexo");
    }
    if (hasParent[u]) {
      result.totalWeight += entry.key;
      ++result.edgeCount;
      result.edges.push_back({parents[u], u, entry.key});
    }
    for (const AdjacentEdge &edge : graph.neighbors(u)) {
      if (!inQueue[edge.to] || edge.weight >= costs[edge.to]) {
        continue;
      }
      costs[edge.to] = edge.weight;
      parents[edge.to] = u;
      hasParent[edge.to] = true;
      ++result.metrics.decreaseCalls;
      if (measureDecreaseKey) {
        const auto begin = std::chrono::steady_clock::now();
        result.metrics.structuralOperations +=
            queue.decreaseKey(edge.to, edge.weight);
        const auto end = std::chrono::steady_clock::now();
        result.metrics.decreaseNanoseconds +=
            std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin)
                .count();
      } else {
        result.metrics.structuralOperations +=
            queue.decreaseKey(edge.to, edge.weight);
      }
    }
  }
  if (result.edgeCount + 1 != graph.vertexCount()) {
    throw std::runtime_error("el resultado de Prim no es un arbol abarcador");
  }
  return result;
}

} // namespace

PrimResult primBinomial(const Graph &graph, bool measureDecreaseKey) {
  return runPrim<BinomialHeap>(graph, measureDecreaseKey);
}

PrimResult primFibonacci(const Graph &graph, bool measureDecreaseKey) {
  return runPrim<FibonacciHeap>(graph, measureDecreaseKey);
}

} // namespace prim
