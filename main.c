#define FUSE_USE_VERSION 31
#include <fuse3/fuse.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

time_t init_time;
time_t last_refresh;

int current_need;

#define MISS_ACTION_UNHAPYNESS 20
#define ACTION_HAPYNESS 40

#define NEED(a, useless__, useless2__) a,
enum {
#include "need_tok.h"
	NEED_CNT
};

#define NEED(useless__, s, useless2__) s,
char *need_str[] = {
#include "need_tok.h"
	NULL
};

#define NEED(useless__, useless2__, s) s,
char *no_need_str[] = {
#include "need_tok.h"
	"whatever you're trying to do !",
	NULL
};

#define MAX_PARASITES 64

int nb_parasite;

const char *parasite_names[MAX_PARASITES] = {
	"Jean-Robet-the-annoying-file.txt",
	"An-overexited-arch-user.BTW",
	"ev&ng*li#n.avi",
	"UwU.owo",
	"baldur-s-door-2.exe",
	"vim.notemacs",
	"windows-user.boring",
	"catgirl-dekimakura.png"
};

const char *parasites[MAX_PARASITES];

#define CRINGY_NAME "cringy"

char cringy_content[2048];

size_t cringy_content_l;

uint64_t unhappy_life;

uint64_t happy_life;

int have_need_been_satisfied;

const char *status = "";

char status_buf[1024];

char parasite_txt_buf[1024];

enum {
	FORCE_REFRESH = 1 << 0
};

static const char *hapiness_text(void)
{
	int happy_cnt = happy_life - unhappy_life;

	if (happy_cnt < -100) {
		return "very unhappy, and want to kill perself";
	} else if (happy_cnt < -25) {
		return "unhappy, and want you to do something";
	} else if (happy_cnt < 0) {
		return "slightly down";
	} else if (happy_cnt < 20) {
		return "okay";
	} else if (happy_cnt < 50) {
		return "mosly happy";
	} else {
		return "very happy";
	}
}


void make_text(int flag)
{
	time_t t = time(NULL);
	int nb_actions = (t - last_refresh) / 10;
	if (nb_actions) {
		flag |= FORCE_REFRESH;
		for (int i = 1; i < nb_actions; ++i) {
			unhappy_life += MISS_ACTION_UNHAPYNESS;
		}
	}
	if (flag & FORCE_REFRESH) {
		if (!have_need_been_satisfied) {
			unhappy_life += MISS_ACTION_UNHAPYNESS;
		}
		unhappy_life += nb_parasite * 2;
		for (int i = 0; i < nb_actions; ++i) {
			printf("check for parasites %d %d %d %d\n",
			       rand() & 1, rand() & 1, rand() & 1, rand() & 1);
			int new_parasite = (rand() & 1) == 0;
			printf("new parasite %d\n", new_parasite);
			if (new_parasite == 1) {
				int idx;
				do {
					idx = rand() % (MAX_PARASITES);
				} while (!parasite_names[idx]);
				printf("new parasite %d !!!!!!\n", idx);
				if (!parasites[idx]) {
					++nb_parasite;
					parasites[idx] = parasite_names[idx];
				}
			}
		}
		current_need = rand() % NEED_CNT;
		have_need_been_satisfied = 0;
		last_refresh = t;
		status = "";
	}
	if (nb_parasite) {
		sprintf(parasite_txt_buf, "there's %d parasited here", nb_parasite);
	}
	cringy_content_l = sprintf(cringy_content, "Hello :)\n"
				   "I live for %zu sec\n"
				   "cringy is: %s\n"
				   "%s\n"
				   "last thing cringy tell you:\n%s\n"
				   "\n%s\n", (size_t)t - init_time, hapiness_text(),
				   have_need_been_satisfied ? "cringy want nothing":
				   need_str[current_need], status,
				   nb_parasite ? parasite_txt_buf : "at last there's no parasites here");
	printf("cringy content: %s\n", cringy_content);
}

static void *cringy_init(struct fuse_conn_info *conn,
			struct fuse_config *cfg)
{
	(void) conn;
	printf("init cringy !\n");
	init_time = time(NULL);
	last_refresh = init_time;
	srand(init_time);

	make_text(FORCE_REFRESH);
	cfg->kernel_cache = 0;
	printf("%d - %d\n", getuid(), getgid());

	return NULL;
}

static int cringy_getattr(const char *path, struct stat *stbuf,
			 struct fuse_file_info *fi)
{
	(void) fi;
	int res = 0;

	memset(stbuf, 0, sizeof(struct stat));
	make_text(0);
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} else if (strcmp(path+1, CRINGY_NAME) == 0) {
		stbuf->st_mode = S_IFREG | 0666;
		stbuf->st_nlink = 1;
		stbuf->st_size = cringy_content_l;
	} else {
		for (int i = 0; i < MAX_PARASITES; ++i) {
			if (parasites[i] && !strcmp(path + 1, parasites[i])) {
				stbuf->st_mode = S_IFREG | 0666;
				stbuf->st_nlink = 1;
				stbuf->st_size = 0;
				return res;
			}
		}
		res = -ENOENT;
	}

	return res;
}

static int cringy_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi,
			 enum fuse_readdir_flags flags)
{
	(void) offset;
	(void) fi;
	(void) flags;

