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

int set_lock_status(unsigned lock_status) {
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
            "Set_Lock_Status",                      /* method name */
            &error,                                 /* object to return error in */
            &m,                                     /* return message on success */
            "u",                                    /* input signature */
            lock_status
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
