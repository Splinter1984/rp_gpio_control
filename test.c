#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <getopt.h>

#include "raspi_led.h"

GPIO_data gpio_data;

int main(){
    unsigned int port = 4;
    int val = 0;
    printf(" _____  _____    _         _ \n");
    printf("| __  ||  _  |  | | ___  _| |\n");
    printf("|    -||   __|  | || -_|| . |Release: ver 0.1\n");
    printf("|__|__||__|     |_||___||___|Author: Splinter1984\n");
    printf("\n");
    printf("raspiLedDev_test: enter <GPIO pin> <ON 1|OFF 0> ");
    scanf("%u %d", &port, &val);
    
    int fd;
    ssize_t count;
    fd = open("/dev/raspiLedDev", O_RDWR);
    if (fd == -1){
        printf("raspiLedDev: open /dev/raspiLedDev fail\n");
        return -1;
    }
    gpio_data.GPIO_port = port;
    gpio_data.on_off = val;

    printf("raspiLedDev_test: send gpio %u, value %s\n",gpio_data.GPIO_port, gpio_data.on_off > 0 ? "ON":"OFF");

    count = write(fd, (void *) &gpio_data, sizeof(GPIO_data));
    if (count == -1){
        printf("raspiLedDev_test: write fail\n");
        exit(-1);
    }
    printf("raspiLedDev_test: write %ld bytes\n", count);

    return 0;
}
