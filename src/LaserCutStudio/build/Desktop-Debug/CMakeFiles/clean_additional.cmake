# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/appLaserCutStudio_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/appLaserCutStudio_autogen.dir/ParseCache.txt"
  "appLaserCutStudio_autogen"
  "tests/CMakeFiles/LaserCutStudioTests_autogen.dir/AutogenUsed.txt"
  "tests/CMakeFiles/LaserCutStudioTests_autogen.dir/ParseCache.txt"
  "tests/LaserCutStudioTests_autogen"
  "tests/benchmarks/CMakeFiles/LaserCutStudioBenchmarks_autogen.dir/AutogenUsed.txt"
  "tests/benchmarks/CMakeFiles/LaserCutStudioBenchmarks_autogen.dir/ParseCache.txt"
  "tests/benchmarks/LaserCutStudioBenchmarks_autogen"
  )
endif()
