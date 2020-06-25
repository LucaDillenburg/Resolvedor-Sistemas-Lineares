#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <math.h>

#include "list.c"

//file -> matrix
double **file_to_matrix(char *, int *, List *, int *);
char *str_from_file(char *, int *);
double **malloc_matrix(int);
void reset_variables(char **, char **);

//matrix -> solution
double *solve_system(double **, int, List *);
char gaussian_elimination(double **, int, List *);
void empty_column_other_rows(double **, int, int);
int search_row_no_zero_in_column(double **, int, int, int);
void switch_rows(double **, int, int);
void multiply_row(double **, int, double, int);
void multiply_row1_and_sum_row2(double **, int, double, int, int);
void print_system(double **, List *, int);

void printarMatriz(double **, int, int);

int main()
{
	printf("Write file name: ");
	char *fileName = (char *)malloc(sizeof(char) * 100);
	scanf("%s", fileName);

	printf("\n");

	int amnt_variables;
	List *variables = (List *)malloc(sizeof(List));
	(*variables).front = NULL;
	(*variables).equals = (char (*)(void *, void *)) & equals_str;
	int error = 255;
	double **matrix = file_to_matrix(fileName, &amnt_variables, variables, &error);

	if (matrix == NULL)
	{
		if (error == 2)
		{
			printf("The amount of variables in the header of the file is different from the amount of variables in the system of equations!");
			return -2;
		}
		else
		{
			printf("The file does not exist!");
			return -3;
		}
	}

	char answer = 'n';
	do
	{
		printf("Do you want to see the step by step solution? Y/N\n");
		scanf(" %c", &answer);
	} while (answer != 'Y' && answer != 'N' && answer != 'y' && answer != 'n');

	char is_step_by_step = answer == 'Y' || answer == 'y';
	double *solution = solve_system(matrix, amnt_variables, is_step_by_step ? variables : NULL);

	if (solution == NULL)
	{
		printf("The system has infinitely many solutions or no solution!");
		return -1;
	}

	printf("\nSolution:\n");
	int i;
	for (i = 0; i < amnt_variables; i++)
		printf("    %s = %lf\n", (char *)element_in(variables, i), *(solution + i));

	free_list(variables);

	return 0;
}

double **file_to_matrix(char *nomeArquivo, int *amnt_variables, List *variables, int *error)
{
	char *file_str = str_from_file(nomeArquivo, amnt_variables);
	if (file_str == NULL)
	{
		*error = 1; //file does not exist
		return NULL;
	}

	double **matrix = malloc_matrix(*amnt_variables);

	printf("System of Equations:\n");
	printf("%s\n\n", file_str);

	int aux_sign = 1;
	char *aux_int;
	char *aux_str;
	reset_variables(&aux_int, &aux_str);

	int amnt_actual_variables = 0;

	int row_index = 0;
	int i;

	char is_variable = 0;

	for (i = 0;; i++)
	{
		double value;
		if (*(file_str + i) == '-' || *(file_str + i) == '+' || *(file_str + i) == '=' || *(file_str + i) == '\n' || *(file_str + i) == '\0')
		{
			value = (double)(((*aux_int == '\0') ? 1 : atoi(aux_int)) * aux_sign);
			if (*aux_str != '\0')
				if (index_of(variables, aux_str) < 0)
				{
					if (amnt_actual_variables >= *amnt_variables)
					//if there are more variable than should
					{
						*error = 2;
						return NULL;
					}
					push_back(variables, aux_str);
					amnt_actual_variables++;
				}

			if (*aux_str == '\0')
			{
				if (*(file_str + i) == '\n' || *(file_str + i) == '\0')
					*(*(matrix + row_index) + *amnt_variables) += value;
			}
			else
				*(*(matrix + row_index) + index_of(variables, aux_str)) += value;

			aux_sign = 1;
			is_variable = 0;
			reset_variables(&aux_int, &aux_str);
		}

		if (*(file_str + i) == '-')
			aux_sign = -1;

		if (*(file_str + i) == '\n')
		{
			row_index++;
			//if there are more equations than variables: consider only the top [amnt_variables] ones
			if (row_index >= *amnt_variables)
				break;
		}

		if (*(file_str + i) == '\0')
			break;
		if (isalpha(*(file_str + i)) || (is_variable && ((*(file_str + i)) != ' ')))
		{
			int j;
			is_variable = 1;
			for (j = 0;; j++)
				if (*(aux_str + j) == '\0')
				{
					*(aux_str + j) = *(file_str + i);
					*(aux_str + (j + 1)) = '\0';
					break;
				}
		}
		else if (isdigit(*(file_str + i)))
		{
			int j;
			for (j = 0;; j++)
				if (*(aux_int + j) == '\0')
				{
					*(aux_int + (j + 1)) = '\0';

					*(aux_int + j) = *(file_str + i);
					break;
				}
		}
	}

	free(aux_str);
	free(aux_int);
	free(file_str);

	//if there are less variables than should
	if (amnt_actual_variables != *amnt_variables)
	{
		*error = 2;
		return NULL;
	}
	return matrix;
}

