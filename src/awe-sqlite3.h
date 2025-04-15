
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sqlite3.h>

struct DatabaseWrapper {
    unsigned  index;  /* Database index     */
    sqlite3*  db;     /* Database pointer   */
    bool      closed; /* Database status    */
    napi_ref  ref;    /* Database reference */
};

napi_value Open(napi_env env, napi_callback_info info);
napi_value Exec(napi_env env, napi_callback_info info);
napi_value Close(napi_env env, napi_callback_info info);
