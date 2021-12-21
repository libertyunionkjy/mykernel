/*
 *  linux/mykernel/myinterrupt.c
 *
 *  Kernel internal my_timer_handler
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

extern tPCB task[MAX_TASK_NUM];
extern tPCB * my_current_task;
extern volatile int my_need_sched;
volatile int time_count = 0;

/*
 * Called by timer interrupt.
 * it runs in the name of current running process,
 * so it use kernel stack of current running process
 */
void my_timer_handler(void)
{
    if(time_count%1000 == 0 && my_need_sched != 1)
    {
        printk(KERN_NOTICE ">>>my_timer_handler here<<<\n");
        my_need_sched = 1;
    } 
    time_count ++ ;  
    return;  	
}

void my_schedule(void)
{
    tPCB * next;
    tPCB * prev;

    if(my_current_task == NULL 
        || my_current_task->next == NULL)
    {
    	return;
    }
    printk(KERN_NOTICE ">>>my_schedule<<<\n");
    /* schedule */
    next = my_current_task->next;
    prev = my_current_task;
    if(next->state == 0)/* -1 unrunnable, 0 runnable, >0 stopped ，0代表这个进程可执行/被执行过 */
    {        
    	my_current_task = next; 
        /* 输出一个提示，从什么进程切换到什么进程 */
    	printk(KERN_NOTICE ">>>switch %d to %d<<<\n",prev->pid,next->pid);  
    	/* switch to next process */
    	asm volatile(
            /* 
            * 每个进程的rbp是当前进程栈的最后一个入栈的值，后面rsp就会变化，
            * 从而换到另外一个栈空间，
            * 另外也可以得知每个进程被调度后一定要先pop一下恢复rbp的值 
            */	
        	"pushq %%rbp\n\t" 	    /* save rbp of prev */
        	"movq %%rsp,%0\n\t" 	/* prev进程的rsp是保存在pcb中的 */
        	"movq %2,%%rsp\n\t"     /* 恢复next进程的rsp值 */
        	"movq $1f,%1\n\t"       /* 保存prev的rip值，这里1f其实就是后面的popq的地址 */	
        	"pushq %3\n\t"          /* 从pcb中取出rip */
        	"ret\n\t" 	            /* 恢复rip的值 */
        	"1:\t"                  /* next进程从这儿开始执行 */
        	"popq %%rbp\n\t"
        	: "=m" (prev->thread.sp),"=m" (prev->thread.ip)
        	: "m" (next->thread.sp),"m" (next->thread.ip)
    	); 
    }  
    return;	
}
