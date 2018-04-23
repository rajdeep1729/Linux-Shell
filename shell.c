#include <stdio.h>
#include <string.h> //strtok(),strcasecmp()
#include <stdlib.h>	//malloc(),exit()
#include <sys/types.h>	
#include <sys/wait.h>
#include <fcntl.h>	
#include <unistd.h>	
#include <error.h>	//perror()
#include <readline/readline.h>
#include <readline/history.h>

#define INDIV_COMMAND_LENGTH 20
#define MAX_COMMAND_SIZE 200
#define MAX_COMMANDS 20
#define TOK_DELIM ";&|><"

char *pwd;//present working directory
char delimiter[MAX_COMMANDS];

void my_prompt();//used for checking commands and printing prompt
char **parse_command(char *command);//function for parsing commands and getting switches
char **parse_command1(char *command);//function for parsing and initializing delimiter array
int execute(char **parsed_command);//function for executing individual commands
void execPipedCmds(char **cmd,int index);//function for single pipe
void exec_in(char **cmd,int index);//function for input redirection
void exec_out(char **cmd,int index);//function for output redirection
void execute_command2(char **cmd,int index,int chk);//multiple pipes and I/O redirection 
void batch_exec(char *filename);//used for batch file execution

int main(int argc,char *argv[])

{
   if(argc==1)
    my_prompt();
   else if(argc==2)
   	batch_exec(argv[1]);
   else
   {
   	fprintf(stderr,"\nWrong number of arguments\n");
   	exit(1);
   }
   return 0;
}

void batch_exec(char *filename)//used for batch file execution
{
  char *cmd=malloc(MAX_COMMAND_SIZE*sizeof(char));
  
  int i=0;
  int count=0;
  FILE *fp;
  fp=fopen(filename,"r");

  char **parsed_command;
  
  while(fscanf(fp,"%[^\n]",cmd)!=EOF)
  {
 	 //cmd=get_command(1);
  	 //fscanf(fp,"%[^\n]]s",cmd);
  	 fgetc(fp);
  	 bzero(delimiter,sizeof(delimiter));
  	 
  	 char **command=parse_command1(cmd);
  	
     while(command[i]!=NULL)
    	
     {
    	 	
           if(delimiter[count]=='p' &&(delimiter[count+1]=='p'||delimiter[count+1]=='>'||delimiter[count+1]=='<'))
            {
              
             if(delimiter[count+1]=='p')
              execute_command2(command,i,0);
             else if(delimiter[count+1]=='>')
              execute_command2(command,i,1);
             i+=3; 
             count+=2;    
             //continue;
            }

           else if(delimiter[count]=='p')

            {
              execPipedCmds(command,i);
              i+=2;
              count++;
            
            }

            else if(delimiter[count]=='>')
            {
              if(delimiter[count+1]=='p')
              {
               	execute_command2(command,i,2);
               	i+=3;
               	count+=2;
               	
              }
              
              else
              {
              	exec_out(command,i);
              	i+=2;
              	count++;
              	
              }
            }
            
            else if(delimiter[count]=='<')
            {
              
             if(delimiter[count+1]=='>')
             {
               	execute_command2(command,i,3);
               	i+=3;
               	count+=2;
             }
              
             else
              {
              	exec_in(command,i);
                i+=2;
                count++;
                
              }
            }
            
            else//not any pipe or I/O redirection
            {
    	 	 
    	    		
             if(delimiter[count]=='&')
              {

              if (delimiter[count+1] == 'p' ||delimiter[count+1] == '>' ||delimiter[count+1] == '<')
              {
               count++;
              }
              else
              {	
                parsed_command=parse_command(command[i]);
    	 	    int status=execute(parsed_command);
                if(status==0)
                {
              	  i++;
              	  count++;
                }
               	else
              	 break;
              }	

            } 

            else if(delimiter[count]=='|')
            {
             
              if (delimiter[count+1] == 'p' ||delimiter[count+1] == '>' ||delimiter[count+1] == '<')
              {
                count++;
                
              }
              else
              {
                 parsed_command=parse_command(command[i]);
    	 	     int status=execute(parsed_command);
                 if(status==0)
              	  break;
               	 else
              	 {
              		i++;
                    count++;
              		
              	 }
               }	
             } 	 

            else if(delimiter[count]==';')
            {
              	if (delimiter[count+1] == 'p' ||delimiter[count+1] == '>' ||delimiter[count+1] == '<')
                { 
                  count++;
                }
                else
                {
              	  parsed_command=parse_command(command[i]);
    	 	      int status=execute(parsed_command);
              	  count++;
              	  i++;
                }
            }
            else
            {  
              	parsed_command=parse_command(command[i]);
    	 	    int status=execute(parsed_command);
                i++;     
            }
    	 }	
      }
        count=0;
        i=0;
  }
 //free(parsed_command);
 free(cmd);
 fclose(fp);

}

