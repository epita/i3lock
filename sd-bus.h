#ifndef I3_LOCK_SD_BUS_H
#define I3_LOCK_SD_BUS_H

int terminate_current_session(void);

int get_session_id(const char **session_id);

int set_lock_status(const char *session_id, unsigned lock_status);

#endif
