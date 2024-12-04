import pygame
import sys

# Inicializar o pygame
pygame.init()

# Dimensões da imagem original e da janela de projeção
IMAGE_SIZE_X = 80  # Largura da imagem
IMAGE_SIZE_Y = 60  # Altura da imagem
PIXEL_SIZE = 8     # Tamanho de cada pixel ampliado
WINDOW_SIZE_X = IMAGE_SIZE_X * PIXEL_SIZE  # Largura da janela
WINDOW_SIZE_Y = IMAGE_SIZE_Y * PIXEL_SIZE  # Altura da janela

# Nome do arquivo de imagem
IMAGE_FILE = "Meteor_Logo2.png"

# Criar a janela
window = pygame.display.set_mode((WINDOW_SIZE_X, WINDOW_SIZE_Y))
pygame.display.set_caption("Projetor de Imagem")

def draw_image(image_surface):
    """Desenha a imagem na janela com pixels ampliados."""
    for y in range(IMAGE_SIZE_Y):  # Percorre as linhas (altura)
        for x in range(IMAGE_SIZE_X):  # Percorre as colunas (largura)
            # Obter a cor do pixel
            color = image_surface.get_at((x, y))
            # Calcular posição do quadrado ampliado
            rect = pygame.Rect(x * PIXEL_SIZE, y * PIXEL_SIZE, PIXEL_SIZE, PIXEL_SIZE)
            # Desenhar o quadrado
            pygame.draw.rect(window, color, rect)

def main():
    try:
        # Carregar a imagem
        image_surface = pygame.image.load(IMAGE_FILE).convert_alpha()
        # Redimensionar a imagem para garantir 80x60
        image_surface = pygame.transform.scale(image_surface, (IMAGE_SIZE_X, IMAGE_SIZE_Y))
    except pygame.error as e:
        print(f"Erro ao carregar a imagem: {e}")
        pygame.quit()
        sys.exit()

    # Loop principal
    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        # Preencher a janela com cor preta
        window.fill((0, 0, 0))
        # Desenhar a imagem ampliada
        draw_image(image_surface)
        # Atualizar a janela
        pygame.display.flip()

    pygame.quit()

if __name__ == "__main__":
    main()
