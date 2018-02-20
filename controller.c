// Elli Howard and Catherine Abbruzzese
// 10/30/17
// Controller

#include <stdio.h>
#include <stdlib.h>
#include "parser.c"
//#include "scanner.c"
//#include "virtualmachine.c"

int main(int argc, char **argv){
	FILE *memes, *machine, *outfile;
	int lFlag = 0, aFlag = 0, vFlag = 0;
	int i;
	for(i = 0; i < argc; i++){
		if(strcmp(argv[i],"-l") == 0){
			lFlag = 1;
		}
		if(strcmp(argv[i],"-a") == 0){
			aFlag = 1;
		}
		if(strcmp(argv[i],"-v") == 0){
			vFlag = 1;
		}
	}
	char character;
	
	scanDocument();
	
	if(lFlag){
		printf("Lexemes:\n");
		memes = fopen(LEXEME_FILE, "r");
		if(memes==NULL){
			printf("Error opening file: %s\n",LEXEME_FILE);
			return 0;
		}
	
	
		while(fscanf(memes, "%c", &character) != EOF){
			printf("%c",character);
		}
		printf("\n\n");
	}
	
	int error = parseAndGenerate();
	if(error){
		return 0;
	}
	
	if(aFlag){
		printf("Assembly Code:\n");
		machine = fopen(MACHINE_FILE, "r");
		if(machine==NULL){
			printf("Error opening file: %s\n",MACHINE_FILE);
			return 0;
		}
	
		while(fscanf(machine, "%c", &character) != EOF){
			printf("%c",character);
		}
		printf("\n\n");
	}
	
	simulateProgram();
	
	if(vFlag){
		printf("Output:\n");
		outfile = fopen(OUTPUT_FILE, "r");
		if(outfile==NULL){
			printf("Error opening file: %s\n",OUTPUT_FILE);
			return 0;
		}
	
		while(fscanf(outfile, "%c", &character) != EOF){
			printf("%c",character);
		}
		printf("\n");
	}
}