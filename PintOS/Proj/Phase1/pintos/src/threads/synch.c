/* This file is derived from source code for the Nachos
   instructional operating system.  The Nachos copyright notice
   is reproduced in full below. */

/* Copyright (c) 1992-1996 The Regents of the University of California.
   All rights reserved.

   Permission to use, copy, modify, and distribute this software
   and its documentation for any purpose, without fee, and
   without written agreement is hereby granted, provided that the
   above copyright notice and the following two paragraphs appear
   in all copies of this software.

   IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO
   ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
   CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE
   AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF CALIFORNIA
   HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
   PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
   BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
   PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
   MODIFICATIONS.
*/

#include "threads/synch.h"
#include <stdio.h>
#include <string.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
/*
  - if a < b return true
  else false
*/
static bool lock_priority_compare(const struct list_elem *a, const struct list_elem *b, void *aux UNUSED);
static bool sema_priority_compare(const struct list_elem *a, const struct list_elem *b, void *aux UNUSED);

/* Initializes semaphore SEMA to VALUE.  A semaphore is a
   nonnegative integer along with two atomic operators for
   manipulating it:

   - down or "P": wait for the value to become positive, then
     decrement it.

   - up or "V": increment the value (and wake up one waiting
     thread, if any). */
void
sema_init (struct semaphore *sema, unsigned value)
{
  ASSERT (sema != NULL);
  sema->value = value;
  list_init (&sema->waiters);
}

/* Down or "P" operation on a semaphore.  Waits for SEMA's value
   to become positive and then atomically decrements it.

   This function may sleep, so it must not be called within an
   interrupt handler.  This function may be called with
   interrupts disabled, but if it sleeps then the next scheduled
   thread will probably turn interrupts back on. */
void
sema_down (struct semaphore *sema)
{
  enum intr_level old_level;

  ASSERT (sema != NULL);
  ASSERT (!intr_context ());

  old_level = intr_disable ();
  while (sema->value == 0)
    {
      list_insert_ordered(&sema->waiters, &thread_current ()->elem,less_priority_func  , NULL); // new
      thread_block ();
    }
  sema->value--;
  intr_set_level (old_level);
}

/* Down or "P" operation on a semaphore, but only if the
   semaphore is not already 0.  Returns true if the semaphore is
   decremented, false otherwise.

   This function may be called from an interrupt handler. */
bool
sema_try_down (struct semaphore *sema)
{
  enum intr_level old_level;
  bool success;

  ASSERT (sema != NULL);

  old_level = intr_disable ();
  if (sema->value > 0)
    {
      sema->value--;
      success = true;
    }
  else
    success = false;
  intr_set_level (old_level);

  return success;
}

/* Up or "V" operation on a semaphore.  Increments SEMA's value
   and wakes up one thread of those waiting for SEMA, if any.

   This function may be called from an interrupt handler. */
void
sema_up (struct semaphore *sema)
{
  enum intr_level old_level;

  ASSERT (sema != NULL);

  //diable the interrupt
  old_level = intr_disable ();
  sema->value++;

  // thread hold the top of the waiters list that has the highist priority
  struct thread *highist_waiters = NULL ; // new

  // check that waiters list is not empty
  if (!list_empty (&sema->waiters)){

    // sort the list waiters
    list_sort(&(sema->waiters), less_priority_func, NULL); // new

    //get the top of waiters list
    highist_waiters = list_entry (list_pop_front (&sema->waiters), struct thread, elem); // new

    //unblock the top of waiters
    thread_unblock (highist_waiters);


     if (thread_mlfqs && highist_waiters->priority > thread_current()->priority) { // new
       thread_yield(); // new
     }

  }

  // yield if highist thread is not  the current thread ( hightist priority )
  if (highist_waiters != NULL &&  !thread_mlfqs&&highist_waiters->priority > thread_current()->priority) { // new
    thread_yield(); // new
  }

  //enable the interrupt
  intr_set_level (old_level);
}

static void sema_test_helper (void *sema_);

/* Self-test for semaphores that makes control "ping-pong"
   between a pair of threads.  Insert calls to printf() to see
   what's going on. */
void
sema_self_test (void)
{
  struct semaphore sema[2];
  int i;

  printf ("Testing semaphores...");
  sema_init (&sema[0], 0);
  sema_init (&sema[1], 0);
  thread_create ("sema-test", PRI_DEFAULT, sema_test_helper, &sema);
  for (i = 0; i < 10; i++)
    {
      sema_up (&sema[0]);
      sema_down (&sema[1]);
    }
  printf ("done.\n");
}

/* Thread function used by sema_self_test(). */
static void
sema_test_helper (void *sema_)
{
  struct semaphore *sema = sema_;
  int i;

  for (i = 0; i < 10; i++)
    {
      sema_down (&sema[0]);
      sema_up (&sema[1]);
    }
}

