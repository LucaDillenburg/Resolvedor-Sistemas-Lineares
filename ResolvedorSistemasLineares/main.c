#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <math.h>

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
void* getElemento(Lista* lista, int n);
void printLista(Lista* lista);
char equalsStr (char* str1, char* str2);
//file -> matriz
double** fileParaMatriz(char* nomeArquivo, int* ordem, Lista* incognitas, int* erro);
char* strFromFile(char* nomeArquivo, int* ordem);
double** mallocMatriz(int ordem);
void resetarVariaveis(char** str1, char** str2);
//matriz -> solucao
double* resolverSistema(double** matriz, int ordem, Lista* incognitas);
char fazerEscalonamento(double** matriz, int ordem, Lista* incognitas);
void zerarColunaDeOutrasLinhas(double** matriz, int indexColLinha, int ordem);
int procurarLinhaSemZeroEmColuna(double** matriz, int linhaInicial, int coluna, int ordem);
void trocarLinhas(double** matriz, int nLinha1, int nLinha2);
void multLinha(double** matriz, int indexLinha, double multiplicador, int ordem);
void multLinhaSomarOutra(double** matriz, int indexLinhaMultiplicada, double mult, int indexLinhaReceber, int ordem);
void printarSistema(double** matriz, Lista* incognitas, int ordem);

void printarMatriz(double** matriz, int linhas, int colunas);

int main()
{
    printf("Escreva o nome do arquivo: ");
    char* nomeArq = (char*)malloc(sizeof(char)*100);
    scanf("%s", nomeArq);

    printf("\n");

    int ordem;
    Lista* incognitas = (Lista*)malloc(sizeof(Lista));
    (*incognitas).prim = NULL;
    (*incognitas).equals = (char(*)(void*,void*))&equalsStr;
    int erro = 255;
    double** matriz = fileParaMatriz(nomeArq, &ordem, incognitas, &erro);

    if (matriz == NULL)
    {
        if (erro == 2)
        {
            printf("O numero de variaveis no arquivo nao corresponde ao numero de variaveis escrito no decorrer do arquivo!");
            return -2;
        }else
        {
            printf("O arquivo nao existe!");
            return -3;
        }
    }

    char resposta = 'n';
    do
    {
        //perguntar se usuario qr ver todos os passos do escalonamento
        printf("Voce gostaria de ver o passo a passo da resolucao do sistema? S/N\n");
        scanf(" %c", &resposta);
    }while(resposta != 'S' && resposta != 'N' && resposta != 's' && resposta != 'n');

    char querVerPassoAPasso = resposta == 'S' || resposta == 's';
    double* solucao = resolverSistema(matriz, ordem, querVerPassoAPasso?incognitas:NULL);

    if (solucao == NULL)
    {
        printf("O sistema eh SPI (Sistema Possivel Indeterminado) ou SI (Sistema Impossivel)!");
        return -1;
    }

    //printar solucao
    printf("\nSolucao:\n");
    int i;
    for (i = 0; i<ordem; i++)
        printf("    %s = %lf\n", (char*)getElemento(incognitas, i), *(solucao+i));

    return 0;
}


    /*FILE PARA MATRIZ*/
