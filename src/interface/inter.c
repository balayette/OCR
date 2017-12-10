#include <stdlib.h>
#include <gtk/gtk.h>

void OnLoadNn(GtkButton *button, gpointer data);
void OnLoadImage(GtkButton *button, gpointer data);
void OnSave(GtkButton *button, gpointer data);

GtkWidget *image;
GtkWidget *window;

int main(int argc, char *argv[]) {

  GtkWidget *tab;
  GtkWidget *button[4];
  GtkWidget *data;

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

  image = gtk_image_new();
  //image = gtk_image_new_from_file("./lena.bmp");

  gtk_grid_attach(GTK_GRID(tab), image, 1, 4, 1, 1);
  gtk_grid_attach(GTK_GRID(tab), button[0], 0, 1, 1, 1);
  gtk_grid_attach_next_to(GTK_GRID(tab), button[1], button[0], GTK_POS_RIGHT, 1, 1);
  gtk_grid_attach_next_to(GTK_GRID(tab), button[2], button[1], GTK_POS_RIGHT, 1, 1);
  gtk_grid_attach_next_to(GTK_GRID(tab), button[3], button[2], GTK_POS_RIGHT, 1, 1);

  g_signal_connect(G_OBJECT(button[0]), "clicked", G_CALLBACK(OnLoadNn), window);
  g_signal_connect(G_OBJECT(button[1]), "clicked", G_CALLBACK(OnLoadImage), data);
  g_signal_connect(G_OBJECT(button[2]), "clicked", G_CALLBACK(OnSave), window);
  g_signal_connect(G_OBJECT(button[3]), "clicked", G_CALLBACK(OnLoadNn), window);

  gtk_widget_show_all(window);
  gtk_main();

  return EXIT_SUCCESS;
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

            //open_file(file); load nn
            //g_free(file);
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
            //gtk_widget_show_all(window);
            //open_file(file);
            g_free(file);
        }

    gtk_widget_destroy(fileSel);
}

void OnSave(GtkButton *button, gpointer window)
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

            gdk_pixbuf_save(gtk_image_get_pixbuf(GTK_IMAGE(image)), file, "bmp", GDK_PIXBUF_ERROR);
            //save_to_file (filename);
            //g_free (filename);
        }

    gtk_widget_destroy (fileSel);
}
