#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAM_FILA 5      // tamanho fixo da fila circular
#define TAM_PILHA 3     // capacidade da pilha de reserva

// -----------------------------------------------------------
// Estrutura que representa cada peça do Tetris Stack
// -----------------------------------------------------------
typedef struct {
    char nome;   // tipo da peça ('I', 'O', 'T', 'L')
    int id;      // identificador único
} Peca;

// -----------------------------------------------------------
// Fila circular
// -----------------------------------------------------------
typedef struct {
    Peca elementos[TAM_FILA];
    int inicio;      // índice do elemento da frente
    int fim;         // índice onde será inserido o próximo elemento
    int quantidade;  // número atual de elementos
} Fila;

// -----------------------------------------------------------
// Pilha simples (array) para reserva
// -----------------------------------------------------------
typedef struct {
    Peca elementos[TAM_PILHA];
    int topo; // índice do próximo elemento livre (topo - 1 é o elemento atual)
} Pilha;

// -----------------------------------------------------------
// Tipos possíveis de peças
// -----------------------------------------------------------
char tipos[4] = {'I', 'O', 'T', 'L'};

// -----------------------------------------------------------
// Gera automaticamente uma nova peça com id incremental
// -----------------------------------------------------------
Peca gerarPeca(int *contadorId) {
    Peca nova;
    nova.nome = tipos[rand() % 4];
    nova.id = (*contadorId)++;
    return nova;
}

// -----------------------------------------------------------
// Inicializa a fila preenchendo-a totalmente com peças
// -----------------------------------------------------------
void inicializarFila(Fila *f, int *contadorId) {
    f->inicio = 0;
    f->fim = 0;
    f->quantidade = 0;

    for (int i = 0; i < TAM_FILA; i++) {
        f->elementos[f->fim] = gerarPeca(contadorId);
        f->fim = (f->fim + 1) % TAM_FILA;
        f->quantidade++;
    }
}

// -----------------------------------------------------------
// Inicializa a pilha vazia
// -----------------------------------------------------------
void inicializarPilha(Pilha *p) {
    p->topo = 0; // pilha vazia
}

// -----------------------------------------------------------
// Verifica se a fila está cheia
// -----------------------------------------------------------
int filaCheia(Fila *f) {
    return f->quantidade == TAM_FILA;
}

// -----------------------------------------------------------
// Verifica se a fila está vazia
// -----------------------------------------------------------
int filaVazia(Fila *f) {
    return f->quantidade == 0;
}

// -----------------------------------------------------------
// Enfileira (enqueue). 
// Se a fila estiver cheia, esta versão retorna 0 (falha).
// Há também uma função enfileirar_com_sobrescrita abaixo que
// garante sempre enfileirar (descartando o mais antigo).
// -----------------------------------------------------------
int enfileirar(Fila *f, Peca p) {
    if (filaCheia(f)) return 0;
    f->elementos[f->fim] = p;
    f->fim = (f->fim + 1) % TAM_FILA;
    f->quantidade++;
    return 1;
}

// -----------------------------------------------------------
// Enfileira mas, se a fila estiver cheia, descarta o elemento
// mais antigo (avanço do início) para manter a fila sempre cheia.
// Retorna 1 sempre (e informa se descartou).
// OBS: essa escolha garante a invariância "fila sempre cheia" mesmo
// para operações que não removem da fila antes de gerar nova peça.
// -----------------------------------------------------------
int enfileirar_com_sobrescrita(Fila *f, Peca p) {
    int descartou = 0;
    if (filaCheia(f)) {
        // Descartar o mais antigo (avanço do início)
        f->inicio = (f->inicio + 1) % TAM_FILA;
        f->quantidade--; // temporariamente diminui para permitir a inserção
        descartou = 1;
    }
    // Inserir normalmente
    f->elementos[f->fim] = p;
    f->fim = (f->fim + 1) % TAM_FILA;
    f->quantidade++;
    return descartou;
}

