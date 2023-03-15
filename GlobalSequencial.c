#include <stdio.h>
#include <stdlib.h>

#define VETSIZE 8

int Similarity(char first, char second)
{
    if(first == second)
        return 2;
    return -1;
}

int FunctionSimilarity(int** mat, char a, char b, int i, int j, char *pos)
{
    int v1=0,v2=0,v3=0;
    int result = 0;

    v1 = mat[i-1][j-1] + Similarity(a,b);
    v2 = mat[i-1][j] + Similarity(a, ' ');
    v3 = mat[i][j-1] + Similarity(' ', b);

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
    for (int i = 0; i < VETSIZE; i++)
    {
        for (int j = 0; j < VETSIZE; j++)
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

int** InitializeMatrix()
{
    int** mat = (int**) calloc(VETSIZE, sizeof(int*));
    for (int i = 0; i < VETSIZE; i++)
        mat[i] = (int*) calloc(VETSIZE, sizeof(int));
    
    return mat;
}

void FreeMatrix(int** mat)
{
    for (int i = 0; i < VETSIZE; i++)
        free(mat[i]);
    free(mat);
}

void CalculateSimilarity(int **mat, char *vetA, char *vetB)
{
    int i,j;
    char *pos = (char*) calloc(1, sizeof(char));

    for (i=1; i < VETSIZE; i++)
    {
        mat[i][0] = mat[i-1][0] + Similarity(vetA[i], '-');
        mat[0][i] = mat[0][i-1] + Similarity('-', vetB[i]);
    }

    for (i=1; i < VETSIZE; i++)
        for(j=1; j < VETSIZE; j++)
        {
            mat[i][j] = FunctionSimilarity(mat, vetA[i], vetB[j], i, j, pos);
        }

    free(pos);
    return;
}

void MountSequence(int **mat, char *vetA, char *vetB, char *vetResA, char *vetResB)
{
    int i,j,k,l;
    char *pos = (char*) calloc(1, sizeof(char));

    for (i=0; i < VETSIZE*2; i++)
    {
        vetResA[i] = ' ';
        vetResB[i] = ' ';
    }

    i = VETSIZE-1;
    j = VETSIZE-1;

    k=VETSIZE*2-1;
    l=VETSIZE*2-1;

    while (i > 0 && j > 0)
    {
        FunctionSimilarity(mat, vetA[i], vetB[j], i, j, pos);
        printf(" %c |", *pos);

        if(*pos == 'D')
        {
            vetResA[k] = vetA[i];
            vetResB[l] = vetB[j];
            i--;
            j--;
        }
        else if(*pos == 'V')
        {
            vetResA[k] = vetA[i];
            vetResB[l] = '-';
            i--;
        }
        else
        {
            vetResA[k] = '-';
            vetResB[l] = vetB[j];
            j--;
        }
        k--;
        l--;
    }
    free(pos);
    printf("\n");
    return;
}

void PrintVector(char *vet)
{
    int i;
    for (i=0; i < VETSIZE*2; i++)
        printf("%c ", vet[i]);
    printf("\n");
}

int main()
{
    char vetA[VETSIZE] = {'-','A','C','A','A','T','C','C'};
    char vetB[VETSIZE] = {'-','A','G','C','A','T','G','C'};
    int **mat;
    int i=0,j=0;
    char vetResA[VETSIZE*2];
    char vetResB[VETSIZE*2];

    printf("<<<Alinhamentador>>>\n");

    mat = InitializeMatrix();
    CalculateSimilarity(mat, vetA, vetB);
    PrintMatrix(mat);
    MountSequence(mat, vetA, vetB, vetResA, vetResB);

    PrintVector(vetResA);
    PrintVector(vetResB);

    FreeMatrix(mat);
    return 0;
}