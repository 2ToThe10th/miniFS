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
  uint64_t size;
  struct INode* next_inode;
  uint64_t empty[2];
  struct INodeData files[I_NODE_FILES_NUMBER];
};

#endif //MINIFS_SRC_I_NODE_H_
