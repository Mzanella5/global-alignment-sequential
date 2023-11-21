#ifndef OPTIONS_H
#define OPTIONS_H

typedef struct
{
    char option[100];
    int value;
} Option;

typedef struct
{
    char sequence_name[100];
    int size;
} read_data_result;

int count_chars(char *s, char c);

Option* process_options(int argc, char *argv[], int *num_options);

#endif