// Elli Howard and Catherine Abbruzzese
// 10/30/17
// Code Parser and Generator

// Libraries
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "scanner.c"
#include "virtualmachine.c"

// Constants
#define MAX_TABLE_SIZE 100
#define LEXEME_FILE "lexeme.txt"
#define MACHINE_FILE "machinecode.txt"
#define MAX_CODE_LENGTH 500

// Structs
typedef struct{
	int kind; 		// const = 1, var = 2, proc = 3
	char name[12]; 	// name of max 11 char + nullchar
	int val;		// current ascii value
	int level;		// L level
	int addr;		// Memory address
	int proc;		// Calling procedure
} Symbol;

// Parsing vars
Lexeme *currToken;
Lexeme *list[10000];
int currIndex;
int errorFlag;

// Code dev vars
Symbol *table[1000];
Instruction *code[500];
int highest;
int codePointer;
int currReg;
int blockPtr;

// Method Headers
void program();
void block(int level, int currProc);
void statement(int level);
void condition(int level);
void expression(int level);
void term(int level);
void factor(int level);

void emit(int op, int r, int level, int m);
void readIn();
void toFile();
void error(int eType);

// Methods
int parseAndGenerate(){
  // Get all tokens
  readIn();
  
  currToken = NULL;
  
  // Set up vars for parsing
  currIndex = 0;
  errorFlag = 0;
  
  // Set up vars for code generation
  highest = 0;
  codePointer = 0;
  currReg = 0;
  
  // parse program
  program();
  
  // create code document
  toFile();
  
  return errorFlag;
}

void toFile(){
  FILE *output;
  int i;
  
  output = fopen(MACHINE_FILE,"w");
	if(output == NULL){
		printf("Error: Unable to open %s\n",MACHINE_FILE);
		return;
	}
  
  for(i = 0; i < codePointer; i++){
    fprintf(output,"%d %d %d %d\n",code[i]->op,code[i]->r,code[i]->l,code[i]->m);
  }
  
  fclose(output);
}

