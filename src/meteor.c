#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "../include/meteor.h"
#include "../include/sprites_offsets.h"

meteor_t meteor = {
    .offset = METEOR,
    .speed= 25,
    .reward = 1,
    .on_frame = 0
};


int random_number(int min, int max){
    int num;

    //coordenada aleatória gerada
    num = (rand() % (max - min +1 ) + min);
    return num;
}

int check_for_empty_reg(int sprites_meteor_status[]){
    int i = 5;//, status, reg;

    while(i < 8){
        if(sprites_meteor_status[i] == 0){
            //reg = base_reg + i;
            sprites_meteor_status[i] = 1; //status registrador ocupado
            return i; 
        }
        i++;
    }
    return -1; //não há registrador vazio.
}

int create_sprite_meteor(meteor_t meteor, int coord_x, int coord_y, int base_reg, int sprites_meteor_status[], meteor_t meteors_on_screen[]){
    sprite_t meteor_sprite;
    int index, reg;
    
    base_reg = 21;
    index = check_for_empty_reg(sprites_meteor_status);
    if(index == -1) return -1; //não há registrador vazio. Nao é possivel criar o sprite
    else{
        reg = base_reg + index;
        meteor_sprite.coord_x = coord_x;
        meteor_sprite.coord_y = coord_y;
        meteor_sprite.offset = meteor.offset;
        meteor_sprite.speed = meteor.speed;
        meteor_sprite.data_register = reg;
        meteor_sprite.visibility = 1;
        //set_sprite(meteor_sprite);
        //set_sprite(meteor_sprite.coord_x, meteor_sprite.coord_y, meteor_sprite.offset, meteor_sprite.data_register, meteor_sprite.visibility, meteor_sprite.speed);
        set_sprite(meteor_sprite.coord_x, meteor_sprite.coord_y, meteor_sprite.offset, meteor_sprite.data_register, meteor_sprite.visibility);
        sprites_meteor_status[index] = 1;

        //identificando meteoro ativo na tela
        meteors_on_screen[index].coord_x = coord_x;
        meteors_on_screen[index].coord_y = coord_y;
        meteors_on_screen[index].offset = meteor.offset;
        meteors_on_screen[index].speed = meteor.speed;
        meteors_on_screen[index].reward = meteor.reward;
        meteors_on_screen[index].size = meteor.size; 
        meteors_on_screen[index].on_frame = 1;     
    }
    
    return 1; //criado com sucesso
 
}

void initialize_meteor_vector(meteor_t vetor_meteors[]){
    srand(time(NULL));

    vetor_meteors[0] = meteor;

}

meteor_t select_random_meteor_from_vector(meteor_t vetor_meteors[]){
    meteor_t selected_meteor; 
    int i;

    //i = random_number(0, 16);
    i = 0;
    if(vetor_meteors[i].offset == 11) i += 1;
    selected_meteor.coord_x = 0;
    selected_meteor.coord_y = 0;
    selected_meteor.offset = vetor_meteors[i].offset;
    selected_meteor.reward = vetor_meteors[i].reward;
    selected_meteor.size = vetor_meteors[i].size;
    selected_meteor.speed = vetor_meteors[i].speed;
    selected_meteor.on_frame = 0;
    return selected_meteor;   
}


int range_min_coord_x(int coord_x_player, int min_limit_action_sector){
    int range_min;
     
    if((coord_x_player - 50) <= min_limit_action_sector){
        range_min = min_limit_action_sector + 10;
    }
    else{
        range_min = coord_x_player - 50;
    }
    return range_min;
}

int range_max_coord_x(int coord_x_player, int max_limit_action_sector){
    int range_max;
     
    if(coord_x_player + 50 >= max_limit_action_sector){
        range_max = max_limit_action_sector - 11;
    }else range_max = coord_x_player + 50;
    return range_max;
}

void move_meteors(meteor_t meteors_on_screen[], int sprites_meteor_status[], int unity, int reg_base_meteors){
    sprite_t new_sprite;
    int deslocamento, new_coord_y;

    for (int i = 0; i < 10; i++)
    {
        if(sprites_meteor_status[i] == 1){
            deslocamento = meteors_on_screen[i].speed * unity;
            new_coord_y = meteors_on_screen[i].coord_y + deslocamento;
            if(meteors_on_screen[i].coord_y >= 480 || new_coord_y >= 471){
                meteors_on_screen[i].coord_x = 0;
                meteors_on_screen[i].coord_y = 0;
                meteors_on_screen[i].on_frame = 0;

                /* Cria um sprite invisivel */
                new_sprite.coord_x = 0;
                new_sprite.coord_y = 0;
                //new_sprite.offset = 0;
                new_sprite.visibility = 0;
                
                // Libera o registrador para uso
                sprites_meteor_status[i] = 0;

            }else{
                meteors_on_screen[i].coord_y = new_coord_y;
                meteors_on_screen[i].on_frame = 1;

                /* Atualiza o sprite */
                new_sprite.coord_x = meteors_on_screen[i].coord_x;
                new_sprite.coord_y = new_coord_y;
                new_sprite.offset = meteors_on_screen[i].offset;
                new_sprite.speed = meteors_on_screen[i].speed;
                new_sprite.data_register = reg_base_meteors + i; //21 + i
                new_sprite.visibility = 1;
            } 
            //set_sprite(new_sprite);
            //set_sprite(new_sprite.coord_x, new_sprite.coord_y, new_sprite.offset, new_sprite.data_register, new_sprite.visibility, new_sprite.speed);
            set_sprite(new_sprite.coord_x, new_sprite.coord_y, new_sprite.offset, new_sprite.data_register, new_sprite.visibility);
        }   
        //printf("Antes: x = %d, y = %d\n", meteors_on_screen[i].coord_x, meteors_on_screen[i].coord_y);
        //printf("Deslocamento: %d\n", deslocamento);
        //printf("Depois: x = %d, y = %d\n", new_sprite.coord_x, new_sprite.coord_y);
    
    }    
    
}

