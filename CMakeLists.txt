project(mycc)
cmake_minimum_required(VERSION 3.1)

find_package(FLEX)
find_package(BISON)

FLEX_TARGET(src/с_preprocessor.l)

FLEX_TARGET(src/c_tokenizer.l)
BISON_TARGET(src/c_parser.l)
