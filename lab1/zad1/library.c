#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

char ***array = NULL;
unsigned int array_size = 0;
char *files_sequence = NULL;

int create_array(unsigned int pairs_count){
    array = (char***)calloc(pairs_count, sizeof(char**));
    array_size = pairs_count;
    return 1;
}

int diff_files(char *file1, char *file2){
  system("touch tmp.txt");
  char tmp[17 + strlen(file1) + strlen(file2)];
  strcpy(tmp, "diff ");
  strcat(tmp, file1);
  strcat(tmp, " ");
  strcat(tmp, file2);
  strcat(tmp, " > tmp.txt");
  system(tmp);
  return 1;
}

int get_file_size(){
  FILE* fp = fopen("tmp.txt", "r");
  fseek(fp, 0L, SEEK_END);
  int file_size = ftell(fp);
  fclose(fp);
  return file_size;
}

char* read_file(){
  int fd = open("tmp.txt", O_RDONLY);
  char *buffer = (char*) calloc(get_file_size(), sizeof(char));
  read(fd,buffer,get_file_size());
  return buffer;
}


int file_to_array(){
  char *buffor = read_file();

  int block_count = 0;
  for(int i = 0; i < (int)strlen(buffor); i++){
    if(i == 0 || (buffor[i] >= '0' && buffor[i] <= '9' && buffor[i-1] == '\n')){
      block_count++;
    }
  }

  char** edit_block_arr = (char**)calloc(block_count, sizeof(char*));
  for(int i=0; i<block_count; i++){
    edit_block_arr[i] = (char*) calloc(100, sizeof(char));
  }

  int edit_block_arr_idx = 0;
  for(int i = 0; i < (int)strlen(buffor); i++){
    if(buffor[i] >= '0' && buffor[i] <= '9' && buffor[i-1] == '\n'){
      edit_block_arr_idx++;
    }
    char *tmp_block = calloc(1, sizeof(char));
    tmp_block[0] = buffor[i];
    strcat(edit_block_arr[edit_block_arr_idx], tmp_block);
  }

  for (int i = 0; i < array_size; i++){
    if (array[i] == NULL){
      array[i] = edit_block_arr;
      return i;
    }
  }

  printf("error adding edit block array to main array\n");
  return -1;
}

int remove_block(int idx){
    free(array[idx]);
    array[idx] = NULL;
    return 1;
}

int get_operations_count(int idx){
  if(array[idx] == NULL){
    printf("error - no such block\n");
    return -1;
  }
  int i = 1;
  while (array[idx][i] != NULL){
    i++;
  }
  return i;
}

int remove_operation(int block_idx, int operation_idx){
  if(array[block_idx] == NULL){
    printf("error - no such block\n");
    return -1;
  }
  if(array[block_idx][operation_idx] == NULL){
    printf("error - no such operation");
    return -1;
  }
  for(int i=operation_idx; i<get_operations_count(block_idx); i++){
    array[block_idx][i] = array[block_idx][i+1];
  }
  return 0;
}
void define_files_sequence(char *sequence){
    files_sequence = sequence;
    return;
}
int compare_pairs(){
  char *string = strdup(files_sequence);
  char* tokend_string = strtok(string, ":");

  while(tokend_string != NULL){
    char* file1 = tokend_string;
    tokend_string = strtok(NULL, ":");
    char* file2 = tokend_string;
    tokend_string = strtok(NULL, ":");
    diff_files(file1, file2);
    file_to_array();
  }
  return 1;
}
