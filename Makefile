FUSE_CFLAGS=$(shell pkg-config --libs fuse3 --cflags fuse3)

cringe-fs: main.c
	gcc $(FUSE_CFLAGS) -Wall -Wextra -Wno-unused-parameter main.c -o cringe-fs

clean:
	rm -rvf cringe-fs
