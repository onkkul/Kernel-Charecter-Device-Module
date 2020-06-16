#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/errno.h>
#include<linux/init.h>
#include<linux/fs.h>
#include<linux/kernel.h>
#include<linux/list.h>
#include<linux/module.h>
#include<linux/moduleparam.h>
#include<linux/miscdevice.h>
#include<linux/sched.h>
#include<linux/sched/signal.h>
#include<linux/semaphore.h>
#include<linux/slab.h>
#include<linux/types.h>
#include<linux/uaccess.h>


#define BUFFER_SIZE 250
#define DEVICE_NAME "process_list"


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Onkar Kulkarni");
MODULE_DESCRIPTION("Process List Character Device.");

static int Success;             /* Success number assigned to our device driver */
static int Device_Open = 0;     /* Lock to prevent multiple opening of device */
static char data[BUFFER_SIZE];   /* The msg the device will give when asked    */


/* Declaring the functions according to the flow of program */
static int proc_list_open(struct inode*,struct file *);
static ssize_t proc_list_read(struct file *file, char __user * out, size_t size, loff_t * off                                                                                                );
static char* getState(int);
static int proc_list_close(struct inode*, struct file *);


/* Pointer pointing to current process of task_struct. */
static struct task_struct * curr;


/* Structure defining the File operation. */
static struct file_operations process_list_fops =
{
    .owner = THIS_MODULE,
    .open = proc_list_open,
    .read = proc_list_read,
    .release = proc_list_close
};


/* Registers the miscellanous device driver. */
int __init initModle(void)
{

    printk(KERN_INFO "\n---------------------------------------------------\nWelcome to the Dark side of the Force, Luke!\n");
    Success = register_chrdev(0, DEVICE_NAME, &process_list_fops);

    if (Success < 0)
    {
        printk ("Failed to establish contact with the Sith Lord; Error: %d\n", Success);
        return Success;

    }

    printk ("Established contact with the Sith Lord; Major =  %d\n", Success);
    curr=next_task(&init_task);

    return 0;
}


/* Opens the Character Driver. */
static int proc_list_open(struct inode * inode,struct file * file)
{
    if (Device_Open)
    {   return -EBUSY;}

    // increament device opened count to keep a track of open/close connection
    Device_Open++;

    printk(KERN_INFO "Portal to the Dark side is now opened\n");
    curr = next_task(&init_task);

    return 0;
}


/* Read function of the driver. */
static ssize_t proc_list_read(struct file *file, char *out, size_t size, loff_t * off)
{
    int status;
    struct task_struct * task;
    memset(data, 0, sizeof(char)*BUFFER_SIZE);
    printk(KERN_INFO "Sith lord is sending the droid army...\n");

    for_each_process(task)
    {
        if(curr==task)
        {
            char* state=getState(task->state);
            memset(data,0,sizeof(char)*BUFFER_SIZE);
            sprintf(data, "PID - %d  PPID - %d  CPU - %d STATE - %s ", task->pid, task->parent->pid, task_cpu(task), state);
            status = copy_to_user(out, data, strlen(data));

            if(status !=0)
            {
                // Returning error code.
                printk(KERN_ALERT "Error in recieving droid army");
                return -EFAULT;
            }
            curr = next_task(task);
            break;
        }
    }
    return strlen(data);
}


/* To convert state codes into strings */
char* getState(int state)
{
        int counter ;
        bool found;
        const int status_code[25] = {0, 1, 2, 3, 4, 8, 15, 16, 32, 63, 64, 96, 128, 130, 132,                                                                                                                                                                                                                                                                                                 136, 256, 512, 1024, 1026, 2048, 4096};
        const char *status[25] = {"TASK_RUNNING", "TASK_INTERRUPTIBLE", "TASK_UNINTERRUPTIBLE                                                                                                                                                                                                                                                                                                ", "TASK_INTERRUPTIBLE , TASK_UNINTERRUPTIBLE",
                            "__TASK_STOPPED", "__TASK_TRACED", "TASK_NORMAL , __TASK_STOPPED                                                                                                                                                                                                                                                                                                 , __TASK_TRACED", "EXIT_DEAD", "EXIT_ZOMBIE",
                            "TASK_RUNNING , TASK_INTERRUPTIBLE , TASK_UNINTERRUPTIBLE , __TAS                                                                                                                                                                                                                                                                                                K_STOPPED , __TASK_TRACED , EXIT_ZOMBIE , EXIT_DEAD",
                            "TASK_DEAD", "EXIT_ZOMBIE , EXIT_DEAD", "TASK_WAKEKILL", "TASK_WA                                                                                                                                                                                                                                                                                                KEKILL , TASK_UNINTERRUPTIBLE",
                            "TASK_WAKEKILL , __TASK_STOPPED", "TASK_WAKEKILL , __TASK_TRACED"                                                                                                                                                                                                                                                                                                , "TASK_WAKING", "TASK_PARKED",
                            "TASK_NOLOAD", "TASK_UNINTERRUPTIBLE , TASK_NOLOAD", "TASK_NEW",                                                                                                                                                                                                                                                                                                 "TASK_STATE_MAX"};
        for (counter = 0; counter  < 22; counter++)
        {
            if (state == status_code[counter])
            {
                return status[counter];
            }
            found = false;
        }

    if (found == false)
    {
        return "INVALID";
    }
}


/* Called when the driver is to be closed */
static int proc_list_close(struct inode * inode,struct file *file)
{
    // Decrement the usage count, or else
    // once you opened the file, you'll never get get rid of the module.
    Device_Open --;


    printk(KERN_INFO "Dark forces have arrived in the new world\n---------------------------------------------------\n");

    // reset the current pointer.
    curr = &init_task;
    return 0;
}


/* exit function which deregisters the device driver. */
static void __exit cleanupModule(void)
{
   unregister_chrdev(Success, DEVICE_NAME);
}


/* Register init and exit process */
module_init(initModle);
module_exit(cleanupModule);
