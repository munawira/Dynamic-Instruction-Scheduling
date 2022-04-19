/*
 * ILP.cc
 *
 *  Created on: May 2, 2012
 *      Author: munawirakotyad
 */


#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <iomanip>
using namespace std;


#include "constants.h"
#include "queues.h"


//Given functions to implement
void Fetch (FILE* trace);
void dispatch();
void issue();
void execute();
void fake_retire();
int Advance_Cycle();
void printScopeInput(int i);


//Values to output:
int numCycles =-1;
int numInstructions=0;

//Limits
int dispatchLimit = 0;
int issueLimit =0;


int main(int argc,char *argv[])
{

	FILE* trace;
	int i;


	if(argc != 4)
	{
		printf("USAGE: sim <S> <N> <traceFile>\n");
		exit(1);
	}
	S = atoi(argv[1]);
	N = atoi(argv[2]);
	trace = fopen(argv[3],"r");

	dispatchLimit = 2*N;
	issueLimit = S;


	//List of instructions in the scheduling queue
	issueList = (long int *)malloc(S*sizeof(long int));
	//List of instructions in the Functional units: Not required as the number of instructions in the functional units is not known before hand
	executeList = (long int*)malloc(N*sizeof(long int));

	for(i=0;i<S;i++)
	{
		issueList[i] = -1;
	}

	for(i=0;i<N;i++)
	{
		executeList[i] = -1;
	}


	for(i=0;i<NUM_ROB_ENTRIES;i++)
	{
		ROB[i].TAG = -1;
		ROB[i].srcReg1 = -1;
		ROB[i].src1_Ready = 0;
		ROB[i].src2_Ready = 0;
		ROB[i].srcReg2 = -1;
		ROB[i].instrReady = 0;
		ROB[i].destReg= -1;
		ROB[i].state = -1;
	}

	for(i=0;i<NUM_REGISTERS;i++)
	{
		regFile[i].ready = 1;
	}


	//Start execution
	do
	{
		//increment counter
		numCycles++;
		fake_retire();
		//cout<<"After Fake retire\n";

		execute();
		//cout<<"After Execute\n";

		issue();
		//cout<<"After Issue\n";

		dispatch();
		//cout<<"After Dispatch\n";



		Fetch(trace);

		//cout<<"After Fetch\n";



	}while(Advance_Cycle());
	//while(numCycles < 50);

	//numCycles--;
	//cout << "NUM CYCLES " << numCycles << " NUM INSTRUCTIONS "<< numInstructions << endl;
/*	cout<< "\nCONFIGURATION\n";
	cout<<"superscalar bandwidth (N) = "<<N<<endl;
	cout<<"dispatch queue size (2*N) = "<<dispatchLimit<<endl;
	cout<<"schedule queue size (S)   = "<<S<<endl;
	cout<<"\nRESULTS\n";
	cout<<"number of instructions = "<<numInstructions<<endl;

	cout<<"number of cycles       = "<<numCycles<<endl;
	cout<<"IPC                    = "<<(float)numInstructions/(float)numCycles<<endl;
*/

	exit(0);

}




//FUNCTION: Fetch()
// Read new instructions from the
// trace as long as 1) you have not
// reached the end-of-file, 2) the
// fetch bandwidth is not exceeded,
// and 3) the dispatch queue is not
// full. Then, for each incoming
// instruction:
// 1) Push the new instruction onto
// the fake-ROB. Initialize the
// instruction’s data structure,
// including setting its state to
// IF.
// 2) Add the instruction to the
// dispatch_list and reserve a
// dispatch queue entry (e.g.,
// increment a count of the number
// of instructions in the dispatch
// queue

void Fetch(FILE* trace)
{

	int fetchCount =0;

	while(dispatchCount < dispatchLimit && TRACE_EMPTY != 1 && fetchCount < N)
	{

		if(fscanf(trace,"%lx %d %ld %ld %ld",&ROB[ROB_TAIL].PC,&ROB[ROB_TAIL].OP,&ROB[ROB_TAIL].destReg,&ROB[ROB_TAIL].srcReg1,&ROB[ROB_TAIL].srcReg2)>0)
		{
			//cout<<"TRACE: "<< ROB[ROB_TAIL].PC << " "<< ROB[ROB_TAIL].OP <<" " <<ROB[ROB_TAIL].destReg <<" "<< ROB[ROB_TAIL].srcReg1 <<" "<< ROB[ROB_TAIL].srcReg2<<endl;

			ROB[ROB_TAIL].state = IF;
			ROB[ROB_TAIL].IF_beginCycle = numCycles;
			ROB[ROB_TAIL].TAG = currentTag++;

			dispatchTail = ROB_TAIL;//Reconsider if this is correct

			dispatchCount++;
			numInstructions++;
			fetchCount++;

			dispatchTail++;
			ROB_TAIL++;
			ROB_EMPTY =0;

			if(dispatchTail== NUM_ROB_ENTRIES)
			{
				dispatchTail = 0;
			}

			if (ROB_TAIL == NUM_ROB_ENTRIES)
				ROB_TAIL = 0;

			if(ROB_TAIL == ROB_HEAD)
				ROB_FULL =1;

		}
		else
		{
			TRACE_EMPTY = 1;
			break;
		}
	}


}


