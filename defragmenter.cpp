#include "defragmenter.h"
#include "DefragRunner.h"
#include "mynew.h"
#include <cstdlib>
#include "LinearProbing.h"

Defragmenter::Defragmenter(DiskDrive *diskDrive)
{
	int arSize = 5000;
	DiskBlock** diskArray = new DiskBlock*[arSize];
	int next;
	int arIx = 0;
	int diskIx = 2;
	int newNext;
	int prevNext;
	LinearHashTable <int> yellowPages(-1, 200000);

	next = diskDrive->directory[0].getFirstBlockID(); //look for beginning of each file
		
	for(int i = 0; i < arSize && next != 1; i++) //loop through array and fill with ordered file
	{
		newNext = yellowPages.find(next);
		if(newNext == -1) { //element is in original pos
			blockToAr(&diskArray, i, next, diskDrive);
		}
		else //item got moved
		{
			while(newNext != -1) //check if moved multiple times
			{
				prevNext = newNext;
				newNext = yellowPages.find(prevNext);
			}
			next = prevNext;
			blockToAr(&diskArray, i, next, diskDrive);
		}
		
	} 

	for(int i = 1; i < diskDrive->getNumFiles() + 1; ++i) {

		while(next != 1) //move element to disk and grab next in file
		{
			//move next beginning element in file to diskdrive			 
			arToBlock(&diskArray, diskDrive, arIx, diskIx, yellowPages);
		 
			//move next end element in file to RAM
			blockToAr(&diskArray, arIx, next, diskDrive);
			diskIx++;
			if(arIx >= arSize)
				arIx = 0;

			else 
				arIx++;
			
		} //ran to end of file 
		if(i < diskDrive->getNumFiles())
		{
			next = diskDrive->directory[i].getFirstBlockID(); //look for beginning of each file
		}
	}
	//moved sorted last file from RAM to diskDrive
	for(int i = 0; i < arSize; i++)
	{
		int pos = (arIx + i) % arSize;
		arToBlock(&diskArray, diskDrive, pos, diskIx, yellowPages);
		diskIx++;
	}

} // Defragmenter()


void Defragmenter::blockToAr(DiskBlock*** diskArray, int pos, int &next, DiskDrive *diskDrive)
{
	DiskBlock* temp = diskDrive->readDiskBlock(next);
	(*diskArray)[pos] = temp; //put next block in file in array
	diskDrive->FAT[next] = false; //that block is now empty 
	next = temp->getNext(); //get next block 
} //moves next block in file to RAM

void Defragmenter::arToBlock(DiskBlock*** diskArray, DiskDrive* diskDrive, int arIx, int diskIx, LinearHashTable <int> &yellowPages)
{
	unsigned newPos;

	if(diskDrive->FAT[diskIx]) //diskblock is full
	{
		//move diskblock to end of diskDrive, update hashtable 
		try
		{
			newPos = findEmpty(diskDrive); //find last open space
			if(newPos == 1)
			{
				throw 1;
			}
		}
		catch(int e)
		{
			cout << "FAT full, out of space" << endl;
			exit(0);
		}

		DiskBlock* temp = diskDrive->readDiskBlock(diskIx); //get element 
		diskDrive->writeDiskBlock(temp, newPos); //move to empty space 
		delete temp; //delete temp
		//NEED TO STORE CHANGE IN HASH
		yellowPages.insert(diskIx, newPos);
	}
	
	diskDrive->writeDiskBlock(*(diskArray[arIx]), diskIx); //write there
	delete diskArray[arIx]; //delete disk from RAM
	diskDrive->FAT[diskIx] = true;
}

unsigned Defragmenter::findEmpty(DiskDrive* diskDrive)
{
	for(int i = diskDrive->getCapacity() - 1; i >= 0; i--)
	{
		if(!diskDrive->FAT[i])
			return i;
	}
	return 1;
} //look from end of FAT to find empty disk space
