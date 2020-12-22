// struc of GPIO_DATA
// use include raspi_led.h in test file of project 
typedef struct GPIO_DATA{
    unsigned int GPIO_port;
    int on_off;
} GPIO_data;