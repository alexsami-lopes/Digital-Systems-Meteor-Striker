

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>                     // Para mapear memória física
#include <sys/time.h>
#include "../include/graphics_fpga.h"
#include "../include/sprites_offsets.h"
#include "../include/gen_sprites.h"
#include "../include/gen_cover.h"
#include "../include/mouse_module.h"
#include "../include/gen_background_animation.h"
#include "../include/input.h"
#include "../include/keys.h"
#include "../include/meteor.h"
#include "../include/colision_module.h"
#include "../include/address_map_arm.h"   // Mapeamento de endereços de memória ARM
#include "../include/draw_trophy_to_bg.h"
//#include "../include/gen_initial_screen.h"


#define PLAYER_SPEED_BASE 2
#define OFFSET_PLAYER_1 28
#define OFFSET_PLAYER_2 28
#define BULLET_SPEED_BASE 15

int meteor_player_gen_turn = 0; // 0 para player1, 1 para player2

/*criação de um tipo pra facilitar a manipulação dos estados do jogo*/
typedef enum {
    running = 1,
    in_pause,
    in_menu,
    finish,
    win,
    lose,
} states;


unsigned int deep_space = 0b010000001;
unsigned int black = 0b000000000;



//do registrador 1 ao 10 são as balas, do 11 ao 19 são cenarios, do 20 ao 29 são meteoros, 30 player 2 e 31 player 1
meteor_t obs[16];
meteor_t screen_obs[10];
sprite_t cenario[9];
//sprite_t sprite_bullets[10];

sprite_t player_1_sprite = {
    .data_register = 31,
    .coord_x = 192,
    .coord_y = 340,
    .offset = OFFSET_PLAYER_1,
    .speed = PLAYER_SPEED_BASE,
    .visibility = 1,
};
sprite_t player_2_sprite = {
    .data_register = 30,
    .coord_x = 432,
    .coord_y = 340,
    .offset = OFFSET_PLAYER_2,
    .speed = PLAYER_SPEED_BASE,
    .visibility = 1,
};

sprite_t star_sprite_t = {
    .data_register = 19,
    .coord_x = 5,
    .coord_y = 50,
    .offset = STAR,
    .speed = PLAYER_SPEED_BASE,
    .visibility = 1,
};

sprite_t smaller_star_sprite_t = {
    .data_register = 18,
    .coord_x = 5,
    .coord_y = 70,
    .offset = SMALLER_STAR,
    .speed = PLAYER_SPEED_BASE,
    .visibility = 1,
};

sprite_t explosion_sprite_t = {
    .data_register = 17,
    .coord_x = 5,
    .coord_y = 90,
    .offset = EXPLOSION,
    .speed = PLAYER_SPEED_BASE,
    .visibility = 1,
};

sprite_t galaxy_sprite_t = {
    .data_register = 16,
    .coord_x = 70,
    .coord_y = 110,
    .offset = GALAXY,
    .speed = PLAYER_SPEED_BASE,
    .visibility = 1,
};

sprite_t saturn_orange_sprite_t = {
    .data_register = 15,
    .coord_x = 230,
    .coord_y = 360,
    .offset = SATURN_ORANGE,
    .speed = PLAYER_SPEED_BASE,
    .visibility = 1,
};

sprite_t saturn_green_sprite_t = {
    .data_register = 14,
    .coord_x = 400,
    .coord_y = 400,
    .offset = SATURN_GREEN,
    .speed = PLAYER_SPEED_BASE,
    .visibility = 1,
};

sprite_t saturn_blue_sprite_t = {
    .data_register = 13,
    .coord_x = 600,
    .coord_y = 50,
    .offset = SATURN_BLUE,
    .speed = PLAYER_SPEED_BASE,
    .visibility = 1,
};

unsigned int *i2c0_base_ptr;   // Ponteiro para a base do controlador I2C0
unsigned int *sysmgr_base_ptr; // Ponteiro para a base do System Manager
void *i2c0base_virtual;        // Endereço virtual mapeado do I2C0
void *sysmgrbase_virtual;      // Endereço virtual mapeado do System Manager
int fd_i2c0base = -1;          // Descritor de arquivo para I2C0
int fd_sysmgr = -1;            // Descritor de arquivo para o System Manager


//mutex utilizados
pthread_mutex_t gpu_mutex;
pthread_mutex_t mouse_1_mutex;
pthread_mutex_t accelerometer_mutex;
pthread_mutex_t meteor_mutex;
pthread_mutex_t player_1_invunerability_mutex;
pthread_mutex_t player_2_invunerability_mutex;
pthread_mutex_t colision_mutex;
//condicionais utilizadas
pthread_cond_t mouse_1_cond;
pthread_cond_t accelerometer_cond;
pthread_cond_t meteor_cond;
pthread_cond_t player_1_invulnerability_cond;
pthread_cond_t player_2_invulnerability_cond;
pthread_cond_t colision_cond;
//threads
pthread_t meteor_thread, mouse_1_thread, accelerometer_thread, player_1_timer_thread, player_2_timer_thread, colision_thread; 
//pthread_t meteor_thread, mouse_1_thread, player_1_timer_thread, colision_thread; 

states state;


int pause_meteor, pause_mouse_1, pause_accel, pause_colision;
int player_1_invunerability;
int player_2_invunerability;
int meteor_on_screen_status[10] = {0,0,0,0,0,0,0,0,0,0};
int score_1;
int score_2;
int key_press_1;
int key_press_2;

float mg_per_lsb = 3.2;    // Fator de conversão do ADXL345 (miligrama por LSB)
int16_t XYZ[3];            // Array para armazenar os valores do acelerômetro nos eixos X, Y e Z

