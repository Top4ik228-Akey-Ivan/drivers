#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/module.h>
#include <linux/printk.h>

#include "ioctl.h"

#define DRV_NAME  "mai_lab1_driver"
#define DEV_NAME  "mai_lab1_dev"
#define CLS_NAME  "mai_lab1_class"
#define HISTO_MAX 500

// Буфер устройства и гистограмма
static int dev_buffer = 0;
static bool buf_is_empty = true;

static size_t histo_len = 0;
static size_t histo_buf[HISTO_MAX];

static ulong last_write_time = 0;
static ulong time_accum = 0;

// Структуры драйвера
static dev_t dev_no;
static struct cdev dev_cdev;
static struct class *dev_class;
static struct device *dev_device;

// Прототипы
static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char __user *, size_t, loff_t *);
static long dev_ioctl(struct file *, unsigned int, unsigned long);

// Операции устройства
static const struct file_operations fops = {
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write,
    .unlocked_ioctl = dev_ioctl,
    .owner = THIS_MODULE,
};

static int dev_open(struct inode *inode, struct file *file)
{
    pr_info(DRV_NAME ": device opened\n");
    return 0;
}

static int dev_release(struct inode *inode, struct file *file)
{
    pr_info(DRV_NAME ": device closed\n");
    return 0;
}

// Чтение значения + обновление гистограммы
static ssize_t dev_read(struct file *flip, char __user *user_buf,
                        size_t count, loff_t *offset)
{
    if (count != sizeof(dev_buffer))
        return -EINVAL;

    if (buf_is_empty)
        return 0;

    if (copy_to_user(user_buf, &dev_buffer, sizeof(dev_buffer)))
        return -EFAULT;

    // Обновляем гистограмму
    histo_buf[histo_len]++;

    ulong delta = jiffies - last_write_time;
    time_accum += delta;

    if (jiffies_to_usecs(time_accum) >= 50) {
        time_accum = 0;
        histo_len++;
    }

    return sizeof(dev_buffer);
}

// Запись одного значения
static ssize_t dev_write(struct file *flip, const char __user *user_buf,
                         size_t count, loff_t *offset)
{
    if (count != sizeof(dev_buffer))
        return -EINVAL;

    if (copy_from_user(&dev_buffer, user_buf, count))
        return -EFAULT;

    buf_is_empty = false;
    last_write_time = jiffies;

    return count;
}

// Обработка пользовательских ioctl
static long dev_ioctl(struct file *flip, unsigned int cmd, unsigned long arg)
{
    switch (cmd) {

    case IOCTL_HISTO_LEN:
        if (copy_to_user((size_t *)arg, &histo_len, sizeof(histo_len)))
            return -EFAULT;
        break;

    case IOCTL_HISTO_BUF:
        if (copy_to_user((size_t *)arg, histo_buf,
                         histo_len * sizeof(size_t)))
            return -EFAULT;
        break;

    default:
        return -EINVAL;
    }

    return 0;
}

static int __init drv_init(void)
{
    int res;

    pr_info(DRV_NAME ": initializing\n");

    // Регистрация символьного устройства
    if ((res = alloc_chrdev_region(&dev_no, 0, 1, DEV_NAME)) < 0)
        return res;

    cdev_init(&dev_cdev, &fops);
    dev_cdev.owner = THIS_MODULE;

    if ((res = cdev_add(&dev_cdev, dev_no, 1)) < 0) {
        unregister_chrdev_region(dev_no, 1);
        return res;
    }

    dev_class = class_create(CLS_NAME);
    if (IS_ERR(dev_class)) {
        cdev_del(&dev_cdev);
        unregister_chrdev_region(dev_no, 1);
        return PTR_ERR(dev_class);
    }

    dev_device = device_create(dev_class, NULL, dev_no, NULL, DEV_NAME);
    if (IS_ERR(dev_device)) {
        class_destroy(dev_class);
        cdev_del(&dev_cdev);
        unregister_chrdev_region(dev_no, 1);
        return PTR_ERR(dev_device);
    }

    pr_info(DRV_NAME ": initialized successfully\n");
    return 0;
}

static void __exit drv_exit(void)
{
    pr_info(DRV_NAME ": unloading\n");

    device_destroy(dev_class, dev_no);
    class_destroy(dev_class);
    cdev_del(&dev_cdev);
    unregister_chrdev_region(dev_no, 1);

    pr_info(DRV_NAME ": unloaded\n");
}

module_init(drv_init);
module_exit(drv_exit);

MODULE_LICENSE("GPL");
