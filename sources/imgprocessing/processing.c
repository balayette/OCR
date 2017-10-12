#include <SDL/SDL_image.h>
#include "../../headers/imgprocessing/pixop.h"
#include <math.h>

void map_image(SDL_Surface *img, Uint32 (*f)(SDL_Surface *, Uint8 *, Uint8 *, Uint8 *)){
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

Uint32 get_gray_level(SDL_Surface *img, Uint8 *r, Uint8 *g, Uint8 *b){
    double average = 0.3 * *r + 0.59 * *g + 0.11 * *b;
    return SDL_MapRGB(img->format, average, average, average);
}

void gray_level(SDL_Surface *img) {
    map_image(img, get_gray_level);
}

Uint32 get_inverse(SDL_Surface *img, Uint8 *r, Uint8 *g, Uint8 *b){
    *r = 255 - *r;
    *g = 255 - *g;
    *b = 255 - *b;
    return SDL_MapRGB(img->format, *r, *g, *b);
}

void inverse(SDL_Surface *img){
    map_image(img, get_inverse);
}

void binarize(SDL_Surface *img, int threshold){
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

void contrast(SDL_Surface *img, int value){
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

void blur(SDL_Surface *img, int radius){
    int avgr = 0;
    int avgg = 0;
    int avgb = 0;
    int count = 0;
    Uint8 r, g, b;
    for(int i = 0; i < img->w; i++){
        for(int j = 0; j < img->h; j++){
            for(int ii = -radius; ii <= radius; ii++){
                for(int ij = -radius; ij <= radius; ij++){
                    int newi = ii + i;
                    int newj = ij + j;
                    if(newi < 0 || newi > img->w || newj < 0 || newj > img->h)
                        continue;
                    if(newj == j || newi == i)
                        continue;
                    Uint32 pix = getpixel(img, newi, newj);
                    SDL_GetRGB(pix, img->format, &r, &g, &b);
                    avgr += r;
                    avgb += b;
                    avgg += g;
                    count++;
                }
            }
            avgr /= count;
            avgg /= count;
            avgb /= count;
            Uint32 newpix = SDL_MapRGB(img->format, avgr, avgg, avgb);
            putpixel(img, i, j, newpix);
            avgb = 0;
            avgg = 0;
            avgr = 0;
            count = 0;
        }
    }
}


int otsu(SDL_Surface *img){
    int hist[256] = {0};
    double proba[256] = {0};
    double omega[256] = {0};
    double sigma[256] = {0};
    double individual_prob = 1.0 / (img->w * img->h);
    double average_pix = 0.0;
    Uint8 r;
    for(int x = 0; x < img->w; x++){
        for(int y = 0; y < img->h; y++){
            Uint32 pix = getpixel(img, x, y);
            SDL_GetRGB(pix, img->format, &r, &r, &r);
            hist[r]++;
            proba[r] += individual_prob;
            average_pix += (r * individual_prob);
        }
    }
    printf("Histogram : \n");
    for(int i = 0; i < 256; i++)
        printf("%d : %d\n", i, hist[i]);

    printf("Probas : \n");
    for(int i = 0; i < 256; i++)
        printf("%d : %f\n", i, proba[i]);

    printf("Average value : %f\n", average_pix);
    omega[0] = proba[0];
    sigma[0] = 0.0;
    for(int i = 1; i < 256; i++){
        omega[i] = omega[i - 1] + proba[i];
        sigma[i] = sigma[i - 1] + i * proba[i];
    }

    printf("Omega : \n");

    for(int i = 0; i < 256; i++)
        printf("%d : %f\n", i, omega[i]);

    printf("Sigma : \n");
    for(int i = 0; i < 256; i++)
        printf("%d : %f\n", i, sigma[i]);

    int optimal_thresh = 0;
    double sigmax = 0;
    for(int i = 0; i < 255; i++){
        if(omega[i] != 0.0 && omega[i] != 1.0){
            sigma[i] = pow(sigma[255] * omega[i] - sigma[i], 2) / (omega[i] * (1.0 - omega[i]));
        }
        else{
            sigma[i] = 0.0;
        }
        if(sigma[i] > sigmax){
            sigmax = sigma[i];
            optimal_thresh = i;
        }
    }
    return optimal_thresh;
}
