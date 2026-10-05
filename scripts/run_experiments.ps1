param(
  [string]$BuildDir = "build",
  [string]$ResultsDir = "results",
  [string]$PlotsDir = "plots",
  [UInt64]$Seed = 4102
)

# Compila, verifica y ejecuta la bateria completa de experimentos de Prim.

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
$cmake = Get-Command cmake -ErrorAction SilentlyContinue
if (-not $cmake) {
  $candidate = "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
  if (Test-Path -LiteralPath $candidate) {
    $cmake = $candidate
  } else {
    throw "No se encontro CMake."
  }
}

& $cmake -S $root -B (Join-Path $root $BuildDir)
& $cmake --build (Join-Path $root $BuildDir) --config Release
& (Join-Path $root "$BuildDir\Release\prim_tests.exe")
& (Join-Path $root "$BuildDir\Release\prim_mst.exe") --memory |
  Set-Content (Join-Path $root "$ResultsDir\memory_estimate.txt")
& (Join-Path $root "$BuildDir\Release\prim_mst.exe") --experiments `
  --results-dir (Join-Path $root $ResultsDir) --seed $Seed
uv run --with matplotlib python (Join-Path $PSScriptRoot "plot_results.py") `
  --csv (Join-Path $root "$ResultsDir\raw_results.csv") `
  --summary (Join-Path $root "$ResultsDir\summary.csv") `
  --constants (Join-Path $root "$ResultsDir\fit_constants.csv") `
  --plots-dir (Join-Path $root $PlotsDir)
