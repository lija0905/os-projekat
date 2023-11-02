/*
 * PCB.cpp
 *
 *  Created on: Sep 7, 2020
 *      Author: OS1
 */

#include "PCB.h"
#include "SCHEDULE.H"
#include "Global.h"

volatile int handlingSignal=0;

const short PCB::BLOCKED=-1;
ID PCB::poslID=0;
SignalId PCB::AllSignals[16]={0};

PCB::PCB(Time timeSlice, int k) { //konstruktor za main nit
	lock
	this->timeSlice=timeSlice;
	this->ready=1;
	this->blocked=0;
	this->finished=0;
	this->bp=sp;
	this->id=0;
	this->waitOnSem=0;
	unlock
}

PCB::PCB(StackSize stackSize, Time timeSlice, Thread* myThread) {
	lock
	size=stackSize/sizeof(unsigned);
	stack=new unsigned[size];

#ifndef BCC_BLOCK_IGNORE
	stack[size-1]=FP_SEG(myThread);
	stack[size-2]=FP_OFF(myThread);
	stack[size-5]=0x200; // cuva s PSW, enablovan I fleg
	stack[size-6]=FP_SEG(PCB::wrapper);
	stack[size-7]=FP_OFF(PCB::wrapper);

	this->sp=FP_OFF(stack+size-16);
	this->ss=FP_SEG(stack+size-16);
	this->bp=sp;

#endif

	this->finished=0;
	this->ready=0;
	this->blocked=0;
	this->waitOnSem=0;
	this->timeSlice=timeSlice;
	this->myThread=myThread;
	this->id=++poslID;

	myParent=(PCB*)PCB::running;
	int i=0;
	while (i < 16) {
	for (int j=0; j < myParent->signalHandlers[i].getSize(); j++ ) {
		this->signalHandlers[i].add(myParent->signalHandlers[i].getIndex(j));
	}
	i++
	}
	this->registerHandler(0, harakiri);
	for (int j=0; j < 16; j++) mySignals[j]=myParent->mySignals[j];
	unlock
}

PCB::PCB(StackSize stackSize, Time timeSlice,  void (*body)()) { //konstruktor za pomocnu nit
	lock
	size=stackSize/sizeof(unsigned);
	stack=new unsigned[size];

#ifndef BCC_BLOCK_IGNORE
	    stack[size-1] =0x200;//setovan I fleg u
	                      // pocetnom PSW-u za nit

		stack[size-2] = FP_SEG(body);
		stack[size-3] = FP_OFF(body);

	    this->sp = FP_OFF(stack+size-12); //svi sacuvani registri
		this->ss = FP_SEG(stack+size-12);
		this->bp = FP_OFF(stack+size-12);

#endif
		this->ready=0;
		this->blocked=0;
		this->finished = 0;
		this->timeSlice=timeSlice;
		this->id=-1;
	unlock
}

void PCB::wrapper() {
	PCB::running->myThread->run();
	lock
	//cout << "kraj" << endl;
    while(waiting.getSize()) {
        PCB* unblock=waiting.takeFirst();
        Scheduler::put(unblock);
    }
    PCB::running->finished=1;
    if (PCB::running->myParent) ((PCB*)PCB::running)->myParent->signal(1);
    ((PCB*)PCB::running)->signal(2);
    unlock
    dispatch();
}

PCB::~PCB() {
lock
if (this->stack!=0) delete[] stack;
this->stack=0;
this->finished=1;
this->blocked=0;
this->ready=0;
unlock
}

//SIGNALI

void PCB::signal(SignalId signal) {
	lock
	AllSignals[signal]=1;
	mySignals[signal]=1;
	registeredSignals.add(signal);

	if ((PCB*)PCB::running==this) {
		unlock
		handleMySignals();
	} else unlock
}

void PCB::registerHandler(SignalId id, SignalHandler handler) {
	lock
	signalHandlers[id].add(handler);
	unlock
}

void PCB::unregisterAllHandlers(SignalId id) {
	lock
	signalHandlers[id].deleteList();
	unlock
}

void PCB::swap(SignalId signal, SignalHandler hand1, SignalHandler hand2) {
	lock
	signalHandlers[signal].swap(hand1, hand2);
	unlock
}

void PCB::blockSignal(SignalId signal) {
	lock
	mySignals[signal]=BLOCKED;
	unlock
}

void PCB::blockSignalGlobally(SignalId signal) {
	lock
	AllSignals[signal]=BLOCKED;
	unlock
}

void PCB::unblockSignal(SignalId signal) {
	lock
	mySignals[signal]=1;
	unlock
}

void PCB::unblockSignalGlobally(SignalId signal) {
	lock
	AllSignals[signal]=1;
	unlock
}

void PCB::handleMySignals() volatile {
	handlingSignal=1;
	int j=0;
	while(j < registeredSignals.getSize()) {
		SignalId signalToHandle=registeredSignals.getIndex(j);
		if (AllSignals[signalToHandle]==1 && mySignals[signalToHandle]==1) {
			 for (int i=0; i < signalHandlers[signalToHandle].getSize(); i++) {
				 SignalHandler handler=signalHandlers[signalToHandle].getIndex(i);
				 handler();
			 }
		 registeredSignals.remove(signalToHandle);
		} else j++;
	}
	handlingSignal=0;
}

void harakiri() {
	lock
	while(waiting.getSize()) {
		PCB* unblock=waiting.takeFirst();
		unblock->blocked=0;
		Scheduler::put(unblock);
	}
	if (PCB::running->myParent) PCB::running->myParent->signal(1);
	PCB::running->finished=1;
	if (handlingSignal==1) handlingSignal=0;
	delete[] PCB::running->stack;
	PCB::running->stack=0;
	unlock
	dispatch();
}
