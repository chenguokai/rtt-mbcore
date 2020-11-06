/*
 * Copyright (c) 2019-2020, Xim
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef BOARD_H__
#define BOARD_H__

#include <rtconfig.h>

extern unsigned int __bss_start;
extern unsigned int __bss_end;

#define RT_HW_HEAP_BEGIN    (void*)(0x400c0000UL) // about 12M above the kernel load address
#define RT_HW_HEAP_END      (void*)(0x400c0000UL + 20 * 1024) // 20K in total

void rt_hw_board_init(void);
void rt_init_user_mem(struct rt_thread *thread, const char *name, unsigned long *entry);


#endif
