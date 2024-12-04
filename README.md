<div align="center"><img src="images/readme/cover.png"></div>

<h3 align="center">Meteor Striker</h3>

<p align="center">Meteor Striker é um jogo multiplayer para a placa DE1-SoC desenvolvido em C. Um driver em Assembly, criado para uma GPU customizada desenvolvida por outro autor e instalada na FPGA, controla a exibição de imagens na saída VGA. O jogo é controlado via mouse e acelerômetro. Dois jogadores interceptam meteoros e acumulam pontos em uma tela dividida.</p>

<div align="center">
  [Sobre o projeto](#sobre-o-projeto) • [Instalação](#instalação) • [Solução geral](#solução-geral) • [O jogo](#o-jogo) • [Testes](#testes)
</div>

## Sobre o projeto

Durante a evolução dos videogames, jogos como <i>Breakout Story</i> demonstraram como gráficos simples e mecânicas interativas podem criar experiências memoráveis. Inspirado nesse conceito, <i>Meteor Striker</i> é um jogo bidimensional multiplayer, <i>desenvolvido em Linguagem C</i>, onde dois jogadores competem interceptando meteoros. Desenvolvido para a <i>DE1-SoC</i>, o projeto utiliza uma <i>GPU customizada criada por Gabriel Sá Barreto Alves</i>, ex-aluno do curso de Engenharia de Computação da UEFS, como parte de seu TCC. Essa GPU, instalada na FPGA, é controlada por <i>um driver em Assembly desenvolvido especialmente para o jogo</i>. Com controles via acelerômetro e mouse, o projeto explora a interação entre hardware e software, destacando a integração entre <i>HPS</i> e <i>FPGA</i> e o uso de representações gráficas avançadas para criar uma experiência imersiva e competitiva.

<details>
  <summary><b>Requisitos</b></summary>

### Requisitos
O problema a ser desenvolvido no Kit de desenvolvimento DE1-SoC deve atender às seguintes restrições:

- O código deve ser escrito em linguagem C e a biblioteca do Processador Gráfico em Assembly;
- O sistema só poderá utilizar os componentes disponíveis na placa DE1-SoC;
- Deverá utilizar todas as funções implementadas na biblioteca do Processador Gráfico e, no mínimo, um novo sprite deve ser colocado na memória e utilizado no jogo;
- Os dois jogadores devem jogar simultaneamente utilizando obrigatoriamente o acelerômetro, e adicionalmente o mouse;
- A variação da velocidade no movimento deve ser refletida na ação do ator do jogo, como no exemplo do jogo Breakout, onde a barra se move com velocidade maior se o movimento do mouse for brusco;
- Informações do jogo (placar, vidas, etc.) devem ser exibidas na tela do jogo;
- O jogo deve permitir ações do usuário através dos botões da DE1-SoC, incluindo pausa, retorno, reinício e término do jogo:
  - O usuário poderá parar e reiniciar o jogo a qualquer momento;
  - O usuário poderá sair do jogo a qualquer momento;
- Pelo menos dois elementos passivos do jogo deverão se mover de maneira independente;
- Jogos de tabuleiro e/ou turno (xadrez, dama, etc.), tetris e jogo da forca e da velha estão vetados, e cada grupo deverá desenvolver um jogo diferente dos demais.

</details>

<details>
  <summary><h2>Índice</h2></summary>

- [Instalação](#instalação)
  - [Pré-requisitos](#pré-requisitos)
- [Softwares utilizados](#softwares-utilizados)
  - [Linguagem C](#linguagem-c)
  - [Linguagem Assembly](#linguagem-assembly)
  - [Compilador GCC](#compilador-gcc)
  - [VS Code](#vs-code)
- [DE1-SoC](#kit-de-desenvolvimento-de1-soc)
  - [Visão geral da DE1-SoC](#visão-geral-da-de1-soc)
  - [Sistema computacional da placa](#sistema-computacional-de1-soc)
- [Processador gráfico desenvolvido por Gabriel Sá Barreto Alves](#processador-gráfico-desenvolvido-por-gabriel-sá-barreto-alves)
  - [Como funciona?](#como-funciona)
- [Dispositivos de Entrada Usados](#dispositivos-de-entrada-usados)
  - [Mouse USB](#mouse-usb)
  - [Biblioteca para Acelerômetro ADXL345 na DE1-SoC](#biblioteca-para-acelerômetro-adxl345-na-de1-soc)
  - [Botões push](#botões-do-tipo-push)
- [O Jogo](#o-jogo)
  - [Interface com o jogador](#interface-com-o-jogador)
  - [Elementos do jogo](#elementos-do-jogo)
  - [Como jogar](#como-jogar)   
- [Solução geral](#solução-geral)
- [Gerenciamento da GPU](#gerenciamento-do-processador-gráfico)
  - [Mapeamento da Memória FPGA](#mapeamento-da-memória-fpga)
  - [Comunicação com as Filas FIFO A e FIFO B](#comunicação-com-as-filas-fifo-a-e-fifo-b)
  - [Representação Gráfica](#representação-gráfica)
  - [Funções Detalhadas](#funções-detalhadas)
  - [Pontos Relevantes](#pontos-relevantes)
- [Threads e kthreads](#threads-e-kthreads)
  - [Threads](#threads)
  - [Kthreads - Kernel Threads](#kthreads---kernel-threads)
  - [Modelo de Threads](#modelo-de-threads)
  - [Vantagens das Threads](#vantagens-das-threads)
  - [Desafios](#desafios) 
- [Gerenciamento dos pushbuttons](#gerenciamento-dos-pushbuttons)
  - [Driver dos botões](#driver-dos-botões)
  - [Biblioteca dos botões](#biblioteca-dos-botões)
  - [Inicialização dos Botões](#inicialização-dos-botões)
  - [Leitura do Estado dos Botões](#leitura-do-estado-dos-botões)
  - [Fechamento e Liberação](#fechamento-e-liberação)
  - [Exemplo de Utilização](#exemplo-de-utilização)
  - [Notas Importantes](#notas-importantes)
- [Algoritmos do Jogo](#algoritmos-do-jogo)
  - [Criação de Sprites](#criação-de-sprites)
  - [Movimento e ações do jogador](#movimento-e-ações-do-jogador)
  - [Geração dos meteoros](#geração-dos-meteoros)
  - [Detecção de colisão](#detecção-de-colisão)
  - [Movimentação de elementos](#movimentação-de-elementos)
    - [Movimentação do plano de fundo](#movimentação-do-plano-de-fundo)
    - [Movimentação dos meteoros](#movimentação-dos-meteoros)
  - [Fluxo do jogo](#fluxo-do-jogo)
    - [Controle do fluxo da execução das threads](#controle-do-fluxo-da-execução-das-threads)
      - [Criação e finalização das threads](#criação-e-finalização-das-threads)
- [Testes](#testes)
  - [Polling dos botões](#polling-dos-botões)
  - [Jogo](#jogo)
- [Conclusão](#conclusão)

</details>



## Instalação

<details>
  <summary><h3>Pré-requisitos</h3></summary>

### Pré-requisitos

- Possuir conexão com internet;
- Possuir instalado o compilador GCC;
- Possuir instalado o Git;
- Utilizar uma placa de desenvolvimento FPGA DE1-SoC;
- Possuir o processador gráfico desenvolvido por Gabriel Sá Barreto Alves na FPGA;
- Possuir um monitor conectado à placa por meio da saída VGA;
- Possuir um mouse USB conectado à placa.

</details>

<details>
  <summary><h3>Compilação e Execução do jogo</h3></summary>

#### 1. Clonar o repositório
Abra o terminal do seu dispositivo e execute o seguinte comando:
```bash
git clone https://github.com/alexsami-lopes/Digital-Systems-Meteor-Striker.git
```
**Transfira os arquivos do projeto para a placa DE1-SoC.**

#### 2. Na placa, acessar a pasta */Digital-Systems-Meteor-Striker-main* e compilar o jogo
Para acessar a pasta e compilar o jogo, execute os seguintes comandos:
```bash
cd /Digital-Systems-Meteor-Striker-main
make
```

#### 3. Executar o jogo
Na placa, dentro da pasta */Digital-Systems-Meteor-Striker-main*, execute o comando:
```bash
sudo ./meteor_striker
```
Ou ainda:
```bash
make execute
```

</details>



## Softwares utilizados

<details>
  <summary><b>Linguagem C</b></summary>

### Linguagem C

A linguagem C é descrita como "uma linguagem de propósito geral com recursos de economia de expressão, controle de fluxo moderno e estruturas de dados, e um rico conjunto de operadores" (Kernighan & Ritchie, 1978). Embora não seja uma linguagem de "alto nível", sua generalidade a torna eficaz para muitas tarefas.

</details>

<details>
  <summary><b>Linguagem Assembly</b></summary>

### Linguagem Assembly

A linguagem Assembly é uma linguagem de programação de baixo nível que representa uma abstração mnemônica direta para as instruções de máquina de um processador específico. Ela permite aos programadores interagir diretamente com o hardware de forma mais próxima do que as linguagens de alto nível.

</details>

<details>
  <summary><b>Compilador GCC</b></summary>

### Compilador GCC

O GCC (GNU Compiler Collection) é uma coleção de compiladores de código aberto amplamente usada para sistemas Unix-like e outros sistemas operacionais.

#### Características principais:
- **Multiplataforma**: Suporte a diversos sistemas operacionais e arquiteturas de processadores.
- **Linguagens Suportadas**: C, C++, Objective-C, Fortran, Ada, Go.
- **Otimização de Código**: Oferece múltiplos níveis de otimização de desempenho.

Exemplo de comando de compilação:
```bash
gcc -Wall -std=gnu99 -Wextra -pthread programa.c -o programa
```

</details>

<details>
  <summary><b>VS Code</b></summary>

### VS Code

O Visual Studio Code (VS Code) é um editor de código-fonte desenvolvido pela Microsoft, gratuito e de código aberto. Ele é multiplataforma e oferece suporte a diversas linguagens de programação, sendo amplamente utilizado por desenvolvedores.

</details>



## Kit de desenvolvimento DE1-SoC

<details>
  <summary><b>Visão geral da DE1-SoC</b></summary>

### Visão geral da DE1-SoC

A DE1-SoC é uma plataforma de desenvolvimento de sistemas embarcados projetada pela Terasic, baseada em um System on Chip (SoC) que integra um processador ARM Cortex-A9 dual-core e um FPGA Altera Cyclone V.

#### Características principais:
- **Processador**: ARM Cortex-A9 dual-core.
- **FPGA**: Altera Cyclone V (5CSEMA5F31C6).
- **Memória**: 
  - 1 GB DDR3.
  - 256 MB SDRAM.
- **Conectividade**:
  - Ethernet.
  - USB.
  - Entrada de áudio/vídeo.
  - Interface GPIO.

#### Recursos de Hardware:
- Displays de 7 segmentos.
- Chaves e botões programáveis.
- LEDs.
- Conectores de expansão.
- Suporte para desenvolvimento de sistemas embarcados e prototipagem de circuitos digitais.

<div align="center">
  <figure>  
    <img src="images/readme/de1_soc_img.png" width="600px">
    <figcaption>
      <p align="center">
        [**Figura 1** - Manual de Usuário da Placa DE1-SoC](https://fpgacademy.org/Downloads/DE1_SoC_User_Manual.pdf)
      </p>
    </figcaption>
  </figure>
</div>

</details>

<details>
  <summary><b>Sistema computacional DE1-SoC</b></summary>

### Sistema computacional DE1-SoC

<div align="center">
  <figure>
    <img src="images/readme/diagrama_de_blocos_de1_soc.png" width="500px">
    <figcaption>
      <p align="center">
        [**Figura 2** - Diagrama de Blocos da DE1-SoC](https://fpgacademy.org/Downloads/DE1_SoC_User_Manual.pdf)
      </p>
    </figcaption>
  </figure>
</div>

O diagrama de blocos do sistema computacional, apresentado na figura 2, explicita os componentes do Cyclone® V da Intel®, bem como suas conexões. O HPS inclui um processador ARM® Cortex-A9 MPCore™ de 2 núcleos com uma distribuição Linux embarcada destinada a processamentos de propósito geral, além da memória DDR3 e dos dispositivos periféricos. Já a FPGA possibilita uma variedade de implementações através da programação dos blocos lógicos.

> A comunicação bidirecional entre o HPS e a FPGA se dá por meio das *FPGA bridges*. No sentido HPS-FPGA, todos os dispositivos de entrada e saída (E/S) conectados à FPGA são acessíveis ao processador através do mapeamento de memória. As informações sobre o **endereçamento original** dos periféricos estão disponíveis na [documentação da placa](https://fpgacademy.org/index.html).

</details>


## Processador Gráfico Desenvolvido por Gabriel Sá Barreto Alves

O processador gráfico desenvolvido por Gabriel Sá Barreto Alves permite mover e controlar elementos em um monitor VGA com resolução de 640×480 pixels. Este processador possibilita o desenho de dois tipos de *polígonos* convexos (quadrado e triângulo) e uma quantidade determinada de *sprites*. Durante o desenvolvimento, Gabriel utilizou como unidade de processamento principal o NIOS II, embarcado na mesma FPGA que o processador gráfico. A figura 3 ilustra a arquitetura desenvolvida.

<details>
  <summary><b>Como funciona?</b></summary>

### Como funciona?

O dispositivo gráfico oferece um conjunto de instruções para gerenciar polígonos, sprites e o background. Mais detalhes sobre essas instruções podem ser encontrados [aqui](https://drive.google.com/file/d/1MlIlpB9TSnoPGEMkocr36EH9-CFz8psO/view). Essas instruções são transmitidas do HPS para o hardware na FPGA por meio dos barramentos *dataA* (responsável pelos dados de acesso à memória e registradores, além dos *opcodes*) e *dataB* (que recebe dados customizáveis como cores e posições).

Os dados nos barramentos são gravados nas filas de instruções A e B (correspondentes aos barramentos *dataA* e *dataB*). As FIFOs armazenam até 16 palavras de 32 bits cada. Para garantir a escrita correta nas filas, foram usados os seguintes sinais de controle:
- *wr_reg (input)*: sinal de escrita nas FIFOs.
- *wr_full (output)*: sinal que indica que as filas estão cheias.

A escrita nas FIFOs pode ocorrer apenas quando a fila não está cheia. Durante esse período, o sinal *wr_reg* pode ser ativado ao armazenar 1 no endereço *#0xc0 a partir do endereço base mapeado*. A escrita pode continuar até que o sinal *wr_full* seja ativado (indicando filas cheias). O código abaixo ilustra esse comportamento:

```asm
check_fifo:            @ Função que verifica se a FIFO está cheia
  sub sp, sp, #8       @ Reservando 8 bytes na pilha
  str r0, [sp, #4]
  str r1, [sp, #0]
  
  ldr r0, =mapped_address   @ Endereço mapeado
  ldr r0, [r0]
checking:                   @ Label para verificar se a FIFO está cheia
  ldr r1, [r0, #0xb0]       @ Lendo o registrador de status da FIFO
  CMP r1, #1                @ Comparando o status da FIFO com 1
  beq checking              @ Se a FIFO estiver cheia, volta para o label checking

  ldr r0, [sp, #4]
  ldr r1, [sp, #0]
  add sp, sp, #8 
  
  bx lr
```

<div align="center">
  <figure>
    <img src="images/readme/GPU-image.png" width="600px">
    <figcaption>
      <p align="center">
        [**Figura 3** - Representação da arquitetura do processador gráfico](https://drive.google.com/file/d/1ogg3s2YPTvnRmb4YHDygRJ0bcZv8vG-N/view)
      </p>
    </figcaption>
  </figure>
</div>

</details>



## Dispositivos de Entrada Usados

Os dispositivos de entrada usados foram:
- 4 botões do tipo push;
- Um mouse conectado via USB para controlar o Player 1;
- O acelerômetro foi usado para controlar o Player 2.

<details>
  <summary><b>Mouse USB</b></summary>

### Mouse USB

Os conectores USB da DE1-SoC são gerenciados por um hub controlador que se comunica diretamente com o HPS. Foi conectado um mouse à primeira porta USB host da placa, conforme ilustrado na Figura 4.

<div align="center">
  <figure>
    <img src="images/readme/videos/mouse_demo.gif" height="400px">
    <figcaption>
      <p align="center">
        **Figura 4** - Mouse USB usado
      </p>
    </figcaption>
  </figure>
</div>

</details>

<details>
  <summary><b>Biblioteca para Acelerômetro ADXL345 na DE1-SoC</b></summary>

### Biblioteca para Acelerômetro ADXL345 na DE1-SoC

A biblioteca fornecida contém funções essenciais para configurar e operar o acelerômetro ADXL345 na plataforma DE1-SoC. As principais funções são:

- **Leitura e escrita de registros do ADXL345**:
  - `ADXL345_REG_READ(uint8_t, uint8_t *)`: Lê um byte de um registro específico.
  - `ADXL345_REG_WRITE(uint8_t, uint8_t)`: Escreve um valor em um registro.
  - `ADXL345_REG_MULTI_READ(uint8_t, uint8_t[], uint8_t)`: Lê múltiplos bytes consecutivos de um endereço.

- **Inicialização e leitura de dados**:
  - `ADXL345_init(uint8_t)`: Configura registros como taxa de amostragem e modos de operação.
  - `ADXL345_XYZ_Read(int16_t *)`: Lê valores de aceleração nos eixos X, Y e Z.

</details>

<details>
  <summary><b>Botões do tipo push</b></summary>

### Botões do tipo push

A DE1-SoC oferece quatro botões *push* conectados diretamente à FPGA e à GPU. Esses botões estão associados a um registrador de 32 bits, sendo os 4 bits menos significativos utilizados para armazenar os estados dos botões:
- **KEY0**: bit 0.
- **KEY1**: bit 1.
- **KEY2**: bit 2.
- **KEY3**: bit 3.

Quando um botão é pressionado, o bit correspondente é setado para 0. Quando liberado, o bit retorna a 1.

<div align="center">
  <figure>
    <img src="images/readme/push_buttons_img.png" width="600px">
    <figcaption>
      <p align="center">
        [**Figura 5** - Circuito dos botões do tipo push](https://fpgacademy.org/Downloads/DE1_SoC_User_Manual.pdf)
      </p>
    </figcaption>
  </figure>
</div>

</details>


## O Jogo

<details>
  <summary><b>Interface do Jogo</b></summary>

### Interface do Jogo

A seguir, são apresentadas as interfaces exibidas para o jogador no monitor VGA e suas possíveis transições, bem como os cenários em que o uso dos botões interfere no estado do jogo.

- Tela inicial:
<div align="center">
  <figure>
    <img src="images/readme/cover.png" width="600px">
    <figcaption>
      **Figura 6** - Tela inicial
    </figcaption>
  </figure>
</div>

- Tela do menu:
<div align="center">
  <figure>
    <img src="images/readme/screenshots/menu_pic.jpg" width="600px">
    <figcaption>
      **Figura 7** - Tela do menu em execução
    </figcaption>
  </figure>
</div>

- Partida em execução:
<div align="center">
  <figure>
    <img src="images/readme/videos/game_demo.gif" width="600px">
    <figcaption>
      **Figura 8** - Tela de uma partida em execução
    </figcaption>
  </figure>
</div>

- Tela de pausa:
<div align="center">
  <figure>
    <img src="images/readme/screenshots/pause_pic.jpg" width="600px">
    <figcaption>
      **Figura 9** - Tela de pausa
    </figcaption>
  </figure>
</div>

- Tela de vitória:
<div align="center">
  <figure>
    <img src="images/readme/trophy_img.png" width="600px">
    <figcaption>
      **Figura 10** - Tela de vitória
    </figcaption>
  </figure>
</div>

</details>

<details>
  <summary><b>Elementos do Jogo</b></summary>

### Elementos do Jogo

O jogo utiliza sprites para representar elementos como meteoros e carros. A Figura 15 ilustra alguns exemplos desses sprites.

<div align="center">
  <figure>
    <img src="images/readme/sprite_de1-soc_img.png" width="600px">
    <figcaption>
      **Figura 11** - Sprite da placa DE1-SoC
    </figcaption>
  </figure>
</div>

</details>

<details>
  <summary><b>Como Jogar</b></summary>

### Como Jogar

Dois jogadores competem para eliminar 10 meteoros antes do adversário. Cada jogador protege metade da tela, e seus alvos eliminam meteoros ao colidir com eles. Pontos são ganhos ao acertar meteoros e perdidos caso meteoros atinjam a parte inferior da tela. O jogador pode usar botões para pausar, reiniciar ou terminar o jogo.

</details>


## Solução Geral

<div align="center">
  <figure>
    <img src="images/readme/logica_do_jogo_img.png" width="600px">
    <figcaption>
      **Figura 12** - Esquema em blocos da solução geral
    </figcaption>
  </figure>
</div>

A aplicação integra módulos para o gerenciamento de hardware (botões, mouse, acelerômetro) e lógica do jogo. Threads específicas são responsáveis por ações como movimentação dos elementos e detecção de colisões.



## Testes

<details>
  <summary><b>Polling dos Botões</b></summary>

### Polling dos Botões

Foi utilizado *printf* para verificar se os botões estavam funcionando corretamente, com os estados sendo lidos e exibidos conforme esperado.

</details>

<details>
  <summary><b>Jogo</b></summary>

### Jogo

Foram realizados testes para verificar:
- Detecção e resposta a colisões.
- Atualização de pontuações.
- Funcionamento do menu e das animações.
- Controle dos alvos por mouse e acelerômetro.

<div align="center">
  <figure>
    <img src="images/readme/videos/game_demo.gif">
    <figcaption>
      **Figura 13** - Atualização da pontuação ao destruir um meteoro
    </figcaption>
  </figure>
</div>

</details>


## Conclusão

O projeto atendeu a todos os requisitos, mas há espaço para melhorias, como:
- Aprimorar as animações para maior imersão.
- Adicionar novos níveis para aumentar o desafio.
- Explorar mais funcionalidades da GPU customizada.



