/*
 * queues.h
 *
 *  Created on: Apr 21, 2012
 *      Author: munawirakotyad
 */

#ifndef QUEUES_H_
#define QUEUES_H_

int ROB_HEAD =0;
int ROB_TAIL =0;

int fetchCount=0;
int dispatchCount =0;
int dispatchHead =0;
int dispatchTail =0;
int IDcount=0;
int IDhead =0;
int IDtail =0;
int issueCount =0;
int issueHead =0;
int issueTail =0;
int executeCount =0;
int executeHead =0;
int executeTail = 0;

enum INSTRUCTION{
	INSTR0 =0,
			INSTR1= 1,
			INSTR2= 2
};

enum INSTR_LATENCY{
	Latency0=1,
			Latency1=2,
			Latency2=5
};

enum EXEC_STATE{
	IF=0,//Fetch
			ID,//Dispatch
			IS,//Issue
			EX,//Execute
			WB//WriteBack
};

typedef struct reOrderBuffer{

	unsigned long PC;
	long int TAG;
	int instrReady;
	int OP;
	int state;
	long int destReg;
	int src1_Ready;
	long int srcReg1;
	long int srcReg1Tag;
	int src2_Ready;
	long int srcReg2;
	long int srcReg2Tag;

	int latency;

	int IF_beginCycle;
	int IF_duration;
	int ID_beginCycle;
	int ID_duration;
	int IS_beginCycle;
	int IS_duration;
	int EX_beginCycle;
	int EX_duration;
	int WB_beginCycle;
	int WB_duration;

}fake_ROB;

fake_ROB ROB[NUM_ROB_ENTRIES];

typedef struct register_files{
	int ready;
	long int TAG;
	long int value;
}registerFile;

registerFile regFile[NUM_REGISTERS];

long int *dispatchList;//Size = 2N
long int *issueList;//Size = S
long int *executeList;//Size = N
long int *IDlist;//Size =N


#endif /* QUEUES_H_ */
