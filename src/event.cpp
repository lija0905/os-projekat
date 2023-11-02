/*
 * event.cpp
 *
 *  Created on: Sep 22, 2020
 *      Author: OS1
 */

#include "event.h"
#include "KERNELEV.h"
#include "Global.h"

Event::Event(IVTNo ivtNo) {
	lock
	myImpl=new KernelEv(ivtNo);
	unlock
}

void Event::wait() {
	lock
	myImpl->wait();
	unlock
}

void Event::signal() {
	lock
	myImpl->signal();
	unlock
}

Event::~Event() {
	lock
	//signal();
	if (myImpl) delete myImpl;
	myImpl=0;
	unlock
}
