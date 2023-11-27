#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include "options.h"
#include <time.h>
#include <sys/time.h>

#define getName(var)  #var
int SIZEA, SIZEB, SIZERES, MATRIX_SIZE;
int VERBOSE = 0;
int MATCH = 2;
int MISMATCH = -1;
int GAP = -1;
int GAP_SEQ = -1;
int NO_AFFINE = 0;
int SEILA = 0;


int Similarity(char first, char second, int *gap_seq_a, int *gap_seq_b)
{
    int gap_seq_count;
    if(NO_AFFINE == 1 || gap_seq_a == NULL || gap_seq_b == NULL)
    {
        // Simple gap penalty
        if(first == '-' || second == '-')
        {
            return GAP;
        }
    }
    else // Aplies affine gap
    {
        if(first == '-')
        {
            *gap_seq_a = *gap_seq_a + 1;
            gap_seq_count = *gap_seq_a -1;
            return GAP + GAP_SEQ * gap_seq_count;
        }
        else
        {
            *gap_seq_a = 0;
        }

        if(second == '-')
        {
            *gap_seq_b = *gap_seq_b + 1;
            gap_seq_count = *gap_seq_b -1;
            return GAP + GAP_SEQ * gap_seq_count;
        }
        else
        {
            *gap_seq_b = 0;
        }
    }

    if(first == second)
        return MATCH;

    return MISMATCH;
}

int FunctionSimilarity(int** mat, char a, char b, int i, int j, char *pos, int *gap_seq_a, int *gap_seq_b)
{
    int v1=INT_MIN,v2=INT_MIN,v3=INT_MIN;
    int result = INT_MIN;

    if(i-1 >= 0 && j-1 >= 0)
        v1 = mat[i-1][j-1] + Similarity(a, b, gap_seq_a, gap_seq_b);
    if(i-1 >= 0)
        v2 = mat[i-1][j] + Similarity(a, '-', gap_seq_a, gap_seq_b);
    if(j-1 >= 0)
        v3 = mat[i][j-1] + Similarity('-', b, gap_seq_a, gap_seq_b);

    if(v2 > result)
    {
        result = v2;
        *pos = 'V';
    }
    if(v3 > result)
    {
        result = v3;
        *pos = 'H';
    }
    if(v1 > result)
    {
        result = v1;
        *pos = 'D';
    }

    if(result == INT_MIN && i > 0 || j > 0)
        printf("\nmat[%d][%d]:%d v1:%d v2:%d v3:%d\n", i,j, mat[i][j], v1, v2, v3);

    return result;
}

