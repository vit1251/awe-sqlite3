
#include <node_api.h>

#include "awe-sqlite3.h"

struct awe_sqlite3_open {
    napi_async_work         worker;
    napi_deferred           deferred;
    char                   *path;
    struct DatabaseWrapper *wrapper;
    int                     err;
};

static void awe_sqlite3_open_execute(napi_env env, void* data) {
    struct awe_sqlite3_open *ptr = (struct awe_sqlite3_open *)data;
    struct DatabaseWrapper *wrapper = ptr->wrapper;
    int rv;

#ifdef _DEBUG
    fprintf(stderr, "debug: New SQLite3 session %u with %s\n", userdata->index, arg->path);
#endif

    /* Step 1. Open SQLite3 database */
    rv = sqlite3_open_v2(ptr->path, &wrapper->db, SQLITE_OPEN_FULLMUTEX | SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);

    /* Step 2. Save return value */
    ptr->err = rv;

}

static void awe_sqlite3_open_complete(napi_env env, napi_status status, void *data) {
    struct awe_sqlite3_open *ptr = (struct awe_sqlite3_open *)data;
    napi_value ret;

    /* Step 1. Resolve promise with SQLite3 session */
    if (ptr->err == 0) {
        napi_create_int32(env, ptr->err, &ret);
        napi_resolve_deferred(env, ptr->deferred, ret);
    }

    /* Step 2. Reject promise with error code */
    if (ptr->err != 0) {
        napi_create_int32(env, ptr->err, &ret);
        napi_reject_deferred(env, ptr->deferred, ret);
    }

    napi_delete_async_work(env, ptr->worker);

    free(ptr->path);
    free(ptr);
}

#define MAX_PATH 512

napi_value Open(napi_env env, napi_callback_info info) {
    struct DatabaseWrapper *wrapper = NULL;
    napi_value jsthis;
    napi_value promise;
    napi_value resource_name;
    napi_status status;
    size_t argc = 2;
    napi_value argv[2];
    char *path = NULL;
    struct awe_sqlite3_open *ptr = NULL;

    status = napi_get_cb_info(env, info, &argc, argv, &jsthis, NULL);
    if (status != napi_ok) {
        goto on_error;
    }

    status = napi_unwrap(env, jsthis, (void **)&wrapper);
    if (status != napi_ok) {
        goto on_error;
    }

    path = (char *)malloc(MAX_PATH);
    status = napi_get_value_string_utf8(env, argv[0], path, MAX_PATH, NULL);
    if (status != napi_ok) {
        goto on_error;
    }

    ptr = (struct awe_sqlite3_open *)malloc(sizeof(struct awe_sqlite3_open));
    if (ptr == NULL) {
        goto on_error;
    }
    ptr->wrapper = wrapper;
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

