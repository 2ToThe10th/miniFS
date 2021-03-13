# miniFS

[![Build Status](https://travis-ci.com/2ToThe10th/miniFS.svg?branch=master)](https://travis-ci.com/2ToThe10th/miniFS)

write small file system

```
Usage:
./miniFS <name of file (better to be multiple of 4096, otherwise file_size % 4096 will be unused)> [init]
So first time or to rewrite a file you need to write:
./miniFS file_name init
And then just:
./miniFS file_name
Commands:
mkdir dir_name - create a directory (for dir1/dir2/dir3 dir1/dir2 must exist and dir3 must not exist) (length of directory name must be not more than 23 symbols)
rmdir dir_name - remove directory (directory must be empty and must exist)
ls dir_name - watch files and dirs in directory
touch file_name - create file (for dir1/dir2/file1 dir1/dir2 must exist and file1 must not exist)
open file_name - return file descriptor of open file
rm file_name - delete file with file_name (file must not have open file descriptors)
read FD number_of_bytes - read number_of_bytes from place set in file descriptor (0 - EOF). Return number_of_read bytes and text
write FD text - write data to file from place in FD. text cant contain \n. Return number of written bytes
writeln FD text - as write, but add \n in the end
seek FD new_place - change FD place to new_place. new_place might be not more than size of file (size of file use to write in the end)
stat file_name - size of file_name
statfd FD - stat of file in FD
close FD - close file descriptor (need to delete files)
```