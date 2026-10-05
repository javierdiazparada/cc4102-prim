#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace prim {

/**
 * @brief Indice de un vertice dentro del grafo
 */
using Vertex = std::uint32_t;

/**
 * @brief Arista guardada en una lista de adyacencia
 */
struct AdjacentEdge {
  Vertex to = 0;       ///< Vertice vecino
  double weight = 0.0; ///< Peso de la arista
};

/**
 * @brief Arista no dirigida usada para representar el MST
 */
struct UndirectedEdge {
  Vertex u = 0;        ///< Primer extremo
  Vertex v = 0;        ///< Segundo extremo
  double weight = 0.0; ///< Peso de la arista
};

/**
 * @brief Entrada extraida desde una cola de prioridad
 */
struct QueueEntry {
  double key = 0.0; ///< Costo del vertice
  Vertex vertex = 0; ///< Vertice asociado
};

/**
 * @brief Mediciones internas de las operaciones decreaseKey
 */
struct PrimMetrics {
  std::uint64_t decreaseCalls = 0; ///< Cantidad de llamadas
  std::uint64_t decreaseNanoseconds = 0; ///< Tiempo acumulado
  std::uint64_t structuralOperations = 0; ///< Intercambios o cortes
};

/**
 * @brief Resultado de ejecutar el algoritmo de Prim
 */
struct PrimResult {
  double totalWeight = 0.0; ///< Peso total del MST
  std::size_t edgeCount = 0; ///< Cantidad de aristas del MST
  std::vector<UndirectedEdge> edges{}; ///< Aristas seleccionadas
  PrimMetrics metrics{}; ///< Mediciones de decreaseKey
};

} // namespace prim
