#define _POSIX_C_SOURCE 200809L

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void die(const char *what)
{
	perror(what);
	exit(EXIT_FAILURE);
}

int main(void)
{
	char buf[32];

	int fd = open("data.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
		die("open");
	if (write(fd, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 26) != 26)
		die("write");
	if (close(fd) < 0)
		die("close");
	printf("Wrote the whole alphabet into data.txt\n");

	fd = open("data.txt", O_RDWR);
	if (fd < 0)
		die("open");

	off_t size = lseek(fd, 0, SEEK_END);
	if (size < 0)
		die("lseek");
	printf("Size before: %lld bytes\n", (long long)size);

	printf("Chop chop... keeping only the first 10 letters\n");
	if (ftruncate(fd, 10) < 0)
		die("ftruncate");

	size = lseek(fd, 0, SEEK_END);
	if (size < 0)
		die("lseek");
	printf("Size after: %lld bytes\n", (long long)size);

	if (lseek(fd, 0, SEEK_SET) < 0)
		die("lseek");
	ssize_t n = read(fd, buf, sizeof buf);
	if (n < 0)
		die("read");
	printf("What survived: %.*s\n", (int)n, buf);

	if (close(fd) < 0)
		die("close");
	return 0;
}
