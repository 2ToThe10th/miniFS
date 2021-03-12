//
// Created by 2ToThe10th on 12.03.2021.
//

#ifndef MINIFS_SRC_I_NODE_H_
#define MINIFS_SRC_I_NODE_H_

#include <stdint.h>
#include "i_node_data.h"
#include "constants.h"

#define I_NODE_FILES_NUMBER ((BLOCK_SIZE - 4 * 8) / 32)

struct INode {
  uint64_t next_inode;
  uint64_t empty[3];
  struct INodeData files[I_NODE_FILES_NUMBER];
};

uint64_t CreateEmptyINode(int filesystem_fd);

#endif //MINIFS_SRC_I_NODE_H_
