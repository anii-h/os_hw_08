#define _POSIX_C_SOURCE 200809L

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PATH_SIZE 4096

static void die(const char *what)
{
	perror(what);
	exit(EXIT_FAILURE);
}

static void ask(const char *question, char *buf, size_t size)
{
	size_t len = 0;
	char c;

	printf("%s", question);
	fflush(stdout);

	while (len + 1 < size)
	{
		ssize_t n = read(STDIN_FILENO, &c, 1);
		if (n < 0)
			die("read");
		if (n == 0 || c == '\n')
			break;
		buf[len++] = c;
	}
	buf[len] = '\0';

	if (len == 0)
	{
		fprintf(stderr, "you didn't type anything :(\n");
		exit(EXIT_FAILURE);
	}
}

int main(void)
{
	char path[PATH_SIZE];
	char c;

	ask("Feed me a file and I'll read it backwards: ", path, sizeof path);

	int fd = open(path, O_RDONLY);
	if (fd < 0)
		die("open");

	off_t size = lseek(fd, 0, SEEK_END);
	if (size < 0)
		die("lseek");

	for (off_t pos = size - 1; pos >= 0; pos--)
	{
		if (lseek(fd, pos, SEEK_SET) < 0)
			die("lseek");
		if (read(fd, &c, 1) != 1)
			die("read");
		if (write(STDOUT_FILENO, &c, 1) != 1)
			die("write");
	}

	if (write(STDOUT_FILENO, "\n", 1) != 1)
		die("write");

	if (close(fd) < 0)
		die("close");
	return 0;
}

