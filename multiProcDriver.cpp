#include "multiProc.h"
#define MAXNOPROC 100
#define DEBUG 1
#define LO 1
#define HI 2

int maxNoOfProcessors;
int procReqd;
int noOfTasks;
int sysCriticality;
int maxCriticality;
int loCriticality = 1;
int hyperPeriod;
std::vector<int> processorTasks;
std::vector<int> periods;
int noOfLoTasks;
std::multimap<pair<double, double>, Task> orderedHiTasks;
std::vector<Task> LoTasks;
Task *LoTaskset;
std::priority_queue<Job> LoJobQ;
double globalNextDecTime;
int procExeCount = 0;
int procInserted = 0;
std::mutex procMutex;
std::mutex decMutex;
std::mutex loQMutex;
std::mutex insertMutex;
std::mutex loQAddMutex;
std::mutex slackMutex;
std::mutex slackFinishMutex;
std::mutex minSlackMutex1;
std::mutex minSlackMutex2;
std::mutex printMutex;
int loopCount1 = 0;
int loopCount2 = 0;
std::vector<int> decProc;
std::vector<int> decProcNext;
int decProcCount = 0;
int decProcCountNext = 0;
int procSlacked = 0;
int slackFinished = 0;
int minSlackCount1 = 0;
int minSlackCount2 = 0;
double minSlack = 0.0;
int minSlackProc;


bool operator <(const Job &a, const Job &b)
{
	if (a.deadline != b.deadline)
		return (a.deadline > b.deadline);
	return (a.currJob < b.currJob);
}

int lcm(int a, int b)
{
	int temp = a;
	while (1) {
		if (temp % b == 0 && temp % a == 0)
			break;
		temp++;
	}
	return temp;
}

void calcHyperPeriod(int n)
{
	int i = 1;
	if(n==0) {
		hyperPeriod = 0;
		return;
	}
	hyperPeriod = periods[0];
	while (i < n) {
		hyperPeriod = lcm(periods[i++], hyperPeriod);
	}
	return;
}

// Functia de calcul a CMMDC a doua numere intregi, pozitive.
int Cmmdc(int nr1, int nr2)
{
	int nr_high, nr_low, result;

	if (nr1 >= nr2) {
		nr_high = nr1;
		nr_low = nr2;
	}
	else {
		nr_high = nr2;
		nr_low = nr1;
	}

	for (result = nr_low; result>1; result--)
	if (((nr_low % result) == 0) && ((nr_high % result) == 0))
		break;

	return result;
}

int partition(Task *taskset, int low, int high)
{
    Task pivot = taskset[high], aux;
    int i = (low - 1);
    for (int j = low; j < high; j++)
    {
        if (taskset[j].period <= pivot.period)
        {
            i++;
            aux = taskset[i];
			taskset[i] = taskset[j];
			taskset[j] = aux;
        }
    }
	aux = taskset[i+1];
	taskset[i+1] = taskset[high];
	taskset[high] = aux;
    return (i + 1);
}

void quickSort(Task *taskset, int low, int high)
{
    if (low < high)
    {
        int pi = partition(taskset, low, high);
        quickSort(taskset, low, pi - 1);
        quickSort(taskset, pi + 1, high);
    }
}

