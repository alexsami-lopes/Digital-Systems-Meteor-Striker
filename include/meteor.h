#ifndef meteor_H
#define meteor_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "graphics_fpga.h"
#include "sprites_offsets.h"



typedef struct {
    uint64_t coord_x;       
    uint64_t coord_y;       
    uint64_t offset;
    uint64_t speed;       
    uint64_t reward;
    uint64_t size;
    uint64_t on_frame;        
} meteor_t;

typedef struct {
    uint64_t coord_x;               /*!< Coordenada do eixo x do centro do sprite:        0 a 639*/
    uint64_t coord_y;               /*!< Coordenada do eixo y do centro do sprite:        0 a 479*/
    uint64_t offset;                /*!< Identificação do sprite:                         0 a 24 */ 
    uint64_t data_register;         /*!< Localização do sprite na memoria:                1 a 31 */
    uint64_t visibility;            /*!< Visibilidade do sprite: 0. invisivel         1. visivel */ 
    uint64_t speed;                 /*!< Velocidade que o sprite se move, 0 se for um sprite fixo*/
} sprite_t;

/**
 * \brief função responsavel por gerar um número dentre o limite mínimo e máximo
 * \param[in] min inteiro que representa o menor valor possível
 * \param[in] max inteiro que representa o maior valor possível
 * \return o numero gerado dentro do espectro definido
 */
int 
random_number(int min, int max);

/**
 * \brief função responsavel por checar a disponibilidade de um dos registradores para uso 
 * \param[in] sprites_meteor_status vetor de inteiros que guarda a disponibilidade (0: livre) ou não (1: ocupado)
 * \return o numero do registrador vazio ou -1 caso não haja espaço disponível.
 */
int 
check_for_empty_reg(int sprites_meteor_status[]);

/**
 * \brief função responsavel pela criação e renderização de um sprite de meteoros na tela
 * \param[in] meteor elemento do tipo meteoros selecionado para renderização na tela
 * \param[in] coord_x
 * \param[in] coord_y
 * \param[in] base_reg
 * \param[in] sprites_meteor_status vetor de inteiros que guarda a disponibilidade (0: livre) ou não (1: ocupado)
 * \param[in] meteors_on_screen
 * \return 1 em caso de colisão, 0 caso contrario
 */
int 
create_sprite_meteor(meteor_t meteor, int coord_x, int coord_y, int base_reg, int sprites_meteor_status[], meteor_t meteors_on_screen[]);

void initialize_meteor_vector(meteor_t vetor_meteor[]);

meteor_t select_random_meteor_from_vector(meteor_t vetor_meteor[]);

int range_min_coord_x(int coord_x_player, int min_limit_action_sector);

int range_max_coord_x(int coord_x_player, int max_limit_action_sector);
 
void move_meteors(meteor_t meteors_on_screen[], int sprites_meteor_status[], int unidade, int reg_base_meteors);

int all_meteors_invisibles(meteor_t meteors_on_screen[], int sprites_meteor_status[]);
 
int restart_all_invisible_meteors(meteor_t meteors_on_screen[], int sprites_meteor_status[]);

void clean_all_meteors(meteor_t meteors_on_screen[], int sprites_meteor_status[]);
 
//int random_meteor(int cord_x_player, int cord_y_player, int min_limit_action_sector, int max_limit_action_sector, meteor_t meteors_on_screen[], int sprites_meteor_status[], meteor_t vetor_meteor[]);
int random_meteor(int cord_x_player, int min_limit_action_sector, int max_limit_action_sector, meteor_t meteors_on_screen[], int sprites_meteor_status[], meteor_t vetor_meteor[]);    

#endif