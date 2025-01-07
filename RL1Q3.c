#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 1000
#define MAX_NUMBERS 100

void insertionSortInt(int vetor[], int size) {
	for (int i = 1; i < size; i++) {
		int key = vetor[i];
		int j = i - 1;
		while (j >= 0 && vetor[j] > key) {
			vetor[j + 1] = vetor[j];
			j--;
		}
		vetor[j + 1] = key;
	}
}

void insertionSortFloat(float vetor[], int size) {
	for (int i = 1; i < size; i++) {
		float key = vetor[i];
		int j = i - 1;
		while (j >= 0 && vetor[j] > key) {
			vetor[j + 1] = vetor[j];
			j--;
		}
		vetor[j + 1] = key;
	}
}

void formatarFloat(char *buffer, float valor) {
	if ((int)valor == valor) {
		sprintf(buffer, "%d", (int)valor); 
	} 
	else {
		sprintf(buffer, "%.2f", valor); 
		
		for (int i = strlen(buffer) - 1; i > 0 && (buffer[i] == '0' || buffer[i] == '.'); i--) {
			if (buffer[i] == '.') {
				buffer[i] = '\0';
				break;
			}
			buffer[i] = '\0';
		}
	}
}

typedef struct NoCircular {
	float valor;
	struct NoCircular *prox;
} NoCircular;

typedef struct NoDuploInt {
	int chave;
	NoCircular *sublista;
	struct NoDuploInt *prox;
	struct NoDuploInt *ant;
} NoDuploInt;

typedef struct ListaDuplamenteLigadaInt {
	NoDuploInt *inicio;
	NoDuploInt *fim;
} ListaDuplamenteLigadaInt;

NoCircular *criarNoCircular(float valor) {
	NoCircular *novo = (NoCircular *)malloc(sizeof(NoCircular));
	novo->valor = valor;
	novo->prox = novo;
	return novo;
}

void inserirCircular(NoCircular **head, float valor) {
	NoCircular *novo = criarNoCircular(valor);
	if (!*head) {
		*head = novo;
		return;
	}

	NoCircular *atual = *head;
	while (atual->prox != *head) {
		atual = atual->prox;
	}

	atual->prox = novo;
	novo->prox = *head;
}

NoDuploInt *criarNoInt(int chave) {
	NoDuploInt *novo = (NoDuploInt *)malloc(sizeof(NoDuploInt));
	novo->chave = chave;
	novo->sublista = NULL;
	novo->prox = NULL;
	novo->ant = NULL;
	return novo;
}

ListaDuplamenteLigadaInt *inicializarListaInt() {
	ListaDuplamenteLigadaInt *lista = (ListaDuplamenteLigadaInt *)malloc(sizeof(ListaDuplamenteLigadaInt));
	lista->inicio = NULL;
	lista->fim = NULL;
	return lista;
}

void inserirOrdenadoInt(ListaDuplamenteLigadaInt *lista, int chave) {
	NoDuploInt *novo = criarNoInt(chave);

	if (lista->inicio == NULL) {
		lista->inicio = novo;
		lista->fim = novo;
		return;
	}

	NoDuploInt *atual = lista->inicio;

	while (atual != NULL && atual->chave < chave) {
		atual = atual->prox;
	}

	if (atual == NULL) {
		lista->fim->prox = novo;
		novo->ant = lista->fim;
		lista->fim = novo;
	} else if (atual == lista->inicio) {
		novo->prox = lista->inicio;
		lista->inicio->ant = novo;
		lista->inicio = novo;
	} else {
		novo->prox = atual;
		novo->ant = atual->ant;
		atual->ant->prox = novo;
		atual->ant = novo;
	}
}

void conectarLE_LI(ListaDuplamenteLigadaInt *listaLE, float *numerosLI, int *countLI) {
	NoDuploInt *atual = listaLE->inicio;
	while (atual != NULL) {
		int chave = atual->chave;
		for (int i = 0; i < *countLI; i++) {
			if (numerosLI[i] >= chave - 0.99 && numerosLI[i] <= chave + 0.99) {
				inserirCircular(&atual->sublista, numerosLI[i]);

				for (int j = i; j < *countLI - 1; j++) {
					numerosLI[j] = numerosLI[j + 1];
				}
				(*countLI)--;
				i--;
			}
		}
		atual = atual->prox;
	}
}

