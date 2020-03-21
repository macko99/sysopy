#ifndef LIBRARY_DYN
#define LIBRARY_DYN
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

static void *handle = NULL;

int (*_create_array)(unsigned int pairs_count);
void (*_define_files_sequence)(char *sequence);
int (*_compare_pairs)();
int (*_get_operations_count)(int idx);
int (*_file_to_array)();
int (*_remove_block)(int idx);
int (*_remove_operation)(int block_idx, int operation_idx);
char *(*_read_file)();
int (*_get_file_size)();
int (*_diff_files)(char *file1, char *file2);

void init_dynamic_library()
{
    handle = dlopen("my_lib.so", RTLD_NOW);
    if (handle == NULL)
    {
        fprintf(stderr, "Error dynamic library open");
        return;
    }

    _diff_files = dlsym(handle, "diff_files");
    _create_array = dlsym(handle, "create_array");
    _define_files_sequence = dlsym(handle, "define_files_sequence");
    _compare_pairs = dlsym(handle, "compare_pairs");
    _get_operations_count = dlsym(handle, "get_operations_count");
    _file_to_array = dlsym(handle, "file_to_array");
    _remove_block = dlsym(handle, "remove_block");
    _remove_operation = dlsym(handle, "remove_operation");
    _read_file = dlsym(handle, "read_file");
    _get_file_size = dlsym(handle, "get_file_size");

    char *error;
    if ((error = dlerror()) != NULL)
    {
        fprintf(stderr, "%s\n", error);
        exit(1);
    }
}

int create_array(unsigned int pairs_count)
{
    return (*_create_array)(pairs_count);
}
int diff_files(char *file1, char *file2){
  return (*_diff_files)(file1, file2);
}
char* read_file()
{
  return (*_read_file)();
}
void define_files_sequence(char *sequence)
{
    return (*_define_files_sequence)(sequence);
}
int compare_pairs()
{
    return (*_compare_pairs)();
}
int get_operations_count(int idx)
{
    return (*_get_operations_count)(idx);
}
int file_to_array()
{
    return (*_file_to_array)();
}
int remove_block(int idx)
{
    return (*_remove_block)(idx);
}
int remove_operation(int block_idx, int operation_idx)
{
    return (*_remove_operation)(block_idx, operation_idx);
}
int get_file_size()
{
  return (*_get_file_size)();
}

#endif
