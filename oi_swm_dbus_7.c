/* 
 * 
 * OpenIndiana Software Manager
 * 
 * Gtk based OI SWM that interacts with OI's pkg manager via dbus messages
 * 
 * (c) 2024 Iggi
 * 
 */


#include <gtk/gtk.h>
#include <glib.h>
#include <gio/gio.h>
#include <json-c/json.h>

GDBusConnection *conn;
GDBusProxy *proxy;

GtkTreeStore		*treeStore;
GtkTreeViewColumn	*pCol;
GtkTreeViewColumn	*dCol;
GtkCellRenderer		*cR1;
GtkCellRenderer		*cR2;

GdkPixbuf *pixbuf, *scaled_pixbuf;

GtkTreeIter iter;		// iterators
GtkTreeIter iterChild1;	// iterators
GtkTreeIter iterChild2;
GtkTreeIter iterChild3;	// iterators
GtkTreeIter iterChild4;	// iterators
GtkTreeIter iterChild5;	// iterators
	
int	level;
char *tempString, *valueString;

void print_json_value(struct json_object *jobj, int depth);

void print_json_tree(struct json_object *jobj, int depth) 
{

	
    if (jobj == NULL) 
    {
        return;
    }

    enum json_type type;
    json_object_object_foreach(jobj, key, val) 
    {
        type = json_object_get_type(val);
        //printf("%*s- %s\n", depth * 2, "", key);
		level = depth;
		tempString = key;
		setTreeEntry();
        print_json_value(val, depth + 1);
    }
}

void setTreeEntry ()
{
	// printf ("Level: %d\n", level);
	// printf ("Key: %s\n", tempString);
	
	if (level == 0 ) {
			gtk_tree_store_append (treeStore, &iter, NULL);
			gtk_tree_store_set(treeStore, &iter, 0, tempString, -1);
			gtk_tree_store_set(treeStore, &iter, 1, valueString, -1);
			}

		if (level == 1 ) {
			gtk_tree_store_append (treeStore, &iterChild1, &iter);
			gtk_tree_store_set(treeStore, &iterChild1, 0, tempString, -1);
			gtk_tree_store_set(treeStore, &iterChild1, 1, valueString, -1);
			}
	
		if (level == 2 ) {
			gtk_tree_store_append (treeStore, &iterChild2, &iterChild1);
			gtk_tree_store_set(treeStore, &iterChild2, 0, tempString, -1);
			gtk_tree_store_set(treeStore, &iterChild2, 1, valueString, -1);
			}
	
		if (level == 3 ) {
			gtk_tree_store_append (treeStore, &iterChild3, &iterChild2);
			gtk_tree_store_set(treeStore, &iterChild3, 0, tempString, -1);
			gtk_tree_store_set(treeStore, &iterChild3, 1, valueString, -1);
			}
	
		if (level == 4 ) {
			gtk_tree_store_append (treeStore, &iterChild4, &iterChild3);
			gtk_tree_store_set(treeStore, &iterChild4, 0, tempString, -1);
			gtk_tree_store_set(treeStore, &iterChild4, 1, valueString, -1);
			}
	
		if (level == 5 ) {
			gtk_tree_store_append (treeStore, &iterChild5, &iterChild4);
			gtk_tree_store_set(treeStore, &iterChild5, 0, tempString, -1);
			gtk_tree_store_set(treeStore, &iterChild5, 1, valueString, -1);
			}
}


void print_json_value(struct json_object *jobj, int depth) 
{
    if (jobj == NULL) 
    {
        return;
    }
    
    enum json_type type = json_object_get_type(jobj);
    switch (type) 
    {
        case json_type_array: 
        {
            size_t len = json_object_array_length(jobj);
            for (size_t i = 0; i < len; i++) 
            {
                struct json_object *arr_obj = json_object_array_get_idx(jobj, i);
                if (json_object_is_type(arr_obj, json_type_object)) 
                {
                    print_json_tree(arr_obj, depth);
                } else 
                {
                    // printf("Value:\n");
                    // printf("%*s %s\n", depth * 2, "", json_object_to_json_string(arr_obj));
                    valueString = json_object_to_json_string(arr_obj);
                }
            }
            break;
        }
        case json_type_object:
            print_json_tree(jobj, depth);
            break;
        case json_type_boolean:
        case json_type_double:
        case json_type_int:
        case json_type_string:
            // printf("Value:\n");
            // printf("%*s %s\n", depth * 2, "", json_object_to_json_string(jobj));
            valueString = json_object_to_json_string(jobj);
            break;
        default:
			valueString = "";
            break;
    }
}
// Callback row selected in tree view
void on_row_selected(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data) 
{
    GtkTreeModel *model = gtk_tree_view_get_model(tree_view);
    GtkTreeIter iter;

    if (gtk_tree_model_get_iter(model, &iter, path)) 
    {
        gchar *text;
        gtk_tree_model_get(model, &iter, 0, &text, -1);

        // TODO: Send message to Python API via D-Bus and update applications tree view

        g_free(text);
    }
}

