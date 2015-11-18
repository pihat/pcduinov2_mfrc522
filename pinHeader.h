/*************************************************************************
    > File Name: digital.c
    > Author: Guoger
    > Mail: pingger.wu@gmail.com 
    > Created Time: Fri Jan  1 00:14:17 2015
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

#define GPIO_MODE_DIR "/sys/devices/virtual/misc/gpio/mode/"
#define GPIO_PIN_DIR "/sys/devices/virtual/misc/gpio/pin/"
#define GPIO_IF_PREFIX "gpio"

#define INPUT 0
#define OUTPUT 1
#define LOW 0
#define HIGH 1

#define MAX_GPIO_NUM 23

int gpio_mode_fd[MAX_GPIO_NUM+1];
int gpio_pin_fd[MAX_GPIO_NUM+1];
extern int err;

int digitalRead(uint8_t pin){
	int fd = -1;
	char buf[4];
	int ret = -1;
	char path[1024];

	memset((void*)buf, 0, sizeof(buf));
	memset(path, 0, sizeof(path));
	
	sprintf(path, "%s%s%d", GPIO_PIN_DIR, GPIO_IF_PREFIX, pin);
//	fprintf(stdout, "%s%s%d", GPIO_PIN_DIR, GPIO_IF_PREFIX, pin);
	gpio_pin_fd[pin] = open(path, O_RDWR);
  	lseek(gpio_pin_fd[pin], 0, SEEK_SET);
	ret = read(gpio_pin_fd[pin], buf, sizeof(buf));
	close(gpio_pin_fd[pin]);

	if(ret <= 0)
		fprintf(stderr, "read gpio%d failied\n", pin);

	ret = buf[0] - '0';
	return ret;
}
void digitalWrite(uint8_t pin, uint8_t value){
	int fd = -1;
	int ret = -1;
	char path[1024];
	char *mesg = NULL;
			
	memset(path, 0 , sizeof(path));

	sprintf(path, "%s%s%d", GPIO_PIN_DIR, GPIO_IF_PREFIX, pin);
//	fprintf(stdout, "%s%s%d\n", GPIO_PIN_DIR, GPIO_IF_PREFIX, pin);
	gpio_pin_fd[pin] = open(path, O_RDWR);

	if(gpio_pin_fd[pin] < 0){
	//	fprintf(stderr, "OPEN GPIO%d Fail\n",pin);
	 	fprintf(stdout, "%s%s%d\n", GPIO_PIN_DIR, GPIO_IF_PREFIX, pin);
		fprintf(stdout, "err = %d\n", err);
		mesg = strerror(err);
		printf("%s\n", mesg);
		exit(-1);
	}

 	lseek(gpio_pin_fd[pin], 0, SEEK_SET);

	if(value == HIGH){
		ret = write(gpio_pin_fd[pin], "1", 1);
		if(ret < 0)
			fprintf(stderr,"Set GPIO%d High Fail\n",pin);
	}
	else{
		ret = write(gpio_pin_fd[pin], "0", 1);
		if(ret < 0)
			fprintf(stderr, "Set GPIO%d LOW Fail\n", pin);
	}
	if(gpio_pin_fd[pin]){
		close(gpio_pin_fd[pin]);
	}
}
void pinMode(uint8_t pin, uint8_t mode){
	int fd = -1;
	int ret = -1;
	unsigned long val = pin;
	char path[1024];
	switch(pin){
		case 3:
		case 9:
		case 10:
		case 11:
			fd = open("/dev/pwmtimer", O_RDONLY);
			if(fd < 0)
				printf("open pwm device fail\n");
			ret = ioctl(fd, 0x102, &val);
			if(ret < 0)
				printf("can't set pwmtimer stop\n");
			if(fd)
				close(fd);
			break;

		default:
			break;
	}
	switch (mode){
		case INPUT:
			memset(path, 0 , sizeof(path));
			sprintf(path, "%s%s%d", GPIO_MODE_DIR, GPIO_IF_PREFIX, pin);
			gpio_mode_fd[pin] = open(path, O_RDWR);
			if(gpio_mode_fd[pin] < 0){
				fprintf(stderr, "open %s failed\n", path);
				return;
			}
			write(gpio_mode_fd[pin], "0", 1);
			break;
		case OUTPUT:
			memset(path, 0 , sizeof(path));
			sprintf(path, "%s%s%d", GPIO_MODE_DIR, GPIO_IF_PREFIX, pin);
			gpio_mode_fd[pin] = open(path, O_RDWR);
			if(gpio_mode_fd[pin] < 0){
				fprintf(stderr, "open %s failed\n", path);
				return;
			}
			write(gpio_mode_fd[pin], "1", 1);
			break;
		default:
			break;
	}
	if(gpio_mode_fd[pin])
		close(gpio_mode_fd[pin]);
}
