#define _XOPEN_SOURCE 700

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

static void die(const char *what)
{
	perror(what);
	exit(EXIT_FAILURE);
}

int main(void)
{
	struct stat st;

	int fd = open("sparse.bin", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
		die("open");
	if (write(fd, "START", 5) != 5)
		die("write");
	if (lseek(fd, 1024 * 1024, SEEK_CUR) < 0)
		die("lseek");
	if (write(fd, "END", 3) != 3)
		die("write");
	if (close(fd) < 0)
		die("close");

	fd = open("sparse.bin", O_RDONLY);
	if (fd < 0)
		die("open");

	off_t size = lseek(fd, 0, SEEK_END);
	if (size < 0)
		die("lseek");
	printf("Apparent size of sparse.bin: %lld bytes\n", (long long)size);

	/*
	 * Why is du so much smaller than the apparent size?
	 * The lseek() jumped over 1 MiB without writing anything there, so the
	 * filesystem never allocated any blocks fr that part - it's a "hole".
	 * Reading from it just gives back zeros. ls -l and lseek show the
	 * apparent size (1048584 bytes), but du only counts the blocks that
	 * really exist on disk: the one with "START" and the one with "END"
	 * (about 8 KiB).
	 */
	if (fstat(fd, &st) < 0)
		die("fstat");
	printf("Actually used on disk: %lld bytes (the rest is a hole!)\n",
	       (long long)st.st_blocks * 512);

	if (close(fd) < 0)
		die("close");
	return 0;
}

