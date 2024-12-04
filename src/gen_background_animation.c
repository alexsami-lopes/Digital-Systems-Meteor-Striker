
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include "../include/gen_background_animation.h"
#include "../include/gen_sprites.h"
#include "../include/sprites_offsets.h"
#include "../include/graphics_fpga.h"
#include "../include/meteor.h"

#define CPU_CLOCK_HZ 1000000000 // 1 GHz 

#define PI 31416 // Representação de π em uma escala de 10000
#define SCALE 10000 // Fator de escala para cálculos inteiros
#define TABLE_SIZE 360 // Tamanho da tabela de seno e cosseno
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

// Tabelas de seno e cosseno escalados
int16_t sin_table[TABLE_SIZE];
int16_t cos_table[TABLE_SIZE];



celestial_t galaxy_sprite_c = {0, 0, GALAXY, 16, 1, 150, 100, 320, 240, 0, 1, 0};        // Elipse horizontal
celestial_t saturn_orange_sprite_c = {0, 0, SATURN_ORANGE, 15, 1, 200, 50, 320, 240, 0, 2, 30}; // Elipse inclinada
celestial_t saturn_green_sprite_c = {0, 0, SATURN_GREEN, 14, 1, 250, 120, 320, 240, 0, 3, 60}; // Elipse inclinada mais vertical
celestial_t saturn_blue_sprite_c = {0, 0, SATURN_BLUE, 13, 1, 240, 200, 320, 240, 0, 5, 150}; // Elipse quase vertical*/

typedef struct {
    uint64_t red;           /*!< Intensidade cor vermelha*/
    uint64_t green;         /*!< Intensidade cor verde*/
    uint64_t blue;          /*!< Intensidade cor azul*/ 
} color_t;

// Estruturas de cores para o espaço sideral
//color_t deep_space = { .blue = 1, .green = 0, .red = 1 }; // Fundo azul/violeta escuro
color_t star = { .blue = 6, .green = 6, .red = 6 };       // Branco para estrelas
color_t laser_center = { .blue = 7, .green = 7, .red = 7 }; // Branco brilhante
color_t laser_violet = { .blue = 5, .green = 0, .red = 5 }; // Gradiente violeta
color_t laser_violet_brighter = { .blue = 6, .green = 1, .red = 6 }; // Gradiente violeta
color_t laser_violet_brighter2 = { .blue = 7, .green = 2, .red = 7 }; // Gradiente violeta
color_t laser_blue = { .blue = 3, .green = 0, .red = 1 };   // Gradiente azul escuro

sprite_t star_sprite_t0 = {
    .data_register = 10,
    .coord_x = 0,
    .coord_y = 0,
    .offset = STAR,
    .speed = 0,
    .visibility = 1,
};
sprite_t star_sprite_t1 = {
    .data_register = 11,
    .coord_x = 0,
    .coord_y = 0,
    .offset = STAR,
    .speed = 0,
    .visibility = 1,
};
sprite_t star_sprite_t2 = {
    .data_register = 12,
    .coord_x = 0,
    .coord_y = 0,
    .offset = STAR,
    .speed = 0,
    .visibility = 1,
};

sprite_t smaller_star_sprite_tt = {
    .data_register = 9,
    .coord_x = 5,
    .coord_y = 70,
    .offset = SMALLER_STAR,
    .speed = 0,
    .visibility = 1,
};
sprite_t player1_score_digit1_sprite = {
    .data_register = 1,
    .coord_x = 0,
    .coord_y = 0,
    .offset = DIGIT0,
    .speed = 0,
    .visibility = 1,
};
sprite_t player1_score_digit2_sprite = {
    .data_register = 2,
    .coord_x = 20,
    .coord_y = 0,
    .offset = DIGIT0,
    .speed = 0,
    .visibility = 1,
};
sprite_t player2_score_digit1_sprite = {
    .data_register = 3,
    .coord_x = 599,
    .coord_y = 0,
    .offset = DIGIT0,
    .speed = 0,
    .visibility = 1,
};
sprite_t player2_score_digit2_sprite = {
    .data_register = 4,
    .coord_x = 619,
    .coord_y = 0,
    .offset = DIGIT0,
    .speed = 0,
    .visibility = 1,
};

