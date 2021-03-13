//
// Created by 2ToThe10th on 13.03.2021.
//

#include <stdio.h>
#include <string.h>
#include "file.h"
#include "superblock.h"
#include "util.h"

void FreeAddressBlock(int filesystem_fd, uint64_t block_offset, int depth);

uint64_t CreateEmptyFile(int filesystem_fd) {
  uint64_t file_place = Alloc(filesystem_fd);
  if (file_place == 0) {
    printf("Place finished");
    return 0;
  }

  struct File empty_file;
  memset(&empty_file, 0, sizeof(empty_file));
  write_to_filesystem(filesystem_fd, file_place, &empty_file, sizeof(empty_file));
  return file_place;
}

void RemoveFile(int filesystem_fd, uint64_t file_location) {
  struct File file;
  read_from_filesystem(filesystem_fd, file_location, &file, sizeof(file));
  for (int i = 0; i < NUMBER_OF_BLOCK_WITH_DATA_IN_FILE; ++i) {
    if (file.offset_of_data[i] != 0) {
      Free(filesystem_fd, file.offset_of_data[i]);
    }
  }
  if (file.indirect_address != 0) {
    FreeAddressBlock(filesystem_fd, file.indirect_address, 1);
  }
  if (file.double_indirect_address != 0) {
    FreeAddressBlock(filesystem_fd, file.double_indirect_address, 2);
  }
  if (file.triple_indirect_address != 0) {
    FreeAddressBlock(filesystem_fd, file.triple_indirect_address, 3);
  }
  Free(filesystem_fd, file_location);
}

void FreeAddressBlock(int filesystem_fd, uint64_t block_offset, int depth) {
  uint64_t addresses[(BLOCK_SIZE / sizeof(uint64_t))];
  read_from_filesystem(filesystem_fd, block_offset, addresses, BLOCK_SIZE);
  if (depth == 1) {
    for (int i = 0; i < (BLOCK_SIZE / sizeof(uint64_t)); ++i) {
      if (addresses[i] != 0) {
        Free(filesystem_fd, addresses[i]);
      }
    }
  } else {
    for (int i = 0; i < (BLOCK_SIZE / sizeof(uint64_t)); ++i) {
      if (addresses[i] != 0) {
        FreeAddressBlock(filesystem_fd, addresses[i], depth - 1);
      }
    }
  }
  Free(filesystem_fd, block_offset);
}