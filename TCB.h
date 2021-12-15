/*TCB.h
 * @file   TCB.h
 *   @author    Clayden Boyd, Jin Terada White
 *   @date      19-May-2021
 *   @brief   defines the TCB for a DDS scheduler stored in a struct
 */

#ifndef TCB_H
#define TCB_H


/**
 * @A struct to define the TCB Scheduler
 * Stores a unique ID, number of times a task has been started/restarted,
 * whether it is READY, RUNNING, or SLEEPING, and the name of the task.
 */
typedef struct TCB {
  int ID; // Unique ID code for task
  char taskName[20]; //String name of the task up to 20 characters
  int startCount; //contains the total number of times a task has been started/restarted
  int state ; //contains state of task 1="READY", 2="RUNNING", 3="SLEEPING", 4="DEAD"
  struct TCB* next; //stores a pointer to the next item in TCB list
} TCB;

/**
 * @Function which constructs a single TCB struct node
 */
TCB* makeTCB(int ID, char taskName[], TCB* next);

/**
 * @Function which allows a task to terminate itself by manipulating 
 * its TCB
 */
void task_self_quit(void);

/**
 * @Function that allows a task to start up another task
 * 
 * @param param1 Pointer to a dead task that we want to revive
 */
void task_start(TCB* task);

#endif
