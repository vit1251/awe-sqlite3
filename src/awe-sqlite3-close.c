
#include <node_api.h>

#include <stdlib.h>
#include <sqlite3.h>

#include "awe-sqlite3.h"

struct awe_sqlite3_close {
  napi_async_work worker;
  napi_deferred deferred;
  sqlite3 *db;
};

static void
awe_sqlite3_close_work(napi_env env, void* data) {
  struct awe_sqlite3_close *arg = (struct awe_sqlite3_close *)data;
  int ok;

  /* Step 1. Close SQLite3 database */
  ok = sqlite3_close(arg->db);
  if (ok != SQLITE_OK) {
  }

}

static void
awe_sqlite3_close_done(napi_env env, napi_status status, void* data) {
  struct awe_sqlite3_close *arg = (struct awe_sqlite3_close *)data;
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
awe_sqlite3_close(napi_env env, napi_callback_info info) {
  napi_value promise;
  napi_value resource_name;
  size_t argc = 2;
  napi_value argv[2];
  napi_status status;
  struct awe_sqlite3 *userdata = NULL;

  status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
  if (status != napi_ok) {
//      return NULL;
  }

  status = napi_get_value_external(env, argv[0], &userdata);
  if (status != napi_ok) {
//      return NULL;
  }

  /* Step 2. Debug message */
  fprintf(stdout, "debug: Close SQLite3 connection #%d\n", userdata->index);

  /* Step 3. Setup promise about close connection */
  struct awe_sqlite3_close *ptr = (struct awe_sqlite3_close *)malloc(sizeof(struct awe_sqlite3_close));
  ptr->db = userdata->db;
  userdata->db = NULL;

  NAPI_CALL(env, napi_create_promise(env, &ptr->deferred, &promise));
  NAPI_CALL(env, napi_create_string_utf8(env, "awe-sqlite3-close", NAPI_AUTO_LENGTH, &resource_name));
  NAPI_CALL(env, napi_create_async_work(env, NULL, resource_name, awe_sqlite3_close_work, awe_sqlite3_close_done, (void *)ptr, &ptr->worker));
  NAPI_CALL(env, napi_queue_async_work(env, ptr->worker));
  return promise;
}
