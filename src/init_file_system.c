//
// Created by 2ToThe10th on 12.03.2021.
//

#include <string.h>
#include "init_file_system.h"
#include "i_node.h"
#include "util.h"

void InitFileSystem(int filesystem_fd, uint64_t filesystem_size) {
  if (filesystem_size < BLOCK_SIZE) {
    printf("Too small. File might be more than %d bytes", BLOCK_SIZE);
    close(filesystem_fd);
    exit(1);
  }
  printf("Init started\n");
  fflush(stdout);
  struct SuperBlock super_block;
  SuperBlockInit(&super_block);
  write_to_filesystem(filesystem_fd, 0, &super_block, sizeof(struct SuperBlock));
  struct INode root_i_node;
  memset(&root_i_node, 0, sizeof(struct INode));
  write_to_filesystem(filesystem_fd, BLOCK_SIZE, &root_i_node, sizeof(struct INode));
  uint64_t percent_of_init = filesystem_size / (BLOCK_SIZE * 100);
  for (uint64_t i = 2; i < filesystem_size / BLOCK_SIZE - 1; ++i) {
    uint64_t next_empty_block = (i + 1) * BLOCK_SIZE;
    write_to_filesystem(filesystem_fd, i * BLOCK_SIZE, &next_empty_block, sizeof(next_empty_block));
    if (i % (5 * percent_of_init) == 0) {
      printf("Init ready on %ld%%\n", i / percent_of_init);
      fflush(stdout);
    }
  }
  uint64_t last_block = PLACE_FINISHED;
  write_to_filesystem(filesystem_fd, (filesystem_size / BLOCK_SIZE - 1) * BLOCK_SIZE, &last_block, sizeof(last_block));
}
