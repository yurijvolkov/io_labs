#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/atomic.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/version.h>

#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>


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

static ssize_t var10_read(struct file *f, char __user *buf, size_t len, loff_t *off) {
	ssize_t answer = kernel_read(f, buf, len, off);
	buf[answer] = 0;
	printk(KERN_INFO "%s", buf);
	return answer;
}

static ssize_t dev_read(struct file *f_, char __user *buf, size_t len, loff_t *off)
{
	if(f != NULL) {
		char *buffer = kvmalloc(len + 1, GFP_USER);
		ssize_t answer;
		while(answer = var10_read(f, buffer, len, off) > 0);
		kvfree(buffer);
		return answer;
	}
	return 0;
}
static ssize_t dev_write(struct file *filep, const char * buffer, size_t len, loff_t *offset) {
	sprintf(message, "%s", buffer);
	message[len-1] = 0;
	printk("Counter: received %d chars\n", len);
	printk("Recevide: %s", message);

	if ( strncmp(message, "open", 4) == 0) {
		if (f == NULL) {
			f = filp_open(message + 5, O_CREAT | O_RDWR, 0666);
			write_pos = 0;
			printk("Counter: file opened.\n");
		}
		else {
			printk("File already opened.\n");
		}

	} else if ( strncmp(message, "close", 5) == 0) {
		if (f != NULL)
			filp_close(f, NULL);
		else 
			printk("File not opened");
	} else {
		if (f != NULL) {
			printk("Writing to file.\n");	
			sprintf(message, "%d\n", len-1);
			kernel_write(f, message, strlen(message), &write_pos);
		}
		else 
			printk("Writing to closed file.");
	}

	return len;
}



module_init(counter_init);
module_exit(counter_exit);
