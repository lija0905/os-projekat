/*
 * KERNELEV.CPP
 *
 *  Created on: Sep 22, 2020
 *      Author: OS1
 */

#include "KERNELEV.h"
#include "Global.h"
#include "SCHEDULE.h"
#include "IVTEntry.h"

KernelEv::KernelEv(IVTNo ivtNo) {
    lock
	val=1;
    boss=(PCB*)PCB::running;
    myEntry=table[ivtNo];
    myEntry->setEvent(this);
    unlock
}

void KernelEv::wait() {
	if ((PCB*)PCB::running!=boss) return;
	lock
	if (val) {
		PCB::running->blocked=1;
		val=0;
		unlock
		dispatch();
	}
	else unlock
}

void KernelEv::signal() {
	lock
	if (!val) {
		/*asm cli;
		cout << "UNBLOCK" << endl;
		asm sti;*/
		boss->blocked=0;
		boss->ready=1;
		Scheduler::put(boss);
		val=1;
	}
	unlock
	dispatch();
}

KernelEv::~KernelEv() {
	lock
	signal();
	boss=0;
	unlock
}
