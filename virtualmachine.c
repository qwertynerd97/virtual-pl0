// Elli Howard and Catherine Abbruzzese
// 9/21/17
// P-Machine operator for PL/0

// Libraries
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Constants
#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3
#define REGISTER_NUM 16
#define MAX_OUTPUT 100
#define FILENAME "machinecode.txt"
#define OUTPUT_FILE "output.txt"
#define BASE_PTR_START 1
#define STACK_PTR_START 0
#define PRGRM_CTR_START 0

// Structs
typedef struct Instruction{
	int op;
	char opName[3];
	int r;
	int l;
	int m;
}Instruction;

// Variables
int stack[MAX_STACK_HEIGHT];
Instruction *prog[MAX_CODE_LENGTH];
int registers[REGISTER_NUM];
int printer[MAX_OUTPUT];
int basePointer;
int sPtr;
int programCounter;
int printerPointer;

int base(int l, int base){     
	int b1;  
	b1 = base;   
	while (l > 0){    
		b1 = stack[b1 + 2];    
		l--;
	}  
	return b1;
}

int retriveInstructions(){
	FILE *filename;
	int curInstr, i = 0;
	Instruction *instructionList;

	filename = fopen(FILENAME, "r");
	if (filename == NULL){
		printf("Error: Unable to open %s\n", FILENAME);
		return 0;
	}

	while (fscanf(filename, "%d", &curInstr) != EOF){
		instructionList = malloc(sizeof(Instruction));

		instructionList->op = curInstr;

		fscanf(filename, "%d", &curInstr);
		instructionList->r = curInstr;

		fscanf(filename, "%d", &curInstr);
		instructionList->l = curInstr;

		fscanf(filename, "%d", &curInstr);
		instructionList->m = curInstr;
		/*
		switch(instructionList->op){
			case 1:
				strcpy(instructionList->opName, "LIT");
				break;
			case 2:
				strcpy(instructionList->opName, "RTN"); 
				break;
			case 3:
				strcpy(instructionList->opName, "LOD");
				break;
			case 4:
				strcpy(instructionList->opName, "STO");
				break;
			case 5:
				strcpy(instructionList->opName, "CAL");
				break;
			case 6:
				strcpy(instructionList->opName, "INC");
				break;
			case 7:
				strcpy(instructionList->opName, "JMP");
				break;
			case 8:
				strcpy(instructionList->opName, "JPC");
				break;
			case 9:
				strcpy(instructionList->opName, "SIO");
				break;
			case 10:
				strcpy(instructionList->opName, "NEG");
				break;
			case 11:
				strcpy(instructionList->opName, "ADD");
				break;
			case 12:
				strcpy(instructionList->opName, "SUB");
				break;
			case 13:
				strcpy(instructionList->opName, "MUL");
				break;
			case 14:
				strcpy(instructionList->opName, "DIV");
				break;
			case 15:
				strcpy(instructionList->opName, "ODD");
				break;
			case 16:
				strcpy(instructionList->opName, "MOD");
				break;
			case 17:
				strcpy(instructionList->opName, "EQL"); 
				break;
			case 18:
				strcpy(instructionList->opName, "NEQ");
				break;
			case 19:
				strcpy(instructionList->opName, "LSS");
				break;
			case 20:
				strcpy(instructionList->opName, "LEQ");
				break;
			case 21:
				strcpy(instructionList->opName, "GTR");
				break;
			case 22:
				strcpy(instructionList->opName, "GEQ");
				break;
			}
		*/
		prog[i] = instructionList;
		i++;
	}

	fclose(filename);
	printf("\n");
	return i; 
}

int runInstruction(int i, Instruction* operation, FILE *output){
	programCounter++;
	
	switch(operation->op){
		// Case LIT
		case 1:
			registers[operation->r] = operation->m;
			break;
		// Case RTN
		case 2:
			sPtr = basePointer-1;
			basePointer = stack[sPtr+3];
			programCounter = stack[sPtr+4];
			break;
		// Case LOD
		case 3:
			registers[operation->r] = stack[base(operation->l,basePointer)+operation->m];
			break;
		// Case STO
		case 4:
			stack[base(operation->l,basePointer) + operation->m] = registers[operation->r];
			break;
		// Case CAL
		case 5:
			stack[sPtr + 1] = 0;
			stack[sPtr + 2] = base(operation->l,basePointer);
			stack[sPtr + 3] = basePointer;
			stack[sPtr + 4] = programCounter;
			basePointer = sPtr+1;
			programCounter = operation->m;
			// stack pointer?
			break;
		// Case INC
		case 6:
			sPtr = sPtr + operation->m;
			break;
		// Case JMP
		case 7:
			programCounter = operation->m;
			break;
		// Case JPC
		case 8:
			if(registers[operation->r] == 0){
				programCounter = operation->m;
			}
			break;
		// Case SIO
		case 9:
			if(operation->m == 1){
				fprintf(output,"%d\n",registers[operation->r]);
				printer[printerPointer++] = registers[operation->r];
				// TODO
			}else if(operation->m == 2){
				scanf("%d",&registers[operation->r]);
			}else{
				// Placeholder for operation->m == 3
			}
			break;
		// Case NEG 
		case 10:
			registers[operation->r] = -registers[operation->l];
			break;
		// Cat
		// Case ADD
		case 11:
			registers[operation->r] = registers[operation->l] + registers[operation->m];
			break;
		// Case SUB
		case 12:
			registers[operation->r] = registers[operation->l] - registers[operation->m];
			break;
		// Case MUL
		case 13:
			registers[operation->r] = registers[operation->l] * registers[operation->m];
			break;
		// Case DIV
		case 14:
			registers[operation->r] = registers[operation->l] / registers[operation->m];
			break;
		// Case ODD
		case 15:
			registers[operation->r] = registers[operation->l] % 2;
			break;
		// Case MOD
		case 16:
			registers[operation->r] = registers[operation->l] % registers[operation->m];
			break;
		// Case EQL
		case 17:
			registers[operation->r] = registers[operation->l] == registers[operation->m];
			break;
		// Case NEQ
		case 18:
			registers[operation->r] = registers[operation->l] != registers[operation->m];
			break;
		// Case LSS
		case 19:
			registers[operation->r] = registers[operation->l] < registers[operation->m];
			break;
		// Case LEQ
		case 20:
			registers[operation->r] = registers[operation->l] <= registers[operation->m];
			break;
		// Case GTR
		case 21:
			registers[operation->r] = registers[operation->l] > registers[operation->m];
			break;
		// Case GEQ
		case 22:
			registers[operation->r] = registers[operation->l] >= registers[operation->m];
			break;

		// End Cat
	}
  
	if(operation->op == 9 && operation->m == 1){
		// Inline printing
		//printf("%d\n",registers[operation->r]);
	}else if(operation->op == 9 && operation->m == 3){
		return 0;
	}
	return 1;
}

int simulateProgram(){
	Instruction *IR;
	FILE *output;
	int runFlag = 1;
	int numInstructions; 
	int i;
	
	output = fopen(OUTPUT_FILE,"w");
	if(output == NULL){
		printf("Error: Unable to open %s\n",OUTPUT_FILE);
		return -1;
	}
	
	numInstructions = retriveInstructions();
	
	// Base values for all of the standards
	basePointer = BASE_PTR_START;
	sPtr = STACK_PTR_START;
	programCounter = PRGRM_CTR_START;
	printerPointer = 0;
	
	while(runFlag){
		IR = prog[programCounter];
		runFlag = runInstruction(i,IR,output);
	}
	
	fclose(output);
	
	return 0;
}
