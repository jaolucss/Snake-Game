#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

// Constantes do jogo
#define BOARD_WIDTH 40
#define BOARD_HEIGHT 20
#define WALL '#'
#define SNAKE_HEAD 'O'
#define SNAKE_BODY 'o'
#define FRUIT '*'
#define EMPTY ' '

// Estrutura para representar uma posição no tabuleiro
typedef struct {
    int x, y;
} Position;

// Estrutura do nó da lista encadeada (segmento da cobra)
typedef struct SnakeNode {
    Position pos;
    struct SnakeNode* next;
} SnakeNode;

// Estrutura da cobra (lista encadeada)
typedef struct {
    SnakeNode* head;  // Cabeça da cobra (primeiro nó)
    SnakeNode* tail;  // Cauda da cobra (último nó)
    int length;       // Tamanho atual da cobra
    int direction;    // Direção atual: 1=cima, 2=baixo, 3=esquerda, 4=direita
} Snake;

// Estrutura da fruta
typedef struct {
    Position pos;
    int active;
} Fruit;

// Variáveis globais
Snake snake;
Fruit fruit;
char board[BOARD_HEIGHT][BOARD_WIDTH];
int score = 0;
int gameOver = 0;

// Função para limpar a tela
void clearScreen() {
    system("cls");
}

// Função para posicionar cursor
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Função para esconder cursor
void hideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

// FUNÇÕES DA LISTA ENCADEADA

// Criar um novo nó da cobra
SnakeNode* createSnakeNode(int x, int y) {
    SnakeNode* newNode = (SnakeNode*)malloc(sizeof(SnakeNode));
    if (newNode == NULL) {
        printf("Erro: Não foi possível alocar memória!\n");
        exit(1);
    }
    newNode->pos.x = x;
    newNode->pos.y = y;
    newNode->next = NULL;
    return newNode;
}

// Adicionar um novo segmento à cabeça da cobra
void addToHead(int x, int y) {
    SnakeNode* newHead = createSnakeNode(x, y);

    if (snake.head == NULL) {
        // Primeira vez - cobra vazia
        snake.head = newHead;
        snake.tail = newHead;
    } else {
        // Adiciona à frente da cabeça atual
        newHead->next = snake.head;
        snake.head = newHead;
    }
    snake.length++;
}

// Remover o último segmento (cauda) da cobra
void removeFromTail() {
    if (snake.head == NULL) return;

    if (snake.head == snake.tail) {
        // Só há um nó
        free(snake.head);
        snake.head = NULL;
        snake.tail = NULL;
    } else {
        // Encontrar o penúltimo nó
        SnakeNode* current = snake.head;
        while (current->next != snake.tail) {
            current = current->next;
        }

        // Remover o último nó
        free(snake.tail);
        snake.tail = current;
        snake.tail->next = NULL;
    }
    snake.length--;
}