void PrintMatrix(int **mat)
{
    printf("<Print Matrix>\n");
    for (int i = 0; i < SIZEA; i++)
    {
        for (int j = 0; j < SIZEB; j++)
        {
            if(mat[i][j] >= 0)
                printf("%0*d | ", 3, mat[i][j]);
            else
                printf("%0*d | ", 3, mat[i][j]);
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

double CalculateSimilarity(int **mat, char *vetA, char *vetB)
{
    struct timeval start_time, end_time;
    double elapsed_time;
    int i,j, gap_seq_a = 0, gap_seq_b = 0;
    char *pos = (char*) calloc(1, sizeof(char));

    gettimeofday(&start_time, NULL);
    for (i=1; i < SIZEA; i++)
    {
        mat[i][0] = i * GAP;
    }

    for (i=1; i < SIZEB; i++)
    {
        mat[0][i] = i * GAP;
    }

    for (i=1; i < SIZEA; i++)
        for(j=1; j < SIZEB; j++)
        {
            mat[i][j] = FunctionSimilarity(mat, vetA[i], vetB[j], i, j, pos, &gap_seq_a, &gap_seq_b);
        }

    if(mat[SIZEA-1][SIZEB-1] == 0)
        printf("<<NOT COMPLETED>> %d\n", mat[SIZEA-1][SIZEB-1]);

    gettimeofday(&end_time, NULL);
    elapsed_time = (end_time.tv_sec - start_time.tv_sec) +
                   (end_time.tv_usec - start_time.tv_usec) / 1000000.0;

    return elapsed_time;
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
        FunctionSimilarity(mat, vetA[i], vetB[j], i, j, pos, NULL, NULL);
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
            *(*(vetResA) + k) = vetA[i];
            *(*(vetResB) + l) = '-';
            i--;
        }
        else if(*pos == 'H')
        {
            *(*(vetResA) + k) = '-';
            *(*(vetResB) + l)= vetB[j];
            j--;
        }
    } while (i >= 0 && j >= 0);

    free(pos);
    if(VERBOSE)
        printf("\n");
    return;
}

void MountInvertedSequence(int **mat, char *vetA, char *vetB, char *vetResA, char *vetResB)
{
    int i,j,k,l;
    char pos;

    for (i=0; i < SIZERES; i++)
    {
        vetResA[i] = '-';
        vetResB[i] = '-';
    }

    i = 1;
    j = 1;
    k = 1;
    l = 1;

    if(VERBOSE)
        printf("Matrix back path: ");

    while (i < SIZEA && j < SIZEB)
    {
        FunctionSimilarity(mat, vetA[i], vetB[j], i, j, &pos, NULL, NULL);

        if(VERBOSE)
            printf(" %c |", pos);

        if(pos == 'D')
        {
            vetResA[k] = vetA[i];
            vetResB[l] = vetB[j];
            i++;
            j++;
        }
        else if(pos == 'V')
        {
            vetResA[k]  = vetA[i];
            vetResB[l] = '-';
            i++;
        }
        else if(pos == 'H')
        {
            vetResA[k] = '-';
            vetResB[l] = vetB[j];
            j++;
        }

        k++;
        l++;
    }

    SIZERES = k+l;

    if(VERBOSE)
        printf("\n");
    return;
}

int PrintVector(char *vet, int size)
{
    int i, count=0;
    for (i=0; i < size; i++)
        if (vet[i] >= 'A' && vet[i] <= 'Z' || vet[i] == '-')
        {
            count++;
            printf("%c ", vet[i]);
        }
    printf("\n");
    return count;
}

read_data_result ReadFastaData(char **vet, char *path)
{
    FILE* file;
    char ch;
    int i, size = 1, skipLine = 0;
    read_data_result result;

    file = fopen(path, "r");
    if(file == NULL)
    {
        printf("\nCan't read the file!\n");
        return result;
    }

    for (i=0; i < 99; i++)
    {
        ch = fgetc(file);
        if (ch == EOF || ch == '\n')
            break;
        else
            result.sequence_name[i] = ch;
    }

    result.sequence_name[i+1] = '\0';
    printf("%s\n", result.sequence_name);

    fseek(file, 0, SEEK_SET);

    do {
        ch = fgetc(file);

        if(skipLine == 1 && ch == '\n')
            skipLine = 0;

        if(ch == '>')
            skipLine = 1;

        if((ch >= 'A' && ch <= 'Z' || ch >= 'a' && ch <= 'z') && skipLine == 0)
            size++;
        
    
    } while(ch != EOF);

    *vet = (char*) calloc(size, sizeof(char));

    fseek(file, 0, SEEK_SET);
    skipLine = 0;
    i=1;
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
    result.size = size;
    return result;
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

int WriteFile(char *vetA, char *vetB, char *metrics, double elapsed_time, read_data_result result_a, read_data_result result_b)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    FILE *file;
    int lineSize = 60, l=0, seq_poss = 1;
    char datetime[50], seqA[lineSize+3], seqB[lineSize+3], identities[lineSize+3], a,b;

    sprintf(datetime, "out/%02d%02d%02d%02d%02d%02d.txt",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec);

    file = fopen(datetime, "w");
    if (file == NULL) {
        printf("\nErro ao criar o arquivo.\n");
        return 0;
    }

    fprintf(file, "Reference sequence: %s\n", result_a.sequence_name);
    fprintf(file, "Subject sequence: %s\n", result_b.sequence_name);
    fprintf(file, "%s\n", metrics);
    fprintf(file, "Alignment time: %f seconds\n\n", elapsed_time);

    for (int i=0; i < SIZERES; i++)
    {
        if (l >= lineSize)
        {
            seqA[l] = '\0';
            seqB[l] = '\0';
            identities[l] = '\0';
            fprintf(file, "%d-%d\n", seq_poss-lineSize, seq_poss -1);
            fprintf(file, "%s\n", seqA);
            fprintf(file, "%s\n", identities);
            fprintf(file, "%s\n\n", seqB);
            l = 0;
        }
        if (l < lineSize)
        {
            a = vetA[i];
            b = vetB[i];

            if(a >= 'A' && a <= 'Z' && b >= 'A' && b <= 'Z' || a == '-' || b == '-')
            {
                if(a == b)
                {
                    identities[l] = '|';
                }
                else
                if(a == '-' || b == '-')
                {
                    identities[l] = '&';
                }
                else identities[l] = '*';

                seqA[l] = a;
                seqB[l] = b;
                l++;
                seq_poss++;
            }
        }

    }

    seqA[l] = '\0';
    seqB[l] = '\0';
    identities[l] = '\0';
    fprintf(file, "%s\n", seqA);
    fprintf(file, "%s\n", identities);
    fprintf(file, "%s\n", seqB);    

    fclose(file);
    return 1;
}

void WriteMatrixFile(int **mat)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    FILE *file;
    char datetime[50];

    sprintf(datetime, "out/Matrix_%02d%02d%02d%02d%02d%02d.txt",
        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
        tm.tm_hour, tm.tm_min, tm.tm_sec);

    file = fopen(datetime, "w");
    if (file == NULL) {
        printf("\nErro ao criar o arquivo.\n");
        return;
    }

    for (int i=0; i< 300; i++)
    {
        for (int j=0; j< 300; j++)
        {
            fprintf(file, "%0*d | ", 11, mat[i][j]);
        }
        fprintf(file, "\n");
    }
}