//read tasks from file
void readTasks(char *inFile)
{
	orderedHiTasks.clear();
	LoTasks.clear();
	fstream fileOp(inFile, ios_base::in);
	fileOp >> maxNoOfProcessors >> maxCriticality >> noOfTasks;
	if (maxNoOfProcessors <= 0 || maxCriticality <= 0 || noOfTasks <= 0) {
		std::cout << "1 - Invalid inputs.\n";
		exit(0);
	}

	sysCriticality = LO; //set Lo system mode
	int i = 0;
	noOfLoTasks = 0;
	while (i < noOfTasks) {
		Task taskset;
		taskset.taskNo = i;
		double auxx;
		fileOp >> auxx;
		for (int j = 0; j < maxCriticality; j++) {
			(taskset.arrivalT).push_back(auxx);
		}
		fileOp >> taskset.period >> taskset.taskCriticality;
		if (taskset.arrivalT[sysCriticality-1] < 0 || taskset.period < 0 || taskset.taskCriticality < 0) {
			std::cout << "2 - Invalid inputs.\n";
			exit(0);
		}
		//periods.push_back(taskset.period);
		for (int j = 0; j < maxNoOfProcessors; j++) {
			for(int k = 0; k < maxCriticality; k++){
				double temp;
				fileOp >> temp;
				if (temp < 0) {
					std::cout << "3 - Invalid inputs.\n";
					exit(0);
				}
				taskset.execT[j][k] = temp;
			}
		}
		fileOp >> taskset.deadline;
		for (int j = 0; j < maxNoOfProcessors; j++) {
			int aux;
			fileOp >> aux;
			if (aux > maxNoOfProcessors) {
				std::cout << "4 - Invalid inputs.\n";
				exit(0);
			}
			(taskset.affinity).push_back(aux);
		}

		taskset.processor = -1;
		for (int j = 0; j < maxCriticality; j++) {
			int z = 0;
			(taskset.taskUtil).push_back(z);
		}
		if (taskset.taskCriticality >= sysCriticality) {
			orderedHiTasks.insert(make_pair(make_pair(taskset.period, taskset.arrivalT[sysCriticality-1]), taskset));
		}
		if(taskset.taskCriticality == sysCriticality){
			LoTasks.push_back(taskset);
			noOfLoTasks++;
		}
		i++;
	}
	fileOp.close();
	//calcHyperPeriod();
	return;
}

void populateLoTasks(void)
{
	int noOfLoTasksTemp, maxCriticalityTemp;
	int i = 0;
	ifstream fileOp;
	fileOp.open("inputG", ios_base::in);
	fileOp >> maxCriticalityTemp >> noOfLoTasksTemp;

	LoTaskset = new Task[noOfLoTasks];

	while (i < noOfLoTasks)
	{
		fileOp >> LoTaskset[i].taskNo;
		for (int j = 0; j < maxCriticalityTemp; j++) {
				double temp;
				fileOp >> temp;
				(LoTaskset[i].arrivalT).push_back(temp);
		}
		fileOp >> LoTaskset[i].period >> LoTaskset[i].taskCriticality;
		for (int j = 0; j < maxNoOfProcessors; j++) {
			for(int k = 0; k < maxCriticality; k++){
				double temp;
				fileOp >> temp;
				LoTaskset[i].execT[j][k] = temp;
			}
		}
		fileOp >> LoTaskset[i].deadline;

		for (int j = 0; j < maxNoOfProcessors; j++) {
			int aux;
			fileOp >> aux;
			(LoTaskset[i].affinity).push_back(aux);
		}
		for (int j = 0; j < maxCriticalityTemp; j++) {
			int z = 0;
			(LoTaskset[i].taskUtil).push_back(z);
		}
		LoTaskset[i].processor = -1;
		i++;
	}
	fileOp.close();
	return;
}

//FFD algorithm,  tasks are sorted in decreasing order of their periods
/*int checkProcessor(void)
{
	double U[MAXNOPROC], tempUtil;
	int K[MAXNOPROC];
	processorTasks.clear();
	U[0] = K[0] = 0;
	int q = 0;
	int j;
	for (std::multimap<pair<double, double>, Task>::iterator it = orderedHiTasks.begin(); it != orderedHiTasks.end(); it++) {
		for (j = 0; j <= q; j++) {
			tempUtil = (it->second).execT[j][maxCriticality - 1] / (it->second).period;
			if (U[j] + tempUtil <= 1) {
				U[j] += tempUtil;
				K[j] += 1;
				(it->second).processor = j;
				cout<<"Task "<<(it->second).taskNo<<" to proc "<<(it->second).processor<<" WCET "<<(it->second).execT[(it->second).processor][(it->second).taskCriticality - 1]<<"\n";
				for(int k = 0; k < maxCriticality; k++){
					(it->second).taskUtil[k] = (it->second).execT[(it->second).processor][k] / (it->second).period;
				}
				break;
			}
		}
		if (j > q) {
			if (j + 1 > maxNoOfProcessors) {
				return 0;
			}
			q++;
			U[q] = tempUtil;
			K[q] = 1;
			(it->second).processor = q;
			cout<<"Task "<<(it->second).taskNo<<" to proc "<<(it->second).processor<<" WCET "<<(it->second).execT[(it->second).processor][(it->second).taskCriticality - 1]<<"\n";
			for(int k = 0; k < maxCriticality; k++){
				(it->second).taskUtil[k] = (it->second).execT[(it->second).processor][k] / (it->second).period;
			}
		}
	}
	for(int j = q + 1; j < maxNoOfProcessors; j++){
		U[j] = 0;
		K[j] = 0;
	}
	for (int i = 0; i < maxNoOfProcessors; i++) {
		processorTasks.push_back(K[i]);
	}
	return procReqd = maxNoOfProcessors;
}*/

