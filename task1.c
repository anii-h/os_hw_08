#define _POSIX_C_SOURCE 200809L

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZE  4096
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
	char src_path[PATH_SIZE], dst_path[PATH_SIZE], buf[BUF_SIZE];
	long long total = 0;
	ssize_t n;

	ask("Which file are we cloning? ", src_path, sizeof src_path);
	ask("Where should the clone go? ", dst_path, sizeof dst_path);

	int src = open(src_path, O_RDONLY);
	if (src < 0)
		die("open source");

	int dst = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (dst < 0)
		die("open destination");

	while ((n = read(src, buf, sizeof buf)) > 0)
	{
		ssize_t done = 0;
		while (done < n)
		{
			ssize_t w = write(dst, buf + done, n - done);
			if (w < 0)
				die("write");
			done += w;
		}
		total += n;
	}
	if (n < 0)
		die("read");

	if (close(src) < 0)
		die("close source");
	if (close(dst) < 0)
		die("close destination");

	printf("Clone complete! %lld bytes copied from %s to %s\n", total, src_path, dst_path);
	return 0;
}
