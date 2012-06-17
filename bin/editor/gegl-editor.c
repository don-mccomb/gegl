#include <glib.h>
#include <gtk/gtk.h>
#include <cairo.h>
#include <gegl.h>

#include "gegl-node-widget.h"
#include "gegl-editor-layer.h"

GtkWidget	*window;

/*void add_operation_dialog (GtkDialog* dialog, gint response_id, gpointer user_data)
{
  if(response_id == GTK_RESPONSE_ACCEPT) {
    g_print("add\n");
  }
  }*/

void menuitem_activated(GtkMenuItem* item, gpointer data)
{
  GeglEditorLayer	*layer = (GeglEditorLayer*)data;
  if(0 == strcmp("Add Operation", gtk_menu_item_get_label(item)))
    {
      g_print("Add an operation\n");
    }
  GtkWidget *add_op_dialog = gtk_dialog_new_with_buttons("AddOperation", GTK_WINDOW(window), 
							 GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
							 GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL,GTK_RESPONSE_REJECT, NULL);
  GtkWidget *text_entry = gtk_entry_new();
  
  gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(add_op_dialog))), text_entry);
  gtk_widget_show(text_entry);

  //g_signal_connect(add_op_dialog, "response", add_operation_dialog, data);

  gint result = gtk_dialog_run(GTK_DIALOG(add_op_dialog));
  GeglNode *node;
  switch(result)
    {
    case GTK_RESPONSE_ACCEPT:
      node = gegl_node_create_child (layer->gegl, gtk_entry_get_text(GTK_ENTRY(text_entry)));
      layer_add_gegl_node(layer, node);
      break;
    default:
      break;
    }
  gtk_widget_destroy(add_op_dialog);
}

gint
main (gint	  argc,
      gchar	**argv)
{
  gtk_init(&argc, &argv);

  GtkWidget	*editor	     = gegl_editor_new ();
  GeglEditor*	 node_editor = GEGL_EDITOR(editor);

  gegl_init(&argc, &argv);
  
  //add some samples nodes
  GeglNode		*gegl  = gegl_node_new();
  GeglEditorLayer*	 layer = layer_create(node_editor, gegl);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
  g_signal_connect (window, "destroy", G_CALLBACK( gtk_main_quit), NULL);

  GtkWidget	*vbox = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox);
  gtk_container_add(GTK_CONTAINER(window), vbox);

////////////////////////////////////////////ADD OPERATION DIALOG//////////////////////////////////////////////

  

/////////////////////////////////////////////////BUILD MENUBAR////////////////////////////////////////////////
 
 GtkWidget	*menubar;
  
  GtkWidget	*process_menu;
  GtkWidget	*process;
  GtkWidget	*process_all;

  GtkWidget	*graph_menu;
  GtkWidget	*graph;
  GtkWidget	*add_operation;

  menubar      = gtk_menu_bar_new();
  process_menu = gtk_menu_new();
  graph_menu   = gtk_menu_new();

  process     = gtk_menu_item_new_with_label("Process");
  process_all = gtk_menu_item_new_with_label("All");

  gtk_menu_item_set_submenu(GTK_MENU_ITEM(process), process_menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(process_menu), process_all);
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), process);

  graph		= gtk_menu_item_new_with_label("Graph");
  add_operation = gtk_menu_item_new_with_label("Add Operation");
  g_signal_connect(add_operation, "activate", (GCallback)menuitem_activated, layer);

  gtk_menu_item_set_submenu(GTK_MENU_ITEM(graph), graph_menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(graph_menu), add_operation);
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), graph);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

  gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), editor, TRUE, TRUE, 0);
  gtk_widget_show_all(window);

////////////////////////////////////////////GEGL OPERATIONS///////////////////////////////////////////////////

  GeglNode	*display = gegl_node_create_child (gegl, "gegl:display");
  GeglNode	*over = gegl_node_new_child (gegl, "operation", "gegl:over", NULL);
  GeglNode	*load = gegl_node_new_child(gegl, "operation", "gegl:load", "path", "./surfer.png", NULL);
  GeglNode	*text = gegl_node_new_child(gegl, "operation", "gegl:text", "size", 10.0, "color", 
					    gegl_color_new("rgb(1.0,1.0,1.0)"), "text", "Hello world!", NULL);

  layer_add_gegl_node(layer, display);
  layer_add_gegl_node(layer, over);
  layer_add_gegl_node(layer, load);
  layer_add_gegl_node(layer, text);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

  g_print("%s\n", G_OBJECT_CLASS_NAME(G_OBJECT_GET_CLASS(load)));
  
  gtk_main();
  
  return 0;
}