int mapping_test(int procNo, int period, double execT_Lo, double execT_Hi){
	int i, j, FENPTasks = 0;
	int gcd;	
	Task *taskset = new Task[noOfTasks];

	for (std::multimap<pair<double, double>, Task>::iterator it = orderedHiTasks.begin(); it != orderedHiTasks.end(); it++) {
		if((it->second).processor == procNo){
			taskset[FENPTasks].period = (it->second).period;
			taskset[FENPTasks].execT[procNo][LO - 1] = (it->second).execT[procNo][LO - 1];
			taskset[FENPTasks].execT[procNo][HI - 1] = (it->second).execT[procNo][HI - 1];
			FENPTasks++;
		}
	}
	taskset[FENPTasks].period = period;
	taskset[FENPTasks].execT[procNo][LO - 1] = execT_Lo;
	taskset[FENPTasks].execT[procNo][HI - 1] = execT_Hi;
	FENPTasks++;

	quickSort(taskset, 0, FENPTasks-1);
	for (i = 1; i < FENPTasks; i++){
		for (j = 0; j < i; j++){
			gcd = Cmmdc(taskset[i].period, taskset[j].period);
			if (taskset[i].execT[procNo][LO - 1] + taskset[j].execT[procNo][LO - 1] > gcd || taskset[i].execT[procNo][HI - 1] + taskset[j].execT[procNo][HI - 1] > gcd){
				cout <<"NEGATIVE MAPPING TEST FOR A TASK ON PROCESSOR "<<procNo<<"!!!\n";
				return 0;
			}
		}
	}
	return 1;
}

//P_FENP
int checkProcessor(void)
{
	double U[MAXNOPROC], tempUtil;
	int K[MAXNOPROC];
	processorTasks.clear();
	U[0] = K[0] = 0;
	int q = 0;
	int j;
	for (std::multimap<pair<double, double>, Task>::iterator it = orderedHiTasks.begin(); it != orderedHiTasks.end(); it++) {
		for (j = 0; j <= q; j++) {
			tempUtil = (it->second).execT[j][maxCriticality - 1] / (it->second).period;
			if (U[j] + tempUtil <= 1 && mapping_test(j,(it->second).period,(it->second).execT[j][LO - 1],(it->second).execT[j][HI - 1]) == 1 ) {  //si daca e respectata conditia de planificare
				U[j] += tempUtil;
				K[j] += 1;
				(it->second).processor = j;
				cout<<"Task "<<(it->second).taskNo<<" to proc "<<(it->second).processor<<" WCET "<<(it->second).execT[(it->second).processor][(it->second).taskCriticality - 1]<<"\n";
				for(int k = 0; k < maxCriticality; k++){
					(it->second).taskUtil[k] = (it->second).execT[(it->second).processor][k] / (it->second).period;
				}
				break;
			}
		}
		if (j > q) {
			if (j + 1 > maxNoOfProcessors) {
				return 0;
			}
			q++;
			U[q] = tempUtil;
			K[q] = 1;
			(it->second).processor = q;
			cout<<"Task "<<(it->second).taskNo<<" to proc "<<(it->second).processor<<" WCET "<<(it->second).execT[(it->second).processor][(it->second).taskCriticality - 1]<<"\n";
			for(int k = 0; k < maxCriticality; k++){
				(it->second).taskUtil[k] = (it->second).execT[(it->second).processor][k] / (it->second).period;
			}
		}
	}
	for(int j = q + 1; j < maxNoOfProcessors; j++){
		U[j] = 0;
		K[j] = 0;
	}
	for (int i = 0; i < maxNoOfProcessors; i++) {
		processorTasks.push_back(K[i]);
	}
	return procReqd = maxNoOfProcessors;
}

