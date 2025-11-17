#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAM_FILA 5   // capacidade da fila circular
#define TAM_PILHA 3  // capacidade da pilha de reserva

/* ----------- Estrutura da peça ----------- */
typedef struct {
    char nome;   // 'I','O','T','L'
    int id;      // id único incremental
} Peca;

/* ----------- Fila circular ----------- */
typedef struct {
    Peca elementos[TAM_FILA];
    int inicio;     // índice do elemento da frente
    int fim;        // índice onde inserir o próximo
    int quantidade; // número de elementos atualmente
} Fila;

/* ----------- Pilha (vetor) ----------- */
typedef struct {
    Peca elementos[TAM_PILHA];
    int topo; // número de elementos; topo efetivo está em topo-1
} Pilha;

/* tipos possíveis */
const char TIPOS[4] = {'I','O','T','L'};

/* Gera peça com id incremental */
Peca gerarPeca(int *contadorId) {
    Peca p;
    p.nome = TIPOS[rand() % 4];
    p.id = (*contadorId)++;
    return p;
}

/* Inicializa fila preenchendo-a com TAM_FILA peças */
void inicializarFila(Fila *f, int *contadorId) {
    f->inicio = 0;
    f->fim = 0;
    f->quantidade = 0;
    for (int i = 0; i < TAM_FILA; ++i) {
        f->elementos[f->fim] = gerarPeca(contadorId);
        f->fim = (f->fim + 1) % TAM_FILA;
        f->quantidade++;
    }
}

/* Inicializa pilha vazia */
void inicializarPilha(Pilha *p) {
    p->topo = 0;
}

/* Retorna índice real na fila dado deslocamento pos (0..quantidade-1) */
int idxFila(Fila *f, int pos) {
    return (f->inicio + pos) % TAM_FILA;
}

/* Desenfileira: remove frente e coloca em out; retorna 1 se sucesso, 0 se vazia */
int desenfileirar(Fila *f, Peca *out) {
    if (f->quantidade == 0) return 0;
    *out = f->elementos[f->inicio];
    f->inicio = (f->inicio + 1) % TAM_FILA;
    f->quantidade--;
    return 1;
}

/* Enfileira normal (assume espaço); retorna 1 em sucesso, 0 se cheia */
int enfileirar(Fila *f, Peca p) {
    if (f->quantidade == TAM_FILA) return 0;
    f->elementos[f->fim] = p;
    f->fim = (f->fim + 1) % TAM_FILA;
    f->quantidade++;
    return 1;
}

/* Push na pilha; retorna 1 se sucesso, 0 se cheia */
int push(Pilha *p, Peca x) {
    if (p->topo == TAM_PILHA) return 0;
    p->elementos[p->topo++] = x;
    return 1;
}

/* Pop da pilha; retorna 1 se sucesso e coloca em out, 0 se vazia */
int pop(Pilha *p, Peca *out) {
    if (p->topo == 0) return 0;
    *out = p->elementos[--p->topo];
    return 1;
}

/* Exibe estado atual da fila (frente->fim) e pilha (Topo -> Base) */
void exibirEstado(Fila *f, Pilha *p) {
    printf("\nEstado atual:\n\n");
    printf("Fila de peças\t");
    if (f->quantidade == 0) {
        printf("[vazia]");
    } else {
        for (int i = 0; i < f->quantidade; ++i) {
            Peca pc = f->elementos[idxFila(f, i)];
            printf("[%c %d] ", pc.nome, pc.id);
        }
    }
    printf("\n");

    printf("Pilha de reserva\t(Topo -> Base): ");
    if (p->topo == 0) {
        printf("(vazia)");
    } else {
        for (int i = p->topo - 1; i >= 0; --i) {
            Peca pc = p->elementos[i];
            printf("[%c %d] ", pc.nome, pc.id);
        }
    }
    printf("\n");
}

/* Opção 1: Jogar peça (remove frente da fila). Gera nova peça e enfileira. */
void opJogar(Fila *f, Pilha *p, int *contadorId) {
    Peca rem;
    if (!desenfileirar(f, &rem)) {
        printf("A fila está vazia. Nenhuma peça foi jogada.\n");
        return;
    }
    printf("Peça jogada: [%c %d]\n", rem.nome, rem.id);

    /* gera nova peça e enfileira para manter fila cheia */
    Peca nova = gerarPeca(contadorId);
    if (!enfileirar(f, nova)) {
        /* caso improvável (se fila cheia), apenas ignora */
        printf("(Não foi possível enfileirar nova peça)\n");
    } else {
        printf("Nova peça gerada: [%c %d]\n", nova.nome, nova.id);
    }
}