void readIn(){
	FILE *input;
  	input = fopen(LEXEME_FILE, "r");
  	int number, i = 0;
  	char character;
  	char meme[12];
  	currIndex = 0;
  
  	if(input == NULL){
		printf("Error opening file: %s\n",LEXEME_FILE);
		return;
	}
  
  	while(fscanf(input, "%c", &character) != EOF){
  		meme[0] = character;
  		int i =1;
  		while(character != '|' && character != ' '){
  			fscanf(input, "%c", &character);
  			meme[i] = character;
  			i++;
  		}
  		meme[i-1] = '\0';
  		number = atoi(meme);
  		
    	currToken = malloc(sizeof(Lexeme));
  		switch(number){
     		case 2: 
      			i = 0;
      			currToken->type = 2;
      			while(character != '|'){
          			fscanf(input, "%c", &character);
        			meme[i] = character;
          			i++;
        		}
        		meme[i-1] = '\0';
        		strcpy(currToken->meme, meme);
        		strcpy(currToken->typeName, "identsym");
      			break;
     		case 3: // same as above
				i = 0;
      			currToken->type = 3;
      			while(character != '|'){
          			fscanf(input, "%c", &character);
        			meme[i] = character;
          			i++;
        		}
        		meme[i-1] = '\0';
        		strcpy(currToken->meme, meme);
        		strcpy(currToken->typeName, "numbersym");
      			break;
     		case 4:
      			currToken->type = 4;
      			strcpy(currToken->meme, "+");
     	  		strcpy(currToken->typeName, "plussym");
     	  		break;
     		case 5:
      			currToken->type = 5;
      			strcpy(currToken->meme, "-");
     	  		strcpy(currToken->typeName, "minussym");
     	  		break;
     		case 6:
     			currToken->type = 6;
      			strcpy(currToken->meme, "*");
     	  		strcpy(currToken->typeName, "multsym");
     	  		break;
      		case 7:
     	  		currToken->type = 7;
      			strcpy(currToken->meme, "/");
     	  		strcpy(currToken->typeName, "slashsym");
     	  		break;
      		case 8:
   	    		currToken->type = 8;
      			strcpy(currToken->meme, "odd");
     	  		strcpy(currToken->typeName, "oddsym");
     	  		break;
      		case 9:
        		currToken->type = 9;
      			strcpy(currToken->meme, "=");
     	  		strcpy(currToken->typeName, "eqlsym");
     	  		break;
      		case 10:
        		currToken->type = 10;
      			strcpy(currToken->meme, "<>");
     	  		strcpy(currToken->typeName, "neqsym");
     	  		break;
      		case 11:
    	  		currToken->type = 11;
      			strcpy(currToken->meme, "<");
     	  		strcpy(currToken->typeName, "lessym");
     	  		break;
      		case 12:
        		currToken->type = 12;
      			strcpy(currToken->meme, "<=");
     	  		strcpy(currToken->typeName, "leqsym");
     	  		break;
      		case 13:
        		currToken->type = 13;
      			strcpy(currToken->meme, ">");
     	  		strcpy(currToken->typeName, "gtrsym");
     	  		break;
      		case 14:
    	  		currToken->type = 14;
      			strcpy(currToken->meme, ">=");
     	  		strcpy(currToken->typeName, "geqsym");
     	  		break;
      		case 15:
        		currToken->type = 15;
      			strcpy(currToken->meme, "(");
     	  		strcpy(currToken->typeName, "lparentsym");
     	  		break;
      		case 16:
     	  		currToken->type = 16;
      			strcpy(currToken->meme, ")");
     	  		strcpy(currToken->typeName, "rparentsym");
     	  		break;
      		case 17:
     	  		currToken->type = 17;
      			strcpy(currToken->meme, ",");
     	  		strcpy(currToken->typeName, "commasym");
     	  		break;
      		case 18:
     	  		currToken->type = 18;
      			strcpy(currToken->meme, ";");
     	  		strcpy(currToken->typeName, "semicolonsym");
     	  		break;
      		case 19:
      			currToken->type = 19;
      			strcpy(currToken->meme, ".");
     	  		strcpy(currToken->typeName, "periodsym");
     	  		break;
      		case 20:
     	  		currToken->type = 20;
      			strcpy(currToken->meme, ":=");
     	  		strcpy(currToken->typeName, "becomessym");
     	  		break;
      		case 21:
     	  		currToken->type = 21;
      			strcpy(currToken->meme, "begin");
     	  		strcpy(currToken->typeName, "beginsym");
     	  		break;
      		case 22:
        		currToken->type = 22;
      			strcpy(currToken->meme, "end");
     	  		strcpy(currToken->typeName, "endsym");
     	  		break;
      		case 23:
        		currToken->type = 23;
      			strcpy(currToken->meme, "if");
     	  		strcpy(currToken->typeName, "ifsym");
     	  		break;
      		case 24:
    	  		currToken->type = 24;
      			strcpy(currToken->meme, "then");
     	  		strcpy(currToken->typeName, "thensym");
     	  		break;
      		case 25:
    	  		currToken->type = 25;
      			strcpy(currToken->meme, "while");
     	  		strcpy(currToken->typeName, "whilesym");
     	  		break;
      		case 26:
 	     		currToken->type = 26;
      			strcpy(currToken->meme, "do");
     	  		strcpy(currToken->typeName, "dosym");
     	  		break;
      		case 27:
     	  		currToken->type = 27;
      			strcpy(currToken->meme, "call");
     	  		strcpy(currToken->typeName, "callsym");
     	  		break;
      		case 28:
    	  		currToken->type = 28;
      			strcpy(currToken->meme, "const");
     	  		strcpy(currToken->typeName, "constsym");
     	  		break;
      		case 29:
      			currToken->type = 29;
      			strcpy(currToken->meme, "var");
     	  		strcpy(currToken->typeName, "varsym");
     	  		break;
      		case 30:
    	  		currToken->type = 30;
      			strcpy(currToken->meme, "procedure");
     	  		strcpy(currToken->typeName, "procsym");
     	  		break;
      		case 31:
    	  		currToken->type = 31;
      			strcpy(currToken->meme, "write");
     	  		strcpy(currToken->typeName, "writesym");
     	  		break;
      		case 32:
      			currToken->type = 32;
      			strcpy(currToken->meme, "read");
     	  		strcpy(currToken->typeName, "readsym");
     	  		break;
      		case 33:
    	  		currToken->type = 33;
      			strcpy(currToken->meme, "else");
     	  		strcpy(currToken->typeName, "elsesym");
     	  		break;
    	}
    	list[currIndex] = currToken;
    	currIndex++;
  	}
}

