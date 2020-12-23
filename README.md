# Linux driver programming practice.
![](https://img.shields.io/badge/made%20by-splinter1984-blue)

Kernel module that provides access to LEDs connected to the GPIO output of raspberry pi. You can link to any of the GUIO pins and set a value on it.

Clone the repository and make it in that directory.

```
make
```

Install the module.

```
sudo insmod raspi_led.ko
```

Run.

```
gcc -o test test.c
sudo ./test
```
