#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");

int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "phone_book"
#define BUF_LEN 255
#define MAX_BOOK_SIZE 100

static int Major;
static char msg[BUF_LEN];
static char *msg_Ptr;

static struct file_operations fops = {
        .read = device_read,
        .write = device_write,
        .open = device_open,
        .release = device_release
};

struct phone_book {
    int size;
    char* data[2][MAX_BOOK_SIZE];
};
static struct phone_book book;


int init_module(void)
{
    Major = register_chrdev(0, DEVICE_NAME, &fops);

    if (Major < 0) {
        printk(KERN_ALERT "Registering char device failed with %d\n", Major);
        return Major;
    }

    printk(KERN_INFO "I was assigned major number %d. To talk to\n", Major);
    printk(KERN_INFO "the driver, create a dev file with\n");
    printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, Major);

    msg[0] = '\0';
    msg_Ptr = msg;

    return SUCCESS;
}

void cleanup_module(void)
{
    int i;
    for (i = 0; i < book.size; i++) {
        kfree(book.data[0][i]);
        kfree(book.data[1][i]);
    }

    unregister_chrdev(Major, DEVICE_NAME);
    printk(KERN_INFO "rmmoded phone_book\n");
}

static int device_open(struct inode *inode, struct file *file)
{
    try_module_get(THIS_MODULE);
    return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file)
{
    module_put(THIS_MODULE);
    return 0;
}

static ssize_t device_read(struct file *filp,
                           char *buffer,
                           size_t length,
                           loff_t * offset)
{
    int bytes_read = 0;
    if (*msg_Ptr == 0)
        return 0;
    while (length && *msg_Ptr) {
        put_user(*(msg_Ptr++), buffer++);

        length--;
        bytes_read++;
    }

    return bytes_read;
}

static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
	if (strstr(buff, "add ") == buff) {
        int len2, t;
        buff += 4;
        len2 = len - 4;
        book.size++;
        for (t = 0; t < 2; t++) {
            int sz = 0;
            int i = 0;
            while (sz < len2 && buff[sz] != ' ' && buff[sz] != '\0' && buff[sz] != '\n')
                sz++;
            book.data[t][book.size - 1] = (char*)kmalloc(sz + 1, GFP_KERNEL);
            while (i < sz) {
                book.data[t][book.size - 1][i] = buff[i];
                i++;
            }
            book.data[t][book.size - 1][i] = '\0';
            buff += sz + 1;
            len2 -= sz + 1;
        }

        printk(KERN_INFO "get add, book size = %d\n", book.size);
        return len;
    }
    else if (strstr(buff, "find ") == buff) {
        int len2, sz, i;
        char* word;
        buff += 5;
        len2 = len - 5;
        
        sz = 0;
        while (sz < len2 && buff[sz] != ' ' && buff[sz] != '\0' && buff[sz] != '\n')
            sz++;
        word = (char*)kmalloc(sz + 1, GFP_KERNEL);
        i = 0;
        while (i < sz) {
            word[i] = buff[i];
            i++;
        }
        word[i] = '\0';
        printk(KERN_INFO "word=%s\n", word);

        strcpy(msg, "No match\0");
        msg_Ptr = msg;
        for (i = 0; i < book.size; i++)
            if (strcmp(word, book.data[0][i]) == 0 || strcmp(word, book.data[1][i]) == 0) {
                strcpy(msg, book.data[0][i]);
                msg[strlen(book.data[0][i])] = ' ';
                strcpy(msg + strlen(book.data[0][i]) + 1, book.data[1][i]);
                msg[strlen(book.data[0][i]) + strlen(book.data[1][i]) + 1] = '\0';
                break;
            }

        kfree(word);
        printk(KERN_INFO "get find\n");
        return len;
    }
  
    printk(KERN_WARNING "nothing get:\(\n");
    return len;
}
