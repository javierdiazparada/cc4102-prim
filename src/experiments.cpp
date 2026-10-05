#include "experiments.hpp"

#include "binomial_heap.hpp"
#include "fibonacci_heap.hpp"
#include "graph.hpp"
#include "prim.hpp"

#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace prim {

namespace {

struct Configuration {
  std::string series;
  int vertexExponent;
  int edgeExponent;
  bool amortized;
};

std::vector<Configuration> requiredConfigurations() {
  std::vector<Configuration> values;
  for (int j = 20; j <= 24; ++j) {
    values.push_back({"A", 20, j, false});
  }
  for (int i = 18; i <= 22; ++i) {
    values.push_back({"B", i, 24, false});
  }
  for (int j = 18; j <= 22; ++j) {
    values.push_back({"C", 18, j, true});
  }
  for (int i = 14; i <= 18; ++i) {
    values.push_back({"D", i, 22, true});
  }
  return values;
}

template <typename Function>
PrimResult measureAndWrite(std::ofstream &csv,
                           const Configuration &configuration,
                           int repetition, const std::string &heapName,
                           Function function) {
  const auto begin = std::chrono::steady_clock::now();
  const PrimResult result = function();
  const auto end = std::chrono::steady_clock::now();
  const double totalMilliseconds =
      std::chrono::duration<double, std::milli>(end - begin).count();
  csv << configuration.series << ',' << configuration.vertexExponent << ','
      << configuration.edgeExponent << ',' << repetition << ',' << heapName
      << ',' << totalMilliseconds << ',' << result.totalWeight << ','
      << result.edgeCount << ',' << result.metrics.decreaseCalls << ','
      << result.metrics.decreaseNanoseconds << ','
      << result.metrics.structuralOperations << '\n';
  return result;
}

} // namespace

void runRequiredExperiments(const std::filesystem::path &resultsDirectory,
                            std::uint64_t baseSeed) {
  std::filesystem::create_directories(resultsDirectory);
  std::ofstream csv(resultsDirectory / "raw_results.csv");
  if (!csv) {
    throw std::runtime_error("no se pudo crear el CSV de resultados");
  }
  csv << std::setprecision(17);
  csv << "series,i,j,repetition,heap,total_ms,mst_weight,mst_edges,"
         "decrease_calls,decrease_ns,structural_operations\n";

  for (const Configuration &configuration : requiredConfigurations()) {
    const std::size_t vertices = std::size_t{1} << configuration.vertexExponent;
    const std::size_t edges = std::size_t{1} << configuration.edgeExponent;
    for (int repetition = 0; repetition < 10; ++repetition) {
      const std::uint64_t seed =
          baseSeed + static_cast<std::uint64_t>(configuration.series[0]) * 1000000ULL +
          static_cast<std::uint64_t>(configuration.vertexExponent) * 10000ULL +
          static_cast<std::uint64_t>(configuration.edgeExponent) * 100ULL +
          static_cast<std::uint64_t>(repetition);
      std::cout << "series=" << configuration.series << " i="
                << configuration.vertexExponent << " j="
                << configuration.edgeExponent << " repetition="
                << (repetition + 1) << "/10" << std::endl;
      Graph graph = GraphGenerator(seed).generate(vertices, edges);

      PrimResult binomial;
      PrimResult fibonacci;
      auto runBinomial = [&] { return primBinomial(graph, configuration.amortized); };
      auto runFibonacci = [&] { return primFibonacci(graph, configuration.amortized); };

      // Se alterna el orden para reducir el sesgo sistematico de cache.
      if (repetition % 2 == 0) {
        binomial = measureAndWrite(csv, configuration, repetition, "binomial",
                                   runBinomial);
        fibonacci = measureAndWrite(csv, configuration, repetition,
                                    "fibonacci", runFibonacci);
      } else {
        fibonacci = measureAndWrite(csv, configuration, repetition,
                                    "fibonacci", runFibonacci);
        binomial = measureAndWrite(csv, configuration, repetition, "binomial",
                                   runBinomial);
      }
      const double tolerance =
          1e-9 * std::max(1.0, std::abs(binomial.totalWeight));
      if (std::abs(binomial.totalWeight - fibonacci.totalWeight) > tolerance) {
        throw std::runtime_error("los pesos del MST difieren entre heaps");
      }
      csv.flush();
    }
  }
}

void printMemoryEstimate() {
  constexpr std::size_t vertices = std::size_t{1} << 15;
  constexpr std::size_t edges = std::size_t{1} << 20;
  const std::size_t adjacency =
      vertices * sizeof(std::vector<AdjacentEdge>) +
      2 * edges * sizeof(AdjacentEdge);
  const std::size_t arrays = vertices *
      (sizeof(double) + sizeof(Vertex) + sizeof(bool) + sizeof(void *));
  const std::size_t binomial = vertices * BinomialHeap::nodeSize();
  const std::size_t fibonacci = vertices * FibonacciHeap::nodeSize();
  auto mib = [](std::size_t bytes) { return bytes / (1024.0 * 1024.0); };
  std::cout << std::fixed << std::setprecision(2)
            << "adjacency_mib=" << mib(adjacency) << '\n'
            << "auxiliary_arrays_mib=" << mib(arrays) << '\n'
            << "binomial_nodes_mib=" << mib(binomial) << '\n'
            << "fibonacci_nodes_mib=" << mib(fibonacci) << '\n'
            << "binomial_total_mib=" << mib(adjacency + arrays + binomial) << '\n'
            << "fibonacci_total_mib=" << mib(adjacency + arrays + fibonacci) << '\n';
}

void rerunMeasurement(const std::filesystem::path &outputCsv,
                      const std::string &series, int vertexExponent,
                      int edgeExponent, int repetition,
                      const std::string &heapName, std::uint64_t baseSeed) {
  if (series.size() != 1 || (heapName != "binomial" && heapName != "fibonacci")) {
    throw std::runtime_error("parametros de repeticion invalidos");
  }
  const Configuration configuration{series, vertexExponent, edgeExponent,
                                    series == "C" || series == "D"};
  const std::uint64_t seed =
      baseSeed + static_cast<std::uint64_t>(series[0]) * 1000000ULL +
      static_cast<std::uint64_t>(vertexExponent) * 10000ULL +
      static_cast<std::uint64_t>(edgeExponent) * 100ULL +
      static_cast<std::uint64_t>(repetition);
  Graph graph = GraphGenerator(seed).generate(std::size_t{1} << vertexExponent,
                                               std::size_t{1} << edgeExponent);
  std::ofstream csv(outputCsv);
  if (!csv) {
    throw std::runtime_error("no se pudo crear el CSV de repeticion");
  }
  csv << std::setprecision(17)
      << "series,i,j,repetition,heap,total_ms,mst_weight,mst_edges,"
         "decrease_calls,decrease_ns,structural_operations\n";
  if (heapName == "binomial") {
    measureAndWrite(csv, configuration, repetition, heapName, [&] {
      return primBinomial(graph, configuration.amortized);
    });
  } else {
    measureAndWrite(csv, configuration, repetition, heapName, [&] {
      return primFibonacci(graph, configuration.amortized);
    });
  }
}

} // namespace prim
