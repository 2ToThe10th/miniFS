//
// Created by 2ToThe10th on 13.03.2021.
//

#include <stdio.h>
#include <string.h>
#include "file.h"
#include "superblock.h"
#include "util.h"

void FreeAddressBlock(int filesystem_fd, uint64_t block_offset, int depth);
uint64_t AllocPlaceInBlockBlock(uint64_t already_alloc,
                                uint64_t need_to_alloc,
                                int filesystem_fd,
                                uint64_t blocks,
                                int depth);
uint64_t GetBlockOffsetByIndex(uint64_t index, struct File *file, int filesystem_fd);

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

uint64_t Max(uint64_t first, uint64_t second) {
  if (first > second) {
    return first;
  } else {
    return second;
  }
}

uint64_t Min(uint64_t first, uint64_t second) {
  if (first < second) {
    return first;
  } else {
    return second;
  }
}

uint64_t GetBlockDataIn(uint64_t data_offset) {
  if (data_offset == 0) {
    return 0;
  } else {
    return (data_offset - 1) / BLOCK_SIZE + 1;
  }
}

int AllocPlaceForFile(int filesystem_fd, struct File *file, uint64_t to_alloc) {
  uint64_t future_size = file->size + to_alloc;
  uint64_t need_block_num = GetBlockDataIn(future_size);
  uint64_t current_block_num = GetBlockDataIn(file->size);
  printf("%ld of %ld\n", current_block_num, need_block_num);
  if (current_block_num < NUMBER_OF_BLOCK_WITH_DATA_IN_FILE) {
    uint64_t alloc_block_to = Min(NUMBER_OF_BLOCK_WITH_DATA_IN_FILE, need_block_num);
    for (; current_block_num < alloc_block_to; ++current_block_num) {
      file->offset_of_data[current_block_num] = Alloc(filesystem_fd);
      if (file->offset_of_data[current_block_num] == 0) {
        file->size = BLOCK_SIZE * current_block_num;
        return -1;
      }
    }
  }
  current_block_num -= NUMBER_OF_BLOCK_WITH_DATA_IN_FILE;
  need_block_num -= NUMBER_OF_BLOCK_WITH_DATA_IN_FILE;
  if (current_block_num < BLOCK_POINTERS_IN_BLOCK && need_block_num >= 0) {
    uint64_t ret = AllocPlaceInBlockBlock(current_block_num,
                                          Min(need_block_num, NUMBER_OF_BLOCK_WITH_DATA_IN_FILE),
                                          filesystem_fd,
                                          file->indirect_address,
                                          1);
    if (ret != UINT64_MAX) {
      file->size = NUMBER_OF_BLOCK_WITH_DATA_IN_FILE + ret;
      return -1;
    }
  }
  current_block_num -= BLOCK_POINTERS_IN_BLOCK;
  need_block_num -= BLOCK_POINTERS_IN_BLOCK;
  if (current_block_num < BLOCK_POINTERS_IN_BLOCK * BLOCK_POINTERS_IN_BLOCK && need_block_num >= 0) {
    uint64_t ret = AllocPlaceInBlockBlock(current_block_num,
                                          Min(need_block_num, BLOCK_POINTERS_IN_BLOCK * BLOCK_POINTERS_IN_BLOCK),
                                          filesystem_fd,
                                          file->double_indirect_address,
                                          2);
    if (ret != UINT64_MAX) {
      file->size = NUMBER_OF_BLOCK_WITH_DATA_IN_FILE + BLOCK_POINTERS_IN_BLOCK + ret;
      return -1;
    }
  }
  current_block_num -= BLOCK_POINTERS_IN_BLOCK * BLOCK_POINTERS_IN_BLOCK;
  need_block_num -= BLOCK_POINTERS_IN_BLOCK * BLOCK_POINTERS_IN_BLOCK;
  if (current_block_num < BLOCK_POINTERS_IN_BLOCK * BLOCK_POINTERS_IN_BLOCK * NUMBER_OF_BLOCK_WITH_DATA_IN_FILE
      && need_block_num >= 0) {
    uint64_t ret = AllocPlaceInBlockBlock(current_block_num,
                                          Min(need_block_num,
                                              BLOCK_POINTERS_IN_BLOCK * BLOCK_POINTERS_IN_BLOCK
                                                  * NUMBER_OF_BLOCK_WITH_DATA_IN_FILE),
                                          filesystem_fd,
                                          file->triple_indirect_address,
                                          3);
    if (ret != UINT64_MAX) {
      file->size =
          NUMBER_OF_BLOCK_WITH_DATA_IN_FILE + BLOCK_POINTERS_IN_BLOCK
              + BLOCK_POINTERS_IN_BLOCK * BLOCK_POINTERS_IN_BLOCK
              + ret;
      return -1;
    }
  }
  file->size = future_size;
  return 0;
}