void pause_threads() {
    
    pthread_mutex_lock(&accelerometer_mutex);
    //pause_accel = 1;
    pthread_mutex_unlock(&accelerometer_mutex);
    pthread_mutex_lock(&mouse_1_mutex);
    pause_mouse_1 = 1;
    pthread_mutex_unlock(&mouse_1_mutex);
    pthread_mutex_lock(&meteor_mutex);
    pause_meteor = 1;
    pthread_mutex_unlock(&meteor_mutex);
    pthread_mutex_lock(&colision_mutex);
    pause_colision = 1;
    pthread_mutex_unlock(&colision_mutex);
    return;
}

void reestart_threads() {
    pthread_mutex_lock(&accelerometer_mutex);
    pause_accel = 0;
    pthread_cond_signal(&accelerometer_cond);
    pthread_mutex_unlock(&accelerometer_mutex);
    pthread_mutex_lock(&mouse_1_mutex);
    pause_mouse_1 = 0;
    pthread_cond_signal(&mouse_1_cond);
    pthread_mutex_unlock(&mouse_1_mutex);
    pthread_mutex_lock(&meteor_mutex);
    pause_meteor = 0;
    pthread_cond_signal(&meteor_cond);
    pthread_mutex_unlock(&meteor_mutex);
    pthread_mutex_lock(&colision_mutex);
    pause_colision = 0;
    pthread_cond_signal(&colision_cond);
    pthread_mutex_unlock(&colision_mutex);
    return;
}

//void* mouse_1_polling_routine(void* args) {
void* mouse_1_polling_routine(void* args) {
    (void)args; // Ignora o argumento
    int value_x_mouse = 0, value_y_mouse = 0;//, i, has_shot = 0;

    while (state != finish) {
        pthread_mutex_lock(&mouse_1_mutex);
        while (pause_mouse_1 || state == in_pause || state == in_menu || state == win || state == lose) {
            pthread_cond_wait(&mouse_1_cond, &mouse_1_mutex);
        }
        pthread_mutex_unlock(&mouse_1_mutex);

        read_mouse_1_event(&key_press_1, &value_x_mouse, &value_y_mouse);

        // Movimentação no eixo X
        if (value_x_mouse < 0 && player_1_sprite.coord_x - 10 >= -10 && (player_1_sprite.coord_x - 10) + value_x_mouse <= -10) {
            player_1_sprite.coord_x -= ((player_1_sprite.coord_x - 10 + value_x_mouse) + 10); 
        } else if (value_x_mouse > 0 && player_1_sprite.coord_x + 10 <= 279 && (player_1_sprite.coord_x + 10) + value_x_mouse >= 279) {
            player_1_sprite.coord_x += (279 - (player_1_sprite.coord_x + 10 + value_x_mouse));
        } else { 
            player_1_sprite.coord_x += value_x_mouse;
        }

        // Movimentação no eixo Y
        if (value_y_mouse < 0 && player_1_sprite.coord_y - 10 >= 50 && (player_1_sprite.coord_y - 10) + value_y_mouse <= 50) {
            player_1_sprite.coord_y -= ((player_1_sprite.coord_y - 10 + value_y_mouse) - 50); 
        } else if (value_y_mouse > 0 && player_1_sprite.coord_y + 10 <= 460 && (player_1_sprite.coord_y + 10) + value_y_mouse >= 460) {
            player_1_sprite.coord_y += (460 - (player_1_sprite.coord_y + 10 + value_y_mouse));
        } else { 
            player_1_sprite.coord_y += value_y_mouse;
        }

        // Controle do disparo
        // if (key_press_1 == 1 && !has_shot) {
        //     i = 0;
        // } else if (key_press_1) {
        //     has_shot = 1;
        // } else {
        //     has_shot = 0;
        // }

        // Atualiza o sprite com a nova posição
        //set_sprite(player_1_sprite.coord_x, player_1_sprite.coord_y, player_1_sprite.offset, player_1_sprite.data_register, player_1_sprite.visibility, player_1_sprite.speed);
        set_sprite(player_1_sprite.coord_x, player_1_sprite.coord_y, player_1_sprite.offset, player_1_sprite.data_register, player_1_sprite.visibility);

        if (state == finish) {
            return NULL;
        }
    }
    return NULL;
}


