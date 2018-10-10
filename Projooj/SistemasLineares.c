#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
/*Lista*/
typedef struct sNo
{
    void* info;
    struct sNo* prox;
} No;
typedef struct
{
    No* prim;
    char (*equals) (void*, void*);
} Lista;
/*Fim da Lista*/

char equalsStr (char* str1, char* str2);

double** mallocMatriz(int n)
{
    double** m = (double**)malloc(n * sizeof(double*));

    int i, j;
    for(i = 0; i < n; i++)
        m[i] = (double*)malloc((n + 1) * sizeof(double));

    for(i = 0; i < n; i++)
        for(j = 0; j < (n + 1); j++)
            m[i][j] = 0;

    return m;
}

void resetarVariaveis(char** a1, char** a2)
{
    *a1     = (char*)malloc(sizeof(char) * 255);
    *a1[0]  = '\0';

    *a2     = (char*)malloc(sizeof(char) * 255);
    *a2[0]  = '\0';
}

char* strFromFile(char* nomeArquivo, int *n)
{
    /*Lendo o arquivo*/
    FILE *f = fopen("sistema.txt"/*nomeArquivo*/, "r");
    char* ret = (char*)malloc(sizeof(char) * 255);
    char* aux = (char*)malloc(sizeof(char) * 255);
    ret[0] = '\0';

    fscanf(f, "%i", n);

    fgets(aux, 1024, f);

    while(fgets(aux, 1024, f) != NULL)
        strcat(ret, aux);
    /*Fim da leitura do arquivo*/

    free(aux);
    /*fclose(f);*/

    return ret;
}

double** fileParaMatriz(char* nomeArquivo, int *n/*o qrquivo contera n linhas com n variaveis*/, Lista* lis)
{
    int i;
    int j;
    char* str = strFromFile(nomeArquivo, n);
    printf("N: %i\n", *n);
    double **m = mallocMatriz(*n);

    printf("\nSistemas:\n---------------\n");
    printf("%s\n", str);
    printf("---------------\n");

    int auxSinal = 1;
    char* auxInt;
    char* auxStr;

    resetarVariaveis(&auxInt, &auxStr);

    int linha    = 0;

    for(i = 0;;i++)
    {
        double valor;
        if(str[i] == '-' || str[i] == '+' || str[i] == '=' || str[i] == '\n' || str[i] == '\0')
        {
            valor = (double)(((auxInt[0]=='\0')?1:atoi(auxInt)) * auxSinal);
            if(auxStr[0] != '\0')
                if(ondeEsta(lis, auxStr) < 0)
                    inserirFinal(lis, auxStr);

            //printf("%lf", (double)(((auxInt[0]=='\0')?1:atoi(auxInt)) * auxSinal));
            if(auxStr[0] == '\0')
            {
                if(str[i] == '\n' || str[i] == '\0')
                    m[linha][*n]                    += valor;
            }
            else
                m[linha][ondeEsta(lis, auxStr)] += valor;

            auxSinal  = 1;
            resetarVariaveis(&auxInt, &auxStr);
        }

        if(str[i] == '-' )
            auxSinal  = -1;

        if(str[i] == '\n' || str[i] == '\0')
            linha++;

        if(isalpha(str[i]))
        {
            int j = 0;
            for(;;j++)
                if(auxStr[j] == '\0')
            {
                auxStr[j] = str[i];
                auxStr[j + 1] = '\0';
                break;
            }
        }

        if(str[i] == '\0')
            break;


        if(isdigit(str[i]))
            for(j = 0;;j++)
                if(auxInt[j] == '\0')
                {
                    auxInt[j] = str[i];
                    auxInt[j + 1] = '\0';
                    break;
                }
    }

    /*Free everything*/
    free(auxStr);
    free(auxInt);
    free(str);

    return m;
}

