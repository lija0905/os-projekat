// PCB.h

#ifndef PCB_H_
#define PCB_H_

#include "thread.h"
#include "List.h"
#include <dos.h>

extern volatile int handlingSignal;

class PCB {
public:
	static ID poslID;
	static volatile PCB* running;
	PCB(Time timeSlice, int);//konstruktor za main nit
	PCB(StackSize stackSize=defaultStackSize, Time timeSlice=defaultTimeSlice, Thread* myThread);
	PCB(StackSize stackSize=defaultStackSize, Time timeSlice=1, void (*body)());//konstruktor za pomocnu nit
	static void wrapper();
	~PCB();

	void signal(SignalId signal);
	void registerHandler(SignalId signal, SignalHandler handler);
	void unregisterAllHandlers(SignalId id);
	void swap(SignalId id, SignalHandler hand1, SignalHandler hand2);
	void blockSignal(SignalId signal);
	static void blockSignalGlobally(SignalId signal);
	void unblockSignal(SignalId signal);
	static void unblockSignalGlobally(SignalId signal);

	void handleMySignals() volatile;

	unsigned* stack;
	unsigned ss;
	unsigned sp;
	unsigned bp;
	volatile int finished;
	volatile int ready;
	volatile int blocked;
	volatile int waitOnSem;
	ID id;

	Time timeSlice;
	StackSize size;
	Thread* myThread;

	PCB* myParent;
	static const short BLOCKED;
	static SignalId AllSignals[16];

	List<SignalHandler> signalHandlers[16];
	SignalId mySignals[16];
	List<SignalId> registeredSignals;

};
void harakiri();
#endif

