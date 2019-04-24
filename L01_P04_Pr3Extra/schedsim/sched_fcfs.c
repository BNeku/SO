#include "sched.h"

//Práctica 3 SO, Grupo 2ºE
//Nerea Jiménez
//Pablo Miranda
static task_t* pick_next_task_fcfs(runqueue_t* rq){
	task_t* task=head_slist(&rq->tasks);

	if(task){
	   remove_slist(&rq->tasks,task);
	   task->on_rq=FALSE;
	   rq->cur_task=task;
	}

	return task;
}

static void enqueue_task_fcfs(task_t* t,runqueue_t* rq, int preempted){


	if (t->on_rq || is_idle_task(t)) //cola vacía
		return;

	insert_slist(&rq->tasks, t);
	t->on_rq=TRUE;

}

static void task_tick_fcfs(runqueue_t* rq){

	task_t* task=rq->cur_task;

	if (is_idle_task(task))
		return;
}

static task_t* steal_task_fcfs(runqueue_t* rq){
	task_t* task = tail_slist(&rq->tasks);

	if (task) {
		remove_slist(&rq->tasks,task);
	    task->on_rq=FALSE;
	}

	return task;
}


sched_class_t fcfs_sched = {
	.pick_next_task=pick_next_task_fcfs,
	.enqueue_task=enqueue_task_fcfs,
	.task_tick=task_tick_fcfs,
	.steal_task=steal_task_fcfs,
};
