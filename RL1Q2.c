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

// Verifica se a pilha está vazia
int isEmpty(pilha *fila) {
		return fila->topo == 0;
}

// Verifica se a pilha está cheia
int isFull(pilha *fila) {
		return fila->topo == MAX_NOMES;
}

// Empilha um elemento na pilha e registra no log
void push(pilha *fila, char *nome, char *log) {
		if (isFull(fila)) {
				fprintf(stderr, "Erro: Pilha cheia\n");
				return;
		}

		memcpy(&fila->valores[fila->topo * MAX_TAMANHO_NOME], nome, MAX_TAMANHO_NOME);
		fila->topo++;

		// Registra a operação de push no log
		if (log) {
				char temp[MAX_TAMANHO_NOME + 10];
				sprintf(temp, "push-%s ", nome);
				strcat(log, temp);
		}
}

// Desempilha um elemento da pilha e registra no log
char *pop(pilha *fila, char *log) {
		if (isEmpty(fila)) {
				fprintf(stderr, "Erro: Pilha vazia\n");
				return NULL;
		}

		fila->topo--;

		// Registra a operação de pop no log
		if (log) {
				strcat(log, "1x-pop ");
		}

		return &fila->valores[fila->topo * MAX_TAMANHO_NOME];
}

// Realiza a ordenação com rastreamento das operações
void ordenacao(pilha *fila, char *nome, char *log) {
		pilha filaAux;
		filaAux.valores = malloc(sizeof(char) * MAX_NOMES * MAX_TAMANHO_NOME);
		filaAux.topo = 0;

		int popCount = 0;

		// Remove elementos da pilha principal até encontrar a posição correta
		while (!isEmpty(fila)) {
				char *nomeAux = pop(fila, NULL); // Pop sem log
				push(&filaAux, nomeAux, NULL);  // Push na auxiliar sem log
				popCount++;

				if (strcmp(nomeAux, nome) <= 0) {
						break; // Parar ao encontrar posição correta
				}
		}

		// Registra o número total de pops consecutivos no log
		if (popCount > 0) {
				char temp[20];
				sprintf(temp, "%dx-pop ", popCount);
				strcat(log, temp);
		}

		// Insere o novo elemento na posição correta
		push(fila, nome, log);

		// Reempilha os elementos da auxiliar de volta para a principal
		while (!isEmpty(&filaAux)) {
				char *nomeAux = pop(&filaAux, NULL); // Pop sem log
				push(fila, nomeAux, NULL);          // Push na principal sem log
		}

		free(filaAux.valores);
}

// Inicializa a pilha
void inicializar() {
		fila.valores = malloc(sizeof(char) * MAX_NOMES * MAX_TAMANHO_NOME);
		fila.topo = 0;
}

// Lê o conteúdo de um arquivo
char *lerArquivo(char *caminho) {
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

// Escreve o conteúdo no arquivo de saída
void escreverArquivo(char *caminho, char *data) {
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

		char *inputCaminho = "./L1Q2.in";
		char *outputCaminho = "./L1Q2.out";

		char *conteudo = lerArquivo(inputCaminho);

		char *linhas[1000];
		int linhasCount = 0;

		char *linha = strtok(conteudo, "\r\n");
		while (linha) {
				linhas[linhasCount++] = strdup(linha);
				linha = strtok(NULL, "\r\n");
		}

		char output[50000] = ""; // Buffer para a saída final

		for (int i = 0; i < linhasCount; i++) {
				char *nomes[MAX_NOMES];
				int nomesCount = 0;

				char *token = strtok(linhas[i], " ");
				while (token) {
						nomes[nomesCount++] = strdup(token);
						token = strtok(NULL, " ");
				}

				// Log para operações da linha atual
				char log[5000] = "";

				for (int j = 0; j < nomesCount; j++) {
						ordenacao(&fila, nomes[j], log);
				}

				strcat(output, log);
				strcat(output, "\n");

				for (int j = 0; j < nomesCount; j++) {
						free(nomes[j]);
				}

				fila.topo = 0; // Limpa a pilha para a próxima linha
		}

		escreverArquivo(outputCaminho, output);

		printf("Resultado gerado:\n%s", output); // Exibe o resultado no terminal

		free(fila.valores);
		free(conteudo);
		for (int i = 0; i < linhasCount; i++) {
				free(linhas[i]);
		}

		return 0;
}