int frame; // Controla o quadro que está sendo exibido no momento

int bg_animation_module_init() {
    int i;

    frame = 0;

    //unsigned int deep_space_b = deep_space.blue | (deep_space.green << 3) | (deep_space.red << 6);
    unsigned int laser_center_b = laser_center.blue | (laser_center.green << 3) | (laser_center.red << 6);
    unsigned int laser_violet_b = laser_violet.blue | (laser_violet.green << 3) | (laser_violet.red << 6);
    unsigned int laser_violet_bb = laser_violet_brighter.blue | (laser_violet_brighter.green << 3) | (laser_violet_brighter.red << 6);
    unsigned int laser_violet_bbb = laser_violet_brighter2.blue | (laser_violet_brighter2.green << 3) | (laser_violet_brighter2.red << 6);
    //unsigned int laser_blue_b = laser_blue.blue | (laser_blue.green << 3) | (laser_blue.red << 6);
    // Define o fundo do espaço sideral
    //set_background_color(deep_space_b);

    // Desenha o "laser" central (gradiente de violeta para azul, com branco no meio)
    /*for (i = 0; i < 2; i++) {
        draw_vertical_block_line(60, 37 - i, 0, laser_violet_b);
        draw_vertical_block_line(60, 42 + i, 0, laser_violet_b);
    }*/
    //draw_vertical_block_line(60, 38, 0, laser_center_b); // Centro branco brilhante
    //draw_vertical_block_line(60, 39, 0, laser_center_b);
    draw_vertical_block_line(60, 36, 0, laser_violet_b);//
    draw_vertical_block_line(60, 37, 0, laser_violet_bb);//
    draw_vertical_block_line(60, 38, 0, laser_violet_bbb);//
    draw_vertical_block_line(60, 39, 0, laser_center_b);
    draw_vertical_block_line(60, 40, 0, laser_center_b);
    draw_vertical_block_line(60, 41, 0, laser_violet_bbb);
    draw_vertical_block_line(60, 42, 0, laser_violet_bb);//
    draw_vertical_block_line(60, 43, 0, laser_violet_b);//
    
    //poligono(laser_center_b, 10, 1, 1);
    init_trig_tables();
    

    return 1;
}

// Função para calcular a posição linear a partir de coordenadas (x, y)
int calculate_position(int x, int y) {
    return y * 80 + x;
}

