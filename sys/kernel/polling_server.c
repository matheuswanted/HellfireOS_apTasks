#include <hal.h>
#include <kprintf.h>
#include <queue.h>
#include <panic.h>
#include <kernel.h>
#include <polling_server.h>

static int32_t ap_queue_next()
{
    do{
	    krnl_task = hf_queue_remhead(krnl_ap_queue);
        if (!krnl_task)
            panic(PANIC_NO_TASKS_AP);
        if (hf_queue_addtail(krnl_ap_queue, krnl_task))
            panic(PANIC_CANT_PLACE_RT);

    }while(krnl_task->state == TASK_BLOCKED)
    krnl_task->bgjobs++;
    return krnl_task->id;
}

void dispatch(int32_t *rc){
    krnl_task = &krnl_tcb[krnl_current_task];
    *rc = _context_save(krnl_task->task_context);
    if (*rc)
        return;

    if (krnl_task->state == TASK_RUNNING)
        krnl_task->state = TASK_READY;
    if (krnl_task->pstack[0] != STACK_MAGIC)
        panic(PANIC_STACK_OVERFLOW);
    if (krnl_tasks > 0){
        krnl_current_task = krnl_pcb.sched_be();
        krnl_task->state = TASK_RUNNING;
        krnl_pcb.coop_cswitch++;
        #if KERNEL_LOG >= 1
                dprintf("\n%d %d %d %d %d ", krnl_current_task, krnl_task->period, krnl_task->capacity, krnl_task->deadline, (uint32_t)_read_us());
        #endif
        _context_restore(krnl_task->task_context, 1);
        panic(PANIC_UNKNOWN);
    }else{
        panic(PANIC_NO_TASKS_LEFT);
    }
}

void polling_server(void){
    int32_t rc;
    volatile int32_t status;

    while(1){
        if(hf_queue_count(krnl_ap_queue) == 0)
            hf_yeld();
        else{
            status = _di();
            krnl_current_task = ap_queue_next();

            if(krnl_current_task->bgjobs >= krnl_current_task->capacity){
                hf_queue_remtail();
                hf_kill(krnl_current_task->id);
            }
            else
                dispatch(&rc);

            _ei(*status);

        }
    }
}