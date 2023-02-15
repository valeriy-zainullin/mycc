find_package(FLEX REQUIRED)
FLEX_TARGET(TOKENIZER_SOURCE src/tokenizer.l ${CMAKE_CURRENT_BINARY_DIR}/tokenizer.cpp)

# Parser

# Tokenizer

set(TOKENIZER_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/tokenizer.cpp src/tokenizer_main.cpp)

add_executable(mycc-tokenizer         ${TOKENIZER_SOURCES})
add_executable(mycc-tokenizer.dbg     ${TOKENIZER_SOURCES})
add_executable(mycc-tokenizer.dbg.san ${TOKENIZER_SOURCES})

target_include_directories(mycc-tokenizer         PUBLIC include/mycc-parser)
target_include_directories(mycc-tokenizer.dbg     PUBLIC include/mycc-parser)
target_include_directories(mycc-tokenizer.dbg.san PUBLIC include/mycc-parser)

target_compile_options(mycc-tokenizer         PRIVATE ${RELEASE_COMPILE_OPTIONS})
target_compile_options(mycc-tokenizer.dbg     PRIVATE ${DEBUG_COMPILE_OPTIONS})
target_compile_options(mycc-tokenizer.dbg.san PRIVATE ${DEBUG_COMPILE_OPTIONS} ${SANITIZER_COMPILE_OPTIONS})