void my_prompt()
{

 char **parsed_command;
 char *cmd;
 int i;
 char **command;
 char shell_prompt[200];
 int count=0;
 char *username=getenv("USER"); 
 while(1) 
  {
    	pwd=getcwd(NULL,0);
    	snprintf(shell_prompt, sizeof(shell_prompt),"%s's shell@:%s> ",username,pwd);
    	cmd=readline(shell_prompt);
    	add_history(cmd);
        command=parse_command1(cmd);
     	i=0;
     	//fprintf(stderr,"Debug=%s %s %s %s",command[0],command[1],command[2],command[3]);
     	 while(command[i]!=NULL)
    	 {
    	 	
    	 	if(delimiter[count]=='p' &&(delimiter[count+1]=='p'||delimiter[count+1]=='>'||delimiter[count+1]=='<'))
            {
              
             if(delimiter[count+1]=='p')
              execute_command2(command,i,0);
             else if(delimiter[count+1]=='>')
              execute_command2(command,i,1);
             i+=3; 
             count+=2;    
             
            }

           else if(delimiter[count]=='p')

            {
              execPipedCmds(command,i);
              i+=2;
              count++;
             
            }

            else if(delimiter[count]=='>')
            {
              	exec_out(command,i);
              	i+=2;
              	count++;
              
            }
            
            else if(delimiter[count]=='<')
            {
              
             if(delimiter[count+1]=='>')
             {
               	execute_command2(command,i,2);
               	i+=3;
               	count+=2;
             }
              
             else
              {
              	exec_in(command,i);
                i+=2;
                count++;
              }
            }
            
            else//not any pipe or I/O redirection
            {
    	 	 
    	    		
             if(delimiter[count]=='&')
              {

              if (delimiter[count+1] == 'p' ||delimiter[count+1] == '>' ||delimiter[count+1] == '<')
              {
               count++;
              }
              else
              {	
                parsed_command=parse_command(command[i]);
    	 	    int status=execute(parsed_command);
                if(status==0)
                {
              	  i++;
              	  count++;
                }
               	else
              	 break;
              }	

            } 

            else if(delimiter[count]=='|')
            {
             
              if (delimiter[count+1] == 'p' ||delimiter[count+1] == '>' ||delimiter[count+1] == '<')
              {
                count++;
              }
              else
              {
                 parsed_command=parse_command(command[i]);
    	 	     int status=execute(parsed_command);
                 if(status==0)
              	  break;
               	 else
              	 {
              		i++;
                    count++;
              	 }
               }	
             } 	 

            else if(delimiter[count]==';')
            {
              	if (delimiter[count+1] == 'p' ||delimiter[count+1] == '>' ||delimiter[count+1] == '<')
                { 
                  count++;
                }
                else
                {
              	  parsed_command=parse_command(command[i]);
    	 	      int status=execute(parsed_command);
              	  count++;
              	  i++;
                }
            }
            else
            { 
               parsed_command=parse_command(command[i]);
    	 	   int status=execute(parsed_command);
               i++;     
            }
    	  }	
        }
        
        bzero(delimiter,sizeof(delimiter));
        free(parsed_command);
        free(cmd);
        count=0;
        i=0;
  }


}

