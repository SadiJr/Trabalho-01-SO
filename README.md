# 1º Trabalho da disciplina de Sistemas Operacionais

O objetivo deste trabalho é que o estudante aplique os conceitos de threads, exclusão mútua e coordenação de processos por meio do projeto e implementação de um jogo simples.

## Descrição do Jogo

O jogo consiste de um tabuleiro 11 X 11 casas (posições). Nesse tabuleiro, inicialmente são distribuídos  cinco tokens (fichas) aleatoriamente em qualquer uma das 121 casas. Além dos tokens, há um cursor posicionado inicialmente no meio do tabuleiro, conforme a figura abaixo:

![alt text](https://github.com/SadiJr/Trabalho-01-SO/blob/master/tabuleiro.png)

Após disparado o jogo, os tokens movimentam-se simultaneamente para qualquer posição do tabuleiro. O objetivo do jogo é que o jogador elimine todos os tokens dentro de um tempo máximo t. Para eliminar um token, o jogador tem que posicionar  o cursor na casa na qual o token encontra-se. 

O jogo tem três níveis de dificuldade: fácil, intermediário e difícil. No fácil, os tokens movem-se mais lentamente e o tempo t é maior; no difícil, os tokens movem-se mais rapidamente; no nível intermediário, a velocidade de movimentação fica entre as velocidades dos níveis e difícil.

Cada token deve ser uma pthread, mas pode existir  mais do que cinco pthreads.

O cursor poderá ser movimentado via setas ou via mouse.
