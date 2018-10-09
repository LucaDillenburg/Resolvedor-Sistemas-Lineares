#include <stdio.h>
#include <stdlib.h>

//matriz -> solucao
double* resolverSistema(double** matriz, int* ordem);
char fazerEscalonamento(double** matriz, int ordem);
void zerarColunaDeOutrasLinhas(double** matriz, int indexColLinha, int ordem);
int procurarLinhaSemZeroEmColuna(double** matriz, int linhaInicial, int coluna, int ordem);
void trocarLinhas(double** matriz, int nLinha1, int nLinha2);
void multLinha(double** matriz, int indexLinha, double multiplicador, int ordem);
void multLinhaSomarOutra(double** matriz, int indexLinhaMultiplicada, double mult, int indexLinhaReceber, int ordem);
void printarMatriz(double** matriz, int linhas, int colunas);
//lista
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
void inserirComeco(Lista* lista, void* info);
void inserirFinal(Lista* lista, void* info);
void inserirEm(Lista* lista, void* info, int onde);
void removerComeco(Lista* lista);
void removerFinal(Lista* lista);
void removerEm(Lista* lista, int onde);
int ondeEsta (Lista* lista, void* info);
void printLista(Lista* lista);
char equalsStr (char** str1, char** str2);

int main()
{
    printf("Escreva o nome do arquivo: ");
    char* nomeArq = (char*)malloc(sizeof(char)*100);
    scanf("%s", nomeArq);

    int ordem;
    Lista* incognitas;
    double** matriz = matrizFromArquivo(nomeArq, &ordem, incognitas);

    double* solucao = resolverSistema(matriz, ordem);

    if (solucao == NULL)
    {
        printf("O sistema não pode ser resolvido!");
        return -1;
    }

    return 0;
}

double* resolverSistema(double** matriz, int* ordem)
{
    //fazer escalonamento
    char res = fazerEscalonamento(matriz, *ordem);
    if (!res)
        return NULL;

    //colocar valores em int*, char***
    double* solucao = (double*)malloc(sizeof(double)* *ordem);
    int i;
    for(i=0; i<*ordem; i++)
        *(solucao+i) = *(*(matriz+i)+*ordem);

    return solucao;
}

char fazerEscalonamento(double** matriz, int ordem)
{
    int i;
    for (i = 0; i<ordem; i++)
    {
        double elem = *(*(matriz+i)+i);
        if (elem == 0)
        {
            int nLinha = procurarLinhaSemZeroEmColuna(matriz, i + 1, i, ordem);
            if (nLinha < 0)
                return 0; //erro de execucao (nao eh possivel resolver sistema)
            trocarLinhas(matriz, i, nLinha);

            elem = *(*(matriz+i)+i);
        }

        multLinha(matriz, i, 1/elem, ordem);
        *(*(matriz+i)+i) = 1; //colocar como sendo 1 (pode ficar aproximadamente isso)

        zerarColunaDeOutrasLinhas(matriz, i, ordem);
    }

    printarMatriz(matriz, ordem, ordem+1);
    printf("\n\n");

    return 1; //execucao normal (deu certo)
}

void zerarColunaDeOutrasLinhas(double** matriz, int indexColLinha, int ordem)
{
    int i;
    for (i=0; i<ordem; i++)
        if (i != indexColLinha)
        {
            double mult = - *(*(matriz+i)+indexColLinha);

            //multiplicar linha indexColLinha por mult e somar com a linha i
            multLinhaSomarOutra(matriz, indexColLinha, mult, i, ordem);

            *(*(matriz+i)+indexColLinha) = 0; //colocar como sendo 0 (pode ficar aproximadamente isso)
        }
    //aqui todas as linhas estao com a coluna indexColLinha = 0 (exceto a indexColLinha)
}

void multLinhaSomarOutra(double** matriz, int indexLinhaMultiplicada, double mult, int indexLinhaReceber, int ordem)
{
    int i;
    for(i=0; i<=ordem; i++)
    //percorre todos os elementos da linha indexLinhaReceber somando com
    //a multiplicacao do elemento correspondende da linha indexLinhaMultiplicada por mult
        *(*(matriz+indexLinhaReceber)+i) += mult * *(*(matriz+indexLinhaMultiplicada)+i);
}

void multLinha(double** matriz, int indexLinha, double multiplicador, int ordem)
{
    int i;
    for(i=0; i<=ordem; i++)
    //percorre todos os elementos da linha multiplicando por multiplicador
        *(*(matriz+indexLinha)+i) *= multiplicador;
}

int procurarLinhaSemZeroEmColuna(double** matriz, int linhaInicial, int coluna, int ordem)
{
    int i;
    for (i=linhaInicial; i<ordem; i++)
    //percorre toda linha procurando um zero numa posicao especifica daquela linha
        if ( *(*(matriz+i)+coluna) != 0)
            return i;

    return -1; //nao ha linhas sem coluna
}

void trocarLinhas(double** matriz, int nLinha1, int nLinha2)
{
    double* aux = *(matriz+nLinha1);
    *(matriz+nLinha1) = *(matriz+nLinha2);
    *(matriz+nLinha2) = aux;
}

void printarMatriz(double** matriz, int linhas, int colunas)
{
    int i;
    int j;
    for(i=0; i<linhas; i++)
    {
        for(j=0; j<colunas; j++)
            printf("%lf  ", matriz[i][j]);
        printf("\n");
    }
}




    /*LISTA LIGADA*/

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
char equalsStr (char** str1, char** str2)
{
    return strcmp(*str1, *str2) == 0;
}
