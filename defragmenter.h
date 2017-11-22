// Author Sean Davis
#ifndef defragmenterH
  #define defragmenterH

#include "mynew.h"
#include "DefragRunner.h"

class Defragmenter
{
	DiskBlock** diskArray;
	DiskDrive* diskDrive;
	int* yellowPages;
	unsigned next;
	int arIx;
	int diskIx;
	int inFCtr;
	unsigned maxFree;
	void blockToAr(int pos);
	void arToBlock();
	unsigned findEmpty();
public:
	Defragmenter(DiskDrive *dDrive);

}; // class Defragmenter

#endif
