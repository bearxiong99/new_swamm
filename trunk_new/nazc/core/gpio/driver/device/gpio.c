#ifndef __KERNEL__
#define __KERNEL__
#endif

#ifndef MODULE
#define MODULE
#endif

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/ioport.h>
#include <linux/slab.h>     // kmalloc() 
#include <linux/poll.h>     // poll
#include <linux/proc_fs.h>
#include <asm/system.h>     // cli(), *_flags 
#include <asm/irq.h>
#include <asm/ioctl.h>
#include <asm/unistd.h>
#include <asm/io.h>

#include "gpiomap.h"

/* 로컬변수 정의 */

static int usage = 0;
static int irq_pending = 0;
static int major = GPIO_MAJOR_DEF;
static wait_queue_head_t	waitqueue;

MODULE_LICENSE("GPL");
MODULE_PARM(major , "i");
MODULE_PARM(showmsg, "i");
MODULE_AUTHOR("aimir@nuritelecom.com");
MODULE_DESCRIPTION("concentrator gpio driver");
MODULE_SUPPORTED_DEVICE("nzc");
EXPORT_NO_SYMBOLS;

/*-----------------------------------------------------------------------------
	추가할 인터럽트를 여기에 등록한다.
-------------------------------------------------------------------------------*/

static GPIOINTENTRY arInterrupt[] =
{
	{ GP_LOW_BATT_INPUT, 		GPIO_BOTH_EDGES },
	{ GP_GSM_DCD_INPUT,			GPIO_BOTH_EDGES },
    { GP_GSM_RI_INPUT,			GPIO_BOTH_EDGES },
	{ GP_PWR_FAIL_INPUT,		GPIO_BOTH_EDGES },
	{ GP_TEMP_OS_INPUT,			GPIO_BOTH_EDGES },
	{ GP_DOOR_OPEN_INPUT,		GPIO_BOTH_EDGES },
	{ GP_BATT_CHG_STATUS_INPUT,	GPIO_BOTH_EDGES },
	{ -1, -1 }
};

// 인터럽트가 발생될때 호출되는 함수
static void gpioInterrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	printk(KERN_DEBUG "gpio interrupt = %d\n", irq);
	irq_pending++;
	wake_up_interruptible(&waitqueue);
}

// 어플리케이션에서 디바이스를  open 하였을때 호출되는 함수
static int gpio_open(struct inode *inode, struct file *filp)
{
	MOD_INC_USE_COUNT;      
	usage++;
	return 0;
}

// 어플리케이션에서 디바이스를  close를 하였을때 호출되는 함수
static int gpio_release(struct inode *inode, struct file *filp)
{
	MOD_DEC_USE_COUNT;      
	usage--;          
	return 0;
}

static unsigned int gpio_poll(struct file *filp, struct poll_table_struct *wait)
{
	unsigned int	mask = 0;
	
//	printk(KERN_DEBUG "poll_in\n");
	poll_wait(filp, &waitqueue, wait);
//	printk(KERN_DEBUG "poll_out\n");

	if (irq_pending > 0) 
	{
		mask |= POLLIN | POLLRDNORM;
		irq_pending = 0;
	}
	else mask |= POLLOUT | POLLWRNORM;
	return mask;
}

static int gpio_dir_func(unsigned int cmd)
{
    int	gpio_num;

    gpio_num = get_gpio_num(cmd);

    if (0 > gpio_num) 
    {
        printk(KERN_ERR "%s() : cannot get gpio number[gpio_%d].\n", __FUNCTION__, gpio_num);
        return -EINVAL;
    }

	set_GPIO_mode(gpio_num | get_direction(cmd));
	return 0;
}

static int gpio_in_func(unsigned int cmd)
{
	unsigned long regv;
	int 	      gnum;
	int 	      read_val;
	
	gnum = get_gpio_num(cmd);
	if (0 > gnum)
		return -EINVAL;

	regv 	 = GPLR(gnum);
	read_val = ((regv & GPIO_bit(gnum)) ? 1 : 0);
	return read_val;
}

static int gpio_out_func(unsigned int cmd)
{
	int 	gnum;
	
	gnum = get_gpio_num(cmd);
	if (0 > gnum)
		return -EINVAL;

	if (is_high_level(cmd))
		 GPSR(gnum) = GPIO_bit(gnum);
	else GPCR(gnum) = GPIO_bit(gnum);
	return 0;
}

static int gpio_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	switch(cmd) {
      case GPIO_IOCTL_DIR : return gpio_dir_func( arg );
      case GPIO_IOCTL_IN  : return gpio_in_func ( arg );
      case GPIO_IOCTL_OUT : return gpio_out_func( arg );
    }

    return - EINVAL;
}

