/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018/10/28     Bernard      The unify RISC-V porting code.
 */

#include <rthw.h>
#include <rtthread.h>
#include <riscv-ops.h>
#include <encoding.h>

unsigned long interval = 10000000/RT_TICK_PER_SECOND;
static volatile unsigned long tick_cycles = 0;
int tick_isr(void)
{
    //uint64_t core_id = current_coreid();

    // clint->mtimecmp[core_id] += tick_cycles;
#if 1
    uint64_t next;
    next = ((uint64_t)read_csr(0x7c0)); // mtime
    next += interval;
    write_csr(0x7c2, (uint64_t)(next)); // mtimecmplo
#elif 0
    uint32_t next;
    next = read_csr(0xc01); // mtime
    next += interval;
    write_csr(0xc01, next);
#endif

    rt_tick_increase();

    return 0;
}
#define MIE_MTIE 1 << 7
/* Sets and enable the timer interrupt */
int rt_hw_tick_init(void)
{
    /* Read core id */

    /* Clear the Machine-Timer bit in MIE */
    // clear_csr(mie, MIE_MTIE); // we have a 
#if 1
    uint64_t next = 0;
    next = ((uint64_t)read_csr(0x7c0)); // mtime
    next += interval;
    
    write_csr(0x7c2, (uint64_t)(next)); // mtimecmplo
#elif 0
    uint32_t next = 0;
    next = read_csr(0xc01);
    next += interval;
    write_csr(0xc01, next);
#endif

    /* Enable the Machine-Timer bit in MIE */
    set_csr(mie, MIE_MTIE);

    return 0;
}