void imprimirListaLEtoString(ListaDuplamenteLigadaInt *lista, char *output) {
	NoDuploInt *atual = lista->inicio;

  strcat(output, "[");	
	while (atual != NULL) {
		char buffer[50];
		sprintf(buffer, "%d(", atual->chave);
		strcat(output, buffer);
		NoCircular *sub = atual->sublista;
		if (sub) {
			NoCircular *inicio = sub;
			do {
				formatarFloat(buffer, sub->valor);
				strcat(output, buffer);
				sub = sub->prox;
				if (sub != inicio) {
					strcat(output, "->");
				}
			} while (sub != inicio);
		}
		strcat(output, ")");
		if (atual->prox != NULL) {
			strcat(output, "->");
		}
		atual = atual->prox;
	}
  strcat(output, "]");
}

void liberarListaLE(ListaDuplamenteLigadaInt *lista) {
	NoDuploInt *atual = lista->inicio;
	while (atual != NULL) {
		NoDuploInt *temp = atual;

		NoCircular *sub = atual->sublista;
		if (sub) {
			NoCircular *inicio = sub;
			NoCircular *prox;
			do {
				prox = sub->prox;
				free(sub);
				sub = prox;
			} while (sub != inicio);
		}

		atual = atual->prox;
		free(temp);
	}
	free(lista);
}

void separarLEeLI(const char *linha, float *numerosLE, float *numerosLI, int *countLE, int *countLI) {
	char *token = strtok((char *)linha, " ");
	int isLI = 0;

	while (token != NULL) {
		if (strcmp(token, "LE") == 0) {
			isLI = 0;
		} else if (strcmp(token, "LI") == 0) {
			isLI = 1;
		} else {
			float numero = atof(token);
			if (isLI) {
				numerosLI[(*countLI)++] = numero;
			} else {
				numerosLE[(*countLE)++] = numero;
			}
		}
		token = strtok(NULL, " ");
	}
}

char *lerArquivo(const char *caminho) {
	FILE *arquivo = fopen(caminho, "r");
	if (!arquivo) {
		perror("Erro ao abrir o arquivo de entrada");
		exit(EXIT_FAILURE);
	}

	fseek(arquivo, 0, SEEK_END);
	long fileSize = ftell(arquivo);
	rewind(arquivo);

	char *buffer = (char *)malloc((fileSize + 1) * sizeof(char));
	fread(buffer, 1, fileSize, arquivo);
	buffer[fileSize] = '\0';

	fclose(arquivo);
	return buffer;
}

void escreverArquivo(const char *caminho, const char *data) {
	FILE *arquivo = fopen(caminho, "w");
	if (!arquivo) {
		perror("Erro ao abrir o arquivo de saída");
		exit(EXIT_FAILURE);
	}

	fprintf(arquivo, "%s", data);
	fclose(arquivo);
}


int main() {
	const char *inputCaminho = "./L1Q3.in";
	const char *outputCaminho = "./L1Q3.out";
	char *fileContent = lerArquivo(inputCaminho);

	char *linhas[MAX_LINES];
	int linhasCount = 0;

	char *token = strtok(fileContent, "\n");
	while (token) {
		linhas[linhasCount++] = strdup(token);
		token = strtok(NULL, "\n");
	}

	char output[500000] = "";

	for (int i = 0; i < linhasCount; i++) {
		float numerosLE[MAX_NUMBERS], numerosLI[MAX_NUMBERS];
		int countLE = 0, countLI = 0;

		separarLEeLI(linhas[i], numerosLE, numerosLI, &countLE, &countLI);

		insertionSortInt((int *)numerosLE, countLE);
		insertionSortFloat(numerosLI, countLI);

		ListaDuplamenteLigadaInt *listaLE = inicializarListaInt();

		for (int j = 0; j < countLE; j++) {
			inserirOrdenadoInt(listaLE, (int)numerosLE[j]);
		}

		conectarLE_LI(listaLE, numerosLI, &countLI);

		imprimirListaLEtoString(listaLE, output);
		if (i + 1 != linhasCount)
			strcat(output, "\n");

		
		liberarListaLE(listaLE);
	}

	escreverArquivo(outputCaminho, output);

	free(fileContent);
	for (int i = 0; i < linhasCount; i++) {
		free(linhas[i]);
	}

	return 0;
}