
set(CMAKE_CXX_STANDARD 11)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../common
                     ${CMAKE_SOURCE_DIR}/include
                     ${CMAKE_SOURCE_DIR}/src)

add_executable(WoplFile
               wopl_file.cpp
                ${libADLMIDI_SOURCE_DIR}/src/wopl/wopl_file.c
               $<TARGET_OBJECTS:Catch-objects>)

set_target_properties(WoplFile PROPERTIES COMPILE_DEFINITIONS "GSL_THROW_ON_CONTRACT_VIOLATION")
add_test(NAME WoplFileTest COMMAND WoplFile)

