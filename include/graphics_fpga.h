#ifndef GRAPHICS_FPGA_H
#define GRAPHICS_FPGA_H

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

/* Barramento de dados do processador gráfico */
#define DATAA 0x80
#define DATAB 0x70
#define WRFULL 0xb0
#define WRREG 0xc0
#define SCREEN 0xa0
#define RESETPULSECOUNTER 0x90

/* Opcodes das instruções */
#define WBR 0  // 0000
#define WSM 1  // 0001
#define WBM 2  // 0010
#define DP  3  // 0011
// typedef struct {
//     uint64_t coord_x;               /*!< Coordenada do eixo x do centro do sprite:        0 a 639*/
//     uint64_t coord_y;               /*!< Coordenada do eixo y do centro do sprite:        0 a 479*/
//     uint64_t offset;                /*!< Identificação do sprite:                         0 a 24 */ 
//     uint64_t data_register;         /*!< Localização do sprite na memoria:                1 a 31 */
//     uint64_t visibility;            /*!< Visibilidade do sprite: 0. invisivel         1. visivel */ 
//     uint64_t speed;                 /*!< Velocidade que o sprite se move, 0 se for um sprite fixo*/
// } sprite_t;

/* Declarações das funções */
int32_t open_men_vgafpga();
void close_men_vgafpga();
int mapping_fpga();
void unmapping_fpga();
void draw_square_8x8(int position, unsigned int color);
void draw_square_16x16(int position, unsigned int color);
//void erase_background();
void erase_background(unsigned int color);
void set_background_color(unsigned int color);
void set_sprite_pixel(int position, unsigned int color);
int create_sprite_from_matrix(unsigned int matrix[], uint64_t offset_sprite);
//int set_sprite(uint64_t coord_x, uint64_t coord_y, uint64_t offset, uint64_t data_register, uint64_t visibility, uint64_t speed);
int set_sprite(uint64_t coord_x, uint64_t coord_y, uint64_t offset, uint64_t data_register, uint64_t visibility);
//int clear();


// Declarações das funções assembly
extern void open_devmem();
extern void bg_block(int posicao, int cor);
extern void poligono(int cor, int tamanho, int forma, int endereco, int pos_x, int pos_y);
extern void bg_color(int cor);
extern void set_sprite2(int offset, int px, int py, int sp, int data_register);
//extern void set_sprite2(uint64_t offset, uint64_t coord_x, uint64_t coord_y, uint64_t visibility, uint64_t data_register);
extern void close_devmen();
extern void set_sprite_pixel_assembly(int position, int color);

/* Variáveis globais */
extern size_t ALT_LWFPGASLVS_SPAN;
extern off_t ALT_LWFPGASLVS_OFST;
extern void *ALT_LWFPGASLVS_BASE_VIRTUAL;

extern volatile unsigned int *DATA_A;
extern volatile unsigned int *DATA_B;
extern volatile unsigned int *WR_FULL;
extern volatile unsigned int *WR_REG;
extern volatile unsigned int *SCREEN_;
extern volatile unsigned int *RESET_PULSECOUNTER;

#endif
