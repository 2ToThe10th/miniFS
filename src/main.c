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

const char USAGE[] = "Usage:\n"
                     "./miniFS <name of file (better to be multiple of 4096, otherwise file_size %% 4096 will be unused)> [init]\n"
                     "So first time or to rewrite a file you need to write:\n"
                     "./miniFS file_name init\n"
                     "And then just:\n"
                     "./miniFS file_name\n"
                     "Commands:\n"
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

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, USAGE);
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
    fprintf(stderr, "Will be used only %ld bytes of file\n", filesystem_size);
    fflush(stderr);
  }
  if (argc >= 3 && strcmp(argv[2], "init") == 0) {
    InitFileSystem(filesystem_fd, filesystem_size);
  }
}