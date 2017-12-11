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
#define SIZE H*V

static const char TOKENS[] =
    "!\"#$%&'()*+,-.abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
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

/* void mat_to_double(t_bool_matrix *mat, double *output, int width, int height)
 * { */
/*     int h_padding = width - mat->cols; */
/*     int left_padding = h_padding / 2; */
/*     int right_padding = h_padding - left_padding; */

/*     int v_padding = height - mat->lines; */
/*     int top_padding = v_padding / 2; */
/*     int bottom_padding = v_padding - top_padding; */

/*     assert(mat->lines + top_padding + bottom_padding == height); */
/*     assert(mat->cols + left_padding + right_padding == width); */

/*     int index = 0; */
/*     for (int top = 0; top < top_padding; top++) { */
/*         output[index] = 0.0; */
/*         index++; */
/*     } */
/*     for (int line = 0; line < mat->lines; line++) { */
/*         for (int left = 0; left < left_padding; left++) { */
/*             output[index] = 0.0; */
/*             index++; */
/*         } */
/*         for (int col = 0; col < mat->cols; col++) { */
/*             output[index] = M_bool_GET(mat, col, line) ? 1.0 : 0.0; */
/*             index++; */
/*         } */
/*         for (int right = 0; right < right_padding; right++) { */
/*             output[index] = 0.0; */
/*             index++; */
/*         } */
/*     } */
/*     for (int bot = 0; bot < bottom_padding; bot++) { */
/*         output[index] = 0.0; */
/*         index++; */
/*     } */
/* } */

int get_letter_index(char letter) {
    if(letter <= '.')
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
    /* pprint_bool_matrix(sca); */

    double *input = malloc(SIZE * sizeof(double));
    mat_to_double(sca, input);

    forward_prop(nn, input);

    if (m != mat)
        M_bool_FREE(m);
    M_bool_FREE(sca);

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

void parse(struct rxy_bintree *bintree) {
    if (!bintree)
        return;
    if (bintree->line_end)
        printf("\n");
    else if (!bintree->left && !bintree->right)
        guess(bintree);
    parse(bintree->left);
    parse(bintree->right);
}

void parse_spaces(struct rxy_bintree *bintree, t_bool_matrix **arr,
                  int *index) {
    if (!bintree)
        return;
    if (!bintree->left && !bintree->right) {
        t_bool_matrix *mat = bintree->values;
        pprint_bool_matrix(mat);
        if(black_count(mat) < 10)
        {
            printf("Doesn't look like a letter\n");
            goto keep_going;
        }
        t_bool_matrix *letter = trim_all(mat);
        /* printf("Matrix : \n"); */
        /* pprint_bool_matrix(mat); */

        /* printf("\n\nLetter : \n"); */
        /* pprint_bool_matrix(letter); */
        /* printf("\n\n"); */
        if (!letter) {
            /* printf("Letter was a space or something\n"); */

        } else {
            if (mat->cols - letter->cols > 10) {
                /* printf("Inserting a space at index %d\n", *index); */
                arr[(*index)++] = NULL;
            }
            /* printf("Inserting the letter at index %d\n", *index); */
            arr[(*index)++] = letter;
            /* pprint_bool_matrix(letter); */
            /* printf("Value = %c\n", prediction(letter)); */
        }
    }
keep_going:
    parse_spaces(bintree->left, arr, index);
    parse_spaces(bintree->right, arr, index);
}

void traverse(t_bool_matrix **arr, int len) {
    printf("Traversing %d\n", len);
    for (int i = 0; i < len; i++) {
        if (!arr[i])
            /* printf("Found a space\n"); */
            printf(" ");
        else {
            /* printf("Found a letter : %p\n", (void *)arr[i]); */
            /* pprint_bool_matrix(arr[i]); */
            char ret = prediction(arr[i]);
            if (ret == -1) {
                continue;
            }
            /* printf("LETTER : %c\n", ret); */
            printf("%c", ret);
        }
    }
}



void DISPLAY(struct rxy_bintree *bintree, int space){
    if(!bintree){
        return;
    }
    if(bintree->line_end){
        printf("\n");
        goto recursion;
    }
    else{
        if(bintree->left || bintree->right)
            goto recursion;
        int white_left = side_white_cols(bintree->values);
        /* printf("Space : %d\n", white_left); */
        t_bool_matrix *mat = bintree->values;
        if(white_left >= mat->cols)
        {
            printf(" ");
            goto recursion;
        }
        if(white_left >= space)
            printf(" ");
        char ret = prediction(mat);
        if(ret != -1)
            printf("%c", ret);
        else
            printf("?");
    }
recursion:
    DISPLAY(bintree->left, space);
    DISPLAY(bintree->right, space);
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

    DISPLAY(bintree, space);
    /* t_bool_matrix **arr = calloc(1000000, sizeof(t_bool_matrix *)); */
    /* int i = 0; */
    /* parse_spaces(bintree, arr, &i); */

    /* printf("Len : %d\n", i); */



    /* traverse(arr, i); */

    /* printf("\n"); */
    display_and_wait(&screen, img);

    free_nn(nn);
    SDL_FreeSurface(img);

    return 0;
}
