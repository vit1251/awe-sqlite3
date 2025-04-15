
#include <node_api.h>

#include "awe-sqlite3.h"

static unsigned database_index = 0;

void DatabaseDestructor(napi_env env, void* data, void* hint) {
    struct DatabaseWrapper* wrapper = (struct DatabaseWrapper*)data;
    int rc;

    if (!wrapper->closed) {
#ifdef _DEBUG
        fprintf(stderr, "debug: close database\n");
#endif
        /* Step 2. Close database (sync operation may be create async work?) */
        rc = sqlite3_close(wrapper->db);
        if (rc != SQLITE_OK) {
        }
    }

    free(wrapper);
}


napi_value DatabaseConstructor(napi_env env, napi_callback_info info) {
    napi_status status;
    size_t argc = 1;
    napi_value argv[1];
    napi_value jsthis;

    status = napi_get_cb_info(env, info, &argc, argv, &jsthis, NULL);
    if (status != napi_ok) {
        return NULL;
    }

    struct DatabaseWrapper* wrapper = (struct DatabaseWrapper*)malloc(sizeof(struct DatabaseWrapper));
    wrapper->index = database_index++;
    wrapper->db = NULL;
    wrapper->closed = false;

    status = napi_wrap(env, jsthis, wrapper, DatabaseDestructor, NULL, &wrapper->ref);
    if (status != napi_ok) {
        free(wrapper);
        return NULL;
    }

    return jsthis;
}


napi_value Init(napi_env env, napi_value exports) {
    napi_status status;

    napi_property_descriptor properties[] = {
      { "Open", NULL, Open, NULL, NULL, NULL, napi_default, NULL },
      { "Exec", NULL, Exec, NULL, NULL, NULL, napi_default, NULL },
      { "Close", NULL, Close, NULL, NULL, NULL, napi_default, NULL },
    };

    napi_value dbClass;
    status = napi_define_class(env, "Database", NAPI_AUTO_LENGTH, DatabaseConstructor, NULL, sizeof(properties) / sizeof(properties[0]),
        properties, &dbClass);
    if (status != napi_ok) {
        return NULL;
    }

    status = napi_set_named_property(env, exports, "Database", dbClass);
    if (status != napi_ok) {
        return NULL;
    }

    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init);
