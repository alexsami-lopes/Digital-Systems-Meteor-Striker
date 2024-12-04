#ifndef GEN_SPRITES_H
#define GEN_SPRITES_H

#include "graphics_fpga.h"


void matrix_meteor_sprite(unsigned int pixels[]);
void matrix_target_sprite(unsigned int pixels[]);
void matrix_star_sprite(unsigned int pixels[]);
void matrix_smaller_star_sprite(unsigned int pixels[]);
void matrix_explosion_sprite(unsigned int pixels[]);
void matrix_galaxy_sprite(unsigned int pixels[]);
void matrix_orange_saturn_sprite(unsigned int pixels[]);
void matrix_green_saturn_sprite(unsigned int pixels[]);
void matrix_blue_saturn_sprite(unsigned int pixels[]);
int set_new_sprites();

void matrix_mouse_sprite(unsigned int pixels[]);
void matrix_vs_sprite(unsigned int pixels[]);
void matrix_de1_soc_sprite(unsigned int pixels[]);
void matrix_pause_sprite(unsigned int pixels[]);
void matrix_menu_sprite(unsigned int pixels[]);
void matrix_shutdown_sprite(unsigned int pixels[]);

#endif