#include "gtk/gtk.h"
#include "stdio.h"

static GtkWidget *entry;

static gboolean kill_window(GtkWidget *widget,GdkEvent *event, gpointer data){
	gtk_main_quit();
	return FALSE;
}

static void button_press(GtkWidget *widget, gpointer data){
	const char *text=gtk_entry_get_text(GTK_ENTRY(entry));
	printf("%s\n",text);
}


int main(void){

		GtkWidget *window;
		GtkWidget *button;
		GtkWidget *hbox;
		GtkWidget *vbox;
		GtkWidget *image;
		
	gtk_init(0,NULL);
	
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	button=gtk_button_new_with_label("print");	
	entry=gtk_entry_new();
	hbox=gtk_hbox_new(FALSE,2);
	vbox=gtk_vbox_new(FALSE,2);
	image=gtk_image_new_from_file("crash.jpg");
	
	gtk_window_set_title(GTK_WINDOW(window),"gtk application");
	
	g_signal_connect(window,"delete_event",G_CALLBACK(kill_window),NULL);
	g_signal_connect(button,"clicked",G_CALLBACK(button_press),NULL);
	
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,2);
	gtk_box_pack_start(GTK_BOX(hbox),entry,TRUE,TRUE,2);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,2);
	gtk_box_pack_start(GTK_BOX(vbox),image,FALSE,FALSE,2);
	gtk_container_add(GTK_CONTAINER(window),vbox);
	
	gtk_widget_show_all(window);
	
	gtk_main();
	return 0;
}	
