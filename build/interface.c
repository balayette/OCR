#include <stdlib.h>
#include <gtk/gtk.h>
#include "neuralnet/neuralnet.h"
#include "neuralnet/save.h"
#define _GNU_SOURCE
#include "misc/bool_matrix.h"
#include "misc/utils.h"
#include <dirent.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "imgprocessing/display.h"
#include "imgprocessing/drawing.h"
#include "imgprocessing/processing.h"
#include "imgprocessing/recxy.h"

#include "neuralnet/neuralnet.h"
#include "neuralnet/save.h"

#include "matrix/matrixop.h"
#include "misc/bool_matrix.h"

#define H 35
#define V 35
#define SIZE H*V


static const char TOKENS[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVW"
    "XYZ0123456789";
static const int TOKENS_LEN = 62;
static const int SYMBOL_OFFSET = 0;
static const int LETTER_OFFSET = 0;
static const int CAPS_OFFSET = 26 + 0;
static const int NUMBER_OFFSET = 26 + 26 + 0;
int neuron_hidden = 150;

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


struct dataset {
    int *sizes;
    t_bool_matrix ***data;
};

struct cache {
    int *sizes;
    double ***data;
    double **output;
};

struct cache *cache_from_dataset(struct dataset *ds) {
    struct cache *c = malloc(sizeof(struct cache));

    c->output = calloc(TOKENS_LEN, sizeof(double *));

    c->sizes = malloc(TOKENS_LEN * sizeof(int));
    memcpy(c->sizes, ds->sizes, sizeof(int) * TOKENS_LEN);

    c->data = calloc(TOKENS_LEN, sizeof(double **));
    for (int i = 0; i < TOKENS_LEN; i++) {
        c->data[i] = calloc(ds->sizes[i], sizeof(double *));
        for (int y = 0; y < ds->sizes[i]; y++) {
            c->data[i][y] = calloc(SIZE, sizeof(double));
            /* pprint_bool_matrix(ds->data[i][y]); */
            mat_to_double(ds->data[i][y], c->data[i][y]);
        }
        c->output[i] = calloc(TOKENS_LEN, sizeof(double));
        get_output(c->output[i], TOKENS[i]);
    }
    return c;
}

void free_cache(struct cache *c) {

    for (int i = 0; i < TOKENS_LEN; i++) {
        for (int y = 0; y < c->sizes[i]; y++) {
            free(c->data[i][y]);
        }
        free(c->data[i]);
        free(c->output[i]);
    }
    free(c->sizes);
    free(c->output);
    free(c->data);
    free(c);
}

struct dataset *load_dataset(char *path) {
    int plen = strlen(path);
    if (path[plen - 1] != '/')
        path[plen++] = '/';

    printf("Loading dataset from %s\n", path);
    struct dataset *ds = malloc(sizeof(struct dataset));
    ds->sizes = calloc(TOKENS_LEN, sizeof(int));
    ds->data = calloc(TOKENS_LEN, sizeof(t_bool_matrix **));
    // FIXME : This has to change if we use more than a 1000 fonts
    for (int i = 0; i < TOKENS_LEN; i++) {
        ds->data[i] = calloc(1000, sizeof(t_bool_matrix *));
    }

    DIR *d;
    struct dirent *dir;
    d = opendir(path);
    if (!d) {
        printf("Couldn't open %s\n", path);
        exit(1);
    }
    char *indiv_path = calloc(1000, sizeof(char));
    strcpy(indiv_path, path);
    while ((dir = readdir(d)) != NULL) {
        if (dir->d_type == DT_REG) {
            strcpy(indiv_path + plen, dir->d_name);

            printf("Found matrix %s\n", indiv_path);
            int index = get_letter_index(indiv_path[strlen(indiv_path) - 1]);
            /* printf("    Letter : %c | Index : %d | Current count : %d\n", */
            /*        indiv_path[strlen(indiv_path) - 1], index,
             * ds->sizes[index]); */
            t_bool_matrix *m = load_bool_matrix(indiv_path);
            if (!m) {
                printf("Loading failed\n");
                continue;
            }
            ds->data[index][ds->sizes[index]++] = m;
        }
    }
    closedir(d);
    free(indiv_path);
    printf("\nLoaded the dataset. Final state : \n");
    printf("    |  Letter  |  Count  |\n");
    int sum = 0;
    for (int i = 0; i < TOKENS_LEN; i++) {
        printf("    |    %c     |    %d   |\n", TOKENS[i], ds->sizes[i]);
        sum += ds->sizes[i];
    }
    printf("    |  Total   |   %d  |\n", sum);
    return ds;
}

void free_dataset(struct dataset *ds) {

    for (int i = 0; i < TOKENS_LEN; i++) {
        for (int y = 0; y < ds->sizes[i]; y++) {
            M_bool_FREE(ds->data[i][y]);
        }
        free(ds->data[i]);
    }
    free(ds->data);
    free(ds->sizes);
    free(ds);
}

void print_diff(double *arr1, double *arr2, int size) {
    for (int i = 0; i < size; i++) {
        printf("%f - %f\n", arr1[i], arr2[i]);
    }
}

void generate_cache(t_bool_matrix **dataset, double **cache) {
    for (int i = 0; i < TOKENS_LEN; i++) {
        if (dataset[i]) {
            mat_to_double(dataset[i], cache[i]);
        }
    }
}

void OnLoadNn(GtkButton *button, gpointer data);
void OnLoadImage(GtkButton *button, gpointer data);
void OnSaveImage(GtkButton *button, gpointer data);
void OnTrainNn(GtkButton *button, gpointer data);

GtkWidget *image = NULL;
GtkWidget *window;

int main(int argc, char *argv[]) {

  GtkWidget *tab;
  GtkWidget *button[6];

  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_title(GTK_WINDOW(window), "OCR");
  gtk_window_set_default_size(GTK_WINDOW(window), 720, 620);
  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

  tab = gtk_grid_new();
  gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(tab));

  button[0] = gtk_button_new_with_label("Load a Neural Network");
  button[1] = gtk_button_new_with_label("Load Image");
  button[2] = gtk_button_new_with_label("Save Image");
  button[3] = gtk_button_new_with_label("Train the Neural Network");
  button[4] = gtk_button_new_with_label("Do the OCR on the selected image");
  button[5] = gtk_button_new_with_label("Change the number of neuron");

  image = gtk_image_new();
  gtk_grid_attach(GTK_GRID(tab), image, 2, 4, 1, 1);
  gtk_grid_attach(GTK_GRID(tab), button[0], 0, 1, 1, 1);
  gtk_grid_attach_next_to(GTK_GRID(tab), button[1], button[0], GTK_POS_RIGHT, 1, 1);
  gtk_grid_attach_next_to(GTK_GRID(tab), button[2], button[1], GTK_POS_RIGHT, 1, 1);
  gtk_grid_attach_next_to(GTK_GRID(tab), button[3], button[2], GTK_POS_RIGHT, 1, 1);
  gtk_grid_attach_next_to(GTK_GRID(tab), button[4], button[3], GTK_POS_RIGHT, 1, 1);
  gtk_grid_attach_next_to(GTK_GRID(tab), button[5], button[4], GTK_POS_RIGHT, 1, 1);

  g_signal_connect(G_OBJECT(button[0]), "clicked", G_CALLBACK(OnLoadNn), window);
  g_signal_connect(G_OBJECT(button[1]), "clicked", G_CALLBACK(OnLoadImage), window);
  g_signal_connect(G_OBJECT(button[2]), "clicked", G_CALLBACK(OnSaveImage), window);
  g_signal_connect(G_OBJECT(button[3]), "clicked", G_CALLBACK(OnTrainNn), window);
  g_signal_connect(G_OBJECT(button[4]), "clicked", G_CALLBACK(OnOCRRun), window);
  g_signal_connect(G_OBJECT(button[5]), "clicked", G_CALLBACK(OnAddNeurons), window);

  gtk_widget_show_all(window);
  gtk_main();

  return EXIT_SUCCESS;
}

