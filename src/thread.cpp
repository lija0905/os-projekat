/*
 * thread.cpp
 *
 *  Created on: Sep 7, 2020
 *      Author: OS1
 */

#include "thread.h"
#include "PCB.h"
#include "SCHEDULE.H"
#include "Global.h"
#include <stdio.h>
#include <iostream.h>

volatile List<PCB*> all;
volatile List<PCB*> waiting;
//volatile List<KernelSem*> semaphores;

Thread::Thread(StackSize stackSize, Time timeSlice) {
	lock
	myPCB=new PCB(stackSize, timeSlice, this);
	all.add(myPCB);
	unlock
}

ID Thread::getId() { return myPCB->id; }

ID Thread::getRunningId() { return PCB::running->id; }

Thread* Thread::getThreadById(ID id) {
    lock
	for (int i=0; i<all.getSize(); i++) {
		if (all.getIndex(i)->id==id) return all.getIndex(i)->myThread;
	}
    unlock
	return 0;

}

void Thread::start() {
    lock
	if(myPCB->ready==0) {
		myPCB->ready=1;
		Scheduler::put(myPCB);
	}
    unlock
}

void Thread::waitToComplete() {
	lock
//    cout << "wait" << endl;
	//cout << myPCB->id << endl;
	if (PCB::running->id==myPCB->id){
		//cout << "kskskss" << endl;
		unlock;
		return;
	}
	if (myPCB->finished==0) {
	//cout << PCB::running->id << endl;
	PCB::running->blocked=1;
	waiting.add((PCB*)PCB::running);
	dispatch();
  }
	unlock;
}

Thread::~Thread() {
	lock
	all.remove(myPCB);
	if (myPCB) delete myPCB;
	myPCB=0;
	unlock
}

void Thread::signal(SignalId signal) { myPCB->signal(signal); }

void Thread::registerHandler(SignalId signal, SignalHandler handler) { myPCB->registerHandler(signal, handler); }

void Thread::unregisterAllHandlers(SignalId id) { myPCB->unregisterAllHandlers(id); }

void Thread::swap(SignalId signalId, SignalHandler hand1, SignalHandler hand2) { myPCB->swap(signalId, hand1, hand2); }

void Thread::blockSignal(SignalId signal) { myPCB->blockSignal(signal); }

void Thread::unblockSignal(SignalId signal) { myPCB->unblockSignal(signal); }

void Thread::blockSignalGlobally(SignalId signal) { PCB::blockSignalGlobally(signal); }

void Thread::unblockSignalGlobally(SignalId signal) { PCB::unblockSignalGlobally(signal); }