// Função para desenhar uma estrela em posição aleatória
void draw_random_star(int star_n)
{
    int x = (rand() % 62) * 10; // Posição horizontal aleatória
    int y = (rand() % 46) * 10; // Posição vertical aleatória
    int star_number = star_n;//rand() % 2;  // Estrela aleatória
    //int position = calculate_position(x, y);
    if (x > 1) {
        if (x < 279 || x > 340)
        {
            /*
            draw_square_8x8(position, star.blue | (star.green << 3) | (star.red << 6));
            set_sprite(x, y, SMALLER_STAR, i, 1);
            usleep(500000); // Meio segundo
            set_sprite(x, y, STAR, i, 1);*/

            switch (star_number)
            {
            case 0:
                star_sprite_t0.visibility = 0;
                set_sprite(star_sprite_t0.coord_x, star_sprite_t0.coord_y, star_sprite_t0.offset, star_sprite_t0.data_register, star_sprite_t0.visibility);
                set_sprite(star_sprite_t0.coord_x, star_sprite_t0.coord_y, smaller_star_sprite_tt.offset, smaller_star_sprite_tt.data_register, 1);
                usleep(80000); // Meio segundo
                //delay_ms(2);
                set_sprite(star_sprite_t0.coord_x, star_sprite_t0.coord_y, smaller_star_sprite_tt.offset, smaller_star_sprite_tt.data_register, 0);
                star_sprite_t0.visibility = 1;
                star_sprite_t0.coord_x = x;
                star_sprite_t0.coord_y = y;
                set_sprite(star_sprite_t0.coord_x, star_sprite_t0.coord_y, smaller_star_sprite_tt.offset, smaller_star_sprite_tt.data_register, 1);
                usleep(80000); // Meio segundo
                //delay_ms(2);
                set_sprite(star_sprite_t0.coord_x, star_sprite_t0.coord_y, smaller_star_sprite_tt.offset, smaller_star_sprite_tt.data_register, 0);
                set_sprite(star_sprite_t0.coord_x, star_sprite_t0.coord_y, star_sprite_t0.offset, star_sprite_t0.data_register, star_sprite_t0.visibility);
                break;
            case 1:
                star_sprite_t1.visibility = 0;
                set_sprite(star_sprite_t1.coord_x, star_sprite_t1.coord_y, star_sprite_t1.offset, star_sprite_t1.data_register, star_sprite_t1.visibility);
                set_sprite(star_sprite_t1.coord_x, star_sprite_t1.coord_y, smaller_star_sprite_tt.offset, smaller_star_sprite_tt.data_register, 1);
                usleep(80000); // Meio segundo
                //delay_ms(2);
                set_sprite(star_sprite_t1.coord_x, star_sprite_t1.coord_y, smaller_star_sprite_tt.offset, smaller_star_sprite_tt.data_register, 0);
                star_sprite_t1.visibility = 1;
                star_sprite_t1.coord_x = x;
                star_sprite_t1.coord_y = y;
                set_sprite(star_sprite_t1.coord_x, star_sprite_t1.coord_y, smaller_star_sprite_tt.offset, smaller_star_sprite_tt.data_register, 1);
                usleep(80000); // Meio segundo
                //delay_ms(2);
                set_sprite(star_sprite_t1.coord_x, star_sprite_t1.coord_y, smaller_star_sprite_tt.offset, smaller_star_sprite_tt.data_register, 0);
                set_sprite(star_sprite_t1.coord_x, star_sprite_t1.coord_y, star_sprite_t1.offset, star_sprite_t1.data_register, star_sprite_t1.visibility);
                break;
            case 2:
                star_sprite_t2.visibility = 0;
                set_sprite(star_sprite_t2.coord_x, star_sprite_t2.coord_y, star_sprite_t2.offset, star_sprite_t2.data_register, star_sprite_t2.visibility);
                set_sprite(star_sprite_t2.coord_x, star_sprite_t2.coord_y, smaller_star_sprite_tt.offset, smaller_star_sprite_tt.data_register, 1);
                usleep(80000); // Meio segundo
                //delay_ms(2);
                set_sprite(star_sprite_t2.coord_x, star_sprite_t2.coord_y, smaller_star_sprite_tt.offset, smaller_star_sprite_tt.data_register, 0);
                star_sprite_t2.visibility = 1;
                star_sprite_t2.coord_x = x;
                star_sprite_t2.coord_y = y;
                set_sprite(star_sprite_t2.coord_x, star_sprite_t2.coord_y, smaller_star_sprite_tt.offset, smaller_star_sprite_tt.data_register, 1);
                usleep(80000); // Meio segundo
                //delay_ms(2);
                set_sprite(star_sprite_t2.coord_x, star_sprite_t2.coord_y, smaller_star_sprite_tt.offset, smaller_star_sprite_tt.data_register, 0);
                set_sprite(star_sprite_t2.coord_x, star_sprite_t2.coord_y, star_sprite_t2.offset, star_sprite_t2.data_register, star_sprite_t2.visibility);
                break;

            default:
                break;
            }
        }

    }

}

