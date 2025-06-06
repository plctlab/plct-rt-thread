/*
 * Copyright (c) 2006-2025 RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023/06/25     flyingcys    first version
 * 2025-05-28     ZhangJing    Porting to ultrarisc dp1000
 */

#ifndef BOARD_H__
#define BOARD_H__

#include <rtconfig.h>

extern unsigned int __bss_start;
extern unsigned int __bss_end;

#ifndef RT_USING_SMART
    #define KERNEL_VADDR_START 0x0
#endif

#define VIRT64_SBI_MEMSZ (0x200000)

#define RT_HW_HEAP_BEGIN ((void *)&__bss_end)
#define RT_HW_HEAP_END   ((void *)(RT_HW_HEAP_BEGIN + 64 * 1024 * 1024))
#define RT_HW_PAGE_START RT_HW_HEAP_END
#define RT_HW_PAGE_END   ((void *)(KERNEL_VADDR_START + (256 * 1024 * 1024 - VIRT64_SBI_MEMSZ)))

void rt_hw_board_init(void);

#endif