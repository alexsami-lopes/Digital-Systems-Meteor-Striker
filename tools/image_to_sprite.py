from numpy import asarray
from PIL import Image

img_name = input("image name: ")
file_name = input("file name destiny: ")

img = Image.open(f"images/{img_name}.png")

rgb_img = img.convert('RGB')

len = img.size

sizex = len[0] // 20
sizey = len[1] // 20

px = img.load()
px_rgb = rgb_img.load()

cores = {}

with open(f"{file_name}.txt", "w") as arquivo:
    arquivo.write('unsigned int pixels[400];\n')

count = 0
for j in range(0, 20):
    for i in range(0, 20):

        # Verifica se o pixel é transparente
        if px[(i * sizey + sizey // 2), (j * sizex + sizex // 2)][3] == 0:
            r = 6  # Valor padrão para vermelho (máximo)
            g = 7  # Valor padrão para verde (máximo)
            b = 7  # Valor padrão para azul (máximo)
        else:
            # Converte cada canal para 3 bits (0-7)
            r = int(round((px_rgb[(i * sizey + sizey // 2), (j * sizex + sizex // 2)][0]) * (7 / 254), 0))
            g = int(round((px_rgb[(i * sizey + sizey // 2), (j * sizex + sizex // 2)][1]) * (7 / 254), 0))
            b = int(round((px_rgb[(i * sizey + sizey // 2), (j * sizex + sizex // 2)][2]) * (7 / 254), 0))

        # Calcula o valor BGR em 9 bits
        bgr_value = (b << 6) | (g << 3) | r

        cor = f"cor{count}"
        bgr_str = f"{bgr_value:#011b}"  # Representação binária com 0b e 9 bits

        if cores.get(bgr_value):
            instrucao = f"pixels[{(j * 20) + i}] = {cores.get(bgr_value)};\n"
        else:
            cores[bgr_value] = cor
            instrucao = f"unsigned int {cor} = {bgr_str};\npixels[{(j * 20) + i}] = {cor};\n"
            count += 1

        with open(f"{file_name}.txt", "a") as arquivo:
            arquivo.write(instrucao)

        print(f"B: {b}\tG: {g}\tR: {r}\tBGR: {bgr_str}")

print(img.getpalette())
print(rgb_img.getpalette())
