#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOMES 1024
#define MAX_TAMANHO_NOME 32

typedef struct {
		char *valores;
		int topo;
} pilha;

pilha fila;

int isEmpty(pilha *fila) {
		return fila->topo == 0;
}

int isFull(pilha *fila) {
		return fila->topo == MAX_NOMES;
}

void push(pilha *fila, const char *nome, char *log) {
		if (isFull(fila)) {
				fprintf(stderr, "Erro: Pilha cheia\n");
				return;
		}

		strncpy(&fila->valores[fila->topo * MAX_TAMANHO_NOME], nome, MAX_TAMANHO_NOME);
		fila->valores[(fila->topo + 1) * MAX_TAMANHO_NOME - 1] = '\0'; // Garantir nulo

		if (log) {
				char temp[MAX_TAMANHO_NOME + 10];
				snprintf(temp, sizeof(temp), "push-%s ", nome);
				strcat(log, temp);
		}
		fila->topo++;
}

char *pop(pilha *fila) {
		if (isEmpty(fila)) {
				fprintf(stderr, "Erro: Pilha vazia\n");
				return NULL;
		}
		fila->topo--;
		return &fila->valores[fila->topo * MAX_TAMANHO_NOME];
}

void ordenacao(pilha *fila, const char *nome, char *log) {
		pilha filaAux;
		filaAux.valores = malloc(sizeof(char) * MAX_NOMES * MAX_TAMANHO_NOME);
		filaAux.topo = 0;

		int popCount = 0;
		while (!isEmpty(fila)) {
				char *nomeAux = pop(fila);
				if (strcmp(nomeAux, nome) <= 0) {
						push(fila, nomeAux, NULL);
						break;
				}
				push(&filaAux, nomeAux, NULL);
				popCount++;
		}

		if (log && popCount > 0) {
				char temp[20];
				snprintf(temp, sizeof(temp), "%dx-pop ", popCount);
				strcat(log, temp);
		}
		push(fila, nome, log);

		while (!isEmpty(&filaAux)) {
				push(fila, pop(&filaAux), log);
		}

		free(filaAux.valores);
}

void inicializar() {
		fila.valores = malloc(sizeof(char) * MAX_NOMES * MAX_TAMANHO_NOME);
		fila.topo = 0;
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
		inicializar();

		const char *inputCaminho = "./L1Q2.in";
		const char *outputCaminho = "./L1Q2.out";

		char *conteudo = lerArquivo(inputCaminho);

		char *linhas[1000];
		int linhasCount = 0;

		char *linha = strtok(conteudo, "\r\n");
		while (linha) {
				linhas[linhasCount++] = strdup(linha);
				linha = strtok(NULL, "\r\n");
		}

		char output[500000] = ""; // Buffer maior

		for (int i = 0; i < linhasCount; i++) {
				char *nomes[MAX_NOMES];
				int nomesCount = 0;

				char *token = strtok(linhas[i], " ");
				while (token) {
						nomes[nomesCount++] = strdup(token);
						token = strtok(NULL, " ");
				}

				char log[50000] = ""; // Buffer maior

				for (int j = 0; j < nomesCount; j++) {
						ordenacao(&fila, nomes[j], log);
				}

				for (int j = 50000; j >= 0; j--) {
					if (log[j] == 32) {
						log[j] = '\0';
						break;
					}
				}

				strcat(output, log);
				strcat(output, "\n");

				for (int j = 0; j < nomesCount; j++) {
						free(nomes[j]);
				}

				fila.topo = 0; // Limpa a pilha para a próxima linha
		}

		escreverArquivo(outputCaminho, output);

		printf("Resultado gerado:\n%s", output);

		free(fila.valores);
		free(conteudo);
		for (int i = 0; i < linhasCount; i++) {
				free(linhas[i]);
		}

		return 0;
}