// Verificar se uma posição está ocupada pelo corpo da cobra (exceto a cabeça)
int isSnakeBodyPosition(int x, int y) {
    if (snake.head == NULL) return 0;

    SnakeNode* current = snake.head->next; // Pula a cabeça
    while (current != NULL) {
        if (current->pos.x == x && current->pos.y == y) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

// Verificar se uma posição está ocupada por qualquer parte da cobra
int isSnakePosition(int x, int y) {
    SnakeNode* current = snake.head;
    while (current != NULL) {
        if (current->pos.x == x && current->pos.y == y) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

// Liberar toda a memória da cobra
void freeSnake() {
    SnakeNode* current = snake.head;
    while (current != NULL) {
        SnakeNode* temp = current;
        current = current->next;
        free(temp);
    }
    snake.head = NULL;
    snake.tail = NULL;
    snake.length = 0;
}

// FUNÇÕES DO JOGO

// Inicializar o tabuleiro
void initBoard() {
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (i == 0 || i == BOARD_HEIGHT - 1 || j == 0 || j == BOARD_WIDTH - 1) {
                board[i][j] = WALL;
            } else {
                board[i][j] = EMPTY;
            }
        }
    }
}

// Gerar nova fruta em posição aleatória
void generateFruit() {
    do {
        fruit.pos.x = rand() % (BOARD_WIDTH - 2) + 1;
        fruit.pos.y = rand() % (BOARD_HEIGHT - 2) + 1;
    } while (isSnakePosition(fruit.pos.x, fruit.pos.y));

    fruit.active = 1;
}

// Inicializar o jogo
void initGame() {
    // Limpar cobra anterior
    freeSnake();

    // Inicializar cobra no centro do tabuleiro
    int centerX = BOARD_WIDTH / 2;
    int centerY = BOARD_HEIGHT / 2;

    // Criar cobra inicial com 3 segmentos (ordem correta)
    addToHead(centerX - 2, centerY);     // Cauda (primeiro a ser adicionado)
    addToHead(centerX - 1, centerY);     // Meio
    addToHead(centerX, centerY);         // Cabeça (último a ser adicionado)

    snake.direction = 4; // Direita

    // Inicializar tabuleiro
    initBoard();

    // Gerar primeira fruta
    srand(time(NULL));
    generateFruit();

    score = 0;
    gameOver = 0;
    hideCursor();
}

// Atualizar o tabuleiro com as posições atuais
void updateBoard() {
    // Limpar tabuleiro (manter apenas paredes)
    for (int i = 1; i < BOARD_HEIGHT - 1; i++) {
        for (int j = 1; j < BOARD_WIDTH - 1; j++) {
            board[i][j] = EMPTY;
        }
    }

    // Colocar cobra no tabuleiro
    SnakeNode* current = snake.head;
    int isFirst = 1;
    while (current != NULL) {
        if (isFirst) {
            board[current->pos.y][current->pos.x] = SNAKE_HEAD;
            isFirst = 0;
        } else {
            board[current->pos.y][current->pos.x] = SNAKE_BODY;
        }
        current = current->next;
    }

    // Colocar fruta no tabuleiro
    if (fruit.active) {
        board[fruit.pos.y][fruit.pos.x] = FRUIT;
    }
}

// Desenhar o tabuleiro
void drawBoard() {
    gotoxy(0, 0);
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            printf("%c", board[i][j]);
        }
        printf("\n");
    }
}

// Desenhar informações do jogo
void drawGameInfo() {
    gotoxy(BOARD_WIDTH + 2, 2);
    printf("=== SNAKE GAME ===");
    gotoxy(BOARD_WIDTH + 2, 4);
    printf("Score: %d", score);
    gotoxy(BOARD_WIDTH + 2, 5);
    printf("Length: %d", snake.length);
    gotoxy(BOARD_WIDTH + 2, 6);
    printf("Direction: %d", snake.direction);
    gotoxy(BOARD_WIDTH + 2, 8);
    printf("Controls:");
    gotoxy(BOARD_WIDTH + 2, 9);
    printf("W - Cima");
    gotoxy(BOARD_WIDTH + 2, 10);
    printf("S - Baixo");
    gotoxy(BOARD_WIDTH + 2, 11);
    printf("A - Esquerda");
    gotoxy(BOARD_WIDTH + 2, 12);
    printf("D - Direita");
    gotoxy(BOARD_WIDTH + 2, 13);
    printf("Q - Sair");
    gotoxy(BOARD_WIDTH + 2, 15);
    printf("Lista Encadeada:");
    gotoxy(BOARD_WIDTH + 2, 16);
    printf("Head: (%d,%d)", snake.head ? snake.head->pos.x : -1,
           snake.head ? snake.head->pos.y : -1);
    gotoxy(BOARD_WIDTH + 2, 17);
    printf("Tail: (%d,%d)", snake.tail ? snake.tail->pos.x : -1,
           snake.tail ? snake.tail->pos.y : -1);

    // Debug: mostrar todos os segmentos
    gotoxy(BOARD_WIDTH + 2, 19);
    printf("Segmentos:");
    SnakeNode* current = snake.head;
    int count = 0;
    while (current != NULL && count < 5) {
        gotoxy(BOARD_WIDTH + 2, 20 + count);
        printf("[%d]: (%d,%d)", count, current->pos.x, current->pos.y);
        current = current->next;
        count++;
    }
}

