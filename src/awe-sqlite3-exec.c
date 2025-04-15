
#include <node_api.h>

#include "awe-sqlite3.h"

struct awe_sqlite3_exec {
    struct DatabaseWrapper *wrapper;
    napi_async_work         worker;
    napi_deferred           deferred;
    sqlite3                *db;
    char                   *query;
    int                     err;
};

static void awe_sqlite3_exec_execute(napi_env env, void* data) {
    struct awe_sqlite3_exec *ptr = (struct awe_sqlite3_exec *)data;
    struct DatabaseWrapper *wrapper = ptr->wrapper;
    char *err_msg = NULL;
    int rv;

    /* Step 1. Perform SQL query */
    rv = sqlite3_exec(wrapper->db, ptr->query, 0, 0, &err_msg);
    if (rv != SQLITE_OK) {
    }

#ifdef _DEBUG
    fprintf(stderr, "error: Execute complete: rc = %u msg = %s\n", rc, err_msg);
#endif

    /* Step 2. Save error code */
    ptr->err = rv;

}

static void awe_sqlite3_exec_complete(napi_env env, napi_status status, void* data) {
    struct awe_sqlite3_exec *ptr = (struct awe_sqlite3_exec *)data;
    napi_value ret;

    if (ptr->err == 0) {
        napi_create_int32(env, ptr->err, &ret);
        napi_resolve_deferred(env, ptr->deferred, ret);
    }

    if (ptr->err != 0) {
        napi_create_int32(env, ptr->err, &ret);
        napi_reject_deferred(env, ptr->deferred, ret);
    }

    napi_delete_async_work(env, ptr->worker);

    /* Release memory */
    free(ptr->query);
    free(ptr);
}

#define MAX_QUERY_SIZE 65536

napi_value Exec(napi_env env, napi_callback_info info) {
    struct DatabaseWrapper *wrapper = NULL;
    napi_value jsthis;
    napi_value promise;
    napi_value resource_name;
    size_t argc = 2;
    napi_value argv[2];
    napi_status status;
    struct awe_sqlite3 *userdata = NULL;
    char *query = NULL;
    struct awe_sqlite3_exec *ptr = NULL;

    status = napi_get_cb_info(env, info, &argc, argv, &jsthis, NULL);
    if (status != napi_ok) {
        goto on_error;
    }

    status = napi_unwrap(env, jsthis, (void **)&wrapper);
    if (status != napi_ok) {
        goto on_error;
    }

    query = (char *)malloc(MAX_QUERY_SIZE);
    if (query == NULL) {
        goto on_error;
    }

    status = napi_get_value_string_utf8(env, argv[0], query, MAX_QUERY_SIZE, NULL);
    if (status != napi_ok) {
        goto on_error;
    }

#ifdef _DEBUG
    fprintf(stderr, "debug: Execute SQLite3 session %u query `%s`\n", wrapper->index, query);
#endif

    /* Step 3. Setup promise about close connection */
    ptr = (struct awe_sqlite3_exec *)malloc(sizeof(struct awe_sqlite3_exec));
    if (ptr == NULL) {
        goto on_error;
    }
    ptr->wrapper = wrapper;
    ptr->query = query;
    ptr->err = -1;

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
