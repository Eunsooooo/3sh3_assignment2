#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 80  //the maximum length command
#define MAX_ARGS (MAX_LINE/2 + 1) //max tokens
#define HIST_SIZE 5

typedef struct {
  int number;
  char cmd[MAX_LINE + 1];
} HistEntry;

static HistEntry hist[HIST_SIZE];
static int hist_count = 0;
static int next_number = 1;

static void history_add(const char *line) {
  if(!line || !*line)
    return;
  HistEntry *slot = &hist[hist_count % HIST_SIZE];
  slot->number = next_number++;
  strncpy(slot->cmd, line, MAX_LINE);
  slot->cmd[MAX_LINE] = '\0';
  hist_count++;
}

static void history_print(void){
  if (hist_count == 0)
    return;
}

int main(void){

  char *args[MAX_LINE/2 + 1]; //command line arguments
  char line[MAX_LINE + 1];  //command line
  int should_run = 1; //flag to determine when to exit program

  while (should_run) {
    printf("osh>");
    fflush(stdout);

    //read the line
    if (fgets(line, sizeof(line), stdin) == NULL) {
      putchar('\n');
      break; // Exit on EOF (ctrl D) or read error
    }

    //strip trailing newline
    size_t len = strlen(line);
    if(len > 0 && line[len - 1] == '\n') {
      line[len - 1] = '\0';
    }

    //skip empty inputs
    if (line[0] == '\0') {
      continue;
    }
      
    //tokenize the input line
    int argc = 0;
    char *token = strtok(line, " \t");
    while (token && argc < (int)(MAX_LINE/2)) {
      args[argc++] = token;
      token = strtok(NULL, " \t");
    } 
    args[argc] = NULL; //null-terminate the args array

    //built in command: exit
    if (argc > 0 && strcmp(args[0], "exit") == 0) {
      should_run = 0;
      continue;

  }

  //run parent in backgroun if '&' is in the end of command
  int background = 0;
  if (argc > 0 && strcmp(args[argc - 1], "&") == 0) {
    background = 1;
    //TODO 


  }

  //fork child process
  pid_t pid = fork();
  if (pid < 0) {
    perror("Fork failed");
    continue;
  }

  //child process
  if (pid == 0) {
    // (2) child process will envoke execvp to run the command

    //parent will envoke wait unless the command included '&'

    //reap any background children that have finsished

  }
  



  return 0;
}