/* Initializes LOCK.  A lock can be held by at most a single
   thread at any given time.  Our locks are not "recursive", that
   is, it is an error for the thread currently holding a lock to
   try to acquire that lock.

   A lock is a specialization of a semaphore with an initial
   value of 1.  The difference between a lock and such a
   semaphore is twofold.  First, a semaphore can have a value
   greater than 1, but a lock can only be owned by a single
   thread at a time.  Second, a semaphore does not have an owner,
   meaning that one thread can "down" the semaphore and then
   another one "up" it, but with a lock the same thread must both
   acquire and release it.  When these restrictions prove
   onerous, it's a good sign that a semaphore should be used,
   instead of a lock. */
void
lock_init (struct lock *lock)
{
  ASSERT (lock != NULL);

  lock->holder = NULL;
  sema_init (&lock->semaphore, 1);

  lock->priority = PRI_MIN; // new
}

/* Acquires LOCK, sleeping until it becomes available if
   necessary.  The lock must not already be held by the current
   thread.

   This function may sleep, so it must not be called within an
   interrupt handler.  This function may be called with
   interrupts disabled, but interrupts will be turned back on if
   we need to sleep. */
void
lock_acquire (struct lock *lock)
{
  ASSERT (lock != NULL);
  ASSERT (!intr_context ());
  ASSERT (!lock_held_by_current_thread (lock));

  if(!thread_mlfqs) { // new

	    struct thread *thread_hold_current_lock = lock->holder; // new
	    struct thread *current_thread = thread_current(); // new
	    struct lock *current_lock = lock; // new

      // make the waiting lock for current thread the current lock
    	current_thread->waiting_lock = lock; // new

      // current lock has no thread waiting for it
    	 if (thread_hold_current_lock == NULL){ // new

         //make the priorty of lock equal current thread priority
          current_lock->priority = current_thread->priority; // new
    	 }

      /*
         this loop for nested donation
      */
    	while (thread_hold_current_lock != NULL && current_thread->priority > thread_hold_current_lock->priority) {// new

          // check if the current thread priority bigger than lock priority
          if (current_thread->priority > current_lock->priority) {// new

            // make lock priority equal to current thread priority
            current_lock->priority = current_thread->priority;// new
          }// new

          // donation occur current thread priority is given to the lock holder thread
          change_priority_or_yielding(thread_hold_current_lock, current_thread->priority); // new

          // check if the waiting lock for the thread that hold the lock is not null
          if (thread_hold_current_lock->waiting_lock!= NULL) {// new
            // make current lock is equal to the lock owns with thread that hold the lock
            current_lock = thread_hold_current_lock->waiting_lock;// new
            // make the current lock holder equal to the lock owner
            thread_hold_current_lock = current_lock->holder;// new

          }else {// new
          	break;// new
           }// new
        }
    }


  sema_down (&lock->semaphore);
  lock->holder = thread_current ();

  if(!thread_mlfqs) {;// new
    // that the thread not wait on any locks
    lock->holder->waiting_lock = NULL;;// new
    // insert the lock on locks list in the thread
    list_insert_ordered(&(lock->holder->locks), &(lock->elem),lock_priority_compare, NULL);;// new
  }

}

/* Tries to acquires LOCK and returns true if successful or false
   on failure.  The lock must not already be held by the current
   thread.

   This function will not sleep, so it may be called within an
   interrupt handler. */
bool
lock_try_acquire (struct lock *lock)
{
  bool success;

  ASSERT (lock != NULL);
  ASSERT (!lock_held_by_current_thread (lock));

  success = sema_try_down (&lock->semaphore);
  if (success){
    lock->holder = thread_current ();

    if(!thread_mlfqs) { // new
      // set the lock the waiting for null
      lock->holder->waiting_lock = NULL ;// new
      // insert the lock in locks list in thread
      list_insert_ordered(&(lock->holder->locks), &(lock->elem),lock_priority_compare, NULL);// new
    }
  }
  return success;
}
//*************************************************************************************
/* Releases LOCK, which must be owned by the current thread.

   An interrupt handler cannot acquire a lock, so it does not
   make sense to try to release a lock within an interrupt
   handler. */
