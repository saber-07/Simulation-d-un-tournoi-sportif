#include <gtk/gtk.h>

// Fonction de rappel pour le bouton
static void button_clicked(GtkWidget *widget, gpointer data)
{
    g_print("Le bouton a été cliqué\n");
}

int main(int argc, char *argv[])
{
    // Initialiser GTK
    gtk_init(&argc, &argv);

    // Créer la fenêtre principale
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Exemple d'interface");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    // Créer une zone de texte
    GtkWidget *label = gtk_label_new("Ceci est une étiquette de texte");

    // Créer un bouton
    GtkWidget *button = gtk_button_new_with_label("Cliquez ici");
    g_signal_connect(button, "clicked", G_CALLBACK(button_clicked), NULL);

    // Ajouter la zone de texte et le bouton à la fenêtre
    GtkWidget *vbox = gtk_vbox_new(TRUE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), button, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Afficher la fenêtre
    gtk_widget_show_all(window);

    // Lancer la boucle principale de GTK
    gtk_main();

    return 0;
}