//FUNCTION: Dispatch();
/*
From the dispatch_list, construct a temp list of instructions in the ID
state (don’t include those in the IF state – you must model the
1 cycle fetch latency). Scan the  temp list in ascending order of
tags and, if the scheduling queue is not full, then:
1) Remove the instruction from the dispatch_list and add it to the
issue_list. Reserve a schedule queue entry (e.g. increment a
count of the number of instructions in the scheduling
queue) and free a dispatch queue entry (e.g. decrement a count of
the number of instructions in the dispatch queue).
2) Transition from the ID state to the IS state.
3) Rename source operands by looking up state in the register
file; rename destination operands by updating state in
the register file.&& IDhead >=0
For instructions in the dispatch_list that are in the IF
state, unconditionally transi&& IDhead >=0&& IDhead >=0tion to the ID state (models the 1 cycle
 */



void dispatch()
{
	//cout<<"In Dispatch \n";
	int i,j;

	j = dispatchHead;

	//cout <<"DISPATCH HEAD : "<< dispatchHead << endl;
	//cout <<"ROB HEAD : "<< ROB_HEAD << endl;
	//cout <<"ROB TAIL : "<< ROB_TAIL << endl;
	//cout << "ID HEAD : "<< IDhead << endl;
	//cout <<"ISSUE COUNT : "<< issueCount<<endl;
	//cout << "DISPATCH COUNT : "<< dispatchCount<<endl;



	/*
	for(i=0;i<100;i++)
	{
		cout<< "ROB_STATE : "<< i << " :: "<< ROB[i].state<<endl;
	}
	 */

	/*
	From the dispatch_list, construct a temp list of instructions in the ID
	state (don’t include those in the IF state – you must model the
	1 cycle fetch latency). Scan the  temp list in ascending order of
	tags
	 */
	for(i=0;i<dispatchCount;i++)
	{
		if(ROB[j].state == ID)
		{
			IDhead = j;
			break;
		}
		else
		{
			j++;
			if(j == NUM_ROB_ENTRIES)
				j=0;
		}

	}


	//cout << "I : "<< i << endl;
	//cout<<"In Dispatch 1\n";
	j= IDhead;

	for(i=0;i<dispatchLimit;i++)
	{
		if(ROB[j].state == ID)
		{
			j++;
			if(j == NUM_ROB_ENTRIES)
				j=0;
		}
		else
			break;
	}
	IDtail = j;


	//cout <<"ID TAIL : " << IDtail << "DISPATCH TAIL :  "<<dispatchTail<<endl;




	//if the scheduling queue is not full, then:

	while(issueCount < S && IDhead != IDtail)
	{

		/*
		 * 1) Remove the instruction from the dispatch_list and add it to the
			issue_list. Reserve a schedule queue entry (e.g. increment a
			count of the number of instructions in the scheduling
			queue) and free a dispatch queue entry (e.g. decrement a count of
			the number of instructions in the dispatch queue).
			2) Transition from the ID state to the IS state.
			3) Rename source operands by looking up state in the register
			file; rename destination operands by updating state in
			the register file.&& IDhead >=0
		 */
		issueList[issueTail] = IDhead;
		ROB[IDhead].state = IS;
		ROB[IDhead].ID_duration = numCycles - ROB[IDhead].ID_beginCycle;
		ROB[IDhead].IS_beginCycle = numCycles;

		if(ROB[IDhead].srcReg1 != -1)
		{
			if(regFile[ROB[IDhead].srcReg1].ready)
			{
				ROB[IDhead].src1_Ready = 1;
				//ROB[IDhead].srcReg1Tag = regFile[ROB[IDhead].srcReg1].value;
			}
			else
			{
				ROB[IDhead].src1_Ready = 0;
				ROB[IDhead].srcReg1Tag = regFile[ROB[IDhead].srcReg1].TAG;
			}
		}
		else
			ROB[IDhead].src1_Ready = 1;


		if(ROB[IDhead].srcReg2 != -1)
		{
			if(regFile[ROB[IDhead].srcReg2].ready)
			{
				ROB[IDhead].src2_Ready = 1;
				//ROB[IDhead].srcReg2Tag = regFile[ROB[IDhead].srcReg2].value;
			}
			else
			{
				ROB[IDhead].src2_Ready = 0;
				ROB[IDhead].srcReg2Tag = regFile[ROB[IDhead].srcReg2].TAG;
			}
		}
		else
			ROB[IDhead].src2_Ready = 1;

		if(ROB[IDhead].destReg != -1)
		{
			regFile[ROB[IDhead].destReg].ready = 0;
			regFile[ROB[IDhead].destReg].TAG = ROB[IDhead].TAG;
		}



		issueTail++;
		issueCount++;
		dispatchHead++;
		IDhead++;

		if(IDhead == NUM_ROB_ENTRIES)
			IDhead =0;

		if(dispatchHead == NUM_ROB_ENTRIES)
			dispatchHead = 0;

		/*if(issueTail == S)
		{
			issueTail = 0;
		}*/

		//Decrement the value of Dispatch Count
		dispatchCount--;
	}
	/*
	For instructions in the dispatch_list that are in the IF
	state, unconditionally transi&& IDhead >=0&& IDhead >=0tion to the ID state (models the 1 cycle
	 */

	j = dispatchHead;
	for(i=0;i<dispatchLimit;i++)
	{
		if(ROB[j].state == IF)
		{
			ROB[j].state = ID;
			ROB[j].IF_duration = numCycles - ROB[j].IF_beginCycle;
			ROB[j].ID_beginCycle = numCycles;

			j++;
			if(j == NUM_ROB_ENTRIES)
				j = 0;
			//fetchCount--;
		}
		else
		{
			j++;
			if(j == NUM_ROB_ENTRIES)
				j = 0;
		}
	}

	//cout<<"DISPATCH OVER \n";
}


