//
// Created by 2ToThe10th on 12.03.2021.
//

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "i_node.h"
#include "superblock.h"

uint64_t CreateEmptyINode(int filesystem_fd) {
  struct INode empty_i_node;
  memset(&empty_i_node, 0, sizeof(empty_i_node));
  struct SuperBlock super_block;
  lseek(filesystem_fd, 0, SEEK_SET);
  read(filesystem_fd, &super_block, sizeof(super_block));
  if (super_block.first_empty_block == PLACE_FINISHED) {
    printf("Place finished");
    return 0;
  }
  uint64_t i_node_place = super_block.first_empty_block;
  UpdateSuperBlock(&super_block, filesystem_fd);
  lseek(filesystem_fd, i_node_place, SEEK_SET);
  write(filesystem_fd, &empty_i_node, sizeof(empty_i_node));
  return i_node_place;
}
