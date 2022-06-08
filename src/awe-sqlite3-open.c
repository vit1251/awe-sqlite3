
#include <node_api.h>

#include <stdlib.h>
#include <sqlite3.h>

#include "awe-sqlite3.h"

struct awe_sqlite3_open {
  napi_async_work worker;
  napi_deferred deferred;
  sqlite3 *db;
};

static void
awe_sqlite3_open_work(napi_env env, void* data) {
  struct awe_sqlite3_open *arg = (struct awe_sqlite3_open *)data;

  /* Step 1. Open SQLite3 database */
  sqlite3_open("1.sqlite3", &arg->db);

}

static void
awe_sqlite3_open_done(napi_env env, napi_status status, void* data) {
  struct awe_sqlite3_open *arg = (struct awe_sqlite3_open *)data;
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

napi_value
awe_sqlite3_open(napi_env env, napi_callback_info info) {
  napi_value promise;
  napi_value resource_name;
  struct awe_sqlite3_open *ptr = (struct awe_sqlite3_open *)malloc(sizeof(struct awe_sqlite3_open));
  NAPI_CALL(env, napi_create_promise(env, &ptr->deferred, &promise));
  NAPI_CALL(env, napi_create_string_utf8(env, "awe-sqlite3-open", NAPI_AUTO_LENGTH, &resource_name));
  NAPI_CALL(env, napi_create_async_work(env, NULL, resource_name, awe_sqlite3_open_work, awe_sqlite3_open_done, (void *)ptr, &ptr->worker));
  NAPI_CALL(env, napi_queue_async_work(env, ptr->worker));
  return promise;
}