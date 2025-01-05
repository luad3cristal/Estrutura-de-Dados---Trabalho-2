#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 1000
#define MAX_LINE_LENGTH 1024

typedef struct {
  char instancia[MAX_LINE_LENGTH];
  int soma;
} bloco;

void insertionSort(int arr[], int n) {
  for (int i = 1; i < n; i++) {
    int key = arr[i];
    int j = i - 1;

    while (j >= 0 && arr[j] > key) {
      arr[j + 1] = arr[j];
      j = j - 1;
    }
    arr[j + 1] = key;
  }
}

void organizarLinha(bloco *linha, int n) {
  for (int i = 1; i < n; i++) {
    bloco key = linha[i];
    int j = i - 1;

    while (j >= 0 && linha[j].soma > key.soma) {
      linha[j + 1] = linha[j];
      j = j - 1;
    }
    linha[j + 1] = key;
  }
}

int removerSomasRepetidas(bloco *linha, int n) {
  int novoTamanho = 0;
  int encontrados[2 * MAX_LINES + 1] = {0};

  bloco tempLinha[n];

  for (int i = n - 1; i >= 0; i--) {
    if (!encontrados[linha[i].soma + MAX_LINES]) {

      encontrados[linha[i].soma + MAX_LINES] = 1;
      tempLinha[novoTamanho++] = linha[i];
    }
  }

  for (int i = 0; i < novoTamanho; i++) {
    linha[i] = tempLinha[novoTamanho - i - 1];
  }

  return novoTamanho;
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
  const char *inputCaminho = "./L1Q1.in";
  const char *outputCaminho = "./L1Q1.out";

  char *fileContent = lerArquivo(inputCaminho);

  char *linhas[MAX_LINES];
  int linhasCount = 0;

  char *token = strtok(fileContent, "\n");
  while (token) {
    linhas[linhasCount++] = strdup(token);
    token = strtok(NULL, "\n");
  }

  char outputBuffer[MAX_LINES * MAX_LINE_LENGTH] = "";

  for (int i = 0; i < linhasCount; i++) {
    char *linha = linhas[i];
    char *instancias[MAX_LINE_LENGTH];
    int instanciasCount = 0;

    char *linhaToken = strtok(linha, "start");

    while (linhaToken != NULL) {
      if (strlen(linhaToken) > 0) {
        instancias[instanciasCount++] = strdup(linhaToken);
      }
      linhaToken = strtok(NULL, "start");
    }

    bloco blocos[instanciasCount];

    for (int j = 0; j < instanciasCount; j++) {
      char *instancia = instancias[j];

      int numeros[MAX_LINE_LENGTH];
      int numCount = 0;

      char *numToken = strtok(instancia, " ");
      while (numToken != NULL) {
        numeros[numCount++] = atoi(numToken);
        numToken = strtok(NULL, " ");
      }

      insertionSort(numeros, numCount);

      char instanciaOrdenada[MAX_LINE_LENGTH] = "";
      for (int k = 0; k < numCount; k++) {
        char numStr[16];
        sprintf(numStr, "%d ", numeros[k]);
        strcat(instanciaOrdenada, numStr);
      }

      instanciaOrdenada[strlen(instanciaOrdenada) - 1] = '\0';
      strcpy(blocos[j].instancia, instanciaOrdenada);

      blocos[j].soma = 0;
      for (int k = 0; k < numCount; k++) {
        blocos[j].soma += numeros[k];
      }
    }

    organizarLinha(blocos, instanciasCount);
    instanciasCount = removerSomasRepetidas(blocos, instanciasCount);

    for (int j = 0; j < instanciasCount; j++) {
      strcat(outputBuffer, "start ");
      strcat(outputBuffer, blocos[j].instancia);
      strcat(outputBuffer, " ");
    }
    if (i < linhasCount - 1) {
      outputBuffer[strlen(outputBuffer) - 1] = '\n';
    }
  }
  outputBuffer[strlen(outputBuffer) - 1] = '\0';

  escreverArquivo(outputCaminho, outputBuffer);

  free(fileContent);
  for (int i = 0; i < linhasCount; i++) {
    free(linhas[i]);
  }

  return 0;
}
