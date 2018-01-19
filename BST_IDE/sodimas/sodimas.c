#include <linux/init.h>
#include <linux/slab.h>
#include <asm/errno.h>
#include <asm/delay.h>
#include <linux/mm.h>
#include <linux/poll.h>
#include <linux/module.h>
#include <asm/hardware.h>
#include <asm/io.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <asm/arch/irqs.h>
#include <linux/interrupt.h>
#include <linux/completion.h>
#include <asm/arch/w55fa95_reg.h>
#include <asm/arch/regs-clock.h>
#include <asm/arch/w55fa95_hmi.h>

#define HMI_IRQ_NUM             W55FA95_IRQ(3)  // nIRQ1
#define HMI_ED01	0
#define HMI_EFLM	1
#define HMI_ESGO	2
#define HMI_EFLD	3
#define HMI_ESF		4
#define HMI_ETALK	5
#define MINUS      104
typedef struct
{
    // no spmaphore protect for data here. seems not necessary for now.
    u32 num;
    u32 opened;
    u32 status;
} sysmgr_data;

volatile unsigned int old_talk_status = 0;
volatile unsigned int old_sgo_status = 0;

extern unsigned int w55fa95_external_clock;

static struct cdev hmi_dev[1];
static sysmgr_data sdata[1];

static struct timer_list hmi_timer[2];
static volatile char timer_active=0;
static volatile unsigned int hmi_value[6];
static volatile char hmi_count[6];
static volatile unsigned int hmi_state[6];

static volatile unsigned int old_timer, new_timer, timer_value;
static volatile unsigned int ED01_count=0;
static volatile char ED01_valid=1;

static DECLARE_WAIT_QUEUE_HEAD(button_waitq);
volatile int ev_press = 0;
volatile unsigned char hmi_framebuffer[12] = {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81};
volatile int hmi_framebuffer_changed = 0;

