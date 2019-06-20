/// @file scheduler_algorithm.c
/// @brief Round Robin algorithm.
/// @date Mar 2019.

#include "prio.h"
#include "debug.h"
#include "assert.h"
#include "list_head.h"
#include "scheduler.h"

#define GET_WEIGHT(prio) prio_to_weight[USER_PRIO((prio))]
#define NICE_0_LOAD GET_WEIGHT(DEFAULT_PRIO)

task_struct *pick_next_task(runqueue_t *runqueue, time_t delta_exec)
{
	// Pointer to the next task to schedule.
	task_struct *next = NULL;

#if defined(SCHEDULER_RR)
	//==== Implementatin of the Round-Robin Scheduling algorithm ============
	struct list_head *nNode=runqueue->curr->run_list.next;

	if(nNode==&runqueue->queue)
		nNode=nNode->next;

	next=list_entry(nNode, struct task_struct, run_list);



	//=======================================================================
#elif defined(SCHEDULER_PRIORITY)
	//==== Implementatin of the Priority Scheduling algorithm ===============

	// get the first element of the list
	next = list_entry(runqueue->curr->run_list.next ,struct task_struct, run_list);

	// Get its static priority.

	time_t min =next->se.prio;

	list_head *it;
	// Inter over the runqueue to find the task with the smallest priority value
	list_for_each (it, &runqueue->queue) {
		task_struct *entry = list_entry(it, struct task_struct, run_list);
		// Check entry has a lower priority
		if (entry->se.prio<min) {
			next=entry;
			min=entry->se.prio;
		}
	}

	//=======================================================================
#elif defined(SCHEDULER_CFS)
	//==== Implementatin of the Completely Fair Scheduling ==================

	// Get the weight of the current process.
	// (use GET_WEIGHT macro
	task_struct *first=runqueue->curr;

	int weight = GET_WEIGHT(first->se.prio);

	if (weight != NICE_0_LOAD) {
		// get the multiplicative factor for its delta_exec.
		double factor = NICE_0_LOAD/weight;

		// weight the delta_exec with the multiplicative factor.
		delta_exec = delta_exec*factor;
	}

	// Update vruntime of the current process.
	first->se.vruntime+=delta_exec;

	// Inter over the runqueue to find the task with the smallest vruntime value
	next = list_entry(runqueue->curr->run_list.next ,struct task_struct, run_list);
	list_head *it;
	list_for_each (it, &runqueue->queue) {
		task_struct *entry = list_entry(it, struct task_struct, run_list);
		// Check entry has a lower priority
		if (entry->se.vruntime<next->se.vruntime) {
			next=entry;
		}
	}

	//========================================================================
#else
#error "You should enable a scheduling algorithm!"
#endif
	assert(next && "No valid task selected. Have you implemented a scheduling algorithm?");

	return next;
}
