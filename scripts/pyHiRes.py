#! /usr/bin/env python3

import os.path
import argparse
from PIL import Image

# HiRes colors
BLACK = [0x00, 0x00, 0x00]
WHITE = [0xFF, 0xFF, 0xFF]
GREEN = [0x00, 0xFF, 0x00]
BLUE = [0x00, 0x00, 0xFF]
ORANGE = [0xFF, 0x77, 0x00]
VIOLET = [0xFF, 0x00, 0xFF]

Msg_Warning = ""
Color_Last = BLACK

def error(msg, err_code):
    print(msg)
    exit(err_code)


# Compute the Euclidean distance between 2 colors
def distance(color1, color2):
    return (color1[0] - color2[0])**2 + (color1[1] - color2[1])**2 + (color1[2] - color2[2])**2


# Returns the nearest color
def nearest_color(color):
    dist_black = distance(BLACK, color)
    dist_white = distance(WHITE, color)
    dist_green = distance(GREEN, color)
    dist_blue = distance(BLUE, color)
    dist_orange = distance(ORANGE, color)
    dist_violet = distance(VIOLET, color)
    dist_min = min([dist_black, dist_white, dist_green, dist_blue, dist_violet, dist_orange])
    if dist_min == dist_black:
        return BLACK
    elif dist_min == dist_white:
        return WHITE
    elif dist_min == dist_green:
        return GREEN
    elif dist_min == dist_blue:
        return BLUE
    elif dist_min == dist_orange:
        return ORANGE
    else:
        return VIOLET

# Returns True if the 7 pixel block can be converted into hires with no artifact
# Returns False otherwise
def compliant_hires(block):
    global Msg_Warning
    global Color_Last
    # Test if colors from the two groups are in the block, aka "clashing"
    group_one = False
    group_two = False
    nb_pixels = len(block)//3
    for i in range(0, nb_pixels):
        color = block[3*i:3*(i+1)]
        if color == ORANGE or color == BLUE:
            group_one = True
        if color == GREEN or color == VIOLET:
            group_two = True
    if group_one and group_two:
        Msg_Warning = "Colors from two groups, some clashing will occur!"
        return False
    # Test if there are two consecutive different colors in the group
    Msg_Warning = "Two consecutive different colors may lead to an artifact"
    color = block[0:3]
    if Color_Last != BLACK and Color_Last != WHITE \
       and color != BLACK and color != WHITE \
       and color != Color_Last:
        return False
    for i in range(0, nb_pixels-1):
        color = block[3*i:3*(i+1)]
        if color != BLACK and color != WHITE:
            color_next = block[3*(i+1):3*(i+2)]
            if color_next != BLACK and color_next != WHITE \
               and color != BLACK and color != WHITE \
               and color != color_next:
                return False
    Color_Last = block[3*(nb_pixels-1):3*nb_pixels]
    return True


# PARSING COMMAND LINE ARGUMENTS
parser = argparse.ArgumentParser(description="This script converts a 24bit RGB image file into an Apple II HiRes image.")
parser.add_argument("file", help="file to convert")
parser.add_argument("-o", "--output", help="output filename", required=False)
args = parser.parse_args()


# SANITIZE PARAMETERS
if not os.path.exists(args.file):
    error("File " + args.file + " does not exist!", -1)


# MAIN
try:
    # Open source image
    image_rgb = Image.open(args.file).convert('RGB')
    w = image_rgb.size[0]
    h = image_rgb.size[1]

    if w != 140 or h != 192:
        error("Source image must be 140x192 pixels", -1)

    # Quantize image to the HiRes's 6 colors
    image_quantized = []
    for y in range(0, h):
        for x in range(0, w):
            color = nearest_color(image_rgb.getpixel((x, y)))
            for i in range(0, 3):
                image_quantized.append(color[i])
    if len(image_quantized) != 3*140*192:
        error("Bad quantized size", -1)

    # image_test = Image.frombytes('RGB', (140, 192), bytes(image_quantized))
    # image_test.show()

    # Test image conpliance to HiRes limitations
    nb_pixels = 7  # nb pixels per line block
    for i in range(0, 140 * 192 // nb_pixels):
        block = image_quantized[3*i*nb_pixels:
                                3*(i+1)*nb_pixels]
        if not compliant_hires(block):
            print("Warning @block #" + str(i+1) + ": " + Msg_Warning)

except ValueError:
    error(ValueError, -1)
