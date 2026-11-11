# Homework 8: Files - Report
## Operating Systems
### Ani Hambardzumyan

All seven programs do their file I/O only with system calls: `open`, `read`,
`write`, `lseek`, `ftruncate` and `close`. If a system call fails, the program
prints the error with `perror` and exits.

## Reading paths from the user

Tasks 1, 3 and 7 ask for file paths. The `ask()` function prints the question
and then reads standard input with `read(0, &c, 1)`, one character at a time,
until Enter. The `'\n'` is not saved, so the name given to `open` has no
extra characters at the end.

## Task 1: Copy a file

The source is opened with `O_RDONLY` and the destination with
`O_WRONLY | O_CREAT | O_TRUNC` and mode `0644`. Then a loop reads up to 4096
bytes from the source and writes them to the destination. When `read` returns
0 we are at the end of the file and the loop stops. `write` can write fewer
bytes than asked, so an inner loop keeps writing until the whole chunk is
written. Every chunk's size is added to a counter, which is printed at the end.

## Task 2: Truncate a file

The program writes the 26 letters into `data.txt` and closes it. Then it opens
the file again with `O_RDWR` and gets the size with `lseek(fd, 0, SEEK_END)`.
`lseek` returns the new offset, and the offset at the end of the file is the
file size (26). `ftruncate(fd, 10)` cuts the file to 10 bytes, and another
`lseek` to the end shows the new size, 10. Finally it goes back to the start
with `lseek(fd, 0, SEEK_SET)`, reads what is left and prints `ABCDEFGHIJ`.

## Task 3: Print a file backwards

The size is found with `lseek(fd, 0, SEEK_END)`. A loop then goes from position
`size - 1` down to 0. At each position it calls `lseek` to move there, `read`s
1 byte and `write`s that byte to standard output. At the end it writes a
newline. For an empty file the size is 0, so the loop never runs and only the
newline is printed.

## Task 4: Append to a log

`log.txt` is opened with `O_WRONLY | O_CREAT | O_APPEND`. One line is read from
standard input with `read(0, ...)` one byte at a time, stopping after the
`'\n'`. The program builds `PID=<pid>: ` with `sprintf` and `getpid()`, adds the
line, and adds a `'\n'` if the line didn't end with one. After writing,
`lseek(fd, 0, SEEK_CUR)` returns the current offset.

**Why the offset keeps growing:** with `O_APPEND` the kernel moves the offset
to the end of the file before every write, then writes, then moves the offset
past the bytes it wrote. So after the write the offset is at the new end of the
file, which is the file size. Every run adds a line, so the number gets bigger
each time.

## Task 5: Sparse file

The program writes `START` (5 bytes). Then `lseek(fd, 1024 * 1024, SEEK_CUR)`
jumps 1 MiB forward without writing anything, and it writes `END` (3 bytes).
The apparent size is 5 + 1048576 + 3 = 1048584 bytes.

**Why `du` shows much less:** nothing was ever written in the 1 MiB that was
skipped, so the filesystem doesn't allocate any blocks for it. That part is a
"hole", and reading it just gives zeros. `ls -l` shows the apparent size, but
`du` only counts the blocks that really exist on disk: the one with `START`
and the one with `END`, about 8 KB. The program also prints this number using
`fstat` (`st_blocks * 512`).

## Task 6: Overwrite line 4

The program writes `1\n` to `10\n` into `numbers.txt`. Then it opens the file
again with `O_RDWR` and reads it one byte at a time, counting the bytes and the
`'\n'`s. Line 4 starts right after the 3rd newline (byte 6), and line 5 starts
right after the 4th newline (byte 8).

`"100\n"` is 4 bytes but `"4\n"` is only 2, so writing it in place would
overwrite `"5\n"`. So it uses approach a). It does an `lseek` to line 5 and
reads the rest of the file into a buffer. Then it does an `lseek` back to
line 4, writes `"100\n"`, and writes the buffer after it. This moves everything
after line 4 two bytes forward. At the end it seeks to 0 and prints the file.

## Task 7: Compare two files

Both files are opened and read in chunks of 4096 bytes. `read_chunk()` keeps
calling `read` until the chunk is full or the file ends, so the two files are
always compared at the same positions. The two chunks are compared byte by
byte, and `offset` remembers where the current chunk starts, so the program
can print the 0-based position of a byte.

- If a byte is different, it prints `Files differ at byte N` and exits with 1.
- If all bytes match but one file ended earlier, the difference is at the end
  of the shorter file. It prints that position and exits with 1.
- If both files end at the same place, it prints `Files are identical` and
  exits with 0.