//Best Affinity Fit (BAF)
/*int checkProcessor(void)
{
	double U[MAXNOPROC];
	int K[MAXNOPROC];
	processorTasks.clear();
	for(int i=0; i<maxNoOfProcessors; i++){
		U[i] = K[i] = 0;
	}
	int q = 0;
	int j;
	int assign = 1;
	int index, maxi = 0;
	for (std::multimap<pair<double, double>, Task>::iterator it = orderedHiTasks.begin(); it != orderedHiTasks.end(); it++) {
		assign = 1;
		maxi = 0;
		for (j = 0; j < maxNoOfProcessors; j++) {
			if((it->second).affinity[j] > maxi){
				maxi = (it->second).affinity[j];
				index = j;
			}
		}
		while(U[index] + (it->second).execT[index][maxCriticality - 1] / (it->second).period > 1){
			(it->second).affinity[index] = 0;
			maxi = 0;
			for (int j = 0; j < maxNoOfProcessors; j++) {
				if((it->second).affinity[j] > maxi){
					maxi = (it->second).affinity[j];
					index = j;
				}
			}
			if(maxi == 0){
				std::cout<<"Cannot assign task "<<(it->second).taskNo<<"!\n";
				assign = 0;
				break;
			}
		}
		if(assign == 1){
			U[index] += (it->second).execT[index][maxCriticality - 1] / (it->second).period;
			K[index] += 1;
			(it->second).processor = index;
			std::cout<<"Task "<<(it->second).taskNo<<" to proc "<<(it->second).processor<<" WCET "<<(it->second).execT[(it->second).processor][sysCriticality - 1]<<"\n";
			for(int j = 0; j < maxCriticality; j++){
				(it->second).taskUtil[j] = (it->second).execT[(it->second).processor][j] / (it->second).period;
			}
		}
	}
	for (int i = 0; i < maxNoOfProcessors; i++) {
		processorTasks.push_back(K[i]);
	}
	return procReqd = maxNoOfProcessors;
}*/

//write tasks in processor files
void allocateTasks(void)
{
	int i = 0;
	while (i < procReqd) {
		ofstream outFile;
		std::string fileName("inputP");
		std::string buffer = to_string(i);
		fileName = fileName + buffer;

		outFile.open(fileName.c_str(), ios_base::trunc);
		outFile << maxCriticality << " " << processorTasks[i];
		for (std::multimap<pair<double, double>, Task>::iterator it = orderedHiTasks.begin(); it != orderedHiTasks.end() ; it++) {
			if ((it->second).processor == i) {
				outFile << "\n";
				outFile << (it -> second).taskNo << "\t";
				for (int j = 0; j < maxCriticality; j++) {
					outFile << (it->second).arrivalT[j] << "\t";
				}
				outFile << (it->second).period << "\t" << (it->second).taskCriticality << "\t";
				for (int j = 0; j < maxNoOfProcessors; j++) {
					for(int k = 0; k < maxCriticality; k++){
						outFile << (it->second).execT[j][k] << "\t";
					}
				}
				outFile << (it->second).deadline<<"\t";
				for (int j = 0; j < maxNoOfProcessors; j++) {
					outFile << (it->second).affinity[j] << "\t";
				}
			}
		}
		outFile.close();
		i++;
	}
	ofstream outFile;
	outFile.open("inputG", ios_base::trunc);
	outFile << maxCriticality << " " <<  noOfLoTasks;
	for (std::vector<Task>::iterator it = LoTasks.begin(); it != LoTasks.end() ; it++) {
		outFile << "\n";
		outFile << it->taskNo << "\t";
		for (int j = 0; j < maxCriticality; j++) {
			outFile << it->arrivalT[j] <<"\t";
		}
		outFile << it->period << "\t" << it->taskCriticality << "\t";
		for (int j = 0; j < maxNoOfProcessors; j++) {
			for(int k = 0; k < maxCriticality; k++){
				outFile << it->execT[j][k] << "\t";
			}
		}
		outFile << it->deadline <<"\t";
		for (int j = 0; j < maxNoOfProcessors; j++) {
			outFile << it->affinity[j] << "\t";
		}
	}
	outFile.close();

	return;
}

