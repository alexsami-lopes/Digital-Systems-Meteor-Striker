from PIL import Image

# Solicitar nomes das imagens
current_img_name = input("Current image name: ")
previous_img_name = input("Previous image name: ")
file_name = input("File name for output: ")
function_name = input("Name for function: ")

# Abrir as imagens
current_img = Image.open(f"images/Player_2/{current_img_name}.png").convert('RGB')
previous_img = Image.open(f"images/Player_2/{previous_img_name}.png").convert('RGB')

# Obter tamanhos e verificações
current_width, current_height = current_img.size
previous_width, previous_height = previous_img.size

if current_width != previous_width or current_height != previous_height:
    raise ValueError("Current and previous images must have the same dimensions!")

# Calcular blocos 8x8
sizex = current_width // 80
sizey = current_height // 60

# Carregar pixels
current_px = current_img.load()
previous_px = previous_img.load()

cores = {}

# Criar arquivo
with open(f"{file_name}.txt", "w") as arquivo:
    arquivo.write(f'int {function_name}() {{\n')

count = 0
for i in range(80):
    for j in range(60):
        print(f'Processing block: {i}, {j}')
        
        # Coordenadas do centro do bloco
        x_index = min(i * sizey + sizey // 2, current_width - 1)
        y_index = min(j * sizex + sizex // 2, current_height - 1)

        # Pixels RGB das imagens atual e anterior
        current_rgb = current_px[x_index, y_index]
        previous_rgb = previous_px[x_index, y_index]

        # Ignorar pixels que não mudaram
        if current_rgb == previous_rgb:
            continue

        # Calcular valores RGB compactados em BGR de 9 bits
        r = int(round((current_rgb[0] * (7 / 254)), 0))
        g = int(round((current_rgb[1] * (7 / 254)), 0))
        b = int(round((current_rgb[2] * (7 / 254)), 0))

        # Criar valor BGR em 9 bits
        bgr_value = (b << 6) | (g << 3) | r

        # Adicionar instrução no arquivo
        cor = f"cor{count}"
        if f"{r} {g} {b}" not in cores:
            cores[f"{r} {g} {b}"] = cor
            instrucao = (
                f"\tunsigned int {cor} = 0b{bgr_value:09b};\n"
                f"\tdraw_square_8x8({j * 80 + i}, {cor});\n"
            )
            count += 1
        else:
            cor = cores[f"{r} {g} {b}"]
            instrucao = f"\tdraw_square_8x8({j * 80 + i}, {cor});\n"

        with open(f"{file_name}.txt", "a") as arquivo:
            arquivo.write(instrucao)

with open(f"{file_name}.txt", "a") as arquivo:
    arquivo.write("\treturn 0;\n}\n")
