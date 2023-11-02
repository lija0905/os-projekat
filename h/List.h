/*
 * List.h
 *
 *  Created on: Sep 8, 2020
 *      Author: OS1
 */

#ifndef LIST_H_
#define LIST_H_
#include <iostream.h>

extern volatile int lockFlag;

#define flagLock lockFlag=0;
#define flagUnlock lockFlag=1;
#define lock asm {pushf; cli; }
#define unlock asm popf

template<class T>
class List {
public:
	List():size(0), head(0), tail(0) {}
	void add(T t) volatile;
	void remove(T t) volatile;
	int getSize() const volatile;
	T getIndex(int i) volatile;
	T takeFirst() volatile;
	void write();
	void swap(T t1, T t2); //swapovanje za signale
	~List() { deleteList(); }
	void deleteList() volatile;

private:
	struct Elem {
		T t;
		Elem* next;
	};
	Elem* head, *tail;
	int size;
};

template <class T>
void List<T>::add(T t) volatile {
	flagLock;

	Elem* newElem=new Elem();
	newElem->t=t;
	newElem->next=0;

	if (!head) head = newElem;
	else tail->next = newElem;
	tail = newElem;

	size++;
	flagUnlock;
}

template<class T>
void List<T>::remove(T t) volatile {
	flagLock;
	Elem* cur=head;
	Elem* prev=0;

	while(cur!=0) {
		if (cur->t==t) {
			if (cur==head) head=head->next;
			else if (cur==tail) {
				tail=prev;
				prev->next=0;
			}
			else prev->next=cur->next;
			delete cur;
			break;
		}
		prev=cur;
		cur=cur->next;
	}

	size--;

	if(size==0) head=tail=0;
	flagUnlock;
}

template<class T>
inline int List<T>::getSize() const volatile {
	return size;
}

template<class T>
T List<T>::getIndex(int i) volatile {
	flagLock;

	if (i < size) {

	Elem* cur = head;

	for (int j = 0; j < size; j++) {
		if (j == i) {
			flagUnlock;
			return cur->t;
		}
		else cur = cur->next;
	}
	}
	flagUnlock;
	return 0;

}

template<class T>
void List<T>::deleteList() volatile {

	flagLock;
	Elem* old=head;

	while (old!=0) {
		Elem* prev=old;
		old=old->next;
		delete prev;
	}
	head=tail=0;
	size=0;
    flagUnlock;
}



template<class T>
void List<T>::swap(T t1, T t2) {
	flagLock
	if ( size < 2 ) return;

	Elem* first=head;
	Elem* second=head;
	T tmp;

	while(first!=0) {
		if (first->t==t1) break;
		first=first->next;
	}

	if (first) {
		while (second!=0) {
			if (second->t==t2) break;
			second=second->next;
		}
	}

	if (first && second) {
	tmp=first->t;
	first->t=second->t;
	second->t=tmp;
	}

	flagUnlock
}

template <class T>
T List<T>::takeFirst() volatile {

	if (head==0) return 0;

	T first=head->t;
	head=head->next;
	size--;
	if (!head) tail=0;

	return first;
}

template<class T>
void List<T>::write() {

	Elem* cur=head;

	while(cur!=0) {
		cout << cur->t << endl;
		cur=cur->next;
	}
}

#endif /* LIST_H_ */
