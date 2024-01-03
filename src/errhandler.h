#ifndef ERRHANDLER_H
#define ERRHANDLER_H

void handle_parse_cmdopts_error(int rc, const char *errmsg[2]);

void handle_parse_cmdargs_error(int rc, const char *errmsg[3]);

void handle_validate_appopt_error(int rc, const char *errmsg[3]);

void handle_init_database_error(int rc, const char *errmsg[2]);

void handle_apply_key_error(int rc, const char *errmsg[2]);

void handle_sqlite_error(int rc);

void handle_missing_operation_error(void);

#endif /* ERRHANDLER_H */