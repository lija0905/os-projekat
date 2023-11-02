/*
 * hello.cpp
 *
 *  Created on: Apr 1, 2015
 *      Author: OS1
 */

#include <iostream.h>
#include <dos.h>
#include <stdio.h>
#include "thread.h"
#include "SCHEDULE.h"
#include "PCB.h"
#include "List.h"
#include "Semaphore.h"
#include "IVTEntry.h"
#include <Math.h>
#include "Global.h"

#include <DOS.H>
#include <STDIO.H>
#include <stdlib.h>
#include <STDARG.H>

volatile PCB* PCB::running=0;
volatile List<KernelSem*> semaphores;

int syncPrintf(const char *format, ...)
{
	int res;
	va_list args;
	asm cli;
		va_start(args, format);
	res = vprintf(format, args);
	va_end(args);
	asm sti;
		return res;
}

volatile unsigned tsp;
volatile unsigned tss;
volatile unsigned tbp;

volatile int counter = 3;
volatile int switch_context_on_demand = 0;
volatile int lockFlag=1;

void defaultFunction() {
	//cout << "u pomocnoj" << endl;
	while(1);
}

volatile PCB* defaultPCB=new PCB(defaultStackSize, 2, defaultFunction);

void tick();

void interrupt timer(){	// prekidna rutina
	/*flagLock;
	cout << "timer" << endl;
	flagUnlock;
*/
	if (!switch_context_on_demand) {
		tick();
	for (int i=0; i < semaphores.getSize(); i++) {
		KernelSem* current=semaphores.getIndex(i);
		 for (int j=0; j < current->blocked.getSize();) {
               current->blocked.getIndex(j)->waitOnSem-=1;
               if (current->blocked.getIndex(j)->waitOnSem==0) {
            	   current->blocked.getIndex(j)->blocked=0;
            	   current->inicVal();
            	   Scheduler::put(current->blocked.getIndex(j));
            	   current->blocked.remove(current->blocked.getIndex(j));
               } else j++;
		 }
	}
	asm int 60h;
	}

	if (!switch_context_on_demand && counter > 0) {
		counter--;
	}

	if (counter == 0 || switch_context_on_demand) {
      if (lockFlag && !handlingSignal) {
    	switch_context_on_demand=0;
		asm {
			mov tsp, sp
			mov tss, ss
		    mov tbp, bp
		}

		PCB::running->sp = tsp;
		PCB::running->ss = tss;
		PCB::running->bp = tbp;

		/*asm cli;
		cout << "timer" <<  endl;
		asm sti;*/

		/*flagLock;
		cout << "timer" << PCB::running->id << PCB::running->blocked << endl;
		flagUnlock;
*/

		if (PCB::running->ready && !PCB::running->blocked && !PCB::running->finished) {
			/*flagLock;
			cout << "if" << endl;
			flagUnlock;
*/
			Scheduler::put((PCB*)PCB::running);
		}

		PCB::running=Scheduler::get();	// Scheduler

		/*flagLock;
		cout << 5 << endl;
		cout << PCB::running->id << endl;
		flagUnlock*/

		if (PCB::running==0) PCB::running=defaultPCB;

		tsp = PCB::running->sp;
		tss = PCB::running->ss;
		tbp = PCB::running->bp;

		counter = PCB::running->timeSlice;

		/*asm cli;
		cout << 6 << endl;
		asm sti;*/

		asm {
			mov sp, tsp   // restore sp
			mov ss, tss
			mov bp, tbp
		}

		((PCB*)PCB::running)->handleMySignals();

      } else switch_context_on_demand=1;
		/*asm cli;
		cout << 10 << endl;
		asm sti;*/
	}

	// poziv stare prekidne rutine koja se
     // nalazila na 08h, a sad je na 60h
     // poziva se samo kada nije zahtevana promena
     // konteksta – tako se da se stara
     // rutina poziva samo kada je stvarno doslo do prekida

    //cout << 8 << endl;
	//switch_context_on_demand = 0;
}

void dispatch(){ // sinhrona promena konteksta
	asm cli;
	switch_context_on_demand = 1;
	timer();
	asm sti;
}

unsigned oldTimerOFF, oldTimerSEG; // stara prekidna rutina

// postavlja novu prekidnu rutinu
void inic(){
	asm{
		cli
		push es
		push ax

		mov ax,0   //  ; inicijalizuje rutinu za tajmer
		mov es,ax

		mov ax, word ptr es:0022h //; pamti staru rutinu
		mov word ptr oldTimerSEG, ax
		mov ax, word ptr es:0020h
		mov word ptr oldTimerOFF, ax

		mov word ptr es:0022h, seg timer	 //postavlja
		mov word ptr es:0020h, offset timer //novu rutinu

		mov ax, oldTimerSEG	 //	postavlja staru rutinu
		mov word ptr es:0182h, ax //; na int 60h
		mov ax, oldTimerOFF
		mov word ptr es:0180h, ax

		pop ax
		pop es
		sti
	}
}

// vraca staru prekidnu rutinu
void restore(){
	asm {
		cli
		push es
		push ax

		mov ax,0
		mov es,ax


		mov ax, word ptr oldTimerSEG
		mov word ptr es:0022h, ax
		mov ax, word ptr oldTimerOFF
		mov word ptr es:0020h, ax

		pop ax
		pop es
		sti
	}
}

int userMain(int argc, char* argv[]);

int main(int argc, char* argv[]) {

     inic();

	 asm cli;
	 PCB* mainThread=new PCB(2, 0);
	 PCB::running=mainThread;
	 asm sti;

	 userMain(argc, argv);

	 restore();

	 return 0;
}

