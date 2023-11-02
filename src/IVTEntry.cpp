/*
 * IVTEntry.cpp
 *
 *  Created on: Sep 23, 2020
 *      Author: OS1
 */

#include "IVTEntry.h"
#include "Global.h"
#include <dos.h>

IVTEntry* table[256];

IVTEntry* IVTEntry::getEntry(IVTNo ivtNo) { return table[ivtNo]; }

IVTEntry::IVTEntry(IVTNo ivtNo, pInterrupt IRoutine) {
	lock
#ifndef BCC_BLOCK_IGNORE
	oldRoutine=getvect(ivtNo);
	setvect(ivtNo, IRoutine);
#endif

	myNum=ivtNo;
	table[ivtNo]=this;
	unlock
}

void IVTEntry::setEvent(KernelEv* myEvent) {
	lock
	this->myEvent=myEvent;
	unlock
}
void IVTEntry::signal() { myEvent->signal(); }

void IVTEntry::callOldRoutine() { if (oldRoutine!=0) oldRoutine(); }

IVTEntry::~IVTEntry() {
	lock
#ifndef BCC_BLOCK_IGNORE
	setvect(myNum, oldRoutine);
#endif
	callOldRoutine();
	table[myNum]=0;
	unlock
}