void OnAddNeurons(GtkButton *button, gpointer data)
{

    GtkWidget* pBoite;
    GtkWidget* pEntry;
    const gchar* sNom;

    /* Création de la boite de dialogue */
    /* 1 bouton Valider */
    /* 1 bouton Annuler */
    pBoite = gtk_dialog_new_with_buttons("Saisie du nom",
        GTK_WINDOW(pWindow),
        GTK_DIALOG_MODAL,
        GTK_STOCK_OK,GTK_RESPONSE_OK,
        GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,
        NULL);

    /* Création de la zone de saisie */
    pEntry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(pEntry), "Saisissez votre nom");
    /* Insertion de la zone de saisie dans la boite de dialogue */
    /* Rappel : paramètre 1 de gtk_box_pack_start de type GtkBox */
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pBoite)->vbox), pEntry, TRUE, FALSE, 0);

    /* Affichage des éléments de la boite de dialogue */
    gtk_widget_show_all(GTK_DIALOG(pBoite)->vbox);

    /* On lance la boite de dialogue et on récupéré la réponse */
    switch (gtk_dialog_run(GTK_DIALOG(pBoite)))
    {
        /* L utilisateur valide */
        case GTK_RESPONSE_OK:
            sNom = gtk_entry_get_text(GTK_ENTRY(pEntry));
            neuron_hidden = atoi(sNom);
            break;
        /* L utilisateur annule */
        case GTK_RESPONSE_CANCEL:
        case GTK_RESPONSE_NONE:
        default:
            gtk_label_set_text(GTK_LABEL(pLabel), "You did not enter anything valid !");
            break;
    }

    /* Destruction de la boite de dialogue */
    gtk_widget_destroy(pBoite);
}



