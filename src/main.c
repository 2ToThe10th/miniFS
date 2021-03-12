#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "superblock.h"
#include "constants.h"
#include "init_file_system.h"
#include "i_node_data.h"
#include "i_node.h"
#include "parse_path.h"

const char USAGE[] = "Usage:\n"
                     "./miniFS <name of file (better to be multiple of 4096, otherwise file_size %% 4096 will be unused)> [init]\n"
                     "So first time or to rewrite a file you need to write:\n"
                     "./miniFS file_name init\n"
                     "And then just:\n"
                     "./miniFS file_name\n";
const char COMMANDS[] = "Commands:\n"
                        "mkdir dir_name - create a directory (for dir1/dir2/dir3 dir1/dir2 must exist and dir3 must not exist) (length of directory name must be not more than 23 symbols)\n"
                        "rmdir dir_name - remove directory (directory must be empty and must exist)\n"
                        "ls dir_name - watch files and dirs in directory\n"
                        "touch file_name - create file (for dir1/dir2/file1 dir1/dir2 must exist and file1 must not exist)\n"
                        "open file_name - return file descriptor of open file\n"
                        "close FD - close file descriptor\n"
                        "rm file_name - delete file with file_name (file must not have open file descriptors)\n"
                        "read FD number_of_bytes - read number_of_bytes from place set in file descriptor (0 - EOF). "
                        "Return number_of_read bytes and text\n"
                        "write FD text - write data to file from place in FD. text cant contain \\n. Return number of written bytes\n"
                        "writeln FD text - as write, but add \\n in the end\n"
                        "seek FD new_place - change FD place to new_place. new_place might be less than size of file\n"
                        "stat file_name - size of file_name\n"
                        "statfd FD - stat of file in FD\n"
                        "close FD - close file descriptor (need to delete files)\n";

void RunCommand(char *input_line, size_t input_line_size, int filesystem_fd);

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf(USAGE);
    printf(COMMANDS);
    exit(1);
  }
  int filesystem_fd = open(argv[1], O_RDWR);
  if (filesystem_fd < 0) {
    perror("Filesystem file");
    exit(1);
  }
  struct stat filesystem_stat;
  fstat(filesystem_fd, &filesystem_stat);
  uint64_t filesystem_size = filesystem_stat.st_size;
  if (filesystem_size % BLOCK_SIZE != 0) {
    filesystem_size = (filesystem_size / BLOCK_SIZE) * BLOCK_SIZE;
    printf("Will be used only %ld bytes of file\n", filesystem_size);
    fflush(stdout);
  }
  if (argc >= 3 && strcmp(argv[2], "init") == 0) {
    InitFileSystem(filesystem_fd, filesystem_size);
  }
  printf(COMMANDS);
  printf("Write your commands:\nminiFS> ");
  fflush(stdout);
  char *input_line = NULL;
  size_t zero = 0;
  size_t input_line_size = 0;
  while ((input_line_size = getline(&input_line, &zero, stdin)) != -1) {
    if (input_line == NULL) {
      perror("Error in getline");
      exit(1);
    }
    if (input_line_size > 0 && input_line[input_line_size - 1] == '\n') {
      --input_line_size;
      input_line[input_line_size] = '\0';
    }
    RunCommand(input_line, input_line_size, filesystem_fd);
    free(input_line);
    input_line = NULL;
    zero = 0;
    printf("miniFS> ");
    fflush(stdout);
  }
  close(filesystem_fd);
}

void CreateDirectory(char *directory_path, int filesystem_fd);
void ListDirectory(char *directory_path, int filesystem_fd);

void RunCommand(char *input_line, size_t input_line_size, int filesystem_fd) {
  char *first_space = strchr(input_line, ' ');
  char command[input_line_size + 1];
  if (first_space == NULL) {
    strcpy(command, input_line);
    command[input_line_size] = '\0';
    input_line += input_line_size;
  } else {
    memcpy(command, input_line, first_space - input_line);
    command[first_space - input_line] = '\0';
    input_line += first_space - input_line + 1;
  }
  printf("Command:|%s|\n", command);
  printf("left:|%s|\n", input_line);
  if (strcmp(command, "mkdir") == 0) {
    CreateDirectory(input_line, filesystem_fd);
  } else if (strcmp(command, "ls") == 0) {
    ListDirectory(input_line, filesystem_fd);
  } else {
    printf("No such command\n");
  }
}

