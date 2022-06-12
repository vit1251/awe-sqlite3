
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
};

void awe_sqlite3_finalizer(napi_env env, void* finalize_data, void* finalize_hint) {
  struct awe_sqlite3 *userdata = (struct awe_sqlite3 *)finalize_data;

  /* Step 0. Debug message */
  fprintf(stdout, "debug: Release SQLite3 connection #%d\n", userdata->index);

  /* Step 1. Close SQLite3 database */
  if (userdata->db != NULL) {
    fprintf(stdout, "debug: Lick SQLite3 connection #%d. Auto close.\n", userdata->index);
    sqlite3_close(userdata->db);
  }

  /* Step 2. Release memory */
  free(userdata);

}

static uint32_t connection_index = 0;

static void
awe_sqlite3_open_work(napi_env env, void* data) {
  struct awe_sqlite3_open *arg = (struct awe_sqlite3_open *)data;
  struct awe_sqlite3 *userdata = NULL;

  /* Step 1. Allocate memory */
  userdata = malloc(sizeof(struct awe_sqlite3));
  if (userdata == NULL) {
    abort();
  }

  /* Stpe 2. Initialize new SQLite3 connection */
  userdata->index = connection_index++;

  /**/
  fprintf(stdout, "debug: Open connection #%d with store %s\n", userdata->index, arg->path);

  /* Step 3. Open SQLite3 database */
  sqlite3_open_v2(arg->path, &userdata->db, SQLITE_OPEN_FULLMUTEX | SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);

  /* Step 4. Save result */
  arg->userdata = userdata;

}

static void
awe_sqlite3_open_done(napi_env env, napi_status status, void* data) {
  struct awe_sqlite3_open *arg = (struct awe_sqlite3_open *)data;
  napi_value ret;

  if (arg->userdata != NULL) {
    napi_create_external(env,
                         (void *)arg->userdata,
                         awe_sqlite3_finalizer,
                         NULL,
                         &ret);
    napi_resolve_deferred(env, arg->deferred, ret);
  } else {
    napi_create_int32(env, 0, &ret);
    napi_reject_deferred(env, arg->deferred, ret);
  }
  napi_delete_async_work(env, arg->worker);
  arg->deferred = NULL;

  /* Release memory */
  free(arg->path);
  free(arg);
}

napi_value
awe_sqlite3_open(napi_env env, napi_callback_info info) {
  napi_value promise;
  napi_value resource_name;
  napi_status status;
  size_t argc = 2;
  napi_value argv[2];
  char *path;

  status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
  if (status != napi_ok) {
//      return NULL;
  }

  path = (char *)malloc(512);
  status = napi_get_value_string_utf8(env,
                                       argv[0],
                                       path,
                                       512,
                                       NULL);
//  if (status )

  struct awe_sqlite3_open *ptr = (struct awe_sqlite3_open *)malloc(sizeof(struct awe_sqlite3_open));
  ptr->path = path;
  NAPI_CALL(env, napi_create_promise(env, &ptr->deferred, &promise));
  NAPI_CALL(env, napi_create_string_utf8(env, "awe-sqlite3-open", NAPI_AUTO_LENGTH, &resource_name));
  NAPI_CALL(env, napi_create_async_work(env, NULL, resource_name, awe_sqlite3_open_work, awe_sqlite3_open_done, (void *)ptr, &ptr->worker));
  NAPI_CALL(env, napi_queue_async_work(env, ptr->worker));
  return promise;
}
