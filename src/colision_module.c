//#include "../Lib/Colenda_Lib/colenda.h"
#include "../include/colision_module.h"


int check_colision_player(sprite_t player, meteor_t meteor) {
    
    /*caso alguma parte da area do jogador esteja dentro da area do objeto*/
    if(((player.coord_x - 10 < meteor.coord_x - 10 && player.coord_x + 10 > meteor.coord_x - 10 && player.coord_x + 10 < meteor.coord_x + 10) || (player.coord_x - 10 < meteor.coord_x + 10 && player.coord_x - 10 > meteor.coord_x - 10 && player.coord_x + 10 > meteor.coord_x + 10) ) && ((player.coord_y - 10 <= meteor.coord_y + meteor.size) && (player.coord_y + 10 >= meteor.coord_y - meteor.size))) {
        
        return 1; /*retorna 1 indicando que houve uma colisão*/
    }
    
    return 0;     /*caso contrario, retorna 0*/
}


int check_collision_bottom(meteor_t meteor) {
    // Verifica se a parte inferior do meteoro atingiu ou passou de 460 pixels em y
    if (meteor.coord_y >= 460) {
        return meteor.coord_x; // Retorna a coordenada x onde ocorreu a colisão
    }
    return 0; // Retorna 0 caso nenhuma colisão tenha ocorrido
}
