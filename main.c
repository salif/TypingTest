#include <gtk/gtk.h>

gint gg_status = 0;
gint gg_all_k = 0;
gint gg_correct_k = 0;
gint gg_incorrect_k = 0;
gint gg_all_words = 800;
gint gg_current = 0;
gchar gg_words[800][20];
GtkBuilder *builder;
GtkApplication *app;

void end () {
  int wpm = (gg_all_k / 5) - (gg_incorrect_k / 5);
  char result[125];
  double accuracy = (gg_correct_k * 100) / gg_all_k;
  snprintf(result, 125, "%d wpm | %.2f%% acc. | %d (+%d/-%d) keystr.", wpm, accuracy, gg_all_k, gg_correct_k, gg_incorrect_k);
  g_printf("\n%s\n\n", result);
  gtk_label_set_text(GTK_LABEL(gtk_builder_get_object (builder, "end")), result);
  
  FILE *fr;
  fr = fopen("/usr/share/TypingTest/log.txt", "a");
  fputs(result, fr);
  fputs("\n", fr);
  fclose(fr);
  
  gg_status = 2;
}

gboolean fill (gpointer user_data) {
  GtkWidget *progress_bar = user_data;
  gdouble fraction;
  fraction = gtk_progress_bar_get_fraction (GTK_PROGRESS_BAR (progress_bar));
  fraction += 0.01666;
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progress_bar), fraction);
  if (fraction < 1.0) { 
    return TRUE;
  }
  end();
  return FALSE;
}

void set_words (gchar *l1, gchar *l2, gchar *l3, gchar *l4, gchar *l5) {
  gtk_label_set_text(GTK_LABEL(gtk_builder_get_object (builder, "word1")), l1);
  gtk_label_set_text(GTK_LABEL(gtk_builder_get_object (builder, "word2")), l2);
  gtk_label_set_text(GTK_LABEL(gtk_builder_get_object (builder, "word3")), l3);
  gtk_label_set_text(GTK_LABEL(gtk_builder_get_object (builder, "word4")), l4);
  gtk_label_set_text(GTK_LABEL(gtk_builder_get_object (builder, "word5")), l5);
}

void trim(char * s) {
  char * p = s;
  int l = strlen(p);
  while(isspace(p[l - 1])) p[--l] = 0;
  while(* p && isspace(* p)) ++p, --l;
  memmove(s, p, l + 1);
}

void on_insert (GtkEntry *entry, gchar *new_text, gint new_text_length, gpointer position, gpointer user_data) {
  if(gg_status == 0) {
    gg_status = 1;
    g_timeout_add (1000, fill, gtk_builder_get_object (builder, "progress"));
  }
  if(gg_status == 1){
    if (strcmp(new_text, " ") == 0) {
      char *input_word[20];
      strcat(input_word, gtk_entry_get_text(entry));
      trim(input_word);
      if(strcmp(input_word, gg_words[gg_current]) == 0) {
        gg_correct_k = gg_correct_k + strlen(input_word);
      }
      else {
        gg_incorrect_k = gg_incorrect_k + strlen(input_word);
        g_print("incorrect: '%s' -> '%s'\n", input_word, gg_words[gg_current]);
      }
      gg_all_k = gg_all_k + strlen(input_word);
      gg_current = gg_current + 1;
      set_words(gg_words[gg_current], gg_words[gg_current + 1], gg_words[gg_current + 2], gg_words[gg_current + 3], gg_words[gg_current + 4]);
      gtk_entry_set_text (entry, "");
    }
  }
}

void on_activate (GtkEntry *entry, gpointer user_data) {
  if(gg_status == 2) {
    gg_status = 0;
    gg_all_k = 0;
    gg_correct_k = 0;
    gg_incorrect_k = 0;
    gg_current = 0;
    gdouble fraction = 0.00;
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (gtk_builder_get_object (builder, "progress")), fraction);
    set_words(gg_words[0], gg_words[1], gg_words[2], gg_words[3], gg_words[4]);
    gtk_entry_set_text (entry, "");
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object (builder, "end")), "");
  }
}

int load_words() {
  FILE *fp;
  fp = fopen("/usr/share/TypingTest/words.txt", "r");
  for(int i=0; i<gg_all_words; i++) {
    fgets(gg_words[i], 20, (FILE*)fp);
    gg_words[i][strlen(gg_words[i])-1] = '\0';
  }
  fclose (fp);
}

void activate (GtkApplication* app, gpointer user_data) {
  builder = gtk_builder_new_from_file ("/usr/share/TypingTest/window.ui");
  GObject *window = gtk_builder_get_object (builder, "window");
  gtk_application_add_window (app, GTK_WINDOW(window));
  
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
  g_signal_connect (gtk_builder_get_object (builder, "input"), "insert-text", G_CALLBACK (on_insert), NULL);
  g_signal_connect (gtk_builder_get_object (builder, "input"), "activate", G_CALLBACK (on_activate), NULL);

  set_words (gg_words[0], gg_words[1], gg_words[2], gg_words[3], gg_words[4]);
  gtk_main ();
}

int main (int argc, char *argv[]) {
  gtk_init (&argc, &argv);
  
  load_words();
  
  app = gtk_application_new ("com.salifm.typingtest", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  int status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