int MFunc(int i, int t, Task *taskset, int critLevel)
{
	int ttemp;
	int sigma1, sigma2;

	ttemp = (int)((t % taskset[i].period) - taskset[i].arrivalT[critLevel-1]);

	if (ttemp < 0)
		sigma1 = 0;
	else
		sigma1 = 1;

	if ((ttemp - (int)taskset[i].execT[taskset[i].processor][critLevel - 1]) < 0)
		sigma2 = 0;
	else
		sigma2 = 1;

	return (sigma1 - sigma2);
}

int schedulability_test(int procNo, Task *taskset, int noOfFENPTasks, int critLevel){

	int i, j;
	int gcd = 1, min = INT_MAX, rez = 1;

	std::string outputFile("outputP");
	std::string buffer = to_string(procNo);
	outputFile = outputFile + buffer;
	std::string buffer2 = to_string(critLevel);
	outputFile = outputFile + buffer2;

	ofstream fileOut;
	fileOut.open(outputFile.c_str(), ios_base::trunc);

	fileOut << "Processor " << procNo <<" Criticality " << critLevel << "\n-----------\n\n";
	if(noOfFENPTasks == 0){
		fileOut<<" \nProcessor is idle!!!\n";
		return 0;
	}
	fileOut << "\nHyperperiod : " << hyperPeriod << "\n";

	if (DEBUG == 1)
		fileOut <<"\nSchedulability test for processor "<<procNo<<" with "<<noOfFENPTasks<<" tasks.\n";

	quickSort(taskset, 0, noOfFENPTasks-1);
	for (i = 0; i < noOfFENPTasks; i++){
		fileOut <<"Task "<<taskset[i].taskNo<<" period "<<taskset[i].period<<" WCET " << taskset[i].execT[procNo][critLevel-1] << ".\n";
	}

	for (i = 1; i < noOfFENPTasks; i++){
		for (j = 0; j < i; j++){
			gcd = Cmmdc(taskset[i].period, taskset[j].period);		// Calculul CMMDC al perioadelor celor doua ModX-uri.
			if(gcd < min)
				rez = min = gcd;
			if (DEBUG == 1)
			{
				fileOut <<" - GCD of task "<<taskset[i].taskNo<<" and task "<<taskset[j].taskNo<<": "<<gcd<<"\n";
			}

			if (taskset[i].execT[taskset[i].processor][critLevel - 1] + taskset[j].execT[taskset[j].processor][critLevel - 1] > gcd){
				if (DEBUG == 1)
				{
					fileOut <<" 1 - NEGATIVE SCHEDULABILITY TEST!!!\n";
				}
				return -1;
			}
		}
	}
	if (DEBUG == 1)
	{
		fileOut <<" - Pozitive sufficiency test for level "<<critLevel<<"!\n";
		return rez;
	}
	fileOut.close();
}