/* Opção 2: Reservar peça (move frente da fila para topo da pilha). */
void opReservar(Fila *f, Pilha *p, int *contadorId) {
    if (p->topo == TAM_PILHA) {
        printf("A pilha de reserva está cheia! Não é possível reservar.\n");
        return;
    }
    Peca rem;
    if (!desenfileirar(f, &rem)) {
        printf("A fila está vazia! Não foi possível reservar.\n");
        return;
    }

    if (!push(p, rem)) {
        printf("Erro ao empilhar a peça reservada.\n");
        return;
    }
    printf("Peça enviada para reserva: [%c %d]\n", rem.nome, rem.id);

    /* gera nova peça e enfileira para manter fila cheia */
    Peca nova = gerarPeca(contadorId);
    if (!enfileirar(f, nova)) {
        printf("(Não foi possível enfileirar a nova peça gerada)\n");
    } else {
        printf("Nova peça gerada: [%c %d]\n", nova.nome, nova.id);
    }
}

/* Opção 3: Usar peça reservada (pop). NÃO gera nova peça */
void opUsarReservada(Fila *f, Pilha *p, int *contadorId) {
    Peca usada;
    if (!pop(p, &usada)) {
        printf("A pilha de reserva está vazia! Não há peça para usar.\n");
        return;
    }
    printf("Peça usada da reserva: [%c %d]\n", usada.nome, usada.id);
    /* conforme regra, NÃO geramos nova peça aqui */
}

/* Opção 4: Trocar peça da frente da fila com o topo da pilha */
void opTrocarTopo(Fila *f, Pilha *p) {
    if (f->quantidade == 0) {
        printf("A fila está vazia. Nada para trocar.\n");
        return;
    }
    if (p->topo == 0) {
        printf("A pilha está vazia. Nada para trocar.\n");
        return;
    }
    int idxFrente = idxFila(f, 0);
    Peca tmp = f->elementos[idxFrente];
    f->elementos[idxFrente] = p->elementos[p->topo - 1];
    p->elementos[p->topo - 1] = tmp;
    printf("Troca realizada entre frente da fila e topo da pilha.\n");
}

/* Opção 5: Troca múltipla: troca os 3 primeiros da fila com as 3 peças da pilha.
   Requer que a fila tenha pelo menos 3 elementos (sempre terá) e pilha tenha 3. */
void opTrocaMultipla(Fila *f, Pilha *p) {
    if (f->quantidade < 3) {
        printf("A fila não tem 3 peças para a troca múltipla.\n");
        return;
    }
    if (p->topo < 3) {
        printf("A pilha não tem 3 peças para a troca múltipla.\n");
        return;
    }

    /* Salva os 3 primeiros da fila */
    Peca q0 = f->elementos[idxFila(f, 0)];
    Peca q1 = f->elementos[idxFila(f, 1)];
    Peca q2 = f->elementos[idxFila(f, 2)];

    /* Salva os 3 da pilha (base->top: indices 0,1,2; top é index 2) */
    Peca p0 = p->elementos[0]; // base
    Peca p1 = p->elementos[1];
    Peca p2 = p->elementos[2]; // top

    /* Após o exemplo do enunciado:
       - fila[0] <- p2 (top)
       - fila[1] <- p1
       - fila[2] <- p0 (base)
       - pilha (base->top) <- q0, q1, q2 (com top sendo q2)
    */
    f->elementos[idxFila(f, 0)] = p2;
    f->elementos[idxFila(f, 1)] = p1;
    f->elementos[idxFila(f, 2)] = p0;

    p->elementos[0] = q0;
    p->elementos[1] = q1;
    p->elementos[2] = q2;

    printf("Troca múltipla realizada entre os 3 primeiros da fila e as 3 peças da pilha.\n");
}

/* Main: loop do menu */
int main() {
    srand((unsigned int)time(NULL));
    Fila fila;
    Pilha pilha;
    int contadorId = 0;
    int opcao = -1;

    inicializarFila(&fila, &contadorId);
    inicializarPilha(&pilha);

    printf("=== TETRIS STACK - GERENCIADOR MESTRE DE PEÇAS ===\n");

    do {
        exibirEstado(&fila, &pilha);

        printf("\nOpções disponíveis:\n");
        printf("1\tJogar peça da frente da fila\n");
        printf("2\tEnviar peça da fila para a pilha de reserva\n");
        printf("3\tUsar peça da pilha de reserva\n");
        printf("4\tTrocar frente da fila com topo da pilha\n");
        printf("5\tTrocar os 3 primeiros da fila com as 3 peças da pilha\n");
        printf("0\tSair\n");
        printf("Opção escolhida: ");

        if (scanf("%d", &opcao) != 1) {
            /* limpa entrada inválida */
            while (getchar() != '\n');
            printf("Entrada inválida. Tente novamente.\n");
            opcao = -1;
            continue;
        }

        switch (opcao) {
            case 1: opJogar(&fila, &pilha, &contadorId); break;
            case 2: opReservar(&fila, &pilha, &contadorId); break;
            case 3: opUsarReservada(&fila, &pilha, &contadorId); break;
            case 4: opTrocarTopo(&fila, &pilha); break;
            case 5: opTrocaMultipla(&fila, &pilha); break;
            case 0: printf("Encerrando o programa... Obrigado por jogar Tetris Stack!\n"); break;
            default: printf("Opção inválida. Tente novamente.\n");
        }

    } while (opcao != 0);

    return 0;
}
