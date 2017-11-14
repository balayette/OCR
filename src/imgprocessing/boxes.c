#include <stdlib.h>
#include "imgprocessing/box.h"
#include "imgprocessing/boxes.h"


t_box *getbox(t_boxes *boxes, int i, int j) { return boxes->boxes[i][j]; }

void setbox(t_boxes *boxes, int i, int j, t_box *box) {
    boxes->boxes[i][j] = box;
}

// v : vertical boxes
// h : horizontal boxes
t_boxes *new_boxes(int v, int h) {
    t_boxes *boxes = malloc(sizeof(t_boxes));
    boxes->h = v;
    boxes->w = h;
    boxes->boxes = calloc(v, sizeof(t_box *));
    for (int i = 0; i < v; i++) {
        boxes->boxes[i] = calloc(h, sizeof(t_box *));
    }
    return boxes;
}
// Get_Next_BoxWithBlackNeigh
// direction = -1 => bottom neigh
// direction = 1 => top neigh
// towards = 1 => right
// towards = -1 => left
t_box *get_next_bwbn(t_boxes *boxes, int i, int j, int towards, int direction){
    t_box *current = getbox(boxes, i, j);
    for(int k = current->x; j + k < boxes->w && j + k > 0; k += towards){
        current = getbox(boxes, i + direction, j + k);
        if(current)
            return current;
    }
    return NULL;
}