uint64_t FindDirectoryToCreate(char *directory_path, int filesystem_fd, char **directory_name);
int AddInfoAboutNewNode(uint64_t i_node_new_place, char *directory_name, struct INode *current_node);

void CreateDirectory(char *directory_path, int filesystem_fd) {
  char *directory_name = NULL;
  uint64_t directory_offset = FindDirectoryToCreate(directory_path, filesystem_fd, &directory_name);
  if (directory_offset == 0) {
    return;
  }
  if (strlen(directory_name) > MAX_NAME_SIZE) {
    printf("Too big dir name\n");
    return;
  }
  if (FindInDirectory(directory_name, filesystem_fd, directory_offset, NULL) == 0) {
    printf("File already exist\n");
    return;
  }
  uint64_t new_i_node_place = CreateEmptyINode(filesystem_fd);
  struct INode current_node;
  lseek(filesystem_fd, directory_offset, SEEK_SET);
  read(filesystem_fd, &current_node, sizeof(current_node));
  if (AddInfoAboutNewNode(new_i_node_place, directory_name, &current_node)) {
    lseek(filesystem_fd, directory_offset, SEEK_SET);
    write(filesystem_fd, &current_node, sizeof(current_node));
    return;
  }
  while (current_node.next_inode != 0) {
    uint64_t current_i_node_place = current_node.next_inode;
    lseek(filesystem_fd, current_i_node_place, SEEK_SET);
    read(filesystem_fd, &current_node, sizeof(current_node));
    if (AddInfoAboutNewNode(new_i_node_place, directory_name, &current_node)) {
      lseek(filesystem_fd, current_i_node_place, SEEK_SET);
      write(filesystem_fd, &current_node, sizeof(current_node));
      return;
    }
  }
  // TODO:
}

int AddInfoAboutNewNode(uint64_t i_node_new_place, char *directory_name, struct INode *current_node) {
  for (int i = 0; i < I_NODE_FILES_NUMBER; ++i) {
    if (current_node->files[i].location == 0) {
      current_node->files[i].location = i_node_new_place;
      current_node->files[i].type = 'd';
      char file_name[MAX_NAME_SIZE + 1];
      memset(file_name, 0, MAX_NAME_SIZE + 1);
      strcpy(file_name, directory_name);
      memcpy(current_node->files[i].name, file_name, MAX_NAME_SIZE);
      return 1;
    }
  }
  return 0;
}

void ListINode(struct INode *i_node);

void ListDirectory(char *directory_path, int filesystem_fd) {
  uint64_t directory_offset = ParsePath(directory_path, filesystem_fd);
  printf("%ld\n", directory_offset);
  if (directory_offset != 0) {
    struct INode current_node;
    lseek(filesystem_fd, directory_offset, SEEK_SET);
    read(filesystem_fd, &current_node, sizeof(current_node));
    ListINode(&current_node);
    while (current_node.next_inode != 0) {
      lseek(filesystem_fd, current_node.next_inode, SEEK_SET);
      read(filesystem_fd, &current_node, sizeof(current_node));
      ListINode(&current_node);
    }
  }
}

void ListINode(struct INode *i_node) {
  for (int i = 0; i < I_NODE_FILES_NUMBER; ++i) {
    if (i_node->files[i].location != 0) {
      char file_name[24];
      file_name[MAX_NAME_SIZE] = '\0';
      memcpy(file_name, i_node->files[i].name, MAX_NAME_SIZE);
      printf("%s\n", file_name);
    }
  }
}

uint64_t FindDirectoryToCreate(char *directory_path, int filesystem_fd, char **directory_name) {
  size_t directory_path_size = strlen(directory_path);
  if (directory_path[directory_path_size - 1] == '/') {
    directory_path[directory_path_size - 1] = '\0';
    --directory_path_size;
  }
  if (directory_path_size == 0) {
    printf("Please write directory name\n");
    return 0;
  }
  *directory_name = directory_path;
  for (int i = 0; i < directory_path_size; ++i) {
    if (directory_path[i] == '/') {
      *directory_name = directory_path + i + 1;
    }
  }
  if (strlen(*directory_name) == 0) {
    printf("Empty directory name\n");
    return 0;
  }
  if (*directory_name != directory_path) {
    *(*directory_name - 1) = '\0';
    return ParsePath(directory_path, filesystem_fd);
  } else {
    return BLOCK_SIZE;
  }
}
