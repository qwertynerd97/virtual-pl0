// Elli Howard and Catherine Abbruzzese
// 10/9/17
// Scanner for PL/0

// Libraries
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// Constants
#define PLO_FILE "pl0.txt"
#define TOKEN_FILE "lexeme.txt"
#define ALPHABET_LENGTH 78
#define SEPARATOR_LENGTH 7
#define NUM_LENGTH 10
#define ALPHA_LENGTH 52

// Structs
typedef struct Lexeme{
	int type;
	char meme[12];
	char typeName[20];
}Lexeme;

// Identifiers
/*
null: nulsym = 1, 
variables: identsym = 2, 
numbers: numbersym = 3, 
"+": plussym = 4, 
"-": minussym = 5, 
"*": multsym = 6,  
"/": slashsym = 7, 
"odd": oddsym = 8,  
"=": eqlsym = 9, 
"<>": neqsym = 10, 
"<": lessym = 11, 
"<=": leqsym = 12, 
">": gtrsym  =  13,  
">=": geqsym  =  14,  
"(": lparentsym  =  15,  
")": rparentsym  =  16, 
",": commasym = 17, 
";": semicolonsym = 18, 
".": periodsym = 19, 
":=": becomessym = 20, 
"begin": beginsym = 21, 
"end": endsym = 22, 
"if": ifsym = 23, 
"then": thensym = 24, 
"while": whilesym = 25, 
"do": dosym = 26, 
"call": callsym = 27, 
"const": constsym = 28, 
"var": varsym = 29, 
"procedure": procsym = 30, 
"write": writesym = 31, 
"read": readsym = 32, 
"else": elsesym = 33.
*/

