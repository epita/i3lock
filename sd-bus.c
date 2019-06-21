#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <systemd/sd-bus.h>

#include "sd-bus.h"

int terminate_current_session(void) {
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message *m = NULL;
    sd_bus *bus = NULL;
    int r;

    /* Connect to the system bus */
    r = sd_bus_open_system(&bus);
    if (r < 0) {
        fprintf(stderr, "Failed to connect to system bus: %s\n", strerror(-r));
        goto finish;
    }

    /* Issue the method call and store the respons message in m */
    r = sd_bus_call_method(bus,
            "org.freedesktop.login1",               /* service to contact */
            "/org/freedesktop/login1/session/self", /* object path */
            "org.freedesktop.login1.Session",       /* interface name */
            "Terminate",                            /* method name */
            &error,                                 /* object to return error in */
            &m,                                     /* return message on success */
            ""                                      /* input signature */
            );
    if (r < 0) {
        fprintf(stderr, "Failed to issue method call: %s\n", error.message);
    }

finish:
    sd_bus_error_free(&error);
    sd_bus_message_unref(m);
    sd_bus_unref(bus);

    return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

int get_session_id(char *session_id)
{
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message *m = NULL;
    sd_bus *bus = NULL;
    int r;
    const char *id;

    /* Connect to the system bus */
    r = sd_bus_open_system(&bus);
    if (r < 0) {
        fprintf(stderr, "Failed to connect to system bus: %s\n", strerror(-r));
        goto finish;
    }

    /* Issue the method call and store the respons message in m */
    r = sd_bus_call_method(bus,
            "org.freedesktop.login1",                       /* service to contact */
            "/org/freedesktop/login1/session/self",         /* object path */
            "org.freedesktop.DBus.Properties",              /* interface name */
            "Get",                                          /* method name */
            &error,                                         /* object to return error in */
            &m,                                             /* return message on success */
            "ss",                                           /* input signature */
            "org.freedesktop.login1.Session",               /* first argument */
            "Id");                                          /* second argument */
    if (r < 0) {
        fprintf(stderr, "Failed to issue method call: %s\n", error.message);
        goto finish;
    }

    // We need to open the variant container
    char type;
    r = sd_bus_message_peek_type(m, &type, &id);
    if (r < 0) {
        fprintf(stderr, "Failed to peek the type container: %s\n", strerror(-r));
        goto finish;
    }
    r = sd_bus_message_enter_container(m, SD_BUS_TYPE_VARIANT, id);
    if (r < 0) {
        fprintf(stderr, "Failed to enter in the container: %s\n", strerror(-r));
        goto finish;
    }
    r = sd_bus_message_read_basic(m, SD_BUS_TYPE_STRING, &id);
    if (r < 0) {
        fprintf(stderr, "Failed to parse response message: %s\n", strerror(-r));
        goto finish;
    }
    if (id == NULL) {
        fprintf(stderr, "Failed to get current session id: %s\n", strerror(-r));
        goto finish;
    }
    size_t len_id = strlen(id);
    if (len_id == 0) {
        fprintf(stderr, "Failed to get current session id: %s\n", strerror(-r));
        goto finish;
    }
    strcpy(session_id, id);

finish:
    sd_bus_error_free(&error);
    sd_bus_message_unref(m);
    sd_bus_unref(bus);

    return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

static int set_lock_status_dbus(char *session_id, const char *method_name) {
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message *m = NULL;
    sd_bus *bus = NULL;
    int r;

    /* Connect to the system bus */
    r = sd_bus_open_system(&bus);
    if (r < 0) {
        fprintf(stderr, "Failed to connect to system bus: %s\n", strerror(-r));
        goto finish;
    }

    /* Issue the method call and store the respons message in m */
    r = sd_bus_call_method(bus,
            "org.cri.MachineState",                 /* service to contact */
            "/org/cri/MachineState",                /* object path */
            "org.cri.MachineState",                 /* interface name */
            method_name,                          /* method name */
            &error,                                 /* object to return error in */
            &m,                                     /* return message on success */
            "s",                                    /* input signature */
            session_id                              /* first argument */
            );
    if (r < 0) {
        fprintf(stderr, "Failed to issue method call: %s\n", error.message);
    }

finish:
    sd_bus_error_free(&error);
    sd_bus_message_unref(m);
    sd_bus_unref(bus);

    return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

int set_lock_status(char *session_id, enum status new_status) {
    const char *status[STATUS_SIZE] =
    {
        "UnlockSession",
        "LockSession",
        "OvertimeLockSession"
    };
    if (new_status < UNLOCKED || new_status > LOCKED_OVERTIME)
    {
        printf("Wrong status.\n");
        return 0;
    }
    return set_lock_status_dbus(session_id, status[new_status]);
}
