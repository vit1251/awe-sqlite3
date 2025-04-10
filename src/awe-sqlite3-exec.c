
#include <node_api.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>

#include "awe-sqlite3.h"

struct awe_sqlite3_exec {
  napi_async_work worker;
  napi_deferred deferred;
  sqlite3 *db;
  char *query;
  int code;
};

static void
awe_sqlite3_exec_execute(napi_env env, void* data) {
  struct awe_sqlite3_exec *arg = (struct awe_sqlite3_exec *)data;
  int rc;
  char *err_msg = NULL;

  /* Step 1. Perform SQL query */
  rc = sqlite3_exec(arg->db, arg->query, 0, 0, &err_msg);
  if (rc != SQLITE_OK) {
  }

#ifdef _DEBUG
  fprintf(stderr, "error: Execute complete: rc = %u msg = %s\n", rc, err_msg);
#endif

  /* Step 2. Save error code */
  arg->code = rc;

}

static void
awe_sqlite3_exec_complete(napi_env env, napi_status status, void* data) {
  struct awe_sqlite3_exec *arg = (struct awe_sqlite3_exec *)data;
  napi_value ret;

  napi_create_int32(env, arg->code, &ret);

  if (arg->db != NULL) {
    napi_resolve_deferred(env, arg->deferred, ret);
  } else {
    napi_reject_deferred(env, arg->deferred, ret);
  }
  napi_delete_async_work(env, arg->worker);
  arg->deferred = NULL;

  /* Release memory */
  free(arg->query);
  free(arg);
}

#define MAX_QUERY_SIZE 32768

napi_value
awe_sqlite3_exec(napi_env env, napi_callback_info info) {
  napi_value promise;
  napi_value resource_name;
  size_t argc = 2;
  napi_value argv[2];
  napi_status status;
  struct awe_sqlite3 *userdata = NULL;
  char *query = NULL;
  struct awe_sqlite3_exec *ptr = NULL;

  status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
  if (status != napi_ok) {
    goto on_error;
  }

  status = napi_get_value_external(env, argv[0], &userdata);
  if (status != napi_ok) {
    goto on_error;
  }

  query = (char *)malloc(MAX_QUERY_SIZE);
  if (query == NULL) {
    goto on_error;
  }

  status = napi_get_value_string_utf8(env,
                                       argv[1],
                                       query,
                                       MAX_QUERY_SIZE,
                                       NULL);
  if (status != napi_ok) {
    goto on_error;
  }

#ifdef _DEBUG
  fprintf(stderr, "debug: Execute SQLite3 session %u query `%s`\n", userdata->index, query);
#endif

  /* Step 3. Setup promise about close connection */
  ptr = (struct awe_sqlite3_exec *)malloc(sizeof(struct awe_sqlite3_exec));
  if (ptr == NULL) {
    goto on_error;
  }
  ptr->db = userdata->db;
  ptr->query = query;

  status = napi_create_promise(env, &ptr->deferred, &promise);
  if (status != napi_ok) {
    goto on_error;
  }

  status = napi_create_string_utf8(env, "awe-sqlite3-exec", NAPI_AUTO_LENGTH, &resource_name);
  if (status != napi_ok) {
    goto on_error;
  }

  status = napi_create_async_work(env, NULL, resource_name, awe_sqlite3_exec_execute, awe_sqlite3_exec_complete, (void *)ptr, &ptr->worker);
  if (status != napi_ok) {
    goto on_error;
  }

  status = napi_queue_async_work(env, ptr->worker);
  if (status != napi_ok) {
    goto on_error;
  }

  return promise;
on_error:
  free(query);
  free(ptr);
  const char *message = "error";
  napi_throw_error(env, NULL, message);
  return NULL;
}
