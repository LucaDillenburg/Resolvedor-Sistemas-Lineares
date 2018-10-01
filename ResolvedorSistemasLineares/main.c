#include <stdio.h>
#include <stdlib.h>

double* resolverSistema(char* nomeArq, char*** incongnitas, int* ordem);

char fazerEscalonamento(double** matriz, int ordem);

void zerarColunaDeOutrasLinhas(double** matriz, int indexColLinha, int ordem);

int procurarLinhaSemZeroEmColuna(double** matriz, int linhaInicial, int coluna, int ordem);
void trocarLinhas(double** matriz, int nLinha1, int nLinha2);
void multLinha(double** matriz, int indexLinha, double multiplicador, int ordem);
void multLinhaSomarOutra(double** matriz, int indexLinhaMultiplicada, double mult, int indexLinhaReceber, int ordem);

void printarMatriz(double** matriz, int linhas, int colunas);

int main()
{
    double** matriz = (double**)malloc(sizeof(double*)*3);
    int i;
    for (i=0; i<3; i++)
        matriz[i] = (double*)malloc(sizeof(double)*4);

    matriz[0][0] = 3;
    matriz[0][1] = 0;
    matriz[0][2] = 1;
    matriz[0][3] = -10;

    matriz[1][0] = 2;
    matriz[1][1] = 7;
    matriz[1][2] = 8;
    matriz[1][3] = 15;

    matriz[2][0] = 0;
    matriz[2][1] = 1;
    matriz[2][2] = 4;
    matriz[2][3] = 10;

    printarMatriz(matriz, 3, 4);
    printf("\n\n");

    char res = fazerEscalonamento(matriz, 3);
    if (!res)
        return NULL;
    //colocar valores em int*, char***
    double* solucao = (int*)malloc(sizeof(int)*3);
    for(i=0; i<3; i++)
        *(solucao+i) = *(*(matriz+i)+3);

    for(i=0; i<3; i++)
        printf("%lf\n", solucao[i]);

    return 0;
}

double* resolverSistema(char* nomeArq, char*** incongnitas, int* ordem)
{
    double** matriz;
    //colocar elementos na matriz e mudar *ordem
    //colocar incognitas em char***

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

