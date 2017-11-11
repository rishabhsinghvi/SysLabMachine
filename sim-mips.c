// Author: Aleksa Deric, Noah Sheridan, Peter Stanwicks
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define SINGLE 1
#define BATCH 0
#define REG_NUM 32

int CLK = 0;
int PC = 0;
int BRANCH_PENDING = 0;
int DECODE_UNFINISHED = 0;
int EXECUTE_UNFINISHED = 0;
int MEMORY_UNFINISHED = 0;
int WRITEBACK_REG_NEW = 0;  //new contents in the writeback buffer register for writeback stage to consume

enum opcode {add, addi, sub, mult, beq, lw, sw, halt, noop};

char* correctOpcode[] = {"add", "addi", "sub", "mult", "beq", "lw", "sw", "halt"};


long* arrayPntr;

struct inst
{
	int opcode;
	int rs;
	int rt;
	int rd;
	int Imm;
};

struct buffer
{
	struct inst instruction;
	int readyToRead;
	int readytoWrite;
	int address;
	int wbReg;
	long data;
};

struct buffer IDEX;
struct buffer IFID;
struct inst *IM;  //can we get an intruction count and do malloc later to get exact size?


void IF(void);  							//author: Noah,		tester: Aleksa
void ID(void);								//author: Aleksa,	tester: Noah
void EX(void);								//author: Noah,		tester: Aleksa, Peter
void MEM(void);								//author: Peter,	tester: Aleksa
void WB(void);								//author: Aleksa,	tester: Noah
char *progScannner(char *c); 				//author: Peter,	tester: Noah,  Done and tested
char *regNumberConverter(char *prog); 			//author: Aleksa,	tester:	Peter
struct inst parser(char *input);			//author: Noah,		tester: Peter
//main  									//author: Peter


int main (int argc, char *argv[]){
	int sim_mode=0;//mode flag, 1 for single-cycle, 0 for batch
	int c,m,n;
	int i;//for loop counter
	long mips_reg[REG_NUM];
	long pgm_c=0;//program counter
	long sim_cycle=0;//simulation cycle counter
	//define your own counter for the usage of each pipeline stage here


	int test_counter=0;
	FILE *input=NULL;
	FILE *output=NULL;
	printf("The arguments are:");
	
	for(i=1;i<argc;i++){
		printf("%s ",argv[i]);
	}
	printf("\n");
	if(argc==7){
		if(strcmp("-s",argv[1])==0){
			sim_mode=SINGLE;
		}
		else if(strcmp("-b",argv[1])==0){
			sim_mode=BATCH;
		}
		else{
			printf("Wrong sim mode chosen\n");
			exit(0);
		}
		
		m=atoi(argv[2]);
		n=atoi(argv[3]);
		c=atoi(argv[4]);
		input=fopen(argv[5],"r");
		output=fopen(argv[6],"w");
		
	}
	
	else{
		printf("Usage: ./sim-mips -s m n c input_name output_name (single-sysle mode)\n or \n ./sim-mips -b m n c input_name  output_name(batch mode)\n");
		printf("m,n,c stand for number of cycles needed by multiplication, other operation, and memory access, respectively\n");
		exit(0);
	}
	if(input==NULL){
		printf("Unable to open input or output file\n");
		exit(0);
	}
	if(output==NULL){
		printf("Cannot create output file\n");
		exit(0);
	}
	//initialize registers and program counter
	if(sim_mode==1){
		for (i=0;i<REG_NUM;i++){
			mips_reg[i]=0;
		} 
	}
	
	//start your code from here



}

