#include "defragmenter.h"
#include "DefragRunner.h"
#include "mynew.h"
#include <cstdlib>

Defragmenter::Defragmenter(DiskDrive *diskDrive)
{
	DiskBlock** diskArray = new DiskBlock*[5000];
	unsigned next;
	int arIx = 0;
	int diskIx = 2;
	unsigned newPos;

	next = diskDrive->directory[0].getFirstBlockID(); //look for beginning of each file
		
	for(int i = 0; i < 5000 && next != 1; i++) //loop through array and fill with ordered file
	{
		blockToAr(&diskArray, i, next, diskDrive);
	} 

	for(int i = 1; i < diskDrive->getNumFiles(); ++i) {

		while(next != 1) //move element to disk and grab next in file
		{
			//move next beginning element in file to diskdrive			 
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
			}
			
			diskDrive->writeDiskBlock(diskArray[arIx], diskIx); //write there
			delete diskArray[arIx]; //delete disk from RAM
			diskDrive->FAT[diskIx] = true;
		 
			//move next end element in file to RAM
			blockToAr(&diskArray, arIx, next, diskDrive);
			diskIx++;
			if(arIx >= 5000)
				arIx = 0;

			else 
				arIx++;
			
		} //ran to end of file 
		next = diskDrive->directory[i].getFirstBlockID(); //look for beginning of each file
	}
} // Defragmenter()


void Defragmenter::blockToAr(DiskBlock*** diskArray, int pos, unsigned &next, DiskDrive *diskDrive)
{
	DiskBlock* temp = diskDrive->readDiskBlock(next);
	(*diskArray)[pos] = temp; //put next block in file in array
	diskDrive->FAT[next] = false; //that block is now empty 
	next = temp->getNext(); //get next block 
} //moves next block in file to RAM

unsigned Defragmenter::findEmpty(DiskDrive* diskDrive)
{
	/*for(int i = diskDrive->getCapacity() - 1; i >= 0; i--)
	{
		if(!diskDrive->FAT[i])
			return i;
	}*/
	return 1;
} //look from end of FAT to find empty disk space
