#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/fs.h>

#define DEVICE_NAME "myCharDev"
#define CLASS_NAME "myChar"

//======================================//

MODULE_DESCRIPTION("my test character device");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("SPLINTER1984");
MODULE_VERSION("0.1");

//======================================//

static struct class* myCharDevClass = NULL; /*<- the device-driver 
                                            classs stuct pointer*/
static struct device* myCharDevDevice = NULL; /*<- the device-driver 
                                              device stuct pointer*/

/** @brief Devices are represented as file structure in the kernel. 
 * The file_operations structure from /linux/fs.h lists the callback functions 
 * that you wish to associated with your file operations using a C99 syntax 
 * structure. char devices usually implement open, read, write and release calls
 */

static struct file_operations myCharDev_fops =
{
    .owner = THIS_MODULE
};

static int myCharDev_major_number = 0;

//=====================================//

int register_device(void)
{
    int res = 0;
    printk(KERN_NOTICE "myCharDev: register_device() is called\n");
    
    //dynamically allocate major number for the device
    res = register_chrdev(0, DEVICE_NAME, &myCharDev_fops);
    if (res < 0)
    {
        printk(KERN_WARNING "myCharDev: failed to register character device major number with error code = %i\n", res);
        return res;
    }
    
    myCharDev_major_number = res;
    printk(KERN_NOTICE "myCharDev: registered character device with major number = %i and minor minor numbers 0...255\n",myCharDev_major_number);
    
    //register the device class
    myCharDevClass= class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(myCharDevClass))
    {
        unregister_chrdev(myCharDev_major_number, DEVICE_NAME);
        printk(KERN_WARNING "myCharDev: failed to register character device class\n");
        return PTR_ERR(myCharDevClass);
    }

    printk(KERN_NOTICE "myCharDev: registered character device class correctly\n");
    
    //register the device driver
    myCharDevDevice = device_create(myCharDevClass, NULL, MKDEV(myCharDev_major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(myCharDevDevice))
    {
        class_destroy(myCharDevClass);
        unregister_chrdev(myCharDev_major_number,  DEVICE_NAME);
        printk(KERN_WARNING "myCharDev: failed to register device");
        return PTR_ERR(myCharDevDevice);
    }

    printk(KERN_NOTICE "myCharDev: character device created correctly\n");
    return 0;
}

void unregister_device(void)
{
    printk(KERN_NOTICE "myCharDev: unregister_device() is called\n");
    device_destroy(myCharDevClass, MKDEV(myCharDev_major_number, 0));
    class_unregister(myCharDevClass);
    class_destroy(myCharDevClass);
    unregister_chrdev(myCharDev_major_number, DEVICE_NAME);
    printk(KERN_NOTICE "myCharDev: character device unregistered correctly\n");
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

module_init(myCharDev_init);
module_exit(myCharDev_exit);