static void read_ED01(void)
{
    volatile unsigned int status;

    status = inl(REG_GPIOG_PIN) & 0x04;

    if(status != 0)
        ED01_valid = 0;

    hmi_count[HMI_ED01] = hmi_count[HMI_ED01] +1;

    if(hmi_count[HMI_ED01] == 3)
    {
        del_timer(&hmi_timer[HMI_ED01]);
        timer_active = 0;

        if(ED01_valid == 1)
        {
            if(hmi_value[HMI_ED01] != ED01_count)
            {
                hmi_value[HMI_ED01] = ED01_count;

                hmi_framebuffer_changed = 1;
                if(ED01_count == 20)
                {
                    hmi_framebuffer_changed = 0;
                }
                else if(ED01_count == 70)
                {
                    hmi_framebuffer[1] = hmi_framebuffer[1] | (1<<6);
                    hmi_framebuffer[1] = hmi_framebuffer[1] & (~((1<<5)|(1<<4)));
                    hmi_framebuffer[9] = hmi_framebuffer[9] | (1);  // 0位表示楼层改变，1位表示基础功能改变，2位表示功能改变
                    hmi_framebuffer[2] = 0;
                    hmi_framebuffer[3] = 0;
                }
                else if(ED01_count < 40 || ((ED01_count >=60) && (ED01_count<=68)))
                {
                    hmi_framebuffer[1] = hmi_framebuffer[1] | (1<<6);
                    hmi_framebuffer[1] = hmi_framebuffer[1] & (~((1<<5)|(1<<4)));
                    hmi_framebuffer[9] = hmi_framebuffer[9] | (1);  // 0位表示楼层改变，1位表示基础功能改变，2位表示功能改变
                    volatile unsigned int newValue = ED01_count;
                    volatile unsigned char tmpvalue;

                    tmpvalue = newValue % 100 / 10;
                    if(tmpvalue == 2)
                        hmi_framebuffer[2] = 3;
                    else if(tmpvalue == 3)
                        hmi_framebuffer[2] = 2;
                    else
                        hmi_framebuffer[2] = tmpvalue;
                    hmi_framebuffer[3] = newValue % 10;
                }
                else
                {
                    hmi_framebuffer[9] = hmi_framebuffer[9] | (1<<2);  // 0位表示楼层改变，1位表示基础功能改变，2位表示功能改变
                    hmi_framebuffer[1] = 1<<6;
                    hmi_framebuffer[4] = hmi_framebuffer[4] & ((1<<3) | (1<<2) | (1<<1) | 1); // 1<<1 :滚动信号只能由53帧清零
                    hmi_framebuffer[5] = 0;
                    hmi_framebuffer[6] = 0;
                    switch(ED01_count)
                    {
                        case 40:
                        {
                            hmi_framebuffer[5] = hmi_framebuffer[5] | (1<<4); //ŒìÐÞ(inspection)
                            break;
                        }
                        case 41:
                        {
                            hmi_framebuffer[4] = hmi_framebuffer[4] | (1<<6); //œûÈë(Access forbidden)
                            break;
                        }
                        case 44:
                        {
                            hmi_framebuffer[5] = hmi_framebuffer[5] | (1<<2);//³¬ÔØ(overload)
                            break;
                        }
                        case 45:
                        {
                            hmi_framebuffer[5] = hmi_framebuffer[5] | (1<<3);//ÂúÔØ(call full)
                            break;
                        }
                        case 46:
                        {
                            hmi_framebuffer[5] = hmi_framebuffer[5] | (1<<5); //Ïû·À(fire mode)
                            break;
                        }
                        case 47:
                        {
                            hmi_framebuffer[6] = hmi_framebuffer[6] | (1<<3); //lifiter
                            break;
                        }
                        case 48:
                        {
                            hmi_framebuffer[4] = hmi_framebuffer[4] | (1<<4); //CLEEULE(cell)
                            break;
                        }
                        case 52:
                        {
                            hmi_framebuffer[4] = hmi_framebuffer[4] | (1<<1); //¹ö¶¯¿ªÊŒ(Arrow scrolling on)
                            break;
                        }
                        case 53:
                        {
                            hmi_framebuffer[4] = hmi_framebuffer[4] & (~(1<<1)); //¹ö¶¯Í£Ö¹(Arrow scrolling off)
                            break;
                        }
                        case 56:
                        {
                            hmi_framebuffer[4] = hmi_framebuffer[4] | (1<<5); //Ç¿¹Ø(Forced closing)
                            break;
                        }
                        case 57:
                        {
                            hmi_framebuffer[1] = hmi_framebuffer[1] | (1<<1); //baraint
                            break;
                        }
                        case 58:
                        {
                            hmi_framebuffer[1] = hmi_framebuffer[1] | 1; //code
                            break;
                        }
                        case 59:
                        {
                            hmi_framebuffer[1] = hmi_framebuffer[1] | (1<<5); //door close
                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }
                }
            }
        }
        hmi_count[HMI_ED01] = 0;
        ED01_valid = 1;
        ED01_count = 0;

        if(hmi_framebuffer_changed)
        {
            wake_up_interruptible(&button_waitq); // »œÐÑÐÝÃßµÄœø³Ì //
            ev_press =1;
        }
    }
    else
    {
        mod_timer(&hmi_timer[HMI_ED01], jiffies + 3);
        timer_active = 1;
    }
}

static void read_flm()
{
   volatile unsigned int status = inl(REG_GPIOG_PIN) & 0x00000008;
   if(hmi_count[HMI_EFLM] == 0)
   {
       if(status != 0)
           hmi_state[HMI_EFLM] = status;
       else
           hmi_state[HMI_EFLM] = 0;

       hmi_count[HMI_EFLM] = 1;
   }
   else
   {
       if(status == hmi_state[HMI_EFLM])
           hmi_count[HMI_EFLM] = hmi_count[HMI_EFLM] +1;
       else
           hmi_count[HMI_EFLM] = 0;

       if(hmi_count[HMI_EFLM] == 3)
       {
           if(status != 0)
           {
               volatile unsigned char tempvalue = hmi_framebuffer[4] & (1<<3);
               if(tempvalue)
               {
                   hmi_framebuffer[4] = hmi_framebuffer[4] & ~(1<<3);
                   hmi_framebuffer_changed = 1;
               }
           }
           else
           {
               volatile unsigned char tempvalue = hmi_framebuffer[4] & (1<<3);
               if(tempvalue == 0)
               {
                   hmi_framebuffer[4] = hmi_framebuffer[4] | (1<<3);
                   hmi_framebuffer_changed = 1;
               }
           }
           hmi_count[HMI_EFLM] = 0;
       }
   }
}

static void read_fld()
{
   volatile unsigned int status = inl(REG_GPIOG_PIN) & 0x00000020;
   if(hmi_count[HMI_EFLD] == 0)
   {
       if(status != 0)
           hmi_state[HMI_EFLD] = status;
       else
           hmi_state[HMI_EFLD] = 0;

       hmi_count[HMI_EFLD] = 1;
   }
   else
   {
       if(status == hmi_state[HMI_EFLD])
           hmi_count[HMI_EFLD] = hmi_count[HMI_EFLD] +1;
       else
           hmi_count[HMI_EFLD] = 0;

       if(hmi_count[HMI_EFLD] == 3)
       {
            if(status != 0)
            {
                volatile unsigned char tempvalue = hmi_framebuffer[4] & (1<<2);
               if(tempvalue)
               {
                   hmi_framebuffer[4] = hmi_framebuffer[4] & ~(1<<2);
                   hmi_framebuffer_changed = 1;
               }
            }
            else
            {
               volatile unsigned char tempvalue = hmi_framebuffer[4] & (1<<2);
               if(tempvalue == 0)
               {
                   hmi_framebuffer[4] = hmi_framebuffer[4] | (1<<2);
                   hmi_framebuffer_changed = 1;
               }
            }

            hmi_count[HMI_EFLD] = 0;
       }
   }
}

static void read_sgo()
{
   unsigned int status = inl(REG_GPIOG_PIN) & 0x00000010;
   if(hmi_count[HMI_ESGO] == 0)
   {
       if(status != 0)
           hmi_state[HMI_ESGO] = status;
       else
           hmi_state[HMI_ESGO] = 0;

       hmi_count[HMI_ESGO] = 1;
   }
   else
   {
       if(status == hmi_state[HMI_ESGO])
           hmi_count[HMI_ESGO] = hmi_count[HMI_ESGO] +1;
       else
           hmi_count[HMI_ESGO] = 0;

       if(hmi_count[HMI_ESGO] == 3)
       {
           if(status)
           {
               if((hmi_framebuffer[4] & (1<<0)))
               {
                   hmi_framebuffer[4] = hmi_framebuffer[4] & (~1);
                   //hmi_framebuffer_changed = 1;
                   hmi_framebuffer[10] = hmi_framebuffer[10] | (1);  // 0位表示SGO 1
               }
           }
           else
           {
               if((hmi_framebuffer[4] & (1<<0)) == 0)
               {
                  hmi_framebuffer[4] = hmi_framebuffer[4] | 1;
                  hmi_framebuffer_changed = 1;
               }
           }

           /*if(status != old_sgo_status)
           {
               old_sgo_status = status;
               if(status==0)  //>@up
               {
                   hmi_framebuffer[4] = hmi_framebuffer[4] | (1);
                   hmi_framebuffer_changed = 1;

                   hmi_framebuffer[10] = hmi_framebuffer[10] | (1);  // 0位表示SGO 1
               }
               else
               {
                    hmi_framebuffer[10] = hmi_framebuffer[10] & ~(1);  // 0位表示SGO 1
               }
           }
           else
           {
               hmi_framebuffer[4] = hmi_framebuffer[4] & ~(1);
           }*/
           hmi_count[HMI_ESGO] = 0;
       }
   }
}

static void read_esf() {}


static void read_talk()
{
   unsigned int status = inl(REG_GPIOA_PIN) & 0x00000010;
   if(hmi_count[HMI_ETALK] == 0)
   {
       if(status != 0)
           hmi_state[HMI_ETALK] = status;
       else
           hmi_state[HMI_ETALK] = 0;

       hmi_count[HMI_ETALK] = 1;
   }
   else
   {
       if(status == hmi_state[HMI_ETALK])
           hmi_count[HMI_ETALK] = hmi_count[HMI_ETALK] +1;
       else
           hmi_count[HMI_ETALK] = 0;

       if(hmi_count[HMI_ETALK] == 3)
       {
/*           if(status != 0)
           {
               volatile unsigned char tempvalue = hmi_framebuffer[6] & (1<<6);
               if(tempvalue)
               {
                  hmi_framebuffer[6] = hmi_framebuffer[6] | (1<<6);
                   hmi_framebuffer_changed = 1;
               }
           }
           else
           {
              volatile unsigned char tempvalue = hmi_framebuffer[6] & (1<<6);
               if(tempvalue == 0)
               {
                  hmi_framebuffer[6] = hmi_framebuffer[6] & (~(1<<6));
                  //hmi_framebuffer_changed = 1;   //>@其下降沿不处理任何事，则屏蔽
               }
           }*/
           if(status != old_talk_status)
           {
               old_talk_status = status;
               if(status)  //>@up
               {
                   hmi_framebuffer[6] = hmi_framebuffer[6] | (1<<6);
                   hmi_framebuffer_changed = 1;
               }
           }
           else
           {
               hmi_framebuffer[6] = hmi_framebuffer[6] & (~(1<<6));
           }
           hmi_count[HMI_ETALK] = 0;
       }
   }
}

static void read_low_speed_state(void)
{
    read_flm();
    read_sgo();
    read_fld();
    read_esf();
    read_talk();
    if(hmi_framebuffer_changed==1)
    {
        wake_up_interruptible(&button_waitq); // »œÐÑÐÝÃßµÄœø³Ì //
        ev_press =1;
    }

    mod_timer(&hmi_timer[HMI_EFLM], jiffies + 3);
}

static irqreturn_t w55fa95_hmi_irq(int irq, void *dev_id, struct pt_regs *regs) {

    u32 src;
    src = inl(REG_IRQTGSRC2);

    outl(1 << HMI_IRQ_NUM, REG_AIC_MDCR);

    if(src & 0x040000)		//ED01
    {
        if(inl(REG_GPIOG_PIN) & 0x04)
        {
            if(timer_active == 1)
            {
                del_timer(&hmi_timer[HMI_ED01]);
                timer_active = 0;

                old_timer = new_timer;
                new_timer = inl(REG_TDR1);

                if(new_timer > old_timer)
                {
                    timer_value = new_timer - old_timer;
                    if((timer_value < 3000) && (timer_value > 7000))
                        ED01_valid = 0;
                }
                else
                {
                    timer_value = 1000000 - old_timer + new_timer;
                    if((timer_value < 3000) && (timer_value > 7000))
                        ED01_valid = 0;
                }
            }
            else
            {
                new_timer = inl(REG_TDR1);	//frame start
                hmi_count[HMI_ED01] = 0;
            }

        }
        else
        {
            old_timer = new_timer;
            new_timer = inl(REG_TDR1);

            if(new_timer > old_timer)
            {
                timer_value = new_timer - old_timer;
                if((timer_value >= 3000) && (timer_value <= 7000))
                {
                    ED01_count++;
                }
                else
                {
                    ED01_valid = 0;
                }

            }
            else
            {
                timer_value = 1000000 - old_timer + new_timer;
                if((timer_value >= 3000) && (timer_value <= 7000))
                {
                    ED01_count++;
                }
                else
                {
                    ED01_valid = 0;
                }
            }

            mod_timer(&hmi_timer[HMI_ED01], jiffies + 3);
            timer_active = 1;
        }
    }

    // clear source
    outl(src & 0x00040000, REG_IRQTGSRC2);

    return IRQ_HANDLED;
}

int w55fa95_hmi_open(struct inode* i,struct file* file)
{
    sdata[MINOR(i->i_rdev)].opened++;
    file->private_data = &sdata[MINOR(i->i_rdev)];

    if(sdata[MINOR(i->i_rdev)].opened == 1)
    {
         // init GPIO
            // PORTG[2,3,4,5]
            outl(inl(REG_SHRPIN_TVDAC) & ~(SMTVDACAEN), REG_SHRPIN_TVDAC);
            outl(inl(REG_GPGFUN) & ~(MF_GPG2 | MF_GPG3 | MF_GPG4 | MF_GPG5), REG_GPGFUN);

            outl(inl(REG_GPIOG_OMD) & ~((1 << 2) | (1 << 3) | (1 << 4) | (1 << 5)), REG_GPIOG_OMD); // input
            outl(inl(REG_GPIOG_PUEN) & ~((1 << 2) | (1 << 3) | (1 << 4) | (1 << 5)), REG_GPIOG_PUEN); // pull-up
            outl((inl(REG_IRQSRCGPG) & ~(0x30)) |0x10, REG_IRQSRCGPG); // GPG[2] as nIRQ1 source

        // PORTA[3,4]
        outl(inl(REG_GPAFUN) & ~(MF_GPA3 | MF_GPA4), REG_GPAFUN);

         outl(inl(REG_GPIOA_OMD) & ~((1 << 3) | (1 << 4)), REG_GPIOA_OMD); // input
            outl(inl(REG_GPIOA_PUEN) |((1 << 3) | (1 << 4)), REG_GPIOA_PUEN); // pull-up

        outl(inl(REG_DBNCECON) |0x72, REG_DBNCECON);

        outl((1 << HMI_IRQ_NUM),  REG_AIC_SCCR); // force clear previous interrupt, if any.
            outl(inl(REG_IRQTGSRC2) & 0x00040000, REG_IRQTGSRC2); // clear source

        memset(hmi_value, 0, sizeof(hmi_value));
        memset(hmi_count, 0, sizeof(hmi_count));
        memset(hmi_state, 0, sizeof(hmi_state));

        outl(inl(REG_APBCLK) | TMR1_CKE, REG_APBCLK);
        outl(0, REG_TCSR1);
        outl(0x02, REG_TISR);  /* clear for safty */
        outl(w55fa95_external_clock/12, REG_TICR1);
        outl(0x4801000B, REG_TCSR1);

        init_timer(&hmi_timer[HMI_ED01]);
            hmi_timer[HMI_ED01].function = read_ED01;	/* timer handler */

        init_timer(&hmi_timer[HMI_EFLM]);
            hmi_timer[HMI_EFLM].function = read_low_speed_state;

            if (request_irq(HMI_IRQ_NUM, w55fa95_hmi_irq, SA_INTERRUPT, "hmi",NULL) != 0) {
                    printk("register the hmi_irq failed!\n");
                    return -1;
            }

        mod_timer(&hmi_timer[HMI_EFLM], jiffies + 3);
        outl(inl(REG_IRQENGPG) | 0x00040004, REG_IRQENGPG); //enable falling/rising edge trigger
    }

    return 0;
}

int w55fa95_hmi_close(struct inode* i,struct file* file)
{
    int num;

    sdata[(num = MINOR(i->i_rdev))].opened--;
    if (sdata[num].opened == 0)
    {
        sdata[num].status = 0;

        //disable falling/rising edge triggers
        outl((inl(REG_IRQENGPG)& ~(0x00040004)), REG_IRQENGPG);
        free_irq(HMI_IRQ_NUM,NULL);
        del_timer(&hmi_timer[HMI_EFLM]);
    }

    return 0;
}

ssize_t w55fa95_hmi_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    int ret = -EFAULT;
    if(hmi_framebuffer_changed == 0)
    {
        wait_event_interruptible(button_waitq, ev_press);
        ev_press = 0 ;
    }
    if(hmi_framebuffer_changed && (count <= 12))
    {
        if(copy_to_user(buf, (const void *)hmi_framebuffer, count) == 0)
        {
            ret = count;
        }
    }
    hmi_framebuffer[6] = hmi_framebuffer[6] & (~(1<<6));
    hmi_framebuffer[9] = 0;  // 0位表示楼层改变，1位表示基础功能改变，2位表示功能改变
    hmi_framebuffer[10] = hmi_framebuffer[10] & ~(1);  // 0位表示SGO 1
    hmi_framebuffer_changed = 0;
    return ret;
}

