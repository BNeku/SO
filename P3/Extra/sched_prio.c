#include "sched.h"

static task_t* pick_next_task_prio(runqueue_t* rq){
	task_t* task=head_slist(&rq->tasks); //igual que en fcfs porque la lista está ordenada (por prioridades)

	if(task){
	   remove_slist(&rq->tasks,task);
	   task->on_rq=FALSE;
	   rq->cur_task=task;
	}

	return task;
}

static int compare_tasks_prio(void *t1,void *t2)
{
	//similar a la función de sjf, pero comparamos prioridades
	task_t* tsk1=(task_t*)t1;
	task_t* tsk2=(task_t*)t2;
	return tsk1->prio-tsk2->prio;
}

static void enqueue_task_prio(task_t* t,runqueue_t* rq, int preempted){

	if (t->on_rq || is_idle_task(t)) //cola vacía
			return;

	if (t->flags & TF_INSERT_FRONT) {
			//Clear flag
			t->flags&=~TF_INSERT_FRONT;
			sorted_insert_slist_front(&rq->tasks, t, 1, compare_tasks_prio);  //Push task
		} else
			sorted_insert_slist(&rq->tasks, t, 1, compare_tasks_prio);  //Insertamos en la lista por prioridad

		t->on_rq=TRUE;

	    /* If the task was not using the current CPU, check whether a preemption is in order or not */
		if (!preempted) {
			task_t* current=rq->cur_task;
			t->last_cpu=rq->cpu_rq;

			/* Trigger a preemption if this task has a shorter CPU burst than current */
			if (preemptive_scheduler && t->runnable_ticks_left<current->runnable_ticks_left) {
				rq->need_resched=TRUE;
				current->flags|=TF_INSERT_FRONT; /* To avoid unfair situations in the event
	                                                another task with the same length wakes up as well*/
			}
		}

}

static void task_tick_prio(runqueue_t* rq){
	task_t* task=rq->cur_task;

	if (is_idle_task(task))
		return;
}

static task_t* steal_task_prio(runqueue_t* rq){
	task_t* task = tail_slist(&rq->tasks);

	if (task) {
		remove_slist(&rq->tasks,task);
	    task->on_rq=FALSE;
	}

	return task;
}



sched_class_t prio_sched = {
	.pick_next_task=pick_next_task_prio,
	.enqueue_task=enqueue_task_prio,
	.task_tick=task_tick_prio,
	.steal_task=steal_task_prio,
};
