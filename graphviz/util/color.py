import colorsys
import random


def get_random_color() -> tuple[int, int, int]:
    r, g, b = colorsys.hsv_to_rgb(random.random(), 0.5, 1)
    return int(r * 255), int(g * 255), int(b * 255)