// Animação de fundo: estrelas piscando aleatoriamente
void bg_animation() {
    //int i;

    /* Remove estrelas aleatórias para simular movimento */
    /*
    //for (i = 0; i < 3; i++) {
        
        //int x = rand() % 60;
        //int y = rand() % 80;
        //int position = calculate_position(x, y);
        //if((x < 37 || x > 45) && x > 1) {
            //draw_square_8x8(position, deep_space.blue | (deep_space.green << 3) | (deep_space.red << 6));
        switch (i)
        {
        case 0:
            star_sprite_t0.visibility = 0;
            set_sprite(star_sprite_t0.coord_x, star_sprite_t0.coord_y, star_sprite_t0.offset, star_sprite_t0.data_register, star_sprite_t0.visibility);
            set_sprite(star_sprite_t0.coord_x, star_sprite_t0.coord_y, smaller_star_sprite_tt.offset, smaller_star_sprite_tt.data_register, 1);
            usleep(500000); // Meio segundo
            set_sprite(star_sprite_t0.coord_x, star_sprite_t0.coord_y, smaller_star_sprite_tt.offset, smaller_star_sprite_tt.data_register, 0);
            break;
        case 1:
            set_sprite(star_sprite_t1.coord_x, star_sprite_t1.coord_y, star_sprite_t1.offset, star_sprite_t1.data_register, star_sprite_t1.visibility);
            set_sprite(star_sprite_t1.coord_x, star_sprite_t1.coord_y, smaller_star_sprite_tt.offset, smaller_star_sprite_tt.data_register, 1);
            usleep(500000); // Meio segundo
            set_sprite(star_sprite_t1.coord_x, star_sprite_t1.coord_y, smaller_star_sprite_tt.offset, smaller_star_sprite_tt.data_register, 0);
            break;   
        case 2:
            set_sprite(star_sprite_t2.coord_x, star_sprite_t2.coord_y, star_sprite_t2.offset, star_sprite_t2.data_register, star_sprite_t2.visibility);
            set_sprite(star_sprite_t2.coord_x, star_sprite_t2.coord_y, smaller_star_sprite_tt.offset, smaller_star_sprite_tt.data_register, 1);
            usleep(500000); // Meio segundo
            set_sprite(star_sprite_t2.coord_x, star_sprite_t2.coord_y, smaller_star_sprite_tt.offset, smaller_star_sprite_tt.data_register, 0);
            break;                 
        
        default:
            break;
        }


        
        
    }*/
   update_all_celestials();
    int star_num = 0;
    /* Adiciona novas estrelas em posições aleatórias */
    for (star_num = 0; star_num < 3; star_num++) {
        draw_random_star(star_num);

    }

    frame++; // Incrementa o quadro
}

void draw_vertical_block_line(int num_blocks, int start_x, int start_y, unsigned int color) {
    //unsigned int color_value = color.blue | (color.green << 3) | (color.red << 6);

    for (int i = 0; i < num_blocks; i++) {
        int current_x = start_x;
        int current_y = start_y + i;
        int position = current_y * 80 + current_x; // Cálculo da posição linear na tela de 640x480

        draw_square_8x8(position, color);
    }
}

void display_write_score(int score, int player) {
    //unsigned int pixels[400]; // Array de pixels para renderização de dígitos
    
    // Separar os dígitos da pontuação (deve ser entre 0 e 99)
    int digit1 = score / 10; // Dígito das dezenas
    int digit2 = score % 10; // Dígito das unidades

    // Configurar sprites com base no jogador
    sprite_t *digit1_sprite, *digit2_sprite;
    if (player == 1) {
        digit1_sprite = &player1_score_digit1_sprite;
        digit2_sprite = &player1_score_digit2_sprite;
    } else {
        digit1_sprite = &player2_score_digit1_sprite;
        digit2_sprite = &player2_score_digit2_sprite;
        //return;
    }

    // Configurar dígitos no sprite de dezena (digit1)
    //matrix_target_sprite(pixels);
    //matrix_digit9_sprite(pixels, DIGIT0 + digit1); // Renderizar o dígito da dezena
    set_sprite(
        digit1_sprite->coord_x,
        digit1_sprite->coord_y,
        digit1_sprite->offset + digit1, // Offset para o dígito correto
        digit1_sprite->data_register,
        digit1_sprite->visibility
    );

    // Configurar dígitos no sprite de unidade (digit2)
    //matrix_target_sprite(pixels);
    //matrix_digit9_sprite(pixels, DIGIT0 + digit2); // Renderizar o dígito da unidade
    set_sprite(
        digit2_sprite->coord_x,
        digit2_sprite->coord_y,
        digit2_sprite->offset + digit2, // Offset para o dígito correto
        digit2_sprite->data_register,
        digit2_sprite->visibility
    );
}


void delay_ms(uint32_t multiplier) {
    uint64_t target_cycles = multiplier * 100 * (CPU_CLOCK_HZ / 1000); // 100ms * multiplier em ciclos
    volatile uint64_t count = 0; // Variável volátil para evitar otimização

    while (count < target_cycles) {
        count++;
    }
}