int all_meteors_invisibles(meteor_t meteors_on_screen[], int sprites_meteor_status[]){
    sprite_t invisible_sprite;
    int reg_base_meteors = 21;

    for (int i = 0; i < 8; i++)
    {   
        invisible_sprite.coord_x = meteors_on_screen[i].coord_x;
        invisible_sprite.coord_y = meteors_on_screen[i].coord_y;
        invisible_sprite.offset = meteors_on_screen[i].offset;
        invisible_sprite.speed = meteors_on_screen[i].speed;
        invisible_sprite.data_register = reg_base_meteors + i; //21 + i
        invisible_sprite.visibility = 0;

        //preserva o meteoro, porém com invisibilidade
        meteors_on_screen[i].on_frame = 1;
        sprites_meteor_status[i] = 1;

        //set_sprite(invisible_sprite);
        //set_sprite(invisible_sprite.coord_x, invisible_sprite.coord_y, invisible_sprite.offset, invisible_sprite.data_register, invisible_sprite.visibility, invisible_sprite.speed);
        set_sprite(invisible_sprite.coord_x, invisible_sprite.coord_y, invisible_sprite.offset, invisible_sprite.data_register, invisible_sprite.visibility);
    }
    return -1;
}

int restart_all_invisible_meteors(meteor_t meteors_on_screen[], int sprites_meteor_status[]){
    sprite_t visible_sprite;
    int reg_base_meteors = 21;

    for (int i = 0; i < 8; i++)
    {   
        visible_sprite.coord_x = meteors_on_screen[i].coord_x;
        visible_sprite.coord_y = meteors_on_screen[i].coord_y;
        visible_sprite.offset = meteors_on_screen[i].offset;
        visible_sprite.speed = meteors_on_screen[i].speed;
        visible_sprite.data_register = reg_base_meteors + i; //21 + i
        visible_sprite.visibility = 1;

        //preserva o meteoro
        meteors_on_screen[i].on_frame = 1;
        sprites_meteor_status[i] = 1;

        //set_sprite(visible_sprite);
        //set_sprite(visible_sprite.coord_x, visible_sprite.coord_y, visible_sprite.offset, visible_sprite.data_register, visible_sprite.visibility, visible_sprite.speed);
        set_sprite(visible_sprite.coord_x, visible_sprite.coord_y, visible_sprite.offset, visible_sprite.data_register, visible_sprite.visibility);
    }
    return -1;
}

void clean_all_meteors(meteor_t meteors_on_screen[], int sprites_meteor_status[]){
    sprite_t new_sprite;
    int reg_base = 21;
    
    for (int i = 0; i < 8; i++)
    {   
        /*Restart sprite*/
        new_sprite.coord_x = 0;
        new_sprite.coord_y = 0;
        new_sprite.offset = 0;
        new_sprite.speed = 0;
        new_sprite.data_register = reg_base + i; //21 + i
        new_sprite.visibility = 0;

        meteors_on_screen[i].on_frame = 0;
        sprites_meteor_status[i] = 0;

        //set_sprite(new_sprite);
        //set_sprite(new_sprite.coord_x, new_sprite.coord_y, new_sprite.offset, new_sprite.data_register, new_sprite.visibility, new_sprite.speed);
        set_sprite(new_sprite.coord_x, new_sprite.coord_y, new_sprite.offset, new_sprite.data_register, new_sprite.visibility);
    }
}


//int random_meteor(int cord_x_player, int cord_y_player, int min_limit_action_sector, int max_limit_action_sector, meteor_t meteors_on_screen[], int sprites_meteor_status[], meteor_t vetor_meteors[]){
int random_meteor(int cord_x_player, int min_limit_action_sector, int max_limit_action_sector, meteor_t meteors_on_screen[], int sprites_meteor_status[], meteor_t vetor_meteors[]){
    meteor_t new_meteor;//, selected_meteor;
    //sprite_t new_sprite;

    
    int coord_x_meteor, coord_y_meteor;

    int base_reg_meteors = 21, unity = 1;
    //gera coordenada aleatória entre 
    coord_x_meteor = random_number(range_min_coord_x(cord_x_player, min_limit_action_sector), range_max_coord_x(cord_x_player, max_limit_action_sector));
    coord_y_meteor = 10; // 0 + 10

    //Executar antes do random_meteor
    //initialize_meteor_vector(vetor_meteors);
    new_meteor = select_random_meteor_from_vector(vetor_meteors);

    create_sprite_meteor(new_meteor,coord_x_meteor, coord_y_meteor, base_reg_meteors, sprites_meteor_status, meteors_on_screen);
    move_meteors(meteors_on_screen, sprites_meteor_status, unity, base_reg_meteors);

    return 1; // sucess
}

