//
// Created by 2ToThe10th on 12.03.2021.
//

#ifndef MINIFS_SRC_I_NODE_DATA_H_
#define MINIFS_SRC_I_NODE_DATA_H_

#include <stdint.h>

struct INodeData {
  uint64_t size;
  char type;
  char name[23];
};

#endif //MINIFS_SRC_I_NODE_DATA_H_