// Processar entrada do usuário
void processInput() {
    if (_kbhit()) {
        char key = _getch();
        switch (key) {
            case 'w':
            case 'W':
                if (snake.direction != 2) snake.direction = 1; // Não pode ir para baixo se estava indo para cima
                break;
            case 's':
            case 'S':
                if (snake.direction != 1) snake.direction = 2; // Não pode ir para cima se estava indo para baixo
                break;
            case 'a':
            case 'A':
                if (snake.direction != 4) snake.direction = 3; // Não pode ir para direita se estava indo para esquerda
                break;
            case 'd':
            case 'D':
                if (snake.direction != 3) snake.direction = 4; // Não pode ir para esquerda se estava indo para direita
                break;
            case 'q':
            case 'Q':
                gameOver = 1;
                break;
        }
    }
}

// Mover a cobra
void moveSnake() {
    if (snake.head == NULL) return;

    // Calcular nova posição da cabeça
    Position newPos = snake.head->pos;

    switch (snake.direction) {
        case 1: // Cima
            newPos.y--;
            break;
        case 2: // Baixo
            newPos.y++;
            break;
        case 3: // Esquerda
            newPos.x--;
            break;
        case 4: // Direita
            newPos.x++;
            break;
        default:
            return; // Se direção inválida, não move
    }

    // Verificar colisão com paredes
    if (newPos.x <= 0 || newPos.x >= BOARD_WIDTH - 1 ||
        newPos.y <= 0 || newPos.y >= BOARD_HEIGHT - 1) {
        gameOver = 1;
        return;
    }

    // Verificar colisão com próprio corpo (exceto cabeça)
    if (isSnakeBodyPosition(newPos.x, newPos.y)) {
        gameOver = 1;
        return;
    }

    // Verificar se comeu fruta
    int ateFood = (newPos.x == fruit.pos.x && newPos.y == fruit.pos.y);

    // Adicionar nova cabeça
    addToHead(newPos.x, newPos.y);

    if (ateFood) {
        // Cobra cresceu - não remove a cauda
        score += 10;
        generateFruit();
    } else {
        // Cobra não cresceu - remove a cauda
        removeFromTail();
    }
}

// Função principal do jogo
void gameLoop() {
    // Desenhar estado inicial
    updateBoard();
    clearScreen();
    drawBoard();
    drawGameInfo();

    while (!gameOver) {
        processInput();

        // Só move se tiver direção válida
        if (snake.direction > 0) {
            moveSnake();
        }

        updateBoard();
        clearScreen();
        drawBoard();
        drawGameInfo();

        Sleep(200); // Velocidade do jogo (mais lento para debug)
    }
}

// Exibir tela de game over
void showGameOver() {
    clearScreen();
    printf("\n\n");
    printf("  ╔══════════════════════════════╗\n");
    printf("  ║         GAME OVER!           ║\n");
    printf("  ║                              ║\n");
    printf("  ║  Score Final: %-14d ║\n", score);
    printf("  ║  Tamanho da Cobra: %-9d ║\n", snake.length);
    printf("  ║                              ║\n");
    printf("  ║    Pressione qualquer        ║\n");
    printf("  ║    tecla para sair...        ║\n");
    printf("  ╚══════════════════════════════╝\n");

    _getch();
}

// Função principal
int main() {
    printf("Iniciando Snake com Lista Encadeada...\n");
    printf("Pressione qualquer tecla para começar!\n");
    _getch();

    initGame();
    gameLoop();
    showGameOver();

    // Liberar memória
    freeSnake();

    return 0;
}