void OnLoadNn(GtkButton *button, gpointer data)
{
    GtkWidget *fileSel;
    gint res;

    fileSel = gtk_file_chooser_dialog_new("Open File", GTK_WINDOW(window), GTK_FILE_CHOOSER_ACTION_OPEN, "_Cancel", GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL);

    res = gtk_dialog_run(GTK_DIALOG(fileSel));
    if (res == GTK_RESPONSE_ACCEPT)
        {
            char *file;
            GtkFileChooser *chooser = GTK_FILE_CHOOSER(fileSel);
            file = gtk_file_chooser_get_filename(chooser);

            nn = load_nn(file);
            g_free(file);
        }

    gtk_widget_destroy(fileSel);
}
void OnLoadImage(GtkButton *button, gpointer data)
{
    GtkWidget *fileSel;
    gint res;

    fileSel = gtk_file_chooser_dialog_new("Open File", GTK_WINDOW(window), GTK_FILE_CHOOSER_ACTION_OPEN, "_Cancel", GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL);

    res = gtk_dialog_run(GTK_DIALOG(fileSel));
    if (res == GTK_RESPONSE_ACCEPT)
        {
            char *file;
            GtkFileChooser *chooser = GTK_FILE_CHOOSER(fileSel);
            file = gtk_file_chooser_get_filename(chooser);

            gtk_image_set_from_file(GTK_IMAGE(image), file);
            g_free(file);
        }

    gtk_widget_destroy(fileSel);
}

void OnSaveImage(GtkButton *button, gpointer window)
{
    GtkWidget *fileSel;
    GtkFileChooser *chooser;
    gint res;

    fileSel = gtk_file_chooser_dialog_new("Save File", window, GTK_FILE_CHOOSER_ACTION_SAVE, "_Cancel", GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL);

    chooser = GTK_FILE_CHOOSER(fileSel);

    gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);

    res = gtk_dialog_run (GTK_DIALOG (fileSel));
    if (res == GTK_RESPONSE_ACCEPT)
        {
            char *file;
            file = gtk_file_chooser_get_filename (chooser);

            GdkPixbuf *pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(image));
            gdk_pixbuf_save(pixbuf, file, "bmp", NULL, NULL);
            g_free (file);
        }

    gtk_widget_destroy (fileSel);
}

void OnTrainNn(GtkButton *button, gpointer window)
{
        srand(time(NULL));
    if (argc < 3) {
        printf("Usage : ./neuralnet DATA_DIR REPS\n");
        exit(1);
    }

    struct neural_net *nn;
    printf("%d\n", argc);
    if (argc == 3) {
        nn = create_nn(SIZE, 1, neuron_hidden, TOKENS_LEN);
    } else {
        printf("Loading\n");
        nn = load_nn(argv[3]);
    }
    char *path = calloc(1000, sizeof(char));
    strcat(path, argv[1]);
    struct dataset *ds = load_dataset(path);
    struct cache *c = cache_from_dataset(ds);

    int reps = atoi(argv[2]);
    /* struct neural_net *nn = load_nn("save.nn"); */

    int count = 0;
    for (int BIGREP = 1; BIGREP <= 1000; BIGREP++) {
        printf("%d / %d\n", BIGREP * (reps / 1000), reps);
        for (int rep = 1; rep <= reps / 1000; rep++) {
            int k = rand() % TOKENS_LEN;
            int y = rand() % c->sizes[k];
            /* for (int y = 0; y < c->sizes[k]; y++) { */
            forward_prop(nn, c->data[k][y]);
            /* printf("Output : "); */
            /* print_double_arr(nn->layers[nn->hidden_layer_count + 1]->values,
             // * TOKENS_LEN); */
            back_prop(nn, c->output[k], c->data[k][y]);
            count++;
            /* } */
        }
    }
    printf("Propagations : %d\n", count);
    save_nn(nn, "save.nn");
    free_nn(nn);

    nn = load_nn("save.nn");
    for (int i = 0; i < TOKENS_LEN; i++) {
        if (!c->sizes[i])
            continue;
        printf("%c ->\n    ", TOKENS[i]);
        for (int k = 0; k < c->sizes[i]; k++) {
            forward_prop(nn, c->data[i][k]);
            printf("%c, ",
                   get_letter(nn->layers[nn->hidden_layer_count + 1]->values,
                              TOKENS_LEN));
        }
        printf("\n");
    }

    free_dataset(ds);
    free_cache(c);
    free_nn(nn);
    free(path);
}

void OnOCRRun(GtkButton *button, gpointer window)
{
    if (image == NULL)
        {
            return;
        }

    SDL_Surface *img = load_image(image->file);

    gray_level(img);

    int t = otsu(img);
    binarize(img, t);

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

    FILE *f = fopen("output.txt", "w");

    DISPLAY(bintree, space, f);
    fclose(f);

    free_nn(nn);
    M_bool_FREE(mat);
    free_bintree(bintree);
    gtk_image_set_from_file(GTK_IMAGE(image), file); //to display the modified image by the OCR?
    SDL_FreeSurface(img);
    SDL_Quit();


}
