#ifndef KEYS_H
#define KEYS_H

#include <stdint.h>
/* Indentificadores dos botões*/
#define BUTTON0  7
#define BUTTON1 11
#define BUTTON2 13
#define BUTTON3 14
// Abre o arquivo /dev/mem e mapeia os registradores
int KEYS_open();

// Lê o valor do registrador de botões
// btn_pressed: ponteiro para armazenar o valor do botão pressionado
// Retorna 0 em caso de sucesso, -1 em caso de erro
//int KEYS_read(uint32_t *btn_pressed);
int KEYS_read(unsigned int *btn_pressed);

// Fecha o arquivo /dev/mem e desfaz o mapeamento
void KEYS_close();

#endif // KEYS_H