int ST(int procNo, Task *taskset, int noOfFENPTasks, int critLevel){

	int gcd = 1;
	int Count;						// Contor pentru numararea unitatilor de timp
	int Delta = 0;
	int t = 0;
	// neocupate de maparea ModX-urilor peste un altul
	int StartTime;								// Marcheaza momentul in care incepe un interval
	// liber in mapare
	// Este "-1" la inceputul maparii
	int i,j, planificabil;		// Indici de ModX si un switch care arata daca setul este planificabil
	int k;							// Folosit pentru algoritmul de mapare

	if(schedulability_test(procNo, taskset, noOfFENPTasks,critLevel) == -1)
		return -1;

	std::string outputFile("outputP");
	std::string buffer = to_string(procNo);
	outputFile = outputFile + buffer;
	std::string buffer2 = to_string(critLevel);
	outputFile = outputFile + buffer2;

	ofstream fileOut;
	fileOut.open(outputFile.c_str(), ios_base::app);
	
	if(noOfFENPTasks == 0){
		return 0;
	}
	//de aici
	for (i = 1; i < noOfFENPTasks; i++)
	{
		planificabil = 0;
		Count = 0;
		gcd = 1;
		StartTime = -1;
		for (t = 0; t <= taskset[i].period; t++)
		{
			Delta = 0;
			for (j = 0; j < i; j++){
				if (Delta)
					break;
				gcd = Cmmdc(taskset[i].period, taskset[j].period);
				for (k = 0; k < (taskset[j].period / gcd); k++){
					if (Delta || MFunc(j, t + k*taskset[i].period, taskset, critLevel)){
						Delta = 1;
						break;
					}
				}
			}
			if (Count >= taskset[i].execT[taskset[i].processor][critLevel - 1]){
				taskset[i].arrivalT[critLevel-1] = StartTime;
				planificabil = 1;
				break;
			}
			if (Delta){
				Count = 0;
				StartTime = -1;
			}
			else{
				Count++;
				if (StartTime == -1){
					StartTime = t;
				}
			}

		}
		if (StartTime == -1){
			if (DEBUG == 1)
			{
				fileOut<<" \n2 - NEGATIVE SCHEDULABILITY TEST!!!\n";
			}
			return -1;
		}
		if (!planificabil){
			if (DEBUG == 1)
			{
				fileOut<<" \n3 - NEGATIVE SCHEDULABILITY TEST!!!\n";
			}
			return -1;
		}
	}
	fileOut.close();
	return 0;
}

void schedule(int procNo, Task *taskset, int noOfFENPTasks, int critLevel){

	double currTime = 0.0;
	double totUtil_lo = 0.0;
	double totUtil_hi = 0.0;
	int gcd;

	std::string outputFile("outputP");
	std::string buffer = to_string(procNo);
	outputFile = outputFile + buffer;
	std::string buffer2 = to_string(critLevel);
	outputFile = outputFile + buffer2;

	ofstream fileOut;
	fileOut.open(outputFile.c_str(), ios_base::app);

	if(ST(procNo, taskset, noOfFENPTasks,critLevel) == -1)
		return;
	else gcd = schedulability_test(procNo, taskset, noOfFENPTasks,critLevel);
	
	int i = 0;
	while (i < noOfFENPTasks) {
		totUtil_lo += taskset[i].taskUtil[0];
		if(taskset[i].taskCriticality == HI)
			totUtil_hi += taskset[i].taskUtil[1];
		i++;
	}

	if (totUtil_lo > 1 || totUtil_hi > 1) {
		fileOut << "Unfeasible schedule as total utilization is greater than 1 on processor " << procNo <<".\n";
		return;
	}

	if(noOfFENPTasks == 0){
		return;
	}

	if(critLevel == LO)
		fileOut << "\nLo Utilization : " << totUtil_lo;
	else
		fileOut <<"\nHi Utilization : " << totUtil_hi;

	fileOut << "\n\nThe FENP schedule is as follows:\n";

	//the actual scheduling
	Job *newJob = new Job[noOfFENPTasks];
	Job aux;
	for (int j = 0; j < noOfFENPTasks; j++) {
			newJob[j].taskNo = taskset[j].taskNo;
			newJob[j].jobNo = (int)(taskset[j].arrivalT[critLevel-1] / taskset[j].period);
			newJob[j].period = taskset[j].period;
			newJob[j].arrivalT = taskset[j].arrivalT[critLevel-1];
			newJob[j].remainingT = taskset[j].execT[taskset[j].processor][critLevel - 1];
			newJob[j].deadline = taskset[j].deadline;
			newJob[j].jobCriticality = taskset[j].taskCriticality;
			newJob[j].jobUtil = taskset[j].taskUtil[critLevel-1];
	}

	//file containing task parameters
	std::string paramFile("param");
	ofstream fileParam;
	if(critLevel == 1){
		fileParam.open(paramFile.c_str(), ios_base::trunc);
		fileParam << noOfFENPTasks << " " << hyperPeriod <<"\n";
	}

	while(1){
		//sort jobs by start time in a nondecreasing manner
		for (int i = 0; i < noOfFENPTasks - 1; i++) 
			for(int j = i + 1; j < noOfFENPTasks; j++){
				if(newJob[i].arrivalT > newJob[j].arrivalT){
					aux = newJob[i];
					newJob[i] = newJob[j];
					newJob[j] = aux;
				}
			}
		if(newJob[0].arrivalT + newJob[0].remainingT > hyperPeriod){
			break;
		}
		fileOut << "\nJ_" << newJob[0].taskNo << "," << newJob[0].jobNo << "\t\t" << newJob[0].arrivalT << " to " << newJob[0].arrivalT + newJob[0].remainingT;

		//task parameters + start time
		if(critLevel == 1 && newJob[0].jobNo == 0)
			fileParam <<newJob[0].taskNo<<" "<<newJob[0].period<<" "<<newJob[0].remainingT<<" "<<newJob[0].arrivalT<<"\n";

		newJob[0].jobNo = (int)((newJob[0].period + newJob[0].arrivalT) / newJob[0].period);
		newJob[0].arrivalT += newJob[0].period;
	}
	fileOut.close();
	fileParam.close();
}