	if (strcmp(path, "/") != 0)
		return -ENOENT;

	filler(buf, ".", NULL, 0, 0);
	filler(buf, "..", NULL, 0, 0);
	filler(buf, CRINGY_NAME, NULL, 0, 0);
	for (int i = 0; i < MAX_PARASITES; ++i) {
		if (parasites[i]) {
			filler(buf, parasites[i], NULL, 0, 0);
		}
	}

	return 0;
}

static int cringy_unlink(const char *path)
{
	printf("remove %s\n", path);
	if (strcmp(path+1, CRINGY_NAME) == 0) {
		status = "YOU TRY TO REMOVE ME? ME !";
		unhappy_life += 100;
		return 0;
	}
	for (int i = 0; i < MAX_PARASITES; ++i) {
		if (!parasites[i])
			continue;
		if (!strcmp(path+1, parasites[i])) {
			happy_life += 10;
			parasites[i] = NULL;
			--nb_parasite;
			return 0;
		}
	}
	return -ENOENT;
}

static int cringy_open(const char *path, struct fuse_file_info *fi)
{
	if (strcmp(path+1, CRINGY_NAME) != 0)
		return -ENOENT;
	return 0;
}

static int cringy_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
	(void) fi;
	if(strcmp(path+1, CRINGY_NAME) != 0)
		return -ENOENT;

	make_text(0);
	if ((size_t)offset < cringy_content_l) {
		if (offset + size > cringy_content_l)
			size = cringy_content_l - offset;
		memcpy(buf, cringy_content + offset, size);
	} else
		size = 0;

	return size;
}

#define cmp(buf, s)				\
	!strncmp(buf, s, size) || !strncmp(buf, s"\n", size)

static int cringy_write(const char *path, const char *buf, size_t size, off_t off, struct fuse_file_info *fi)
{
	printf("write %s - %s\n", path, buf);
	if (strcmp(path+1, CRINGY_NAME)) {
		return -ENOENT;
	}

	if (cmp(buf, "food") || cmp(buf, "feed")) {
		printf("give food\n");
		if (current_need != NEED_FOOD) {
			status = status_buf;
			if (have_need_been_satisfied)
				goto wrong;
			sprintf(status_buf, "%s your disgusting food", no_need_str[current_need]);
			unhappy_life += 5;
		} else if (have_need_been_satisfied) {
			status = "I've aleready eat :(";
			unhappy_life += 5;
		} else {
			have_need_been_satisfied = 1;
			status = "mon mon mon, thanks for the food";
			happy_life += 40;
		}
	} else if (cmp(buf, "hug")) {
		printf("hug\n");
		if (current_need != NEED_HUG) {
			status = status_buf;
			if (have_need_been_satisfied)
				goto wrong;
			sprintf(status_buf, "%s an hug", no_need_str[current_need]);
			unhappy_life += 5;
		} else if (have_need_been_satisfied) {
			status = "stop hugging me, that's harasing";
			unhappy_life += 5;
		} else {
			have_need_been_satisfied = 1;
			status = "cringy hug you back, he fell happy, you not so much";
			happy_life += 40;
		}
	} else if (cmp(buf, "wash")) {
		if (current_need != NEED_BATH) {
			status = status_buf;
			if (have_need_been_satisfied)
				goto wrong;
			sprintf(status_buf, "%s a bath", no_need_str[current_need]);
			unhappy_life += 5;
		} else if (have_need_been_satisfied) {
			status = "stop washing me, I'M ALREADY CLEAN";
			unhappy_life += 5;
		} else {
			have_need_been_satisfied = 1;
			status = "WATTER !";
			happy_life += 40;
		}

		printf("wash\n");
	} else if (cmp(buf, "play")) {
		if (current_need != NEED_PLAY) {
			status = status_buf;
			if (have_need_been_satisfied)
				goto wrong;
			sprintf(status_buf, "%s to play with you, you, I mean erk", no_need_str[current_need]);
			unhappy_life += 5;
		} else if (have_need_been_satisfied) {
			status = "i'm wasted, stop it";
			unhappy_life += 5;
		} else {
			have_need_been_satisfied = 1;
			int r = rand();
			switch (r & 3) {
			case 0:
				status = "We played baldur's door 1, the best one";
				break;
			case 1:
				status = "I fall out in love with this game\nso sad 3 and 4 where bad, at last tactics was pretty good";
				break;
			default:
				status = "YAY, let's play !";
			}
			happy_life += 40;
		}
	} else if (cmp(buf, "help")) {
		status = "you can, 'feed' me, 'hug' me, 'wash' me, and 'play' with me";
	} else {
		status = "what are you doing ?\nyou can ask for help, instead of doing that";
		printf("unknow commande\n");
	}
	goto out;
wrong:
	sprintf(status_buf, "i don't want %s", no_need_str[current_need]);
	unhappy_life += 3;
out:
	make_text(0);
	return size;
}

static const struct fuse_operations cringy_oper = {
	.init           = cringy_init,
	.getattr	= cringy_getattr,
	.readdir	= cringy_readdir,
	.open		= cringy_open,
	.read		= cringy_read,
	.unlink		= cringy_unlink,
	.write		= cringy_write,
};

int main(int argc, char *argv[])
{
	int ret;

	ret = fuse_main(argc, argv, &cringy_oper, NULL);
	return ret;
}
