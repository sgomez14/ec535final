#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
//#include <linux/proc_fs.h>
#include <linux/interrupt.h> 
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/uaccess.h>
#include <linux/pwm.h>

MODULE_LICENSE("Dual BSD/GPL");

/*Character device and /proc/ variables */
static int witi_kl_major = 61;

/* Declaration of mytraffic functions */
static void witi_kl_exit(void);
static int witi_kl_init(void);

/* Declaration of the module init and exit functions */
module_init(witi_kl_init);
module_exit(witi_kl_exit);

/* Structure that declares the usual file access functions*/
struct file_operations witi_kl_fops = {
    .owner = THIS_MODULE,
};

/* Creating Kernel Land Functions */
static int witi_kl_init(void)
{
    int result;
    
    printk("Hello Main!\n");
    /* Registering device. fops := file operations */
    result = register_chrdev(witi_kl_major, "witi_kl", &witi_kl_fops);
    if (result < 0)
    {
        printk(KERN_ALERT "witi_kl: cannot obtain major number %d\n", witi_kl_major);
        return result;
    }
    
    /* Body Section */

    
    
    printk(KERN_ALERT "\nInserting witi_kl module\n");
    return 0;
    
fail:
    witi_kl_exit();
    return result;
} 

static void witi_kl_exit(void)
{
    /* Freeing the major number */
    unregister_chrdev(witi_kl_major, "witi_kl");


    printk(KERN_ALERT "Removing witi_kl module\n");
}