#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 80  //the maximum length command
#define MAX_ARGS (MAX_LINE/2 + 1)
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
  return 0;
}
