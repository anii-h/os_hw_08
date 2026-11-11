#define _POSIX_C_SOURCE 200809L

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZE 4096

static void die(const char *what)
{
	perror(what);
	exit(EXIT_FAILURE);
}

int main(void)
{
	char buf[BUF_SIZE], rest[BUF_SIZE];
	ssize_t n = 0;
	char c;

	int fd = open("numbers.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
		die("open");
	for (int i = 1; i <= 10; i++)
	{
		int len = sprintf(buf, "%d\n", i);
		if (write(fd, buf, len) != len)
			die("write");
	}
	if (close(fd) < 0)
		die("close");

	fd = open("numbers.txt", O_RDWR);
	if (fd < 0)
		die("open");

	off_t pos = 0, line4 = -1, line5 = -1;
	int newlines = 0;
	while (line5 < 0 && (n = read(fd, &c, 1)) == 1)
	{
		pos++;
		if (c == '\n')
		{
			newlines++;
			if (newlines == 3)
				line4 = pos;
			if (newlines == 4)
				line5 = pos;
		}
	}
	if (n < 0)
		die("read");
	if (line5 < 0)
	{
		fprintf(stderr, "numbers.txt doesn't even have 4 lines??\n");
		return 1;
	}

	if (lseek(fd, line5, SEEK_SET) < 0)
		die("lseek");
	ssize_t rest_len = 0;
	while ((n = read(fd, rest + rest_len, sizeof rest - rest_len)) > 0)
		rest_len += n;
	if (n < 0)
		die("read");

	printf("Found line 4 at byte %lld, swapping 4 -> 100...\n", (long long)line4);
	if (lseek(fd, line4, SEEK_SET) < 0)
		die("lseek");
	if (write(fd, "100\n", 4) != 4)
		die("write");
	if (write(fd, rest, rest_len) != rest_len)
		die("write");

	printf("Here's numbers.txt now:\n");
	fflush(stdout);

	if (lseek(fd, 0, SEEK_SET) < 0)
		die("lseek");
	while ((n = read(fd, buf, sizeof buf)) > 0)
		if (write(STDOUT_FILENO, buf, n) != n)
			die("write");
	if (n < 0)
		die("read");

	if (close(fd) < 0)
		die("close");
	return 0;
}