void execute_command2(char **cmd,int index,int chk)//multiple pipes and I/O redirection 
{
 
 char **parsedcommand1=parse_command(cmd[index]);
 char **parsedcommand2=parse_command(cmd[index+1]);
 char **parsedcommand3=parse_command(cmd[index+2]);
 int pipefd[2],pipefd1[2];
 pid_t p1,p2,p3;

 if(pipe(pipefd)==-1)
 {
	perror("Error in pipe(): ");
	return;
 }
 
 if(pipe(pipefd1)==-1)
 {
	perror("Error in pipe(): ");
	return;
 }

 if(chk==0)//double pipe
 {
 	p1=fork();
	if(p1==-1)
	{
		perror("Error in fork(): ");
		return;		
	}

	if(p1==0)
	{
		//child process
		close(pipefd[0]);
		dup2(pipefd[1],STDOUT_FILENO);
		close(pipefd[1]);
		close(pipefd1[0]);
		close(pipefd1[1]);

		if(execvp(parsedcommand1[0], parsedcommand1)<0)
			perror("Error in execvp():");

	}
	else
	{
		//parent process

		p2=fork();

		if(p2==-1)
		{
			perror("Error in fork():");
			return;
		}
        //second child process 
		if(p2==0)
		{
			close(pipefd[1]);
			dup2(pipefd[0], STDIN_FILENO);
			close(pipefd[0]);
			close(pipefd1[0]);
            dup2(pipefd1[1],STDOUT_FILENO);
			close(pipefd1[1]);
			
			if(execvp(parsedcommand2[0], parsedcommand2)<0)
				perror("Error in execvp");
		}
		else
		{	
         
         p3=fork();

         if(p3==-1)
		 {
			perror("Error in fork():");
			return;
		 }
         
		 if(p3==0)//third child process
		 {
			close(pipefd1[1]);
			dup2(pipefd1[0], STDIN_FILENO);
			close(pipefd1[0]);
			close(pipefd[0]);
			close(pipefd[1]);
			
			if(execvp(parsedcommand3[0], parsedcommand3)<0)
				perror("Error in execvp");
		 }

		 else
		 {
			
            close(pipefd[0]);
			close(pipefd[1]);
			close(pipefd1[0]);
			close(pipefd1[1]);
            for(int i=0;i<3;i++)
            	wait(NULL);
			
		 }

	    }
 
    }
 }
 else if(chk==1)//pipe then output redirection
 {
    
    int fd=open(parsedcommand3[0],O_CREAT|O_WRONLY,S_IRWXU);
	p1=fork();
	if(p1==-1)
	{
		perror("Error in fork(): ");
		return;		
	}

	if(p1==0)
	{
		//child process
		close(pipefd[0]);
		dup2(pipefd[1],STDOUT_FILENO);
		close(pipefd[1]);

		if(execvp(parsedcommand1[0], parsedcommand1)<0)
			perror("Error in execvp():");

	}
	else
	{
		//parent process
		p2=fork();

		if(p2==-1)
		{
			perror("Error in fork():");
			return;
		}
        //second child process 
		if(p2==0)
		{
			close(pipefd[1]);
			dup2(pipefd[0], STDIN_FILENO);
			dup2(fd,STDOUT_FILENO);
			close(pipefd[0]);
			if(execvp(parsedcommand2[0], parsedcommand2)<0)
				perror("Error in execvp");
		}
		else
		{
			close(pipefd[0]);
			close(pipefd[1]);
			wait(NULL);
			wait(NULL);
		}
	}
	
	close(fd);

 }
 
 else if(chk==2)//input and output redirection
 {
    
    
    int fd=open(parsedcommand2[0],O_RDONLY,S_IRWXU);
    if(fd==-1)
    {
    	perror("fd:");
    }
    
    int fd1=open(parsedcommand3[0],O_CREAT|O_WRONLY,S_IRWXU);
    if(fd1==-1)
    {
    	perror("fd1:");
    } 	
	
	p1=fork();
	if(p1==-1)
	{
		perror("Error in fork(): ");
		return;		
	}

	if(p1==0)
	{
		//child process
		dup2(fd,STDIN_FILENO);
		dup2(fd1,STDOUT_FILENO);
		if(execvp(parsedcommand1[0], parsedcommand1)<0)
			perror("Error in execvp():");

	}
	else
	{
		//parent proces
		wait(NULL);
	}

	close(fd);
	close(fd1);

  }
 
}

