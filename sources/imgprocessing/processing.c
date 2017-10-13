#include "../../headers/imgprocessing/pixop.h"
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

typedef struct s_box {
    int x;
    int y;
    int height;
    int width;
    bool has_black;
} t_box;

bool check_black_percent(SDL_Surface *img, t_box *box) {
    /*
      At the moment, the box isn't really centered, and it doesn't check the
      whole box in all cases.
      This function should take a percentage as input. Will be done later
     */
    int startx = box->x - (box->width / 2);
    int starty = box->y - (box->height / 2);
    int endx = box->x + (box->width / 2);
    int endy = box->y + (box->height / 2);
    (void)img;
    Uint8 r, g, b;
    int count = 0;
    for (int y = starty; y < endy; y++) {
        for (int x = startx; x < endx; x++) {
            Uint32 pix = getpixel(img, x, y);
            SDL_GetRGB(pix, img->format, &r, &g, &b);
            if (r == 0)
                count++;
        }
    }
    return count != 0;
}

#define CUTS 200

void draw_line(SDL_Surface *img, Uint32 color, int xstart, int xend, int y){
    for(int i = xstart; i < xend; i++)
        putpixel(img, i, y, color);
}

void divide_and_conquer(SDL_Surface *img) {
    // We want boxes of 1% of the pixels
    printf("Image : %d x %d\n", img->h, img->w);
    int hcuts = img->h / CUTS;
    int wcuts = img->w / CUTS;
    int size = hcuts < wcuts ? hcuts : wcuts;
    size = size % 2 == 1 ? size : size + 1;
    int hbox_count = img->w / size;
    int vbox_count = img->h / size;
    int box_count = vbox_count * hbox_count;
    t_box *boxes[vbox_count][hbox_count];
    printf("t_box *boxes[%d][%d]\n", vbox_count, hbox_count);
    printf("Going to create %d boxes of %dx%d pixels\n", box_count, size, size);
    int i = 0;
    int j = 0;
    int created_count = 0;
    for (int y = size / 2; y + size / 2 < img->h; y += size) {
        for (int x = size / 2; x + size / 2 < img->w; x += size) {
            t_box *box = malloc(sizeof(t_box));
            box->x = x;
            box->y = y;
            box->height = size;
            box->width = size;
            box->has_black = check_black_percent(img, box);
            if(box->has_black)
                putpixel(img, x, y, SDL_MapRGB(img->format, 255, 0, 0));
            boxes[i][j] = box;
            j++;
            created_count++;
        }
        i++;
        j = 0;
    }

    int *line_len = calloc(vbox_count, sizeof(int));

    printf("Created %d boxes\n", created_count);
    for(int i = 0; i < vbox_count; i++){
        // For each line, the length of the line is the distance from the
        // first block with black in the line to the last
        t_box *first = NULL;
        t_box *last = NULL;
        for(int j = 0; j < hbox_count; j++){
            printf("i : %d j : %d\n", i, j);
            if(boxes[i][j]->has_black){
                printf("Box has black\n");
                if(first)
                    last = boxes[i][j];
                else{
                    first = boxes[i][j];
                    last = first;
                }
                continue;
            }
            printf("Box doesn't have black\n");
        }
        if(first && last){
            line_len[i] = last->x - first->x;
            draw_line(img, SDL_MapRGB(img->format, 255, 0, 0), first->x, last->x, first->y);
        }
        printf("Len of line %d : %d\n", i, line_len[i]);
    }

}
