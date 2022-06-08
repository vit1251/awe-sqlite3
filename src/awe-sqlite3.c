
#include <node_api.h>

#include <stdlib.h>
#include <sqlite3.h>

#include "awe-sqlite3.h"

napi_value create_addon(napi_env env) {
  napi_value result;
  NAPI_CALL(env, napi_create_object(env, &result));

  napi_value exported_function;
  NAPI_CALL(env, napi_create_function(env,
                                      "open",
                                      NAPI_AUTO_LENGTH,
                                      awe_sqlite3_open,
                                      NULL,
                                      &exported_function));

  NAPI_CALL(env, napi_set_named_property(env,
                                         result,
                                         "open",
                                         exported_function));

  return result;
}

NAPI_MODULE_INIT() {
  return create_addon(env);
}
