#pragma once

#include "types.hpp"

#include <cstddef>
#include <vector>

namespace prim {

class BinomialHeap {
public:
  BinomialHeap() = default;

  /**
   * @brief Construye la cola mediante inserciones sucesivas.
   * @param keys Costos iniciales indexados por vertice.
   */
  void build(const std::vector<double> &keys);
  /** @return true si la cola no contiene nodos. */
  [[nodiscard]] bool empty() const;
  /** @return Par costo-vertice minimo, que se elimina de la cola. */
  QueueEntry extractMin();
  /**
   * @brief Reduce el costo asociado a un vertice.
   * @return Cantidad de intercambios de contenido realizados.
   */
  std::uint64_t decreaseKey(Vertex vertex, double newKey);
  /** @return true si los grados de raices y handles son consistentes. */
  [[nodiscard]] bool validate() const;
  /** @return Tamano de nodo usado en la estimacion de memoria. */
  [[nodiscard]] static std::size_t nodeSize();

private:
  struct Node {
    double key = 0.0;
    Vertex vertex = 0;
    std::size_t degree = 0;
    Node *parent = nullptr;
    Node *child = nullptr;
    Node *sibling = nullptr;
  };

  static void link(Node *child, Node *parent);
  static Node *mergeRootLists(Node *first, Node *second);
  void unite(Node *other);

  std::vector<Node> nodes_;
  std::vector<Node *> handles_;
  Node *head_ = nullptr;
  std::size_t size_ = 0;
};

} // namespace prim
