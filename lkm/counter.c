#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "counter_dev_"
#define CLASS_NAME "counter_class_"

MODULE_LICENSE("GPL"); 

static int dev_open(struct inode*, struct file*);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops = {
	.open = dev_open,
	.read = dev_read,
	.write = dev_write,
	.release = dev_release
};

static int major_number;
static struct class*  counter_class  = NULL;
static struct device* counter_device = NULL;
loff_t write_pos = 0;

static int __init counter_init(void) {
	printk("Counter: Initialization started.\n");
	major_number = register_chrdev(0, DEVICE_NAME, &fops);
	printk("Counter: major number %d allocated.\n", major_number);
	counter_class = class_create(THIS_MODULE, CLASS_NAME);
	printk("Counter: device class registered correctly.\n");
	counter_device = device_create(counter_class, NULL, MKDEV(major_number, 0),
				       NULL, DEVICE_NAME);
	printk("Counter: device created correctly.\n");

	char* file = "/etc/work_file";
	struct file *f;
	f = filp_open(file, O_CREAT | O_RDWR, 0666);
	printk("Counter: file opened.\n");

	kernel_write(f, "text", 4, &write_pos);
	printk("Counter: written.\n");



	return 0;
}

static void __exit counter_exit(void) {
	device_destroy(counter_class, MKDEV(major_number, 0));
	class_unregister(counter_class);
	class_destroy(counter_class);
	unregister_chrdev(major_number, DEVICE_NAME);
	printk("Counter: desctroy succeeded\n");
}

static int dev_open(struct inode* inodep, struct file* filep){
	printk("Counter: device opened.

module_init(counter_init);
module_exit(counter_exit);
