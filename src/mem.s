@  .section .data
@mapped_address: .space 4
@devmem: .asciz "/dev/mem"
@fpga_bridge: .word 0xff200             @ Endereço do bridge FPGA

.section .data
devmem:          .asciz "/dev/mem"      @ Caminho para o arquivo /dev/mem
fpga_bridge:     .word 0xff200 @ endereco da ponte
HW_REGS_SPAN:          .word 0x100       @ tamanho da pagina do mapeamento (256 kb)
mapped_address:  .space   4
ADRESS_FD:      .space   4

  .section .text
  .global open_devmem
  .type open_devmem, %function
  .global close_devmen
  .type close_devmen, %function
  .global bg_block
  .type bg_block, %function
  .global poligono
  .type poligono, %function
  .global check_fifo
  .type check_fifo, %function
  .global bg_color
  .type bg_color, %function
  .global set_sprite2
  .type set_sprite2, %function
  .global acess_btn
  .type acess_btn, %function
  .global acess_display
  .type acess_display, %function
  .global	sendInstruction
  .type	sendInstruction, %function
  .global set_sprite_pixel_assembly
  .type	set_sprite_pixel_assembly, %function

@ Constantes
.equ data_a, 0x80
.equ data_b, 0x70
.equ wrreg, 0xc0

