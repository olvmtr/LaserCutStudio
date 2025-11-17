# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/appLaserCutStudio_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/appLaserCutStudio_autogen.dir/ParseCache.txt"
  "appLaserCutStudio_autogen"
  )
endif()
