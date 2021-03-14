//
// Created by 2ToThe10th on 14.03.2021.
//

#include "file_descriptor.h"
#include "file.h"
#include "util.h"

void ClearListRecursive(struct FileDescriptor* current, int filesystem_fd);
void IncreaseDescriptorNumbers(uint64_t file_offset, int filesystem_fd);

int AddFileDescriptorToList(struct FileDescriptor **head, uint64_t file_offset, int filesystem_fd) {
  int index = 0;
  struct FileDescriptor* current = *head;
  do {
    if (index > 0) {
      current = current->next_file_descriptor;
    }
    if (current == NULL) {
      break;
    }
    if (current->file_offset == 0) {
      IncreaseDescriptorNumbers(file_offset, filesystem_fd);
      current->file_offset = file_offset;
      current->current_position = 0;
      return index;
    } else {
      ++index;
    }
  } while (current->next_file_descriptor != NULL);

  struct FileDescriptor* new_fd = (struct FileDescriptor *) malloc(sizeof(struct FileDescriptor));
  if (new_fd == NULL) {
    printf("failed to allocate memory\n");
    return -1;
  }
  if (index == 0) {
    *head = new_fd;
  } else {
    current->next_file_descriptor = new_fd;
  }
  IncreaseDescriptorNumbers(file_offset, filesystem_fd);
  new_fd->file_offset = file_offset;
  new_fd->current_position = 0;
  new_fd->next_file_descriptor = NULL;
  return index;
}

void IncreaseDescriptorNumbers(uint64_t file_offset, int filesystem_fd) {
  struct File file;
  read_from_filesystem(filesystem_fd, file_offset, &file, sizeof(file));
  file.file_descriptors_number += 1;
  write_to_filesystem(filesystem_fd, file_offset, &file, sizeof(file));
}

void ClearListOfFileDescriptor(struct FileDescriptor *head, int filesystem_fd) {
  if (head != NULL) {
    ClearListRecursive(head, filesystem_fd);
  }
}

void ClearListRecursive(struct FileDescriptor *current, int filesystem_fd) {
  if (current->next_file_descriptor != NULL) {
    ClearListRecursive(current->next_file_descriptor, filesystem_fd);
  }
  if (current->file_offset != 0) {
    CloseFileDescriptor(current, filesystem_fd);
  }
  free(current);
}

void CloseFileDescriptor(struct FileDescriptor *fd, int filesystem_fd) {
  if (fd->file_offset == 0) {
    printf("Incorrect descriptor\n");
    return;
  }
  struct File file;
  read_from_filesystem(filesystem_fd, fd->file_offset, &file, sizeof(file));
  file.file_descriptors_number -= 1;
  write_to_filesystem(filesystem_fd, fd->file_offset, &file, sizeof(file));
  fd->file_offset = 0;
  fd->current_position = 0;
}

struct FileDescriptor *GetFileDescriptorByIndex(struct FileDescriptor *head, unsigned index) {
  // index from 1
  for (int i = 1; i < index; ++i) {
    if (head == NULL) {
      return NULL;
    } else {
      head = head->next_file_descriptor;
    }
  }
  return head;
}
