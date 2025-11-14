#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define DRV_NAME "char_driver"

static dev_t dev_num;
static struct cdev drv_cdev;
static struct class *drv_class;
static struct device *drv_device;

static int drv_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO DRV_NAME ": opened\n");
    return 0;
}

static int drv_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO DRV_NAME ": closed\n");
    return 0;
}

static ssize_t drv_read(struct file *file, char __user *buf, size_t len, loff_t *off)
{
    const char msg[] = "Message from char_driver!\n";
    size_t msg_len = sizeof(msg);

    if (*off >= msg_len)
        return 0;

    if (copy_to_user(buf, msg, msg_len))
        return -EFAULT;

    *off += msg_len;
    return msg_len;
}

static const struct file_operations drv_fops = {
    .owner   = THIS_MODULE,
    .open    = drv_open,
    .release = drv_release,
    .read    = drv_read,
};

static int __init drv_init(void)
{
    int ret;

    ret = alloc_chrdev_region(&dev_num, 0, 1, DRV_NAME);
    if (ret < 0)
        return ret;

    cdev_init(&drv_cdev, &drv_fops);
    ret = cdev_add(&drv_cdev, dev_num, 1);
    if (ret < 0) {
        unregister_chrdev_region(dev_num, 1);
        return ret;
    }

    drv_class = class_create(DRV_NAME);
    if (IS_ERR(drv_class)) {
        cdev_del(&drv_cdev);
        unregister_chrdev_region(dev_num, 1);
        return PTR_ERR(drv_class);
    }

    drv_device = device_create(drv_class, NULL, dev_num, NULL, DRV_NAME);
    if (IS_ERR(drv_device)) {
        class_destroy(drv_class);
        cdev_del(&drv_cdev);
        unregister_chrdev_region(dev_num, 1);
        return PTR_ERR(drv_device);
    }

    printk(KERN_INFO DRV_NAME ": loaded (major %d minor %d)\n",
           MAJOR(dev_num), MINOR(dev_num));

    return 0;
}

static void __exit drv_exit(void)
{
    device_destroy(drv_class, dev_num);
    class_destroy(drv_class);
    cdev_del(&drv_cdev);
    unregister_chrdev_region(dev_num, 1);

    printk(KERN_INFO DRV_NAME ": unloaded\n");
}

module_init(drv_init);
module_exit(drv_exit);

MODULE_LICENSE("GPL");
