/* 
 * Compile main: gcc -I/usr/include/dbus-1.0 `pkg-config --cflags -libs dbus-1` dbus_test.c -o dbus_test
 *
 * Adapted from https://leonardoce.wordpress.com/2015/03/17/dbus-tutorial-part-3/
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <dbus/dbus.h>

static void check_and_abort(DBusError *error);
static void abort_on_error(DBusError *error);

static char *get_property(DBusConnection *connection, const char *bus_name, const char *path, const char *iface, const char *propname, DBusError *error, int type);
static DBusMessage *create_property_get_message(const char *bus_name, const char *path, const char *iface, const char *propname);
static const char *extract_from_variant(DBusMessage *reply, DBusError *error, int type);

static void set_string_property(DBusConnection *connection, const char *bus_name, const char *path, const char *iface, const char *propname, const char *value);

int main() {
    DBusConnection *connection = NULL;
    DBusError error;
    DBusMessage *msgQuery = NULL;
    DBusMessage *msgReply = NULL;
	DBusMessage *sigQuery = NULL;
	DBusMessage *sigReply = NULL;
	DBusPendingCall *sigPending = NULL;
    const char *path = malloc(50);
	char *location = "/";
	char *latitude = malloc(10);
	char *longitude = malloc(10);
 
    dbus_error_init(&error);
    connection = dbus_bus_get(DBUS_BUS_SYSTEM, &error);
    check_and_abort(&error);
 
	/* Gets client path for connection */
    msgQuery = dbus_message_new_method_call(
        "org.freedesktop.GeoClue2",
        "/org/freedesktop/GeoClue2/Manager",
        "org.freedesktop.GeoClue2.Manager",
        "GetClient");
 
    msgReply = dbus_connection_send_with_reply_and_block(connection, msgQuery, 1000, &error);
    check_and_abort(&error);
    dbus_message_unref(msgQuery);
 
    dbus_message_get_args(msgReply, &error, DBUS_TYPE_OBJECT_PATH, &path, DBUS_TYPE_INVALID);
	dbus_message_unref(msgReply);

	// printf("Client path: %s\n", path);

	/* Sets DesktopId (still required for connection) */
	set_string_property(connection, "org.freedesktop.GeoClue2",
		path,
		"org.freedesktop.GeoClue2.Client",
		"DesktopId",
		"1");

	/* Starts client from path */
	msgQuery = dbus_message_new_method_call(
		"org.freedesktop.GeoClue2",
		path,
		"org.freedesktop.GeoClue2.Client",
		"Start");

	dbus_connection_send_with_reply_and_block(connection, msgQuery, 1000, &error);
	check_and_abort(&error);
	dbus_message_unref(msgQuery);

	while ( !strcmp(location, "/") ) {
		/* Gets location path */
		location = get_property(connection, "org.freedesktop.GeoClue2",
			 path,
			 "org.freedesktop.GeoClue2.Client",
			 "Location",
			 &error, DBUS_TYPE_OBJECT_PATH);
		abort_on_error(&error);

		sleep(1);
	}

	const char *loc = strdup(location);
	// printf("Location: %s\n", location);
 
	latitude = get_property(connection, "org.freedesktop.GeoClue2",
		 loc,
		 "org.freedesktop.GeoClue2.Location",
		 "Latitude",
		 &error, DBUS_TYPE_DOUBLE);
	abort_on_error(&error);

	longitude = get_property(connection, "org.freedesktop.GeoClue2",
		 loc,
		 "org.freedesktop.GeoClue2.Location",
		 "Longitude",
		 &error, DBUS_TYPE_DOUBLE);
	abort_on_error(&error);

	printf("%s - %s", latitude, longitude);

    return 0;
}
 
static void check_and_abort(DBusError *error) {
    if (!dbus_error_is_set(error)) return;
	printf("Error: ");
    puts(error->message);
    abort();
}