void
lock_release (struct lock *lock)
{
  ASSERT (lock != NULL);
  ASSERT (lock_held_by_current_thread (lock));

  lock->holder = NULL;

  sema_up(&lock->semaphore);

  struct thread *current_thread = thread_current();// new
  struct list_elem *front_of_lock_list; // new
  struct lock *max_lock;// new

  if(!thread_mlfqs) { // new
    //Remove the lock from its list
     list_remove(&(lock->elem)); // new
     // check if the list locks is empty
     if (list_empty(&(current_thread->locks))) { // new
       //back to the base priority of the thread
       	change_priority_or_yielding(current_thread, current_thread->base_priority);// new
     }else {// new
        // sort the locks list
         list_sort(&(current_thread->locks), lock_priority_compare, NULL);// new
         // get the front of lock list
         front_of_lock_list = list_front(&(current_thread->locks));// new
         // get the highist lock
         max_lock = list_entry(front_of_lock_list, struct lock, elem);// new
         change_priority_or_yielding(current_thread, max_lock->priority);// new
     }
  }
}

/* Returns true if the current thread holds LOCK, false
   otherwise.  (Note that testing whether some other thread holds
   a lock would be racy.) */
bool
lock_held_by_current_thread (const struct lock *lock)
{
  ASSERT (lock != NULL);

  return lock->holder == thread_current ();
}

/* One semaphore in a list. */
struct semaphore_elem
  {
    struct list_elem elem;              /* List element. */
    struct semaphore semaphore;         /* This semaphore. */
  };

/* Initializes condition variable COND.  A condition variable
   allows one piece of code to signal a condition and cooperating
   code to receive the signal and act upon it. */
void
cond_init (struct condition *cond)
{
  ASSERT (cond != NULL);

  list_init (&cond->waiters);
}

/* Atomically releases LOCK and waits for COND to be signaled by
   some other piece of code.  After COND is signaled, LOCK is
   reacquired before returning.  LOCK must be held before calling
   this function.

   The monitor implemented by this function is "Mesa" style, not
   "Hoare" style, that is, sending and receiving a signal are not
   an atomic operation.  Thus, typically the caller must recheck
   the condition after the wait completes and, if necessary, wait
   again.

   A given condition variable is associated with only a single
   lock, but one lock may be associated with any number of
   condition variables.  That is, there is a one-to-many mapping
   from locks to condition variables.

   This function may sleep, so it must not be called within an
   interrupt handler.  This function may be called with
   interrupts disabled, but interrupts will be turned back on if
   we need to sleep. */
void
cond_wait (struct condition *cond, struct lock *lock)
{
  struct semaphore_elem waiter;

  ASSERT (cond != NULL);
  ASSERT (lock != NULL);
  ASSERT (!intr_context ());
  ASSERT (lock_held_by_current_thread (lock));

  sema_init (&waiter.semaphore, 0);

  //
  if(!thread_mlfqs) { // new
      waiter.semaphore.priority = thread_current()->priority;// new
      list_insert_ordered(&(cond->waiters), &(waiter.elem),sema_priority_compare, NULL);// new
  }
  lock_release (lock);
  sema_down (&waiter.semaphore);
  lock_acquire (lock);
}

/* If any threads are waiting on COND (protected by LOCK), then
   this function signals one of them to wake up from its wait.
   LOCK must be held before calling this function.

   An interrupt handler cannot acquire a lock, so it does not
   make sense to try to signal a condition variable within an
   interrupt handler. */

   //signal highist priority
void
cond_signal (struct condition *cond, struct lock *lock UNUSED)
{
  ASSERT (cond != NULL);
  ASSERT (lock != NULL);
  ASSERT (!intr_context ());
  ASSERT (lock_held_by_current_thread (lock));

  if (!list_empty (&cond->waiters))
    sema_up (&list_entry (list_pop_front (&cond->waiters),
                          struct semaphore_elem, elem)->semaphore);
}

/* Wakes up all threads, if any, waiting on COND (protected by
   LOCK).  LOCK must be held before calling this function.

   An interrupt handler cannot acquire a lock, so it does not
   make sense to try to signal a condition variable within an
   interrupt handler. */
void
cond_broadcast (struct condition *cond, struct lock *lock)
{
  ASSERT (cond != NULL);
  ASSERT (lock != NULL);

  while (!list_empty (&cond->waiters))
    cond_signal (cond, lock);
}
//***************************************************************************************************
static bool lock_priority_compare(const struct list_elem *a, const struct list_elem *b, void *aux UNUSED) {

  ASSERT (a != NULL && b != NULL);
  struct lock *l1  = list_entry(a, struct lock, elem);
  struct lock *l2 = list_entry(b, struct lock, elem);
  return (l1->priority > l2->priority);
}
//***************************************************************************************************
static bool sema_priority_compare(const struct list_elem *a, const struct list_elem *b, void *aux UNUSED) {

  ASSERT (a != NULL && b != NULL);
  struct semaphore_elem *s1 = list_entry(a, struct semaphore_elem, elem);
  struct semaphore_elem *s2 = list_entry(b, struct semaphore_elem, elem);
  return (s1->semaphore.priority > s2->semaphore.priority);
}
