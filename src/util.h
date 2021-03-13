//
// Created by 2ToThe10th on 13.03.2021.
//

#ifndef MINIFS_SRC_UTIL_H_
#define MINIFS_SRC_UTIL_H_

#include <unistd.h>
#include <stdint.h>
void read_from_filesystem(int filesystem_fd, uint64_t offset_in_filesystem, void *read_to, uint64_t bytes_to_read);

void write_to_filesystem(int filesystem_fd, uint64_t offset_in_filesystem, void *write_from, uint64_t bytes_to_write);

#endif //MINIFS_SRC_UTIL_H_
