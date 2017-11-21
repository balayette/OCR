#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline Uint8 *pixelref(SDL_Surface *surf, unsigned x, unsigned y) {
    int bpp = surf->format->BytesPerPixel;
    return (Uint8 *)surf->pixels + y * surf->pitch + x * bpp;
}

Uint32 getpixel(SDL_Surface *surface, unsigned x, unsigned y) {
    Uint8 *p = pixelref(surface, x, y);
    switch (surface->format->BytesPerPixel) {
    case 1:
        return *p;
    case 2:
        return *(Uint16 *)p;
    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
    case 4:
        return *(Uint32 *)p;
    }
    return 0;
}

void putpixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel) {
    Uint8 *p = pixelref(surface, x, y);
    switch (surface->format->BytesPerPixel) {
    case 1:
        *p = pixel;
        break;
    case 2:
        *(Uint16 *)p = pixel;
        break;
    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;
    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

#define INIT_MATRIX(Type)                                                      \
    t_##Type##_matrix *CREATE_##Type##_MATRIX(int lines, int cols) {           \
        t_##Type##_matrix *ret = malloc(sizeof(t_##Type##_matrix));            \
        ret->values = calloc(cols * lines, sizeof(bool));                      \
        ret->cols = cols;                                                      \
        ret->lines = lines;                                                    \
        return ret;                                                            \
    }                                                                          \
    Type M_##Type##_GET(t_##Type##_matrix *m, int x, int y) {                  \
        assert(x < m->cols);                                                   \
        assert(y < m->lines);                                                  \
        return m->values[x + y * m->cols];                                     \
    }                                                                          \
    void M_##Type##_SET(t_##Type##_matrix *m, int x, int y, Type value) {      \
        assert(x < m->cols);                                                   \
        assert(y < m->lines);                                                  \
        m->values[x + y * m->cols] = value;                                    \
    }                                                                          \
    void M_##Type##_FREE(t_##Type##_matrix *m) {                               \
        free(m->values);                                                       \
        free(m);                                                               \
    }

#define INIT_MATRIX_HEADER(Type)                                               \
    typedef struct s_##Type##_matrix {                                         \
        Type *values;                                                          \
        int cols;                                                              \
        int lines;                                                             \
    } t_##Type##_matrix;                                                       \
    t_##Type##_matrix *CREATE_##Type##_MATRIX(int lines, int cols);            \
    Type M_##Type##_GET(t_##Type##_matrix *m, int x, int y);                   \
    void M_##Type##_FREE(t_##Type##_matrix *m);                                \
    void M_##Type##_SET(t_##Type##_matrix *m, int x, int y, Type value);

INIT_MATRIX_HEADER(bool)
INIT_MATRIX(bool)

t_bool_matrix *surface_to_matrix(SDL_Surface *img) {
    t_bool_matrix *ret = CREATE_bool_MATRIX(img->h, img->w);
    Uint8 r;
    for (int i = 0; i < img->h; i++) {
        for (int j = 0; j < img->w; j++) {
            SDL_GetRGB(getpixel(img, j, i), img->format, &r, &r, &r);
            M_bool_SET(ret, j, i, !r);
        }
    }
    return ret;
}
void init_sdl(void) {
    if (SDL_Init(SDL_INIT_VIDEO) == -1)
        errx(1, "Couldn't initialize SDL: %s.\n", SDL_GetError());
    SDL_WM_SetCaption("Image Processing", NULL);
}

void print_mouse_pos(void) {
    int a = 0;
    int b = 0;
    SDL_GetMouseState(&a, &b);
    printf("Position x : %d y : %d\n", a, b);
}

void wait_for_keypressed(void) {
    SDL_Event event;
    for (;;) {
        SDL_WaitEvent(&event);
        switch (event.type) {
        case SDL_MOUSEBUTTONUP:
            print_mouse_pos();
            return;
        case SDL_KEYDOWN:
            return;
        default:
            break;
        }
    }
}

SDL_Surface *load_image(char *path) {
    SDL_Surface *img;
    img = IMG_Load(path);
    if (!img)
        errx(3, "can't load %s: %s", path, IMG_GetError());
    return img;
}

void display(SDL_Surface **screen, SDL_Surface *img) {
    if (!(*screen))
        *screen =
            SDL_SetVideoMode(img->w, img->h, 0, SDL_SWSURFACE | SDL_ANYFORMAT);
    if (img->w != (*screen)->w || img->h != (*screen)->h)
        *screen =
            SDL_SetVideoMode(img->w, img->h, 0, SDL_SWSURFACE | SDL_ANYFORMAT);
    SDL_BlitSurface(img, NULL, *screen, NULL);
    SDL_Flip(*screen);
}

void display_and_wait(SDL_Surface **screen, SDL_Surface *img) {
    display(screen, img);
    wait_for_keypressed();
}

SDL_Surface *new_surface() {
    return SDL_CreateRGBSurface(0, 20, 20, 32, 0, 0, 0, 0);
}

void save_bool_matrix(char *path, t_bool_matrix *mat) {
    FILE *f = fopen(path, "wb+");
    fwrite(mat->values, sizeof(bool), mat->lines * mat->cols, f);
    fclose(f);
}

void draw(char l, char *tmp, char *path, char *path2, TTF_Font *font, SDL_Color c) {
    printf("%c\n", l);
    tmp[0] = l;
    SDL_Surface *text = TTF_RenderText_Solid(font, tmp, c);
    path[5] = l;
    printf("%s\n", path);
    SDL_SaveBMP(text, path);
    path2[5] = l;
    printf("bool : %lu\n", sizeof(bool));
    save_bool_matrix(path2, surface_to_matrix(text));
    SDL_FreeSurface(text);
}

int main() {
    init_sdl();
    TTF_Init();
    /* SDL_Surface *letter = new_surface(); */
    /* SDL_FillRect(letter, NULL, SDL_MapRGB(letter->format, 255, 255, 255)); */
    TTF_Font *font = TTF_OpenFont("fonts/OpenSans-Regular.ttf", 20);
    /* printf("%s\n", TTF_GetError()); */
    SDL_Color c = {0, 0, 0, 0};
    /* SDL_BlitSurface(text, NULL, letter, &position); */

    char *path = calloc(11, sizeof(char));
    char *path2 = calloc(11, sizeof(char));
    strcat(path, "data/#.bmp");
    strcat(path2, "data/#.bin");
    char tmp[2] = {0, 0};
    for (char l = 'A'; l <= 'Z'; l++) {
        draw(l, tmp, path, path2, font, c);
        draw(l + 26 + 6, tmp, path, path2, font, c);
    }
    return 0;
}
