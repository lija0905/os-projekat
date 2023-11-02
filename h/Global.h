/*
 * Global.h
 *
 *  Created on: Sep 9, 2020
 *      Author: OS1
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_
#include "List.h"
#include "KERNEL.h"

extern volatile List<PCB*> all;
extern volatile List<PCB*> waiting;
extern volatile List<KernelSem*> semaphores;

#define lock asm cli;
#define unlock asm sti;
#endif /* GLOBAL_H_ */
