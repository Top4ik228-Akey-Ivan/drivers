#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

#define DRV_NAME "char_driver"
#define BUF_SIZE 32

/* IOCTL команды */
#define IOCTL_CLEAR _IO('x', 1)
#define IOCTL_CHECK _IOR('x', 2, int*)

static dev_t dev_num;
static struct cdev drv_cdev;
static struct class *drv_class;
static struct device *drv_device;

static char data_buf[BUF_SIZE];
static bool is_empty = true;

static int drv_open(struct inode *inode, struct file *file)
{
	pr_info(DRV_NAME ": open\n");
	return 0;
}

static int drv_release(struct inode *inode, struct file *file)
{
	pr_info(DRV_NAME ": close\n");
	return 0;
}

static ssize_t drv_read(struct file *file, char __user *user_buf, size_t len, loff_t *off)
{
	size_t n = BUF_SIZE;

	if (*off >= n)
		return 0;

	if (copy_to_user(user_buf, data_buf, n))
		return -EFAULT;

	*off += n;
	return n;
}

static ssize_t drv_write(struct file *file, const char __user *user_buf, size_t len, loff_t *off)
{
	if (len >= BUF_SIZE)
		return -EINVAL;

	if (copy_from_user(data_buf, user_buf, len))
		return -EFAULT;

	data_buf[len] = '\0';
	is_empty = false;

	return len;
}

static long drv_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int temp;

	switch (cmd) {

	case IOCTL_CLEAR:
		memset(data_buf, 0, BUF_SIZE);
		is_empty = true;
		break;

	case IOCTL_CHECK:
		temp = is_empty;
		if (copy_to_user((int*)arg, &temp, sizeof(temp)))
			return -EFAULT;
		break;

	default:
		return -ENOTTY;
	}

	return 0;
}

static const struct file_operations drv_fops = {
	.owner          = THIS_MODULE,
	.open           = drv_open,
	.release        = drv_release,
	.read           = drv_read,
	.write          = drv_write,
	.unlocked_ioctl = drv_ioctl,
};

static int __init drv_init(void) // Вызывается при insmod
{
	int ret;

	ret = alloc_chrdev_region(&dev_num, 0, 1, DRV_NAME); // 📌 Ядро выдаёт: major/minor
	if (ret)
		return ret;

	// Регистрируем символьное устройство
	cdev_init(&drv_cdev, &drv_fops);
	ret = cdev_add(&drv_cdev, dev_num, 1);

	if (ret)
		goto err_region;

	// Создание класса и устройства В /dev появляется: /dev/char_driver
	drv_class = class_create(DRV_NAME);
	if (IS_ERR(drv_class)) {
		ret = PTR_ERR(drv_class);
		goto err_cdev;
	}
	drv_device = device_create(drv_class, NULL, dev_num, NULL, DRV_NAME);
	if (IS_ERR(drv_device)) {
		ret = PTR_ERR(drv_device);
		goto err_class;
	}

	pr_info(DRV_NAME ": loaded major=%d minor=%d\n",
	        MAJOR(dev_num), MINOR(dev_num));
	return 0;

// обработка ошибико при инициализации драйвера
err_class:
	class_destroy(drv_class);
err_cdev:
	cdev_del(&drv_cdev);
err_region:
	unregister_chrdev_region(dev_num, 1);
	return ret;
}

static void __exit drv_exit(void) // Вызывается при rmmod
{
	// Полная очистка ресурсов
	device_destroy(drv_class, dev_num);
	class_destroy(drv_class);
	cdev_del(&drv_cdev);
	unregister_chrdev_region(dev_num, 1);
	pr_info(DRV_NAME ": unloaded\n");
}
//Точки входа и выхода
module_init(drv_init);
module_exit(drv_exit);

MODULE_LICENSE("GPL");
