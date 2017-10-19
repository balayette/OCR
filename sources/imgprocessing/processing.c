#include "../../headers/imgprocessing/boxes.h"
#include "../../headers/imgprocessing/drawing.h"
#include "../../headers/imgprocessing/pixop.h"
#include "../../headers/misc/bool_matrix.h"
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

#define CUTS 200

// Init_DivideAndConquer
t_boxes *init_dac(SDL_Surface *img, int *size, int *hbox_count,
                  int *vbox_count) {
    // printf("Image : %d x %d\n", img->h, img->w);
    int hcuts = img->h / CUTS;
    int wcuts = img->w / CUTS;
    *size = hcuts < wcuts ? hcuts : wcuts;
    *size = *size % 2 == 1 ? *size : *size + 1;
    *hbox_count = img->w / *size;
    *vbox_count = img->h / *size;
    t_boxes *boxes = new_boxes(*vbox_count, *hbox_count);
    return boxes;
}

/* void generate_boxes(SDL_Surface *img, t_boxes *boxes){ */

/* } */
// NextBot_with_BlackBottomNeigh
/* void nb_with_bbn(SDL_Surface *img, t_boxes ***boxes, int i, int j) {} */

int get_nth_height(int *ln, int lnlen, int n) {
    int k = -1;
    for (int i = 0; i < lnlen; i++) {
        if (ln[i] == 0)
            continue;
        k++;
        if (k == n)
            return ln[i];
    }
    return 0;
}

// DO NOT USE
// EXPERIMENTAL
void divide_and_conquer(SDL_Surface *img) {
    int size, hbox_count, vbox_count;
    t_boxes *boxes = init_dac(img, &size, &hbox_count, &vbox_count);
    int i = 0;
    int j = 0;
    int created_count = 0;
    for (int y = 0; y + size < img->h; y += size) {
        for (int x = 0; x + size < img->w; x += size) {
            t_box *box = new_box(x, y, size, size);
            if (check_black_percent(img, box)) {
                /* draw_box(img, box); */
                setbox(boxes, i, j, box);
                created_count++;
            }
            // TODO : Change this to fill the squares without black
            // with NULL instead. Will have to adapt the rest of the code
            else {
                setbox(boxes, i, j, NULL);
                free(box);
            }

            j++;
        }
        i++;
        j = 0;
    }

    int *line_len = calloc(vbox_count, sizeof(int));

    // printf("Created %d boxes\n", created_count);
    for (int i = 0; i < vbox_count; i++) {
        // For each line, the length of the line is the distance from the
        // first block with black in the line to the last
        t_box *first = NULL;
        t_box *last = NULL;
        for (int j = 0; j < hbox_count; j++) {
            t_box *current = getbox(boxes, i, j);
            if (!current)
                continue;
            /* //printf("Box has black\n"); */
            if (first)
                last = current;
            else {
                first = current;
                last = first;
            }
            // printf("Box doesn't have black\n");
        }
        if (first && last) {
            line_len[i] = last->x - first->x;
            /* draw_line(img, SDL_MapRGB(img->format, 255, 0, 0), first->x, */
            /* last->x, first->y); */
        }
        // printf("Len of line %d : %d\n", i, line_len[i]);
    }
    // We now have the length of each individual line.
    // We want to combine those lines together into a single, real lines
    // If a box has a bottom neigh that has black, we assume that they're part
    // of the same line
    // At the moment, this only checks for bigger lines towards the right
    // We want it to check left, too.
    int *line_hs = calloc(vbox_count, sizeof(int));
    int actual_lines_count = 0;
    // TODO : Carefuly check how this algo works. (It does, but it isn't)
    // completely clear to me now that I read it again
    // It looks like it does way too many computations
    for (int i = 0; i < vbox_count; i++) {
        // There are no black chars on this line
        if (line_len[i] == 0)
            continue;
        // printf("Line %d has a line\n", i);
        int line_height = 0;
        for (int j = 0; j < hbox_count; j++) {
            t_box *current = getbox(boxes, i, j);
            if (!current)
                continue;
            // Current has black pixels. We go down until the bottom neigh
            // doesn't
            while (current && j < hbox_count) {
                if (!getbox(boxes, i + 1, j))
                    break;
                i++;
                line_height++;
                current = getbox(boxes, i, j);
            }
        }
        line_hs[i] = line_height;
        line_height = 0;
        actual_lines_count++;
    }

    // printf("We found %d actual lines of text\n", actual_lines_count);
    for (int i = 0; i < vbox_count; i++)
        if (line_hs[i] != 0)
            printf("Line height : %d\n", line_hs[i]);

    // At this point, we have the len of all lines of boxes, and the height of
    // actual lines.
    // We need to compute the actual start of the line on the x axis
    // (could be before the start of first line we found) and the end of it.

    t_box *line_start[actual_lines_count];
    t_box *line_end[actual_lines_count];
    int line = 0;
    for (int i = 0; i < vbox_count; i++) {
        for (int j = 0; j < vbox_count; j++) {
            t_box *current = getbox(boxes, i, j);
            if (!current)
                continue;
            printf("Current is the leftmost top\n");
            // We have found leftmost block of the top of the line
            // Let's scan from x = 0 to x = boxes->w for blocks
            // using a line_hs[i] pixels high scanner
            // The first box we find is the start of the line, the last
            // box we find is the end of it
            t_box *start = NULL;
            t_box *end = NULL;
            int height = get_nth_height(line_hs, vbox_count, line);
            // This fixes a sefgault
            if (!height)
                break;
            printf("Scanning the %dth line\n", line);
            printf("Line height : %d\n", height);
            for (int k = 0; k < hbox_count; k++) {
                for (int x = 0; x < height; x++) {
                    t_box *scanned = getbox(boxes, x + i, k);
                    if (!scanned)
                        continue;
                    if (!start) {
                        start = scanned;
                        end = scanned;
                    } else {
                        end = scanned;
                    }
                }
            }
            if (!start || !end)
                break;
            line_start[line] = start;
            line_end[line] = end;
            line++;
            i += height + 1;
        }
    }
    for (int i = 0; i < actual_lines_count; i++) {
        if (!line_start[i] || !line_end[i])
            continue;
        printf("Line start : %d | Line end : %d\n", line_start[i]->x,
               line_end[i]->x);
        /* draw_line(img, SDL_MapRGB(img->format, 0, 0, 255), line_start[i]->x,
         * line_end[i]->x, line_start[i]->y); */
        /* t_box *start = line_start[i]; */
        /* t_box *end = line_start[i]; */
    }
}