void* accelerometer_polling_routine(void* args) {
    (void)args; // Ignora o argumento
    int value_x_accel = 0, value_y_accel = 0;
    int target_speed_x, target_speed_y;
    int insensibility = 4; // Limite de sensibilidade para movimento
    int accelered = 0; // Variável para controlar a aceleração

    while (state != finish) {
        // Bloqueio para evitar leitura em estado de pausa ou quando o jogo estiver em um menu
        pthread_mutex_lock(&accelerometer_mutex);
        while (pause_accel || state == in_pause || state == in_menu || state == win || state == lose) {
            pthread_cond_wait(&accelerometer_cond, &accelerometer_mutex);
        }
        pthread_mutex_unlock(&accelerometer_mutex);

        // Leitura dos valores do acelerômetro XYZ
        ADXL345_XYZ_Read(XYZ); // Função para ler os dados do acelerômetro

        // Movimentação lateral com base no acelerômetro (eixo X)
        if ((XYZ[0] * mg_per_lsb) > insensibility) {
            target_speed_x = PLAYER_SPEED_BASE; // Move para a direita
        }
        else if ((XYZ[0] * mg_per_lsb) < (insensibility * -1)) {
            target_speed_x = -PLAYER_SPEED_BASE; // Move para a esquerda
        } else {
            target_speed_x = 0; // Sem movimento lateral
        }


        // Detecção de borda para movimentação lateral no eixo X
        if (target_speed_x < 0 && player_2_sprite.coord_x - 10 >= 340 && (player_2_sprite.coord_x - 10) + target_speed_x <= 340) {
            player_2_sprite.coord_x -= ((player_2_sprite.coord_x - 10 + target_speed_x) - 340);
        } else if (target_speed_x > 0 && player_2_sprite.coord_x + 10 <= 630 && (player_2_sprite.coord_x + 10) + target_speed_x >= 630) {
            player_2_sprite.coord_x += (630 - (player_2_sprite.coord_x + 10 + target_speed_x));
        } else {
            player_2_sprite.coord_x += target_speed_x;
        }



        // Movimentação vertical com base no acelerômetro (eixo Y)
        if ((XYZ[1] * mg_per_lsb) > (insensibility)) {
            //accelered = 0; // Ajusta para o tempo de atualização normal (movimento mais lento)
            target_speed_y = -PLAYER_SPEED_BASE; // Move para cima
        }
        if ((XYZ[1] * mg_per_lsb) < (insensibility * -1)) {
            //accelered = 1; // Ajusta para o tempo de atualização acelerado (movimento mais rápido)
            target_speed_y = PLAYER_SPEED_BASE; // Move para baixo
        }

                // Movimentação no eixo Y
        if (target_speed_y < 0 && player_2_sprite.coord_y - 10 >= 50 && (player_2_sprite.coord_y - 10) + target_speed_y <= 50) {
            player_2_sprite.coord_y -= ((player_2_sprite.coord_y - 10 + target_speed_y) - 50); 
        } else if (target_speed_y > 0 && player_2_sprite.coord_y + 10 <= 460 && (player_2_sprite.coord_y + 10) + target_speed_y >= 460) {
            player_2_sprite.coord_y += (460 - (player_2_sprite.coord_y + 10 + target_speed_y));
        } else { 
            player_2_sprite.coord_y += target_speed_y;
        }

        // Atualiza a posição da peça
        set_sprite(player_2_sprite.coord_x, player_2_sprite.coord_y, player_2_sprite.offset, player_2_sprite.data_register, player_2_sprite.visibility);

        usleep(10000); // Atraso para controlar a taxa de atualização
    }

    
}
/*
void* accelerometer_polling_routine(void* args) {
    (void)args; // Ignora o argumento
    int value_x_accel = 0, value_y_accel = 0;
    int target_speed_x = 0, target_speed_y = 0;
    int insensibility = 10; // Limite de sensibilidade para movimento
    int accelered = 0; // Variável para controlar a aceleração

    // Variáveis para velocidade
    int velocity_x = 0, velocity_y = 0;
    float last_time = 0; // Tempo da última leitura (em segundos)
    struct timeval current_time; // Variável para calcular o tempo
    float delta_time = 0; // Diferença de tempo entre as leituras

    while (state != finish) {
        // Bloqueio para evitar leitura em estado de pausa ou quando o jogo estiver em um menu
        pthread_mutex_lock(&accelerometer_mutex);
        while (pause_accel || state == in_pause || state == in_menu || state == win || state == lose) {
            pthread_cond_wait(&accelerometer_cond, &accelerometer_mutex);
        }
        pthread_mutex_unlock(&accelerometer_mutex);

        // Leitura dos valores do acelerômetro XYZ
        ADXL345_XYZ_Read(XYZ); // Função para ler os dados do acelerômetro

        // Calculando o tempo passado
        gettimeofday(&current_time, NULL);
        delta_time = (current_time.tv_sec - last_time) + (current_time.tv_usec - last_time) / 1000000.0f;
        last_time = current_time.tv_sec + current_time.tv_usec / 1000000.0f;

        // Movimentação lateral com base no acelerômetro (eixo X)
        if ((XYZ[0] * mg_per_lsb) > insensibility) {
            // Calcular velocidade em X
            velocity_x += (XYZ[0] * mg_per_lsb) * delta_time;
            target_speed_x = velocity_x; // Move para a direita
        } else if ((XYZ[0] * mg_per_lsb) < (insensibility * -1)) {
            // Calcular velocidade em X
            velocity_x += (XYZ[0] * mg_per_lsb) * delta_time;
            target_speed_x = velocity_x; // Move para a esquerda
        } else {
            target_speed_x = 0; // Sem movimento lateral
        }

        // Detecção de borda para movimentação lateral no eixo X
        if (target_speed_x < 0 && player_2_sprite.coord_x - 10 >= 350 && (player_2_sprite.coord_x - 10) + target_speed_x <= 350) {
            player_2_sprite.coord_x -= ((player_2_sprite.coord_x - 10 + target_speed_x) - 350);
        } else if (target_speed_x > 0 && player_2_sprite.coord_x + 10 <= 620 && (player_2_sprite.coord_x + 10) + target_speed_x >= 620) {
            player_2_sprite.coord_x += (620 - (player_2_sprite.coord_x + 10 + target_speed_x));
        } else {
            player_2_sprite.coord_x += target_speed_x;
        }

        // Movimentação vertical com base no acelerômetro (eixo Y)
        if ((XYZ[1] * mg_per_lsb) > (insensibility)) {
            // Calcular velocidade em Y
            velocity_y += (XYZ[1] * mg_per_lsb) * delta_time;
            target_speed_y = velocity_y; // Move para cima
        }
        if ((XYZ[1] * mg_per_lsb) < (insensibility * -1)) {
            // Calcular velocidade em Y
            velocity_y += (XYZ[1] * mg_per_lsb) * delta_time;
            target_speed_y = velocity_y; // Move para baixo
        }

        // Movimentação no eixo Y
        if (target_speed_y < 0 && player_2_sprite.coord_y - 10 >= 50 && (player_2_sprite.coord_y - 10) + target_speed_y <= 50) {
            player_2_sprite.coord_y -= ((player_2_sprite.coord_y - 10 + target_speed_y) - 50); 
        } else if (target_speed_y > 0 && player_2_sprite.coord_y + 10 <= 460 && (player_2_sprite.coord_y + 10) + target_speed_y >= 460) {
            player_2_sprite.coord_y += (460 - (player_2_sprite.coord_y + 10 + target_speed_y));
        } else { 
            player_2_sprite.coord_y += target_speed_y;
        }

        // Atualiza a posição da peça
        set_sprite(player_2_sprite.coord_x, player_2_sprite.coord_y, player_2_sprite.offset, player_2_sprite.data_register, player_2_sprite.visibility);

        usleep(10000); // Atraso para controlar a taxa de atualização
    }

    return NULL;
}*/



