#include <SDL/SDL_image.h>
#include "../../headers/imgprocessing/pixop.h"

void gray_level(SDL_Surface *img) {
    Uint8 r, g, b;
    for (int x = 0; x < img->h; x++) {
        for (int y = 0; y < img->w; y++) {
            Uint32 pix = getpixel(img, y, x);
            SDL_GetRGB(pix, img->format, &r, &g, &b);
            double average = 0.3 * r + 0.59 * g + 0.11 * b;
            Uint32 npix = SDL_MapRGB(img->format, average, average, average);
            putpixel(img, y, x, npix);
        }
    }
}
