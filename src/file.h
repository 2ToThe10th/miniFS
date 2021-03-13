//
// Created by 2ToThe10th on 13.03.2021.
//

#ifndef MINIFS_SRC_FILE_H_
#define MINIFS_SRC_FILE_H_

#include <stdint.h>
#include "constants.h"
#include "superblock.h"
#include "util.h"

#define NUMBER_OF_BLOCK_WITH_DATA_IN_FILE (((BLOCK_SIZE) / sizeof(uint64_t)) - 5)

struct File {
  uint64_t size;
  uint64_t file_descriptors_number;
  uint64_t offset_of_data[NUMBER_OF_BLOCK_WITH_DATA_IN_FILE];
  uint64_t indirect_address;
  uint64_t double_indirect_address;
  uint64_t triple_indirect_address;
};

uint64_t CreateEmptyFile(int filesystem_fd);

void RemoveFile(int filesystem_fd, uint64_t file_location);

#endif //MINIFS_SRC_FILE_H_
