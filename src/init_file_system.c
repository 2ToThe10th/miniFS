//
// Created by 2ToThe10th on 12.03.2021.
//

#include <string.h>
#include "init_file_system.h"
#include "i_node.h"

void InitFileSystem(int filesystem_fd, uint64_t filesystem_size)  {
  if (filesystem_size < BLOCK_SIZE) {
    printf("Too small. File might be more than %d bytes", BLOCK_SIZE);
    close(filesystem_fd);
    exit(1);
  }
  printf("Init started\n");
  fflush(stdout);
  struct SuperBlock super_block;
  SuperBlockInit(&super_block);
  lseek(filesystem_fd, 0, SEEK_SET);
  write(filesystem_fd, &super_block, sizeof(struct SuperBlock));
  struct INode root_i_node;
  memset(&root_i_node, 0, sizeof(struct INode));
  lseek(filesystem_fd, BLOCK_SIZE, SEEK_SET);
  write(filesystem_fd, &root_i_node, sizeof(struct INode));
  uint64_t percent_of_init = filesystem_size / (BLOCK_SIZE * 100);
  for (uint64_t i = 2; i < filesystem_size / BLOCK_SIZE - 1; ++i) {
    lseek(filesystem_fd, i * BLOCK_SIZE, SEEK_SET);
    uint64_t next_empty_block = (i + 1) * BLOCK_SIZE;
    write(filesystem_fd, &next_empty_block, sizeof(next_empty_block));
    if (i % (5 * percent_of_init) == 0) {
      printf("Init ready on %ld%%\n", i / percent_of_init);
      fflush(stdout);
    }
  }
  lseek(filesystem_fd, (filesystem_size / BLOCK_SIZE - 1) * BLOCK_SIZE, SEEK_SET);
  uint64_t last_block = PLACE_FINISHED;
  write(filesystem_fd, &last_block, sizeof(last_block));
}