void executeFENP(int procNo)
{
	int noOfFENPTasks = 0, noOfFENPTasksHI = 0, maxCriticalityTemp;
	int i = 0, gcd1 = 1, gcd2 = 1;

	periods.clear();

	//read FENP tasks from processor files
	std::string fileName("inputP");
	std::string outputFile("outputP");
	std::string buffer = to_string(procNo);
	fileName = fileName + buffer;
	outputFile = outputFile + buffer;

	ifstream fileOp;
	fileOp.open(fileName.c_str(), ios_base::in);
	fileOp >> maxCriticalityTemp >> noOfFENPTasks;

	Task *taskset = new Task[noOfFENPTasks];
	Task *tasksetHI = new Task[noOfFENPTasks];

	while (i < noOfFENPTasks) {
		fileOp >> taskset[i].taskNo;
		for (int j = 0; j < maxCriticalityTemp; j++) {
			double auxx;
			fileOp >> auxx;
			(taskset[i].arrivalT).push_back(auxx);	
		}
		fileOp >> taskset[i].period >> taskset[i].taskCriticality;
		periods.push_back(taskset[i].period);

		if(taskset[i].taskCriticality == HI){
			tasksetHI[noOfFENPTasksHI].taskNo = taskset[i].taskNo;
			tasksetHI[noOfFENPTasksHI].period = taskset[i].period;
			tasksetHI[noOfFENPTasksHI].taskCriticality = taskset[i].taskCriticality;
		}
		for (int j = 0; j < maxCriticalityTemp; j++) {
			double auxx = 0;
			if(taskset[i].taskCriticality == HI){
				(tasksetHI[noOfFENPTasksHI].arrivalT).push_back(auxx);
			}
		}
		for (int j = 0; j < maxNoOfProcessors; j++) {
			for(int k = 0; k < maxCriticalityTemp; k++){
				double temp;
				fileOp >> temp;
				taskset[i].execT[j][k] = temp;
				if(taskset[i].taskCriticality == HI){
					tasksetHI[noOfFENPTasksHI].execT[j][k] = temp;
				}
			}
		}
		fileOp >> taskset[i].deadline;
		if(taskset[i].taskCriticality == HI){
			tasksetHI[noOfFENPTasksHI].deadline = taskset[i].deadline;
		}
		for (int j = 0; j < maxNoOfProcessors; j++) {
			int aux;
			fileOp >> aux;
			(taskset[i].affinity).push_back(aux);
			if(taskset[i].taskCriticality == HI){
				(tasksetHI[noOfFENPTasksHI].affinity).push_back(aux);
			}
		}
		for(int j = 0; j < maxCriticalityTemp; j++){
			double u = taskset[i].execT[procNo][j] / taskset[i].period;
			(taskset[i].taskUtil).push_back(u);
			if(taskset[i].taskCriticality == HI){
				(tasksetHI[noOfFENPTasksHI].taskUtil).push_back(u);
			}
		}
		taskset[i].processor = procNo;
		if(taskset[i].taskCriticality == HI){
			tasksetHI[noOfFENPTasksHI].processor = procNo;
			noOfFENPTasksHI++;
		}
		i++;
	}
	fileOp.close();
	
	calcHyperPeriod(noOfFENPTasks);

	schedule(procNo, taskset, noOfFENPTasks, LO);
	schedule(procNo, tasksetHI, noOfFENPTasksHI, HI);

	return;
}