void emit(int op, int r, int level, int m){
 	if(codePointer > MAX_CODE_LENGTH){
    	error(25);  
  	}else{  
		code[codePointer] = malloc(sizeof(Instruction)); 
    	code[codePointer]->op = op; //opcode    
    	code[codePointer]->r = r;
    	code[codePointer]->l = level;// lexicographical level    
    	code[codePointer]->m = m;// modifier    
    	codePointer++;
  	}
}

void program(){

  	// Get Token
	currToken = list[currIndex];
	currIndex++;
	
  	// Parse a block
  	// Start at level 0 with no procedure
	block(0, -1);
	if(errorFlag){
		return;
	}
	
  	// Make sure program ends with a period
	if(currToken->type != 19){  // periodsym
		error(9);
		errorFlag = 1;
		return;
	}
  
  emit(9,0,0,3); // SIO 3 = END Program
}

void printTable(){
	int i = 0;
	Symbol *a;
	for(i = 0; i < highest; i++){
		a = table[i];
		printf("%d: kind=%d name=%s val=%d level=%d addr=%d proc=%d\n",i, a->kind, a->name, a->val, a->level, a->addr, a->proc);
	}
}

void block(int level, int currProc){
	// All stacks start at 4 due to activation records
	int stackPointer = 4;
	
  	int blockVars;
  	// At the start of each block, save the return value, the static link, the dynamic link, and the program counter
  	// If level == 0, we are at the main body of the code, and so all values are zero.
  	if(level == 0){
  		emit(6,0,0,4);
    
  	// Otherwise we store things
  	}else{
    	if(level > 3){
      		printf("Too many levels.\n");
      		errorFlag = 1;
      		return;
    	}
    	
  	}
  	
  	// Read in constants
	if(currToken->type == 28){ // constsym
		do{ // commasym
      		// Set up space for each constant
			table[highest] = malloc(sizeof(Symbol));
    		table[highest]->kind = 1;
    
      		// Get Token
			currToken = list[currIndex];
			currIndex++;
			
      		// Token should be an identifier
			if(currToken->type != 2){ // identsym
				error(4);
				errorFlag = 1;
				return;
      		}
          	
      		// Put constant into table
      		strcpy(table[highest]->name,currToken->meme);
      		table[highest]->level = level;
      		table[highest]->addr = stackPointer;
      		table[highest]->proc = currProc;
          
      		// Get Token
			currToken = list[currIndex];
			currIndex++;
      		// Token should be =
			if(currToken->type != 9){ // equsym
				error(3);
				errorFlag = 1;
				return;
			}
			
      		// Get Token
			currToken = list[currIndex];
			currIndex++;
      		// Token should be a number
			if(currToken->type != 3){ // numbersym
				error(2);
				errorFlag = 1;
				return;
			}
          	
     	 	// set value in table
      		table[highest]->val = atoi(currToken->meme); // get the value
      		// Store the variables only for the base run
      		if(level == 0){
      			emit(1,currReg,0,table[highest]->val); // Read val into a register
      			emit(6,0,0,1); // increment the stack
      			emit(4,currReg,0,stackPointer); // dump register into stack

      		// Otherwise, let curr procedure know that it needs to allocate this
      		// space when it is called
      		}else{
      			table[currProc]->val++;
      		}
      		
      		// dont incremant currReg because we are done with it.
      		blockVars++;
      		stackPointer++;
      		highest++;
			
      		// Get Token
			currToken = list[currIndex];
			currIndex++;
		}while(currToken->type == 17); // commasym
    
    	// Line ends with a semicolon
		if(currToken->type != 18){ // semicolonsym
			error(10);
			errorFlag = 1;
			return;
		}
		
    	// Get Token
		currToken = list[currIndex];
		currIndex++;
	}
  
  	// Read in variables
	if(currToken->type == 29){ // varsym
		do{
      		// Set up space for variable
    		table[highest] = malloc(sizeof(Symbol));
    		table[highest]->kind = 2;
      
      		// Get Token
			currToken = list[currIndex];
			currIndex++;
      
      		// token should be an identifier
			if(currToken->type != 2){ // identsym
				error(4);
				errorFlag = 1;
				return;
			}
          
      		// Put variable into table
      		strcpy(table[highest]->name,currToken->meme); // get var name
      		table[highest]->level = level; // Add in lexographical level
      		table[highest]->addr = stackPointer;
      		table[highest]->proc = currProc;
      		
      		// Store the variables only for the base run
      		if(level == 0){
      			emit(6,0,0,1); // increment stack to store var in

      		// Otherwise, let curr procedure know that it needs to allocate this
      		// space when it is called
      		}else{
      			table[currProc]->val++;
      		}
      		
      		blockVars++;
    		stackPointer++;
      		highest++;
			
      		// Get Token
			currToken = list[currIndex];
			currIndex++;
		}while(currToken->type == 17); // commasym
    
    	// Line should end in a semicolon
		if(currToken->type != 18){ // semicolonsym
			error(10);
			errorFlag = 1;
			return;
		}
		
    	// Get Token
		currToken = list[currIndex];
		currIndex++;
	}
  
  	while(currToken->type == 30){ // procsym
    	currToken = list[currIndex];
		currIndex++;
    
    	// procedure should have a variable name
		if(currToken->type != 2){ // identsym
			error(4);
			errorFlag = 1;
			return;
		}
    	// Set up space for procedure
    	table[highest] = malloc(sizeof(Symbol));
    	table[highest]->kind = 3;
    	strcpy(table[highest]->name,currToken->meme);
    	table[highest]->level = level;
    	table[highest]->addr = codePointer;
    	highest++;
    
    	currToken = list[currIndex];
    	currIndex++;
    
    	// procedure name should be followed by semicolon
		if(currToken->type != 18){ // semicolonsym
			error(6);
			errorFlag = 1;
			return;
		}
    
    	currToken = list[currIndex];
    	currIndex++;
    
    	// Go down one level in the block
    	// Jump to main body of code
    	if(level == 0){
    		blockPtr = codePointer;
  			emit(7,0,0,0);
  		}
    	block(level+1,highest-1);
    
    	currToken = list[currIndex];
    	currIndex++;
    
    	/* DEFUNCT
    	
    	// procedure should end in a semicolon
		if(currToken->type != 18){ // semicolonsym
			error(6);
			errorFlag = 1;
			return;
		}
    
    	currToken = list[currIndex];
    	currIndex++;
    	*/
  	}
  	//printTable();
	
  	// Process statement
	statement(level);
	if(errorFlag){
		return;
	}
	
  	if(level > 0){
    	// emit a return to exit out of the procedure
    	emit(2,0,0,0);
    	// edit the beggining jump
    	code[blockPtr]->m = codePointer;
  }
}
         
