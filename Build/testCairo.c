#include <gtk/gtk.h>
#include <cairo/cairo.h>

/*static boolean on_draw_event(GtkWidget *widget,cairo_t *cr,gpointer user_data){
	cairo_set_source_surface(cr,//*/

int main(gint argc, gchar *argv[])
{
    GdkPixmap *pixmap;
    GtkWidget *image;
    GtkWidget *hbox;
    GtkWidget *button;
    GtkWidget *window;
    cairo_t *cr;
	cairo_surface_t *surface;
	
    gtk_init(&argc, &argv);


	hbox = gtk_hbox_new(FALSE, 10);
	button = gtk_button_new_with_label("OK");
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    //g_signal_connect(G_OBJECT(window->window, "draw", G_CALLBACK(on_draw_event), NULL);
    g_signal_connect(G_OBJECT(window), "delete-event", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
	
    pixmap = gdk_pixmap_new(window->window, 200, 20, 24);
    
    cr = gdk_cairo_create(pixmap);
    surface=cairo_image_surface_create_from_png("colormap.png");
    cairo_set_source_surface(cr,surface,0,0);
    cairo_paint(cr);
    /*cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);    
    cairo_rectangle(cr, 10, 10, 80, 80);
    cairo_fill(cr);//*/
    cairo_destroy(cr);

    cr = NULL;



    image = gtk_image_new_from_pixmap(pixmap, NULL);

    gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, TRUE, 10);
    gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, TRUE, 10);

    gtk_container_add(GTK_CONTAINER(window), hbox);
    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
