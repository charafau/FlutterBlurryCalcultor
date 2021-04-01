
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define GDK_VERSION_MIN_REQUIRED GDK_VERSION_3_4
#define GDK_VERSION_MAX_ALLOWED GDK_VERSION_3_4
#include <cairo.h>

#include <gtk/gtk.h>
#include <flutter_linux/flutter_linux.h>
#include "flutter/generated_plugin_registrant.h"

gboolean draw(GtkWidget *widget, cairo_t *cr, gpointer data)
{
  double x, y, w, h;
  cairo_clip_extents(cr, &x, &y, &w, &h);
  cairo_set_source_rgba(cr, 1., 1., 1., 0.25); //translucent red
  cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
  cairo_rectangle(cr, x, y, w, h);
  cairo_fill(cr);

  return FALSE;
}

void fix_visual(GtkWidget *w)
{
  GdkScreen *screen = gtk_widget_get_screen(w);
  GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
  gtk_widget_set_visual(w, visual);
}

void screen_changed(GtkWidget *widget, GdkScreen *screen, gpointer user_data)
{
  fix_visual(widget);
}

int main(int argc, char *argv[])
{

  GtkWidget *w;
  GtkWidget *q;

  gtk_init(&argc, &argv);

  // w = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  w = gtk_window_new(GTK_WINDOW_POPUP);
  g_signal_connect(w, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  gtk_widget_set_size_request(w, 400, 600);
  // q = gtk_layout_new(NULL, NULL);
  q = gtk_overlay_new();

  g_signal_connect(w, "screen-changed", G_CALLBACK(screen_changed), NULL);
  g_signal_connect(q, "draw", G_CALLBACK(draw), NULL);

  gtk_container_add(GTK_CONTAINER(w), q);

  gtk_widget_set_app_paintable(w, TRUE);

  fix_visual(w);

  // create flutter view
  g_autoptr(FlDartProject) project = fl_dart_project_new();
  fl_dart_project_set_dart_entrypoint_arguments(project, argv);

  FlView *view = fl_view_new(project);

  // gtk_container_add(GTK_CONTAINER(q), GTK_WIDGET(view));
  gtk_container_add(GTK_CONTAINER(q), GTK_WIDGET(view));

  gtk_widget_show(GTK_WIDGET(view));

  fl_register_plugins(FL_PLUGIN_REGISTRY(view));

  gtk_widget_grab_focus(GTK_WIDGET(view));


  // finish flutter view

  gtk_widget_show_all(w);

  gtk_main();

  return 0;
}