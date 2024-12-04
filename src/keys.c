#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>
#include "../include/keys.h"

// Define constantes e endereços mapeados
#define FPGA_BASE_ADDR  0xFF200000  // Endereço base da ponte FPGA
#define HW_REGS_SPAN    0x100       // Tamanho da região de mapeamento
#define BUTTONS_OFFSET  0x0         // Offset para o registrador de botões

// Variáveis globais para o mapeamento e o descritor do arquivo

static volatile unsigned int *KEY_ptr = NULL; 
static int fd = -1; 

int KEYS_open() {
    fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        perror("Erro ao abrir /dev/mem");
        return -1;
    }

    void *virtual_base = mmap(NULL, HW_REGS_SPAN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, FPGA_BASE_ADDR);
    if (virtual_base == MAP_FAILED) {
        perror("Erro no mmap");
        close(fd);
        return -1;
    }

    KEY_ptr = (volatile unsigned int *)((char *)virtual_base);
    return 0;
}

int KEYS_read(unsigned int *btn_pressed) {
    if (KEY_ptr == NULL || btn_pressed == NULL) {
        fprintf(stderr, "Erro: KEYS não inicializado ou ponteiro nulo.\n");
        return -1;
    }

    *btn_pressed = *(KEY_ptr);
    return 0;
}

void KEYS_close() {
    if (KEY_ptr != NULL) {
        munmap((void *)KEY_ptr, 0x100);
        KEY_ptr = NULL;
    }
    if (fd != -1) {
        close(fd);
        fd = -1;
    }
}
