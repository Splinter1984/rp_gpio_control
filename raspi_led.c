#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include "raspi_led.h"

MODULE_AUTHOR("Splinter1984");
MODULE_DESCRIPTION("raspi led control");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

static volatile uint32_t __iomem *gpio_base;
#define BCM2837_GPIO_BASE 0x20200000    //for raspi zero W model and 0x3F200000 for raspi B+
#define BCM2837_GPIO_SET_OFFSET (0x1C/4)
#define BCM2837_GPIO_CLR_OFFSET (0x28/4)

#define GPIO_INPUT(g) ((*(gpio_base + ((g)/10))) &= ~(0x7 << (((g)%10)*3)))
#define GPIO_CLEAN(g) ((*(gpio_base + ((g)/10))) &= ~(0x7 << (((g)%10)*3)))
#define GPIO_OUTPUT(g) ((*(gpio_base + ((g)/10))) |= (0x1 << (((g)%10)*3)))

//to pull high on GPIO pin(g)
#define GPIO_HIGH(g) ((*(gpio_base + BCM2837_GPIO_SET_OFFSET + ((g)/32))) = (0x1 << ((g)%32)))
//to pull low on GPIO pin(g)
#define GPIO_LOW(g) ((*(gpio_base + BCM2837_GPIO_CLR_OFFSET + ((g)/32))) = (0x1 << ((g)%32)))

#define LED_DEV_CNT 1   //we want to register only one dev with one minor number
#define LED_DEV_NAME "raspiLedDev"

static struct LED_DEVICE 
{
    dev_t devno;
    struct class *ledClass;
    struct device *ledDev;
    struct cdev cdev;
} LED_device;

static int __init led_init(void);
static int led_open(struct inode *inode, struct file *filp);
static int led_release(struct inode *indoe, struct file *filp);
static ssize_t led_write(struct file *filp, const char *buf, size_t len, loff_t *pos);

static struct file_operations led_ops=
{
    .owner = THIS_MODULE,
    .open = led_open,
    .release = led_release,
    .write = led_write,
};

static int __init led_init(void)
{
    int res = -EINVAL;
    int i = 0;
    // transfer from phys GPIO mem to Kernel virt mem
    gpio_base = ioremap(BCM2837_GPIO_BASE, 0xA0);   
    if (!gpio_base)
    {
        printk(KERN_INFO "raspiLedDev: ioremap for GPIO_BASE error: %i\n", res);
        return res;
    }

    for (i = 2; i < 10; i++)
    {
        GPIO_CLEAN(i);  //set register to 0b000 for GPIO BCM pin
        GPIO_OUTPUT(i); //set register to 0b001 for GPIO BCM pin
        GPIO_HIGH(i);   //set register to 0b1 for GPIO BCM pin
    }

    // allocate dev number in kernel
    res = alloc_chrdev_region(&LED_device.devno, 0, LED_DEV_CNT, LED_DEV_NAME);
    if (res)
    {
        printk(KERN_INFO "raspiLedDev: Unable to allocate number: %i\n", res);
        return res;
    }

    cdev_init(&LED_device.cdev, &led_ops);
    LED_device.cdev.owner = THIS_MODULE;

    //add cdev into kernel
    res = cdev_add(&LED_device.cdev, LED_device.devno, LED_DEV_CNT);
    if (res)
    {
        printk(KERN_INFO "raspiLedDev: add cdev in Kernel fail\n");
        unregister_chrdev_region(LED_device.devno, LED_DEV_CNT);
        return res;
    }

    //create class
    LED_device.ledClass = class_create(THIS_MODULE, LED_DEV_NAME);
    if (IS_ERR(LED_device.ledClass))
    {
        printk(KERN_INFO "raspiLedDev: failed to register character device class\n");
        cdev_del(&LED_device.cdev);
        return PTR_ERR(LED_device.ledClass);
    }

    //create device
    LED_device.ledDev = device_create(LED_device.ledClass, NULL, LED_device.devno, NULL, LED_DEV_NAME);
    if (IS_ERR(LED_device.ledDev))
    {
        printk(KERN_INFO "raspiLedDev: device create fail\n");
        class_destroy(LED_device.ledClass);
        return PTR_ERR(LED_device.ledDev);
    }
    
    return 0;
}

static int led_open(struct inode *inode, struct file *filep)
{
    filep->private_data = container_of(inode->i_cdev, struct LED_DEVICE, cdev);
    printk(KERN_INFO "raspiLedDev: device has been opened\n");
    return 0;
}

static int led_release(struct inode *inode, struct file *filep)
{
    printk(KERN_INFO "raspiLedDev: device successfully closed\n");
    return 0;
}

static ssize_t led_write(struct file *filep, const char *buf, size_t len, loff_t *pos)
{
    unsigned long res;
    GPIO_data gpio_data;
    res = copy_from_user((void *)&gpio_data, buf, len);
    if (res)
    {
        printk(KERN_INFO "raspiLedDev: copy_from_user do not finish\n");
        return -1;
    }

    if (gpio_data.on_off == 0)
    {
        GPIO_LOW(gpio_data.GPIO_port);
    }
    else
    {
        GPIO_HIGH(gpio_data.GPIO_port);
    }
    
    return len;
}

static void __exit led_exit(void)
{
    int i = 0;
    for (i = 2; i < 28; i++)
    {
        GPIO_LOW(i);
    }
    device_destroy(LED_device.ledClass, LED_device.devno);
    class_destroy(LED_device.ledClass);
    cdev_del(&LED_device.cdev);
    unregister_chrdev_region(LED_device.devno, LED_DEV_CNT);
}

module_init(led_init);
module_exit(led_exit);
