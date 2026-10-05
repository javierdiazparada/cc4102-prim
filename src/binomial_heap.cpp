#include "binomial_heap.hpp"

#include <cmath>
#include <limits>
#include <stdexcept>

namespace prim {

void BinomialHeap::link(Node *child, Node *parent) {
  child->parent = parent;
  child->sibling = parent->child;
  parent->child = child;
  ++parent->degree;
}

BinomialHeap::Node *BinomialHeap::mergeRootLists(Node *first, Node *second) {
  Node dummy;
  Node *tail = &dummy;
  while (first != nullptr && second != nullptr) {
    Node *&chosen = first->degree <= second->degree ? first : second;
    tail->sibling = chosen;
    chosen = chosen->sibling;
    tail = tail->sibling;
  }
  tail->sibling = first != nullptr ? first : second;
  return dummy.sibling;
}

void BinomialHeap::unite(Node *other) {
  head_ = mergeRootLists(head_, other);
  if (head_ == nullptr) {
    return;
  }
  Node *previous = nullptr;
  Node *current = head_;
  Node *next = current->sibling;
  while (next != nullptr) {
    if (current->degree != next->degree ||
        (next->sibling != nullptr && next->sibling->degree == current->degree)) {
      previous = current;
      current = next;
    } else if (current->key <= next->key) {
      current->sibling = next->sibling;
      link(next, current);
    } else {
      if (previous == nullptr) {
        head_ = next;
      } else {
        previous->sibling = next;
      }
      link(current, next);
      current = next;
    }
    next = current->sibling;
  }
}

void BinomialHeap::build(const std::vector<double> &keys) {
  nodes_.clear();
  nodes_.resize(keys.size());
  handles_.assign(keys.size(), nullptr);
  head_ = nullptr;
  size_ = 0;
  for (std::size_t i = 0; i < keys.size(); ++i) {
    Node &node = nodes_[i];
    node.key = keys[i];
    node.vertex = static_cast<Vertex>(i);
    node.degree = 0;
    node.parent = node.child = node.sibling = nullptr;
    handles_[i] = &node;
    unite(&node);
    ++size_;
  }
}

bool BinomialHeap::empty() const { return size_ == 0; }

QueueEntry BinomialHeap::extractMin() {
  if (head_ == nullptr) {
    throw std::runtime_error("extractMin sobre heap binomial vacio");
  }
  Node *minimum = head_;
  Node *minimumPrevious = nullptr;
  Node *previous = nullptr;
  for (Node *node = head_; node != nullptr; node = node->sibling) {
    if (node->key < minimum->key) {
      minimum = node;
      minimumPrevious = previous;
    }
    previous = node;
  }
  if (minimumPrevious == nullptr) {
    head_ = minimum->sibling;
  } else {
    minimumPrevious->sibling = minimum->sibling;
  }

  Node *reversed = nullptr;
  Node *child = minimum->child;
  while (child != nullptr) {
    Node *next = child->sibling;
    child->parent = nullptr;
    child->sibling = reversed;
    reversed = child;
    child = next;
  }
  unite(reversed);
  --size_;
  handles_[minimum->vertex] = nullptr;
  return {minimum->key, minimum->vertex};
}

std::uint64_t BinomialHeap::decreaseKey(Vertex vertex, double newKey) {
  if (vertex >= handles_.size() || handles_[vertex] == nullptr) {
    throw std::runtime_error("referencia invalida en heap binomial");
  }
  Node *node = handles_[vertex];
  if (newKey > node->key) {
    throw std::runtime_error("decreaseKey no puede aumentar una llave");
  }
  node->key = newKey;
  std::uint64_t swaps = 0;
  while (node->parent != nullptr && node->key < node->parent->key) {
    Node *parent = node->parent;
    std::swap(node->key, parent->key);
    std::swap(node->vertex, parent->vertex);
    handles_[node->vertex] = node;
    handles_[parent->vertex] = parent;
    node = parent;
    ++swaps;
  }
  return swaps;
}

bool BinomialHeap::validate() const {
  std::size_t previousDegree = 0;
  bool first = true;
  for (Node *root = head_; root != nullptr; root = root->sibling) {
    if (root->parent != nullptr || (!first && root->degree <= previousDegree)) {
      return false;
    }
    previousDegree = root->degree;
    first = false;
  }
  for (std::size_t vertex = 0; vertex < handles_.size(); ++vertex) {
    if (handles_[vertex] != nullptr && handles_[vertex]->vertex != vertex) {
      return false;
    }
  }
  return true;
}

std::size_t BinomialHeap::nodeSize() { return sizeof(Node); }

} // namespace prim
