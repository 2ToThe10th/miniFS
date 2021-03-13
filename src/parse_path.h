//
// Created by 2ToThe10th on 12.03.2021.
//

#ifndef MINIFS_SRC_PARSE_PATH_H_
#define MINIFS_SRC_PARSE_PATH_H_

#include <stdint.h>
#include "i_node.h"

/**
 *
 * @param directory_path
 * @param filesystem_fd
 * @return 0 if dont exist, location otherwise
 */
uint64_t ParsePathToDirectory(char *directory_path, int filesystem_fd);
uint64_t ParsePathToFile(char *file_path, int filesystem_fd);

/**
 *
 * @param name
 * @param filesystem_fd
 * @param directory_offset
 * @param return_info - return block i_node_data
 * @param offset - offset of block with i_node_data
 * @return 0 if exist, -1 if dont
 */
int FindInDirectory(char *name,
                    int filesystem_fd,
                    uint64_t directory_offset,
                    struct INodeData *return_info,
                    uint64_t *offset);

#endif //MINIFS_SRC_PARSE_PATH_H_
