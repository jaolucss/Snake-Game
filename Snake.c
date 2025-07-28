#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

// Configuracoes do jogo
#define LARGURA 30
#define ALTURA 15
#define PAREDE '#'
#define CABECA 'O'
#define CORPO 'o'
#define COMIDA '*'
#define VAZIO ' '

// Estrutura para posicao
typedef struct {
    int x, y;
} Posicao;

// No da lista encadeada (cada pedaco da cobra)
typedef struct Pedaco {
    Posicao pos;
    struct Pedaco* proximo;
} Pedaco;

// A cobra (lista encadeada)
typedef struct {
    Pedaco* cabeca;
    int tamanho;
    int direcao; // 1=cima, 2=baixo, 3=esquerda, 4=direita
} Cobra;

// Variaveis globais
Cobra cobra;
Posicao comida;
char tabuleiro[ALTURA][LARGURA];
int pontos = 0;
int fim_jogo = 0;

// Funcao para limpar tela
void limpar_tela() {
    system("cls");
}

// Criar um novo pedaco da cobra
Pedaco* criar_pedaco(int x, int y) {
    Pedaco* novo = (Pedaco*)malloc(sizeof(Pedaco));
    novo->pos.x = x;
    novo->pos.y = y;
    novo->proximo = NULL;
    return novo;
}

// Adicionar pedaco na frente da cobra (nova cabeca)
void adicionar_cabeca(int x, int y) {
    Pedaco* nova_cabeca = criar_pedaco(x, y);
    nova_cabeca->proximo = cobra.cabeca;
    cobra.cabeca = nova_cabeca;
    cobra.tamanho++;
}

// Remover ultimo pedaco da cobra (cauda)
void remover_cauda() {
    if (cobra.cabeca == NULL) return;
    
    if (cobra.cabeca->proximo == NULL) {
        // So tem um pedaco
        free(cobra.cabeca);
        cobra.cabeca = NULL;
    } else {
        // Encontrar o penultimo pedaco
        Pedaco* atual = cobra.cabeca;
        while (atual->proximo->proximo != NULL) {
            atual = atual->proximo;
        }
        free(atual->proximo);
        atual->proximo = NULL;
    }
    cobra.tamanho--;
}

// Verificar se posicao esta ocupada pela cobra
int posicao_ocupada_cobra(int x, int y) {
    Pedaco* atual = cobra.cabeca;
    while (atual != NULL) {
        if (atual->pos.x == x && atual->pos.y == y) {
            return 1; // Verdadeiro
        }
        atual = atual->proximo;
    }
    return 0; // Falso
}

// Criar o tabuleiro
void criar_tabuleiro() {
    for (int i = 0; i < ALTURA; i++) {
        for (int j = 0; j < LARGURA; j++) {
            if (i == 0 || i == ALTURA-1 || j == 0 || j == LARGURA-1) {
                tabuleiro[i][j] = PAREDE;
            } else {
                tabuleiro[i][j] = VAZIO;
            }
        }
    }
}

// Colocar comida em posicao aleatoria
void gerar_comida() {
    do {
        comida.x = rand() % (LARGURA - 2) + 1;
        comida.y = rand() % (ALTURA - 2) + 1;
    } while (posicao_ocupada_cobra(comida.x, comida.y));
}

// Inicializar o jogo
void iniciar_jogo() {
    // Limpar cobra anterior
    while (cobra.cabeca != NULL) {
        Pedaco* temp = cobra.cabeca;
        cobra.cabeca = cobra.cabeca->proximo;
        free(temp);
    }
    
    // Criar cobra inicial no centro
    int centro_x = LARGURA / 2;
    int centro_y = ALTURA / 2;
    
    adicionar_cabeca(centro_x, centro_y);
    cobra.direcao = 4; // Direita
    
    criar_tabuleiro();
    srand(time(NULL));
    gerar_comida();
    
    pontos = 0;
    fim_jogo = 0;
}