open_devmem:
    @ Abrir /dev/mem

    sub sp, sp, #32
    str lr, [sp, #28]
    str r7, [sp, #24]
    str r5, [sp, #20]
    str r4, [sp, #16]
    str r3, [sp, #12]
    str r2, [sp, #8]
    str r1, [sp, #4]
    str r0, [sp, #0]


    @abre o arquivo /dev/mem
    MOV r7, #5          @ syscall open
    LDR r0, =devmem     @ caminho do arquivo
    MOV r1, #2          @ para leitura e escrita 
    MOV r2, #0          @ sem flags
    SWI 0               @ chama o sistema para executar

    ldr r1, =ADRESS_FD
    str r0, [r1]
    mov r4, r0              @guarda em r4

    @ configurar o mmap
    mov r7, #192        @ syscall do mmap2
    mov r0, #0          @ para deixar o kernel decidir o enderço virtual
    ldr r1, =HW_REGS_SPAN @ tamanho da pagina
    ldr r1, [r1]
    mov r2, #3          @ leitura/escrita
    mov r3, #1          @ compartilhado com outros processos
    ldr r5, =fpga_bridge @carrega o endereço base da FPGA 
    ldr r5, [r5]        @ carrega o valor real do enderço da FPGA
    svc 0               @ kernel é chamado para executar a syscall

    ldr r1, =mapped_address  @endereco e carregado aqui
    str r0, [r1]

    ldr lr, [sp, #28]
    ldr r11, [sp, #24]
    ldr r7, [sp, #20]
    ldr r5, [sp, #16]
    ldr r3, [sp, #12]
    ldr r2, [sp, #8]
    ldr r1, [sp, #4]
    ldr r0, [sp, #0]
    add sp, sp, #32

    bx lr

@-----------------------------------------------------------------------
close_devmen:
    @ Fechar /dev/mem

    @reservando na pilha os resgitradores que vai usar
    sub sp, sp, #12
    str r0, [sp, #8]
    str r1, [sp, #4]
    str r7, [sp, #0]

    ldr r0, =mapped_address  @ Endereço mapeado
    ldr r0, [r0]
    mov r1, #4096 @ tamanho da página mapeada
    mov r7, #91   @ system call: munmap
    svc 0
    
    ldr r0, =devmem  @ /dev/mem
    ldr r0, [r0]
    mov r7, #6   @ system call: close
    svc 0
    
    ldr r0, [sp, #8]
    ldr r1, [sp, #4]
    ldr r7, [sp, #0]

    add sp, sp, #12

    bx lr

@------------------------------------------------------------------------
bg_block:
    @ r0-> parametro posição  ( (linhas * 80) +  coluna)
    @ r1-> parametro cor

    @reservando na pilha os resgitradores que vai usar
    sub sp, sp, #28  @reservando 28 bytes na pilha
    str lr, [sp, #24]
    str r1, [sp, #20]
    str r2, [sp, #16]
    str r4, [sp, #12]
    str r5, [sp, #8]
    str r6, [sp, #4]
    str r0, [sp, #0]


    ldr r4, =mapped_address  @ Endereço mapeado
    ldr r4, [r4]

    @DATAA
    mov r2, #2      @opcode
    lsl r0, #4       @deslocamento
    add r2, r2, r0    @opcode + posição

    mov r5, #0
    mov r6, #1

    str r5, [r4, #0xc0]   @desabilitando a escrita no wrreg
    str r2, [r4, #0x80]   @passando o opcode e a posição para dataA
    str r1, [r4, #0x70]   @passando a cor para dataB
    bl check_fifo         @verifica se o fifo está cheio entes de ativar o wrreg
    str r6, [r4, #0xc0]   @ativando a escrita no wrreg
    str r5, [r4, #0xc0]   @desativando a escrita no wrreg

    @carregando os registradores que foram salvos na pilha
    ldr lr, [sp, #24]
    ldr r1, [sp, #20]
    ldr r2, [sp, #16]
    ldr r4, [sp, #12]
    ldr r5, [sp, #8]
    ldr r6, [sp, #4]
    ldr r0, [sp, #0]

    add sp, sp, #24  @liberando a pilha

    bx lr

@------------------------------------------------------------------------
check_fifo: @função que verifica se a fifo está cheia
  sub sp, sp, #8    @reservando 8 bytes na pilha
  str r0, [sp, #4]
  str r1, [sp, #0]
  
  ldr r0, =mapped_address   @ Endereço mapeado
  ldr r0, [r0]
checking:                   @label para verificar se a fifo está cheia
  ldr r1, [r0, #0xb0]       @lendo o registrador de status da fifo
  CMP r1, #1                @comparando o status da fifo com 1
  beq checking              @se a fifo estiver cheia, volta para a label checking

  ldr r0, [sp, #4]
  ldr r1, [sp, #0]
  add sp, sp, #8 
  
  bx lr

@--------------------------------------------------------------------------

poligono:  @função que desenha um poligono
  @ r0 -> cor    (rgb)
  @ r1 -> tamanho (0 a 15)
  @ r2 -> forma   (0 -> quadrado, 1 -> triângulo)
  @ r3 -> endereço  (0 a 15)
  @ r4 -> posiçãoX  
  @ r5 -> posiçãoy

    sub sp, sp, #40       @reservando 40 bytes na pilha
    str lr, [sp, #36]
    str r11, [sp, #32] 
    str r7, [sp, #28] 
    str r6, [sp, #24] 
    str r5, [sp, #20]
    str r4, [sp, #16]
    str r3, [sp, #12] 
    str r2, [sp, #8]
    str r1, [sp, #4]
    str r0, [sp, #0]
    ldr r4, [sp, #40]
    ldr r5, [sp, #44]

  ldr r11, =mapped_address        @ Endereço mapeado
  ldr r11, [r11]

  @desabilitando a escrição no wrreg
  mov r9, #0
  str r9, [r11, #wrreg]

  @DATAA
  mov r8, #3  @opcode
  lsl r3, #4
  add r3, r3, r8    @opcode + endereço

  str r3, [r11, #data_a]    @passando o opcode e o endereço para dataA

  @DATAB
  mov r6, r4 @x
  mov r7, r5 @y

  @deslocamento
  lsl r2, #31
  lsl r0, #22
  lsl r1, #18
  lsl r6, #9      
  
  @calculando o data B
  add r2, r2, r0
  add r2, r2, r1
  add r2, r2, r6
  add r2, r2, r7

  str r2, [r11, #data_b]    @passando o dataB

  mov r9, #1                @habilitando a escrita no wrreg 
  str r9, [r11, #wrreg]
  mov r9, #0                @desabilitando a escrita no wrreg
  str r9, [r11, #wrreg]

    ldr lr, [sp, #36]
    ldr r11, [sp, #32] 
    ldr r7, [sp, #28] 
    ldr r6, [sp, #24] 
    ldr r9, [sp, #20]
    ldr r8, [sp, #16]
    ldr r3, [sp, #12] 
    ldr r2, [sp, #8]
    ldr r1, [sp, #4]
    ldr r0, [sp, #0]

    add sp, sp, #40        @liberando a pilha

  bx lr

@---------------------------------------------------------------------
bg_color:   @função que seta a cor de fundo de background
    @ r0 : cor

    sub sp, sp, #16       @reservando 16 bytes na pilha
    str lr, [sp, #12]
    str r4, [sp, #8]
    str r1, [sp, #4]
    str r0, [sp, #0]


    ldr r4, =mapped_address         @ Endereço mapeado
    ldr r4, [r4]

    mov r1, r0                      @ Carrega a cor no reg
    str r1, [r4, #0x70]               @ Passa a cor para o registrador dataB

    mov r1, #0                          @ Configura opcode WBR + registrador 0 (background)
    str r1, [r4, #0x80]               @ Passa o opcode e o registrador para dataA

    mov r1, #1                         @ Sinal de início de escrita no WRREG
    bl check_fifo                      @checa de a pilha está cheia
    str r1, [r4, #0xc0]                @ Ativa a escrita no registrador WRREG
    mov r1, #0                          @ Desativa o sinal de escrita
    str r1, [r4, #0xc0]                @ Confirma a finalização da escrita

    ldr lr, [sp, #12]
    ldr r4, [sp, #8]
    ldr r1, [sp, #4]
    ldr r0, [sp, #0]
    add sp, sp, #16                   @liberando a pilha

    bx lr                             @retorna 

@----------------------------------------------------------------
set_sprite2:          @ Função que seta o sprite
    @ Parâmetros:
    @ r0 -> offset (Sprite)
    @ r1 -> px      (posição x)
    @ r2 -> py      (posição y)
    @ r3 -> sp      (1 -> habilita, 0 -> desabilita)
    @ r4 -> registrador (1 a 15)

    @ Reserva espaço na pilha para salvar registradores
    sub sp, sp, #44         
    str lr, [sp, #40]        @ Salva o valor do link register
    str r10, [sp, #36] 
    str r9, [sp, #32] 
    str r7, [sp, #28] 
    str r6, [sp, #24] 
    str r5, [sp, #20]
    str r4, [sp, #16]        @ Salva o r4 (registrador do data_register)
    str r3, [sp, #12] 
    str r2, [sp, #8]
    str r1, [sp, #4]
    str r0, [sp, #0]

    @ Cálculo do DataA
    mov r9, #0               @ opcode inicial
    lsl r4, r4, #4           @ desloca o registrador (data_register)
    add r4, r4, r9           @ opcode + registrador

    @ Cálculo do DataB
    mov r5, #0               @ Inicializa r5
    mov r6, r0               @ r6 <- offset
    mov r7, r3               @ r7 <- sp (1 -> habilita, 0 -> desabilita)

    lsl r7, r7, #29          @ desloca sp para formar parte do data_b
    lsl r1, r1, #19          @ desloca coord_x
    lsl r2, r2, #9           @ desloca coord_y

    add r5, r7, r1           @ soma os deslocamentos
    add r5, r5, r2
    add r5, r5, r6           @ adiciona o offset ao resultado

    @ Endereço mapeado e configuração de registradores
    ldr r10, =mapped_address @ Carrega endereço base mapeado
    ldr r10, [r10]           @ Desreferencia o endereço

    mov r7, #0               @ Desabilita escrita no wrreg
    str r7, [r10, #wrreg]

    str r4, [r10, #data_a]   @ Escreve o DataA (opcode + registrador)
    str r5, [r10, #data_b]   @ Escreve o DataB (posições e offset)

    bl check_fifo              @ Verifica se o FIFO está pronto (implementação separada)

    mov r7, #1               @ Habilita escrita no wrreg
    str r7, [r10, #wrreg]
    mov r7, #0               @ Desabilita escrita no wrreg
    str r7, [r10, #wrreg]

    @ Restaura os valores dos registradores salvos na pilha
    ldr lr, [sp, #40]
    ldr r10, [sp, #36] 
    ldr r9, [sp, #32] 
    ldr r7, [sp, #28] 
    ldr r6, [sp, #24] 
    ldr r5, [sp, #20]
    ldr r4, [sp, #16]
    ldr r3, [sp, #12] 
    ldr r2, [sp, #8]
    ldr r1, [sp, #4]
    ldr r0, [sp, #0]

    add sp, sp, #44          @ Libera a pilha

    bx lr                    @ Retorna da função


@-------------------------------------------------------------------------
sendInstruction:
    @ r0 -> data_a
    @ r1 -> data_b

    sub sp, sp, #40       @ Reservando 40 bytes na pilha
    str lr, [sp, #36]     @ Salvando LR
    str r11, [sp, #32]    @ Salvando R11 (frame pointer)
    str r1, [sp, #4]      @ Salvando r1 (data_b)
    str r0, [sp, #0]      @ Salvando r0 (data_a)

    ldr r11, =mapped_address   @ Carrega o endereço base mapeado
    ldr r11, [r11]             @ Obtém o ponteiro do FPGA

    

    @ Desabilitando a escrita no wrreg
    mov r9, #0                 @ Escreve 0 em wrreg para desabilitar
    str r9, [r11, #wrreg]

    @ Escrevendo DATAA
    str r0, [r11, #data_a]     @ Escreve o opcode e o endereço em data_a

    @ Escrevendo DATAB
    str r1, [r11, #data_b]     @ Escreve data_b

    bl check_fifo              @ Verifica se o FIFO está pronto (implementação separada)

    @ Habilitando a escrita no wrreg
    mov r9, #1                 @ Escreve 1 em wrreg para habilitar
    str r9, [r11, #wrreg]

    @ Finaliza desabilitando o wrreg
    mov r9, #0                 @ Desabilita o wrreg
    str r9, [r11, #wrreg]

    @ Restaurando os registradores
    ldr lr, [sp, #36]          @ Restaura LR
    ldr r11, [sp, #32]         @ Restaura R11
    ldr r1, [sp, #4]           @ Restaura r1
    ldr r0, [sp, #0]           @ Restaura r0

    add sp, sp, #40            @ Libera a pilha
    bx lr                      @ Retorna ao chamador


  bx lr


set_sprite_pixel_assembly:

    @ r0-> parametro posição  ( (linhas * 80) +  coluna)
    @ r1-> parametro cor

    @reservando na pilha os resgitradores que vai usar
    sub sp, sp, #28  @reservando 28 bytes na pilha
    str lr, [sp, #24]
    str r1, [sp, #20]
    str r2, [sp, #16]
    str r4, [sp, #12]
    str r5, [sp, #8]
    str r6, [sp, #4]
    str r0, [sp, #0]


    ldr r4, =mapped_address  @ Endereço mapeado
    ldr r4, [r4]

    @DATAA
    mov r2, #1      @opcode
    lsl r0, #4       @deslocamento
    add r2, r2, r0    @opcode + posição

    mov r5, #0
    mov r6, #1

    str r5, [r4, #0xc0]   @desabilitando a escrita no wrreg
    str r2, [r4, #0x80]   @passando o opcode e a posição para dataA
    str r1, [r4, #0x70]   @passando a cor para dataB
    bl check_fifo         @verifica se o fifo está cheio entes de ativar o wrreg
    str r6, [r4, #0xc0]   @ativando a escrita no wrreg
    str r5, [r4, #0xc0]   @desativando a escrita no wrreg

    @carregando os registradores que foram salvos na pilha
    ldr lr, [sp, #24]
    ldr r1, [sp, #20]
    ldr r2, [sp, #16]
    ldr r4, [sp, #12]
    ldr r5, [sp, #8]
    ldr r6, [sp, #4]
    ldr r0, [sp, #0]

    add sp, sp, #24  @liberando a pilha

    bx lr

