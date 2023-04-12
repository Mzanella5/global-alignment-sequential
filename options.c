#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "options.h"

int count_chars(char *s, char c) {
    int count = 0;
    int len = strlen(s);
    for (int i = 0; i < len - 1; i++) {
        if (s[i] == c) {
            count++;
        }
    }
    return count;
}

Option* process_options(int argc, char *argv[], int *num_options) {
    int i, j;
    bool verbose = false, know_option = false;
    *num_options = 4;

    Option *options = (Option*) malloc(*num_options * sizeof(Option));

    strcpy(options[0].option, "--match");
    options[0].value = 2;

    strcpy(options[1].option, "--mismatch");
    options[1].value = -1;

    strcpy(options[2].option, "--gap");
    options[2].value = -1;

    strcpy(options[3].option, "-verbose");
    options[3].value = 0;

    for(i = 3; i < argc; i++) {
        know_option = false;
        for(j = 0; j < *num_options; j++) {
            if(strcmp(argv[i], options[j].option) == 0) {
                if(i+1 < argc && count_chars(argv[i], '-') == 2) {
                    options[j].value = atoi(argv[i+1]);
                    i++;
                } 
                if(count_chars(argv[i], '-') == 1) {
                    options[j].value =  1;
                }
                know_option = true;
                break;          
            }
        }
        if(!know_option) {
            printf("Unknown argument: %s\n", argv[i]);
        }
    } 
    return options;
}

