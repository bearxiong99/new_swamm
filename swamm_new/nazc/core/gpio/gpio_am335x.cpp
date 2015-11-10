//////////////////////////////////////////////////////////////////////
//
//    gpio_am335x.cpp: TI AM335X Serial 용 GPIO Library
//
//
//////////////////////////////////////////////////////////////////////


#define __TI_AM335X__

#ifdef __TI_AM335X__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "gpio_am335x.h"

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define SYSFS_ADC_DIR  "/sys/bus/iio/devices/iio:device0"

int open_value_file(unsigned nNum, unsigned int nState)
{
    int fd;
    char filename[64];

    switch (nState)
    {
        case 0: // GPIO Open Value
            // create file descriptor of value file
            sprintf(filename, SYSFS_GPIO_DIR "/gpio%d/value", nNum);
            //printf( " GPIO open file name : %s \n", filename);            
            break;

        case 1: // ADC Open Value
            // create file descriptor of value file
            sprintf(filename, SYSFS_ADC_DIR "/in_voltage%d_raw", nNum);
            //printf( " ADC open file name : %s \n", filename);
            break;
    }
    if ((fd = open(filename, O_RDONLY | O_NONBLOCK)) < 0)
        return -1;

    return fd;
}


int gpio_export(unsigned gpio)
{
    int fd, len;
    char buf[64];
     //printf(" ****** export ******* \n"); 
    fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd < 0) 
    {
        printf( " Export Error \n"); 
        perror("gpio/export");
        return fd;
    }
    len = snprintf(buf, sizeof(buf), "%d", gpio);
    //printf("buf = %s, len = %d , fd = %d \n",buf, len, fd ); 
    write(fd, buf, len);
    close(fd);
 
    return 0;
}
 
int gpio_unexport(unsigned gpio)
{
    int fd, len;
    char buf[64];
 
    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (fd < 0) {
        perror("gpio/export");
        return fd;
    }
 
    len = snprintf(buf, sizeof(buf), "%d", gpio);
    write(fd, buf, len);
    close(fd);
    return 0;
}
 
int gpio_dir(unsigned gpio, unsigned dir)
{
    int fd/*, len*/;
    char buf[64];
    //int n; 
     //printf("****** gpio dir ****** \n"); 
    snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/direction", gpio); 
    fd = open(buf, O_WRONLY);
    //printf( " buf : %s  fd : %d \n", buf, fd); 
    if (fd < 0) {
        // Error message를 표시하지 않는다
        //perror(buf);
        return fd;
    }

    if (dir == GPIO_DIR_OUT_LOW)
    {
        //printf(" Direction OUT \n");
        write(fd, "out", 3);
    }
    else if (dir == GPIO_DIR_OUT_HIGH)
    {
        //printf(" Direction HIGH \n");
        write(fd, "high", 4);
    }
    else
    {
        //printf(" Direction IN \n"); 
        write(fd, "in", 2);
    }
    //printf( " write return Num = %d \n" , n ); 
    //printf(" %s  dir = %x \n", strerror(errno), dir ); 
    close(fd);
    return 0;
}
 
int gpio_dir_out(unsigned gpio , unsigned option)
{
    switch(option)
    {
        case GPIO_DIR_OUT_HIGH:
        case GPIO_DIR_OUT_LOW:
            return gpio_dir(gpio, option);
    }
    return 0;
}
 
int gpio_dir_in(unsigned gpio)
{
    return gpio_dir(gpio, GPIO_DIR_IN);
}

 
int gpio_set_value(unsigned int gpio, unsigned int value)
 {
     int fd;
     char buf[60];
  
     sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio);
     fd = open(buf, O_WRONLY);
     //printf( " set Val Name : %s  fd : %d \n", buf, fd );    
     if (fd < 0) {
         perror(buf);
         return fd;
     }
  
     if (value)
     {
         //printf(" Set value 1 \n");
        write(fd, "1", 1); 
     }
     else
     {
          //printf(" Set value 0 \n");    
        write(fd, "0", 1);
     }
     close(fd);
     return 0;
 }
 
 /****************************************************************
  * gpio_get_value
  ****************************************************************/
int gpio_get_value(unsigned int gpio,  unsigned int *value)
 {
     int fd;
     char buf[60];
     char ch;
 
     if(value == NULL) return 0;

     sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio);
     fd = open(buf, O_RDONLY);
     //     printf( " get Val Name : %s fd : %d \n", buf, fd);
     if (fd < 0) {
         perror(buf);
         return fd;
     }
  
     read(fd, &ch, 1);
 
     if (ch != '0') {
         *value = 1;
     } else {
         *value = 0;
     }
  
     close(fd);
     return 0;
 }

int adc_get_value(unsigned int adc,  unsigned int *value)
 {
     int fd;
     char buf[128];
 
     if(value == NULL) return 0;

     sprintf(buf, SYSFS_ADC_DIR "/in_voltage%d_raw", adc);
     fd = open(buf, O_RDONLY);
     if (fd < 0) {
         perror(buf);
         return fd;
     }
  
     memset(buf, 0, sizeof(buf));
     if(read(fd, buf, sizeof(buf)))
     {
         *value = strtol(buf, (char**)NULL, 10);
     }
  
     close(fd);
     return 0;
 }


#endif
