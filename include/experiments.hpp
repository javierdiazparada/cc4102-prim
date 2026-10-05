#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

namespace prim {

/**
 * @brief Ejecuta las series A-D con diez grafos por configuracion.
 * @param resultsDirectory Directorio de salida para raw_results.csv.
 * @param baseSeed Base para derivar una semilla reproducible por grafo.
 * @note La grilla de configuraciones corresponde a las series solicitadas.
 */
void runRequiredExperiments(const std::filesystem::path &resultsDirectory,
                            std::uint64_t baseSeed);

/**
 * @brief Imprime la estimacion analitica de memoria pedida.
 */
void printMemoryEstimate();

/**
 * @brief Repite una medicion individual y la escribe en un CSV.
 * @param outputCsv Archivo de salida.
 * @param series Serie experimental de la configuracion.
 * @param vertexExponent Exponente i para la cantidad de vertices.
 * @param edgeExponent Exponente j para la cantidad de aristas.
 * @param repetition Numero de repeticion que determina la semilla.
 * @param heapName Heap que se medira.
 * @param baseSeed Semilla base usada por los experimentos.
 */
void rerunMeasurement(const std::filesystem::path &outputCsv,
                      const std::string &series, int vertexExponent,
                      int edgeExponent, int repetition,
                      const std::string &heapName, std::uint64_t baseSeed);

} // namespace prim
