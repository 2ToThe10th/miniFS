//
// Created by 2ToThe10th on 12.03.2021.
//

#include "superblock.h"
#include "constants.h"
void SuperBlockInit(struct SuperBlock *super_block) {
  super_block->first_empty_block = 2 * BLOCK_SIZE;
}
