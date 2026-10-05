#include "graph.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>
#include <unordered_set>

namespace prim {

Graph::Graph(std::size_t vertexCount) : adjacency_(vertexCount) {}

std::size_t Graph::vertexCount() const { return adjacency_.size(); }

std::size_t Graph::edgeCount() const { return edgeCount_; }

const std::vector<AdjacentEdge> &Graph::neighbors(Vertex vertex) const {
  return adjacency_.at(vertex);
}

void Graph::reserve(Vertex vertex, std::size_t degree) {
  adjacency_.at(vertex).reserve(degree);
}

void Graph::addUndirectedEdge(Vertex u, Vertex v, double weight) {
  if (u == v || u >= adjacency_.size() || v >= adjacency_.size() ||
      !(weight > 0.0 && weight <= 1.0)) {
    throw std::runtime_error("arista no dirigida invalida");
  }
  adjacency_[u].push_back({v, weight});
  adjacency_[v].push_back({u, weight});
  ++edgeCount_;
}

namespace {

std::uint64_t edgeCode(Vertex u, Vertex v) {
  if (u > v) {
    std::swap(u, v);
  }
  return (static_cast<std::uint64_t>(u) << 32U) |
         static_cast<std::uint64_t>(v);
}

} // namespace

GraphGenerator::GraphGenerator(std::uint64_t seed) : seed_(seed) {}

Graph GraphGenerator::generate(std::size_t vertexCount,
                               std::size_t edgeCount) {
  if (vertexCount == 0 || vertexCount > std::numeric_limits<Vertex>::max()) {
    throw std::runtime_error("cantidad de vertices fuera del rango soportado");
  }
  const std::uint64_t maxEdges =
      static_cast<std::uint64_t>(vertexCount) * (vertexCount - 1) / 2;
  if (edgeCount < vertexCount - 1 || edgeCount > maxEdges) {
    throw std::runtime_error(
        "la cantidad de aristas no permite un grafo simple conexo");
  }

  std::mt19937_64 engine(seed_);
  std::uniform_real_distribution<double> weights(
      std::nextafter(0.0, 1.0), 1.0);
  std::uniform_int_distribution<Vertex> vertices(
      0, static_cast<Vertex>(vertexCount - 1));

  std::vector<UndirectedEdge> edges;
  edges.reserve(edgeCount);
  std::unordered_set<std::uint64_t> used;
  used.reserve(edgeCount * 2);

  auto addEdge = [&](Vertex u, Vertex v) {
    const std::uint64_t code = edgeCode(u, v);
    if (!used.insert(code).second) {
      return false;
    }
    edges.push_back({u, v, weights(engine)});
    return true;
  };

  for (Vertex vertex = 1; vertex < vertexCount; ++vertex) {
    std::uniform_int_distribution<Vertex> parent(0, vertex - 1);
    addEdge(vertex, parent(engine));
  }
  while (edges.size() < edgeCount) {
    const Vertex u = vertices(engine);
    const Vertex v = vertices(engine);
    if (u != v) {
      addEdge(u, v);
    }
  }

  std::vector<std::size_t> degrees(vertexCount, 0);
  for (const auto &edge : edges) {
    ++degrees[edge.u];
    ++degrees[edge.v];
  }

  Graph graph(vertexCount);
  for (Vertex vertex = 0; vertex < vertexCount; ++vertex) {
    graph.reserve(vertex, degrees[vertex]);
  }
  for (const auto &edge : edges) {
    graph.addUndirectedEdge(edge.u, edge.v, edge.weight);
  }
  return graph;
}

bool validateGraph(const Graph &graph) {
  if (graph.vertexCount() == 0 || graph.edgeCount() < graph.vertexCount() - 1) {
    return false;
  }
  std::vector<bool> visited(graph.vertexCount(), false);
  std::vector<Vertex> stack{0};
  visited[0] = true;
  std::size_t seen = 0;
  while (!stack.empty()) {
    const Vertex u = stack.back();
    stack.pop_back();
    ++seen;
    for (const auto &edge : graph.neighbors(u)) {
      if (edge.to == u || !(edge.weight > 0.0 && edge.weight <= 1.0)) {
        return false;
      }
      if (!visited[edge.to]) {
        visited[edge.to] = true;
        stack.push_back(edge.to);
      }
    }
  }
  return seen == graph.vertexCount();
}

} // namespace prim
