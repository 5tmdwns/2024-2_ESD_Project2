from PIL import Image

resize_to_fixed_size = True 
fixed_width, fixed_height = 240, 240  

def swap_bytes(rgb565):
    high_byte = (rgb565 >> 8) & 0xFF  
    low_byte = rgb565 & 0xFF          
    return (low_byte << 8) | high_byte  

image_path = "dongHyun.jpeg"  
output_file = "dongHyun.c"   

image = Image.open(image_path)

if resize_to_fixed_size:
    image = image.resize((fixed_width, fixed_height), Image.Resampling.LANCZOS)
    width, height = fixed_width, fixed_height  
else:
    width, height = image.size  

rgb565_data = []


for y in range(height):
    row = [] 
    for x in range(width):
        r, g, b = image.getpixel((x, y))[:3]  
        rgb565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
        swapped_rgb565 = swap_bytes(rgb565)
        row.append(swapped_rgb565) 
    rgb565_data.append(row)  

with open(output_file, "w") as file:
    file.write("#include \"image.h\"\n\n")
    file.write(f"const uint16_t dongHyun[{height}][{width}] = {{\n")
    for row in rgb565_data:
        file.write("    {")
        file.write(",".join(f"0x{value:04X}" for value in row))
        file.write("},\n")
    file.write("};\n")

print(f" {output_file} save!")
