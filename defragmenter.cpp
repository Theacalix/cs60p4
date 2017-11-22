#include "defragmenter.h"
#include "DefragRunner.h"
#include "mynew.h"
#include <cstdlib>

Defragmenter::Defragmenter(DiskDrive *dDrive): diskDrive(dDrive)
{
	
	int arSize = 10;
	diskArray = new DiskBlock*[arSize];
	arIx = 0;
	diskIx = 2;
	inFCtr = 0;
	maxFree = diskDrive->getCapacity() -1;
	int newNext;
	int prevNext;
	int fileNum = 0;
	int maxArItem = arSize + 1; 
	int totFiles = diskDrive->getNumFiles();
	//LinearHashTable <int> yellowPages(0, 200000);
	yellowPages = new int[maxFree + 1];

	next = diskDrive->directory[fileNum].getFirstBlockID(); //look for beginning of first file
	diskDrive->directory[inFCtr].setFirstBlockID(2); //beginning is now 2, since it will be moved to that postition
	inFCtr++;
	fileNum++;

	for(int i = 0; i < arSize; i++) //loop through array and fill with ordered file
	{
		/*newNext = yellowPages.find(next);
		if(newNext == 0) { //element is in original pos
			blockToAr(i);
		}
		else //item got moved
		{
			while(newNext != 0) //check if moved multiple times
			{
				prevNext = newNext;
				newNext = yellowPages.find(prevNext);
			}
			next = prevNext;
			blockToAr(i);
		}*/
		blockToAr(i); //move block to file

		if(next == 1) //if end of file get next file 
		{
			if(fileNum == totFiles)
			{
				maxArItem = i; //tracks the largest intialized value in the array
				break;
			}
			else
			{
				next = diskDrive->directory[fileNum].getFirstBlockID();
				fileNum++;
			}
		}
	} 
	//we're now swiss cheesed, so we'll find the hole in the diskBlock with the largest address.
	maxFree = findEmpty();

	for(int i = fileNum; i < totFiles + 1; ++i) {

		while(next != 1) //move element to disk and grab next in file
		{
			//move next beginning element in file to diskdrive			 
			arToBlock();
		 
			//move next end element in file to RAM, check yellowpages for moved
			
			newNext = yellowPages[next];
			if(newNext == 0) { //element is in original pos
				blockToAr(arIx);
			}
			else //item got moved
			{
				while(newNext != 0) //check if moved multiple times
				{
					prevNext = newNext;
					newNext = yellowPages[prevNext];
				}
				next = prevNext;
				blockToAr(arIx);
			}

			diskIx++;
			arIx = (arIx + 1) % arSize; //keeps arIx within bounds of array
			
		} //ran to end of file 
		if(i < totFiles)
		{
			next = diskDrive->directory[i].getFirstBlockID(); //look for beginning of each file
		}
	}
	//moved sorted last file from RAM to diskDrive
	int pos = arIx;
	for(int i = 0; i < arSize; i++)
	{ 
		arIx = (pos + i) % arSize; //this accounts for starting in the middle of the array and wrapping around to the beginning 
		if(arIx > maxArItem) //checks to ensure not using uninitialized part of array 
			break;
		arToBlock();
		diskIx++;
	}

} // Defragmenter()


void Defragmenter::blockToAr(int pos)
{
	DiskBlock* temp = diskDrive->readDiskBlock(next);
	diskArray[pos] = temp; //put next block in file in array
	diskDrive->FAT[next] = false; //that block is now empty 
	if(next > maxFree) //update maxFree
		maxFree = next;
	next = temp->getNext(); //get next block 
} //moves next block in file to RAM

void Defragmenter::arToBlock()
{

	if(diskDrive->FAT[diskIx]) //diskblock is full
	{
		//move diskblock to end of diskDrive, update hashtable 
		DiskBlock* temp = diskDrive->readDiskBlock(diskIx); //get element 
		diskDrive->writeDiskBlock(temp, maxFree); //move to empty space 
		delete temp; //delete temp
		yellowPages[diskIx] = maxFree; //store change in hash
		try
		{
			maxFree = findEmpty(); //find next empty space
			if(maxFree == 1)
			{
				throw 1;
			}
		}
		catch(int e)
		{
			cout << "FAT full, out of space" << endl;
			exit(0);
		}
	}
	
	diskDrive->writeDiskBlock(diskArray[arIx], diskIx); //write there
	delete diskArray[arIx]; //delete disk from RAM
	diskDrive->FAT[diskIx] = true;
	if(diskArray[arIx]->getNext() == 1) //were at end of a file
	{
		diskDrive->directory[inFCtr].setFirstBlockID(diskIx + 1); //next block is beginning of next file
		inFCtr++;
	}
}

unsigned Defragmenter::findEmpty()
{
	for(int i = maxFree; i >= 0; i--)
	{
		if(!diskDrive->FAT[i])
			return i;
	}
	return 1;
} //look from end of FAT to find empty disk space
