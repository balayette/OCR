#include "imgprocessing/display.h"
#include "imgprocessing/drawing.h"
#include "imgprocessing/processing.h"
#include "imgprocessing/recxy.h"

#include "neuralnet/neuralnet.h"
#include "neuralnet/save.h"

#include "matrix/matrixop.h"
#include "misc/bool_matrix.h"

static const char TOKENS[] =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
static const int TOKENS_LEN = 62;
static const int LETTER_OFFSET = 0;
static const int NUMBER_OFFSET = 52;
static const int CAPS_OFFSET = 26;

struct neural_net *nn;


void mat_to_double(t_bool_matrix *mat, double *output, int width, int height) {
    int h_padding = width - mat->cols;
    int left_padding = h_padding / 2;
    int right_padding = h_padding - left_padding;

    int v_padding = height - mat->lines;
    int top_padding = v_padding / 2;
    int bottom_padding = v_padding - top_padding;

    assert(mat->lines + top_padding + bottom_padding == height);
    assert(mat->cols + left_padding + right_padding == width);

    int index = 0;
    for (int top = 0; top < top_padding; top++) {
        output[index] = 0.0;
        index++;
    }
    for (int line = 0; line < mat->lines; line++) {
        for (int left = 0; left < left_padding; left++) {
            output[index] = 0.0;
            index++;
        }
        for (int col = 0; col < mat->cols; col++) {
            output[index] = M_bool_GET(mat, col, line) ? 1.0 : 0.0;
            index++;
        }
        for (int right = 0; right < right_padding; right++) {
            output[index] = 0.0;
            index++;
        }
    }
    for (int bot = 0; bot < bottom_padding; bot++) {
        output[index] = 0.0;
        index++;
    }
}

int get_letter_index(char letter) {
    if (letter <= '9')
        return letter - 48 + NUMBER_OFFSET;
    else if (letter <= 'Z')
        return letter - 65 + CAPS_OFFSET;
    else if (letter <= 'z')
        return letter - 97 + LETTER_OFFSET;
    return -1;
}

char get_letter(double *arr, int size) {
    int index = 0;
    double max = *arr;
    for (int i = 1; i < size; i++) {
        if (arr[i] > max) {
            max = arr[i];
            index = i;
        }
    }
    return TOKENS[index];
}

void aol(struct rxy_bintree *b){
    _recxy_only_v(b, 1);
}

void guess(struct rxy_bintree *bintree){
    t_bool_matrix *m = trim_all(bintree->values);
    if(!m) return;
    t_bool_matrix *sca = scale(m, 25, 25);
    /* pprint_bool_matrix(sca); */

    double *input = malloc(625 * sizeof(double));
    mat_to_double(sca, input, 25, 25);
    forward_prop(nn, input);
    printf("OUTPUT : %c\n", get_letter(nn->layers[nn->hidden_layer_count + 1]->values, TOKENS_LEN));
}

int main(int argc, char *argv[]){
    if(argc < 3){
        printf("USAGE : ./all SAVE_FILE IMAGE");
    }

    nn = load_nn(argv[1]);
    SDL_Surface *screen = NULL;
    SDL_Surface *img = load_image(argv[2]);
    display_and_wait(&screen, img);

    gray_level(img);
    display_and_wait(&screen, img);

    int t = otsu(img);
    binarize(img, t);
    display_and_wait(&screen, img);

    t_bool_matrix *mat = surface_to_matrix(img);


    struct rxy_bintree *bintree = recxy(mat, true);
    apply_on_leaves(bintree, aol);

    draw_boxes_leaves(img, bintree, 255, 0, 0);
    printf("AAAA\n\n");
    display_and_wait(&screen, img);
    apply_on_leaves(bintree, guess);
    free_nn(nn);
    SDL_FreeSurface(img);

    return 0;
}