char* PrintResults(char *vetA, char *vetB, int size_sequence, int **mat)
{
    int i, hits=0, misses=0, gaps=0;
    char a, b, *ret;

    ret = (char*) calloc(100, sizeof(char));

    for(i=0; i < SIZERES; i++)
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

    sprintf(ret, "Score: %d Identities: %d Gaps: %d Misses: %d AlignmentSize: %d", mat[SIZEA-1][SIZEB-1], hits, gaps, misses, size_sequence);

    printf("==========Results==========\n");
    printf("Hits: %d \nMisses: %d \nGaps: %d \nAlignmentSize: %d\nScore: %d\n", hits, misses, gaps, size_sequence, mat[SIZEA-1][SIZEB-1]);
    printf("===========================\n");
    return ret;
}

void PrintHelp(char* prog_name)
{
    printf("\n\nUse: %s [File Path 1] [File Path 2]\n", prog_name);
    printf("Options:\n");
    printf("--match | INT\n");
    printf("--mismatch | INT\n");
    printf("--gap | INT\n");
    printf("--gap_seq | INT\n");
    printf("-help\n");
    printf("-verbose\n");
    printf("-no_affine\n\n");
}

int CountFinalSequence(char *vet, int size)
{
    int i, count=0;
    for (i=0; i < size; i++)
        if (vet[i] >= 'A' && vet[i] <= 'Z' || vet[i] == '-')
            count++;
        
    return count;
}

int main(int argc, char *argv[7])
{
    char *vetA, *vetB;
    int **mat;
    int i=0,j=0, res, alignment_size;
    char *vetResA;
    char *vetResB;
    int num_options = 0;
    double elapsed_time;
    read_data_result result_a;
    read_data_result result_b;

    if(argv[1] == NULL || argv[2] == NULL)
    {
        PrintHelp(argv[0]);
        return 0;
    }

    Option* options = process_options(argc, argv, &num_options);

    MATCH = options[0].value;
    MISMATCH = options[1].value;
    GAP = options[2].value;
    GAP_SEQ = options[3].value;
    VERBOSE = options[4].value;
    NO_AFFINE = options[5].value;

    if(options[6].value)
    {
        PrintHelp(argv[0]);
        return 0;
    }

    printf("<Reading Files>\n");
    printf("Reference Sequence: ");
    result_a = ReadFastaData(&vetA, argv[1]);
    printf("Subject Sequence: ");
    result_b = ReadFastaData(&vetB, argv[2]);
    SIZEA = result_a.size;
    SIZEB = result_b.size;
    SIZERES = SIZEA + SIZEB;
    MATRIX_SIZE = SIZEA * SIZEB;
    printf("Size A: %d Size B: %d\n", SIZEA-1, SIZEB-1);

    vetResA = (char*) calloc(SIZERES, sizeof(char));
    vetResB = (char*) calloc(SIZERES, sizeof(char));

    printf("<Initialize Matrix>\n");
    mat = InitializeMatrix(SIZEA, SIZEB);
    printf("<Calculate Matrix>\n");
    elapsed_time = CalculateSimilarity(mat, vetA, vetB);
    printf("Alignment time: %f seconds\n", elapsed_time);
    if(VERBOSE)
        PrintMatrix(mat);
    
    printf("<Mount Sequence>\n");
    //WriteMatrixFile(mat);
    SEILA = 1;
    MountSequence(mat, vetA, vetB, &vetResA, &vetResB);
    //MountInvertedSequence(mat, vetA, vetB, vetResA, vetResB);

    if(VERBOSE)
    {
        printf("Sequence A: ");
        alignment_size = PrintVector(vetResA, SIZERES);
        printf("Sequence B: ");
        PrintVector(vetResB, SIZERES);
    }

    printf("<Calculate Results>\n");
    alignment_size = CountFinalSequence(vetResA, SIZERES);
    char *result = PrintResults(vetResA, vetResB, alignment_size, mat);

    printf("<Writing file>\n");
    WriteFile(vetResA, vetResB, result, elapsed_time, result_a, result_b);
    printf("<Done>\n");

    FreeMatrix(mat);
    free(vetA);
    free(vetB);
    free(vetResA);
    free(vetResB);
    return 0;
}