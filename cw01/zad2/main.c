#ifdef DYNAMIC
#include "library_dyn.h"
#else
#include "library.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <sys/resource.h>

clock_t start_time, end_time;
struct tms start_cpu, end_cpu;
FILE *report_file;

void start_timer(){
    start_time = times(&start_cpu);
}

void end_timer(){
    end_time = times(&end_cpu);
}

void write_file_header(FILE *f){
    fprintf(f, "%30s\t\t%15s\t%15s\t%15s\n",
            "Name",
            "Real time[s]",
            "User time[s]",
            "System time[s]");
}

void save_report(char *name, FILE *f){
    end_timer();
    int clk_tics = sysconf(_SC_CLK_TCK);
    double real_time = (double)(end_time - start_time) / clk_tics;
    double user_time = (double)(end_cpu.tms_utime - start_cpu.tms_utime) / clk_tics;
    double system_time = (double)(end_cpu.tms_stime - start_cpu.tms_stime) / clk_tics;
    fprintf(f, "%30s:\t\t%15f\t%15f\t%15f\t\n",
            name,
            real_time,
            user_time,
            system_time);
}

int parse_create_array(char *argv[], int i, int argc){
    if (i + 1 >= argc){
        fprintf(stderr, "create array - bad args number\n");
        return -1;
    }
    int size = atoi(argv[i + 1]);
    return create_array(size);
}
int parse_end_timer(char *argv[], int i, int argc){
    if (i + 1 >= argc){
        fprintf(stderr, "end timer - bad args number\n");
        return -1;
    }
    end_timer();
    char *timer_name = argv[i + 1];
    save_report(timer_name, report_file);
    return 0;
}

int parse_remove_block(char *argv[], int i, int argc){
    if (i + 1 >= argc){
        fprintf(stderr, "remove block - bad args number\n");
        return -1;
    }
    int index = atoi(argv[i + 1]);
    return remove_block(index);
}
int parse_compare_pairs(char *argv[], int i, int argc){
  if (i + 1 >= argc){
      fprintf(stderr, "compare pairs - bad args number\n");
      return -1;
  }
  char *pairs_sequence = argv[i + 1];
  define_files_sequence(pairs_sequence);
  compare_pairs();
  return 1;
}
int parse_remove_operation(char *argv[], int i, int argc){
  if (i + 2 >= argc){
      fprintf(stderr, "remove operation - bad args number\n");
      return -1;
  }
    int block_idx = atoi(argv[i + 1]);
    int op_idx = atoi(argv[i + 2]);
    remove_operation(block_idx, op_idx);
    return 1;
}

int main(int argc, char *argv[]){
  #ifdef DYNAMIC
    init_dynamic_library();
  #endif
    report_file = fopen("raport2.txt", "a");
    write_file_header(report_file);

    int i = 1;
    while (i < argc){
        if (!strcmp(argv[i], "create_array")){
            int status = parse_create_array(argv, i, argc);
            if (status < 0){
                fprintf(stderr, "error -> stopping\n");
                return -1;
            }
            i += 2;
        }
        else if (!strcmp(argv[i], "compare_pairs")){
            int status = parse_compare_pairs(argv, i, argc);
            if (status < 0){
                fprintf(stderr, "error -> stopping\n");
                return -1;
            }
            i += 2;
        }
        else if (!strcmp(argv[i], "remove_operation")){
            int status = parse_remove_operation(argv, i, argc);
            if (status < 0){
                fprintf(stderr, "error -> stopping\n");
                return -1;
            }
            i += 3;
        }
        else if (!strcmp(argv[i], "remove_block")){
            int status = parse_remove_block(argv, i, argc);
            if (status < 0){
                fprintf(stderr, "error -> stopping\n");
                return -1;
            }
            i += 2;
        }
        else if (!strcmp(argv[i], "file_to_array")){
            file_to_array();
            i++;
        }
        else if (!strcmp(argv[i], "start")){
            start_timer();
            i += 1;
        }
        else if (!strcmp(argv[i], "end")){
          int status = parse_end_timer(argv, i, argc);
          if (status < 0){
              fprintf(stderr, "error -> but not stopping\n");
          }
            i += 2;
        }
        else{
            fprintf(stderr, "Command undeclared -> stopping\n");
            return -1;
        }
    }
    return 0;
}