// Methods
void scanDocument(){
	char currParse[100];
	Lexeme *memeList[10000];
	int cIdex = 0; // Tracks the position in currParse
	int currMeme = 0; // Tracks index in list
	int lineNumber = 1; // Tracks which line of the input file we are on for better error messages

	char alphabet[78] = {' ', '\n', '\t', ',', ';', '(', ')', '*', '/', '+', '-', '=', 
						 '<', '>', ':', '.', '1', '2', '3', '4', '5', '6', '7', '8', '9', 
						 '0', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 
						 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 
						 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 
						 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 
						 'Z'};
					 
	char* reservedWords[14] = {"const", "var", "procedure", "call", "begin", "end", 
							   "if", "then", "else", "while", "do", "read", "write", 
							   "odd"};
	typedef enum {nulsym = 1, identsym, numbersym, plussym, minussym,multsym,  slashsym, 
				  oddsym, eqsym, neqsym, lessym, leqsym,gtrsym, geqsym, lparentsym, 
				  rparentsym, commasym, semicolonsym,periodsym, becomessym, beginsym, 
				  endsym, ifsym, thensym, whilesym, dosym, callsym, constsym, varsym, 
				  procsym, writesym,readsym , elsesym} token_type;

	FILE *input, *output;
	int i, j, flag = 0, isNum = 1, isAlpha = 0;
	char sep;
	
	// Redo vars
	char overScan = '@';
	int overFlowFlag = 0;
	int reserved = -1;
	int commentEnd = 0;
	
	Lexeme *next;

	// Open necessary files
	input = fopen(PLO_FILE, "r");
	if(input==NULL){
		printf("Error opening file: %s\n",PLO_FILE);
		return;
	}
	output = fopen(TOKEN_FILE, "w");
	if(output==NULL){
		printf("Error opening file: %s\n",TOKEN_FILE);
		return;
	}
	
	// Scan in program character by character
	// NOTE: documents that do not end with a "." may cause strange errors
	while(overScan != '@' || fscanf(input, "%c", &currParse[0]) != EOF){
		// If we over scanned last time, reuse the over scanned character;
		if(overScan != '@'){
			currParse[0] = overScan;
			overScan = '@';
		}
		
		/// If we hit a return, increment line number to facilitate error messages
		if (currParse[0] == 13 || currParse[0] == '\n'){
			lineNumber++;
		}
		
		// Ignore whitespace
		if(currParse[0] == ' ' || currParse[0] == '\n' || currParse[0] == '\t' || currParse[0] == 13){
			continue;
		}
		
		// If a character is scanned that is not in the alphabet, output an error code (give line number for readability)
		for(i = 0; i < 78; i++){
			if (currParse[0] == alphabet[i]){
				flag = 1;
				break;
			}			
		}
		if(!flag){
			printf("Error: Non-alphabetic character \"%c\" on line %d \n", currParse[0], lineNumber);
			return;
    }
		
		// If the read character is alphabetic, we are starting either a reserved word or a variable
		if(isalpha(currParse[0])){
			cIdex = 1;
			overFlowFlag = 0;
			fscanf(input, "%c", &currParse[cIdex]);
			// Continue to scan in while still a variable or reserved word
			while(isalpha(currParse[cIdex]) || isdigit(currParse[cIdex])){
				cIdex++;
				if(cIdex > 11){ // Length has exceeded storeable length;
					overFlowFlag = 1;
					// keep scanning to end of variable, but do not store.
					fscanf(input, "%c", &currParse[cIdex]);
					while(isalpha(currParse[cIdex]) || isdigit(currParse[cIdex])){
						fscanf(input, "%c", &currParse[cIdex]);
					}
					break;
				}
				fscanf(input, "%c", &currParse[cIdex]);
			}
			
			// Unfortunately, this function always overscans
			overScan = currParse[cIdex];
			currParse[cIdex] = '\0';
			
			if(overFlowFlag){
				printf("Error: Variable identifier longer than 11 characters on line %d\n", lineNumber);
			}
			
			// Check against the reserved words
			reserved = -1;
			for(i=0; i<14;i++){
                if(strcmp(currParse, reservedWords[i])==0){
                    reserved = i;
                }
            }
            
            next = malloc(sizeof(Lexeme));
            if(next == NULL){
            	printf("Out Of Memory Error: Failed to malloc space for lexeme %d\n",currMeme);
            	return;
            }
            switch(reserved){
            	case -1: // variable name
            		next->type = 2;
            		strcpy(next->meme,currParse);
            		strcpy(next->typeName,"identsym");
            		break;
            	case 0: // const
            		next->type = 28;
            		strcpy(next->meme,currParse);
            		strcpy(next->typeName,"constsym");
            		break;
            	case 1: // var
            		next->type = 29;
            		strcpy(next->meme,currParse);
            		strcpy(next->typeName,"varsym");
            		break;
            	case 2: // procedure
            		next->type = 30;
            		strcpy(next->meme,currParse);
            		strcpy(next->typeName,"procsym");
            		break;
            	case 3: // call
            		next->type = 27;
            		strcpy(next->meme,currParse);
            		strcpy(next->typeName,"callsym");
            		break;
            	case 4: // begin
            		next->type = 21;
            		strcpy(next->meme,currParse);
            		strcpy(next->typeName,"beginsym");
            		break;
            	case 5: // end
            		next->type = 22;
            		strcpy(next->meme,currParse);
            		strcpy(next->typeName,"endsym");
            		break;
            	case 6: // if
            		next->type = 23;
            		strcpy(next->meme,currParse);
            		strcpy(next->typeName,"ifsym");
            		break;
            	case 7: // then
            		next->type = 24;
            		strcpy(next->meme,currParse);
            		strcpy(next->typeName,"thensym");
            		break;
            	case 8: // else
            		next->type = 33;
            		strcpy(next->meme,currParse);
            		strcpy(next->typeName,"elsesym");
            		break;
            	case 9: // while
            		next->type = 25;
            		strcpy(next->meme,currParse);
            		strcpy(next->typeName,"whilesym");
            		break;
            	case 10: // do
            		next->type = 26;
            		strcpy(next->meme,currParse);
            		strcpy(next->typeName,"dosym");
            		break;
            	case 11: // read
            		next->type = 32;
            		strcpy(next->meme,currParse);
            		strcpy(next->typeName,"readsym");
            		break;
            	case 12: // write
            		next->type = 31;
            		strcpy(next->meme,currParse);
            		strcpy(next->typeName,"writesym");
            		break;
            	case 13: // odd
            		next->type = 8;
            		strcpy(next->meme,currParse);
            		strcpy(next->typeName,"oddsym");
            		break;
            }
            memeList[currMeme] = next;
            currMeme++;
		}else if(isdigit(currParse[0])){ // Its probably a number
			cIdex = 1;
			overFlowFlag = 0;
			fscanf(input, "%c", &currParse[cIdex]);
			// Continue to scan in while still a variable or reserved word
			while(isdigit(currParse[cIdex])){
				cIdex++;
				if(cIdex > 5){ // Length has exceeded acceptable number length
					overFlowFlag = 1;
					// keep scanning to end of variable, but do not store.
					fscanf(input, "%c", &currParse[cIdex]);
					while(isdigit(currParse[cIdex])){
						fscanf(input, "%c", &currParse[cIdex]);
					}
					break;
				}
				fscanf(input, "%c", &currParse[cIdex]);
			}
			// This may also be a variable that starts with a number
			if(isalpha(currParse[cIdex])){
				printf("Error: Variable identifier does not begin with an alpha character on line %d\n", lineNumber);
				fscanf(input, "%c", &currParse[cIdex]);
				while(isalpha(currParse[cIdex]) || isdigit(currParse[cIdex])){
					fscanf(input, "%c", &currParse[cIdex]);
				}
				// Skip this lexeme if this occurs
				continue;
			}
			
			// Unfortunately, this function always overscans
			overScan = currParse[cIdex];
			currParse[cIdex] = '\0';
			
			if(overFlowFlag){
				printf("Error: Number longer than 5 characters on line %d\n", lineNumber);
			}
			
			next = malloc(sizeof(Lexeme));
			if(next == NULL){
            	printf("Out Of Memory Error: Failed to malloc space for lexeme %d\n",currMeme);
            	return;
            }
            next->type = 3;
            strcpy(next->meme, currParse);
            strcpy(next->typeName, "numbersym");
            memeList[currMeme] = next;
            currMeme++;
		}else{ // character is a special symbol
			switch(currParse[0]){
				case '+':
					next = malloc(sizeof(Lexeme));
					if(next == NULL){
            			printf("Out Of Memory Error: Failed to malloc space for lexeme %d\n",currMeme);
            			return;
            		}
            		next->type = 4;
            		strcpy(next->meme, "+");
            		strcpy(next->typeName, "plussym");
            		memeList[currMeme] = next;
            		currMeme++;
            		break;
				case '-':
					next = malloc(sizeof(Lexeme));
					if(next == NULL){
            			printf("Out Of Memory Error: Failed to malloc space for lexeme %d\n",currMeme);
            			return;
            		}
            		next->type = 5;
            		strcpy(next->meme, "-");
            		strcpy(next->typeName, "minussym");
            		memeList[currMeme] = next;
            		currMeme++;
            		break;
				case '*':
					next = malloc(sizeof(Lexeme));
					if(next == NULL){
            			printf("Out Of Memory Error: Failed to malloc space for lexeme %d\n",currMeme);
            			return;
            		}
            		next->type = 6;
            		strcpy(next->meme, "*");
            		strcpy(next->typeName, "multsym");
            		memeList[currMeme] = next;
            		currMeme++;
            		break;
				case '(':
					next = malloc(sizeof(Lexeme));
					if(next == NULL){
            			printf("Out Of Memory Error: Failed to malloc space for lexeme %d\n",currMeme);
            			return;
            		}
            		next->type = 15;
            		strcpy(next->meme, "(");
            		strcpy(next->typeName, "lparentsym");
            		memeList[currMeme] = next;
            		currMeme++;
            		break;
				case ')':
					next = malloc(sizeof(Lexeme));
					if(next == NULL){
            			printf("Out Of Memory Error: Failed to malloc space for lexeme %d\n",currMeme);
            			return;
            		}
            		next->type = 16;
            		strcpy(next->meme, ")");
            		strcpy(next->typeName, "rparentsym");
            		memeList[currMeme] = next;
            		currMeme++;
            		break;
				case '=':
					next = malloc(sizeof(Lexeme));
					if(next == NULL){
            			printf("Out Of Memory Error: Failed to malloc space for lexeme %d\n",currMeme);
            			return;
            		}
            		next->type = 9;
            		strcpy(next->meme, "=");
            		strcpy(next->typeName, "eqlsym");
            		memeList[currMeme] = next;
            		currMeme++;
            		break;
				case ',':
					next = malloc(sizeof(Lexeme));
					if(next == NULL){
            			printf("Out Of Memory Error: Failed to malloc space for lexeme %d\n",currMeme);
            			return;
            		}
            		next->type = 17;
            		strcpy(next->meme, ",");
            		strcpy(next->typeName, "commasym");
            		memeList[currMeme] = next;
            		currMeme++;
            		break;
				case '.':
					next = malloc(sizeof(Lexeme));
					if(next == NULL){
            			printf("Out Of Memory Error: Failed to malloc space for lexeme %d\n",currMeme);
            			return;
            		}
            		next->type = 19;
            		strcpy(next->meme, ".");
            		strcpy(next->typeName, "periodsym");
            		memeList[currMeme] = next;
            		currMeme++;
            		break;
				case ';':
					next = malloc(sizeof(Lexeme));
					if(next == NULL){
            			printf("Out Of Memory Error: Failed to malloc space for lexeme %d\n",currMeme);
            			return;
            		}
            		next->type = 18;
            		strcpy(next->meme, ";");
            		strcpy(next->typeName, "semicolonsym");
            		memeList[currMeme] = next;
            		currMeme++;
            		break;
            	case '/':
					fscanf(input, "%c", &currParse[0]);
					if(currParse[0] == '*'){
						commentEnd = 0;
						while(!commentEnd){
							fscanf(input, "%c", &currParse[0]);
							if(currParse[0] == '*'){
								fscanf(input, "%c", &currParse[0]);
								if(currParse[0] == '/'){
									commentEnd = 1;
								}
							}
						}
					}else{
						overScan = currParse[0];
						
						next = malloc(sizeof(Lexeme));
						if(next == NULL){
            				printf("Out Of Memory Error: Failed to malloc space for lexeme %d\n",currMeme);
            				return;
            			}
            			next->type = 7;
            			strcpy(next->meme, "/");
            			strcpy(next->typeName, "slashsym");
            			memeList[currMeme] = next;
            			currMeme++;
					}
					break;
				case '<':
					fscanf(input, "%c", &currParse[0]);
					if(currParse[0] == '>'){
						next = malloc(sizeof(Lexeme));
						if(next == NULL){
            				printf("Out Of Memory Error: Failed to malloc space for lexeme %d\n",currMeme);
            				return;
            			}
            			next->type = 10;
            			strcpy(next->meme, "<>");
            			strcpy(next->typeName, "neqsym");
            			memeList[currMeme] = next;
            			currMeme++;
					}else if(currParse[0] == '='){
						next = malloc(sizeof(Lexeme));
						if(next == NULL){
            				printf("Out Of Memory Error: Failed to malloc space for lexeme %d\n",currMeme);
            				return;
            			}
            			next->type = 12;
            			strcpy(next->meme, "<=");
            			strcpy(next->typeName, "leqsym");
            			memeList[currMeme] = next;
            			currMeme++;
					}else{
						overScan = currParse[0];
						
						next = malloc(sizeof(Lexeme));
						if(next == NULL){
            				printf("Out Of Memory Error: Failed to malloc space for lexeme %d\n",currMeme);
            				return;
            			}
            			next->type = 11;
            			strcpy(next->meme, "<");
            			strcpy(next->typeName, "lesssym");
            			memeList[currMeme] = next;
            			currMeme++;
					}
					break;
				case '>':
					fscanf(input, "%c", &currParse[0]);
					if(currParse[0] == '='){
						next = malloc(sizeof(Lexeme));
						if(next == NULL){
            				printf("Out Of Memory Error: Failed to malloc space for lexeme %d\n",currMeme);
            				return;
            			}
            			next->type = 14;
            			strcpy(next->meme, ">=");
            			strcpy(next->typeName, "geqsym");
            			memeList[currMeme] = next;
            			currMeme++;
					}else{
						overScan = currParse[0];
						
						next = malloc(sizeof(Lexeme));
						if(next == NULL){
            				printf("Out Of Memory Error: Failed to malloc space for lexeme %d\n",currMeme);
            				return;
            			}
            			next->type = 13;
            			strcpy(next->meme, ">");
            			strcpy(next->typeName, "gtrsym");
            			memeList[currMeme] = next;
            			currMeme++;
					}
					break;
				case ':':
					fscanf(input, "%c", &currParse[0]);
					if(currParse[0] == '='){
						next = malloc(sizeof(Lexeme));
						if(next == NULL){
            				printf("Out Of Memory Error: Failed to malloc space for lexeme %d\n",currMeme);
            				return;
            			}
            			next->type = 20;
            			strcpy(next->meme, ":=");
            			strcpy(next->typeName, "becomesym");
            			memeList[currMeme] = next;
            			currMeme++;
					}else{
						overScan = currParse[0];
						
						printf("Invalid Symbol Error: \":\" is not a valid operator\n");
					}
					break;
			}
		}
	}
	// Print out the lexemes in a string
	for(i = 0; i < currMeme;i++){
		if(memeList[i]->type == 2 || memeList[i]->type == 3){
			fprintf(output, "%d %s|",memeList[i]->type,memeList[i]->meme);
		}else{
			fprintf(output, "%d|",memeList[i]->type);
		}
	}
	fclose(input);
	fclose(output);
}
