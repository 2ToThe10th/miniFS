//
// Created by 2ToThe10th on 12.03.2021.
//

#include <string.h>
#include <stdio.h>
#include "i_node.h"
#include "superblock.h"
#include "util.h"

uint64_t CreateEmptyINode(int filesystem_fd) {
  uint64_t i_node_place = Alloc(filesystem_fd);
  if (i_node_place == 0) {
    printf("Place finished");
    return 0;
  }

  struct INode empty_i_node;
  memset(&empty_i_node, 0, sizeof(empty_i_node));
  write_to_filesystem(filesystem_fd, i_node_place, &empty_i_node, sizeof(empty_i_node));
  return i_node_place;
}
