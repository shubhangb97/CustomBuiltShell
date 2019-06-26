#include  <stdio.h>
#include  <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<signal.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

/* Splits the string by space and returns the array of tokens
*
*/
void doStuff(char **,int isBack);
void printfull(char **);
int getComm(char **);
struct sigaction s,s2;
int *myPid;
int back;
int numB=0;
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
  return tokens ;
}

void signal_handle(int signum, siginfo_t *info, void *ptr)
{
  if(getpid()== *(myPid))
  kill(0,SIGTERM);
//  printf("ctrl");
  while(numB>0) //checkfor background processes
  {
    int p=waitpid(-1,NULL,WNOHANG);
    if(p==0)
    {
      break;
    }
    else
    {
    printf("Shell:Background process finished");
    numB=numB-1;
    }
  }
}
void sigt_handle(int signum, siginfo_t *info, void *ptr)
{
  if(getpid()== *(myPid))
  {
    return;
  }
  exit(0);
}
int main(int argc, char* argv[]) {
	char  line[MAX_INPUT_SIZE];
	char  **tokens;
	int i;
  myPid=(int *)malloc(sizeof(int));
  *(myPid)=getpid();
  setpgid(0,getpid());
  char *str;
  s.sa_sigaction=signal_handle;
  s.sa_flags= SA_SIGINFO;
  sigaction(SIGINT,&s,NULL);
  s2.sa_sigaction=sigt_handle;
  s2.sa_flags= SA_SIGINFO;
  sigaction(SIGTERM,&s2,NULL);
  //int *p=(int *)malloc(64*sizeof(int));
  int numB=0;
  //bzero(p,64*sizeof(int));
  back=fork();
  //printf(" my is s%d",getpgid(0));
  if(back==0)
  {
    //exit(0);
  }
  else
  wait(NULL);

	FILE* fp;
	if(argc == 2) {
		fp = fopen(argv[1],"r");
		if(fp < 0) {
			printf("File doesn't exists.");
			return -1;
		}
	}

	while(1) {
		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
		if(argc == 2) { // batch mode
      if(fgets(line, sizeof(line), fp) == NULL) { // file reading finished
				break;
			}
			line[strlen(line) - 1] = '\0';
		} else { // interactive mode
			printf("$ ");
			scanf("%[^\n]", line);
			getchar();
		}
		//printf("Command entered: %s\n", line);
		/* END: TAKING INPUT */

		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);
    int p=0;
    //printfull(tokens);
    numB=getComm(tokens);
    while(numB>0) //checkfor background processes
    {
      p=waitpid(-1,NULL,WNOHANG);
      if(p==0)
      {
        break;
      }
      else
      {
      printf("Shell:Background process finished");
      numB=numB-1;
      }
    }
    //doStuff(tokens);

       //do whatever you want with the commands, here we just print them
	//	for(i=0;tokens[i]!=NULL;i++){
      //printf("string got %s \n",tokens[i]);

      //printf("found token %s\n", tokens[i]);
		//}

		// Freeing the allocated memory
		for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);

	}
	return 0;
}
int getComm(char **tokens)
{
  //int *p=(int *)malloc(64*(sizeof(int)));
  int l=0;
  int ret;
  int j=0;
  int isBack=0;
  int p=0;
  while(numB>0) //checkfor background processes
  {
    p=waitpid(-1,NULL,WNOHANG);
    if(p==0)
    {
      break;
    }
    else
    {
    printf("Shell:Background process finished");
    numB=numB-1;
    }
  }

  if(tokens[1]!=NULL && strcmp(tokens[1],"&&&")==0)//1
  {
    //printf("triple \n ");
    //printf("%d",numB);
    tokens[1]=NULL;
    doStuff(tokens,isBack);
    numB=getComm(tokens+2);
    ret=wait(NULL);

  }
  else if(tokens[2]!=NULL && strcmp(tokens[2],"&&&")==0)//2
  {
    //printf("triple 2 \n");
    tokens[2]=NULL;
    doStuff(tokens,isBack);
    numB=getComm((tokens+3));
    ret=wait(NULL);

  }
  else if(tokens[1]!=NULL && strcmp(tokens[1],"&&")==0)//3
  {
    //printf("double 1 \n");
    tokens[1]=NULL;
    doStuff(tokens,isBack);
    ret=wait(NULL);
    numB=getComm(tokens+2);

  }
  else if(tokens[2]!=NULL && strcmp(tokens[2],"&&")==0)//4
  {
    //printf("double 2 \n");
    tokens[2]=NULL;
    doStuff(tokens,isBack);
    ret=wait(NULL);
    numB=getComm(tokens+3);

  }
  else if(tokens[1]!=NULL && strcmp(tokens[1],"&")==0)//5
  {
    //printf("double 11 \n");
    isBack=1;
    tokens[1]=NULL ;
    numB=numB+1;
      doStuff(tokens,isBack);
      numB=getComm(tokens+2);

  }
  else if(tokens[2]!=NULL && strcmp(tokens[2],"&")==0)//6
 {
   //printf("double 12 \n");
   isBack=1;
   tokens[2]=NULL;
   numB=numB+1;
     doStuff(tokens,isBack);
     numB=getComm(tokens+3);
 }
  else if(tokens[0]!=NULL)
  {
    //printf("single");
    doStuff(tokens,isBack);
    ret=wait(NULL);
  }
  return numB;
  //return p;
}
void doStuff(char **tokens, int isBack)
{
  //printfull(tokens);
  int i;
  int p;
  int ret;
  //int *v2;
  //v2=malloc(4);
  char *namels="/bin/ls";
  char *namecat="/bin/cat";
  char *nameecho="/bin/echo";
  char *namesleep="/bin/sleep";
  char *namepwd="/bin/pwd";
  char *nameps="/bin/ps";
  char *namemkdir="/bin/mkdir";
  char *namerm="/bin/rm";
  char *namechmod="/bin/chmod";
  char *nametouch="/bin/touch";
  char *nametar="/bin/tar";
  char *namedate="/bin/date";
  char *namegrep="/bin/grep";
  char *nameping="/bin/ping";
  char *namermdir="/bin/rmdir";
  char *nameumount="/bin/umount";
  char *namekill="/bin/kill";
  char *namemount="/bin/mount";
  char *namegzip="/bin/gzip";
  char *namemv="/bin/mv";
  char **argv=tokens;
 //char *namels="/bin/sleep";
  //char *argv[]={"bin/ls.exe",NULL};
  i=0;
  //for(i=0;tokens[i]!=NULL;i++){
    if(strcmp("ls",tokens[i])==0)
    {
      //printfull(argv);
      p=fork();
      if(p==0)
      {
        if(isBack)
        {
          setpgid(0,back);
        }
        execv(namels,argv);//,envp);
        printf("Shell:Incorrect command\n");
      }
      else;
      //break;

    }
    else if(strcmp("cat",tokens[i])==0)
    {
      p=fork();
      if(p==0)
      {
        if(isBack)
        {
          setpgid(0,back);
        }
        execv(namecat,argv);
        printf("Shell:Incorrect command\n");
      }
    }
    else if(strcmp("echo",tokens[i])==0)
    {
      p=fork();
      if(p==0)
      {
        if(isBack)
        {
          setpgid(0,back);
        }
        execv(nameecho,argv);
        printf("Shell:Incorrect command\n");
      }
    }
    else if(strcmp("sleep",tokens[i])==0)
    {
        //printf("back is %d ",back);
        //printf("isBack is %d ",isBack);
        p=fork();
        if(p==0)
        {

          if(isBack)
          {

            setpgid(getpid(),back);

          }
          execv(namesleep,argv);
          printf("Shell:Incorrect command\n");
        }
        //printf("%d",getpgid(p));
      //  else
        //ret=wait(NULL);
    }
    else if(strcmp("pwd",tokens[i])==0)
    {
        p=fork();
        if(p==0)
        {
          if(isBack)
          {
            setpgid(0,back);
          }
          execv(namepwd,argv);
          printf("Shell:Incorrect command\n");
        }
      //  else
        //ret=wait(NULL);
    }
    else if(strcmp("ps",tokens[i])==0)
    {
        p=fork();
        if(p==0)
        {
          if(isBack)
          {
            setpgid(0,back);
          }
          execv(nameps,argv);
          printf("Shell:Incorrect command\n");
        }
      //  else
        //ret=wait(NULL);
    }
    else if(strcmp("mkdir",tokens[i])==0)
    {
        p=fork();
        if(p==0)
        {
          if(isBack)
          {
            setpgid(0,back);
          }
          execv(namemkdir,argv);
          printf("Shell:Incorrect command\n");
        }
      //  else
        //ret=wait(NULL);
    }
    else if(strcmp("rm",tokens[i])==0)
    {
        p=fork();
        if(p==0)
        {
          if(isBack)
          {
            setpgid(0,back);
          }
          execv(namerm,argv);
          printf("Shell:Incorrect command\n");
        }
      //  else
        //ret=wait(NULL);
    }
    else if(strcmp("chmod",tokens[i])==0)
    {
        p=fork();
        if(p==0)
        {
          if(isBack)
          {
            setpgid(0,back);
          }
          execv(namechmod,argv);
          printf("Shell:Incorrect command\n");
        }
      //  else
        //ret=wait(NULL);
    }
    else if(strcmp("touch",tokens[i])==0)
    {
        p=fork();
        if(p==0)
        {
          if(isBack)
          {
            setpgid(0,back);
          }
          execv(nametouch,argv);
          printf("Shell:Incorrect command\n");
        }
      //  else
        //ret=wait(NULL);
    }
    else if(strcmp("tar",tokens[i])==0)
    {
        p=fork();
        if(p==0)
        {
          if(isBack)
          {
            setpgid(0,back);
          }
          execv(nametar,argv);
          printf("Shell:Incorrect command\n");
        }
      //  else
        //ret=wait(NULL);
    }
    else if(strcmp("date",tokens[i])==0)
    {
        p=fork();
        if(p==0)
        {
          if(isBack)
          {
            setpgid(0,back);
          }
          execv(namedate,argv);
          printf("Shell:Incorrect command\n");
        }
      //  else
        //ret=wait(NULL);
    }
    else if(strcmp("grep",tokens[i])==0)
    {
        p=fork();
        if(p==0)
        {
          if(isBack)
          {
            setpgid(0,back);
          }
          execv(namegrep,argv);
          printf("Shell:Incorrect command\n");
        }
      //  else
        //ret=wait(NULL);
    }
    else if(strcmp("ping",tokens[i])==0)
    {
        p=fork();
        if(p==0)
        {
          if(isBack)
          {
            setpgid(0,back);
          }
          execv(nameping,argv);
          printf("Shell:Incorrect command\n");
        }
      //  else
        //ret=wait(NULL);
    }

    else if(strcmp("rmdir",tokens[i])==0)
    {
        p=fork();
        if(p==0)
        {
          if(isBack)
          {
            setpgid(0,back);
          }
          execv(namermdir,argv);
          printf("Shell:Incorrect command\n");
        }
      //  else
        //ret=wait(NULL);
    }

    else if(strcmp("umount",tokens[i])==0)
    {
        p=fork();
        if(p==0)
        {
          if(isBack)
          {
            setpgid(0,back);
          }
          execv(nameumount,argv);
          printf("Shell:Incorrect command\n");
        }
      //  else
        //ret=wait(NULL);
    }

    else if(strcmp("kill",tokens[i])==0)
    {
        p=fork();
        if(p==0)
        {
          if(isBack)
          {
            setpgid(0,back);
          }
          execv(namekill,argv);
          printf("Shell:Incorrect command\n");
        }
      //  else
        //ret=wait(NULL);
    }

    else if(strcmp("mount",tokens[i])==0)
    {
        p=fork();
        if(p==0)
        {
          if(isBack)
          {
            setpgid(0,back);
          }
          execv(namemount,argv);
          printf("Shell:Incorrect command\n");
        }
      //  else
        //ret=wait(NULL);
    }
    else if(strcmp("gzip",tokens[i])==0)
    {
        p=fork();
        if(p==0)
        {
          if(isBack)
          {
            setpgid(0,back);
          }
          execv(namegzip,argv);
          printf("Shell:Incorrect command\n");
        }
      //  else
        //ret=wait(NULL);
    }

    else if(strcmp("mv",tokens[i])==0)
    {
        p=fork();
        if(p==0)
        {
          if(isBack)
          {
            setpgid(0,back);
          }
          execv(namemv,argv);
          printf("Shell:Incorrect command\n");
        }
      //  else
        //ret=wait(NULL);
    }

    else if(strcmp("cd",tokens[i])==0)
    {
      //printf("cd change directory \n   %s \n",*(argv+1));
      //p=fork();
      char **path=argv+1;
      ret=chdir(*(path));
      p=fork();//fork done so that the wait does not block
      if(p==0)
      {
        exit(0);
      }
      //printf("%d",ret);
    }

    else if(strcmp("exit",tokens[i])==0)
    {
      free(myPid);
      kill(-back,SIGTERM);// all background process also closed
      kill(0,SIGTERM);
      exit(0);
      //free();
    }

      else
      {
        if(isBack)
        {
          numB=numB-1;
        }
        printf("Shell:Incorrect command\n");
      }
  //}
  //ret=wait(NULL);
  //printf("%d",ret);
  //printf("done $");
  //free(v2);
}
void printfull(char **p)
{
  for(int i=0;p[i]!=NULL;i++){
      printf("string got %s \n",p[i]);

      //printf("found token %s\n", tokens[i]);
		}
}
