
#include <node_api.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>

#include "awe-sqlite3.h"


napi_value
awe_sqlite3_exec(napi_env env, napi_callback_info info) {
  napi_value promise;
  napi_value resource_name;
  size_t argc = 2;
  napi_value argv[2];
  napi_status status;
  struct awe_sqlite3 *userdata = NULL;
  char *query = NULL;
  int rc;
  char *err_msg = NULL;

  status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
  if (status != napi_ok) {
//      return NULL;
  }

  status = napi_get_value_external(env, argv[0], &userdata);
  if (status != napi_ok) {
//      return NULL;
  }
  
  query = (char *)malloc(65536);
  status = napi_get_value_string_utf8(env,
                                       argv[1],
                                       query,
                                       65536,
                                       NULL);

  /* Step 2. Debug message */
  fprintf(stdout, "debug: Exec SQLite3 query on connection #%d with SQL = %s\n", userdata->index, query);

  rc = sqlite3_exec(userdata->db, query, 0, 0, &err_msg);
  if (rc != SQLITE_OK) {
    // TODO - ...
  }

  return NULL;
}
