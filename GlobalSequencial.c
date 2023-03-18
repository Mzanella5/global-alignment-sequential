#include <stdio.h>
#include <stdlib.h>

int SIZEA, SIZEB, SIZERES;

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

    while (i > 0 && j > 0)
    {
        k--;
        l--;
        FunctionSimilarity(mat, vetA[i], vetB[j], i, j, pos);
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
    }
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

void ReadData(char **vetA, char **vetB)
{
    FILE* file;
    char ch;
    int next = 1, i, j;
    SIZEA = 1;
    SIZEB = 1;

    file = fopen("/tmp/test.txt", "r");
    if(file == NULL)
    {
        printf("\nCan't read the file!\n");
        return;
    }

    do {
        ch = fgetc(file);
        if(ch == '\n')
        {
            next = 0;
        }
        if(next == 1)
        {
            if(ch >= 'A' && ch <= 'Z')
                SIZEA++;
        }
        else
        {
            if(ch >= 'A' && ch <= 'Z')
                SIZEB++;   
        }
    
    } while(ch != EOF);

    printf("%d\n", SIZEA);
    printf("%d\n", SIZEB);

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
            if(ch >= 'A' && ch <= 'Z')
            {
                *((*vetA) + i) = ch;
                i++;
            }
        }
        else
        {
            if(ch >= 'A' && ch <= 'Z') 
            {
                *((*vetB) + j) = ch;
                j++;
            }
        }
    } while (ch != EOF);
    
    fclose(file);
    return;
}

int main()
{
    char *vetA, *vetB;
    int **mat;
    int i=0,j=0, res;
    char *vetResA;
    char *vetResB;

    printf("<<<Alinhamentador>>>\n");
    ReadData(&vetA, &vetB);
    SIZERES = SIZEA + SIZEB;

    vetResA = (char*) calloc(SIZERES, sizeof(char));
    vetResB = (char*) calloc(SIZERES, sizeof(char));

    PrintVector(vetA, SIZEA);
    PrintVector(vetB, SIZEB);

    PrintVector(vetResA, SIZERES);
    PrintVector(vetResB, SIZERES);

    mat = InitializeMatrix(SIZEA, SIZEB);
    CalculateSimilarity(mat, vetA, vetB);
    PrintMatrix(mat);
    MountSequence(mat, vetA, vetB, &vetResA, &vetResB);

    PrintVector(vetResA, SIZERES);
    PrintVector(vetResB, SIZERES);

    FreeMatrix(mat);
    free(vetA);
    free(vetB);
    free(vetResA);
    free(vetResB);
    return 0;
}