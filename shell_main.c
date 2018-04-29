/* Name : taegun_kim ID : 2016112129 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define Max_line 80

typedef enum {false, true} bool;

int main(void)
{
FILE *history_reset;   
FILE *his_write;FILE *his_read;
char command[Max_line/2 + 1];
char *args[Max_line/2 +1];
char *cmd_parse; 


int should_run = 1;
int cmd_indexnum;
int i; bool redirection_check = false;
bool history_check = false;
bool exclamation_check = false;
bool pipecheck = false;

pid_t pid;
history_reset = fopen("history.txt", "w");
fclose(history_reset);


	while (should_run) {
		his_write = fopen("history.txt", "a");   //write history
		redirection_check = false; exclamation_check = false;
		history_check = false; pipecheck = false;
		
		cmd_indexnum = 0; //
		printf("osh>");
		fflush(stdout);
		fgets(command, Max_line/2, stdin);    //input command
Label1:
		if(strcspn(command, "!") != 0){    //! command doesn't save the history 
		fprintf(his_write, command);
		}
		fclose(his_write);
	 	cmd_parse = strtok(command, " ");   // command parsing
	
		do{
			args[cmd_indexnum] = cmd_parse;
			cmd_parse = strtok(NULL, " ");
			cmd_indexnum++;
			if(cmd_indexnum > 41){
				printf("too many operation\n");
				break;
			} 
		}while(cmd_parse != NULL);
		args[cmd_indexnum] = NULL;  //for excvp  . excvp need Null parameter last
		
		for(i = 0; i < cmd_indexnum; i++){
			if(!strcmp(args[i], "|")) pipecheck = true;   // only 1 
			if(!strcmp(args[0], "history\n")) history_check = true;  
			if(!strcmp(args[0], "history")) history_check = true;
			if(args[0][0] == '!') {exclamation_check = true; history_check = true;}  //exclamation is history fucntion
			if(args[i][strlen(args[i])-1] == '\n')
			args[i][strlen(args[i])-1] = '\0';    //args[0] dosen't need \n, it can't recognize execvp
			if(!strcmp(args[i], "<") || !strcmp(args[i], ">") ) redirection_check = true;	
		}

		if(pipecheck == true){
			int i; int check_num; char *args2[Max_line/2 +1]; pid_t pid2; pid_t pid3;
			for(i = 0; i < cmd_indexnum; i++){
				if(!strcmp(args[i], "|")){check_num = i;break;} 
			}
			int fd[2];  //for make pipe
			
			pid2 = fork();   //parent is background process , so we need 2 child process
				if(pid2 == -1) {printf("fork error\n"); }
				else if(pid2 == 0){
					if(pipe(fd)==-1){printf("pipe error\n"); continue;}
					pid3 = fork();
						
					if(pid3 == -1){printf("fork2 error\n"); break;}
					else if(pid3 == 0){   // 2 child process, they connect pipe
						close(fd[0]); dup2(fd[1], STDOUT_FILENO); //output move other process throught pipe
						 for(i = 0; i < check_num; i++){
							args2[i] = args[i];
						 }args2[i] = NULL; 
						 execvp(args2[0], args2); printf("execvp1 error\n");  //this is input value for other process
					}
					else{   wait();
						 close(fd[1]); dup2(fd[0], STDIN_FILENO);  // input value come to other process throught pipe
						  for(int i = 0; args[check_num + i+1] != NULL; i++){ 
							args2[i] = args[check_num + i+1];
						  }args2[i] = NULL;
					          execvp(args2[0], args2); printf("execvp2 error\n");  
					}
				}
				else{
				wait();  
				}
		continue; // go to start
		}
		if(history_check == true){
			int num = 1;
			his_read = fopen("history.txt", "r");
			char r_cmd[Max_line/2];
			if(exclamation_check ==true){
				if(atoi(args[0]+1) != 0){
					while(!feof(his_read)){
						if(fgets(r_cmd, Max_line/2, his_read)){
							if(atoi(args[0]+1) == num){  // ex !23 -> args[0]+1 = 23
								strcpy(command , r_cmd); //match number and history command
								fclose(his_read);
							 his_write = fopen("history.txt", "a");
              						  redirection_check = false; exclamation_check = false;
             						   history_check = false;cmd_indexnum = 0;
							goto Label1;  //go to Label1 with command inforamtion

							}
							num++;
						}
					}
					printf("The %d instruction does not exist.\n", atoi(args[0]+1) );		
					fclose(his_read);	continue;
				}
				else if((args[0]+1)[0] == '!'){} //if !! the history action operate
				else{
					printf("syntax error\n"); continue; //ex !df22 it can't
				}
			}
			if(redirection_check == false){
			while(!feof(his_read)){
				if(fgets(r_cmd, Max_line/2, his_read)){
					printf("%d, %s", num, r_cmd);
					num++;
				}
			}
			fclose(his_read);
			continue;
			}
			else{
			int i; int ck_num;
				for(i=0; i < cmd_indexnum; i++){
               	        	         if(!strcmp(args[i], "<")){ //not supported redirection in history
                                       		printf("history error"); 
                               		 }
                              		  else if(!strcmp(args[i], ">")){ // redirection out history
                                      		   ck_num = i;
                              		  }
                       		 }
				if(args[ck_num+1]!=NULL){
					his_write = fopen(args[ck_num+1], "w");
					while(!feof(his_read)){
						if(fgets(r_cmd, Max_line/2, his_read)){ //redirect out
							fprintf(his_write, "%d, %s", num, r_cmd);
							num++;
						}
					}
					fclose(his_read);fclose(his_write);continue;
				}
			}
		}	
		if(redirection_check==true){  // need 1 child process
			int i; int txt; int ck_num; bool re_in = false; bool re_out = false; 
			for(i=0; i < cmd_indexnum; i++){	
				if(!strcmp(args[i], "<")){
					re_in = true; ck_num = i;
				}
				else if(!strcmp(args[i], ">")){
					re_out = true; ck_num = i;
				}

			}
			if(re_in == true){
				if(args[ck_num+1] == NULL){
					printf("cannot find file\n");
				}
				else{
					if((txt=open(args[ck_num+1], O_RDONLY)) == -1){
						printf("read failed\n");
					}
				for(i = ck_num; i < cmd_indexnum; i++){   //relocate command
                                        args[i] = args[i+2];
                                }
                                args[i] = NULL;
                                pid = fork();
                                if(pid < 0){
                                  printf("fork error\n");
                                  exit(0);
                                  }

                                else if (pid==0){
                                         if(dup2(txt, STDIN_FILENO)&&close(txt)){
                                         }
                                         if(execvp(args[0], args) == -1){
                                         printf("%s error\n", args[0]);
                                         exit(0);
                                         }
                                 }
                                 else{
                                         wait();
                                         continue;
                                 }

			}
		}
		else if(re_out == true){
				if(args[ck_num+1] == NULL){
					printf("cannot find file2\n");
				}
				else{
					if((txt=open(args[ck_num+1], O_RDWR | O_CREAT | S_IROTH,0644)) == -1){
						printf("read failed\n");
					}
				}
				for(i = ck_num; i < cmd_indexnum; i++){
                                        args[i] = args[i+2];
                                }
				args[i] = NULL;
				pid = fork();
				if(pid < 0){
                      		  printf("fork error\n");
                       		  exit(0);
              			  }

             		        else if (pid==0){
					 if(dup2(txt, STDOUT_FILENO)&&close(txt)){
					 }
                       			 if(execvp(args[0], args) == -1){
                       			 printf("%s error\n", args[0]);
                    			 exit(0);
                       			 }
               			 }
               			 else{
               				 wait();
					 continue;
               			 }
	
			
		}
	}
		//standard operation 
		pid = fork();
		if(pid < 0){
			printf("fork error\n");
			exit(0);
		}
		else if (pid==0){
			if(execvp(args[0], args) == -1){
			printf("%s error\n", args[0]);
			exit(0);
			}	
		}
		else{
		wait();
		}
	}
	return 0;
	
} 
