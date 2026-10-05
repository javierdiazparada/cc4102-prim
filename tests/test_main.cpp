#include "binomial_heap.hpp"
#include "fibonacci_heap.hpp"
#include "graph.hpp"
#include "prim.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace {

using namespace prim;

/**
 * @brief Falla una prueba cuando la condicion no se cumple.
 */
void require(bool condition, const char *message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

/**
 * @brief Prueba las operaciones basicas de un heap.
 */
template <typename Heap> void testHeap() {
  Heap heap;
  heap.build({8.0, 3.0, 7.0, 9.0, 5.0});
  require(heap.validate(), "heap invalido despues de build");
  heap.decreaseKey(3, 1.0);
  require(heap.extractMin().vertex == 3, "decreaseKey no movio el minimo");
  require(heap.validate(), "heap invalido despues de extractMin");
  double previous = -1.0;
  while (!heap.empty()) {
    const auto entry = heap.extractMin();
    require(entry.key >= previous, "el heap extrajo fuera de orden");
    previous = entry.key;
  }
}

/**
 * @brief Prueba el heap con una secuencia mayor de operaciones.
 */
template <typename Heap> void stressHeap() {
  constexpr std::size_t count = 1000;
  std::vector<double> keys(count);
  for (std::size_t i = 0; i < count; ++i) {
    keys[i] = 1000.0 + static_cast<double>(i);
  }
  keys[0] = 0.0;
  Heap heap;
  heap.build(keys);
  require(heap.extractMin().vertex == 0, "primer minimo inesperado");
  for (Vertex vertex = 1; vertex < count; ++vertex) {
    heap.decreaseKey(vertex, -static_cast<double>(vertex));
  }
  double previous = -std::numeric_limits<double>::infinity();
  while (!heap.empty()) {
    const QueueEntry entry = heap.extractMin();
    require(entry.key >= previous, "orden de extraccion invalido");
    previous = entry.key;
  }
}

/**
 * @brief Oraculo de Kruskal utilizado solamente por las pruebas.
 * @note Proceso Union-Find adaptado desde
 * cc4102-kruskal/src/union_find.hpp, repositorio previo del curso en que
 * participo el equipo. Se simplifico la API y se explicito la cantidad de
 * vertices.
 */
double kruskalWeight(std::size_t vertexCount,
                     std::vector<UndirectedEdge> edges) {
  std::sort(edges.begin(), edges.end(),
            [](const auto &a, const auto &b) { return a.weight < b.weight; });
  std::vector<Vertex> parent(vertexCount);
  std::vector<std::size_t> sizes(vertexCount, 1);
  std::iota(parent.begin(), parent.end(), 0);
  auto find = [&](Vertex value) {
    Vertex root = value;
    while (parent[root] != root) {
      root = parent[root];
    }
    while (parent[value] != value) {
      const Vertex next = parent[value];
      parent[value] = root;
      value = next;
    }
    return root;
  };

  double total = 0.0;
  std::size_t count = 0;
  for (const auto &edge : edges) {
    Vertex a = find(edge.u);
    Vertex b = find(edge.v);
    if (a == b) {
      continue;
    }
    if (sizes[a] < sizes[b]) {
      std::swap(a, b);
    }
    parent[b] = a;
    sizes[a] += sizes[b];
    total += edge.weight;
    if (++count + 1 == vertexCount) {
      break;
    }
  }
  require(count + 1 == vertexCount,
          "Kruskal no encontro un arbol abarcador");
  return total;
}

void testPrim() {
  Graph graph(5);
  std::vector<UndirectedEdge> edges{{0, 1, 0.1}, {0, 2, 0.4},
                                    {1, 2, 0.2}, {1, 3, 0.5},
                                    {2, 3, 0.1}, {3, 4, 0.3},
                                    {2, 4, 0.6}};
  for (const auto &edge : edges) {
    graph.addUndirectedEdge(edge.u, edge.v, edge.weight);
  }
  const auto binomial = primBinomial(graph, true);
  const auto fibonacci = primFibonacci(graph, true);
  const double reference = kruskalWeight(graph.vertexCount(), edges);
  require(std::abs(binomial.totalWeight - 0.7) < 1e-12,
          "peso incorrecto del MST binomial");
  require(std::abs(fibonacci.totalWeight - reference) < 1e-12,
          "Prim y Kruskal difieren");
  require(binomial.edgeCount == 4 && fibonacci.edgeCount == 4,
          "cantidad incorrecta de aristas del MST");
}

void testGenerator() {
  Graph first = GraphGenerator(4102).generate(100, 500);
  Graph second = GraphGenerator(4102).generate(100, 500);
  require(first.vertexCount() == 100 && first.edgeCount() == 500,
          "el generador retorno un tamano incorrecto");
  require(validateGraph(first), "el grafo generado es invalido");
  require(std::abs(primBinomial(first, false).totalWeight -
                   primFibonacci(second, false).totalWeight) < 1e-12,
          "los grafos reproducibles o sus pesos MST difieren");
}

} // namespace

int main() {
  try {
    testHeap<BinomialHeap>();
    testHeap<FibonacciHeap>();
    stressHeap<BinomialHeap>();
    stressHeap<FibonacciHeap>();
    testPrim();
    testGenerator();
    std::cout << "todas las pruebas pasaron\n";
    return 0;
  } catch (const std::exception &error) {
    std::cerr << "fallo de prueba: " << error.what() << '\n';
    return 1;
  }
}