void* random_meteor_generate_routine(void* args) {
    (void)args; // Ignora o argumento
    while (state != finish) {
        pthread_mutex_lock(&meteor_mutex);
        while (pause_meteor || state == in_pause || state == in_menu || state == win || state == lose) {
            pthread_cond_wait(&meteor_cond, &meteor_mutex);
        }
        pthread_mutex_unlock(&meteor_mutex);

        if (meteor_player_gen_turn == 0) {
            random_meteor(player_1_sprite.coord_x, 20, 289, screen_obs, meteor_on_screen_status, obs);
        } else {
            random_meteor(player_2_sprite.coord_x, 336, 620, screen_obs, meteor_on_screen_status, obs);
        }
        meteor_player_gen_turn = 1 - meteor_player_gen_turn; // Alterna entre 0 e 1

        bg_animation(); // Atualiza fundo e animações

        if (state == finish) {
            return NULL;
        }
        usleep(100000);
    }
    return NULL;
}

int clear() {
    size_t i;

    /* Cria um sprite invisivel */
    sprite_t disable_sprite = {
        .coord_x = 0,
        .coord_y = 0,
        .offset = 0,
        .visibility = 0,
    };
 

    /* Passa por todos os registradores de sprite */
    for (i = 1; i < 32; ++i) {
        disable_sprite.data_register = i;
        /* insere um sprite desabilitado */
        //set_sprite(disable_sprite);
        //set_sprite(disable_sprite.coord_x, disable_sprite.coord_y, disable_sprite.offset, disable_sprite.data_register, disable_sprite.visibility, disable_sprite.speed);
        set_sprite(disable_sprite.coord_x, disable_sprite.coord_y, disable_sprite.offset, disable_sprite.data_register, disable_sprite.visibility);
    }
    unsigned int cor_azul = 0x000000FF;   // Azul
    poligono(cor_azul, 0, 1, 0, 0, 0);
	poligono(cor_azul, 0, 0, 1, 0, 0);

    //unsigned int black = 0b000000000;

    /* muda a cor do fundo para preto */ 
    erase_background(deep_space);
    //set_background_color(black);
    
    set_background_color(deep_space);

    return 1;
}
int clear_sides() {
    int i;

    /* Cria um sprite invisivel */
    sprite_t disable_sprite = {
        .coord_x = 0,
        .coord_y = 0,
        .offset = 0,
        .visibility = 0,
    };
 

    /* Passa por todos os registradores de sprite */
    for (i = 1; i < 32; ++i) {
        disable_sprite.data_register = i;
        /* insere um sprite desabilitado */
        //set_sprite(disable_sprite);
        //set_sprite(disable_sprite.coord_x, disable_sprite.coord_y, disable_sprite.offset, disable_sprite.data_register, disable_sprite.visibility, disable_sprite.speed);
        set_sprite(disable_sprite.coord_x, disable_sprite.coord_y, disable_sprite.offset, disable_sprite.data_register, disable_sprite.visibility);
    }

    unsigned int black = 0b000000000;
    poligono(black, 0, 1, 0, 0, 0);
	poligono(black, 0, 0, 1, 0, 0);

    /* muda a cor do fundo para preto */ 
    //erase_background();
    for (i = 0; i < 36; i++) {
        draw_vertical_block_line(60, 35 - i, 0, deep_space);
        draw_vertical_block_line(60, 44 + i, 0, deep_space);
    }
    //set_background_color(black);

    return 1;
}

void pause_screen() {
    /*sprite_t invisible_sprite = {
    .coord_x = 1, 
    .coord_y = 1, 
    .offset = 0, 
    .speed = 0, 
    .visibility = 0};*/
    //int msg_letters[5] = {P, A, U, S, E};
    //int msg_letters[6] = {25, 25, 25, 25, 25, 25};
    pause_threads();

    
    all_meteors_invisibles(screen_obs, meteor_on_screen_status);
    /*for (int i = 0; i < 10; i++)
    {
        if (sprite_bullets[i].visibility) {
            invisible_sprite.data_register = 1 + i;
            set_sprite(invisible_sprite);
        }
        
    }*/
    player_1_sprite.visibility = 0;
    player_2_sprite.visibility = 0;
    //set_sprite(player_1_sprite);
    //set_sprite(player_2_sprite);
    set_sprite(player_1_sprite.coord_x, player_1_sprite.coord_y, player_1_sprite.offset, player_1_sprite.data_register, player_1_sprite.visibility);
    set_sprite(player_2_sprite.coord_x, player_2_sprite.coord_y, player_2_sprite.offset, player_2_sprite.data_register, player_2_sprite.visibility);

    /*for (int i = 0; i < 5; i++)
    {
        cenario[i].coord_x = 280 + (20 * i);
        cenario[i].coord_y = 240;
        cenario[i].data_register = 11 + i;
        cenario[i].visibility = 1;
        cenario[i].offset = msg_letters[i];
        cenario[i].speed = 0;*/
        //set_sprite(cenario[i]);
        set_sprite(310, 230, PAUSE, 11, 1);
    //}
    
    return;

}

