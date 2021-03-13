//
// Created by 2ToThe10th on 12.03.2021.
//

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "parse_path.h"
#include "constants.h"
#include "util.h"

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
    int result = FindInDirectory(current_position_in_path, filesystem_fd, offset, &i_node_data, NULL);
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

/**
 *
 * @param name
 * @param i_node
 * @return -1 if not found, index in i_node otherwise
 */
int FindInINode(char *name, struct INode *i_node);

int FindInDirectory(char *name,
                    int filesystem_fd,
                    uint64_t directory_offset,
                    struct INodeData *return_info,
                    uint64_t *offset) {
  int current_node_offset = directory_offset;
  do {
    struct INode current_node;
    read_from_filesystem(filesystem_fd, current_node_offset, &current_node, sizeof(current_node));
    int search_result_index;
    if ((search_result_index = FindInINode(name, &current_node)) != -1) {
      if (return_info != NULL) {
        memcpy(return_info, &(current_node.files[search_result_index]), sizeof(struct INodeData));
      }
      if (offset != NULL) {
        *offset = current_node_offset + (search_result_index + 1) * sizeof(struct INodeData);
      }
      return 0;
    }
    current_node_offset = current_node.next_inode;
  } while (current_node_offset != 0);
  return -1;
}

int FindInINode(char *name, struct INode *i_node) {
  for (int i = 0; i < I_NODE_FILES_NUMBER; ++i) {
    if (i_node->files[i].location != 0) {
      char file_name[24];
      file_name[MAX_NAME_SIZE] = '\0';
      memcpy(file_name, i_node->files[i].name, MAX_NAME_SIZE);
      if (strcmp(name, file_name) == 0) {
        return i;
      }
    }
  }
  return -1;
}
