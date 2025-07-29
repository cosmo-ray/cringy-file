FUSE_CFLAGS=$(shell pkg-config --cflags fuse3)

FUSE_LD=$(shell pkg-config --libs fuse3)

cringe-fs: main.c
	gcc $(FUSE_CFLAGS) -Wall -Wextra -Wno-unused-parameter main.c -o cringe-fs $(FUSE_LD)

clean:
	rm -rvf cringe-fs