void return_screen() {
    /*sprite_t invisible_sprite = {
    .coord_x = 1, 
    .coord_y = 1, 
    .offset = 0, 
    .speed = 0, 
    .visibility = 0};*/
    player_1_sprite.visibility = 1;
    player_2_sprite.visibility = 1;
    

    for (int i = 0; i < 5; i++) {
        cenario[i].visibility = 0;
        //set_sprite(cenario[i]);
        set_sprite(cenario[i].coord_x, cenario[i].coord_y, cenario[i].offset, cenario[i].data_register, cenario[i].visibility);
    }
    //set_sprite(player_1_sprite);
    //set_sprite(player_2_sprite);
    set_sprite(player_1_sprite.coord_x, player_1_sprite.coord_y, player_1_sprite.offset, player_1_sprite.data_register, player_1_sprite.visibility);
    set_sprite(player_2_sprite.coord_x, player_2_sprite.coord_y, player_2_sprite.offset, player_2_sprite.data_register, player_2_sprite.visibility);
    reestart_threads();
    state = running;
}

void win_screen() {
    /*sprite_t invisible_sprite = {
    .coord_x = 1, 
    .coord_y = 1, 
    .offset = 0, 
    .speed = 0, 
    .visibility = 0};*/
    int coord_x = 0;
    //int msg_letters[6] = {V, E, N, C, E, U};
    //int msg_letters[6] = {25, 25, 25, 25, 25, 25};
    pause_threads();

    all_meteors_invisibles(screen_obs, meteor_on_screen_status);
    /*for (int i = 0; i < 10; i++)
    {
        if (sprite_bullets[i].visibility) {
            invisible_sprite.data_register = 1 + i;
            set_sprite(invisible_sprite);
        }
    }*/
    clear_all_celestials();
    
    if (score_1 >= 10) {
        for(int i = player_1_sprite.coord_y; i >=0; --i) {
            player_1_sprite.coord_y = i;
            //set_sprite(player_1_sprite);
            set_sprite(player_1_sprite.coord_x, player_1_sprite.coord_y, DIGIT1, 5, player_1_sprite.visibility);
            coord_x = 130;
            usleep(10000);
        }
        draw_animated_trophy(1);
    } else {
        for(int i = player_2_sprite.coord_y; i >=0; --i) {
            player_2_sprite.coord_y = i;
            //set_sprite(player_2_sprite);
            set_sprite(player_2_sprite.coord_x, player_2_sprite.coord_y, DIGIT2, 6, player_2_sprite.visibility);
            coord_x = 370;
            usleep(10000);
        }
        draw_animated_trophy(2);
    }

/*   for(int i = 0; i < 6; i++) {
        cenario[i].coord_x = coord_x + (20 * i);
        cenario[i].coord_y = 240;
        cenario[i].data_register = 11 + i;
        cenario[i].visibility = 1;
        cenario[i].offset = msg_letters[i];
        cenario[i].speed = 0;
        //set_sprite(cenario[i]);
        set_sprite(cenario[i].coord_x, cenario[i].coord_y, cenario[i].offset, cenario[i].data_register, cenario[i].visibility);
    }*/

    sleep(1);

    state = in_menu;
    clear();
    //draw_cover_to_background();
    set_menu();
    return;
}


void lose_screen() {
    /*sprite_t invisible_sprite = {
    .coord_x = 1, 
    .coord_y = 1, 
    .offset = 0, 
    .speed = 0, 
    .visibility = 0};*/

    int coord_x;
    //int msg_letters[6] = {P, E, R, D, E, U};
    int msg_letters[6] = {25, 25, 25, 25, 25, 25};
    pause_threads();
    //while(player_1_invunerability || player_2_invunerability){
    //}
    if(score_1 < 0) {
        coord_x = 130;
    } else {
        coord_x = 370;
    }
    all_meteors_invisibles(screen_obs, meteor_on_screen_status);
    /*for (int i = 0; i < 10; i++)
    {
        if (bullets[i]) {
            invisible_sprite.data_register = 1 + i;
            set_sprite(invisible_sprite);
        }
    }*/

    player_1_sprite.visibility = 0;
    player_2_sprite.visibility = 0;

    //set_sprite(player_1_sprite);
    //set_sprite(player_2_sprite);
    set_sprite(player_1_sprite.coord_x, player_1_sprite.coord_y, player_1_sprite.offset, player_1_sprite.data_register, player_1_sprite.visibility);
    set_sprite(player_2_sprite.coord_x, player_2_sprite.coord_y, player_2_sprite.offset, player_2_sprite.data_register, player_2_sprite.visibility);
    
    //player_1_invunerability = 0;
    //player_2_invunerability = 0;

    for (int i = 0; i < 6; i++) {
        cenario[i].coord_x = coord_x + (20 * i);
        cenario[i].coord_y = coord_x + (20 * i);
        /*if (score_1 < 0)
        {
            coord_x = 260;
            cenario[i].coord_x = (rand() % coord_x);
            cenario[i].coord_y = coord_x + (20 * i);
        }
        else
        {
            coord_x = 620;
            int rand_x = (rand() % coord_x);
            if(rand_x < 350) {
                rand_x = 360;
            }
            cenario[i].coord_x = rand_x;
            cenario[i].coord_y = coord_x + (20 * i);
        }*/
        
        //cenario[i].coord_y = 240;
        cenario[i].data_register = 11 + i;
        cenario[i].visibility = 1;
        cenario[i].offset = msg_letters[i];
        cenario[i].speed = 0;
        //set_sprite(cenario[i]);
        set_sprite(cenario[i].coord_x, cenario[i].coord_y, cenario[i].offset, cenario[i].data_register, cenario[i].visibility);
    }

    sleep(1);
    clear();
    state = in_menu;
    draw_cover_to_background();
    set_menu();
    return;
}   