// Callback when a button is clicked
void on_button_clicked(GtkButton *button, gpointer user_data) 
{
    const gchar *action = gtk_button_get_label(button);
    gchar *method;

    if (g_strcmp0(action, "Install") == 0) 
    {
        method = "pkg_install";
    } else if (g_strcmp0(action, "Uninstall") == 0) 
    {
        method = "pkg_uninstall";
    } else if (g_strcmp0(action, "Update") == 0) 
    {
        method = "pkg_update";
    } else 
    {
        g_printerr("Unknown action: %s\n", action);
        return;
    }

    GError *error = NULL;
    GVariant *result = g_dbus_proxy_call_sync(proxy,
                                              method,
                                              NULL,
                                              G_DBUS_CALL_FLAGS_NONE,
                                              -1,
                                              NULL,
                                              &error);

    if (result == NULL) 
    {
        g_printerr("Error calling method on D-Bus proxy: %s\n", error->message);
        g_error_free(error);
        return;
    }

    g_variant_unref(result);
}

static void on_activate(GtkApplication* app) 
{
    GtkBuilder *builder = gtk_builder_new_from_file("interface.glade");

    GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    gtk_window_set_application(GTK_WINDOW(window), app);
    gtk_window_set_title(GTK_WINDOW(window), "OpenIndiana Software Manager");

    GtkWidget *install_button = GTK_WIDGET(gtk_builder_get_object(builder, "button1"));
    GtkWidget *uninstall_button = GTK_WIDGET(gtk_builder_get_object(builder, "button2"));
    GtkWidget *update_button = GTK_WIDGET(gtk_builder_get_object(builder, "button3"));
    GtkImage *logo = GTK_WIDGET(gtk_builder_get_object(builder, "image"));

	treeStore	= GTK_TREE_STORE(gtk_builder_get_object(builder, "treeStore"));
	pCol		= GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "pCol"));
	dCol		= GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "dCol"));
	cR1			= GTK_CELL_RENDERER(gtk_builder_get_object(builder, "cR1"));
	cR2			= GTK_CELL_RENDERER(gtk_builder_get_object(builder, "cR2"));
	
	gtk_tree_view_column_add_attribute(pCol, cR1, "text", 0); // attach the renderer to the column
	gtk_tree_view_column_add_attribute(dCol, cR2, "text", 1); // attach the renderer to the column
	
    g_signal_connect(install_button, "clicked", G_CALLBACK(on_button_clicked), NULL);
    g_signal_connect(uninstall_button, "clicked", G_CALLBACK(on_button_clicked), NULL);
    g_signal_connect(update_button, "clicked", G_CALLBACK(on_button_clicked), NULL);
    
    // Load the logo and resize it to 48x48 pixels
    pixbuf = gdk_pixbuf_new_from_file("oi_logo_small.png", NULL);
    scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, 48, 48, GDK_INTERP_BILINEAR);
    logo = gtk_image_new_from_pixbuf(scaled_pixbuf);
    gtk_image_set_from_pixbuf(logo, scaled_pixbuf);
    g_object_unref(pixbuf); // Clean up the original pixbuf

	FILE *file = fopen("catalog.base.json", "r");
    if (file == NULL) 
    {
        perror("Error opening file");
        return 1;
    }

    struct json_object *parsed_json;
    parsed_json = json_object_from_file("catalog.base.json");
    //Large OpenIndiana catalog
    //parsed_json = json_object_from_file("/var/pkg/publisher/openindiana.org/catalog/catalog.base.C");
    if (parsed_json == NULL) 
    {
        printf("Error parsing JSON file\n");
        return 1;
    }

    // Initialize DBus connection
    GError *error = NULL;
    conn = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, &error);
    if (conn == NULL) 
    {
        g_printerr("Error connecting to D-Bus: %s\n", error->message);
        g_error_free(error);
        return;
    }

    // Create a new proxy for the interface
    proxy = g_dbus_proxy_new_sync(conn,
                                  G_DBUS_PROXY_FLAGS_NONE,
                                  NULL,
                                  "swm.openindiana.org",  // The name of the interface
                                  "/org/openindiana/SoftwareManager", // The object path
                                  "org.openindiana.swm",  // The interface name
                                  NULL,
                                  &error);

    if (proxy == NULL) 
    {
        g_printerr("Error creating D-Bus proxy: %s\n", error->message);
        g_error_free(error);
        return;
    }

	print_json_tree(parsed_json, 0);

    json_object_put(parsed_json);
    
    fclose(file);
    
    gtk_widget_show_all(window);
}

void on_destroy() 
{
	gtk_main_quit();
}


int main(int argc, char **argv) 
{
    GtkApplication *app = gtk_application_new("swm.openindiana.org", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}

