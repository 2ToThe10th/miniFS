//
// Created by 2ToThe10th on 12.03.2021.
//

#include <unistd.h>
#include "superblock.h"
#include "constants.h"
void SuperBlockInit(struct SuperBlock *super_block) {
  super_block->first_empty_block = 2 * BLOCK_SIZE;
}

void UpdateSuperBlock(struct SuperBlock *super_block, int filesystem_fd) {
  lseek(filesystem_fd, super_block->first_empty_block, SEEK_SET);
  uint64_t new_empty_block;
  read(filesystem_fd, &new_empty_block, sizeof(new_empty_block));
  super_block->first_empty_block = new_empty_block;
  lseek(filesystem_fd, 0, SEEK_SET);
  write(filesystem_fd, super_block, sizeof(struct SuperBlock));
}
