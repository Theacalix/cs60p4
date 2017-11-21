// Author Sean Davis
#ifndef defragmenterH
  #define defragmenterH

#include "mynew.h"
#include "DefragRunner.h"
#include "LinearProbing.h"

class Defragmenter
{
	DiskBlock** diskArray;
	DiskDrive* diskDrive;
	unsigned next;
	int arIx;
	int diskIx;
	unsigned maxFree;
	void blockToAr(int pos);
	void arToBlock(LinearHashTable <int> &yellowPages);
	unsigned findEmpty();
public:
	Defragmenter(DiskDrive *dDrive);

}; // class Defragmenter

#endif