double** fileParaMatriz(char* nomeArquivo, int* ordem, Lista* incognitas, int* erro)
{
    char* strArq = strFromFile(nomeArquivo, ordem);
    if (strArq == NULL)
    {
        *erro = 1; //se arquivo nao existe
        return NULL;
    }

    double** matriz = mallocMatriz(*ordem);

    printf("SISTEMA ESCRITO NO ARQUIVO:\n", *ordem);
    printf("%s\n\n", strArq);

    int auxSinal = 1;
    char* auxInt;
    char* auxStr;
    resetarVariaveis(&auxInt, &auxStr);

    int qtdIncognitas = 0;

    int linha = 0;
    int i;

    char ehVariavel = 0;

    for(i = 0; ; i++)
    {
        double valor;
        if(*(strArq+i) == '-' || *(strArq+i) == '+' || *(strArq+i) == '=' || *(strArq+i) == '\n' || *(strArq+i) == '\0')
        {
            valor = (double)(((*auxInt=='\0')?1:atoi(auxInt))*auxSinal);
            if(*auxStr != '\0')
                if(ondeEsta(incognitas, auxStr) < 0)
                {
                    //verifica se nao ha incognitas a mais do que deveria
                    if (qtdIncognitas >= *ordem)
                    {
                        *erro = 2; //se o numero de variaveis no arquivo nao corresponder ao numero de variaveis escrito no decorrer do arquivo
                        return NULL;
                    }
                    inserirFinal(incognitas, auxStr);
                    qtdIncognitas++;
                }

            if(*auxStr == '\0')
            {
                if(*(strArq+i) == '\n' || *(strArq+i) == '\0')
                    *(*(matriz+linha)+*ordem) += valor;
            }
            else
                *(*(matriz+linha)+ondeEsta(incognitas, auxStr)) += valor;

            auxSinal  = 1;
            ehVariavel = 0;
            resetarVariaveis(&auxInt, &auxStr);
        }

        if(*(strArq+i) == '-' )
            auxSinal  = -1;

        if(*(strArq+i) == '\n')
        {
            linha++;
            //verifica se nao ha mais equacoes do que variaveis: considera as [ordem] primeiras apenas
            if (linha >= *ordem)
                break;
        }

        if(*(strArq+i) == '\0')
            break;
        if(isalpha(*(strArq+i)) || (ehVariavel && ((*(strArq+i)) != ' ')))
        {
            int j;
            ehVariavel = 1;
            for(j = 0;;j++)
                if(*(auxStr+j) == '\0')
                {
                    *(auxStr+j) = *(strArq+i);
                    *(auxStr+(j + 1)) = '\0';
                    break;
                }
        }else
        if(isdigit(*(strArq+i)))
        {
            int j;
            for(j = 0;;j++)
                if(*(auxInt+j) == '\0')
                {
                    *(auxInt+(j + 1)) = '\0';

                    *(auxInt+j) = *(strArq+i);
                    break;
                }
        }
    }

    /*Free tudo que deu malloc*/
    free(auxStr);
    free(auxInt);
    free(strArq);

    //verifica se nao ha incognitas a menor do que deveria
    if (qtdIncognitas != *ordem)
    {
        *erro = 2; //se o numero de variaveis no arquivo nao corresponder ao numero de variaveis escrito no decorrer do arquivo
        return NULL;
    }
    return matriz;
}

char* strFromFile(char* nomeArquivo, int* ordem)
{
    /*Lendo o arquivo*/
    FILE* file = fopen(nomeArquivo, "r");
    if(file == NULL)
        return NULL;

    char* auxRet = (char*)malloc(sizeof(char) * 7500);
    char* auxConcat = (char*)malloc(sizeof(char) * 1024);
    *auxRet = '\0';

    fscanf(file, "%i", ordem);
    fgets(auxConcat, 1024, file);
    while(fgets(auxConcat, 1024, file) != NULL)
        strcat(auxRet, auxConcat);

    //criar char* com strlen(aux) + 1
    char* ret = (char*)malloc(sizeof(char)*(strlen(auxRet) + 1));
    strcpy(ret, auxRet);

    /*Fim da leitura do arquivo*/
    free(auxConcat);
    free(auxRet);
    fclose(file);

    return ret;
}

double** mallocMatriz(int ordem)
{
    //instanciacao
    double** matriz = (double**)malloc(ordem * sizeof(double*));
    int i;
    for(i = 0; i < ordem; i++)
        *(matriz+i) = (double*)malloc((ordem + 1) * sizeof(double));

    int j;
    //deixar matriz inteira de zeros
    for(i = 0; i < ordem; i++)
        for(j = 0; j < (ordem + 1); j++)
            *(*(matriz+i)+j) = 0;

    return matriz;
}

void resetarVariaveis(char** str1, char** str2)
{
    *str1 = (char*)malloc(sizeof(char) * 255);
    **str1  = '\0';
    *str2 = (char*)malloc(sizeof(char) * 255);
    **str2  = '\0';
}


    /*RESOLVER SISTEMA*/
double* resolverSistema(double** matriz, int ordem, Lista* incognitas)
{
    //fazer escalonamento
    char res = fazerEscalonamento(matriz, ordem, incognitas);
    if (!res)
        return NULL;

    //colocar valores em int*, char***
    double* solucao = (double*)malloc(sizeof(double)*ordem);
    int i;
    for(i=0; i<ordem; i++)
        *(solucao+i) = *(*(matriz+i)+ordem);

    return solucao;
}

