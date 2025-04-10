
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
  struct awe_sqlite3_close *ptr = NULL;

  status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
  if (status != napi_ok) {
    goto on_error;
  }

  status = napi_get_value_external(env, argv[0], &userdata);
  if (status != napi_ok) {
    goto on_error;
  }

  /* Step 2. Debug message */
#ifdef _DEBUG
  fprintf(stderr, "debug: Close SQLite3 session %u\n", userdata->index);
#endif

  /* Step 3. Setup promise about close connection */
  ptr = (struct awe_sqlite3_close *)malloc(sizeof(struct awe_sqlite3_close));
  if (ptr == NULL) {
    goto on_error;
  }
  ptr->db = userdata->db;
  userdata->db = NULL;

  status = napi_create_promise(env, &ptr->deferred, &promise);
  if (status != napi_ok) {
    goto on_error;
  }

  status = napi_create_string_utf8(env, "awe-sqlite3-close", NAPI_AUTO_LENGTH, &resource_name);
  if (status != napi_ok) {
    goto on_error;
  }

  status = napi_create_async_work(env, NULL, resource_name, awe_sqlite3_close_work, awe_sqlite3_close_done, (void *)ptr, &ptr->worker);
  if (status != napi_ok) {
    goto on_error;
  }

  status = napi_queue_async_work(env, ptr->worker);
  if (status != napi_ok) {
    goto on_error;
  }

  return promise;
on_error:
  free(ptr);
  const char *message = "error";
  napi_throw_error(env, NULL, message);
  return NULL;
}
