#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/cdev.h>

module_init(myCharDev_init);
module_exit(myCharDev_exit);
MODULE_DESCRIPTION("my test character device");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("SPLINTER1984");

static struct file_operations myCharDev_fops =
{
    .owner = THIS_MODULE,
}

static int myCharDev_major_number = 0;
static const char myCharDev_name[] = "myCharDev";

int register_device(void)
{
    int res = 0;
    printk(KERN_NOTICE "myCharDev: register_device() is called\n");

    res = register_chrdev(0, myCharDev_name, &myCharDev_fops);
    if (res < 0)
    {
        printk(KERN_WARNING "myCharDev: can not register character device with errorcode = %i\n", res);
        return res;
    }

    myCharDev_major_number = res;
    printk(KERN_NOTICE "myCharDev: registered character device with major number = %i and minor minor numbers 0...255\n"
            ,myCharDev_major_number);

    return 0;
}

void unregister_device(void)
{
    printk(KERN_NOTICE "myCharDev: unregister_device() is called\n");
    if (myCharDev_major_number != 0)
    {
        unregister_chrdev(myCharDev_major_number, myCharDev_name);
    }
}

static int myCharDev_init(void)
{
    int res = 0;
    printk(KERN_INFO "myCharDev: Initialization started\n");
    res = register_device();
    return res;
}

static void myCharDev_exit(void)
{
    printk(KERN_NOTICE "myCharDev: Exiting\n");
    unregister_device();
}
