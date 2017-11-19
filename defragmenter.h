// Author Sean Davis
#ifndef defragmenterH
  #define defragmenterH

#include "mynew.h"
#include "DefragRunner.h"

class Defragmenter
{
	void blockToAr(DiskBlock*** diskArray, int pos, unsigned &next, DiskDrive *diskDrive);
	unsigned findEmpty(DiskDrive* diskDrive);
public:
	Defragmenter(DiskDrive *diskDrive);

}; // class Defragmenter

#endif