uint64_t AllocPlaceInBlockBlock(uint64_t already_alloc,
                                uint64_t need_to_alloc,
                                int filesystem_fd,
                                uint64_t blocks_offset,
                                int depth) {
  uint64_t blocks[BLOCK_POINTERS_IN_BLOCK];
  read_from_filesystem(filesystem_fd, blocks_offset, blocks, BLOCK_SIZE);
  uint64_t blocks_number = 1;
  for (int i = 1; i < depth; ++i) {
    blocks_number *= BLOCK_POINTERS_IN_BLOCK;
  }
  while (already_alloc < need_to_alloc) {
    if (blocks_number == 1) {
      blocks[already_alloc] = Alloc(filesystem_fd);
      ++already_alloc;
    } else {
      uint64_t already_alloc_blocks = already_alloc / blocks_number;
      if (already_alloc % blocks_number == 0) {
        blocks[already_alloc_blocks] = Alloc(filesystem_fd);
      }
      uint64_t ret = AllocPlaceInBlockBlock(already_alloc % blocks_number,
                                            Min(blocks_number, need_to_alloc - already_alloc_blocks * blocks_number),
                                            filesystem_fd,
                                            blocks[already_alloc_blocks],
                                            depth - 1);
      if (ret != UINT64_MAX) {
        write_to_filesystem(filesystem_fd, blocks_offset, blocks, BLOCK_SIZE);
        return already_alloc_blocks * blocks_number + ret;
      }
      already_alloc = Min((already_alloc_blocks + 1) * blocks_number, need_to_alloc);
    }
  }
  write_to_filesystem(filesystem_fd, blocks_offset, blocks, BLOCK_SIZE);
  return UINT64_MAX;
}

void WriteToFile(int filesystem_fd, struct File *file, uint64_t position, uint64_t size, char *buffer) {
  while (size > 0) {
    uint64_t block_offset = GetBlockOffsetByIndex(position / BLOCK_SIZE, file, filesystem_fd);
    printf("block_offset: %ld\n", block_offset); // TODO: delete
    if (block_offset == UINT64_MAX) {
      printf("Error while writing\n");
      return;
    }
    uint64_t to_write = Min(BLOCK_SIZE - (position % BLOCK_SIZE), size);
    write_to_filesystem(filesystem_fd, block_offset + position % BLOCK_SIZE, buffer, to_write);
    size -= to_write;
    position += to_write;
    buffer += to_write;
  }
}

void ReadFromFile(int filesystem_fd, struct File *file, uint64_t position, uint64_t to_read) {
  char buffer[BLOCK_SIZE];
  while (to_read > 0) {
    uint64_t block_offset = GetBlockOffsetByIndex(position / BLOCK_SIZE, file, filesystem_fd);
    printf("block_offset: %ld\n", block_offset); // TODO: delete
    if (block_offset == UINT64_MAX) {
      printf("Error while writing\n");
      return;
    }
    uint64_t to_read_from_this_block = Min(BLOCK_SIZE - (position % BLOCK_SIZE), to_read);
    read_from_filesystem(filesystem_fd, block_offset + position % BLOCK_SIZE, buffer, to_read_from_this_block);
    write(STDOUT_FILENO, buffer, to_read_from_this_block);
    to_read -= to_read_from_this_block;
    position += to_read_from_this_block;
  }
}

uint64_t GetBlockOffsetByIndex(uint64_t index, struct File *file, int filesystem_fd) {
  if (index < NUMBER_OF_BLOCK_WITH_DATA_IN_FILE) {
    return file->offset_of_data[index];
  }
  index -= NUMBER_OF_BLOCK_WITH_DATA_IN_FILE;
  if (index < BLOCK_POINTERS_IN_BLOCK) {
    uint64_t blocks[BLOCK_POINTERS_IN_BLOCK];
    read_from_filesystem(filesystem_fd, file->indirect_address, blocks, BLOCK_SIZE);
    return blocks[index];
  }
  index -= BLOCK_POINTERS_IN_BLOCK;
  if (index < BLOCK_POINTERS_IN_BLOCK * BLOCK_POINTERS_IN_BLOCK) {
    uint64_t blocks[BLOCK_POINTERS_IN_BLOCK];
    read_from_filesystem(filesystem_fd, file->double_indirect_address, blocks, BLOCK_SIZE);
    read_from_filesystem(filesystem_fd, blocks[index / BLOCK_POINTERS_IN_BLOCK], blocks, BLOCK_SIZE);
    return blocks[index % BLOCK_POINTERS_IN_BLOCK];
  }
  index -= BLOCK_POINTERS_IN_BLOCK * BLOCK_POINTERS_IN_BLOCK;
  if (index < BLOCK_POINTERS_IN_BLOCK * BLOCK_POINTERS_IN_BLOCK * BLOCK_POINTERS_IN_BLOCK) {
    uint64_t blocks[BLOCK_POINTERS_IN_BLOCK];
    read_from_filesystem(filesystem_fd, file->triple_indirect_address, blocks, BLOCK_SIZE);
    read_from_filesystem(filesystem_fd, blocks[index / BLOCK_POINTERS_IN_BLOCK], blocks, BLOCK_SIZE);
    read_from_filesystem(filesystem_fd,
                         blocks[index / (BLOCK_POINTERS_IN_BLOCK * BLOCK_POINTERS_IN_BLOCK)],
                         blocks,
                         BLOCK_SIZE);
    return blocks[index % (BLOCK_POINTERS_IN_BLOCK * BLOCK_POINTERS_IN_BLOCK)];
  }
  return UINT64_MAX;
}
