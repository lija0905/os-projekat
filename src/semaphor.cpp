/*
 * semaphor.cpp
 *
 *  Created on: Sep 14, 2020
 *      Author: OS1
 */


#include "KERNEL.h"
#include "Global.h"

Semaphore::Semaphore(int init) {
    lock
	myImpl=new KernelSem(init,this);
    semaphores.add(myImpl);
    unlock
}

int Semaphore::signal(int init) {
	return myImpl->signal(init);
}

int Semaphore::wait(Time maxTimeToWait) {
	return myImpl->wait(maxTimeToWait);
}

int Semaphore::val() const {
	return myImpl->getVal();
}

Semaphore::~Semaphore() {
	lock
	semaphores.remove(myImpl);
	if (myImpl) delete myImpl;
	myImpl=0;
	unlock
}