void exec_in(char **cmd,int index)//function for input redirection
{

    char **parsedcommand1=parse_command(cmd[index]);
	char **parsedcommand2=parse_command(cmd[index+1]);
	pid_t p1;
	int fd=open(parsedcommand2[0],O_CREAT|O_RDONLY,S_IRWXU);
    //fprintf(stderr, "%s\n",parsedcommand2[0] );
    if(fd==-1)
    {
      perror("fd:");
      return;
    }
      
	p1=fork();
	if(p1==-1)
	{
		perror("Error in fork(): ");
		return;		
	}

	if(p1==0)
	{
		//child
		int i=dup2(fd,STDIN_FILENO);
		//fprintf(stderr,"1st=%d",i);
		if(execvp(parsedcommand1[0], parsedcommand1)<0)
			perror("Error in execvp():");


	}


	else	//parent
	{
		wait(NULL);
	}

   close(fd);
}

void exec_out(char **cmd,int index)//function for output redirection
{

    char  **parsedcommand1=parse_command(cmd[index]);
	char **parsedcommand2=parse_command(cmd[index+1]);
	pid_t p1;
	int fd=open(parsedcommand2[0],O_CREAT|O_WRONLY,S_IRWXU);
    //fprintf(stderr, "%s\n",parsedcommand2[0] );
	if(fd==-1)
    {
      perror("fd:");
      return;
    }

	p1=fork();
	if(p1==-1)
	{
		perror("Error in fork(): ");
		return;		
	}

	if(p1==0)
	{
		//child
		int i=dup2(fd,STDOUT_FILENO);
		//fprintf(stderr,"1st=%d",i);
		if(execvp(parsedcommand1[0], parsedcommand1)<0)
			perror("Error in execvp():");

    }


	else	//parent
	{
		wait(NULL);
	}


   close(fd);
}

void execPipedCmds(char **cmd,int index)
{
  	char** parsedcommand1=parse_command(cmd[index]);
	char** parsedcommand2=parse_command(cmd[index+1]);
	
	int pipefd[2];
	pid_t p1,p2;

	if(pipe(pipefd)==-1)
	{
		perror("Error in pipe(): ");
	    return;
	}

	p1=fork();
	if(p1==-1)
	{
		perror("Error in fork(): ");
		return;		
	}

	if(p1==0)
	{
		//child process
		close(pipefd[0]);
		dup2(pipefd[1],STDOUT_FILENO);
		close(pipefd[1]);

		if(execvp(parsedcommand1[0], parsedcommand1)<0)
			perror("Error in execvp():");

	}
	else
	{
		//parent process
		p2=fork();

		if(p2==-1)
		{
			perror("Error in fork():");
			return;
		}
        //second child process 
		if(p2==0)
		{
			close(pipefd[1]);
			dup2(pipefd[0], STDIN_FILENO);
			close(pipefd[0]);
			if(execvp(parsedcommand2[0], parsedcommand2)<0)
				perror("Error in execvp");
		}
		else
		{
			close(pipefd[0]);
			close(pipefd[1]);
			wait(NULL);
			wait(NULL);
			
		}
	}

  return;
	
}

