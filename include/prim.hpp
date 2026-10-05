#pragma once

#include "graph.hpp"
#include "types.hpp"

namespace prim {

/**
 * @brief Calcula un MST con Prim y una cola binomial.
 * @param graph Grafo conexo de entrada.
 * @param measureDecreaseKey Indica si se cronometra cada decreaseKey.
 * @return Aristas, peso total y mediciones del MST.
 */
PrimResult primBinomial(const Graph &graph, bool measureDecreaseKey);

/**
 * @brief Calcula un MST con Prim y una cola de Fibonacci.
 * @param graph Grafo conexo de entrada.
 * @param measureDecreaseKey Indica si se cronometra cada decreaseKey.
 * @return Aristas, peso total y mediciones del MST.
 */
PrimResult primFibonacci(const Graph &graph, bool measureDecreaseKey);

} // namespace prim
