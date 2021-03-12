//
// Created by 2ToThe10th on 12.03.2021.
//

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "parse_path.h"
#include "constants.h"

uint64_t ParsePath(char *path, int filesystem_fd) {
  uint64_t offset = BLOCK_SIZE;
  char *path_end = path + strlen(path);
  for (char *i = path; i < path_end; ++i) {
    if (*i == '/') {
      *i = '\0';
    }
  }
  char *current_position_in_path = path;
  while (current_position_in_path < path_end) {
    if (strlen(current_position_in_path) == 0) {
      ++current_position_in_path;
      continue;
    }
    struct INodeData i_node_data;
    int result = FindInDirectory(current_position_in_path, filesystem_fd, offset, &i_node_data);
    if (result == -1) {
      printf("No such directory: %s\n", current_position_in_path);
      return 0;
    }
    if (i_node_data.type != 'd') {
      printf("It is a file: %s\n", current_position_in_path);
      return 0;
    }
    offset = i_node_data.location;
    current_position_in_path += strlen(current_position_in_path) + 1;
  }
  return offset;
}

struct INodeData *FindInINode(char *name, struct INode *i_node);

int FindInDirectory(char *name, int filesystem_fd, uint64_t directory_offset, struct INodeData *return_info) {
  struct INode current_node;
  lseek(filesystem_fd, directory_offset, SEEK_SET);
  read(filesystem_fd, &current_node, sizeof(current_node));
  struct INodeData *search_result;
  if ((search_result = FindInINode(name, &current_node)) != NULL) {
    if (return_info != NULL) {
      memcpy(return_info, search_result, sizeof(struct INodeData));
    }
    return 0;
  }
  while (current_node.next_inode != 0) {
    lseek(filesystem_fd, current_node.next_inode, SEEK_SET);
    read(filesystem_fd, &current_node, sizeof(current_node));
    if ((search_result = FindInINode(name, &current_node)) != NULL) {
      if (return_info != NULL) {
        memcpy(return_info, search_result, sizeof(struct INodeData));
      }
      return 0;
    }
  }
  return -1;
}

struct INodeData *FindInINode(char *name, struct INode *i_node) {
  for (int i = 0; i < I_NODE_FILES_NUMBER; ++i) {
    if (i_node->files[i].location != 0) {
      char file_name[24];
      file_name[MAX_NAME_SIZE] = '\0';
      memcpy(file_name, i_node->files[i].name, MAX_NAME_SIZE);
      if (strcmp(name, file_name) == 0) {
        return &(i_node->files[i]);
      }
    }
  }
  return NULL;
}
