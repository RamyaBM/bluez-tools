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
#include <gio/gio.h>
#include <glib.h>
#include <string.h>

#include "../dbus-common.h"
#include "../properties.h"

#include "cycling_speed_manager.h"

#define CYCLING_SPEED_MANAGER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), CYCLING_SPEED_MANAGER_TYPE, CyclingSpeedManagerPrivate))

struct _CyclingSpeedManagerPrivate {
	GDBusProxy *proxy;
	gchar *object_path;
};

G_DEFINE_TYPE_WITH_PRIVATE(CyclingSpeedManager, cycling_speed_manager, G_TYPE_OBJECT);

enum {
	PROP_0,
	PROP_DBUS_OBJECT_PATH /* readwrite, construct only */
};

static void _cycling_speed_manager_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void _cycling_speed_manager_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

static void _cycling_speed_manager_create_gdbus_proxy(CyclingSpeedManager *self, const gchar *dbus_service_name, const gchar *dbus_object_path, GError **error);

static void cycling_speed_manager_dispose(GObject *gobject)
{
	CyclingSpeedManager *self = CYCLING_SPEED_MANAGER(gobject);

	/* Proxy free */
	g_clear_object (&self->priv->proxy);
	/* Object path free */
	g_free(self->priv->object_path);
	/* Chain up to the parent class */
	G_OBJECT_CLASS(cycling_speed_manager_parent_class)->dispose(gobject);
}

static void cycling_speed_manager_finalize (GObject *gobject)
{
	CyclingSpeedManager *self = CYCLING_SPEED_MANAGER(gobject);
	G_OBJECT_CLASS(cycling_speed_manager_parent_class)->finalize(gobject);
}

static void cycling_speed_manager_class_init(CyclingSpeedManagerClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->dispose = cycling_speed_manager_dispose;

	/* Properties registration */
	GParamSpec *pspec = NULL;

	gobject_class->get_property = _cycling_speed_manager_get_property;
	gobject_class->set_property = _cycling_speed_manager_set_property;
	
	/* object DBusObjectPath [readwrite, construct only] */
	pspec = g_param_spec_string("DBusObjectPath", "dbus_object_path", "CyclingSpeedManager D-Bus object path", NULL, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
	g_object_class_install_property(gobject_class, PROP_DBUS_OBJECT_PATH, pspec);
	if (pspec)
		g_param_spec_unref(pspec);
}

static void cycling_speed_manager_init(CyclingSpeedManager *self)
{
	self->priv = cycling_speed_manager_get_instance_private (self);
	self->priv->proxy = NULL;
	self->priv->object_path = NULL;
	g_assert(system_conn != NULL);
}

static void _cycling_speed_manager_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	CyclingSpeedManager *self = CYCLING_SPEED_MANAGER(object);

	switch (property_id) {
	case PROP_DBUS_OBJECT_PATH:
		g_value_set_string(value, cycling_speed_manager_get_dbus_object_path(self));
		break;

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
		break;
	}
}

static void _cycling_speed_manager_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	CyclingSpeedManager *self = CYCLING_SPEED_MANAGER(object);
	GError *error = NULL;

	switch (property_id) {
	case PROP_DBUS_OBJECT_PATH:
		self->priv->object_path = g_value_dup_string(value);
		_cycling_speed_manager_create_gdbus_proxy(self, CYCLING_SPEED_MANAGER_DBUS_SERVICE, self->priv->object_path, &error);
		break;

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
		break;
	}

	if (error != NULL)
		g_critical("%s", error->message);

	g_assert(error == NULL);
}

/* Constructor */
CyclingSpeedManager *cycling_speed_manager_new(const gchar *dbus_object_path)
{
	return g_object_new(CYCLING_SPEED_MANAGER_TYPE, "DBusObjectPath", dbus_object_path, NULL);
}

/* Private DBus proxy creation */
static void _cycling_speed_manager_create_gdbus_proxy(CyclingSpeedManager *self, const gchar *dbus_service_name, const gchar *dbus_object_path, GError **error)
{
	g_assert(CYCLING_SPEED_MANAGER_IS(self));
	self->priv->proxy = g_dbus_proxy_new_sync(system_conn, G_DBUS_PROXY_FLAGS_NONE, NULL, dbus_service_name, dbus_object_path, CYCLING_SPEED_MANAGER_DBUS_INTERFACE, NULL, error);

	if(self->priv->proxy == NULL)
		return;
}

/* Methods */

/* Get DBus object path */
const gchar *cycling_speed_manager_get_dbus_object_path(CyclingSpeedManager *self)
{
	g_assert(CYCLING_SPEED_MANAGER_IS(self));
	g_assert(self->priv->proxy != NULL);
	return g_dbus_proxy_get_object_path(self->priv->proxy);
}

