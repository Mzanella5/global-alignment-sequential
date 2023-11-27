# Global Alignment

### This is an DNA alignment sequential tool that implements the Needleman-Wunsch algorithm that uses a Global Alignment method

#### To Execute this application you can use Linux and have the compiler gcc and the OpenMP library
#### Compile: gcc -fopenmp main.c options.c options.h -o main
#### Run: ./main sequence_file_1 sequence_file_2 -help -flag_options --parameters_options
#### Example: ./main samples/30k1.fna samples/30k2.fna --match 2 --mismatch -3 --gap -5 --gap_seq -2 -verbose
