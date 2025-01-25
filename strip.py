from PIL import Image
import struct

img = Image.open("./data/testingimage.jpg")
img = img.convert("RGB")
img = img.resize((400, 400))

w, h = img.size

pixels = list(img.getdata())

bytes = bytearray()

for r, g, b in pixels:
    bytes.extend([r, g, b])

with open("iso/file/image.bin", "wb") as bin_file:
    bin_file.write(bytes)

print(f"Written {w * h * 3} bytes ({w * h * 3 / 1000} Kbyte)") 
