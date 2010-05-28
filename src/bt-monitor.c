/*
 *
 *  bluez-tools - a set of tools to manage bluetooth devices for linux
 *
 *  Copyright (C) 2010  Alexander Orlenko <zxteam@gmail.com>
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <glib.h>
#include <dbus/dbus-glib.h>

#include "dbus.h"
#include "monitor.h"

static gchar *adapter = NULL; // TODO: g_free() ?

static GOptionEntry entries[] = {
	{ "adapter", 'a', 0, G_OPTION_ARG_STRING, &adapter, "Adapter name or MAC", NULL },
	{ NULL }
};

int main(int argc, char *argv[])
{
	GError *error; // TODO: g_error_free()
	GOptionContext *context; // TODO: g_option_context_free()
	DBusGConnection *conn;
	
	g_type_init();
	
	context = g_option_context_new("- a bluetooth monitor");
	g_option_context_add_main_entries(context, entries, NULL);
	if (!g_option_context_parse(context, &argc, &argv, &error)) {
		g_print("%s: %s\n", g_get_prgname(), error->message);
		g_print("Try `%s --help` for more information.\n", g_get_prgname());
		// ME: should I call this functions?
		g_error_free(error);
		g_option_context_free(context);
		exit(EXIT_FAILURE);
	}
	g_option_context_free(context);
	
	conn = dbus_g_bus_get(DBUS_BUS_SYSTEM, &error);
	if (!conn) {
		g_printerr("Couldn't connect to system bus: %s", error->message);
		g_error_free(error);
		exit(EXIT_FAILURE);
	}
	
	if (!adapter) {
		// Listen for all events
	} else {
		// Listen for an adapter events
	}
	
	exit(EXIT_SUCCESS);
}

// --------------------------------
// Some functions -- temporary here
// --------------------------------

#define BLUEZ_DBUS_NAME "org.bluez"
#define BLUEZ_DBUS_MANAGER_PATH "/"
#define BLUEZ_DBUS_MANAGER_INTERFACE "org.bluez.Manager"
#define BLUEZ_DBUS_ADAPTER_INTERFACE "org.bluez.Adapter"

gboolean find_adapter(const gchar *adapter, GError **error, DBusGProxy **adapter_obj)
{
	if (adapter == NULL) {
		return default_adapter(error, adapter_obj);
	}
	
	DBusGProxy *manager_obj;
	manager_obj = dbus_g_proxy_new_for_name(conn, BLUEZ_DBUS_NAME, BLUEZ_DBUS_MANAGER_PATH, BLUEZ_DBUS_MANAGER_INTERFACE);
	
}

gboolean default_adapter(GError **error, DBusGProxy **adapter_obj)
{
	DBusGProxy *manager_obj;
	manager_obj = dbus_g_proxy_new_for_name(conn, BLUEZ_DBUS_NAME, BLUEZ_DBUS_MANAGER_PATH, BLUEZ_DBUS_MANAGER_INTERFACE);
	
	if (!dbus_g_proxy_call(manager_obj, "DefaultAdapter", &error, G_TYPE_INVALID, DBUS_TYPE_G_PROXY, adapter_obj, G_TYPE_INVALID)) {
		return false;
	} else {
		dbus_g_proxy_set_interface(adapter_obj, "org.bluez.Adapter");
	}
	
	return true;
}
