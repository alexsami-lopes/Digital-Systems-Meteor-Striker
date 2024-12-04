#ifndef GEN_BACKGROUND_ANIMATION_H
#define GEN_BACKGROUND_ANIMATION_H
//#include "../src/gen_background_animation.c"

// Estrutura para armazenar informações dos corpos celestes
typedef struct {
    int coord_x;
    int coord_y;
    int offset;
    int data_register;
    int visibility;
    int semi_major_axis;  // Semi-eixo maior (em pixels)
    int semi_minor_axis;  // Semi-eixo menor (em pixels)
    int orbit_center_x;   // Centro da órbita no eixo X
    int orbit_center_y;   // Centro da órbita no eixo Y
    int orbit_angle;      // Ângulo atual (em graus, 0 a 359)
    int orbit_speed;      // Velocidade angular (em graus por frame)
    int inclination_angle; // Ângulo de inclinação da elipse (em graus, 0 a 359)
} celestial_t;

int bg_animation_module_init();
int calculate_position(int x, int y);
//void draw_random_star();
void draw_random_star(int star_number);
void bg_animation();
void draw_vertical_block_line(int num_blocks, int start_x, int start_y, unsigned int color);
void display_write_score(int score, int player);
void delay_ms(uint32_t multiplier);
void init_trig_tables();
void update_ellipse_orbit(celestial_t *celestial);
void update_all_celestials();
void clear_celestial(celestial_t *celestial);
void clear_all_celestials();

//int set_menu();
#endif