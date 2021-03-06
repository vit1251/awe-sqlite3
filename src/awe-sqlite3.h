
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

#define NAPI_CALL(env, call)                                      \
  do {                                                            \
    napi_status status = (call);                                  \
    if (status != napi_ok) {                                      \
      const napi_extended_error_info* error_info = NULL;          \
      napi_get_last_error_info((env), &error_info);               \
      bool is_pending;                                            \
      napi_is_exception_pending((env), &is_pending);              \
      if (!is_pending) {                                          \
        const char* message = (error_info->error_message == NULL) \
            ? "empty error message"                               \
            : error_info->error_message;                          \
        napi_throw_error((env), NULL, message);                   \
        return NULL;                                              \
      }                                                           \
    }                                                             \
  } while(0)

struct awe_sqlite3 {
  uint32_t index;
  sqlite3 *db;
};

napi_value awe_sqlite3_open(napi_env env, napi_callback_info info);
napi_value awe_sqlite3_exec(napi_env env, napi_callback_info info);
napi_value awe_sqlite3_close(napi_env env, napi_callback_info info);
