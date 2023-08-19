#include <linux/fs.h>

struct inode *
hellofs_get_inode(struct super_block *sb, int mode, dev_t dev);
