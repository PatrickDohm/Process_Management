Process: process.o
	g++ process.cpp handler.cpp -o process


clean:
	rm -f process *~