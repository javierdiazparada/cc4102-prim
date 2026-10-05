#include "experiments.hpp"

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

/**
 * @brief Imprime las formas de uso disponibles.
 */
void usage() {
  std::cout << "Uso:\n"
            << "  prim_mst --experiments [--results-dir DIR] [--seed N]\n"
            << "  prim_mst --retry --series C --i N --j N --repetition N "
               "--heap binomial|fibonacci --out FILE [--seed N]\n"
            << "  prim_mst --memory\n";
}

/**
 * @brief Obtiene el valor que sigue a una opcion.
 * @param argc Cantidad de argumentos.
 * @param argv Argumentos recibidos.
 * @param name Nombre de la opcion.
 * @param fallback Valor utilizado cuando la opcion no esta presente.
 * @return Valor asociado a la opcion.
 */
std::string optionalOption(int argc, char **argv, const std::string &name,
                           const std::string &fallback) {
  for (int i = 1; i < argc; ++i) {
    if (argv[i] == name) {
      if (i + 1 >= argc) {
        throw std::runtime_error("falta el valor para " + name);
      }
      return argv[i + 1];
    }
  }
  return fallback;
}

/**
 * @brief Revisa si una opcion esta presente.
 * @return true si la opcion fue entregada.
 */
bool hasOption(int argc, char **argv, const std::string &name) {
  for (int i = 1; i < argc; ++i) {
    if (argv[i] == name) {
      return true;
    }
  }
  return false;
}

} // namespace

int main(int argc, char **argv) {
  try {
    if (argc < 2 || hasOption(argc, argv, "--help")) {
      usage();
      return argc < 2 ? 1 : 0;
    }
    if (hasOption(argc, argv, "--memory")) {
      prim::printMemoryEstimate();
      return 0;
    }
    if (hasOption(argc, argv, "--experiments")) {
      const auto directory =
          optionalOption(argc, argv, "--results-dir", "results");
      const std::uint64_t seed =
          std::stoull(optionalOption(argc, argv, "--seed", "4102"));
      prim::runRequiredExperiments(directory, seed);
      return 0;
    }
    if (hasOption(argc, argv, "--retry")) {
      prim::rerunMeasurement(
          optionalOption(argc, argv, "--out", "retry.csv"),
          optionalOption(argc, argv, "--series", ""),
          std::stoi(optionalOption(argc, argv, "--i", "-1")),
          std::stoi(optionalOption(argc, argv, "--j", "-1")),
          std::stoi(optionalOption(argc, argv, "--repetition", "-1")),
          optionalOption(argc, argv, "--heap", ""),
          std::stoull(optionalOption(argc, argv, "--seed", "4102")));
      return 0;
    }
    usage();
    return 1;
  } catch (const std::exception &error) {
    std::cerr << "error: " << error.what() << '\n';
    return 2;
  }
}