//FUNCTION : ISSUE()
/*From the issue_list, construct a
temp list of instructions whose
operands are ready – these are the
 READY instructions. Scan the READY
instructions in ascending order of
tags and issue up to N of them.
To issue an instruction:
1) Remove the instruction from the
issue_list and add it to the
execute_list.
2) Transition from the IS state to
the EX state.
3) Free up the scheduling queue
entry (e.g., decrement a count
of the number of instructions in
the scheduling queue)
4) Set a timer in the instruction’s
 data structure that will allow
you to model the execution
latency.*/


void issue()
{
	int j,i=0;
	executeCount =0;

	int temp =0;


	//cout<<"ISSUE LIST START: "<< issueList[0]<<endl;

	//cout << "Conyents of Issue List\n";
	//for(i=0;i<issueLimit;i++)
	//{

	//cout << "ISSUE LIST : "<< i << "  :: "<<issueList[i]<<endl;
	//}
	i=0;

	while(temp < issueLimit && executeCount < N)

	{
		temp++;

		//cout <<"Inside ISSUE \n";

		if(issueList[i] != -1)
		{
			//cout<<"Inside Issue 1\n";

			if(ROB[issueList[i]].src1_Ready && ROB[issueList[i]].src2_Ready)
			{
				ROB[issueList[i]].state = EX;
				ROB[issueList[i]].IS_duration = numCycles - ROB[issueList[i]].IS_beginCycle;
				ROB[issueList[i]].EX_beginCycle = numCycles;

				ROB[issueList[i]].instrReady = 1;

				if(ROB[issueList[i]].OP == INSTR0)
					ROB[issueList[i]].latency = Latency0;
				if(ROB[issueList[i]].OP == INSTR1)
					ROB[issueList[i]].latency = Latency1;
				if(ROB[issueList[i]].OP == INSTR2)
					ROB[issueList[i]].latency = Latency2;

				//executeList[executeTail] = issueList[i];
				//executeTail++;

				//if(executeTail == NUM_ROB_ENTRIES)
				//executeTail =0;

				executeCount++;
				issueCount--;

				/*if((i+1) < issueLimit)
				{
					issueList[i] = issueList[i+1];
					issueList[i+1]=-1;
				}
				else
				{
					issueList[i] =-1;
					break;
				}*/

				for(j=i;(j+1)<issueLimit;j++)
				{
					issueList[j] = issueList[j+1];
				}
				issueList[j] = -1;

				//issueTail--;
			}
			else
			{
				i++;
			}

		}
		else
		{
			/*if((i+1) < issueLimit)
			{
				issueList[i] = issueList[i+1];
				issueList[i+1]= -1;
			}
			else
			{
				issueList[i] = -1;
				break;
			}*/
			for(j=i;(j+1)<issueLimit;j++)
			{
				issueList[j] = issueList[j+1];
			}
			issueList[j] = -1;
			//issueTail--;
		}
	}

	issueHead =0;
	for(i=issueHead;i<issueLimit;i++)
	{
		if(issueList[i] == -1)
		{
			break;
		}
	}

	issueTail =i;
}

