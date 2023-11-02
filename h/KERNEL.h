/*
 * KERNEL.h
 *
 *  Created on: Sep 14, 2020
 *      Author: OS1
 */

#ifndef KERNEL_H_
#define KERNEL_H_

#include "List.h"
#include "PCB.h"
#include "semaphor.h"

class KernelSem {

public:
	volatile List<PCB*> blocked;

	KernelSem(int init, Semaphore* mySem);
	int getVal() const;
	int wait (Time maxTimeToWait);
	int signal(int n);
	void inicVal();
	~KernelSem();

private:
	int val;
};



#endif /* KERNEL_H_ */
