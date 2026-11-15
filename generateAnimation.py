import math

# Create a sine wave ranging from 2 to 99 over the span of 255 frames.
for x in range(255):

    v = math.floor(
        (math.sin(((x * 2 * math.pi) / 255) + (1.5 * math.pi)) * 50) + 50
    )

    if v == 0:
        v = 2
    if v == 1:
        v = 2

    print(v, end=",")
