
#include <node_api.h>

#include <stdlib.h>
#include <sqlite3.h>

#include "awe-sqlite3.h"

struct sqlite3_open {
  napi_async_work worker;
  napi_deferred deferred;
  sqlite3 *db;
};

void
sqlite3_open_work(napi_env env, void* data) {
  struct sqlite3_open *arg = (struct sqlite3_open *)data;

  /* Step 1. Open SQLite3 database */
  sqlite3_open("1.sqlite3", &arg->db);

}

void
sqlite3_open_done(napi_env env, napi_status status, void* data) {
  struct sqlite3_open *arg = (struct sqlite3_open *)data;
  napi_value ret;

  if (arg->db != NULL) {
    napi_create_int32(env, 1, &ret);
    napi_resolve_deferred(env, arg->deferred, ret);
  } else {
    napi_create_int32(env, 0, &ret);
    napi_reject_deferred(env, arg->deferred, ret);
  }
  napi_delete_async_work(env, arg->worker);
  arg->deferred = NULL;

  /* Release memory */
  free(arg);
}

static napi_value
DoSomethingUseful(napi_env env, napi_callback_info info) {
  napi_value promise;
  napi_value resource_name;
  struct sqlite3_open *ptr = malloc(sizeof(struct sqlite3_open));
  NAPI_CALL(env, napi_create_promise(env, &ptr->deferred, &promise));
  NAPI_CALL(env, napi_create_string_utf8(env, "awe-sqlite3-open", NAPI_AUTO_LENGTH, &resource_name));
  NAPI_CALL(env, napi_create_async_work(env, NULL, resource_name, sqlite3_open_work, sqlite3_open_done, (void *)ptr, &ptr->worker));
  NAPI_CALL(env, napi_queue_async_work(env, ptr->worker));
  return promise;
}

napi_value create_addon(napi_env env) {
  napi_value result;
  NAPI_CALL(env, napi_create_object(env, &result));

  napi_value exported_function;
  NAPI_CALL(env, napi_create_function(env,
                                      "open",
                                      NAPI_AUTO_LENGTH,
                                      DoSomethingUseful,
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