// loop principal do jogo
void* colision_routine(void* args) {
    (void)args; // Ignora o argumento
    sprite_t invisible_sprite = {
        .coord_x = 1, 
        .coord_y = 1, 
        .offset = 0, 
        .speed = 0, 
        .visibility = 0
    };

    int points = 0;
    const uint64_t SCREEN_HEIGHT = 480; // Ajuste para a altura real da tela

    while (state != finish) {
        pthread_mutex_lock(&colision_mutex);
        while(pause_colision || state == in_pause || state == in_menu || state == win || state == lose) {
            pthread_cond_wait(&colision_cond, &colision_mutex);
        }
        pthread_mutex_unlock(&colision_mutex);
        
        if (state == finish) {
            return NULL;
        }
        
        for (int i = 0; i < 10; i++) {
            if (meteor_on_screen_status[i]) {
                pthread_mutex_lock(&meteor_mutex);
                pause_meteor = 1;
                pthread_mutex_unlock(&meteor_mutex);

                // Verificar se o meteoro colidiu com o limite inferior da tela
                if (check_collision_bottom(screen_obs[i])) {
                    int collition_x = check_collision_bottom(screen_obs[i]);
                    if(collition_x > 340 && collition_x < 640 && score_2 > 0) {
                        score_2 -= screen_obs[i].reward; // Decresce pontos do jogador 2
                    }else if(collition_x > 0 && collition_x < 279 && score_1 > 0) {
                        score_1 -= screen_obs[i].reward; // Decresce pontos do jogador 1
                    }
                    meteor_on_screen_status[i] = 0;
                    screen_obs[i].on_frame = 0;
                    invisible_sprite.data_register = 20 + i;
                    //set_sprite(invisible_sprite);
                    set_sprite(screen_obs[i].coord_x, screen_obs[i].coord_y, explosion_sprite_t.offset, explosion_sprite_t.data_register, explosion_sprite_t.visibility);
                    usleep(100000); // Meio segundo
                    //delay_ms(2);
                    set_sprite(screen_obs[i].coord_x, screen_obs[i].coord_y, explosion_sprite_t.offset, explosion_sprite_t.data_register, invisible_sprite.visibility);
                    set_sprite(screen_obs[i].coord_x, screen_obs[i].coord_y, star_sprite_t.offset, star_sprite_t.data_register, star_sprite_t.visibility);
                    usleep(100000); // Meio segundo
                    //delay_ms(2);
                    set_sprite(screen_obs[i].coord_x, screen_obs[i].coord_y, smaller_star_sprite_t.offset, smaller_star_sprite_t.data_register, smaller_star_sprite_t.visibility);
                    usleep(50000); // Meio segundo
                    //delay_ms(2);
                    set_sprite(invisible_sprite.coord_x, invisible_sprite.coord_y, invisible_sprite.offset, invisible_sprite.data_register, invisible_sprite.visibility);
                    printf("meteoro %d colidiu com o limite inferior. Pontuação reduzida.\n", i);
                } else if (check_colision_player(player_1_sprite, screen_obs[i])) {
                    score_1 += screen_obs[i].reward;
                    meteor_on_screen_status[i] = 0;
                    screen_obs[i].on_frame = 0;
                    invisible_sprite.data_register = 20 + i;
                    //set_sprite(invisible_sprite);
                    set_sprite(screen_obs[i].coord_x, screen_obs[i].coord_y, explosion_sprite_t.offset, explosion_sprite_t.data_register, explosion_sprite_t.visibility);
                    usleep(100000); // Meio segundo
                    //delay_ms(2);
                    set_sprite(screen_obs[i].coord_x, screen_obs[i].coord_y, explosion_sprite_t.offset, explosion_sprite_t.data_register, invisible_sprite.visibility);
                    set_sprite(screen_obs[i].coord_x, screen_obs[i].coord_y, star_sprite_t.offset, star_sprite_t.data_register, star_sprite_t.visibility);
                    usleep(100000); // Meio segundo
                    //delay_ms(2);
                    set_sprite(screen_obs[i].coord_x, screen_obs[i].coord_y, smaller_star_sprite_t.offset, smaller_star_sprite_t.data_register, smaller_star_sprite_t.visibility);
                    usleep(50000); // Meio segundo
                    //delay_ms(2);
                    set_sprite(invisible_sprite.coord_x, invisible_sprite.coord_y, invisible_sprite.offset, invisible_sprite.data_register, invisible_sprite.visibility);
                } else if (check_colision_player(player_2_sprite, screen_obs[i])) {
                    score_2 += screen_obs[i].reward;
                    meteor_on_screen_status[i] = 0;
                    screen_obs[i].on_frame = 0;
                    invisible_sprite.data_register = 20 + i;
                    //set_sprite(invisible_sprite);
                    //set_sprite(invisible_sprite.coord_x, invisible_sprite.coord_y, invisible_sprite.offset, invisible_sprite.data_register, invisible_sprite.visibility);
                    set_sprite(screen_obs[i].coord_x, screen_obs[i].coord_y, explosion_sprite_t.offset, explosion_sprite_t.data_register, explosion_sprite_t.visibility);
                    usleep(100000); // Meio segundo
                    //delay_ms(2);
                    set_sprite(screen_obs[i].coord_x, screen_obs[i].coord_y, explosion_sprite_t.offset, explosion_sprite_t.data_register, invisible_sprite.visibility);
                    set_sprite(screen_obs[i].coord_x, screen_obs[i].coord_y, star_sprite_t.offset, star_sprite_t.data_register, star_sprite_t.visibility);
                    usleep(100000); // Meio segundo
                    //delay_ms(2);
                    set_sprite(screen_obs[i].coord_x, screen_obs[i].coord_y, smaller_star_sprite_t.offset, smaller_star_sprite_t.data_register, smaller_star_sprite_t.visibility);
                    usleep(50000); // Meio segundo
                    //delay_ms(2);
                    set_sprite(invisible_sprite.coord_x, invisible_sprite.coord_y, invisible_sprite.offset, invisible_sprite.data_register, invisible_sprite.visibility);
                }

                pthread_mutex_lock(&meteor_mutex);
                pause_meteor = 0;
                pthread_cond_signal(&meteor_cond);
                pthread_mutex_unlock(&meteor_mutex);
            }
        }

        // Atualizar placares
        if (score_1 >= 0) {
            display_write_score(score_1, 1);
        }
        if (score_2 >= 0) {
            display_write_score(score_2, 0);
        }

        // Verificar condições de término do jogo
        if (score_1 < 0) {
            // printf("Jogador 1 perdeu\n");
            // state = lose;
            // lose_screen();
        } else if (score_2 < 0) {
            // printf("Jogador 2 perdeu\n");
            // state = lose;
            // lose_screen();
        } else if (score_1 >= 10 && score_2 >= 10) {
            printf("Empate\n");
            state = win;
            win_screen();
        } else if (score_1 >= 10) {
            printf("Jogador 1 venceu\n");
            state = win;
            win_screen();
        } else if (score_2 >= 10) {
            printf("Jogador 2 venceu\n");
            state = win;
            win_screen();
        }
    }
    return NULL;
}


