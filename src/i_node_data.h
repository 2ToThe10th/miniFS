//
// Created by 2ToThe10th on 12.03.2021.
//

#ifndef MINIFS_SRC_I_NODE_DATA_H_
#define MINIFS_SRC_I_NODE_DATA_H_

#include <stdint.h>

#define MAX_NAME_SIZE 23

struct INodeData {
  uint64_t location;
  char type;
  char name[MAX_NAME_SIZE];
};

#endif //MINIFS_SRC_I_NODE_DATA_H_
