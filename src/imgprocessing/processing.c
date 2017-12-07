#include "imgprocessing/drawing.h"
#include "imgprocessing/pixop.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <math.h>
#include <stdbool.h>


void map_image(SDL_Surface *img,
               Uint32 (*f)(SDL_Surface *, Uint8 *, Uint8 *, Uint8 *)) {
    Uint8 r, g, b;
    for (int x = 0; x < img->h; x++) {
        for (int y = 0; y < img->w; y++) {
            Uint32 pix = getpixel(img, y, x);
            SDL_GetRGB(pix, img->format, &r, &g, &b);
            Uint32 npix = f(img, &r, &g, &b);
            putpixel(img, y, x, npix);
        }
    }
}

Uint32 get_gray_level(SDL_Surface *img, Uint8 *r, Uint8 *g, Uint8 *b) {
    double average = 0.3 * *r + 0.59 * *g + 0.11 * *b;
    return SDL_MapRGB(img->format, average, average, average);
}

void gray_level(SDL_Surface *img) { map_image(img, get_gray_level); }

Uint32 get_inverse(SDL_Surface *img, Uint8 *r, Uint8 *g, Uint8 *b) {
    *r = 255 - *r;
    *g = 255 - *g;
    *b = 255 - *b;
    return SDL_MapRGB(img->format, *r, *g, *b);
}

void inverse(SDL_Surface *img) { map_image(img, get_inverse); }

void binarize(SDL_Surface *img, int threshold) {
    Uint8 r;
    for (int x = 0; x < img->h; x++) {
        for (int y = 0; y < img->w; y++) {
            Uint32 pix = getpixel(img, y, x);
            SDL_GetRGB(pix, img->format, &r, &r, &r);
            Uint8 v = r < threshold ? 0 : 255;
            Uint32 npix = SDL_MapRGB(img->format, v, v, v);
            putpixel(img, y, x, npix);
        }
    }
}

void contrast(SDL_Surface *img, int value) {
    double factor = (259.0 * (value + 255.0)) / (255.0 * (259.0 - value));
    Uint8 r, g, b;
    for (int x = 0; x < img->h; x++) {
        for (int y = 0; y < img->w; y++) {
            Uint32 pix = getpixel(img, y, x);
            SDL_GetRGB(pix, img->format, &r, &g, &b);
            r = factor * (r - 128) + 128;
            g = factor * (g - 128) + 128;
            b = factor * (b - 128) + 128;
            Uint32 npix = SDL_MapRGB(img->format, r, g, b);
            putpixel(img, y, x, npix);
        }
    }
}

SDL_Surface *blur(SDL_Surface *input, int radius) {
    SDL_Surface *output =
        SDL_ConvertSurface(input, input->format, SDL_SWSURFACE);
    int avgr = 0;
    int avgg = 0;
    int avgb = 0;
    int count = 0;
    Uint8 r, g, b;
    for (int i = 0; i < input->w; i++) {
        for (int j = 0; j < input->h; j++) {
            for (int ii = -radius; ii <= radius; ii++) {
                for (int ij = -radius; ij <= radius; ij++) {
                    int newi = ii + i;
                    int newj = ij + j;
                    if (newi < 0 || newi > input->w || newj < 0 ||
                        newj > input->h)
                        continue;
                    if (newj == j || newi == i)
                        continue;
                    Uint32 pix = getpixel(input, newi, newj);
                    SDL_GetRGB(pix, input->format, &r, &g, &b);
                    avgr += r;
                    avgb += b;
                    avgg += g;
                    count++;
                }
            }
            avgr /= count;
            avgg /= count;
            avgb /= count;
            Uint32 newpix = SDL_MapRGB(output->format, avgr, avgg, avgb);
            putpixel(output, i, j, newpix);
            avgb = 0;
            avgg = 0;
            avgr = 0;
            count = 0;
        }
    }
    return output;
}

