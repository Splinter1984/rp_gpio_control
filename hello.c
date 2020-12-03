#include <linux/init.h>
#include <linux/module.h>

static int hello_init(void)
{
	printk(KERN_ALERT "Hello, samurai!\n");
	return 0;
}

static void hello_exit(void)
{
	printk(KERN_ALERT "SEE YOUR SPACE SAMURAI\n");
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
MODULE_AUTHOR("SPLINTER1984");
