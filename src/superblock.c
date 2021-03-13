//
// Created by 2ToThe10th on 12.03.2021.
//

#include "superblock.h"
#include "constants.h"
#include "util.h"

void SuperBlockInit(struct SuperBlock *super_block) {
  super_block->first_empty_block = 2 * BLOCK_SIZE;
}

uint64_t Alloc(int filesystem_fd) {
  struct SuperBlock super_block;
  read_from_filesystem(filesystem_fd, 0, &super_block, sizeof(super_block));
  uint64_t alloc_block = super_block.first_empty_block;
  if (alloc_block == PLACE_FINISHED) {
    return 0;
  }
  uint64_t new_first_empty_block;
  read_from_filesystem(filesystem_fd,
                       super_block.first_empty_block,
                       &new_first_empty_block,
                       sizeof(new_first_empty_block));
  super_block.first_empty_block = new_first_empty_block;
  write_to_filesystem(filesystem_fd, 0, &super_block, sizeof(struct SuperBlock));
  return alloc_block;
}

void Free(int filesystem_fd, uint64_t free_block_offset) {
  struct SuperBlock super_block;
  read_from_filesystem(filesystem_fd, 0, &super_block, sizeof(super_block));

  write_to_filesystem(filesystem_fd, free_block_offset, &super_block.first_empty_block, sizeof(uint64_t));
  super_block.first_empty_block = free_block_offset;

  write_to_filesystem(filesystem_fd, 0, &super_block, sizeof(super_block));
}
