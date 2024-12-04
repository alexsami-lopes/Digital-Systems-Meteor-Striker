#ifndef COLISION_MODULE_H
#define COLISION_MODULE_H

//#include "../Lib/Colenda_Lib/colenda.h"
#include "meteor.h"

/**
 * \brief função responsavel por checar a colisão entre um jogador e um obstaculo
 * \param[in] player struct do tipo sprite do jogador a ser verificada colisão
 * \param[in] obstacle struct do obstaculo que será comparada para verificar se o player colidiu
 * \return 1 em caso de colisão, 0 caso contrario
 */
int check_colision_player(sprite_t player, meteor_t obstacle);

/**
 * \brief Função responsável por checar se o meteoro colidiu com a parte inferior da tela
 * \param[in] meteor Struct do tipo meteor_t representando o meteoro cuja colisão será verificada
 * \return Coordenada X do meteoro em caso de colisão (colisão ocorre quando a posição Y é maior ou igual a 460), ou 0 caso contrário
 */

int check_collision_bottom(meteor_t meteor);


#endif