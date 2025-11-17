#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAM_FILA 5   // tamanho fixo da fila circular

// -----------------------------------------------------------
// Estrutura que representa cada peça do Tetris Stack
// -----------------------------------------------------------
typedef struct {
    char nome;   // tipo da peça ('I', 'O', 'T', 'L')
    int id;      // identificador único
} Peca;

// -----------------------------------------------------------
// Estrutura da fila circular
// -----------------------------------------------------------
typedef struct {
    Peca elementos[TAM_FILA];
    int inicio;
    int fim;
    int quantidade;
} Fila;

// -----------------------------------------------------------
// Tipos possíveis de peças
// -----------------------------------------------------------
char tipos[4] = {'I', 'O', 'T', 'L'};

// -----------------------------------------------------------
// Gera automaticamente uma nova peça com id incremental
// -----------------------------------------------------------
Peca gerarPeca(int id) {
    Peca nova;
    nova.nome = tipos[rand() % 4];
    nova.id = id;
    return nova;
}

// -----------------------------------------------------------
// Inicializa a fila com peças já geradas
// -----------------------------------------------------------
void inicializarFila(Fila *f, int *contadorId) {
    f->inicio = 0;
    f->fim = 0;
    f->quantidade = 0;

    for (int i = 0; i < TAM_FILA; i++) {
        f->elementos[f->fim] = gerarPeca((*contadorId)++);
        f->fim = (f->fim + 1) % TAM_FILA;
        f->quantidade++;
    }
}

// -----------------------------------------------------------
// Insere uma peça no fim da fila (enqueue)
// -----------------------------------------------------------
void enfileirar(Fila *f, Peca p) {
    if (f->quantidade == TAM_FILA) {
        printf("A fila está cheia! Não é possível adicionar nova peça.\n");
        return;
    }

    f->elementos[f->fim] = p;
    f->fim = (f->fim + 1) % TAM_FILA;
    f->quantidade++;
}

// -----------------------------------------------------------
// Remove uma peça da frente da fila (dequeue)
// -----------------------------------------------------------
void desenfileirar(Fila *f) {
    if (f->quantidade == 0) {
        printf("A fila está vazia! Não há peça para jogar.\n");
        return;
    }

    Peca removida = f->elementos[f->inicio];
    printf("Peça jogada: [%c %d]\n", removida.nome, removida.id);

    f->inicio = (f->inicio + 1) % TAM_FILA;
    f->quantidade--;
}

// -----------------------------------------------------------
// Mostra o estado atual da fila
// -----------------------------------------------------------
void exibirFila(Fila *f) {
    printf("\nFila de peças:\n");

    if (f->quantidade == 0) {
        printf("[vazia]\n");
        return;
    }

    int i = f->inicio;

    for (int c = 0; c < f->quantidade; c++) {
        Peca p = f->elementos[i];
        printf("[%c %d] ", p.nome, p.id);
        i = (i + 1) % TAM_FILA;
    }
    printf("\n");
}

// -----------------------------------------------------------
// Programa principal
// -----------------------------------------------------------
int main() {
    srand(time(NULL));

    Fila fila;
    int contadorId = 0;  // garante IDs únicos
    int opcao;

    inicializarFila(&fila, &contadorId);

    printf("Confira a seguir seu estado:\n");

    do {
        exibirFila(&fila);

        printf("\nOpções de ação:\n");
        printf("1 - Jogar peça (dequeue)\n");
        printf("2 - Inserir nova peça (enqueue)\n");
        printf("0 - Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                desenfileirar(&fila);
                break;

            case 2:
                if (fila.quantidade < TAM_FILA) {
                    enfileirar(&fila, gerarPeca(contadorId++));
                } else {
                    printf("A fila está cheia! Não é possível adicionar nova peça.\n");
                }
                break;

            case 0:
                printf("Encerrando...\n");
                break;

            default:
                printf("Opção inválida!\n");
        }

    } while (opcao != 0);

    return 0;
}
