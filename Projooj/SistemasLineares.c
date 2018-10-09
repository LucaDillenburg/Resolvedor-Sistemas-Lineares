#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/*void trocar();

/*void resolver();

/*void abrirArquivo();*/

typedef
    struct No
    {
        char*      info;
        struct No* prox;
    }
    No;

typedef
    struct Lista
    {
        No*  inicio;
        void (*print)     (No*);
        int  (*achar)     (No*, char*);
    }
    Lista;

void strPrint (No* head)
{
    No* current = head;
    while (current != NULL)
    {
        printf("%s\n", current->info);
        current = current->prox;
    }
}

int achaCarinha (No* head, char* c)
{
    No* current = head;
    int i = 0;
    for(; current != NULL; i++)
    {
        if(strcmp(current->info, c) == 0)
            return i;
        current = current->prox;
    }

    if(head == NULL)
        return -1;
    else
        return -i;
}

char/*boolean*/ insira (Lista* lis, char* inf)
{
    No* atual = lis->inicio;

    if(atual == NULL)
    {
        No* novo    = (No*)malloc(sizeof(No));
        novo->prox  = NULL;
        novo ->info = inf;
        lis->inicio = novo;
        return;
    }

    for(;;) /*parar ao achar onde inserir*/
    {
        if (atual->prox == NULL)
            break;

        atual = atual->prox;
    }

    No* novo    = (No*)malloc(sizeof(No));
    novo ->info = inf;
    novo ->prox = atual->prox;
    atual->prox = novo;
    novo->prox  = NULL;
}

int main()
{
    FILE *f = fopen("sistema.txt", "r");

    char* str = (char*)malloc(sizeof(char) * 255);
    char* aux = (char*)malloc(sizeof(char) * 255);
    str[0]='\0';

    int* n;/*o qrquivo contera n linhas com n variaveis*/

    fscanf(f, "%i", &n);

    printf("N:%i\n", n);

    int **m;
    m = (int**)malloc((int)n * sizeof(int*));
    int i;
    int j;
    for(i = 0; i < n; i++)
        m[i] = (int*)malloc((int)(n + 1) * sizeof(int));

    for(i = 0; i < n; i++)
        for(j = 0; j < ((int)n + 1); j++)
            m[i][j] = 0;

    fgets(aux, 1024, f);

    /*Declaring list*/
    Lista lis;
    lis.inicio    = NULL;
    lis.print     = (No*)               &strPrint;
    lis.achar     = (int (*) (No*, char*)) &achaCarinha;
    /*End of List Declaration*/

    while(fgets(aux, 1024, f) != NULL)
        strcat(str, aux);

    free(aux);
    /*fclose(f);*/
    printf("\nSistemas:\n---------------\n");
    printf("%s\n", str);
    printf("---------------\n");

    int auxSinal = 1;
    int linha    = 0;

    char* auxInt = (char*)malloc(sizeof(char) * 255);
    auxInt[0] = '\0';

    char* auxStr = (char*)malloc(sizeof(char) * 255);
    auxStr[0] = '\0';

    for(i = 0;;i++)
    {
        if(str[i] == '-')
        {
            if(auxStr[0] != '\0')
                if(lis.achar(lis.inicio, auxStr) < 0)
                {
                    insira(&lis, auxStr);
                }

            if(auxStr[0] == '\0')
                m[linha][((int)n + 0)]                  += ((auxInt[0]=='\0')?1:atoi(auxInt)) * auxSinal;
            else
                m[linha][lis.achar(lis.inicio, auxStr)] += ((auxInt[0]=='\0')?1:atoi(auxInt)) * auxSinal;

            auxSinal  = -1;
            auxInt    = (char*)malloc(sizeof(char) * 255);
            auxInt[0] = '\0';
            auxStr    = (char*)malloc(sizeof(char) * 255);
            auxStr[0] = '\0';
        }

        if(str[i] == '+' || str[i] == '=')
        {
            if(auxStr[0] != '\0')
                if(lis.achar(lis.inicio, auxStr) < 0)
                {
                    insira(&lis, auxStr);
                }

            if(auxStr[0] == '\0')
                m[linha][((int)n + 0)]                  += ((auxInt[0]=='\0')?1:atoi(auxInt)) * auxSinal;
            else
                m[linha][lis.achar(lis.inicio, auxStr)] += ((auxInt[0]=='\0')?1:atoi(auxInt)) * auxSinal;

            auxSinal  = 1;
            auxInt    = (char*)malloc(sizeof(char) * 255);
            auxInt[0] = '\0';;
            auxStr    = (char*)malloc(sizeof(char) * 255);
            auxStr[0] = '\0';
        }

        if(str[i] == '\n' || str[i] == '\0')
        {
            if(auxStr[0] != '\0')
                if(lis.achar(lis.inicio, auxStr) < 0)
                {
                    insira(&lis, auxStr);
                }

            if(auxStr[0] == '\0')
                m[linha][((int)n + 0)]                  += ((auxInt[0]=='\0')?1:atoi(auxInt)) * auxSinal;
            else
                m[linha][lis.achar(lis.inicio, auxStr)] += ((auxInt[0]=='\0')?1:atoi(auxInt)) * auxSinal;

            auxSinal  = 1;
            auxInt    = (char*)malloc(sizeof(char) * 255);
            auxInt[0] = '\0';
            auxStr    = (char*)malloc(sizeof(char) * 255);
            auxStr[0] = '\0';

            linha++;
        }

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
    /*printing everything*/
    printf("\nVariaveis:\n---------------\n");
    lis.print(lis.inicio);
    printf("---------------\n");

    printf("\nMATRIZ:\n---------------\n");

    for(i = 0; i < n; i++)
    {
       for(j = 0; j < ((int)n + 1); j++)
            printf(((j==0)?"%i;":"\t%i;"),m[i][j]);

        printf("\n");
    }

    printf("---------------\n");

    /*Free everything*/
    free(i);
    free(j);
    free(linha);
    free(auxSinal);
    free(auxStr);
    free(auxInt);
    free(str);

    for(i = 0; i < n; i++)
    {
        for(j = 0; j < ((int)n + 1); j++)
            free(m[i][j]);

        free(m[i]);
    }
    free(m);
}
