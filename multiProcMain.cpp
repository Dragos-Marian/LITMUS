#include "multiProc.h"
#define MAXNOPROC 100

int main(int argc, char *argv[])
{
	readTasks(argv[1]);
	if (!checkProcessor()) {
		std::cout << "More number of processors/cores are required to execute the task-set.\n";
		exit(0);
	}
	std::cout << "Processors used: " << procReqd << "\n";
	allocateTasks();
	/*for (int i = 0; i < procReqd; i++) {
		decProc.insert(decProc.begin() + i, 1);
		decProcNext.insert(decProcNext.begin() + i, 1);
	}
	std::thread procThread[MAXNOPROC];
	for (int i = 0; i < procReqd; i++) {
		procThread[i] = thread(executeFENP, i);
	}

	for (int i = 0; i < procReqd; i++) {
		procThread[i].join();
	}*/

	for (int i = 0; i < procReqd; i++) {
		executeFENP(i);
	}
	return 0;
}
