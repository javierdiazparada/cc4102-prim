# CC4102 - Tarea 1: Prim con colas binomiales y de Fibonacci

Implementación en C++20 del algoritmo de Prim usando dos colas de
prioridad propias. El proyecto genera los grafos, ejecuta las cuatro series
pedidas, verifica que ambos MST tengan el mismo peso y produce los doce
gráficos requeridos.

## Requisitos

- CMake 3.16 o superior.
- Compilador con C++20.
- Python y Matplotlib solamente para los gráficos. Con `uv` no es necesario
  instalar paquetes globales.

## Compilar y probar

```powershell
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

En Windows el ejecutable queda en `build/Release/prim_mst.exe`.

## Ejecutar la tarea completa

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts\system_info.ps1 `
  -Out results\system_info.txt
powershell -NoProfile -ExecutionPolicy Bypass -File scripts\run_experiments.ps1
```

La corrida completa usa los tamaños grandes del enunciado y puede tardar
bastante. No usa paralelismo para no contaminar las mediciones.

También puede ejecutarse manualmente:

```powershell
.\build\Release\prim_mst.exe --experiments --results-dir results --seed 4102
uv run --with matplotlib python scripts\plot_results.py
```

Estimación de memoria solicitada:

```powershell
.\build\Release\prim_mst.exe --memory
```

## Salidas

- `results/raw_results.csv`: las 10 repeticiones de cada configuración.
- `results/summary.csv`: promedios usados en la tabla y gráficos.
- `results/fit_constants.csv`: constantes usadas para escalar las cotas.
- `results/memory_estimate.txt`: cálculo basado en `sizeof`.
- `results/system_info.txt`: CPU, RAM, caché y sistema operativo.
- `plots/`: cuatro gráficos de costo total y ocho de costo amortizado.

## Decisiones simples de implementación

- El grafo usa listas de adyacencia y pesos uniformes en `(0,1]`.
- Se construye primero un árbol aleatorio y luego se agregan aristas simples
  hasta llegar exactamente a `e`.
- Los nodos de ambas colas se guardan en vectores contiguos. Los enlaces siguen
  siendo punteros, pero se evita hacer millones de asignaciones individuales.
- La cola binomial intercambia `(costo, vértice)` en `decreaseKey` y actualiza
  los dos handles afectados.
- La cola de Fibonacci mantiene listas circulares, marcas y cortes en cascada.
- La generación del grafo no forma parte del tiempo medido.
- En A/B no se cronometra cada `decreaseKey`; en C/D sí se acumula ese tiempo.
- Cada repetición alterna cuál cola se ejecuta primero.

## Correspondencia con el enunciado

| Requisito | Archivo |
|---|---|
| Generador conexo y simple | `include/graph.hpp`, `src/graph.cpp` |
| Cola binomial | `include/binomial_heap.hpp`, `src/binomial_heap.cpp` |
| Cola de Fibonacci | `include/fibonacci_heap.hpp`, `src/fibonacci_heap.cpp` |
| Prim con ambas colas | `src/prim.cpp` |
| Experimentos A-D y verificación MST | `src/experiments.cpp` |
| Doce gráficos y promedios | `scripts/plot_results.py` |
| Información del sistema | `scripts/system_info.ps1` |
| Pruebas | `tests/test_main.cpp` |
| Informe | `Algoritmo de Prim.pdf` |

## Procedencia de código y procesos

- El patrón de semilla reproducible y medición monotónica proviene del trabajo
  previo `Splay-tree`. Esto proviene de la tarea 1 del semestre de Otoño del 2026 del cuál también fui parte.
- La estructura del proyecto, pruebas y captura de sistema siguen el trabajo
  previo `R-tree`. Esto proviene de la tarea 1 del semestre de Otoño del 2026 del cuál también fui parte.
- El pequeño oráculo Kruskal de las pruebas adapta el proceso de Union-Find de
  `Kruskral`; no se usa en los experimentos. Este procedimiento fue sacado de la tarea del Semestre de Otoño del 2025 del cual también fui parte.
- El método de generación del grafo proviene directamente del enunciado.

Las colas binomial y de Fibonacci son implementaciones basadas en el
pseudocódigo del enunciado. No se usa `std::priority_queue` ni una biblioteca de
grafos.
