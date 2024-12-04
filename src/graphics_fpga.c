#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include "../include/graphics_fpga.h"

/* Lightweight HPS-to-FPGA Bridge */
size_t ALT_LWFPGASLVS_SPAN = 4096;
off_t ALT_LWFPGASLVS_OFST = 0xff200000;
void *ALT_LWFPGASLVS_BASE_VIRTUAL;

/* Barramentos e sinais do processador gráfico */
volatile unsigned int *DATA_A;
volatile unsigned int *DATA_B;
volatile unsigned int *WR_FULL;
volatile unsigned int *WR_REG;
volatile unsigned int *SCREEN_;
volatile unsigned int *RESET_PULSECOUNTER;

int32_t fd, fd_map;



int32_t open_men_vgafpga() {
    //int32_t fd = open("/dev/mem", O_RDWR | O_SYNC);
    // fd = open("/dev/mem", O_RDWR | O_SYNC);
    // if (fd == -1) {
    //     perror("Erro ao abrir /dev/mem");
    //     return -1;
    // }
    // mapping_fpga();
    // return fd;

    open_devmem();
    return 1;
}

void close_men_vgafpga() {
    // unmapping_fpga();
    // close(fd);
    close_devmen();
}

int mapping_fpga() {
    ALT_LWFPGASLVS_BASE_VIRTUAL = mmap(NULL, ALT_LWFPGASLVS_SPAN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, ALT_LWFPGASLVS_OFST);
    if (ALT_LWFPGASLVS_BASE_VIRTUAL == MAP_FAILED) {
        perror("Erro ao mapear memória com mmap");
        close(fd);
        return -1;
    }

    DATA_A = ALT_LWFPGASLVS_BASE_VIRTUAL + DATAA;
    DATA_B = ALT_LWFPGASLVS_BASE_VIRTUAL + DATAB;
    WR_FULL = ALT_LWFPGASLVS_BASE_VIRTUAL + WRFULL;
    WR_REG = ALT_LWFPGASLVS_BASE_VIRTUAL + WRREG;
    SCREEN_ = ALT_LWFPGASLVS_BASE_VIRTUAL + SCREEN;
    RESET_PULSECOUNTER = ALT_LWFPGASLVS_BASE_VIRTUAL + RESETPULSECOUNTER;

    return 0;
}

void unmapping_fpga() {
    if (munmap(ALT_LWFPGASLVS_BASE_VIRTUAL, ALT_LWFPGASLVS_SPAN) == -1) {
        perror("Erro ao desmapear a memória");
    }
}

void draw_square_8x8(int position, unsigned int color) {
    // while (*WR_FULL != 0) { /* Aguarda até que WR_FULL esteja em 0 */ }
    // *DATA_A = (position << 4) | 0x2;
    // *DATA_B = color;
    // *WR_REG = 0x01;
    // *WR_REG = 0x00;
    bg_block(position, color);
}

void set_sprite_pixel(int position, unsigned int color) {
    // while (*WR_FULL != 0) { /* Aguarda até que WR_FULL esteja em 0 */ }
    // *DATA_A = (position << 4) | 0x1;
    // *DATA_B = color;
    // *WR_REG = 0x01;
    // *WR_REG = 0x00;
    set_sprite_pixel_assembly(position, color);
}
void set_background_color(unsigned int color) {
    // while (*WR_FULL != 0) { /* Aguarda até que WR_FULL esteja em 0 */ }
    // *DATA_A = (0b00000 << 4) | create_sprite_from_matrix
    // *WR_REG = 0x00;
    bg_color(color);
}

void draw_square_16x16(int position, unsigned int color) {
    draw_square_8x8(position, color);
    position += 1;
    draw_square_8x8(position, color);
    position += 79;
    draw_square_8x8(position, color);
    position += 1;
    draw_square_8x8(position, color);   
}

void erase_background(unsigned int color) {
    int pos = 0;
    //unsigned int color = 0b000000000;
    unsigned int deep_space = 0b001000001;
    while (pos < 4800) {
        draw_square_8x8(pos, color);
        pos++;
    }
}



int set_sprite(uint64_t coord_x, uint64_t coord_y, uint64_t offset, uint64_t data_register, uint64_t visibility) {
    /* validação dos valores inseridos  */
    // if ((visibility > 1 || coord_x > 639 || coord_y > 479 || offset > 40 || data_register > 32) || data_register < 1) {
    //     // printf("valor fora do limite de representação\n");
    //     return 0;
    // }

    // while (*WR_FULL != 0) { /* Aguarda até que WR_FULL esteja em 0 */ }
    // *DATA_A = (data_register << 4) | WBR;
    // *DATA_B = (visibility << 29) | (coord_x << 19) | (coord_y << 9) | offset;
    // *WR_REG = 0x01;
    // *WR_REG = 0x00;

    set_sprite2(offset, coord_x, coord_y, visibility, data_register);


    return 1;
}

// int clear() {
//     size_t i;

//     /* Cria um sprite invisivel */
//     sprite_t disable_sprite = {
//         .coord_x = 0,
//         .coord_y = 0,
//         .offset = 0,
//         .visibility = 0,
//     };
 

//     /* Passa por todos os registradores de sprite */
//     for (i = 1; i < 32; ++i) {
//         disable_sprite.data_register = i;
//         /* insere um sprite desabilitado */
//         //set_sprite(disable_sprite);
//         //set_sprite(disable_sprite.coord_x, disable_sprite.coord_y, disable_sprite.offset, disable_sprite.data_register, disable_sprite.visibility, disable_sprite.speed);
//         set_sprite(disable_sprite.coord_x, disable_sprite.coord_y, disable_sprite.offset, disable_sprite.data_register, disable_sprite.visibility);
//     }

//     unsigned int black = 0b000000000;

//     /* muda a cor do fundo para preto */ 
//     erase_background();
//     //set_background_color(black);

//     return 1;
// }
