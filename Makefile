defrag.out : defragmenter.o DefragRunner.o mynew.o  
	g++ -ansi -Wall -g -o defrag.out defragmenter.o DefragRunner.o mynew.o  

defragmenter.o : defragmenter.cpp defragmenter.h LinearProbing.o
	g++ -ansi -Wall -g -c defragmenter.cpp

LinearProbing.o : LinearProbing.cpp LinearProbing.h
	g++ -ansi -Wall -g -c LinearProbing.cpp

DefragRunner.o : DefragRunner.cpp DefragRunner.h mynew.h CPUTimer.h defragmenter.h 
	g++ -ansi -Wall -g -c DefragRunner.cpp

mynew.o : mynew.cpp mynew.h 
	g++ -ansi -Wall -g -c mynew.cpp

clean : 
	rm -f defrag.out defragmenter.o  DefragRunner.o  mynew.o LinearProbing.o  