/*static int w55fa95_hmi_ioctl(struct inode *inode, struct file *file, unsigned int cmd, void *arg)
{
    unsigned int cmd_data, state;
    int ret = 0;

    switch (cmd) {

    case HMI_IOC_GET_ED01_COUNT:
        if (copy_to_user((void *)arg, (void *)&hmi_value[HMI_ED01], sizeof(unsigned int))) {
            ret = -EFAULT;
            break;
        }
        break;

    case HMI_IOC_GET_EFLM_STATE:
        if (copy_to_user((void *)arg, (void *)&hmi_value[HMI_EFLM], sizeof(unsigned int))) {
            ret = -EFAULT;
            break;
        }
        break;

    case HMI_IOC_GET_ESGO_STATE:
        if (copy_to_user((void *)arg, (void *)&hmi_value[HMI_ESGO], sizeof(unsigned int))) {
            ret = -EFAULT;
            break;
        }
        break;

    case HMI_IOC_GET_EFLD_STATE:
        if (copy_to_user((char *)arg, (void *)&hmi_value[HMI_EFLD], sizeof(char))) {
            ret = -EFAULT;
            break;
        }
        break;

    case HMI_IOC_GET_ESF_STATE:
        if (copy_to_user((char *)arg, (void *)&hmi_value[HMI_ESF], sizeof(char))) {
            ret = -EFAULT;
            break;
        }
        break;

    case HMI_IOC_GET_ETALK_STATE:
        if (copy_to_user((char *)arg, (void *)&hmi_value[HMI_ETALK], sizeof(char))) {
            ret = -EFAULT;
            break;
        }
        break;

    default:
        ret = -ENOIOCTLCMD;
    }

    return ret;
}
*/

