#define _POSIX_C_SOURCE 200809L

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define CHUNK     4096
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

static ssize_t read_chunk(int fd, char *buf)
{
	ssize_t got = 0;

	while (got < CHUNK)
	{
		ssize_t n = read(fd, buf + got, CHUNK - got);
		if (n < 0)
			die("read");
		if (n == 0)
			break;
		got += n;
	}
	return got;
}

int main(void)
{
	char path1[PATH_SIZE], path2[PATH_SIZE];
	char buf1[CHUNK], buf2[CHUNK];
	long long offset = 0;

	ask("First suspect: ", path1, sizeof path1);
	ask("Second suspect: ", path2, sizeof path2);

	int fd1 = open(path1, O_RDONLY);
	if (fd1 < 0)
		die("open first file");
	int fd2 = open(path2, O_RDONLY);
	if (fd2 < 0)
		die("open second file");

	while (1)
	{
		ssize_t n1 = read_chunk(fd1, buf1);
		ssize_t n2 = read_chunk(fd2, buf2);
		ssize_t min = n1 < n2 ? n1 : n2;

		for (ssize_t i = 0; i < min; i++)
		{
			if (buf1[i] != buf2[i])
			{
				printf("Files differ at byte %lld\n", offset + i);
				return 1;
			}
		}

		if (n1 != n2)
		{
			printf("Files differ at byte %lld\n", offset + min);
			return 1;
		}

		if (n1 == 0)
			break;
		offset += n1;
	}

	printf("Files are identical\n");

	if (close(fd1) < 0)
		die("close");
	if (close(fd2) < 0)
		die("close");
	return 0;
}