// White pixels are 0s and black pixels are 1s
// Makes it possible to use binary logic and speeds up the RLSA
t_bool_matrix *surface_to_matrix(SDL_Surface *img){
    t_bool_matrix *ret = CREATE_bool_MATRIX(img->h, img->w);
    Uint8 r;
    for(int i = 0; i < img->h; i++){
        for(int j = 0; j < img->w; j++)
        {
            SDL_GetRGB(getpixel(img, j, i), img->format, &r, &r, &r);
            M_bool_SET(ret, j, i, !r);
        }
    }
    return ret;
}

void matrix_to_surface(SDL_Surface *surface, t_bool_matrix *m){
    for(int y = 0; y < m->lines; y++){
        for(int x = 0; x < m->cols; x++){
            int val = M_bool_GET(m, x, y) ? 0 : 255;
            /* printf("%d\n", val); */
            putpixel(surface, x, y, SDL_MapRGB(surface->format, val, val, val));
        }
    }
}

SDL_Surface *copy_surface(SDL_Surface *img) {
    SDL_Surface *ret;
    SDL_PixelFormat *fmt = img->format;
    ret = SDL_CreateRGBSurface(img->flags, img->w, img->h, fmt->BitsPerPixel,
                               fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
    SDL_BlitSurface(img, NULL, ret, NULL);
    return ret;
}

t_bool_matrix *hrlsa_bm(t_bool_matrix *img, int c){
    t_bool_matrix *copy = CREATE_bool_MATRIX(img->lines, img->cols);
    memcpy(copy->values, img->values, img->lines * img->cols);
    for(int y = 0; y < img->lines; y++){
        for(int x = 0; x < img->cols; x++){
            bool val = M_bool_GET(img, x, y);
            if(val)
                continue;
            int count = 0;
            for(int k = -c; k <= c; k++){
                int newx = x + k;
                if(newx < 0 || newx >= img->cols)
                    count++;
                else{
                    if(!M_bool_GET(img, newx, y))
                        count++;
                    else
                        count = 0;
                }
                if(count >= c)
                    break;
            }
            if(count < c)
                M_bool_SET(copy, x, y, true);
        }
    }
    return copy;
}

t_bool_matrix *vrlsa_bm(t_bool_matrix *img, int c){
    t_bool_matrix *copy = CREATE_bool_MATRIX(img->lines, img->cols);
    memcpy(copy->values, img->values, img->lines * img->cols);
    for(int x = 0; x < img->cols; x++){
        for(int y = 0; y < img->lines; y++){
            bool val = M_bool_GET(img, x, y);
            if(val)
                continue;
            int count = 0;
            for(int k = -c; k <= c; k++){
                int newy = y + k;
                if(newy < 0 || newy >= img->lines)
                  count++;
                else{
                    if(!M_bool_GET(img, x, newy))
                        count++;
                    else
                        count = 0;
                }
                if(count >= c)
                    break;
            }
            if(count < c)
                M_bool_SET(copy, x, y, true);
        }
    }
    return copy;
}

t_bool_matrix *recombine_bm(t_bool_matrix *a, t_bool_matrix *b){
  t_bool_matrix *ret = CREATE_bool_MATRIX(a->lines, a->cols);
  for(int y = 0; y < ret->lines; y++){
    for(int x = 0; x < ret->cols; x++){
      M_bool_SET(ret, x, y, M_bool_GET(a, x, y) & M_bool_GET(b, x, y));
    }
  }
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
    *v = vrlsa(img, img->h / 10);
    SDL_Surface *rec = recombine(*h, *v);
    return hrlsa(rec, 5);
}
