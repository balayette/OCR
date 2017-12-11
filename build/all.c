#include "imgprocessing/display.h"
#include "imgprocessing/drawing.h"
#include "imgprocessing/processing.h"
#include "imgprocessing/recxy.h"

#include "neuralnet/neuralnet.h"
#include "neuralnet/save.h"

#include "matrix/matrixop.h"
#include "misc/bool_matrix.h"

#define H 35
#define V 45
#define SIZE H *V

static const char TOKENS[] = "!\"#$%&'()*+,-."
                             "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVW"
                             "XYZ0123456789";
static const int TOKENS_LEN = 73;
static const int SYMBOL_OFFSET = 0;
static const int LETTER_OFFSET = 14;
static const int CAPS_OFFSET = 26 + 14;
static const int NUMBER_OFFSET = 26 + 26 + 14;

struct neural_net *nn;

void mat_to_double(t_bool_matrix *mat, double *output) {
    /* pprint_bool_matrix(mat); */
    for (int i = 0; i < mat->cols * mat->lines; i++)
        output[i] = mat->values[i] ? 0.0 : 1.0;
}

int get_letter_index(char letter) {
    if (letter <= '.')
        return letter - 33 + SYMBOL_OFFSET;
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

void aol(struct rxy_bintree *b) { _recxy_only_v(b, 1); }

char prediction(t_bool_matrix *mat) {
    t_bool_matrix *m = trim_all(mat);
    if (!m)
        m = mat;
    t_bool_matrix *sca = scale(m, H, V);

    double *input = malloc(SIZE * sizeof(double));
    mat_to_double(sca, input);

    forward_prop(nn, input);

    if (m != mat)
        M_bool_FREE(m);
    M_bool_FREE(sca);

    free(input);
    return get_letter(nn->layers[nn->hidden_layer_count + 1]->values,
                      TOKENS_LEN);
}

void guess(struct rxy_bintree *bintree) {
    if (!bintree->values)
        return;
    char ret = prediction(bintree->values);
    if (ret == -1)
        return;
    printf("%c", prediction(bintree->values));
}

void set_line_end(struct rxy_bintree *bintree) { bintree->line_end = true; }

void DISPLAY(struct rxy_bintree *bintree, int space, FILE *f) {
    if (!bintree) {
        return;
    }
    if (bintree->line_end) {
        fputc('\n', f);
        goto recursion;
    } else {
        if (bintree->left || bintree->right)
            goto recursion;
        int white_left = side_white_cols(bintree->values);
        /* printf("Space : %d\n", white_left); */
        t_bool_matrix *mat = bintree->values;
        if (white_left >= mat->cols) {
            fputc(' ', f);
            goto recursion;
        }
        if (white_left >= space)
            fputc(' ', f);
        char ret = prediction(mat);
        if (ret != -1)
            fputc(ret, f);
        else
            fputc('?', f);
    }
recursion:
    DISPLAY(bintree->left, space, f);
    DISPLAY(bintree->right, space, f);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("USAGE : ./all SAVE_FILE IMAGE\n");
        return 1;
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
    apply_on_leaves(bintree, set_line_end);
    int line_count = bcount_leaves(bintree) - 1;
    apply_on_leaves(bintree, trim_white_lines);
    apply_on_leaves(bintree, aol);
    int letter_count = bcount_leaves(bintree);
    printf("%d lines, %d letters\n", line_count, letter_count);
    printf("%d letters / line\n", letter_count / line_count);
    int space = img->w / (letter_count / line_count);
    space /= 3;
    printf("%d space\n", space);
    draw_boxes_leaves(img, bintree, 255, 0, 0);

    FILE *f = fopen("output.txt", "w");

    DISPLAY(bintree, space, f);
    fclose(f);
    display_and_wait(&screen, img);

    free_nn(nn);
    M_bool_FREE(mat);
    free_bintree(bintree);
    SDL_FreeSurface(img);
    SDL_FreeSurface(screen);
    SDL_Quit();

    return 0;
}