char *str_from_file(char *nomeArquivo, int *amnt_variables)
{
	FILE *file = fopen(nomeArquivo, "r");
	if (file == NULL)
		return NULL;

	char *aux_return = (char *)malloc(sizeof(char) * 7500);
	char *aux_concat = (char *)malloc(sizeof(char) * 1024);
	*aux_return = '\0';

	fscanf(file, "%i", amnt_variables);
	fgets(aux_concat, 1024, file);
	while (fgets(aux_concat, 1024, file) != NULL)
		strcat(aux_return, aux_concat);

	char *ret = (char *)malloc(sizeof(char) * (strlen(aux_return) + 1));
	strcpy(ret, aux_return);

	free(aux_concat);
	free(aux_return);
	fclose(file);

	return ret;
}

double **malloc_matrix(int amnt_variables)
{
	double **matrix = (double **)malloc(amnt_variables * sizeof(double *));
	int i;
	for (i = 0; i < amnt_variables; i++)
		*(matrix + i) = (double *)malloc((amnt_variables + 1) * sizeof(double));

	int j;
	for (i = 0; i < amnt_variables; i++)
		for (j = 0; j < (amnt_variables + 1); j++)
			*(*(matrix + i) + j) = 0;

	return matrix;
}

void reset_variables(char **str1, char **str2)
{
	*str1 = (char *)malloc(sizeof(char) * 255);
	**str1 = '\0';
	*str2 = (char *)malloc(sizeof(char) * 255);
	**str2 = '\0';
}

double *solve_system(double **matrix, int amnt_variables, List *variables)
{
	char res = gaussian_elimination(matrix, amnt_variables, variables);
	if (!res)
		return NULL;

	double *solution = (double *)malloc(sizeof(double) * amnt_variables);
	int i;
	for (i = 0; i < amnt_variables; i++)
		*(solution + i) = *(*(matrix + i) + amnt_variables);

	free(matrix);

	return solution;
}

