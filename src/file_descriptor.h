//
// Created by 2ToThe10th on 14.03.2021.
//

#ifndef MINIFS_SRC_FILE_DESCRIPTOR_H_
#define MINIFS_SRC_FILE_DESCRIPTOR_H_

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

struct FileDescriptor {
  uint64_t file_offset;
  uint64_t current_position;
  struct FileDescriptor *next_file_descriptor;
};

int AddFileDescriptorToList(struct FileDescriptor **head, uint64_t file_offset, int filesystem_fd);

void ClearListOfFileDescriptor(struct FileDescriptor *head, int filesystem_fd);

void CloseFileDescriptor(struct FileDescriptor *fd, int filesystem_fd);

struct FileDescriptor* GetFileDescriptorByIndex(struct FileDescriptor* head, int index);

#endif //MINIFS_SRC_FILE_DESCRIPTOR_H_
