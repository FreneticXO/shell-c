#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64


int bg_flag[65];

/* Splits the string by space and returns the array of tokens
*
*/
char **tokenize(char *line)
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;

  for(i =0; i < strlen(line); i++){

    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
      token[tokenIndex] = '\0';
      if (tokenIndex != 0){
	tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
	strcpy(tokens[tokenNo++], token);
	tokenIndex = 0; 
      }
    } else {
      token[tokenIndex++] = readChar;
    }
  }
 
  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
}


int background(char* line){
	char *commands = strtok(line, "&");
	char **tok = tokenize(commands);

	int k = fork();

	if(k<0){
		printf("ERROR IN FORKING");
	}else if(k == 0){
		setpgid(0, 0);
		int p = execvp(tok[0], tok);
		if(p < 0){
			printf("Shell: Incorrect Command\n");
			return 0;
		}
	}else if(k>0){
		for(int i = 0; i < 64; i++){
				if (bg_flag[i] == 0){
					bg_flag[i] = k;
					break;
				}
			}
	}
}

void series(char* line){

	char *token = strtok(line, "&&");

	while(token != NULL){

		char **tk = tokenize(token);

		int f = fork();
		if(f<0) printf("error in fork\n");


		else if(f == 0){
			int p = execvp(tk[0], tk);
			if(p < 0){
				printf("Shell: Incorrect Command\n");
			}
		}

		
		else if(f > 0){
			int w = waitpid(f, NULL, 0);
		}
		
		token = strtok(NULL, "&&");
	}
}


void parallel(char* line){

	char *token = strtok(line, "&&&");

	int fo[65];
	int cmdcount = 0;
	int i = 0;
	while(token != NULL){

		char **tk = tokenize(token);


		fo[i] = fork();
		if(fo[i]<0) printf("error in fork\n");


		else if(fo[i] == 0){
			int p = execvp(tk[0], tk);
			if(p < 0){
				printf("Shell: Incorrect Command\n");
			}
		}

		token = strtok(NULL, "&&&");
		cmdcount++;
		i++;
	}

	for(int j=0; j<cmdcount; j++){
		if(fo[j]>0){
			//printf("%d", j);
			waitpid(fo[j], NULL, 0);
		}
	}

}

void sig(){
	printf("\n");
}




int main(int argc, char* argv[]) {
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              
	int i;
	for(i=0; i<65; i++) bg_flag[i]=0;

	signal(SIGINT,sig);

	while(1) {		

		for(i=0;i<64;i++){
			if (bg_flag[i]>0){
				int w = waitpid(bg_flag[i], NULL, WNOHANG);
				if(w==bg_flag[i]){
					bg_flag[i] = 0;
					printf("Shell: Background process finished\n");
				}
			}
		}	

		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
		printf("$ ");
		scanf("%[^\n]", line);
		getchar();

		// printf("Command entered: %s (remove this debug output later)\n", line);
		/* END: TAKING INPUT */

		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);

		int extype = 0;

		for(i=0; tokens[i]!=NULL; i++){
			if(!strcmp("&", tokens[i])){
				extype = 1;		//background process
			}
			if(!strcmp("&&", tokens[i])){
				extype = 2;		// series foreground processes
			}
			if(!strcmp("&&&", tokens[i])){
				extype = 3;		// prallel foreground processes
			}
			
		}
		//printf("%d\n", extype);

		if(tokens[0] == NULL){
			free(tokens[0]);
			free(tokens);
			continue;
		}else if(!strcmp(tokens[0], "exit")){

			for(i=0;i<64;i++){

				if(bg_flag[i]>0){

					kill(bg_flag[i],SIGKILL);
					bg_flag[i] = 0;
				}
			}

			for(i=0; tokens[i]!=NULL; i++){
				free(tokens[i]);
			}
			free(tokens);

			exit(0);

		

		}else if(!strcmp(tokens[0], "cd")){
			if(tokens[2]!=NULL){
				printf("Shell: Incorrect command\n");
			}else{
				int ch = chdir(tokens[1]);
				if(ch < 0){
					printf("Shell: Incorrect command\n");
				}
			}


		}else if(extype == 1){
			background(line);
		}else if(extype == 2){
			series(line);
		}else if(extype == 3){
			parallel(line);
		}
		else{

			int com = fork();
				if(com < 0) {
					printf("Shell: Error in fork\n");
				}
				else if(com==0){
					int p = execvp(tokens[0],tokens);
					if(p==-1){
						printf("Shell: Incorrect command\n");
						exit(0);
					}
				}else if(com>0){
					int w = waitpid(com,NULL,0);
				}

		}




       
		// Freeing the allocated memory	
		for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);

	}
	return 0;
}