int otsu(SDL_Surface *img) {
    int hist[256] = {0};
    double proba[256] = {0};
    double omega[256] = {0};
    double sigma[256] = {0};
    double individual_prob = 1.0 / (img->w * img->h);
    double average_pix = 0.0;
    Uint8 r;
    for (int x = 0; x < img->w; x++) {
        for (int y = 0; y < img->h; y++) {
            Uint32 pix = getpixel(img, x, y);
            SDL_GetRGB(pix, img->format, &r, &r, &r);
            hist[r]++;
            proba[r] += individual_prob;
            average_pix += (r * individual_prob);
        }
    }
    omega[0] = proba[0];
    sigma[0] = 0.0;
    for (int i = 1; i < 256; i++) {
        omega[i] = omega[i - 1] + proba[i];
        sigma[i] = sigma[i - 1] + i * proba[i];
    }

    int optimal_thresh = 0;
    double sigmax = 0;
    for (int i = 0; i < 255; i++) {
        if (omega[i] != 0.0 && omega[i] != 1.0) {
            sigma[i] = pow(sigma[255] * omega[i] - sigma[i], 2) /
                       (omega[i] * (1.0 - omega[i]));
        } else {
            sigma[i] = 0.0;
        }
        if (sigma[i] > sigmax) {
            sigmax = sigma[i];
            optimal_thresh = i;
        }
    }
    return optimal_thresh;
}

SDL_Surface *copy_surface(SDL_Surface *img) {
    SDL_Surface *ret;
    SDL_PixelFormat *fmt = img->format;
    ret = SDL_CreateRGBSurface(img->flags, img->w, img->h, fmt->BitsPerPixel,
                               fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
    SDL_BlitSurface(img, NULL, ret, NULL);
    return ret;
}

SDL_Surface *hrlsa(SDL_Surface *img, int c) {
    SDL_Surface *copy = copy_surface(img);
    Uint8 color;
    for (int x = 0; x < img->h; x++) {
        for (int y = 0; y < img->w; y++) {
            int count = 0;
            Uint32 pix = getpixel(img, y, x);
            SDL_GetRGB(pix, img->format, &color, &color, &color);
            if (color == 0) {
                putpixel(copy, y, x, pix);
                continue;
            }
            for (int k = -c; k <= c; k++) {
                int newy = y + k;
                if (newy < 0 || newy > copy->w) {
                    count++;
                    continue;
                }
                Uint32 pix = getpixel(img, newy, x);
                SDL_GetRGB(pix, img->format, &color, &color, &color);
                if (color == 255)
                    count++;
                else
                    count = 0;
                if (count >= c)
                    break;
            }
            if (count >= c) {
                putpixel(copy, y, x, SDL_MapRGB(img->format, 255, 255, 255));
            } else {
                putpixel(copy, y, x, SDL_MapRGB(img->format, 0, 0, 0));
            }
        }
    }
    return copy;
}

SDL_Surface *vrlsa(SDL_Surface *img, int c) {
    SDL_Surface *copy = copy_surface(img);
    Uint8 color;
    for (int y = 0; y < img->w; y++) {
        for (int x = 0; x < img->h; x++) {
            int count = 0;
            Uint32 pix = getpixel(img, y, x);
            SDL_GetRGB(pix, img->format, &color, &color, &color);
            if (color == 0) {
                putpixel(copy, y, x, pix);
                continue;
            }
            for (int k = -c; k <= c; k++) {
                int newx = x + k;
                if (newx < 0 || newx > copy->w) {
                    count++;
                    continue;
                }
                Uint32 pix = getpixel(img, y, newx);
                SDL_GetRGB(pix, img->format, &color, &color, &color);
                if (color == 255)
                    count++;
                else
                    count = 0;
                if (count >= c)
                    break;
            }
            if (count >= c) {
                putpixel(copy, y, x, SDL_MapRGB(img->format, 255, 255, 255));
            } else {
                putpixel(copy, y, x, SDL_MapRGB(img->format, 0, 0, 0));
            }
        }
    }
    return copy;
}

SDL_Surface *recombine(SDL_Surface *h, SDL_Surface *v) {
    SDL_Surface *ret = copy_surface(h);
    Uint8 a, b;
    for (int x = 0; x < h->h; x++) {
        for (int y = 0; y < h->w; y++) {
            Uint32 p1 = getpixel(h, y, x);
            Uint32 p2 = getpixel(v, y, x);
            SDL_GetRGB(p1, h->format, &a, &a, &a);
            SDL_GetRGB(p2, v->format, &b, &b, &b);
            if (a == 0 && b == 0) {
                putpixel(ret, y, x, SDL_MapRGB(ret->format, 0, 0, 0));
            } else {
                putpixel(ret, y, x, SDL_MapRGB(ret->format, 255, 255, 255));
            }
        }
    }

    return ret;
}


SDL_Surface *rlsa(SDL_Surface *img, int c, SDL_Surface **h, SDL_Surface **v) {
    (void)c;
    *h = hrlsa(img, img->w / 5);
    *v = vrlsa(*h, img->h / 15);
    SDL_Surface *rec = recombine(*h, *v);
    return hrlsa(rec, 5);
}
