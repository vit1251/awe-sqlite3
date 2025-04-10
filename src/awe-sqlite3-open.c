
#include <node_api.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>

#include "awe-sqlite3.h"


struct awe_sqlite3_open {
  napi_async_work worker;
  napi_deferred deferred;
  char *path;
  struct awe_sqlite3 *userdata;
  int err;
};

void awe_sqlite3_finalizer(napi_env env, void* finalize_data, void* finalize_hint) {
  struct awe_sqlite3 *userdata = (struct awe_sqlite3 *)finalize_data;

  /* Step 1. Debug message */
#ifdef _DEBUG
  fprintf(stderr, "debug: Finalize SQLite3 session%u\n", userdata->index);
#endif

  /* Step 2. Close SQLite3 database */
  if (userdata->db != NULL) {
    sqlite3_close(userdata->db);
  }

  /* Step 3. Cleanup */
  free(userdata);

}

static uint32_t connection_index = 0;

static void
awe_sqlite3_open_execute(napi_env env, void* data) {
  struct awe_sqlite3_open *arg = (struct awe_sqlite3_open *)data;
  struct awe_sqlite3 *userdata = NULL;

  /* Step 1. Allocate memory */
  userdata = malloc(sizeof(struct awe_sqlite3));
  if (userdata == NULL) {
    arg->err = -1;
    return;
  }

  /* Step 2. Initialize new SQLite3 connection */
  userdata->index = connection_index++;

  /* Step 3. Debug message */
#ifdef _DEBUG
  fprintf(stderr, "debug: New SQLite3 session %u with %s\n", userdata->index, arg->path);
#endif

  /* Step 4. Open SQLite3 database */
  sqlite3_open_v2(arg->path, &userdata->db, SQLITE_OPEN_FULLMUTEX | SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);

  /* Step 5. Save result */
  arg->userdata = userdata;

}

static void
awe_sqlite3_open_complete(napi_env env, napi_status status, void* data) {
  struct awe_sqlite3_open *arg = (struct awe_sqlite3_open *)data;
  napi_value ret;

  /* Step 1. Resolve promise with SQLite3 session */
  if ((arg->err == 0) && (arg->userdata != NULL)) {
    napi_create_external(env,
                         (void *)arg->userdata,
                         awe_sqlite3_finalizer,
                         NULL,
                         &ret);
    napi_resolve_deferred(env, arg->deferred, ret);
  }

  /* Step 2. Reject promise with error code */
  if (arg->err != 0) {
    napi_create_int32(env, arg->err, &ret);
    napi_reject_deferred(env, arg->deferred, ret);
  }

  napi_delete_async_work(env, arg->worker);

  free(arg->path);
  free(arg);
}

#define MAX_PATH 512

napi_value
awe_sqlite3_open(napi_env env, napi_callback_info info) {
  napi_value promise;
  napi_value resource_name;
  napi_status status;
  size_t argc = 2;
  napi_value argv[2];
  char *path = NULL;
  struct awe_sqlite3_open *ptr = NULL;

  status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
  if (status != napi_ok) {
    goto on_error;
  }

  path = (char *)malloc(MAX_PATH);
  status = napi_get_value_string_utf8(env,
                                       argv[0],
                                       path,
                                       MAX_PATH,
                                       NULL);

  ptr = (struct awe_sqlite3_open *)malloc(sizeof(struct awe_sqlite3_open));
  if (ptr == NULL) {
    goto on_error;
  }
  ptr->path = path;
  ptr->err = 0;

  status = napi_create_promise(env, &ptr->deferred, &promise);
  if (status != napi_ok) {
    goto on_error;
  }

  status = napi_create_string_utf8(env, "awe-sqlite3-open", NAPI_AUTO_LENGTH, &resource_name);
  if (status != napi_ok) {
    goto on_error;
  }

  status = napi_create_async_work(env, NULL, resource_name, awe_sqlite3_open_execute, awe_sqlite3_open_complete, (void *)ptr, &ptr->worker);
  if (status != napi_ok) {
    goto on_error;
  }

  status = napi_queue_async_work(env, ptr->worker);
  if (status != napi_ok) {
    goto on_error;
  }

  return promise;
on_error:

  free(path);
  free(ptr);
  const char *message = "error";
  napi_throw_error(env, NULL, message);
  return NULL;
}