// -----------------------------------------------------------
// Desenfileira (dequeue). Retorna 1 se removeu + coloca peça em out.
// Se vazia, retorna 0.
// -----------------------------------------------------------
int desenfileirar(Fila *f, Peca *out) {
    if (filaVazia(f)) return 0;
    *out = f->elementos[f->inicio];
    f->inicio = (f->inicio + 1) % TAM_FILA;
    f->quantidade--;
    return 1;
}

// -----------------------------------------------------------
// Verifica se pilha cheia/vazia
// -----------------------------------------------------------
int pilhaCheia(Pilha *p) {
    return p->topo == TAM_PILHA;
}
int pilhaVazia(Pilha *p) {
    return p->topo == 0;
}

// -----------------------------------------------------------
// Push na pilha. Retorna 1 se sucesso, 0 se cheia.
// -----------------------------------------------------------
int push(Pilha *p, Peca item) {
    if (pilhaCheia(p)) return 0;
    p->elementos[p->topo++] = item;
    return 1;
}

// -----------------------------------------------------------
// Pop na pilha. Retorna 1 se sucesso (coloca item em out), 0 se vazia.
// -----------------------------------------------------------
int pop(Pilha *p, Peca *out) {
    if (pilhaVazia(p)) return 0;
    *out = p->elementos[--p->topo];
    return 1;
}

// -----------------------------------------------------------
// Exibe a fila (da frente para o fim) e a pilha (Topo -> Base)
// -----------------------------------------------------------
void exibirEstado(Fila *f, Pilha *p) {
    printf("\nEstado atual:\n\n");

    // Fila
    printf("Fila de peças\t");
    if (filaVazia(f)) {
        printf("[vazia]\n");
    } else {
        int idx = f->inicio;
        for (int c = 0; c < f->quantidade; c++) {
            Peca pc = f->elementos[idx];
            printf("[%c %d] ", pc.nome, pc.id);
            idx = (idx + 1) % TAM_FILA;
        }
        printf("\n");
    }

    // Pilha
    printf("Pilha de reserva\t(Topo -> Base): ");
    if (pilhaVazia(p)) {
        printf("[vazia]\n");
    } else {
        // topo - 1 é o topo atual
        for (int i = p->topo - 1; i >= 0; i--) {
            Peca pc = p->elementos[i];
            printf("[%c %d] ", pc.nome, pc.id);
        }
        printf("\n");
    }
}

// -----------------------------------------------------------
// Operação: Jogar peça
// - Remove da frente da fila (desenfileira) e exibe a peça jogada.
// - Gera automaticamente nova peça e enfileira (mantendo fila cheia).
// -----------------------------------------------------------
void operacaoJogar(Fila *f, Pilha *p, int *contadorId) {
    Peca jogada;
    if (!desenfileirar(f, &jogada)) {
        printf("A fila está vazia — não foi possível jogar.\n");
        // Mesmo se não houve remoção, por decisão de design geramos uma nova peça
        // e enfileiramos com sobrescrita para manter a fila cheia.
        Peca nova = gerarPeca(contadorId);
        int desc = enfileirar_com_sobrescrita(f, nova);
        if (desc) {
            printf("(Ao gerar nova peça a fila estava cheia — descartado o elemento mais antigo.)\n");
        }
        return;
    }

    printf("Peça jogada: [%c %d]\n", jogada.nome, jogada.id);

    // Gerar nova peça e enfileirar (fila tinha espaço após desenfileirar)
    Peca nova = gerarPeca(contadorId);
    if (!enfileirar(f, nova)) {
        // caso improvável, usar a versão com sobrescrita
        int desc = enfileirar_com_sobrescrita(f, nova);
        if (desc) {
            printf("(Ao gerar nova peça a fila estava cheia — descartado o elemento mais antigo.)\n");
        }
    }
}

