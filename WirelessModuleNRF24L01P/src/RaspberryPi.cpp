/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#include "RaspberryPi.h"

int32_t  mem_fd;
void *gpio_map;
volatile unsigned *gpio;

/* ======================================================= */
// Set up a memory regions to access GPIO
void setup_io()
{
   /* open /dev/mem */
   if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      printf("can't open /dev/mem \n");
      exit(-1);
   }

   /* mmap GPIO */
   gpio_map = mmap(
      NULL,             //Any adddress in our space will do
      BLOCK_SIZE,       //Map length
      PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
      MAP_SHARED,       //Shared with other processes
      mem_fd,           //File to map
      GPIO_BASE         //Offset to GPIO peripheral
   );

   close(mem_fd); //No need to keep mem_fd open after mmap

   if (gpio_map == MAP_FAILED) {
      printf("mmap error %d\n", (int32_t)gpio_map);//errno also set!
      exit(-1);
   }

   // Always use volatile pointer!
   gpio = (volatile unsigned *)gpio_map;

   INP_GPIO(SPI_CSN);
   OUT_GPIO(SPI_CSN);

   INP_GPIO(SPI_CE);
   OUT_GPIO(SPI_CE);

} // setup_io

/* ======================================================= */
void setCSN(uint8_t value)
{
	if (value) {
		GPIO_SET = 1<<SPI_CSN;
	} else {
		GPIO_CLR = 1<<SPI_CSN;
	}
}

/* ======================================================= */
void setCE(uint8_t value)
{
	if (value) {
		GPIO_SET = 1<<SPI_CE;
	} else {
		GPIO_CLR = 1<<SPI_CE;
	}
}

/* ======================================================= */
// Set up SPI interface
void setup_spi()
{
    int32_t ret = 0;
	fd = open(device, O_RDWR);
	if (fd < 0)
		pabort("can't open device");

	/*
	 * spi mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		pabort("can't get spi mode");

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word");

	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz");

	printf("spi mode: %d\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);
} // setup_spi

/* ======================================================= */
// SPI transfer
uint8_t transfer_spi(uint8_t tx_)
{
	int32_t ret;
	// One byte is transfered at once
	uint8_t tx[] = {0};
	tx[0] = tx_;

	uint8_t rx[ARRAY_SIZE(tx)] = {0};
	struct spi_ioc_transfer tr;
	tr.tx_buf = (unsigned long)tx;
	tr.rx_buf = (unsigned long)rx;
	tr.len = ARRAY_SIZE(tx);
	tr.delay_usecs = 0;
//	tr.cs_change = 1;
	tr.speed_hz = speed;
	tr.bits_per_word = bits;

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
	{
		perror("can't send spi message");
		abort();
	}

	return rx[0];
} // transfer_spi

/* ======================================================= */
void __msleep(int32_t milisec)
{
	struct timespec req = {0};
	req.tv_sec = 0;
	req.tv_nsec = milisec * 1000000L;
	nanosleep(&req, (struct timespec *)NULL);
}

/* ======================================================= */
void __usleep(int32_t micros)
{
	struct timespec req = {0};
	req.tv_sec = 0;
	req.tv_nsec = micros * 1000L;
	nanosleep(&req, (struct timespec *)NULL);
}

/* ======================================================= */
void __start_timer()
{
	gettimeofday(&start, NULL);
}

/* ======================================================= */
long __millis()
{
	gettimeofday(&end, NULL);
    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
	return mtime;
}

/* ======================================================= */
void pabort(const char *s)
{
    perror(s);
    abort();
}

