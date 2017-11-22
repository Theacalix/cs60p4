defrag.out : defragmenter.o DefragRunner.o mynew.o  
	g++ -ansi -pg -Wall -g -o defrag.out defragmenter.o DefragRunner.o mynew.o  

defragmenter.o : defragmenter.cpp defragmenter.h 
	g++ -ansi -pg -Wall -g -c defragmenter.cpp

DefragRunner.o : DefragRunner.cpp DefragRunner.h mynew.h CPUTimer.h defragmenter.h 
	g++ -ansi -pg -Wall -g -c DefragRunner.cpp

mynew.o : mynew.cpp mynew.h 
	g++ -ansi -pg -Wall -g -c mynew.cpp

clean : 
	rm -f defrag.out defragmenter.o  DefragRunner.o  mynew.o LinearProbing.o  
