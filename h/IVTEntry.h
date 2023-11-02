/*
 * IVTEntry.h
 *
 *  Created on: Sep 23, 2020
 *      Author: OS1
 */

#ifndef IVTENTRY_H_
#define IVTENTRY_H_

#include "KERNELEV.h"

class IVTEntry;
typedef void interrupt(*pInterrupt)(...);

extern IVTEntry* table[256];

#define PREPAREENTRY(numEntry, callOld)\
void interrupt inter##numEntry(...); \
IVTEntry newEntry##numEntry(numEntry, inter##numEntry); \
void interrupt inter##numEntry(...) {\
newEntry##numEntry.signal();\
if (callOld == 1)\
newEntry##numEntry.callOldRoutine();\
}

class IVTEntry {

public:
	static IVTEntry* getEntry(IVTNo ivtNo);
	IVTEntry(IVTNo ivtNo, pInterrupt Iroutine);
	void signal();
	void setEvent(KernelEv* myKernel);
	void callOldRoutine();
	~IVTEntry();

private:
	IVTNo myNum;
	pInterrupt oldRoutine;
	KernelEv* myEvent;
};

#endif /* IVTENTRY_H_ */