static void abort_on_error(DBusError *error) {
    if (dbus_error_is_set(error)) {
        fprintf(stderr, "%s", error->message);
        abort();
    }
}

static char *get_property(DBusConnection *connection, const char *bus_name, const char *path, const char *iface, const char *propname, DBusError *error, int type) {
    DBusError err;
    char *result;
    DBusMessage *queryMessage = NULL;
    DBusMessage *replyMessage = NULL;
 
    dbus_error_init(&err);
     
    queryMessage = create_property_get_message(bus_name, path, iface, propname);
    replyMessage = dbus_connection_send_with_reply_and_block(connection,
                          queryMessage,
                          1000,
                          &err);

    dbus_message_unref(queryMessage);
    if (dbus_error_is_set(&err)) {
        dbus_move_error(&err, error);
        return 0;
    }
 
    const char *r = extract_from_variant(replyMessage, &err, type);

	dbus_message_unref(replyMessage);
    if (dbus_error_is_set(&err)) {
        dbus_move_error(&err, error);
        return 0;
    }
 
	result = strdup(r);
	puts(result);
    return result;
}

static DBusMessage *create_property_get_message(const char *bus_name, const char *path, const char *iface, const char *propname) {
    DBusMessage *queryMessage = NULL;
    queryMessage = dbus_message_new_method_call(bus_name, path, 
                            "org.freedesktop.DBus.Properties",
                            "Get");
    dbus_message_append_args(queryMessage,
                 DBUS_TYPE_STRING, &iface,
                 DBUS_TYPE_STRING, &propname,
                 DBUS_TYPE_INVALID);
 
    return queryMessage;
}

static const char *extract_from_variant(DBusMessage *reply, DBusError *error, int type) {
    DBusMessageIter iter;
    DBusMessageIter sub;
    char *result;
     
    dbus_message_iter_init(reply, &iter);
 
    if (DBUS_TYPE_VARIANT != dbus_message_iter_get_arg_type(&iter)) {
        dbus_set_error_const(error, "reply_should_be_variant", "This message hasn't a variant response type");
        return 0;
    }
 
    dbus_message_iter_recurse(&iter, &sub);
 
    if (type != dbus_message_iter_get_arg_type(&sub)) {
        dbus_set_error_const(error, "Wrong type", "This variant reply message has incorrect type");
        return 0;
    }
 
	switch (type) {
		case DBUS_TYPE_OBJECT_PATH: {
			dbus_message_iter_get_basic(&sub, &result);
			break;
		}
		
		case DBUS_TYPE_DOUBLE: {
			double result_d;
			dbus_message_iter_get_basic(&sub, &result_d);
			snprintf(result, 10, "%f", result_d);
			break;
		}
	}

    return result;
}

static void set_string_property(DBusConnection *connection, const char *bus_name, const char *path, const char *iface, const char *propname, const char *value) {
    DBusMessage *queryMessage = NULL;
	DBusMessage *replyMessage = NULL;
	DBusMessageIter iter, subIter;
	DBusError error;
 
    dbus_error_init(&error);

    queryMessage = dbus_message_new_method_call(bus_name, path, 
                            "org.freedesktop.DBus.Properties",
                            "Set");

	/*
	 * Set operation's value parameter must be VARIANT, so we must append it using iter.
	 * Container creation is explained in https://stackoverflow.com/a/23017594/10717087 
	 */ 
	dbus_message_iter_init_append(queryMessage, &iter);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &iface);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &propname);
	dbus_message_iter_open_container(&iter, DBUS_TYPE_VARIANT, DBUS_TYPE_STRING_AS_STRING, &subIter);
	dbus_message_iter_append_basic(&subIter, DBUS_TYPE_STRING, &value);
	dbus_message_iter_close_container(&iter, &subIter);

	replyMessage = dbus_connection_send_with_reply_and_block(connection,
				  queryMessage,
				  1000,
				  &error);
	check_and_abort(&error);
	dbus_message_unref(queryMessage);
	dbus_message_unref(replyMessage);

    return;
}
