#pragma once

#include "types.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace prim {

class Graph {
public:
  /**
   * @brief Crea una lista de adyacencia vacia.
   * @param vertexCount Cantidad de vertices del grafo.
   */
  explicit Graph(std::size_t vertexCount = 0);

  /** @return Cantidad de vertices del grafo. */
  [[nodiscard]] std::size_t vertexCount() const;
  /** @return Cantidad de aristas no dirigidas. */
  [[nodiscard]] std::size_t edgeCount() const;
  /**
   * @param vertex Identificador del vertice.
   * @return Lista de vecinos del vertice.
   */
  [[nodiscard]] const std::vector<AdjacentEdge> &neighbors(Vertex vertex) const;
  /**
   * @brief Reserva espacio para la lista de un vertice.
   * @param vertex Identificador del vertice.
   * @param degree Grado esperado.
   */
  void reserve(Vertex vertex, std::size_t degree);
  /**
   * @brief Agrega una arista de peso positivo en ambos sentidos.
   * @param u Primer extremo.
   * @param v Segundo extremo.
   * @param weight Peso en el rango (0,1].
   */
  void addUndirectedEdge(Vertex u, Vertex v, double weight);

private:
  std::vector<std::vector<AdjacentEdge>> adjacency_;
  std::size_t edgeCount_ = 0;
};

class GraphGenerator {
public:
  explicit GraphGenerator(std::uint64_t seed);

  /**
   * @brief Genera un grafo simple y conexo con exactamente m aristas.
   * @param vertexCount Cantidad de vertices.
   * @param edgeCount Cantidad de aristas no dirigidas.
   * @return Grafo por listas de adyacencia con pesos en (0,1].
   * @note Se agrega primero un arbol para garantizar conectividad y luego se
   * completan las aristas restantes descartando repeticiones y lazos.
   */
  Graph generate(std::size_t vertexCount, std::size_t edgeCount);

private:
  std::uint64_t seed_;
};

[[nodiscard]] bool validateGraph(const Graph &graph);

} // namespace prim
