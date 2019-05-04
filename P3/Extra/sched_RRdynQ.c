#include "sched.h"

#define DEFAULT_QUANTUM     3

/* Global RR quantum parameter */
const int dynq_rr_quantum=DEFAULT_QUANTUM;

/* Global max slice */
const int global_max_slice=DEFAULT_QUANTUM+2;
const int global_min_slice=2;

struct RR_dynQ_data {
    int  remaining_ticks_slice;      // Remaining ticks
    int  current_slice;              // Initial ticks when extracted from runqueue
};

static int task_new_rrdynQ(task_t* t)
{
	struct RR_dynQ_data* cs_data=malloc(sizeof(struct RR_dynQ_data));

	if (!cs_data)
		return 1;  /* Cannot reserve memory */

	/* initialize the quantum */
	cs_data->remaining_ticks_slice = dynq_rr_quantum;
	cs_data->current_slice = dynq_rr_quantum;
	t->tcs_data=cs_data;
	return 0;
}

static void task_free_rrdynQ(task_t* t)
{
	if (t->tcs_data) {
		free(t->tcs_data);
		t->tcs_data=NULL;
	}
}

static task_t* pick_next_task_rrdynQ(runqueue_t* rq)
{
	task_t* t=head_slist(&rq->tasks);

		/* Current is not on the rq -> let's remove it */
		if (t)
			remove_slist(&rq->tasks,t);

		return t;
}

static void enqueue_task_rrdynQ(task_t* t,runqueue_t* rq, int preempted) 
{
	struct RR_dynQ_data* cs_data=(struct RR_dynQ_data*) t->tcs_data;

		if (is_idle_task(t))
			return;

		//Sanity check, the task should NOT be on the runqueue
		assert(!t->on_rq);

		insert_slist(&rq->tasks,t); //Push task

		cs_data->remaining_ticks_slice= cs_data->current_slice; // Reset slice


}


static void task_tick_rrdynQ(runqueue_t* rq)
{
	task_t* current=rq->cur_task;
	struct RR_dynQ_data* cs_data=(struct RR_dynQ_data*) current->tcs_data;

		if (is_idle_task(current))
			return;

		cs_data->remaining_ticks_slice--; /* Charge tick */


		//si todavia quedan ticks para terminar la rafaga
		if(current->runnable_ticks_left != 1){
			//y ha terminado el quando
			if(cs_data->remaining_ticks_slice <= 0){
					//para que no reste menos del global_min_slice
					if(cs_data->current_slice > global_min_slice){
						cs_data->current_slice--;
					}

			}
			//si finaliza la rafaga
		}else if(current->runnable_ticks_left == 1){
			//y todavia dispone de quando
			if(cs_data->remaining_ticks_slice > 0) {
				//para que no sume más que el global_max_slice
				if(cs_data->current_slice < global_max_slice){
					cs_data->current_slice++;
				}

			}
		}


		if (cs_data->remaining_ticks_slice<=0)
			rq->need_resched=TRUE; //Force a resched !!
}

static task_t* steal_task_rrdynQ(runqueue_t* rq)
{
	task_t* t=tail_slist(&rq->tasks);

		if (t)
			remove_slist(&rq->tasks,t);

		return t;
}
    

sched_class_t RR_dynQ_sched={
    .task_new       = task_new_rrdynQ,
    .task_free      = task_free_rrdynQ,
    .pick_next_task = pick_next_task_rrdynQ,
    .enqueue_task   = enqueue_task_rrdynQ,
    .task_tick      = task_tick_rrdynQ,
    .steal_task     = steal_task_rrdynQ
};
