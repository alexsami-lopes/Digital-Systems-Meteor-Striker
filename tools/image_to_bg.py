from numpy import asarray
from PIL import Image

img_name = input("image name: ")
file_name = input("file name destiny: ")
function_name = input("name for function: ")

img = Image.open(f"images/{img_name}.png")
#img = Image.open(f"images/Player_1/{img_name}.png")

rgb_img = img.convert('RGB')

width, height = img.size

sizex = width // 80
sizey = height // 60

px = img.load()
px_rgb = rgb_img.load()

cores = {}

print(img.getpalette)
print(rgb_img.getpalette)

with open(f"{file_name}.txt", "w") as arquivo:
    arquivo.write(f'int {function_name}() {{\n')

count = 0
for i in range(0, 80):
    for j in range(0, 60):
        print(f'{i}, {j}')
        
        # Garantir índices dentro do limite da imagem
        x_index = min(i * sizey + sizey // 2, width - 1)
        y_index = min(j * sizex + sizex // 2, height - 1)
        
        if px[x_index, y_index][3] == 0:
            r = 6
            g = 7
            b = 7
        else:
            r = int(round(((px_rgb[x_index, y_index][0]) * (7 / 254)), 0))
            g = int(round(((px_rgb[x_index, y_index][1]) * (7 / 254)), 0))
            b = int(round(((px_rgb[x_index, y_index][2]) * (7 / 254)), 0))

        cor = "".join(f"cor{count}")
        rgb = "".join(f"{r} {g} {b}")

        if cores.get(rgb):
            instrucao = f"\tdraw_square_8x8({j * 80 + i}, {cores.get(rgb)});\n"
        else:
            cores[rgb] = cor
            bgr_value = (b << 6) | (g << 3) | r
            instrucao = f"\tunsigned int cor{count} = 0b{bgr_value:09b};\n\tdraw_square_8x8({j * 80 + i}, cor{count});\n"
            count += 1

        with open(f"{file_name}.txt", "a") as arquivo:
            arquivo.write(instrucao)

with open(f"{file_name}.txt", "a") as arquivo:
    arquivo.write("\treturn 0;\n}\n")