void statement(int level){
	// Process assignment statement
	if(currToken->type == 2) { // identsym
    	// Find identifier
    	int i = 0, flag = 1, maxLev = -2;
    	Symbol *currVar;
    	for(i = 0; i < highest; i++){
    		if(strcmp(table[i]->name, currToken->meme) == 0){
    			if(table[i]->level == level){
    				flag = 0;
    				currVar = table[i];
        			break;
    			}else if(table[i]->level < level){
    				flag = 0;
    				if(table[i]->level > maxLev){
    					currVar = table[i];
    				}
    			}
      		}
    	}
    	if (flag){
    		error(11);
      		errorFlag = 1;
      		return;
    	}
      
    	// If the identifier is not a var, we have a problem
    	if(currVar->kind != 2){
      		error(12);
      		errorFlag = 1;
      		return;
    	}
      
    	// Get Token
		currToken = list[currIndex];
		currIndex++;
			
    	// Token should be :=
		if(currToken->type != 20){ // becomesym
			error(13);
			errorFlag = 1;
			return;
		}
		
    	// Get Token
		currToken = list[currIndex];
		currIndex++;
		
    	// Parse expression
		expression(level);
		if(errorFlag){
			return;
		}
    
    	// Store the value of the expression, then get rid of the register containing the value
    	emit(4,currReg-1,level - currVar->level,currVar->addr);
    	currReg--;
  
  	}else if(currToken->type == 27){ // callsym
  		currToken = list[currIndex];
		currIndex++;
  	
  	
  		if(currToken->type != 2) { 
  			error(14);
			errorFlag = 1;
			return;
  		}
  	
    	int i = 0, flag = 1, maxLev = -2;
    	Symbol *currVar;
    
    	for(i = 0; i < highest; i++){
    		if(strcmp(table[i]->name, currToken->meme) == 0){
    			if(table[i]->level == level){
    				flag = 0;
    				currVar = table[i];
        			break;
    			}else if(table[i]->level < level){
    				flag = 0;
    				if(table[i]->level > maxLev){
    					currVar = table[i];
    				}
    			}
      		}
    	}
    	if (flag){
    		error(11);
      		errorFlag = 1;
      		return;
    	}
    
    	if(currVar->kind != 3){
      		error(15);
      		errorFlag = 1;
      		return;
    	}
    
    	int currProc = i;
    	
    	// Emit the call
    	emit(5,0,0,currVar->addr);
    	// Emit the space for all the constants and variables
    	emit(6,0,0,currVar->val);
    	
    	// Save constants for procedure
    	for(int i = 0; i < highest; i++){
    		if(table[i]->kind == 1 && table[i]->proc == currProc){
    			emit(1,currReg,0,table[highest]->val); // Read val into a register
      			emit(6,0,0,1); // increment the stack
      			emit(4,currReg,0,table[highest]->addr); // dump register into stack
      		}
    	}
    
    	currToken = list[currIndex];
		currIndex++;
	
    	if(currToken->type != 18){ // semicolon
			error(10);
			errorFlag = 1;
			return;
		}
  	
  	// Deal with a statement block - no code directly generated here
	}else if(currToken->type == 21){ // beginsym
    	// Get Token
		currToken = list[currIndex];
		currIndex++;
		
    	// Parse statement
		statement(level);
		if(errorFlag){
			return;
		}
		
    	// parse each statement in a block
		while(currToken->type == 18){ // semicolon sym
      		// Get Token
			currToken = list[currIndex];
			currIndex++;
		
      		// Parse statement
			statement(level);
			if(errorFlag){
				return;
			}
		}
    
    	// Token should be end
		if(currToken->type != 22){ // endsym
			error(19);
			errorFlag = 1;
			return;
		}
		
    	// Get Token
		currToken = list[currIndex];
		currIndex++;
    
  	// deal with if-then
	}else if(currToken->type == 23){ // if sym
    // Get Token
		currToken = list[currIndex];
		currIndex++;
      
    // Parse condition
    condition(level);
    if(errorFlag){
      return;
    }
		
    	// Token should be then
		if(currToken->type != 24){ // thensym
			error(16);
			errorFlag = 1;
			return;
		}
    
    	// Get Token
		currToken = list[currIndex];
		currIndex++;
    
    	// Emit placeholder jump, then free that register
    	int tempPtr = codePointer; 
    	emit(8,currReg,0,0); // JPC
    	currReg--;
		
    	// Parse statement
		statement(level);
		if(errorFlag){
			return;
		}
    
    	// Edit Jump to pass the body of the if statement
    	code[tempPtr]->m = codePointer;
    
    	// Deal with potential else
    	if(currToken->type == 33){ //elsesym
    		tempPtr = codePointer;
    		emit(7,0,0,0);
    		
    		currToken = list[currIndex];
			currIndex++;
		
			statement(level);
      		if(errorFlag){
      			return;
      		}
      		
      		code[tempPtr]->m = codePointer;
    	}

  	// Deal with while-do
	}else if(currToken->type == 25){ // whilesym
    	// Save the code index before the conditional
    	int code1 = codePointer;
    
    	// Get Token
		currToken = list[currIndex];
		currIndex++;
		
    	// Parse condition
		condition(level);
		if(errorFlag){
			return;
		}
    
    	// Save the code index after the conditional
		int code2 = codePointer;
    
    	// Create a jump place holder
    	emit(8,currReg,0,0);
    
    	// Token should be do
		if(currToken->type != 26){ // dosym
			error(18);
			errorFlag = 1;
			return;
		}
		
    	// Get token
		currToken = list[currIndex];
		currIndex++;
		
    	// Parse statement
		statement(level);
		if(errorFlag){
			return;
		}
    
    	// Emit a jump to return to the conditional
    	emit(7,currReg,0,code1);
    	// edit the first jump to skip the body of the while
    	code[code2]->m = codePointer;
	}else if(currToken->type == 31){ // writesym
		// Get token
		currToken = list[currIndex];
		currIndex++;
		
		// Token should be idenifer
		if(currToken->type != 2){ // dosym
			error(26);
			errorFlag = 1;
			return;
		}
		
		// Find identifier
    	int i = 0, flag = 1;
   	 	Symbol *currVar;
    	while(flag && i < highest){
    		if (strcmp(table[i]->name, currToken->meme)){
        		flag = 0;
        		break;
      		}
      	i++;
    	}
    	if (i == highest){
      		error(11);
      		errorFlag = 1;
      		return;
    	}else{
      		currVar = table[i];
    	}
      	
      	printf("loading: kind=%d name=%s val=%d level=%d addr=%d proc=%d\n",currVar->kind, currVar->name, currVar->val, currVar->level, currVar->addr, currVar->proc);
      	emit(3,currReg,level - currVar->level,currVar->addr);
    	emit(9,currReg,0,1);
    	
    	// Get token
		currToken = list[currIndex];
		currIndex++;
	}else if(currToken->type == 32){ // readsym
		// Get token
		currToken = list[currIndex];
		currIndex++;
		
		// Token should be identifier
		if(currToken->type != 2){ // dosym
			error(26);
			errorFlag = 1;
			return;
		}
		
		// Get token
		currToken = list[currIndex];
		currIndex++;
	}
}
         
