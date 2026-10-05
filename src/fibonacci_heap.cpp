#include "fibonacci_heap.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace prim {

void FibonacciHeap::detach(Node *node) {
  node->left->right = node->right;
  node->right->left = node->left;
  node->left = node;
  node->right = node;
}

void FibonacciHeap::insertBefore(Node *position, Node *node) {
  node->left = position->left;
  node->right = position;
  position->left->right = node;
  position->left = node;
}

void FibonacciHeap::addRoot(Node *node) {
  node->parent = nullptr;
  if (minimum_ == nullptr) {
    node->left = node->right = node;
    minimum_ = node;
  } else {
    insertBefore(minimum_, node);
    if (node->key < minimum_->key) {
      minimum_ = node;
    }
  }
}

void FibonacciHeap::build(const std::vector<double> &keys) {
  nodes_.clear();
  nodes_.resize(keys.size());
  handles_.assign(keys.size(), nullptr);
  minimum_ = nullptr;
  size_ = 0;
  for (std::size_t i = 0; i < keys.size(); ++i) {
    Node &node = nodes_[i];
    node.key = keys[i];
    node.vertex = static_cast<Vertex>(i);
    node.degree = 0;
    node.marked = false;
    node.parent = node.child = nullptr;
    node.left = node.right = &node;
    handles_[i] = &node;
    addRoot(&node);
    ++size_;
  }
}

bool FibonacciHeap::empty() const { return size_ == 0; }

void FibonacciHeap::link(Node *child, Node *parent) {
  detach(child);
  child->parent = parent;
  child->marked = false;
  if (parent->child == nullptr) {
    parent->child = child;
  } else {
    insertBefore(parent->child, child);
  }
  ++parent->degree;
}

void FibonacciHeap::consolidate() {
  std::vector<Node *> roots;
  Node *start = minimum_;
  Node *current = start;
  do {
    roots.push_back(current);
    current = current->right;
  } while (current != start);

  std::vector<Node *> degrees(8, nullptr);
  for (Node *root : roots) {
    if (root->parent != nullptr) {
      continue;
    }
    Node *x = root;
    std::size_t degree = x->degree;
    while (true) {
      if (degree >= degrees.size()) {
        degrees.resize(degree + 2, nullptr);
      }
      Node *y = degrees[degree];
      if (y == nullptr) {
        break;
      }
      if (y->key < x->key) {
        std::swap(x, y);
      }
      link(y, x);
      degrees[degree] = nullptr;
      degree = x->degree;
    }
    degrees[degree] = x;
  }

  minimum_ = nullptr;
  for (Node *root : degrees) {
    if (root != nullptr) {
      root->left = root->right = root;
      addRoot(root);
    }
  }
}

QueueEntry FibonacciHeap::extractMin() {
  Node *removed = minimum_;
  if (removed == nullptr) {
    throw std::runtime_error("extractMin sobre heap de Fibonacci vacio");
  }

  std::vector<Node *> children;
  if (removed->child != nullptr) {
    Node *child = removed->child;
    Node *current = child;
    do {
      children.push_back(current);
      current = current->right;
    } while (current != child);
  }
  for (Node *child : children) {
    detach(child);
    child->parent = nullptr;
    child->marked = false;
    addRoot(child);
  }
  removed->child = nullptr;
  removed->degree = 0;

  if (removed->right == removed) {
    minimum_ = nullptr;
  } else {
    Node *next = removed->right;
    detach(removed);
    minimum_ = next;
    consolidate();
  }
  --size_;
  handles_[removed->vertex] = nullptr;
  return {removed->key, removed->vertex};
}

void FibonacciHeap::cut(Node *node, Node *parent, std::uint64_t &cuts) {
  if (parent->child == node) {
    parent->child = node->right == node ? nullptr : node->right;
  }
  detach(node);
  --parent->degree;
  node->parent = nullptr;
  node->marked = false;
  addRoot(node);
  ++cuts;
}

void FibonacciHeap::cascadingCut(Node *node, std::uint64_t &cuts) {
  Node *parent = node->parent;
  if (parent == nullptr) {
    return;
  }
  if (!node->marked) {
    node->marked = true;
  } else {
    cut(node, parent, cuts);
    cascadingCut(parent, cuts);
  }
}

std::uint64_t FibonacciHeap::decreaseKey(Vertex vertex, double newKey) {
  if (vertex >= handles_.size() || handles_[vertex] == nullptr) {
    throw std::runtime_error("referencia invalida en heap de Fibonacci");
  }
  Node *node = handles_[vertex];
  if (newKey > node->key) {
    throw std::runtime_error("decreaseKey no puede aumentar una llave");
  }
  node->key = newKey;
  std::uint64_t cuts = 0;
  Node *parent = node->parent;
  if (parent != nullptr && node->key < parent->key) {
    cut(node, parent, cuts);
    cascadingCut(parent, cuts);
  }
  if (minimum_ == nullptr || node->key < minimum_->key) {
    minimum_ = node;
  }
  return cuts;
}

bool FibonacciHeap::validate() const {
  if ((size_ == 0) != (minimum_ == nullptr)) {
    return false;
  }
  for (std::size_t vertex = 0; vertex < handles_.size(); ++vertex) {
    if (handles_[vertex] != nullptr && handles_[vertex]->vertex != vertex) {
      return false;
    }
  }
  if (minimum_ != nullptr) {
    Node *current = minimum_;
    do {
      if (current->parent != nullptr || current->key < minimum_->key) {
        return false;
      }
      current = current->right;
    } while (current != minimum_);
  }
  return true;
}

std::size_t FibonacciHeap::nodeSize() { return sizeof(Node); }

} // namespace prim
