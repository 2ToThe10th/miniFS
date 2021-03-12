//
// Created by 2ToThe10th on 12.03.2021.
//

#ifndef MINIFS_SRC_INIT_FILE_SYSTEM_H_
#define MINIFS_SRC_INIT_FILE_SYSTEM_H_

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "superblock.h"
#include "constants.h"

void InitFileSystem(int filesystem_fd, uint64_t filesystem_size);

#endif //MINIFS_SRC_INIT_FILE_SYSTEM_H_
