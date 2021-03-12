//
// Created by 2ToThe10th on 12.03.2021.
//

#ifndef MINIFS_SRC_PARSE_PATH_H_
#define MINIFS_SRC_PARSE_PATH_H_

#include <stdint.h>
#include "i_node.h"

/**
 *
 * @param path
 * @param filesystem_fd
 * @return 0 if dont exist, location otherwise
 */
uint64_t ParsePath(char *path, int filesystem_fd);


/**
 *
 * @param name
 * @param filesystem_fd
 * @param directory_offset
 * @param return_info
 * @return 0 if exist, -1 if dont
 */
int FindInDirectory(char *name, int filesystem_fd, uint64_t directory_offset, struct INodeData *return_info);

#endif //MINIFS_SRC_PARSE_PATH_H_