// -----------------------------------------------------------
// Operação: Reservar peça
// - Move a peça da frente da fila para o topo da pilha, se houver espaço.
// - Em seguida gera nova peça e enfileira para manter fila cheia.
// -----------------------------------------------------------
void operacaoReservar(Fila *f, Pilha *p, int *contadorId) {
    if (pilhaCheia(p)) {
        printf("A pilha de reserva está cheia! Não é possível reservar.\n");
        return;
    }

    Peca removida;
    if (!desenfileirar(f, &removida)) {
        printf("A fila está vazia! Não foi possível reservar.\n");
        // mesmo aqui, manteremos a fila cheia gerando nova peça com sobrescrita
        Peca nova = gerarPeca(contadorId);
        int desc = enfileirar_com_sobrescrita(f, nova);
        if (desc) {
            printf("(Ao gerar nova peça a fila estava cheia — descartado o elemento mais antigo.)\n");
        }
        return;
    }

    // Colocar na pilha
    if (!push(p, removida)) {
        // Caso não consiga (a checagem anterior deveria impedir), devolve/remaneja
        printf("Erro: não foi possível empilhar a peça reservada.\n");
    } else {
        printf("Peça reservada: [%c %d] (empilhada no topo)\n", removida.nome, removida.id);
    }

    // Gerar nova peça e enfileirar (fila tinha espaço após desenfileirar)
    Peca nova = gerarPeca(contadorId);
    if (!enfileirar(f, nova)) {
        int desc = enfileirar_com_sobrescrita(f, nova);
        if (desc) {
            printf("(Ao gerar nova peça a fila estava cheia — descartado o elemento mais antigo.)\n");
        }
    }
}

// -----------------------------------------------------------
// Operação: Usar peça reservada
// - Remove a peça do topo da pilha (pop) e mostra qual foi usada.
// - Após isso, gera nova peça e enfileira-a. Como esta operação NÃO
//   removeu elemento da fila, para manter a fila sempre cheia,
//   enfileiramos using enfileirar_com_sobrescrita, que descarta o mais antigo.
//   Essa é uma escolha de design para manter a invariância "fila sempre cheia".
// -----------------------------------------------------------
void operacaoUsarReservada(Fila *f, Pilha *p, int *contadorId) {
    Peca usada;
    if (!pop(p, &usada)) {
        printf("A pilha de reserva está vazia! Não há peça para usar.\n");
        return;
    }

    printf("Peça usada da reserva: [%c %d]\n", usada.nome, usada.id);

    // Gerar nova peça e enfileirar. Como não houve remoção da fila, usamos
    // a versão com sobrescrita para garantir manutenção do tamanho.
    Peca nova = gerarPeca(contadorId);
    int descartou = enfileirar_com_sobrescrita(f, nova);
    if (descartou) {
        printf("(Ao gerar nova peça a fila estava cheia — descartado o elemento mais antigo.)\n");
    }
}

// -----------------------------------------------------------
// Menu principal e loop
// -----------------------------------------------------------
int main() {
    srand((unsigned int)time(NULL));
    Fila fila;
    Pilha pilha;
    int contadorId = 0; // ID global e incremental de peças
    int opcao;

    inicializarFila(&fila, &contadorId);
    inicializarPilha(&pilha);

    do {
        exibirEstado(&fila, &pilha);

        printf("\nOpções de Ação:\n");
        printf("1\tJogar peça\n");
        printf("2\tReservar peça\n");
        printf("3\tUsar peça reservada\n");
        printf("0\tSair\n");
        printf("Opção: ");
        if (scanf("%d", &opcao) != 1) {
            // limpar stdin e continuar
            while (getchar() != '\n');
            printf("Entrada inválida. Tente novamente.\n");
            opcao = -1;
            continue;
        }

        switch (opcao) {
            case 1:
                operacaoJogar(&fila, &pilha, &contadorId);
                break;
            case 2:
                operacaoReservar(&fila, &pilha, &contadorId);
                break;
            case 3:
                operacaoUsarReservada(&fila, &pilha, &contadorId);
                break;
            case 0:
                printf("Encerrando...\n");
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
        }

    } while (opcao != 0);

    return  0 ;
}