char **parse_command1(char *command)
{

  char **tokens = malloc(INDIV_COMMAND_LENGTH * sizeof(char *));
  int count=0;

  for(int i=0;i<strlen(command);i++)
    {
       if(command[i]=='|'&& command[i+1]!='|'&& command[i+1]!=' ')
          delimiter[count++]='p'; 

       else if((command[i]=='&'&&command[i+1]=='&')|| (command[i]=='|' && command[i+1]=='|'))
          {
          	delimiter[count++]=command[i];
          	i+=1;
          }
        else if(command[i]== ';'||command[i]=='>'||command[i]=='<')
        {
        	delimiter[count++]=command[i];
        }  	
       
    }

   /*for(int i=0;i<10;i++)
    printf("delim=%c\n",delimiter[i]); */

  char *token;
  int position = 0;

  token = strtok(command,TOK_DELIM);

  while (token != NULL) 
  {
      tokens[position] = token;
      position++;

      token = strtok(NULL, TOK_DELIM);
   }

  tokens[position] = NULL;
    
  return tokens;

}

char ** parse_command(char *command)
{

  char **tokens = malloc(INDIV_COMMAND_LENGTH * sizeof(char *));
  char *token;
  int position = 0;

  token = strtok(command," ");

  while (token != NULL) 
  {
      tokens[position] = token;
      position++;

      token = strtok(NULL, " ");
  }

  tokens[position] = NULL;
    
  return tokens;

}

int execute(char **parsed_command)
{
  int status;

  if (strcmp(parsed_command[0], "exit") == 0)
  {
    exit(0);
  }

  else if (strcmp(parsed_command[0], "clear") == 0)
  {
  	printf("\e[1;1H\e[2J");
  	return 0;
  }

  else if(strcmp(parsed_command[0], "pwd") == 0)	
  {
   
    pwd=getcwd(NULL,0);

    if(pwd==NULL)
     {
     	perror("pwd:");
        return 1; 
     }
    else
     printf("%s\n",pwd);

    free(pwd);
    return 0;   
  }

  else if(strcmp(parsed_command[0], "cd") == 0)
  {

    if (parsed_command[1] == NULL) 
    {
    	char x[100]="/home/";
    	char *y=getenv("USER");
    	strcat(x,y);
    	chdir(x);
    	return 0;
    }

    else if(strcmp(parsed_command[1],"..")==0)
    {

    	for(int i=strlen(pwd)-1;i>=0;i--)
    	{
    		if(pwd[i]=='/')
    			{
    				pwd[i]='\0';
    				break;
    			}
    	}

    	if (chdir(parsed_command[1]) != 0) 
    	{
      		perror("mysh");
      		return 1;
        }		

    }


   else 
    {

    	if (chdir(parsed_command[1]) != 0)
    	{
      		perror("mysh");
      		return 1;
        }
    }

  } 

   else if(strcmp(parsed_command[0], "echo") == 0)
   {
    
    int i=1;
    
    while(parsed_command[i]!=NULL)
    {
    	printf("%s ",parsed_command[i++]);
    }
    
    printf("\n");
    return 0;

   }

   else if(strcmp(parsed_command[0], "type") == 0)
   {

     if(parsed_command[1]==0)
     {
     	fprintf(stderr, "mysh:Expected command name after type\n");
        return 1;
     }
     
     if(!strcmp(parsed_command[1],"echo")||!strcmp(parsed_command[1],"pwd")||!strcmp(parsed_command[1],"cd")||!strcasecmp(parsed_command[1],"type")||!strcasecmp(parsed_command[1],"clear")||!strcasecmp(parsed_command[1],"exit"))
      printf("Internal Command\n");
     else
      printf("External Command\n");    
     return 0;

   }   

   else
  {

    pid_t p=fork();

    if(p==0)//child
    {
      int k=execvp(parsed_command[0],parsed_command);
      if(k==-1)
      {
      	perror("mysh");
      	return 1;
      }
      exit(0);
    }

    else
      wait(&status);

    return 0;
  }

}