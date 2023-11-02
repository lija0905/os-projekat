/*
 * KERNELEV.h
 *
 *  Created on: Sep 22, 2020
 *      Author: OS1
 */

#ifndef KERNELEV_H_
#define KERNELEV_H_

#include "PCB.h"
#include "event.h"

class IVTEntry;

class KernelEv {

public:

	KernelEv(IVTNo ivtNo);
	void signal();
	void wait();
	~KernelEv();

	IVTEntry* myEntry;

private:
	int val;
	PCB* boss;


};

#endif /* KERNELEV_H_ */