//
char *progScannner(char *c){

	char *ret; ret = (char *)malloc(strlen(c));
	char t[2];
	char space,open;

	
	for (int i = 0; i < strlen(c); i++){


		if( ('a' <= c[i] && c[i] <= 'z') || ('A' <= c[i] && c[i] <= 'Z') || ('0' <= c[i] && c[i] <= '9')){
			t[0] = c[i];
			t[1] = '\0';
			strcat(ret, t);

			space = 0;
		}else{
			if(space == 0){
				t[0] = ' ';
				t[1] = '\0';
				strcat(ret, t);
			}

			space = 1;
		}


		if(c[i] == '('){
			if(open == 1){
				printf("%s\n", "Mismatched paranthesis");
				exit(0);
			}
			open = 1;
		}else if(c[i] == ')'){
			if(open == 0){
				printf("%s\n", "Mismatched paranthesis");
				exit(0);
			}
			open = 0;
		}


	}
	if(open == 1){
		printf("%s\n", "Mismatched paranthesis");
		exit(0);
	}
	

	return ret;
	free(ret);
}

char *regNumberConverter(char *prog){
	
	char* registers1[] = {"zero", "at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7"};
	char* registers2[] = {"s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7","t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"};	//+16
	
	int len = strlen(prog); 
	int i =0; int last = 0; int end, copy, tempCopy, size, registerNumber, regarg;
	
	
	char* finalString = (char*)malloc(len);
	
	char* temp;
	char* numberTemp;
	
	
	for (i = 0; i < (len);i++){			//parse the string
		
		
		
		if(prog[i] == '$'){		//if $ is found
			
			end = i;
			
			while(!(prog[end]==' ')&&!(prog[end]=='\0')){	//find the edge
				
				end++;
			}
			
			
			temp = (char*)malloc(end-i); 	//temporarily copy the argument in temp
			numberTemp = (char*)malloc(5);
			
			tempCopy = 0;
			
			for (copy = i+1; copy < end; copy++){
				temp[tempCopy] = prog[copy];
				tempCopy++;
			}
			temp[tempCopy] = '\0';
			
			
			
			if (!(strcmp(temp,"0"))){		//check if the argument is 0
				
				sprintf(numberTemp, "%d",0);
				//printf("DIGIT ARGUMENT (zero detected) [%s]\n",numberTemp);
			}
			
			else if(!(atoi(temp) == 0)){	//check if the argument is a number less than 32
				assert(atoi(temp) < 32);
				sprintf(numberTemp, "%d",atoi(temp));
				//printf("DIGIT ARGUMENT [%s]\n",numberTemp);
				
			}
			else{		//check if the argument is a name of one of the registers
			
			//this is ugly and should be changed eventually, i couldnt get it to run with strcmp and arrays
				registerNumber = -1;

					if(!(strcmp("zero",temp))){
						registerNumber = 0;
					}
					else if(!(strcmp("at",temp))){
						registerNumber = 1;
					}
					else if(!(strcmp("v0",temp))){
						registerNumber = 2;
					}
					else if(!(strcmp("v1",temp))){
						registerNumber = 3;
					}
					else if(!(strcmp("a0",temp))){
						registerNumber = 4;
					}
					else if(!(strcmp("a1",temp))){
						registerNumber = 5;
					}
					else if(!(strcmp("a2",temp))){
						registerNumber = 6;
					}
					else if(!(strcmp("a3",temp))){
						registerNumber = 7;
					}
					else if(!(strcmp("t0",temp))){
						registerNumber = 8;
					}
					else if(!(strcmp("t1",temp))){
						registerNumber = 9;
					}
					else if(!(strcmp("t2",temp))){
						registerNumber = 10;
					}
					else if(!(strcmp("t3",temp))){
						registerNumber = 11;
					}
					else if(!(strcmp("t4",temp))){
						registerNumber = 12;
					}
					else if(!(strcmp("t5",temp))){
						registerNumber = 13;
					}
					else if(!(strcmp("t6",temp))){
						registerNumber = 14;
					}
					else if(!(strcmp("t7",temp))){
						registerNumber = 15;
					}
					else if(!(strcmp("s0",temp))){
						registerNumber = 16;
					}
					else if(!(strcmp("s1",temp))){
						registerNumber = 17;
					}
					else if(!(strcmp("s2",temp))){
						registerNumber = 18;
					}
					else if(!(strcmp("s3",temp))){
						registerNumber = 19;
					}
					else if(!(strcmp("s4",temp))){
						registerNumber = 20;
					}
					else if(!(strcmp("s5",temp))){
						registerNumber = 21;
					}
					else if(!(strcmp("s6",temp))){
						registerNumber = 22;
					}
					else if(!(strcmp("s7",temp))){
						registerNumber = 23;
					}
					else if(!(strcmp("t8",temp))){
						registerNumber = 24;
					}
					else if(!(strcmp("t9",temp))){
						registerNumber = 25;
					}
					else if(!(strcmp("k0",temp))){
						registerNumber = 26;
					}
					else if(!(strcmp("k1",temp))){
						registerNumber = 27;
					}
					else if(!(strcmp("gp",temp))){
						registerNumber = 28;
					}
					else if(!(strcmp("sp",temp))){
						registerNumber = 29;
					}
					else if(!(strcmp("fp",temp))){
						registerNumber = 30;
					}
					else if(!(strcmp("ra",temp))){
						registerNumber = 31;
					}
					
				
				assert(registerNumber > -1);			//if no argument found, crash the program
				sprintf(numberTemp, "%d",registerNumber);
				//printf("REGISTER [%s] maps to [%s]\n",temp, numberTemp);
			}
			
			copy = 0;
			
			while (!(numberTemp[copy]=='\0')){		//append the number
				finalString[last] = numberTemp[copy];
				copy++;
				last++;
			}
			
			free(temp);
			free(numberTemp);
			i=end-1;
			
		}
		else{
			finalString[last] = prog[i];
			last++;
		}
		
	}
	finalString[last] = '\0';
	//printf("[%s]\n\n",finalString);

	//printf("TERM\n");
	return finalString;
}