struct file_operations w55fa95_hmi_fops = {
owner:
    THIS_MODULE,
open:
    w55fa95_hmi_open,
/*ioctl:
    w55fa95_hmi_ioctl,
*/
release:
    w55fa95_hmi_close,
read:
    w55fa95_hmi_read,
};


static int __init w55fa95_hmi_reg(void)
{
    dev_t dev = MKDEV(HMI_MAJOR, HMI_MINOR);

    if (register_chrdev_region(dev, 1, "hmi")) {
        printk("HMI initial the device error!\n");
        return -1;
    }

    cdev_init(&hmi_dev[0], &w55fa95_hmi_fops);
    hmi_dev[0].owner = THIS_MODULE;
    hmi_dev[0].ops = &w55fa95_hmi_fops;

    if (cdev_add(&hmi_dev[0], dev, 1))
    printk("Error adding w55fa95 Sysmgr\n");

    memset(sdata, 0, sizeof(sdata));
    sdata[0].num = 0;

    printk("w55fa95 HMI driver has been initialized successfully!\n");

    return 0;
}

static void __exit w55fa95_hmi_exit(void)
{
    dev_t devno = MKDEV(HMI_MAJOR, HMI_MINOR);

    cdev_del(&hmi_dev[0]);

    unregister_chrdev_region(devno, 1);
}

module_init(w55fa95_hmi_reg);
module_exit(w55fa95_hmi_exit);

