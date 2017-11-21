#include "defragmenter.h"
#include "DefragRunner.h"
#include "mynew.h"
#include <cstdlib>
#include "LinearProbing.h"

Defragmenter::Defragmenter(DiskDrive *dDrive): diskDrive(dDrive)
{
	
	int arSize = 5000;
	diskArray = new DiskBlock*[arSize];
	arIx = 0;
	diskIx = 2;
	maxFree = diskDrive->getCapacity() -1;
	int newNext;
	int prevNext;
	int fileNum = 0;
	int totFiles = diskDrive->getNumFiles();
	LinearHashTable <int> yellowPages(0, 200000);


	next = diskDrive->directory[fileNums].getFirstBlockID(); //look for beginning of each file
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
			arToBlock(yellowPages);
		 
			//move next end element in file to RAM, check yellowpages for moved
			newNext = yellowPages.find(next);
			if(newNext == 0) { //element is in original pos
				blockToAr(arIx);
			}
			else //item got moved
			{
				while(newNext != 0) //check if moved multiple times
				{
					prevNext = newNext;
					newNext = yellowPages.find(prevNext);
				}
				next = prevNext;
				blockToAr(arIx);
			}

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
		int pos = arIx;
		arIx = (pos + i) % arSize;
		arToBlock(yellowPages);
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

void Defragmenter::arToBlock(LinearHashTable <int> &yellowPages)
{

	if(diskDrive->FAT[diskIx]) //diskblock is full
	{
		//move diskblock to end of diskDrive, update hashtable 
		DiskBlock* temp = diskDrive->readDiskBlock(diskIx); //get element 
		diskDrive->writeDiskBlock(temp, maxFree); //move to empty space 
		delete temp; //delete temp
		yellowPages.insert(diskIx, maxFree); //store change in hash
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
