# 🐍 Jogo da Cobrinha em C (Console)  
*Descrição:* Implementação do clássico Snake Game em C para Windows (terminal). Renderização em modo texto com controles via teclado.  

## 📌 Tópicos  
Pré-requisitos | Como executar | Controles | Lógica | Estrutura | Exemplo | Observações | Licença  

## ✅ *Pré-requisitos*  
- *SO:* Windows  
- *Compilador:* GCC/MinGW/MSVC  
- *Bibliotecas:* conio.h, windows.h, stdio.h, stdlib.h, time.h  

## ▶️ *Execução*  
1. Compile: gcc cobra.c -o cobra  
2. Execute: ./cobra  
⚠️ Incompatível com Linux/macOS sem adaptações (depende de conio.h).  

## 🎮 *Controles*  
W (cima) | S (baixo) | A (esquerda) | D (direita) | Q (sair)  

## 🧠 *Lógica*  
- Cobra como lista encadeada (cresce ao comer).  
- Comida aleatória em tabuleiro[][].  
- *Fim de jogo:* Colisão (paredes/corpo) ou tecla Q.  

## 🗂️ *Estrutura*  
- *Structs:* Posicao (x,y), Pedaco (nó), Cobra (cabeça/tamanho/direção).  
- *Funções-chave:* iniciar_jogo(), mover_cobra(), gerar_comida(), mostrar_tabuleiro().  

## 📸 *Exemplo*  
```shell
##############################
#           Ooo*            #
##############################
Pontos: 20 | Tamanho: 3 | Controles: W/A/S/D/Q