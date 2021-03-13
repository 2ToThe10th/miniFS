//
// Created by 2ToThe10th on 13.03.2021.
//

#include "util.h"
void read_from_filesystem(int filesystem_fd, uint64_t offset_in_filesystem, void *read_to, uint64_t bytes_to_read) {
  lseek(filesystem_fd, offset_in_filesystem, SEEK_SET);
  read(filesystem_fd, read_to, bytes_to_read);
}
void write_to_filesystem(int filesystem_fd, uint64_t offset_in_filesystem, void *write_from, uint64_t bytes_to_write) {
  lseek(filesystem_fd, offset_in_filesystem, SEEK_SET);
  write(filesystem_fd, write_from, bytes_to_write);
}
