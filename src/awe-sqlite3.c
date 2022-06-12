
#include <node_api.h>

#include <stdlib.h>
#include <sqlite3.h>

#include "awe-sqlite3.h"

napi_value create_addon(napi_env env) {
  napi_value result;
  NAPI_CALL(env, napi_create_object(env, &result));

  /* Open */
  napi_value awe_sqlite3_open_export;
  NAPI_CALL(env, napi_create_function(env,
                                      "open",
                                      NAPI_AUTO_LENGTH,
                                      awe_sqlite3_open,
                                      NULL,
                                      &awe_sqlite3_open_export));

  NAPI_CALL(env, napi_set_named_property(env,
                                         result,
                                         "open",
                                         awe_sqlite3_open_export));

  /* Exec */
  napi_value awe_sqlite3_exec_export;
  NAPI_CALL(env, napi_create_function(env,
                                      "exec",
                                      NAPI_AUTO_LENGTH,
                                      awe_sqlite3_exec,
                                      NULL,
                                      &awe_sqlite3_exec_export));

  NAPI_CALL(env, napi_set_named_property(env,
                                         result,
                                         "exec",
                                         awe_sqlite3_exec_export));

  /* Close */
  napi_value awe_sqlite3_close_export;
  NAPI_CALL(env, napi_create_function(env,
                                      "close",
                                      NAPI_AUTO_LENGTH,
                                      awe_sqlite3_close,
                                      NULL,
                                      &awe_sqlite3_close_export));

  NAPI_CALL(env, napi_set_named_property(env,
                                         result,
                                         "close",
                                         awe_sqlite3_close_export));

  return result;
}

NAPI_MODULE_INIT() {
  return create_addon(env);
}
