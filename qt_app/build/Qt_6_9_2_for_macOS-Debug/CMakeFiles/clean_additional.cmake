# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/Debug_ToolV2_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/Debug_ToolV2_autogen.dir/ParseCache.txt"
  "Debug_ToolV2_autogen"
  )
endif()
