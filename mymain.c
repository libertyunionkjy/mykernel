/*
 *  linux/mykernel/mymain.c
 *
 *  Kernel internal my_start_kernel
 *  Change IA32 to x86-64 arch, 2020/4/26
 *
 *  Copyright (C) 2013, 2020  Mengning
 *  
 */
#include <linux/types.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/tty.h>
#include <linux/vmalloc.h>


#include "mypcb.h"

/* tPCB是PCB结构体的别名 在mypcb.h中声明 */
tPCB task[MAX_TASK_NUM];
tPCB * my_current_task = NULL;
volatile int my_need_sched = 0;

void my_process(void);


void __init my_start_kernel(void)
{
    int pid = 0;
    int i;
    /* Initialize process 0*/
    task[pid].pid = pid;
    task[pid].state = 0;/* -1 unrunnable, 0 runnable, >0 stopped */
    /* 这里直接使用函数名代表函数的起始地址 */
    task[pid].task_entry = task[pid].thread.ip = (unsigned long)my_process;
    task[pid].thread.sp = (unsigned long)&task[pid].stack[KERNEL_STACK_SIZE-1];
    task[pid].next = &task[pid];
    /*fork more process */
    for(i=1;i<MAX_TASK_NUM;i++)
    {
        memcpy(&task[i],&task[0],sizeof(tPCB));
        task[i].pid = i;
	    task[i].thread.sp = (unsigned long)(&task[i].stack[KERNEL_STACK_SIZE-1]);
        task[i].next = task[i-1].next;
        task[i-1].next = &task[i];
    }
    /* start process 0 by task[0] */
    pid = 0;
    my_current_task = &task[pid];
	asm volatile(
    	"movq %1,%%rsp\n\t" 	/* set task[pid].thread.sp to rsp 这里是初始化为0*/
    	"pushq %1\n\t" 	        /* push rbp */
    	"pushq %0\n\t" 	        /* push task[pid].thread.ip */
    	"ret\n\t" 	            /* pop task[pid].thread.ip to rip ，然后就会开始执行my_process函数*/
    	: 
    	: "c" (task[pid].thread.ip),"d" (task[pid].thread.sp)	/* input c or d mean %ecx/%edx*/
	);
} 

int i = 0;

void my_process(void)
{    
    while(1)
    {
        i++;
        if(i%10000000 == 0)
        {
            printk(KERN_NOTICE "this is process %d -\n",my_current_task->pid);
            if(my_need_sched == 1)
            {
                my_need_sched = 0;
        	    my_schedule();
        	}
        	printk(KERN_NOTICE "this is process %d +\n",my_current_task->pid);
        }     
    }
}
