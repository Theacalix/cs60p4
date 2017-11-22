// Author Sean Davis
#ifndef defragmenterH
  #define defragmenterH

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
	int totFiles;
	unsigned maxFree;
	void blockToAr(int pos);
	void arToBlock();
	//unsigned findEmpty();
public:
	Defragmenter(DiskDrive *dDrive);

}; // class Defragmenter

#endif