// Atualizar tabuleiro com posicoes atuais
void atualizar_tabuleiro() {
    // Limpar area interna
    for (int i = 1; i < ALTURA-1; i++) {
        for (int j = 1; j < LARGURA-1; j++) {
            tabuleiro[i][j] = VAZIO;
        }
    }
    
    // Colocar cobra
    Pedaco* atual = cobra.cabeca;
    int primeira = 1;
    while (atual != NULL) {
        if (primeira) {
            tabuleiro[atual->pos.y][atual->pos.x] = CABECA;
            primeira = 0;
        } else {
            tabuleiro[atual->pos.y][atual->pos.x] = CORPO;
        }
        atual = atual->proximo;
    }
    
    // Colocar comida
    tabuleiro[comida.y][comida.x] = COMIDA;
}

// Mostrar o tabuleiro
void mostrar_tabuleiro() {
    limpar_tela();
    
    for (int i = 0; i < ALTURA; i++) {
        for (int j = 0; j < LARGURA; j++) {
            printf("%c", tabuleiro[i][j]);
        }
        printf("\n");
    }
    
    printf("\nPontos: %d", pontos);
    printf("\nTamanho da cobra: %d", cobra.tamanho);
    printf("\nControles: W(cima) S(baixo) A(esquerda) D(direita) Q(sair)");
}

// Ler tecla do jogador
void ler_entrada() {
    if (_kbhit()) {
        char tecla = _getch();
        switch (tecla) {
            case 'w':
            case 'W':
                if (cobra.direcao != 2) cobra.direcao = 1;
                break;
            case 's':
            case 'S':
                if (cobra.direcao != 1) cobra.direcao = 2;
                break;
            case 'a':
            case 'A':
                if (cobra.direcao != 4) cobra.direcao = 3;
                break;
            case 'd':
            case 'D':
                if (cobra.direcao != 3) cobra.direcao = 4;
                break;
            case 'q':
            case 'Q':
                fim_jogo = 1;
                break;
        }
    }
}

// Mover a cobra
void mover_cobra() {
    if (cobra.cabeca == NULL) return;
    
    // Calcular nova posicao da cabeca
    Posicao nova_pos = cobra.cabeca->pos;
    
    switch (cobra.direcao) {
        case 1: nova_pos.y--; break; // Cima
        case 2: nova_pos.y++; break; // Baixo
        case 3: nova_pos.x--; break; // Esquerda
        case 4: nova_pos.x++; break; // Direita
    }
    
    // Verificar colisao com parede
    if (nova_pos.x <= 0 || nova_pos.x >= LARGURA-1 || 
        nova_pos.y <= 0 || nova_pos.y >= ALTURA-1) {
        fim_jogo = 1;
        return;
    }
    
    // Verificar colisao com proprio corpo
    if (posicao_ocupada_cobra(nova_pos.x, nova_pos.y)) {
        fim_jogo = 1;
        return;
    }
    
    // Verificar se comeu a comida
    int comeu = (nova_pos.x == comida.x && nova_pos.y == comida.y);
    
    // Adicionar nova cabeca
    adicionar_cabeca(nova_pos.x, nova_pos.y);
    
    if (comeu) {
        // Cobra cresceu - nao remove cauda
        pontos += 10;
        gerar_comida();
    } else {
        // Cobra nao cresceu - remove cauda
        remover_cauda();
    }
}

// Loop principal do jogo
void jogar() {
    while (!fim_jogo) {
        ler_entrada();
        mover_cobra();
        atualizar_tabuleiro();
        mostrar_tabuleiro();
        Sleep(300); // Velocidade do jogo
    }
}

// Mostrar fim de jogo
void mostrar_fim_jogo() {
    limpar_tela();
    printf("\n** FIM DE JOGO! **\n");
    printf("Pontos finais: %d\n", pontos);
    printf("Tamanho da cobra: %d\n", cobra.tamanho);
    printf("\nPressione qualquer tecla para sair...\n");
    _getch();
}

// Liberar memoria da cobra
void limpar_cobra() {
    while (cobra.cabeca != NULL) {
        Pedaco* temp = cobra.cabeca;
        cobra.cabeca = cobra.cabeca->proximo;
        free(temp);
    }
}

// Funcao principal
int main() {
    printf("=== JOGO DA COBRINHA ===\n");
    printf("Pressione qualquer tecla para comecar!\n");
    _getch();
    
    iniciar_jogo();
    jogar();
    mostrar_fim_jogo();
    limpar_cobra();
    
    return 0;
}