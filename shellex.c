/* $begin shellmain */
#include "csapp.h"
#define MAXARGS   128

/* Function prototypes */
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv); 

/* Functions linked to built in commands */
void mypid() {
  printf("Current process id of process: %d\n", getpid());
}

void myppid() {
  printf("Current process id of parent process: %d\n", getppid());
}

void myhelp() {
  printf("Name of shell devloper: Joe Brashear\n");
  printf("Change prompt: using ./shell -p [prompt]\n");
  printf("man [command] for help with system commands...\n");
  printf("List of built-in-commands: \n");
  printf("\t exit - exit from shell \n\t pid - shows id of currnet process \n"); 
  printf("\t ppid - shows id of parent process \n\t help - shows help menu \n");
  printf("\t cd - changes current working directory \n");
}
 

/* Catch function for CRTL + C */
void mycatch() {
  printf(" --> CRTL + C is disabled... \n");
}

int main(int argc, char **argv) 
{
  /* If the user executes "./sh257 -p [propmt]" this will flag the request to 
     change the shell propmt and update the prompt                        */
  int changePrompt = 0;
  char userPrompt[MAXARGS] = "";
  if(argc > 2 && !strcmp(argv[1], "-p")) {
    strcpy(userPrompt, argv[2]);
    changePrompt++;
  }	   

  char cmdline[MAXLINE];
    while (1) {
      signal(SIGINT, mycatch);
      /* Read */

      if(!changePrompt) {
        printf("sh257> ");
      }
      else {
        printf("%s> ", userPrompt); 
      }

      Fgets(cmdline, MAXLINE, stdin); 
      if (feof(stdin))
        exit(0);

      /* Evaluate */
      eval(cmdline);
    } 

}
/* $end shellmain */
  
/* $begin eval */
/* eval - Evaluate a command line */
void eval(char *cmdline) 
{
  char *argv[MAXARGS]; /* Argument list execvp() */             
  char buf[MAXLINE];   /* Holds modified command line */
  int bg;              /* Should the job run in bg or fg? */
  pid_t pid;           /* Process id */
    
  strcpy(buf, cmdline);
  bg = parseline(buf, argv); 
  if (argv[0] == NULL)  
  return;   /* Ignore empty lines */

  if (!builtin_command(argv)) { 
    if ((pid = Fork()) == 0) {   /* Child runs user job */
      if (execvp(argv[0], argv) < 0) {                         
         printf("%s: Command not found.\n", argv[0]);
         exit(0);
      }
    }
    /* Parent waits for foreground job to terminate */
    if (!bg) {
      int status;
      if (waitpid(pid, &status, 0) < 0)
      unix_error("waitfg: waitpid error");
    }
    else
      printf("%d %s", pid, cmdline);
    }
    return;
}

/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv) 
{
  if (!strcmp(argv[0], "quit")) /* quit command */
    exit(0);  

  if (!strcmp(argv[0], "exit")) { /* exit command */
    raise(6);
    return 1;
  }

  if (!strcmp(argv[0], "pid")) { /* get  pid */
    mypid();
    return 1;
  }

  if (!strcmp(argv[0], "ppid")) { /* get  ppid */
    myppid();
    return 1;
  }

  if (!strcmp(argv[0], "help")) { /* get  help */
    myhelp();
    return 1;
  }

  if (!strcmp(argv[0], "cd")) { /* gets working directory or changes it */
    if(argv[1] == NULL){
      printf("%s\n", getcwd(*argv, MAXARGS));
      return 1;
    }
    else{
      //Changes the directory to the value specified by argv[1]
      chdir(argv[1]);
      return 1;
    }
  }

  else if (!strcmp(argv[0], "&"))    /* Ignore singleton & */
    return 1;
   
  return 0;                     /* Not a builtin command */
}
/* $end eval */

/* $begin parseline */
/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv) 
{
  char *delim;         /* Points to first space delimiter */
  int argc;            /* Number of args */
  int bg;              /* Background job? */

  buf[strlen(buf)-1] = ' ';  /* Replace trailing '\n' with space */
  while (*buf && (*buf == ' ')) /* Ignore leading spaces */
    buf++;

    /* Build the argv list */
    argc = 0;
    while ((delim = strchr(buf, ' '))) {
      argv[argc++] = buf;
      *delim = '\0';
      buf = delim + 1;
      while (*buf && (*buf == ' ')) /* Ignore spaces */
        buf++;
    }
    argv[argc] = NULL;
    
    if (argc == 0)  /* Ignore blank line */
      return 1;

    /* Should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0)
    argv[--argc] = NULL;

    return bg;
}
/* $end parseline */
