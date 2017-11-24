#include "misc/bool_matrix.h"
#include <string.h>
#include <SDL/SDL.h>
#include "imgprocessing/pixop.h"
#include "matrix/matrixop.h"

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


t_bool_matrix *rlsa_bm(t_bool_matrix *img, int c, t_bool_matrix **h, t_bool_matrix **v){
    (void)c;
    *h = hrlsa_bm(img, img->cols / 5);
    *v = vrlsa_bm(*h, img->lines / 15);
    return recombine_bm(*h, *v);
}


t_bool_matrix *_trim_cols_before(t_bool_matrix *mat){
    if(!mat)
        return NULL;
    int x = 0;
    for(; x < mat->cols; x++){
        if(!is_white_col(mat, x))
            break;
    }
    if(x == 0 || x == mat->cols - 1){
        return NULL;
    }
    t_bool_matrix *ret = CREATE_bool_MATRIX(mat->lines, mat->cols - x);
    for(int y = 0; y < mat->lines; y++){
        for(int i = x; i < mat->cols; i++){
            M_bool_SET(ret, i - x, y, M_bool_GET(mat, i, y));
        }
    }
    return ret;
}

t_bool_matrix *_trim_lines_before(t_bool_matrix *mat){
    if(!mat){
        return NULL;
    }
    int y = 0;
    for(; y < mat->lines; y++){
        if(!is_white_line(mat, y))
            break;
    }
    if(y == 0 || y == mat->lines - 1){
        return NULL;
    }
    t_bool_matrix *ret = CREATE_bool_MATRIX(mat->lines - y , mat->cols);
    for(int x = 0; x < mat->cols; x++){
        for(int j = y; j < mat->lines; j++){
            M_bool_SET(ret, x, j - y, M_bool_GET(mat, x, j));
        }
    }
    return ret;
}

t_bool_matrix *_trim_lines_after(t_bool_matrix *mat){
    if(!mat)
        return NULL;

    int y = mat->lines - 1;
    for(; y >= 0; y--){
        printf("Checking line %d\n", y);
        if(!is_white_line(mat, y))
            break;
    }
    printf("First line with black is %d\n", y);
    if(y == mat->lines || y == 0)
        return NULL;

    t_bool_matrix *ret = CREATE_bool_MATRIX(y + 1, mat->cols);
    for(int x = 0; x < mat->cols; x++){
        for(int j = 0; j <= y; j++){
            M_bool_SET(ret, x, j, M_bool_GET(mat, x, j));
        }
    }
    return ret;
}

t_bool_matrix *trim_lines(t_bool_matrix *mat){
    if(!mat){
        return NULL;
    }
    t_bool_matrix *before = _trim_cols_before(mat);
    if(!before){
        return NULL;
    }
    t_bool_matrix *after = _trim_lines_after(before);

    M_bool_FREE(before);

    return after;
}


// y included
t_bool_matrix *before_h(t_bool_matrix *mat, int y) {
    t_bool_matrix *ret = CREATE_bool_MATRIX(y + 1, mat->cols);
    for (int x = 0; x < mat->cols; x++) {
        for (int j = 0; j <= y; j++) {
            M_bool_SET(ret, x, j, M_bool_GET(mat, x, j));
        }
    }
    return ret;
}

// y included
t_bool_matrix *after_h(t_bool_matrix *mat, int y) {
    t_bool_matrix *ret = CREATE_bool_MATRIX(mat->lines - y, mat->cols);
    for (int x = 0; x < mat->cols; x++) {
        for (int j = y; j < mat->lines; j++) {
            M_bool_SET(ret, x, j - y, M_bool_GET(mat, x, j));
        }
    }
    return ret;
}

// x included
t_bool_matrix *before_v(t_bool_matrix *mat, int x) {
    t_bool_matrix *ret = CREATE_bool_MATRIX(mat->lines, x + 1);
    for (int i = 0; i <= x; i++) {
        for (int y = 0; y < mat->lines; y++) {
            M_bool_SET(ret, i, y, M_bool_GET(mat, i, y));
        }
    }
    return ret;
}

// x included
t_bool_matrix *after_v(t_bool_matrix *mat, int x) {
    t_bool_matrix *ret = CREATE_bool_MATRIX(mat->lines, mat->cols - x);
    for (int i = x; i < mat->cols; i++) {
        for (int y = 0; y < mat->lines; y++) {
            M_bool_SET(ret, i - x, y, M_bool_GET(mat, i, y));
        }
    }
    return ret;
}

bool is_white_col(t_bool_matrix *mat, int x) {
    for (int i = 0; i < mat->lines; i++)
        if (M_bool_GET(mat, x, i))
            return false;
    return true;
}

bool is_white_line(t_bool_matrix *mat, int y) {
    for (int i = 0; i < mat->cols; i++)
        if (M_bool_GET(mat, i, y))
            return false;
    return true;
}

bool is_white(t_bool_matrix *mat, int x, int y, int h, int w) {
    int xend = x + w;
    int yend = y + h;
    for (int i = x; i < xend; i++) {
        for (int j = 0; j < yend; j++) {
            if (M_bool_GET(mat, i, j))
                return false;
        }
    }
    return true;
}

int find_v_cut(t_bool_matrix *mat, int span) {
    bool black = false;
    for (int x = 0; x < mat->cols; x++) {
        if (!is_white_col(mat, x)) {
            black = true;
            continue;
        }
        int count = 1;
        while (count < span && x + count < mat->cols) {
            if (is_white_col(mat, x + count))
                count++;
            else
                break;
        }
        if (!(count < span) && black)
            return x;
    }
    return -1;
}

int find_h_cut(t_bool_matrix *mat, int span) {
    bool black = false;
    for (int y = 0; y < mat->lines; y++) {
        if (!is_white_line(mat, y)) {
            black = true;
            continue;
        }
        int count = 1;
        while (count < span && y + count < mat->lines) {
            if (is_white_line(mat, y + count))
                count++;
            else
                break;
        }
        if (!(count < span) && black)
            return y;
    }
    return -1;
}