//Function : execute()
//From the execute_list, check for
//instructions that are finishing
//execution this cycle, and:
//1) Remove the instruction from
//the execute_list.
//2) Transition from EX state to
//WB state.
//3) Update the register file state
//(e.g., ready flag) and wakeup
//dependent instructions (set their
//operand ready flags).


void execute()
{
	executeHead =0;

	int i = 0;
	int readyTag = 0;
	int j=0;

	for(i=0;i<NUM_ROB_ENTRIES;i++)
	{
		if(ROB[i].state == EX)
			ROB[i].latency--;
	}
	i=0;

	while(i < NUM_ROB_ENTRIES)
	{
		if(ROB[i].state == EX && ROB[i].latency == 0)
		{
			ROB[i].state = WB;
			ROB[i].EX_duration = numCycles - ROB[i].EX_beginCycle;
			ROB[i].WB_beginCycle = numCycles;
			readyTag = ROB[i].TAG;
			//check for issue lIst contents if any instructions are dependent on this instruction

			for(j=issueHead;j<issueLimit;j++)
			{
				if(issueList[j] != -1)
				{
					if(ROB[issueList[j]].src1_Ready != 1 &&  ROB[issueList[j]].srcReg1Tag == readyTag )
					{
						ROB[issueList[j]].src1_Ready =1;
					}
					if(ROB[issueList[j]].src2_Ready != 1 &&  ROB[issueList[j]].srcReg2Tag == readyTag )
					{
						ROB[issueList[j]].src2_Ready =1;
					}
				}
			}

			if(regFile[ROB[i].destReg].TAG == readyTag)
			{
				regFile[ROB[i].destReg].ready = 1;
			}

			i++;

		}
		else
			i++;


	}



}


// Remove instructions from the head of
// the fake-ROB until an instruction is
// reached that is not in the WB state.
void fake_retire()
{
	int i =ROB_HEAD;
	int startFlag =0;

	while(ROB[i].state == WB)
	{
		ROB[i].state = -1;
		ROB[i].WB_duration = numCycles - ROB[i].WB_beginCycle;

		printScopeInput(i);

		ROB_HEAD++;
		if(ROB_HEAD == NUM_ROB_ENTRIES)
			ROB_HEAD =0;

		ROB_FULL = 0;

		if(ROB_HEAD == ROB_TAIL)
		{
			ROB_EMPTY = 1;
			break;
		}
		i++;

	}
}

// Advance_Cycle performs several functions.
//First, if you want to use the scope tool
//(below), then it checks for instructions
//that changed states and maintains a history
//of these transitions. When an instruction is
//removed from the fake-ROB –- FakeRetire()
//function -- you can dump out this timing
//history in the format read by the scope
//tool. Second, it advances the simulator
//cycle. Third, when it becomes known that the
//fake-ROB is empty AND the trace is depleted,
//the function returns “false” to terminate
//the loop.
int Advance_Cycle()
{

	if(TRACE_EMPTY && ROB_EMPTY)
	{
		return 0;
	}
	else
		return 1;
}


void printScopeInput(int i)
{

	/*0 fu{2} src{29,-1} dst{21} IF{0,1} ID{1,1} IS{2,1} EX{3,5} WB{8,1}*/

	//cout <<currentTag<<" fu{"<<ROB[i].OP<<"} "<<" src{"<<29,-1} dst{21} IF{0,1} ID{1,1} IS{2,1} EX{3,5} WB{8,1}

	printf("%ld fu{%d} src{%ld,%ld} dst{%ld} IF{%d,%d} ID{%d,%d} IS{%d,%d} EX{%d,%d} WB{%d,1}\n", ROB[i].TAG,ROB[i].OP,ROB[i].srcReg1,ROB[i].srcReg2,ROB[i].destReg,ROB[i].IF_beginCycle,ROB[i].IF_duration,ROB[i].ID_beginCycle,ROB[i].ID_duration,ROB[i].IS_beginCycle,ROB[i].IS_duration,ROB[i].EX_beginCycle,ROB[i].EX_duration,ROB[i].WB_beginCycle);


}