void condition(int level){

  // process odd calls
	if(currToken->type == 8){ // oddsym
    // Get Token
		currToken = list[currIndex];
		currIndex++;
		
    // Parse Expression
		expression(level);
		if(errorFlag){
			return;
		}
    
    // Emit code for odd
    emit(15, currReg-1, currReg-1, 0);
	}else{
    // Parse expression
		expression(level); 
		if(errorFlag){
			return;
		}
    
    // register correct op
		int op = -1;
		switch(currToken->type){
          // eqlsym
          case 9: 
      			op = 17;
          	break;
          // neqsym
          case 10:
      			op = 18;
          	break;
          // lesssym
          case 11:
      			op = 19;
          	break;
          // leqsym
          case 12:
      			op = 20;
          	break;
          // gtrsym
          case 13:
      			op = 21;
          	break;
          // geqsym
          case 14:
      			op = 22;
          	break;
          // Not a relational operator
          default:
          	error(20);
						errorFlag = 1;
						return;
			}
    	
      // Get Token
      currToken = list[currIndex];
      currIndex++;
      
    	// parse expression
      expression(level);
      if(errorFlag){
          return;
      }
    
    	// emit code for the comparison, and then free the op registers
    	emit(op,currReg-2,currReg-2,currReg-1);
    	currReg--;
	}
}

