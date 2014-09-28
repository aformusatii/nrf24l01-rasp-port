/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

/* =========== SPI & GPIO configuration ========= */
#define SPI_DEVICE "/dev/spidev0.0"
#define SPI_SPEED 250000
#define SPI_CSN 25
#define SPI_CE  24

/* =========== GPIO MEM STUFF =================== */
#define BCM2708_PERI_BASE 0x20000000
#define GPIO_BASE         (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */
#define INP_GPIO(g)      *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g)      *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define GPIO_SET         *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR         *(gpio+10) // clears bits which are 1 ignores bits which are 0
#define BLOCK_SIZE        (4*1024)

/* =========== SPI STUFF ======================== */
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
static const char *device = SPI_DEVICE;
static uint8_t mode;
static uint8_t bits = 8;
static int32_t speed = SPI_SPEED;
static int32_t fd;

/* =========== SPI and GPIO function ============ */
void setup_io();
void setup_spi();
void setCSN(uint8_t value);
void setCE(uint8_t value);
uint8_t transfer_spi(uint8_t tx_);
void pabort(const char *s);

/* =========== TIME function and variables ====== */
#ifdef	__cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <time.h>
#include <sys/time.h>

// added attribute unused to avoid compiler warnings
static struct timeval start __attribute__ ((unused)) ,end __attribute__ ((unused));

static long __attribute__ ((unused)) mtime;
static long __attribute__ ((unused)) seconds;
static long __attribute__ ((unused)) useconds;

void __msleep(int32_t milisec);
void __usleep(int32_t micros);
void __start_timer();
long __millis();

#ifdef	__cplusplus
}
#endif