void init_game() {

    bg_animation_module_init();
    clean_all_meteors(screen_obs, meteor_on_screen_status);
    //clear();
    
    score_1 = 0;
    score_2 = 0;

    reestart_threads();
    player_1_sprite.visibility = 1;
    player_2_sprite.visibility = 1;
    
    //clear_sides();
    //erase_background(deep_space);
    //set_background_color(black);
    //set_background_color(deep_space);
    //clear();
    //bg_animation_module_init();
    //set_sprite(player_1_sprite.coord_x, player_1_sprite.coord_y, player_1_sprite.offset, player_1_sprite.data_register, player_1_sprite.visibility, player_1_sprite.speed);
    //set_sprite(player_2_sprite.coord_x, player_2_sprite.coord_y, player_2_sprite.offset, player_2_sprite.data_register, player_2_sprite.visibility, player_2_sprite.speed);
    set_sprite(player_1_sprite.coord_x, player_1_sprite.coord_y, player_1_sprite.offset, player_1_sprite.data_register, player_1_sprite.visibility);
    set_sprite(player_2_sprite.coord_x, player_2_sprite.coord_y, player_2_sprite.offset, player_2_sprite.data_register, player_2_sprite.visibility);
    //set_sprite(star_sprite_t.coord_x, star_sprite_t.coord_y, star_sprite_t.offset, star_sprite_t.data_register, star_sprite_t.visibility);
    //set_sprite(smaller_star_sprite_t.coord_x, smaller_star_sprite_t.coord_y, smaller_star_sprite_t.offset, smaller_star_sprite_t.data_register, smaller_star_sprite_t.visibility);
    //set_sprite(explosion_sprite_t.coord_x, explosion_sprite_t.coord_y, explosion_sprite_t.offset, explosion_sprite_t.data_register, explosion_sprite_t.visibility);
    set_sprite(galaxy_sprite_t.coord_x, galaxy_sprite_t.coord_y, galaxy_sprite_t.offset, galaxy_sprite_t.data_register, galaxy_sprite_t.visibility);
    set_sprite(saturn_orange_sprite_t.coord_x, saturn_orange_sprite_t.coord_y, saturn_orange_sprite_t.offset, saturn_orange_sprite_t.data_register, saturn_orange_sprite_t.visibility);
    set_sprite(saturn_green_sprite_t.coord_x, saturn_green_sprite_t.coord_y, saturn_green_sprite_t.offset, saturn_green_sprite_t.data_register, saturn_green_sprite_t.visibility);
    set_sprite(saturn_blue_sprite_t.coord_x, saturn_blue_sprite_t.coord_y, saturn_blue_sprite_t.offset, saturn_blue_sprite_t.data_register, saturn_blue_sprite_t.visibility);
    return;
}
// void clean_all_meteors(meteor_t meteoros_na_tela[], int sprites_meteor_status[]){
//     sprite_t new_sprite;
//     int reg_base = 20;
    
//     for (int i = 0; i < 10; i++)
//     {   
//         /*Restart sprite*/
//         new_sprite.coord_x = 0;
//         new_sprite.coord_y = 0;
//         new_sprite.offset = 0;
//         new_sprite.speed = 0;
//         new_sprite.data_register = reg_base + i; //20 + i
//         new_sprite.visibility = 0;

//         meteoros_na_tela[i].on_frame = 0;
//         sprites_meteor_status[i] = 0;

//         set_sprite(new_sprite.coord_x, new_sprite.coord_y, new_sprite.offset, new_sprite.data_register, new_sprite.visibility, new_sprite.speed);
//     }
// }

void menu() {
    state = in_menu;
    unsigned int btn_pressed;
    while(state != finish) {
        KEYS_read(&btn_pressed);
        // scanf("%c", &btn_pressed);

        if (btn_pressed == BUTTON0 && state == in_menu) {
            state=running;
            //clear();
            clear_sides();
            init_game();
        } else if (btn_pressed == BUTTON1 && state == running) {
            state = in_pause;
            pause_screen();
        } else if (btn_pressed == BUTTON1 && state == in_pause) {
            state = running;
            return_screen();
        } else if (btn_pressed == BUTTON2) {
            state = in_menu;
            pause_threads();
            clear();
            clean_all_meteors(screen_obs, meteor_on_screen_status);

            score_1 = 0;
            score_2 = 0;
            //draw_super_galaxy_bg();
            set_menu();
        } else if (btn_pressed == BUTTON3) {

            state = finish;
        } 
        //printf("\nButton pressed: %d", btn_pressed);

    }
    clear();
    clean_all_meteors(screen_obs, meteor_on_screen_status);
    clear_all_celestials();
    draw_cover_to_background();
    usleep(500000);
    clear();
    return;
}