void expression(int level){
  int op = -1;
  printf("Retrieved token %s of type %d\n",currToken->meme,currToken->type);
  if(currToken->type == 4 || currToken->type == 5){ // minus sym or plussym
    if(currToken->type == 5){
      op = 0;
    }
    currToken = list[currIndex];
    currIndex++;
  }
  
  term(level);
  if(errorFlag){
      return;
  }
  // If negative, negate
  if(op == 0){
  	emit(10,currReg-1,currReg-1,0); // NEG
  }
  
 
  while (currToken->type == 4 || currToken->type == 5){ // minussym or plussym
  	if(currToken->type == 4){
  		op = 1;
  	}else{
  		op = 0;
  	}
    currToken = list[currIndex];
    currIndex++;
      
    term(level);
    if(errorFlag){
    	return;
		}
    
    
  	if(op){ // plussym
      emit(11,currReg-2,currReg-2,currReg-1); // ADD
      currReg--;
    }else{
      emit(12,currReg-2,currReg-2,currReg-1); // SUB 
      currReg--;
    }
  }
}

void term(int level){
  factor(level);
  if(errorFlag){
      return;
  }
  
  	
  while(currToken->type == 6 || currToken->type == 7){ //multsym or slashsym
    currToken = list[currIndex];
  	currIndex++;
    
    factor(level);
    if(errorFlag){
      return;
		}
    
    if(currToken->type == 6){
      emit(13,currReg-2,currReg-2,currReg-1); // MUL
      currReg--;
    }else{
      emit(14,currReg-2,currReg-2,currReg-1); // DIV
      currReg--;
    }
  }
}