char gaussian_elimination(double **matrix, int amnt_variables, List *variables)
{
	if (variables != NULL)
	{
		printf("\n\n\nAdding all the same variables for each row, the system is as follows:\n");
		print_system(matrix, variables, amnt_variables);

		printf("\nWe will use Gaussian Elimination to solve the system. We can multiply by a real number, switch row positions or sum one row into the other. Our goal is to transform the matrix into the identity matrix.");
	}

	int i;
	int step = 1;
	for (i = 0; i < amnt_variables; i++)
	{
		double elem = *(*(matrix + i) + i);
		if (elem == 0)
		{
			if (variables != NULL)
			{
				printf("\n\n\n  -> Step %i (current row: %i):\n", step, i + 1);
				printf(" - The element from the main diagonal in this row is zero. Therefore, we will have to find another row below this one that has a number different from zero in this column (colunm %i).\n", i + 1);
			}

			int index_row_found = search_row_no_zero_in_column(matrix, i + 1, i, amnt_variables);

			if (variables != NULL)
			{
				if (index_row_found >= 0)
				{
					printf(" - We found! In the row %i and column %i, the element is not zero! So, we will switch these two rows!\n\n", index_row_found + 1, i + 1);
				}
				else
					printf(" - There were no other row with an element in the column %i that is diferent from zero!\n\n", i + 1);
			}

			if (index_row_found < 0)
				return 0; //error: invalid system
			switch_rows(matrix, i, index_row_found);

			if (variables != NULL)
			{
				printf("Now we will switch the rows with indexes %i e %i. The matrix is now as follows:\n", i + 1, index_row_found + 1);
				print_system(matrix, variables, amnt_variables);
			}

			elem = *(*(matrix + i) + i);

			step++;
		}

		double multiply_by = 1 / elem;
		multiply_row(matrix, i, multiply_by, amnt_variables);
		if (variables != NULL)
		{
			printf("\n\n\n  -> Step %i (current row: %i):\n", step, i + 1);
			printf(" - We will now multiply the whole row by a real number so that the element in this row from the main diagonal becomes one.\n");
			printf(" - In this case, we will mutiply by %.2lf, because 1/%.2lf = %.2lf\n\n", multiply_by, elem, multiply_by);

			printf("After the multiplication, the row becomes as follows:\n");
			print_system(matrix, variables, amnt_variables);
		}
		step++;

		empty_column_other_rows(matrix, i, amnt_variables);
		if (variables != NULL)
		{
			printf("\n\n\n  -> Step %i (current row: %i):\n", step, i + 1);
			printf(" - Now we need to multiply this row by real numbers and add to the other rows.\n");
			printf(" - The goal is to set the element in the column %i of every row below the current one to zero.\n\n", i + 1);

			printf("After this procedure, the system becomes as follows:\n");
			print_system(matrix, variables, amnt_variables);
		}
		step++;
	}

	if (variables != NULL)
	{
		printf("\n\n\nNow we finished Gaussian Elimination and the system of equations is solved.");
	}

	return 1; //no errors
}

void empty_column_other_rows(double **matrix, int column_row_index, int amnt_variables)
{
	int i;
	for (i = 0; i < amnt_variables; i++)
		if (i != column_row_index)
		{
			double mult = -*(*(matrix + i) + column_row_index);
			multiply_row1_and_sum_row2(matrix, column_row_index, mult, i, amnt_variables);

			*(*(matrix + i) + column_row_index) = 0; //just in case it is not an exact result
		}
}

void multiply_row1_and_sum_row2(double **matrix, int row_index_multiplied, double mult, int indexLinhaReceber, int amnt_variables)
{
	int i;
	for (i = 0; i <= amnt_variables; i++)
		*(*(matrix + indexLinhaReceber) + i) += mult * *(*(matrix + row_index_multiplied) + i);
}

void multiply_row(double **matrix, int row_index, double multiply_by, int amnt_variables)
{
	int i;
	for (i = 0; i <= amnt_variables; i++)
		*(*(matrix + row_index) + i) *= multiply_by;
}

int search_row_no_zero_in_column(double **matrix, int first_row, int colunm_index, int amnt_variables)
{
	int i;
	for (i = first_row; i < amnt_variables; i++)
		if (*(*(matrix + i) + colunm_index) != 0)
			return i;

	return -1;
}

void switch_rows(double **matrix, int index_row1, int index_row2)
{
	double *aux = *(matrix + index_row1);
	*(matrix + index_row1) = *(matrix + index_row2);
	*(matrix + index_row2) = aux;
}

void print_system(double **matrix, List *variables, int amnt_variables)
{
	int i, j;
	for (i = 0; i < amnt_variables; i++)
	{
		for (j = 0; j < amnt_variables; j++)
		{
			if (*(*(matrix + i) + j) >= 0)
			{
				if (j != 0)
					printf(" + ");
			}
			else
			{
				if (j != 0)
					printf(" - ");
				else
					printf("-");
			}
			printf("%.2lf%s", fabs(*(*(matrix + i) + j)), element_in(variables, j));
		}
		printf(" = %.2lf\n", *(*(matrix + i) + amnt_variables));
	}
}

void printarMatriz(double **matrix, int linhas, int colunas)
{
	int i;
	int j;
	for (i = 0; i < linhas; i++)
	{
		for (j = 0; j < colunas; j++)
			printf("%lf  ", *(*(matrix + i) + j));
		printf("\n");
	}
}
