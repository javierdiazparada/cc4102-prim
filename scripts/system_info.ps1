param([string]$Out = "results/system_info.txt")

# Guarda la informacion del sistema utilizada durante los experimentos.

$parent = Split-Path -Parent $Out
if ($parent) { New-Item -ItemType Directory -Force -Path $parent | Out-Null }

@(
  "date=$((Get-Date).ToString('o'))"
  "os=$((Get-CimInstance Win32_OperatingSystem).Caption)"
  "os_version=$((Get-CimInstance Win32_OperatingSystem).Version)"
  "ram_bytes=$((Get-CimInstance Win32_ComputerSystem).TotalPhysicalMemory)"
  "cpu=$((Get-CimInstance Win32_Processor).Name)"
  "cores=$((Get-CimInstance Win32_Processor).NumberOfCores)"
  "logical_processors=$((Get-CimInstance Win32_Processor).NumberOfLogicalProcessors)"
  "l2_cache_kb=$((Get-CimInstance Win32_Processor).L2CacheSize)"
  "l3_cache_kb=$((Get-CimInstance Win32_Processor).L3CacheSize)"
) | Set-Content -Path $Out -Encoding UTF8
