#include <gtk/gtk.h>

gboolean gg_status = TRUE;
gint gg_typed_words = 0;
gint gg_all_words = 50;
gchar gg_words[50][20];
GtkBuilder *builder;

gboolean fill (gpointer user_data) {
  GtkWidget *progress_bar = user_data;
  gdouble fraction;
  fraction = gtk_progress_bar_get_fraction (GTK_PROGRESS_BAR (progress_bar));
  fraction += 0.01666;
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progress_bar), fraction);
  if (fraction < 1.0) { 
    return TRUE;
  }
  return FALSE;
}

void set_words(gchar *l1, gchar *l2, gchar *l3, gchar *l4, gchar *l5) {
  gtk_label_set_text(GTK_LABEL(gtk_builder_get_object (builder, "l1")), l1);
  gtk_label_set_text(GTK_LABEL(gtk_builder_get_object (builder, "l2")), l2);
  gtk_label_set_text(GTK_LABEL(gtk_builder_get_object (builder, "l3")), l3);
  gtk_label_set_text(GTK_LABEL(gtk_builder_get_object (builder, "l4")), l4);
  gtk_label_set_text(GTK_LABEL(gtk_builder_get_object (builder, "l5")), l5);
  gtk_label_set_use_underline(GTK_LABEL(gtk_builder_get_object (builder, "l1")), TRUE);
}
void on_activate(GtkEntry *entry, gpointer user_data) {
  g_printerr (gtk_entry_get_text (entry));
  g_printerr("\n");
  gg_typed_words = gg_typed_words + 1;
  gtk_entry_set_text (entry, "");
}

void on_insert(GtkEntry *entry, gchar *new_text, gint new_text_length, gpointer position, gpointer user_data) {
  if(gg_status) {
    gg_status = FALSE;
    g_timeout_add (1000, fill, gtk_builder_get_object (builder, "progress"));
  }
}

int main (int argc, char *argv[]) {
  GObject *window;
  GError *error = NULL;
  gtk_init (&argc, &argv);
  builder = gtk_builder_new ();
  if (gtk_builder_add_from_file (builder, "/usr/share/TypingTest/window.ui", &error) == 0) {
    g_printerr ("Error loading file: %s\n", error->message);
    g_clear_error (&error);
    return 1;
  }

  FILE *fp;
  fp = fopen("/usr/share/TypingTest/words.txt", "r");
  for(int i=0; i<gg_all_words; i++) {
    fgets(gg_words[i], 20, (FILE*)fp);
    gg_words[i][strlen(gg_words[i])-1] = '\0';
  }
  fclose(fp);

  window = gtk_builder_get_object (builder, "window");
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
  g_signal_connect (gtk_builder_get_object (builder, "input"), "activate", G_CALLBACK (on_activate), NULL);
  g_signal_connect (gtk_builder_get_object (builder, "input"), "insert-text", G_CALLBACK (on_insert), NULL);

  set_words(gg_words[0], gg_words[1], gg_words[2], gg_words[3], gg_words[4]);

  gtk_main ();
  return 0;
}
