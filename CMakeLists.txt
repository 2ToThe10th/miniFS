cmake_minimum_required(VERSION 2.8)
project(miniFS)

set(SOURCE_FILES src/main.c src/superblock.c src/superblock.h src/constants.h src/init_file_system.c src/init_file_system.h src/i_node.c src/i_node.h src/i_node_data.c src/i_node_data.h src/parse_path.c src/parse_path.h src/util.c src/util.h src/file.c src/file.h src/file_descriptor.c src/file_descriptor.h)

add_executable(miniFS ${SOURCE_FILES})