/*
 * Copyright (c) 2019-2020, Xim
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#include "board.h"
#include "drv_uart.h"

#include <stdio.h>

char *uart_buf = (char *)(0x80080000UL);

#define UART_DEFAULT_BAUDRATE               115200

struct device_uart
{
    rt_uint32_t hw_base;
    rt_uint32_t irqno;
};

static rt_err_t  rt_uart_configure(struct rt_serial_device *serial, struct serial_configure *cfg);
static rt_err_t uart_control(struct rt_serial_device *serial, int cmd, void *arg);
static int       drv_uart_putc(struct rt_serial_device *serial, char c);
static int       drv_uart_getc(struct rt_serial_device *serial);

const struct rt_uart_ops _uart_ops =
{
    rt_uart_configure,
    uart_control,
    drv_uart_putc,
    drv_uart_getc,
    //TODO: add DMA support
    RT_NULL
};


void uart_init(void)
{
    // foo implementation
}

struct rt_serial_device  serial1;
struct device_uart       uart1;

/*
 * UART Initiation
 */
int rt_hw_uart_init(void)
{
    struct rt_serial_device *serial;
    struct device_uart      *uart;
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

    {
        

        serial  = &serial1;
        uart    = &uart1;

        serial->ops              = &_uart_ops;
        serial->config           = config;
        serial->config.baud_rate = UART_DEFAULT_BAUDRATE;

        uart->hw_base   = 0; // FOO implementation
        uart->irqno     = 0; // FOO implementation
        

        // uart_init(UART_DEVICE_1);

        rt_hw_serial_register(serial,
                              "uart",
                              RT_DEVICE_FLAG_STREAM | RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                              uart);
    }

    return 0;
}

/*
 * UART interface
 */

static rt_err_t rt_uart_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
    struct device_uart *uart;

    RT_ASSERT(serial != RT_NULL);
    serial->config = *cfg;

    return (RT_EOK);
}

static rt_err_t uart_control(struct rt_serial_device *serial, int cmd, void *arg)
{
    struct device_uart *uart;

    uart = serial->parent.user_data;
    rt_uint32_t channel = 1;

    RT_ASSERT(uart != RT_NULL);
    RT_ASSERT(channel != 3);

    switch (cmd)
    {
    case RT_DEVICE_CTRL_CLR_INT:
        /* Disable the UART Interrupt */
        //rt_hw_interrupt_mask(uart->irqno);
        // _uart[channel]->IER &= ~0x1;
        break;

    case RT_DEVICE_CTRL_SET_INT:
        /* install interrupt */
        // rt_hw_interrupt_install(uart->irqno, uart_irq_handler,
        //                        serial, serial->parent.parent.name);
        // rt_hw_interrupt_umask(uart->irqno);
        //_uart[channel]->IER |= 0x1;
        break;
    }

    return (RT_EOK);
}

static int drv_uart_putc(struct rt_serial_device *serial, char c)
{
#if 1
    // using memory interface
    uart_buf[0] = c;
#else
    #define CSR_SIM_CTRL_PUTC (1 << 24)
    unsigned int arg = CSR_SIM_CTRL_PUTC | (c & 0xFF);
    asm volatile ("csrw dscratch,%0": : "r" (arg));
#endif

    return (1);
}

const char* uart_input = "help\nversion\nlist_thread\n";
int uart_input_off = 0;
int uart_input_len = 25;

static int drv_uart_getc(struct rt_serial_device *serial)
{
    if (uart_input_off < uart_input_len) {
        return uart_input[uart_input_off++];
    }
    // no input here
    return -1;
}

void drv_uart_puts(char *str) {
    for (int i =  0; str[i] != 0; ++i) {
        drv_uart_putc(NULL, str[i]);  // null here is not a problem since we do not have a second serial
    }
}

char rt_hw_console_getchar(void) {
    return 0; // do not implement input for now
}