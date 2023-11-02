/*
 * KERNEL.cpp
 *
 *  Created on: Sep 14, 2020
 *      Author: OS1
 */

#include "KERNEL.h"
#include "SCHEDULE.H"
#include "Global.h"

KernelSem::KernelSem(int init, Semaphore* mySem) {
      lock
	  this->val=init;
      unlock
}

int KernelSem::getVal() const { return val; }

int KernelSem::signal(int n) {
	lock
	int ret=0;
	if (n==0) {
		if (blocked.getSize()!=0) {
			PCB* unblock=blocked.getIndex(0);

			unblock->blocked=0;
			blocked.remove(unblock);
			Scheduler::put(unblock);
		}
		val++;
		ret=0;
	}
	else if (n >0 && blocked.getSize()) {
		int number;
		if (n > blocked.getSize()) number=blocked.getSize();
		else number=n;
		int i=0;
		while( i < number) {
			PCB* unblock=blocked.takeFirst();
			 /*asm cli;
					       cout << "odblokiraj " << unblock->id << endl;
					            	               	   asm sti;*/
		    unblock->blocked=0;
			//blocked.remove(unblock);
			Scheduler::put(unblock);
			i++;
		}
		val+=n;
		ret=number;
	} else if (n < 0) ret=n;
	unlock
	return ret;
}

int KernelSem::wait(Time maxTimeToWait) {
	flagLock
	int ret=1;
	val--;
	if (val < 0) {
		/*asm cli;
		cout << "blokiraj" << PCB::running->id << endl;
		asm sti;*/
		PCB::running->waitOnSem=maxTimeToWait;
		PCB::running->blocked=1;
		blocked.add((PCB*)PCB::running);
		flagUnlock
		dispatch();
		if (PCB::running->waitOnSem==0) ret=0;
	} else flagUnlock;

	return ret;

}

void KernelSem::inicVal() { flagLock val++; flagUnlock }

KernelSem::~KernelSem() {

}
