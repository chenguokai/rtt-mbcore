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
#include "tick.h"

#include "drv_uart.h"

#include "encoding.h"


void init_bss(void)
{
    unsigned int *dst;

    dst = &__bss_start;
    while (dst < &__bss_end)
    {
        *dst++ = 0;
    }
}

void primary_cpu_entry(void)
{
    extern void entry(void);

    /* disable global interrupt */
    init_bss();
    rt_hw_interrupt_disable();
    entry();
}

void rt_hw_interrupt_init() {
    rt_hw_interrupt_enable(0x8); // enable MIE in mstatus
}

void rt_hw_board_init(void)
{
    /* initalize interrupt */
    rt_hw_interrupt_init();
    /* initialize hardware interrupt */
    rt_hw_uart_init();

#ifdef RT_USING_CONSOLE
    /* set console device */
    rt_console_set_device("uart\0\0\0");
#endif /* RT_USING_CONSOLE */

#ifdef RT_USING_HEAP
    rt_kprintf("heap: [0x%08x - 0x%08x]\n", (rt_ubase_t) RT_HW_HEAP_BEGIN, (rt_ubase_t) RT_HW_HEAP_END);
    /* initialize memory system */
    rt_system_heap_init(RT_HW_HEAP_BEGIN, RT_HW_HEAP_END);
#endif

#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

    rt_hw_tick_init();
}
void rt_hw_cpu_reset(void)
{
    // sysctl->soft_reset.soft_reset = 1;
    while(1);
}

MSH_CMD_EXPORT_ALIAS(rt_hw_cpu_reset, reboot, reset machine);

extern struct rt_serial_device  serial1;

void handle_syscall(uintptr_t mcause, uintptr_t mepc, uintptr_t sp) {
    // foo implementation here
}

#define SYSCALL_MCAUSE 11

uintptr_t handle_trap(uintptr_t mcause, uintptr_t mepc, uintptr_t sp) {
    // while (1);
    rt_hw_interrupt_disable();
    if (mcause == (uint32_t)(0x80000007UL)) {
        // timer interrupt
        tick_isr();
    }
    if (mcause != SYSCALL_MCAUSE) {
        if (serial1.serial_rx != NULL)
            rt_hw_serial_isr(&serial1, RT_SERIAL_EVENT_RX_IND); // try grabing some data
        // not a syscall
        rt_schedule();
    } else {
        // syscall
        handle_syscall(mcause, mepc, sp);
    }
    return 0;
}