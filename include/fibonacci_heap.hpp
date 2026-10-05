#pragma once

#include "types.hpp"

#include <cstddef>
#include <vector>

namespace prim {

class FibonacciHeap {
public:
  FibonacciHeap() = default;

  /**
   * @brief Construye la lista de raices mediante inserciones sucesivas.
   * @param keys Costos iniciales indexados por vertice.
   */
  void build(const std::vector<double> &keys);
  /** @return true si la cola no contiene nodos. */
  [[nodiscard]] bool empty() const;
  /** @return Par costo-vertice minimo luego de consolidar las raices. */
  QueueEntry extractMin();
  /**
   * @brief Reduce el costo asociado a un vertice.
   * @return Cantidad de cortes realizados, incluidos los de cascada.
   */
  std::uint64_t decreaseKey(Vertex vertex, double newKey);
  /** @return true si los enlaces de raices y handles son consistentes. */
  [[nodiscard]] bool validate() const;
  /** @return Tamano de nodo usado en la estimacion de memoria. */
  [[nodiscard]] static std::size_t nodeSize();

private:
  struct Node {
    double key = 0.0;
    Vertex vertex = 0;
    std::size_t degree = 0;
    bool marked = false;
    Node *parent = nullptr;
    Node *child = nullptr;
    Node *left = this;
    Node *right = this;
  };

  static void detach(Node *node);
  static void insertBefore(Node *position, Node *node);
  void addRoot(Node *node);
  void link(Node *child, Node *parent);
  void consolidate();
  void cut(Node *node, Node *parent, std::uint64_t &cuts);
  void cascadingCut(Node *node, std::uint64_t &cuts);

  std::vector<Node> nodes_;
  std::vector<Node *> handles_;
  Node *minimum_ = nullptr;
  std::size_t size_ = 0;
};

} // namespace prim
