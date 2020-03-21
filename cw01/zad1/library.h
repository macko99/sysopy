#ifndef LIBRARY
#define LIBRARY

int create_array(unsigned int pairs_count);
int diff_files(char *file1, char *file2);
int get_file_size();
char* read_file();
int file_to_array();
int remove_block(int idx);
int get_operations_count(int idx);
int remove_operation(int block_idx, int operation_idx);
void define_files_sequence(char *sequence);
int compare_pairs();

#endif
