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
  char *args[MAX_ARGS];          // use MAX_ARGS
  char line[MAX_LINE + 1];
  char input[MAX_LINE + 1];      // pristine copy for history
  int should_run = 1;

  while (should_run) {
    printf("osh>");
    fflush(stdout);

    if (fgets(line, sizeof(line), stdin) == NULL) { putchar('\n'); break; }

    // strip newline
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
    if (line[0] == '\0') continue;

    // keep an original copy before tokenizing
    strncpy(input, line, MAX_LINE);
    input[MAX_LINE] = '\0';

    // built-ins that should not be stored
    if (strcmp(input, "history") == 0) { 
      history_print(); continue; 
    }

    if (strcmp(args[0], "!!") == 0) {
      const char *last = history_last();
      if (!last) { printf("No commands in history.\n"); continue; }
      printf("%s\n", last);

      char execbuf[MAX_LINE + 1];
      strncpy(execbuf, last, MAX_LINE);
      execbuf[MAX_LINE] = '\0';
      char *eargs[MAX_ARGS];
      int bg = parse_to_args(execbuf, eargs);

      pid_t pid = fork();
      if (pid < 0) { perror("fork"); continue; }
      if (pid == 0) { execvp(eargs[0], eargs); perror("execvp"); _exit(1); }
      if (!bg) waitpid(pid, NULL, 0);

      history_add(last);
      continue;
    }

    char execbuf[MAX_LINE + 1];
    strncpy(execbuf, input, MAX_LINE);
    execbuf[MAX_LINE] = '\0';
      
    int background = parse_to_args(execbuf, args);
    if (!args[0]) continue;

    // built-in exit
    if (strcmp(args[0], "exit") == 0) {
      should_run = 0;
      continue;
    }
    history_add(input);  // store only real commands

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
  }
  return 0;
}