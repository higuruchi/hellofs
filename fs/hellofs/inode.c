#include <linux/sched.h>

#include "inode.h"

const char read_str[] = "HelloWorld";

ssize_t
hellofs_read(struct file *filp, char __user *buf, size_t len, loff_t *ppos)
{
	size_t read_str_len = strlen(read_str),
		   size = len > read_str_len ? read_str_len : len;

	if (*ppos >= read_str_len) {
		return 0;
	}

	strncpy(buf, read_str, size);
	*ppos += read_str_len;

	return size;
}

static int
hellofs_mknod(struct inode *dir, struct dentry *dentry, int mode, dev_t dev)
{
	struct inode * inode = hellofs_get_inode(dir->i_sb, mode, dev);
	int error = -ENOSPC;

	if (inode) {
		if (dir->i_mode & S_ISGID) {
			inode->i_gid = dir->i_gid;
		}
		d_instantiate(dentry, inode);
		dget(dentry);
		error = 0;
		dir->i_mtime = dir->i_ctime = CURRENT_TIME;

		if (mode & S_IFREG) {
			inode->i_size = strlen(read_str);
		}
	}
	return error;
}

static int
hellofs_create(struct inode *dir, struct dentry *dentry, int mode, struct nameidata *nd)
{
	return hellofs_mknod(dir, dentry, mode | S_IFREG, 0);
}

const struct inode_operations hellofs_file_inode_operations = {
	.getattr	= simple_getattr,
};

static const struct inode_operations hellofs_dir_inode_operations = {
	.create		= hellofs_create,
	.lookup		= simple_lookup,
};

const struct file_operations hellofs_file_operations = {
	.read		= hellofs_read,
};

struct inode *
hellofs_get_inode(struct super_block *sb, int mode, dev_t dev)
{
	struct inode *inode = new_inode(sb);

	if (inode) {
		inode->i_mode = mode;
		inode->i_uid = current_fsuid();
		inode->i_gid = current_fsgid();
		inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;

		switch (mode & S_IFMT) {
		case S_IFREG:
			inode->i_op = &hellofs_file_inode_operations;
			inode->i_fop = &hellofs_file_operations;
			break;

		case S_IFDIR:
			inode->i_op = &hellofs_dir_inode_operations;
			inode->i_fop = &simple_dir_operations;
			inc_nlink(inode);
			break;
		}
	}

	return inode;
}

