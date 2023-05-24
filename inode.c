#include <linux/init.h>
#include <linux/time.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/string.h>

static const struct super_operations ramfs_ops = {
	.statfs		= simple_statfs,
	.drop_inode	= generic_delete_inode,
	.show_options	= generic_show_options,
};

const struct inode_operations hellofs_file_inode_operations = {
	.getattr	= simple_getattr,
};

static const struct inode_operations hellofs_dir_inode_operations = {
	.creat		= hellofs_creat,
	.lookup		= simple_lookup,
};

const struct file_operations hellofs_file_operations = {
	.read		= hellofs_read,
};

ssize_t hellofs_read(struct file *filp, char __user *buf, size_t len, loff_t *ppos)
{
	char read_str[] = "HelloWorld";
	size_t read_str_len = strlen(read_str),
		   size = len > read_str_len ? read_str_len : len;

	strncpy(buf, read_str, size);
	*ppos = size;

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
		dget(dentry);	/* Extra count - pin the dentry in core */
		error = 0;
		dir->i_mtime = dir->i_ctime = CURRENT_TIME;
	}
	return error;
}

static int hellofs_create(struct inode *dir, struct dentry *dentry, int mode, struct nameidata *nd)
{
	return hellofs_mknod(dir, dentry, mode | S_IFREG, 0);
}

struct inode *hellofs_get_inode(struct super_block *sb, int mode, dev_t dev)
{
	struct inode * inode = new_inode(sb);

	if (inode) {
		inode->i_mode = mode;
		inode->i_uid = current_fsuid();
		inode->i_gid = current_fsgid();
		inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;

		switch (inode & S_IFMT) {
		case S_IFREG:
			inode->i_op = &hellofs_file_inode_operations;
			inode->i_fop = &hellofs_file_operations;
			break;

		case S_IFDIR:
			inode->i_op = &hellofs_dir_inode_operations;

			inc_nlink(inode);
			break;
		}
	}

	return inode;
}

static int hellofs_fill_super(struct super_block * sb, void * data, int silent)
{
	struct inode *inode = NULL;
	struct dentry *root; 
	int err;

	sb->s_maxbytes		= MAX_LFS_FILESIZE;
	sb->s_blocksize		= PAGE_CACHE_SIZE;
	sb->s_blocksize_bits	= PAGE_CACHE_SHIFT;
	sb->s_magic		= RAMFS_MAGIC;
	sb->s_op		= &ramfs_ops;
	sb->s_time_gran		= 1;

	inode = ramfs_get_inode(sb, S_IFDIR | 0755, 0);
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

static void __exit exit_ramfs_fs(void)
{}

module_init(init_hellofs_fs)
module_exit(exit_hellofs_fs)

