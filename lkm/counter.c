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
loff_t write_pos_zero = 0;
char* file = "/etc/work_file";
struct file *f = NULL;
static char   message[256] = {0};

static int __init counter_init(void) {
	printk("Counter: Initialization started.\n");
	major_number = register_chrdev(0, DEVICE_NAME, &fops);
	printk("Counter: major number %d allocated.\n", major_number);
	counter_class = class_create(THIS_MODULE, CLASS_NAME);
	printk("Counter: device class registered correctly.\n");
	counter_device = device_create(counter_class, NULL, MKDEV(major_number, 0),
				       NULL, DEVICE_NAME);
	printk("Counter: device created correctly.\n");


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
	printk("Counter: device opened.\n");
	return 0;
}

static int dev_release(struct inode* inodep, struct file* filep){
	printk("Counter: device released.\n");
	return 0;
}
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
	printk("In dev read\n");
	if (f != NULL){
		printk("In if\n");
		write_pos_zero = 0;
		kernel_read(f, message, 2, &write_pos_zero);
		printk("READED: %s\n", message);
		copy_to_user(buffer, message, strlen(message));
		return 0;
	}	
}

static ssize_t dev_write(struct file *filep, const char * buffer, size_t len, loff_t *offset) {
	printk("CHE\n");
	sprintf(message, "%s", buffer);
	message[len-1] = 0;
	printk("Counter: received %d chars\n", len);
	printk("Recevide: %s", message);

	if (f == NULL && strncmp(message, "open", 4) == 0) {
		f = filp_open(message + 5, O_CREAT | O_RDWR, 0666);
		write_pos = 0;

		printk("Counter: file opened.\n");
		return len;
	}
	
	if (f != NULL) {
		printk("Writing to file.\n");	
		sprintf(message, "%d\n", len-1);
		kernel_write(f, message, strlen(message), &write_pos);

		return len;
	}

	printk("Writing to closed file!\n");
	return len;
}



module_init(counter_init);
module_exit(counter_exit);