void factor(int level){
	if(currToken->type == 2){ // identsym
    	// Find identifier
    	int i = 0, flag = 1, maxLev = -2;
    	Symbol *currVar;
    	for(i = 0; i < highest; i++){
    		if(strcmp(table[i]->name, currToken->meme) == 0){
    			if(table[i]->level == level){
    				flag = 0;
    				currVar = table[i];
        			break;
    			}else if(table[i]->level < level){
    				flag = 0;
    				if(table[i]->level > maxLev){
    					currVar = table[i];
    				}
    			}
      		}
    	}
    	if (flag){
    		error(11);
      		errorFlag = 1;
      		return;
    	}
    
    	printf("factor loading: kind=%d name=%s val=%d level=%d addr=%d proc=%d\n",currVar->kind, currVar->name, currVar->val, currVar->level, currVar->addr, currVar->proc);
    	emit(3,currReg,level - currVar->level,currVar->addr);
		currReg++;
    
    	currToken = list[currIndex];
    	currIndex++;
  	}else if(currToken->type == 3){ // number sym
    	emit(1,currReg,0,atoi(currToken->meme));
		currReg++;
    
    	currToken = list[currIndex];
    	currIndex++;
  	}else if(currToken->type == 15){ // lparen
  		currToken = list[currIndex];
    	currIndex++;
    	expression(level);
    	if(errorFlag){
        	return;
    	}
    
    	if(currToken->type == 16){ // r paren
    		error(22);
			errorFlag = 1;
			return;
    	}
    
    	currToken = list[currIndex];
    	currIndex++;
  	}else{
  		error(23);
		errorFlag = 1;
		return;
  	}
}

void error(int eType){
	printf("Syntax Error: ");
	switch(eType){
	case 1:   printf("Use = instead of :=.\n"); break;
	case 2:   printf("= must be followed by a number.\n"); break;
	case 3:   printf("Identifier must be followed by =.\n"); break;
	case 4:   printf("const, var, procedure must be followed by identifier.\n"); break;
	case 5:   printf("Semicolon or comma missing.\n"); break;
	case 6:   printf("Incorrect symbol after procedure declaration.\n"); break;
	case 7:   printf("Statement expected.\n"); break;
	case 8:   printf("Incorrect symbol after statement part in block.\n"); break;
	case 9:   printf("Period expected.\n"); break;
	case 10:  printf("Semicolon between statements missing.\n"); break;
	case 11:  printf("Undeclared identifier.\n"); break;
	case 12:  printf("Assignment to constant or procedure is not allowed.\n"); break;
	case 13:  printf("Assignment operator expected.\n"); break;
	case 14:  printf("call must be followed by an identifier.\n"); break;
	case 15:  printf("Call of a constant or variable is meaningless.\n"); break;
	case 16:  printf("then expected.\n"); break;
	case 17:  printf("Semicolon or } expected.\n"); break;
	case 18:  printf("do expected.\n"); break;
	case 19:  printf("Incorrect symbol following statement.\n"); break;
	case 20:  printf("Relational operator expected.\n"); break;
	case 21:  printf("Expression must not contain a procedure identifier.\n"); break;
	case 22:  printf("Right parenthesis missing.\n"); break;
	case 23:  printf("The preceding factor cannot begin with this symbol.\n"); break;
	case 24:  printf("An expression cannot begin with this symbol.\n"); break;
	case 25:  printf("This number is too large.\n"); break;
	case 26:  printf("Identifier expected\n");break;
	}
}