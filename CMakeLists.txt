project(mycc)
cmake_minimum_required(VERSION 3.5)

find_package(FLEX)

function(add_sanitizers target)
	target_compile_options("${target}" PRIVATE "-g" "-fsanitize=address,undefined")
	# Propagate sanitizer options to linker for libraries by using PUBLIC,
	#   so that linker finds the requested libasan and libubsan (references
	#   are put at compialtion stage of a library).
	target_link_options("${target}" PUBLIC  "-fsanitize=address,undefined")
endfunction()

# FLEX_TARGET(preprocessor src/с_preprocessor.l)
FLEX_TARGET(cc_tokenizer "${CMAKE_CURRENT_LIST_DIR}/src/c_compiler.l" "${CMAKE_CURRENT_BINARY_DIR}/c_compiler.l.c")

add_library(datastructures STATIC "${CMAKE_CURRENT_LIST_DIR}/src/vector.c" "${CMAKE_CURRENT_LIST_DIR}/src/hashtable.c")
target_link_directories(datastructures INTERFACE "${CMAKE_CURRENT_LIST_DIR}/src")
add_sanitizers(datastructures)

add_executable(mycc "${CMAKE_CURRENT_LIST_DIR}/src/c_compiler.c" "${CMAKE_CURRENT_BINARY_DIR}/c_compiler.l.c")
target_include_directories(mycc PRIVATE "${CMAKE_CURRENT_LIST_DIR}/src")
target_link_libraries(mycc datastructures)
add_sanitizers(mycc)


add_subdirectory(tests)