struct inst parser(char *input){

	

	struct inst output;

	/*
	char delimeter[] = " ";
	//char *opcode; opcode = strtok(input, delimeter); 
	for(int i=0; i<9; i++){
		if(i==8){
			printf("Ooop! Something was entered-in incorrectly!");
			exit;
		}
		if (opcode = correctOpcode[i]){
			return;
		}
	}
	
	//enum opcode inst = opcode;
	//output.opcode = inst;
	if(inst == 8){  //halt program command detected
		return ouput;
	}
	if(inst == 0 || inst == 2 || inst == 3){
		output.rs = strtok(NULL, delimeter);  //saving rs value
		output.rt = strtok(NULL, delimeter);  //saving rt value
		output.rd = strtok(NULL, delimeter);  //saving rd value
	}
	else{
		output.rs = strtok(NULL, delimeter);
		output.rt = strtok(NULL, delimeter);
		output.Imm = strtok(NULL, delimeter);
		if(output.Imm>=65536){
			printf("Ooop! Something was entered-in incorrectly!");
			exit;
		}
	}
	//free(inst);
	free(delimeter);
	*/

	return output;
}



void IF(void){
    if(BRANCH_PENDING!=0){//instroduce noop to the system
        CLK++;  //????
        struct inst toBuffer;
        //toBuffer.op = noop;
        IFID.instruction = toBuffer;
        return;
    }
    if(DECODE_UNFINISHED!=0){//do nothing because decode is not finished reading from bufffer
    	CLK++;
    	return;
    }
    IFID.instruction = IM[PC];
    PC++;
    IFID.readyToRead = 1;
    CLK++;
    return;
}

void ID(void){  //please make sure that if opcode is 'halt_program' everything stops and control is returned to main()


}

void EX(void){
	/*
	if(IDEX.instruction.op==noop){
		CLK++; //insert VARIABLE HERE
		EXECUTE_UNFINISHED = 0;
		return;
	}
	if(IDEX.instruction.op==add){
		EXMEM.data = arrayPntr[IDEX.instruction.rs]+arrayPntr[IDEX.instruction.rt];
		EXMEM.wbReg = IDEX.instruction.rd;
		EXMEM.address = -1;  //so you know nothing needs to be written to memory!
		CLK++;
		//add to useful process count
	}
	if(IDEX.instruction.op==addi){
		
	}
	*/
}

void MEM(void){

}

void WB(void){

}