int main() {

    //inicializacao dos inteiros
    score_1 = 0;
    score_2 = 0;
    pause_mouse_1 = 1;
    pause_accel = 1;
    pause_meteor = 1;
    pause_colision = 1;
    key_press_1 = 0;
    key_press_2 = 0;
    state = in_menu;

    uint8_t devid = 0;         // ID do dispositivo (ADXL345)


    // module_init_mouse_1();
    // //module_init_mouse_2();
    // open_men_vgafpga();
    // //display_open();
    // KEYS_open();
    // initialize_meteor_vector(obs);
    // set_new_sprites();
    // clear();
    // //draw_cover_to_background();
    // set_menu();
    // draw_cover_to_background();
    // draw_text_meteor_striker();
    // usleep(7000000);
    // clear();

    // Configura o MUX para conectar o controlador I2C0 ao acelerômetro ADXL345
    if ((fd_sysmgr = open_physical(fd_sysmgr)) == -1)  // Abre acesso físico ao System Manager
    {
        return (-1);  // Retorna erro se falhar
    }

    if ((sysmgrbase_virtual = map_physical(fd_sysmgr, SYSMGR_BASE, SYSMGR_SPAN)) == NULL) // Mapeia a memória do System Manager
    {
        return (-1);  // Retorna erro se falhar
    }

    sysmgr_base_ptr = (unsigned int *)sysmgrbase_virtual;  // Define o ponteiro para a base do System Manager

    if ((fd_i2c0base = open_physical(fd_i2c0base)) == -1)  // Abre acesso físico ao I2C0
    {
        return (-1);  // Retorna erro se falhar
    }

    if ((i2c0base_virtual = map_physical(fd_i2c0base, I2C0_BASE, I2C0_SPAN)) == NULL) // Mapeia a memória do I2C0
    {
        return (-1);  // Retorna erro se falhar
    }

    i2c0_base_ptr = (unsigned int *)i2c0base_virtual;  // Define o ponteiro para a base do I2C0
    printf("Begining I2C0 communication...\n");        // Inicia a comunicação com o I2C0
    I2C0_Init();                                       // Inicializa o controlador I2C0
    printf("Getting ID...\n");                         // Exibe mensagem para indicar que está lendo o ID do acelerômetro
    ADXL345_IdRead(&devid);                            // Lê o ID do dispositivo ADXL345
    printf("Device ID: %#x\n", devid);                 // Exibe o ID do dispositivo

    module_init_mouse_1();
    //module_init_mouse_2();
    open_men_vgafpga();
    //display_open();
    KEYS_open();
    initialize_meteor_vector(obs);
    set_new_sprites();
    clear();
    //draw_cover_to_background();
    
    //draw_cover_to_background();
    //draw_text_meteor_striker();
    draw_Meteor_Logo();
    usleep(3000000);
    clear();
    set_menu();

    //inicialização dos mutex
    pthread_mutex_init(&gpu_mutex, NULL);
    pthread_mutex_init(&mouse_1_mutex, NULL);
    pthread_mutex_init(&accelerometer_mutex, NULL);
    pthread_mutex_init(&meteor_mutex, NULL);
    pthread_mutex_init(&colision_mutex, NULL);

    //inicialização das condições
    pthread_cond_init(&mouse_1_cond, NULL);
    pthread_cond_init(&accelerometer_cond, NULL);
    pthread_cond_init(&meteor_cond, NULL);
    pthread_cond_init(&colision_cond, NULL);

    //inicialização das threads
    
    pthread_create(&mouse_1_thread, NULL, mouse_1_polling_routine, NULL);
    if (devid == 0xE5) // Verifica se o ID do dispositivo corresponde ao acelerômetro ADXL345
    {
        ADXL345_init(devid); // Inicializa o acelerômetro
        pthread_create(&accelerometer_thread, NULL, accelerometer_polling_routine, NULL);
    }

    pthread_create(&meteor_thread, NULL, random_meteor_generate_routine, NULL);
    pthread_create(&colision_thread, NULL, colision_routine, NULL);


    //loop principal do jogo
    menu();
    // bg_animation_module_init();
    // state=running;
    // while(state==running){
    //     init_game();
    // }
    
    
    
    //TODO: tela de finalizar o jogo
    
    pthread_cancel(mouse_1_thread);
    //pthread_cancel(accelerometer_thread);
    pthread_cancel(player_1_timer_thread);
    //pthread_cancel(player_2_timer_thread);
    pthread_cancel(meteor_thread);


    pause_colision = 0;
    pause_mouse_1 = 0;
    //pause_accel = 0;
    pause_meteor = 0;
    pthread_cond_broadcast(&colision_cond);
    pthread_cond_broadcast(&meteor_cond);
    pthread_cond_broadcast(&mouse_1_cond);
    //pthread_cond_broadcast(&accelerometer_cond);

    pthread_join(colision_thread, NULL);
    //encerrando os mutex
    pthread_mutex_destroy(&gpu_mutex);
    pthread_mutex_destroy(&mouse_1_mutex);
    //pthread_mutex_destroy(&accelerometer_mutex);
    pthread_mutex_destroy(&meteor_mutex);
    pthread_mutex_destroy(&colision_mutex);
    printf("encerrou mutex\n");

    //encerrando as condicionais
    pthread_cond_destroy(&mouse_1_cond);
    //pthread_cond_destroy(&accelerometer_cond);
    pthread_cond_destroy(&meteor_cond);
    pthread_cond_destroy(&colision_cond);
    printf("encerrou tudo e ta no close\n");

    printf("encerrou os mouses\n");
    close_men_vgafpga();
    printf("encerrou a gpu\n");
    //display_close();
    //printf("encerrou o display\n");
    KEYS_close();
    printf("encerrou os botões\n");
    // Limpeza da memória mapeada e fechamento de arquivos
    unmap_physical(i2c0base_virtual, I2C0_SPAN);
    close_physical(fd_i2c0base);
    unmap_physical(sysmgrbase_virtual, SYSMGR_SPAN);
    close_physical(fd_sysmgr);

    return 0;
}