char fazerEscalonamento(double** matriz, int ordem, Lista* incognitas)
{
    if (incognitas != NULL)
    {
        printf("\n\n\nSomando todas as variaveis que estavam duplicadas o sistema fica da seguinte forma:\n");
        printarSistema(matriz, incognitas, ordem);

        printf("\nO metodo que vamos usar eh o do escalonamento. Teremos que buscar deixar a diagonal principal do sistema sendo um e o resto igual a zero.");
        printf("Para isso poderemos multiplicar as linhas por um certo valor, trocar linhas de posicao ou multiplicar com uma linha e somar com outra.\n\n");
    }

    int i;
    int passo = 1;
    for (i = 0; i<ordem; i++)
    {
        double elem = *(*(matriz+i)+i);
        if (elem == 0)
        {
            if (incognitas != NULL)
            {
                printf("\n\n\n  -> Passo %i (linha atual: %i):\n", passo, i+1);
                printf(" - Como o elemento dessa equacao da diagonal principal do sistema eh zero, ");
                printf("teremos que achar outra linha abaixo dessa que tenha um numero diferente de zero nessa coluna (coluna %i).\n", i+1);
            }

            int nLinha = procurarLinhaSemZeroEmColuna(matriz, i + 1, i, ordem);

            if (incognitas != NULL)
            {
                if (nLinha >= 0)
                {
                    printf(" - Na linha %i, o elemento da coluna %i eh diferente de zero! ");
                    printf("Entao vamos trocar essa linha com a linha em que o elemento da diagonal principal era nulo!\n\n", nLinha+1, i+1);
                }
                else
                    printf(" - Nao ha nenhuma outra linha com o elemento da coluna %i diferente de zero!\n\n");
            }

            if (nLinha < 0)
                return 0; //erro: sistema invalido (nao eh possivel resolver sistema)
            trocarLinhas(matriz, i, nLinha);

            if (incognitas != NULL)
            {
                printf("Apos trocar as linhas %i e %i do sistema, ele ficou da seguinte forma:\n", i+1, nLinha+1);
                printarSistema(matriz, incognitas, ordem);
            }

            elem = *(*(matriz+i)+i);

            passo++;
        }

        double multiplicador = 1/elem;
        multLinha(matriz, i, multiplicador, ordem);
        if (incognitas != NULL)
        {
            printf("\n\n\n  -> Passo %i (linha atual: %i):\n", passo, i+1);
            printf(" - Depois de deixar o elemento da diagonal principal da linha atual diferente de zero, ");
            printf("deve-se multiplicar a linha por um numero para que o elemento torne-se um.\n");
            printf(" - Nesse caso o multiplicador eh %.2lf pois 1/%.2lf = %.2lf\n\n", multiplicador, elem, multiplicador);

            printf("Apos multiplicar a fileira inteira por %.2lf, o sistema ficou da seguinte maneira:\n");
            printarSistema(matriz, incognitas, ordem);
        }
        passo++;

        //*(*(matriz+i)+i) = 1; //colocar como sendo 1 (pode ficar aproximadamente isso)

        zerarColunaDeOutrasLinhas(matriz, i, ordem);
        if (incognitas != NULL)
        {
            printf("\n\n\n  -> Passo %i (linha atual: %i):\n", passo, i+1);
            printf(" - Entao devemos fazer um procedimento com todas as outras linhas: multiplicar essa equacao por um numero e somar a mesma com a outra.\n");
            printf(" - Os multiplicadores da linha atual para cada outra linha deve ser escolhido de forma a garantir que a linha que vai ser somada fique ");
            printf("com zero no elemento da coluna %i.\n\n", i+1);

            printf("Apos fazer esse processo com todas as linhas, o sistema ficou assim:\n");
            printarSistema(matriz, incognitas, ordem);
        }
        passo++;
    }

    if (incognitas != NULL)
    {
        printf("\n\n\nApos o procedimento feito na ultima linha, o sistema jah estah completamente escalonado. ");
        printf("\nBasta atribuir o termo independente a variavel restante em cada linha e essa serah a solucao!\n");
    }

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

void printarSistema(double** matriz, Lista* incognitas, int ordem)
{
    int i, j;
    for(i = 0; i<ordem; i++)
    {
        for (j=0; j<ordem; j++)
        {
            if (*(*(matriz+i)+j) >= 0)
            {
                if (j!=0)
                    printf(" + ");
            }
            else
            {
                if (j!=0)
                    printf(" - ");
                else
                    printf("-");
            }
            printf("%.2lf%s", fabs(*(*(matriz+i)+j)), getElemento(incognitas, j));
        }
        printf(" = %.2lf\n", *(*(matriz+i)+ordem));
    }
}


void printarMatriz(double** matriz, int linhas, int colunas)
{
    int i;
    int j;
    for(i=0; i<linhas; i++)
    {
        for(j=0; j<colunas; j++)
            printf("%lf  ", *(*(matriz+i)+j));
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

void* getElemento(Lista* lista, int n)
{
    No* atual = (*lista).prim;
    int i;
    for (i=0; atual != NULL; i++, atual = (*atual).prox)
        if (i == n)
            return (*atual).info;
    return NULL;
}

void printListaStr(Lista* lista)
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
