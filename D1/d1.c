#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>

#define SUCCES 0

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Remigiusz Kozdra");
MODULE_DESCRIPTION("Character Device");
MODULE_VERSION("0.1");


// zmienna Z
int z;

int32_t value = 0;
char etx_array[20]="try_proc_array\n";
static int len = 1;
static short size_of_message; 
static char message[256] = {0}; 

static int Major;
char msg[100];
char *msg_ptr;

/***************** Driver Functions *******************/

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops = {
  .read    =  device_read,
  .write   =  device_write,
  .open    =  device_open,
  .release =  device_release
};

/***************** Procfs Functions *******************/
static int open_proc(struct inode *inode, struct file *file);
static int release_proc(struct inode *inode, struct file *file);
static ssize_t read_proc(struct file *filp, char __user *buffer, size_t length,loff_t * offset);
static ssize_t write_proc(struct file *filp, const char *buff, size_t len, loff_t * off);
 
 
static struct file_operations proc_fops = {
        .open = open_proc,
        .read = read_proc,
        .write = write_proc,
        .release = release_proc
};
 
static int open_proc(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "proc file opend.....\t");
    return 0;
}
 
static int release_proc(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "proc file released.....\n");
    return 0;
}
 
static ssize_t read_proc(struct file *filp, char __user *buffer, size_t length,loff_t * offset)
{
    char tab[255] = {0};
    printk(KERN_INFO "proc file read.....\n");
    if(len)
        len=0;
    else{
        len=1;
        return 0;
    }
    sprintf(tab,"z= %d",z);
    copy_to_user(buffer,tab,sizeof(int));
 
    return length;;
}
 
static ssize_t write_proc(struct file *filp, const char *buff, size_t len, loff_t * off)
{
    printk(KERN_INFO "proc file wrote.....\n");
    copy_from_user(etx_array,buff,len);
    kstrtoint(buff, 10, &z);
    z = (z*z) % 1024;
    return len;
}
// *** DRIVER FUNCTIONS ***

int init_module(void){
  Major = register_chrdev(0, "mydevice", &fops);
  if(Major < 0){
    printk(KERN_ERR "rejestracja sie nie udala - Major = %d\n", Major);
  return Major;
  }
  // create proc in procfs
  proc_create("D1",0666,NULL,&proc_fops);
  printk(KERN_INFO "zarejestrowano mydevice z numerem Major = %d\n", Major);
  return 0;
}

void cleanup_module(void){
  remove_proc_entry("D1", NULL);
  unregister_chrdev(Major,"mydevice");
}

static int device_open(struct inode * inode, struct file * f)
{
  static int counter = 0;
  sprintf(msg, "wysylam ten tekst %d raz \n", counter++);
  msg_ptr = msg;
  try_module_get(THIS_MODULE);
  return SUCCES;
}

static int device_release(struct inode * inode, struct file * f)
{
  module_put(THIS_MODULE);
  return SUCCES;
}

static ssize_t  device_read(struct file * filp, char * buff, size_t len,loff_t * off)
{
  int bytes_read = 0;
  if(msg_ptr == 0) return 0;
  while(*msg_ptr && len){
    put_user(*(msg_ptr++), buff++);
    len--;
    bytes_read++;
  }
  return bytes_read;
}

static ssize_t device_write(struct file * filp, const char * buff, size_t len,loff_t * off)
{
  sprintf(message, "%s(%zu letters)", buff, len);   // appending received string with its length
  size_of_message = strlen(message);                 // store the length of the stored message
  kstrtoint(buff, 10, &z);
  printk(KERN_INFO "D1: z=%d\n", z);
  return len;
}


// module_init(init_module);
// module_exit(cleanup_module);
// sudo insmod lkm1.ko
// dmesg    - zanotuj numer major
// mknod /dev/mydevice c 238 0
// lsmod
// cat /dev/myde
// sudo rmmod lkm1
