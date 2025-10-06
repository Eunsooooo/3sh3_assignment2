#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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
  int n = hist_count < HIST_SIZE ? hist_count : HIST_SIZE;
  for (int i = 0; i < n; ++i){
    int pos = (hist_count - 1 - i) % HIST_SIZE;
    if (pos < 0) pos += HIST_SIZE;
    printf("%d %s\n", hist[pos].number, hist[pos].cmd);
  }
}

static const char* history_last(void){
  if (hist_count == 0)
    return NULL;
  return hist[(hist_count - 1) % HIST_SIZE].cmd;
}

static int parse_to_args(char *line, char *args[MAX_ARGS]){
  int argc = 0;
  char *tok = strtok(line, " \t\n");
  while (tok && argc < MAX_ARGS - 1){
    args[argc++] = tok;
    tok = strtok(NULL, " \t\n");
  }
  args[argc] = NULL;
  if(argc == 0)
    return 0;
  int background = 0;
  char *last = args[argc - 1];
  size_t L = strlen(last);
  if (strcmp(last, "&") == 0){
    args[argc - 1] = NULL;
    background = 1;
  }
  else if (L > 0 && last[L - 1] == '&'){
    last[L - 1] = '\0';
    if (last[0] == '\0') args[argc - 1] = NULL;
    background = 1;
  }
  return background;
}

int main(void) {

  char *args[MAX_LINE/2 + 1]; //command line arguments
  char line[MAX_LINE + 1];  //command line
  int should_run = 1; //flag to determine when to exit program

  while (should_run) {
    printf("osh>");
    fflush(stdout);

    //read the line
    //Exit on EOF (ctrl D) or read error
    if (fgets(line, sizeof(line), stdin) == NULL) {
      putchar('\n');
      break; 
    }

    //Strip trailing newline
    size_t len = strlen(line);
    if(len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';

    //Empty input
    if (line[0] == '\0') continue;
      

    //Tokenize the input into args array
    int argc = 0;
    char *token = strtok(line, " \t");
    while (token && argc < (int)(MAX_LINE/2)) {
      args[argc++] = token;
      token = strtok(NULL, " \t");
    } 
    //NULL terminate the args array
    args[argc] = NULL; 
    if (argc == 0) continue; 


    //Built in exit command
    if (strcmp(args[0], "exit") == 0) {
      should_run = 0;
      continue;
    }

    //Running parent in background if last arg '&'
    int background = 0;
    if (argc > 0 && strcmp(args[argc - 1], "&") == 0) {
      background = 1;
      
      //remove '&' from args
      args[argc - 1] = NULL;  
      argc--;
      if (argc == 0) continue;

    }

    //Fork child process
    pid_t pid = fork();
    //child process
    if (pid == 0) {
      // (2) child process will envoke execvp to run the command
      execvp(args[0], args);
      //error handling, child process exits if execvp fails
      perror("Execution failed");
      _exit(127); 
    } else if (pid > 0) {
      // (3) parent process will envoke wait unless the command included '&'
      if (!background) {
        waitpid(pid, NULL, 0); 
      }   
    } else {
      //fork failed
      perror("Fork failed");
    }

    char input[MAX_LINE + 2];

    if (strcmp(args[0], "history") == 0){  //history
      if (hist_count) history_print();
      continue;
    }

    history_add(input);

  }
  return 0;
}
