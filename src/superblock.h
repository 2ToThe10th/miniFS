//
// Created by 2ToThe10th on 12.03.2021.
//

#ifndef MINIFS_SRC_SUPERBLOCK_H_
#define MINIFS_SRC_SUPERBLOCK_H_

#include <stdint.h>

#define PLACE_FINISHED UINT64_MAX

struct SuperBlock {
  uint64_t first_empty_block;
};

void SuperBlockInit(struct SuperBlock* super_block);

void UpdateSuperBlock(struct SuperBlock* super_block, int filesystem_fd);

#endif //MINIFS_SRC_SUPERBLOCK_H_
