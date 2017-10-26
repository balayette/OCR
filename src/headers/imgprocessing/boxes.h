#ifndef BOXES_H
#define BOXES_H

#include "box.h"

typedef struct s_boxes {
    t_box ***boxes;
    int h;
    int w;
} t_boxes;

t_box *getbox(t_boxes *boxes, int i, int j);

void setbox(t_boxes *boxes, int i, int j, t_box *box);

t_boxes *new_boxes(int v, int h);

t_box *get_next_bwbn(t_boxes *boxes, int i, int j, int towards, int direction);

#endif
