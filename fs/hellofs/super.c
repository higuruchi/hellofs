#include <linux/pagemap.h>

#include "inode.h"

#define HELLOFS_DEFAULT_MODE	0755

static const struct super_operations hellofs_opts = {
	.statfs		= simple_statfs,
	.drop_inode	= generic_delete_inode,
	.show_options	= generic_show_options,
};

static int hellofs_fill_super(struct super_block * sb, void * data, int silent)
{
	struct inode *inode = NULL;
	struct dentry *root; 
	int err;

	save_mount_options(sb, data);

	sb->s_maxbytes		= MAX_LFS_FILESIZE;
	sb->s_blocksize		= PAGE_CACHE_SIZE;
	sb->s_blocksize_bits	= PAGE_CACHE_SHIFT;
	sb->s_op				= &hellofs_opts;
	sb->s_time_gran			= 1;

	inode = hellofs_get_inode(sb, S_IFDIR | S_IALLUGO | HELLOFS_DEFAULT_MODE, 0);
	if (!inode) {
		err = -ENOMEM;
		goto fail;
	}

	root = d_alloc_root(inode);
	sb->s_root = root;
	if (!root) {
		err = -ENOMEM;
		goto fail;
	}

	return 0;

fail:
	iput(inode);
	return err;
}

int hellofs_get_sb(struct file_system_type *fs_type,
	int flags, const char *dev_name, void *data, struct vfsmount *mnt)
{
	return get_sb_nodev(fs_type, flags, data, hellofs_fill_super, mnt);
}

static void hellofs_kill_sb(struct super_block *sb)
{}

static struct file_system_type hellofs_fs_type = {
	.name		= "hellofs",
	.get_sb		= hellofs_get_sb,
	.kill_sb	= hellofs_kill_sb,
};

static int __init init_hellofs_fs(void)
{
	return register_filesystem(&hellofs_fs_type);
}

static void __exit exit_hellofs_fs(void)
{}

module_init(init_hellofs_fs)
module_exit(exit_hellofs_fs)

