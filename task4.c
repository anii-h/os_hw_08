#define _POSIX_C_SOURCE 200809L

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LINE_SIZE 1024

static void die(const char *what)
{
	perror(what);
	exit(EXIT_FAILURE);
}

int main(void)
{
	char line[LINE_SIZE];
	char entry[LINE_SIZE + 32];
	size_t len = 0;

	int fd = open("log.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd < 0)
		die("open");

	printf("Dear diary... ");
	fflush(stdout);

	while (len < sizeof line)
	{
		ssize_t n = read(STDIN_FILENO, &line[len], 1);
		if (n < 0)
			die("read");
		if (n == 0)
			break;
		if (line[len++] == '\n')
			break;
	}

	int total = sprintf(entry, "PID=%d: ", (int)getpid());
	memcpy(entry + total, line, len);
	total += len;
	if (len == 0 || line[len - 1] != '\n')
		entry[total++] = '\n';

	if (write(fd, entry, total) != total)
		die("write");

	/*
	 * Why does SEEK_CUR keep growing even with O_APPEND?
	 * O_APPEND doesn't get rid of the file offset. Before every write() the
	 * kernel moves the offset to the end of the file, writes there, and then
	 * moves the offset forward past what it wrote (just like a normal write).
	 * So after the write the offset sits at the new end of the file, and
	 * since every run adds another line, the number keeps getting bigger.
	 */
	off_t pos = lseek(fd, 0, SEEK_CUR);
	if (pos < 0)
		die("lseek");
	printf("Saved to log.txt! The file offset is now at byte %lld\n", (long long)pos);

	if (close(fd) < 0)
		die("close");
	return 0;
}