static int gpio_read_proc(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int len;
	char *p = page;
	
	p += sprintf(p, " version %s (major=%d)\n", GPIO_VERSION, major);
	p += sprintf(p, " parm : showmsg, major\n" );
	p += sprintf(p, " proc : dir=, out=\n" );
	p += sprintf(p, "        dir_out=0x80nn, gpio_high=0x40nn  (nn:gpio-num)\n" );
	p += sprintf(p, " GPLR0=0x%08x  GPLR1=0x%08x  GPLR2=0x%08x\n", GPLR0, GPLR1, GPLR2 );
	p += sprintf(p, " GPDR0=0x%08x  GPDR1=0x%08x  GPDR2=0x%08x\n", GPDR0, GPDR1, GPDR2 );
	p += sprintf(p, " GPSR0=0x%08x  GPSR1=0x%08x  GPSR2=0x%08x\n", GPSR0, GPSR1, GPSR2 );
	p += sprintf(p, " GPCR0=0x%08x  GPCR1=0x%08x  GPCR2=0x%08x\n", GPCR0, GPCR1, GPCR2 );
	p += sprintf(p, " GRER0=0x%08x  GRER1=0x%08x  GRER2=0x%08x\n", GRER0, GRER1, GRER2 );
	p += sprintf(p, " GFER0=0x%08x  GFER1=0x%08x  GFER2=0x%08x\n", GFER0, GFER1, GFER2 );
	p += sprintf(p, " GEDR0=0x%08x  GEDR1=0x%08x  GEDR2=0x%08x\n", GEDR0, GEDR1, GEDR2 );
	p += sprintf(p, " GAFR0_L=0x%08x  GAFR0_U=0x%08x  GAFR1_L=0x%08x\n", GAFR0_L, GAFR0_U, GAFR1_L );
	p += sprintf(p, " GAFR1_U=0x%08x  GAFR2_L=0x%08x  GAFR2_U=0x%08x\n", GAFR1_U, GAFR2_L, GAFR2_U );

	// ----------------------------
	len = ( p - page );
	if (len <= off+count) *eof = 1;
	*start = page + off;
	len -= off;
	if (len>count) len = count;
	if (len<0) len = 0;
	return len;
}

static int gpio_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char  kbuf[128];
	int   len, val;
	
	// 어플버퍼의 내용을 복사한다.
	len = count;
	if (len > 128) len = 128;
	memset( kbuf, 0, len );
	copy_from_user( kbuf, buffer, len );
	
	if ( 0 == strncmp( "dir=", kbuf, 4 ) )
	{
		val = simple_strtoul( &kbuf[4],NULL,0 );
		gpio_dir_func( val & 0xffff );
	}
	else if ( 0 == strncmp( "out=", kbuf, 4 ) )
	{
		val = simple_strtoul( &kbuf[4],NULL,0 );
		gpio_out_func( val & 0xffff );
	}

	return count;
}

/*-----------------------------------------------------------------------------
  설 명 : proc 를 등록한다.
-------------------------------------------------------------------------------*/
static void gpio_reg_proc( void )
{
	static struct proc_dir_entry *ent;
	
	ent = create_proc_entry("driver/gpio", 0, 0);
	ent->read_proc  = gpio_read_proc;
	ent->write_proc = gpio_write_proc;
}

/*-----------------------------------------------------------------------------
  설 명 : proc 를 해제한다.
-------------------------------------------------------------------------------*/
static void gpio_unreg_proc( void )
{
	remove_proc_entry( "driver/gpio", NULL );
}

/*-----------------------------------------------------------------------------
  드라이버에 사용되는 접근 함수에 대한 함수 포인터 구조체를 정의 한다.
  이 구조체는 fs.h에 정의 되어 있다.
-------------------------------------------------------------------------------*/

static struct file_operations gpio_fops =
{
        open    : gpio_open, 
        release : gpio_release, 
        ioctl   : gpio_ioctl,
		poll	: gpio_poll,
};

static __init int gpio_init( void )
{
	int 	i, retval;

	// Init Wait Queue
	init_waitqueue_head(&waitqueue);

	// Set Interrupt 
	major &= 0xff;
	GPDR1 &= ~(1<<4);

	for(i=0; arInterrupt[i].gpio_nr != -1; i++)
	{
		set_GPIO_IRQ_edge(arInterrupt[i].gpio_nr, arInterrupt[i].edge_mask);
		retval = request_irq(IRQ_GPIO(arInterrupt[i].gpio_nr), gpioInterrupt, SA_INTERRUPT, GPIO_NAME, NULL);
		if (retval < 0)
			return -ENODEV;
	}

	if (register_chrdev(major, GPIO_NAME, &gpio_fops) > 0)
	{        
		printk(KERN_ALERT "unable to get major %d for %s \n", major, GPIO_NAME);
		return -EBUSY;
	}

	printk(KERN_NOTICE "%s (major=%d) Register OK\n", GPIO_VERSION, major);
	gpio_reg_proc();
	return 0;		
}

static __exit void gpio_free( void )
{
	int		i;

	gpio_unreg_proc();
	for(i=0; arInterrupt[i].gpio_nr != -1; i++)
		free_irq(IRQ_GPIO(arInterrupt[i].gpio_nr), NULL);
	unregister_chrdev(major, GPIO_NAME);

	printk(KERN_ALERT "Unregister %s OK\n", GPIO_NAME);
}

module_init(gpio_init);
module_exit(gpio_free);