// Função para inicializar as tabelas de seno e cosseno para o movimento dos corpos celestes
void init_trig_tables() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        sin_table[i] = (int16_t)(SCALE * sin((double)i * 2 * PI / TABLE_SIZE / SCALE)); // Precisa ser calculado fora do microcontrolador
        cos_table[i] = (int16_t)(SCALE * cos((double)i * 2 * PI / TABLE_SIZE / SCALE)); // Precisa ser calculado fora do microcontrolador
    }
}



// Função para atualizar a posição de um sprite em órbita elíptica
void update_ellipse_orbit(celestial_t *celestial) {
    // Incrementa o ângulo com base na velocidade angular
    celestial->orbit_angle += celestial->orbit_speed;

    // Mantém o ângulo dentro do intervalo [0, 359]
    if (celestial->orbit_angle >= TABLE_SIZE) {
        celestial->orbit_angle -= TABLE_SIZE;
    }

    // Calcula as posições X e Y na elipse, sem inclinação
    int raw_x = (celestial->semi_major_axis * cos_table[celestial->orbit_angle]) / SCALE;
    int raw_y = (celestial->semi_minor_axis * sin_table[celestial->orbit_angle]) / SCALE;

    // Rotaciona as coordenadas pela inclinação da elipse
    int rotated_x = (raw_x * cos_table[celestial->inclination_angle] - raw_y * sin_table[celestial->inclination_angle]) / SCALE;
    int rotated_y = (raw_x * sin_table[celestial->inclination_angle] + raw_y * cos_table[celestial->inclination_angle]) / SCALE;

    // Ajusta as coordenadas ao centro da órbita
    int new_x = celestial->orbit_center_x + rotated_x;
    int new_y = celestial->orbit_center_y + rotated_y;

    // Garante que as coordenadas fiquem dentro dos limites da tela
    if (new_x < 0) new_x = 0;
    if (new_x >= SCREEN_WIDTH) new_x = SCREEN_WIDTH - 1;
    if (new_y < 0) new_y = 0;
    if (new_y >= SCREEN_HEIGHT) new_y = SCREEN_HEIGHT - 1;

    // Atualiza as coordenadas
    celestial->coord_x = new_x;
    celestial->coord_y = new_y;
    //usleep(30000);
    if ((celestial->coord_x > 340 && celestial->coord_x < 640) || (celestial->coord_x > 0 && celestial->coord_x < 279))
    {
        // Atualiza a posição do sprite na tela
        set_sprite(celestial->coord_x, celestial->coord_y, celestial->offset, celestial->data_register, celestial->visibility);
    }
    else 
    {
        // Atualiza a posição do sprite na tela
        set_sprite(celestial->coord_x, celestial->coord_y, celestial->offset, celestial->data_register, 0);
    }

}

void clear_all_celestials() {
    clear_celestial(&galaxy_sprite_c);
    clear_celestial(&saturn_orange_sprite_c);
    clear_celestial(&saturn_green_sprite_c);
    clear_celestial(&saturn_blue_sprite_c);

}

void clear_celestial(celestial_t *celestial) {

    set_sprite(celestial->coord_x, celestial->coord_y, celestial->offset, celestial->data_register, 0);

}

// Função principal que atualiza todos os sprites
void update_all_celestials() {
    usleep(300);
    /*static celestial_t galaxy_sprite_c = {0, 0, 0, 0, 1, 150, 100, 320, 240, 0, 1, 0};   // Elipse horizontal
    static celestial_t saturn_orange_sprite_c = {0, 0, 0, 1, 1, 200, 50, 320, 240, 0, 2, 30}; // Elipse inclinada
    static celestial_t saturn_green_sprite_c = {0, 0, 0, 2, 1, 250, 120, 320, 240, 0, 3, 60}; // Elipse inclinada mais vertical
    static celestial_t saturn_blue_sprite_c = {0, 0, 0, 3, 1, 240, 200, 320, 240, 0, 5, 150}; // Elipse quase vertical*/

    // Atualiza cada sprite individualmente
    update_ellipse_orbit(&galaxy_sprite_c);
    update_ellipse_orbit(&saturn_orange_sprite_c);
    update_ellipse_orbit(&saturn_green_sprite_c);
    update_ellipse_orbit(&saturn_blue_sprite_c);
}



