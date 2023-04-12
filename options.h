#ifndef OPTIONS_H
#define OPTIONS_H

typedef struct
{
    char option[100];
    int value;
} Option;

int count_chars(char *s, char c);

Option* process_options(int argc, char *argv[], int *num_options);

#endif