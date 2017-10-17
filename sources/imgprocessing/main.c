#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdbool.h>
#include "../../headers/imgprocessing/display.h"
#include "../../headers/misc/bool_matrix.h"
#include "../../headers/imgprocessing/pixop.h"
#include "../../headers/imgprocessing/processing.h"

int main(int argc, char *argv[]) {
    (void)argc;
    SDL_Surface *screen = NULL;
    SDL_Surface *img = load_image(argv[1]);
    display_and_wait(&screen, img);
    int t = otsu(img);
    binarize(img, t);
    display_and_wait(&screen, img);
    t_bool_matrix *m = surface_to_matrix(img);
    t_bool_matrix *h = hrlsa_bm(m, 20);
    for(int y = 0; y < m->lines; y++)
    {
        for(int x = 0; x < m->cols; x++){
            printf("M : %d | H : %d\n", M_bool_GET(m, x, y), M_bool_GET(h, x, y));
        }
    }

    SDL_Surface *ret = copy_surface(img);
    matrix_to_surface(ret, h);
    display_and_wait(&screen, ret);

    SDL_FreeSurface(img);
    SDL_FreeSurface(screen);
    SDL_Quit();
    return 0;
}