int main()
{
    printf("Digite o nome do arquivo:\n");

    char* nomeArquivo = (char*)malloc(sizeof(char) * 255);

    gets(nomeArquivo);

    int n;

    /*Declaring list*/
    Lista* lis     = (Lista*)malloc(sizeof(Lista*));
    lis->prim      = NULL;
    lis->equals    = (char(*)(void*,void*))&equalsStr;
    /*End of List Declaration*/

    double** m = fileParaMatriz(nomeArquivo, &n, lis);

    /*printing everything*/
    printf("\nVariaveis:\n---------------\n");
    printLista(lis);
    printf("\n---------------\n");

    printf("\nMATRIZ:\n---------------\n");

    int i;
    int j;

    for(i = 0; i < n; i++)
    {
       for(j = 0; j < ((int)n + 1); j++)
            printf(((j==0)?"%lf;":"\t%lf;"),m[i][j]);

        printf("\n");
    }

    printf("---------------\n");
}

/* INSERIR */
void inserirComeco(Lista* lista, void* info)
{
    No* novoPrim = (No*)malloc(sizeof(No));
    (*novoPrim).info = info;
    (*novoPrim).prox = (*lista).prim;

    (*lista).prim = novoPrim;
}

void inserirFinal(Lista* lista, void* info)
{
    if ((*lista).prim == NULL)
    {
        inserirComeco(lista, info);
        return;
    }

    No* atual = (*lista).prim;
    while ((*atual).prox != NULL)
        atual = (*atual).prox;

    No* novo = (No*)malloc(sizeof(No));
    (*novo).info = info;
    (*novo).prox = NULL;

    (*atual).prox = novo;
}

void inserirEm(Lista* lista, void* info, int onde)
{
    if (onde == 0)
    {
        inserirComeco(lista, info);
        return;
    }

    No* atual = (*lista).prim;
    int i;
    for(i=1; i<onde; i++)
    {
        if (atual == NULL)
            return;
        atual = (*atual).prox;
    }

    /*deve ser adicionado o noh no atual.prox*/
    No* novo= (No*)malloc(sizeof(No));
    (*novo).info = info;
    (*novo).prox = (*atual).prox;

    (*atual).prox = novo;
}

/*REMOVER*/
void removerComeco(Lista* lista)
{
    if ((*lista).prim == NULL)
        return;

    No* aux = (*lista).prim;
    (*lista).prim = (*(*lista).prim).prox;
    free(aux);
}

void removerFinal(Lista* lista)
{
    if ((*lista).prim == NULL)
        return;

    No* anterior = NULL;
    No* atual = (*lista).prim;
    while ((*atual).prox != NULL)
    {
        anterior = atual;
        atual = (*atual).prox;
    }

    if (anterior == NULL) //soh tinha um elemento
        (*lista).prim = NULL;
    else
        (*anterior).prox = NULL;

    //libera o espaco do ultimo Noh
    free(atual);
}

void removerEm(Lista* lista, int onde)
{
    if (onde == 0)
    {
        removerComeco(lista);
        return;
    }

    No* atual = (*lista).prim;
    int i;
    for(i=1; i<onde; i++)
    {
        if (atual == NULL)
            return;
        atual = (*atual).prox;
    }

    /*atual.prox deve ser removido*/
    No* aux = (*atual).prox;
    if (aux == NULL)
        return;
    (*atual).prox = (*(*atual).prox).prox;

    //libera o espaco do noh removido
    free(aux);
}

/*OUTROS*/
int ondeEsta (Lista* lista, void* info)
{
    No* atual = (*lista).prim;

    int i = 0;
    for(; atual != NULL; i++, atual = (*atual).prox)
        if( (*(*lista).equals)((*atual).info, info) )
            return i;
    return -1;
}


void printLista(Lista* lista)
{
    No* atual = (*lista).prim;
    while(atual != NULL)
    {
        printf("%s -> ", (*atual).info);
        atual = (*atual).prox;
    }
    printf("null");
}

/**/
char equalsStr (char* str1, char* str2)
{
    return strcmp(str1, str2) == 0;
}
