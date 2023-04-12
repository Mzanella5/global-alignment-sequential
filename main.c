#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include "options.h"

#define getName(var)  #var
int SIZEA, SIZEB, SIZERES;
int VERBOSE = 0;
int MATCH = 2;
int MISMATCH = -1;
int GAP = -1;


int Similarity(char first, char second)
{
    if(first == '-' || second == '-')
        return GAP;
    if(first == second)
        return MATCH;
    return MISMATCH;
}

int FunctionSimilarity(int** mat, char a, char b, int i, int j, char *pos)
{
    int v1=INT_MIN,v2=INT_MIN,v3=INT_MIN;
    int result = 0;

    if(i-1 >= 0 && j-1 >= 0)
        v1 = mat[i-1][j-1] + Similarity(a, b);
    if(i-1 >= 0)
        v2 = mat[i-1][j] + Similarity(a, '-');
    if(j-1 >= 0)
        v3 = mat[i][j-1] + Similarity('-', b);

    result = v2;
    *pos = 'V';
    if(v1 > result)
    {
        result = v1;
        *pos = 'D';
    }
    if(v3 > result)
    {
        result = v3;
        *pos = 'H';
    }

    return result;
}

void PrintMatrix(int **mat)
{
    for (int i = 0; i < SIZEA; i++)
    {
        for (int j = 0; j < SIZEB; j++)
        {
            if(mat[i][j] >= 0)
                printf(" %d | ", mat[i][j]);
            else
                printf("%d | ", mat[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int** InitializeMatrix(int sizex, int sizey)
{
    int** mat = (int**) calloc(sizex, sizeof(int*));
    for (int i = 0; i < sizex; i++)
        mat[i] = (int*) calloc(sizey, sizeof(int));
    
    return mat;
}

void FreeMatrix(int** mat)
{
    for (int i = 0; i < SIZEA; i++)
        free(mat[i]);
    free(mat);
}

void CalculateSimilarity(int **mat, char *vetA, char *vetB)
{
    int i,j;
    char *pos = (char*) calloc(1, sizeof(char));

    for (i=1; i < SIZEA; i++)
    {
        mat[i][0] = mat[i-1][0] + Similarity(vetA[i], '-');
    }

    for (i=1; i < SIZEB; i++)
    {
        mat[0][i] = mat[0][i-1] + Similarity('-', vetB[i]);
    }

    for (i=1; i < SIZEA; i++)
        for(j=1; j < SIZEB; j++)
        {
            mat[i][j] = FunctionSimilarity(mat, vetA[i], vetB[j], i, j, pos);
        }

    free(pos);
    return;
}

void MountSequence(int **mat, char *vetA, char *vetB, char **vetResA, char **vetResB)
{
    int i,j,k,l;
    char *pos = (char*) calloc(1, sizeof(char));

    for (i=0; i < SIZERES; i++)
    {
        *(*(vetResA) + i) = ' ';
        *(*(vetResB) + i) = ' ';
    }

    i = SIZEA-1;
    j = SIZEB-1;

    k=SIZERES;
    l=SIZERES;

    if(VERBOSE)
        printf("Matrix back path: ");

    do
    {
        k--;
        l--;
        FunctionSimilarity(mat, vetA[i], vetB[j], i, j, pos);
        if(i==0 && j==0) break;

        if(VERBOSE)
            printf(" %c |", *pos);

        if(*pos == 'D')
        {
            *(*(vetResA) + k) = vetA[i];
            *(*(vetResB) + l) = vetB[j];
            i--;
            j--;
        }
        else if(*pos == 'V')
        {
            *(*(vetResA) + k)  = vetA[i];
            *(*(vetResB) + l) = '-';
            i--;
        }
        else
        {
            *(*(vetResA) + k)  = '-';
            *(*(vetResB) + l)= vetB[j];
            j--;
        }
    } while (i >= 0 && j >= 0);

    free(pos);
    printf("\n");
    return;
}

void PrintVector(char *vet, int size)
{
    int i;
    for (i=0; i < size; i++)
        printf("%c ", vet[i]);
    printf("\n");
}

int ReadFastaData(char **vet, char *path)
{
    FILE* file;
    char ch;
    int i = 1, size = 1, skipLine = 0;

    file = fopen(path, "r");
    if(file == NULL)
    {
        printf("\nCan't read the file!\n");
        return size;
    }

    do {
        ch = fgetc(file);

        if(skipLine == 1 && ch == '\n')
            skipLine = 0;

        if(ch == '>')
            skipLine = 1;

        if((ch >= 'A' && ch <= 'Z' || ch >= 'a' && ch <= 'z') && skipLine == 0)
            size++;
        
    
    } while(ch != EOF);

    printf("SIZE: %d\n", size);

    *vet = (char*) calloc(size, sizeof(char));

    fseek( file, 0, SEEK_SET );
    skipLine = 0;
    do {
        ch = fgetc(file);

        if(skipLine == 1 && ch == '\n')
            skipLine = 0;

        if(ch == '>')
            skipLine = 1;

        if(ch >= 'a' && ch <= 'z')
            ch -= 32;

        if(ch >= 'A' && ch <= 'Z' && skipLine == 0)
        {
            *((*vet) + i) = ch;
            i++;
        }
    } while (ch != EOF);
    
    fclose(file);
    return size;
}

void ReadData(char **vetA, char **vetB, char *path)
{
    FILE* file;
    char ch;
    int next = 1, i, j;
    SIZEA = 1;
    SIZEB = 1;

    file = fopen(path, "r");
    if(file == NULL)
    {
        printf("\nCan't read the file!\n");
        return;
    }

    do {
        ch = fgetc(file);
        // ch = (int)toupper(ch);
        // printf("%c", ch);
        // if(ch >= 'a' && ch <= 'z')
        //     ch = ch - ' ';

        if(ch == '\n')
        {
            next = 0;
        }
        if(next == 1)
        {
            if(ch >= 'A' && ch <= 'Z' || ch >= 'a' && ch <= 'z')
            {
                SIZEA++;
            }
        }
        else
        {
            if(ch >= 'A' && ch <= 'Z' || ch >= 'a' && ch <= 'z')
            {
                SIZEB++;   
            }
        }
    
    } while(ch != EOF);

    printf("SIZEA: %d\n", SIZEA);
    printf("SIZEB: %d\n", SIZEB);

    *vetA = (char*) calloc(SIZEA, sizeof(char));
    *vetB = (char*) calloc(SIZEB, sizeof(char));

    fseek( file, 0, SEEK_SET );

    next = 1;
    i=1;
    j=1;
    do {
        ch = fgetc(file);

        if(ch == '\n')
            next = 0;

        if(next)
        {
            if(ch >= 'A' && ch <= 'Z' || ch >= 'a' && ch <= 'z')
            {
                *((*vetA) + i) = ch;
                i++;
            }
        }
        else
        {
            if(ch >= 'A' && ch <= 'Z' || ch >= 'a' && ch <= 'z') 
            {
                *((*vetB) + j) = ch;
                j++;
            }
        }
    } while (ch != EOF);
    
    fclose(file);
    return;
}

void PrintResults(char *vetA, char *vetB, int size)
{
    int i, hits=0, misses=0, gaps=0;
    char a, b;
    for(i=0; i < size; i++)
    {
        a = vetA[i];
        b = vetB[i];

        if(a >= 'A' && a <= 'Z' && b >= 'A' && b <= 'Z')
        {
            if(a == b)
            {
                hits++;
                continue;
            }
            else
            {
                misses++;
                continue;
            }
        }

        if(a == '-')
            gaps++;

        if(b == '-')
            gaps++;

    }

    printf("======Results======\n");
    printf("Hits: %d \nMisses: %d \nGaps: %d \nAlignmentSize: %d \n", hits, misses, gaps, size);

}

int main(int argc, char *argv[7])
{
    char *vetA, *vetB;
    int **mat;
    int i=0,j=0, res;
    char *vetResA;
    char *vetResB;
    int num_options = 0;

    if(argv[1] == NULL || argv[2] == NULL)
    {
        printf("\n\nUse: %s [File Path 1] [File Path 2]\n", argv[0]);
        printf("Options:\n");
        printf("--match | INT\n");
        printf("--mismatch | INT\n");
        printf("--gap | INT\n");
        printf("-verbose | 1=TRUE 0=FALSE\n\n");
        return 0;
    }

    Option* options = process_options(argc, argv, &num_options);

    MATCH = options[0].value;
    MISMATCH = options[1].value;
    GAP = options[2].value;
    VERBOSE = options[3].value;

    if(VERBOSE)
        for(i=0; i < num_options; i++)
        {
            printf("%s = %d\n", options[i].option, options[i].value);
        }

    printf("Reading Files...\n");
    SIZEA = ReadFastaData(&vetA, argv[1]);
    SIZEB = ReadFastaData(&vetB, argv[2]);
    SIZERES = SIZEA + SIZEB;

    vetResA = (char*) calloc(SIZERES, sizeof(char));
    vetResB = (char*) calloc(SIZERES, sizeof(char));

    if(VERBOSE)
    {
        PrintVector(vetA, SIZEA);
        PrintVector(vetB, SIZEB);

        PrintVector(vetResA, SIZERES);
        PrintVector(vetResB, SIZERES);
    }

    printf("Initialize Matrix...\n");
    mat = InitializeMatrix(SIZEA, SIZEB);
    printf("Calculate Matrix...\n");
    CalculateSimilarity(mat, vetA, vetB);
    if(VERBOSE)
        PrintMatrix(mat);
    
    printf("Mount Sequence...\n");
    MountSequence(mat, vetA, vetB, &vetResA, &vetResB);

    if(VERBOSE)
    {
        PrintVector(vetResA, SIZERES);
        PrintVector(vetResB, SIZERES);
    }

    printf("Calculate Results...\n");
    PrintResults(vetResA, vetResB, SIZERES);

    FreeMatrix(mat);
    free(vetA);
    free(vetB);
    free(vetResA);
    free(vetResB);
    return 0;
}