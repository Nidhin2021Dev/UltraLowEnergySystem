/* Board support package for TM4C123GH6PM */
#ifndef __BSP_H__
#define __BSP_H__

#define LEDRED (1U << 1)
#define LEDGREEN (1U << 3)
#define LEDBLUE (1U << 2)

#include <stdint.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <stdio.h>

#define toggleBit(reg,bit) (reg ^= bit)
void init_gpio(void);

#endif