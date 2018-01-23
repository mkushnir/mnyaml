#ifndef MRKYAML_H_DEFINED
#define MRKYAML_H_DEFINED

#include <libgen.h>
#include <yaml.h>

#include <mrkcommon/array.h>
#include <mrkcommon/bytes.h>
#include <mrkcommon/bytestream.h>
#include <mrkcommon/dumpm.h>
#include <mrkcommon/util.h>

#define YAML_TOKEN_TYPE_STR(ty)                                                \
((ty) == YAML_NO_TOKEN ? "NO_TOKEN" :                                          \
 (ty) == YAML_STREAM_START_TOKEN ? "STREAM_START_TOKEN" :                      \
 (ty) == YAML_STREAM_END_TOKEN ? "STREAM_END_TOKEN" :                          \
 (ty) == YAML_VERSION_DIRECTIVE_TOKEN ? "VESION_DIRECTIVE_TOKEN" :             \
 (ty) == YAML_TAG_DIRECTIVE_TOKEN ? "TAG_DIRECTIVE_TOKEN" :                    \
 (ty) == YAML_DOCUMENT_START_TOKEN ? "DOCUMENT_START_TOKEN" :                  \
 (ty) == YAML_DOCUMENT_END_TOKEN ? "DOCUMENT_END_TOKEN" :                      \
 (ty) == YAML_BLOCK_SEQUENCE_START_TOKEN ? "BLOCK_SEQUENCE_START_TOKEN" :      \
 (ty) == YAML_BLOCK_MAPPING_START_TOKEN ? "BLOCK_MAPPING_START_TOKEN" :        \
 (ty) == YAML_BLOCK_END_TOKEN ? "BLOCK_END_TOKEN" :                            \
 (ty) == YAML_FLOW_SEQUENCE_START_TOKEN ? "FLOW_SEQUENCE_START_TOKEN" :        \
 (ty) == YAML_FLOW_SEQUENCE_END_TOKEN ? "FLOW_SEQUENCE_END_TOKEN" :            \
 (ty) == YAML_FLOW_MAPPING_START_TOKEN ? "FLOW_MAPPING_START_TOKEN" :          \
 (ty) == YAML_FLOW_MAPPING_END_TOKEN ? "FLOW_MAPPING_END_TOKEN" :              \
 (ty) == YAML_BLOCK_ENTRY_TOKEN ? "BLOCK_ENTRY_TOKEN" :                        \
 (ty) == YAML_FLOW_ENTRY_TOKEN ? "FLOW_ENTRY_TOKEN" :                          \
 (ty) == YAML_KEY_TOKEN ? "KEY_TOKEN" :                                        \
 (ty) == YAML_VALUE_TOKEN ? "VALUE_TOKEN" :                                    \
 (ty) == YAML_ALIAS_TOKEN ? "ALIAS_TOKEN" :                                    \
 (ty) == YAML_ANCHOR_TOKEN ? "ANCHOR_TOKEN" :                                  \
 (ty) == YAML_TAG_TOKEN ? "TAG_TOKEN" :                                        \
 (ty) == YAML_SCALAR_TOKEN ? "SCALAR_TOKEN" :                                  \
 "<unknown>")                                                                  \


#define YAML_EVENT_TYPE_STR(ev)                                \
((ev) == YAML_NO_EVENT ? "NO_EVENT" :                          \
 (ev) == YAML_STREAM_START_EVENT ? "STREAM_START_EVENT" :      \
 (ev) == YAML_STREAM_END_EVENT ? "STREAM_END_EVENT" :          \
 (ev) == YAML_DOCUMENT_START_EVENT ? "DOCUMENT_START_EVENT" :  \
 (ev) == YAML_DOCUMENT_END_EVENT ? "DOCUMENT_END_EVENT" :      \
 (ev) == YAML_ALIAS_EVENT ? "ALIAS_EVENT" :                    \
 (ev) == YAML_SCALAR_EVENT ? "SCALAR_EVENT" :                  \
 (ev) == YAML_SEQUENCE_START_EVENT ? "SEQUENCE_START_EVENT" :  \
 (ev) == YAML_SEQUENCE_END_EVENT ? "SEQUENCE_END_EVENT" :      \
 (ev) == YAML_MAPPING_START_EVENT ? "MAPPING_START_EVENT" :    \
 (ev) == YAML_MAPPING_END_EVENT ? "MAPPING_END_EVENT" :        \
 "<unknown>")                                                  \


#define YAML_ERROR_TYPE_STR(ty)                        \
((ty) == YAML_NO_ERROR ? "NO_ERROR" :                  \
 (ty) == YAML_MEMORY_ERROR ? "MEMORY_ERROR" :          \
 (ty) == YAML_READER_ERROR ? "READER_ERROR" :          \
 (ty) == YAML_SCANNER_ERROR ? "SCANNER_ERROR" :        \
 (ty) == YAML_PARSER_ERROR ? "PARSER_ERROR" :          \
 (ty) == YAML_COMPOSER_ERROR ? "COMPOSER_ERROR" :      \
 (ty) == YAML_WRITER_ERROR ? "WRITER_ERROR" :          \
 (ty) == YAML_EMITTER_ERROR ? "EMITTER_ERROR" :        \
 "unknown")                                            \


#define YAML_NODE_TYPE_STR(ty)                 \
((ty) == YAML_NO_NODE ? "NO" :                 \
 (ty) == YAML_SCALAR_NODE ? "SCALAR" :         \
 (ty) == YAML_SEQUENCE_NODE ? "SEQUENCE" :     \
 (ty) == YAML_MAPPING_NODE ? "MAPPING" :       \
 "<unknown>")                                  \


#define YM_CAST_TAG_NF (-2)
#define YM_CHECK_NODE_NF (-3)
#define YM_TRAVERSE_NODES_NF (-4)
#define YM_CHECK_NODE_SEQ_NF (-5)
#define YM_INIT_INVALID (-6)
#define YM_PARSE_INTO_ERROR (-7)

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _ym_node_info {
    char *tag;
    char *name;
    int (*init)(void *, yaml_node_t *);
    int (*fini)(void *);
    ssize_t (*str)(mnbytestream_t *, void *);
    int (*cmp)(void *, void *);
    void *(*addr)(void *);
#define YM_FLAG_IGNORE_UNKNOWN_SUBNODES (0x01)
    uintptr_t flags;
    struct _ym_node_info *subs[];
} ym_node_info_t;


typedef struct _ym_node_info_traverse_ctx {
    const char *nsep;
    const char *sub0;
    const char *sub1;
    mnbytes_t *prefix;
} ym_node_info_traverse_ctx_t;


typedef struct _ym_enum {
    char *_name;
    int _value;
} ym_enum_t;

typedef int (*ym_node_info_traverser_t)(ym_node_info_traverse_ctx_t *,
                                        ym_node_info_t *,
                                        void *,
                                        void *);

typedef int (*ym_node_info_traverser2_t)(ym_node_info_traverse_ctx_t *,
                                        ym_node_info_t *,
                                        void *,
                                        void *,
                                        void *);

#define YM_NAME(scope, name) _ym_ ## scope ## name
#define YM_INIT(scope, name) _ym_ ## scope ## name ## _init
#define YM_FINI(scope, name) _ym_ ## scope ## name ## _fini
#define YM_STR(scope, name) _ym_ ## scope ## name ## _str
#define YM_CMP(scope, name) _ym_ ## scope ## name ## _cmp
#define YM_ADDR(scope, name) _ym_ ## scope ## name ## _addr


#define YM_ADDR_TY(scope, name, n)     \
static void *                          \
YM_ADDR(scope, name)(void *data)       \
{                                      \
    YM_CONFIG_TYPE *root = data;       \
    __typeof__(&root->n) v = &root->n; \
    return (void *)v;                  \
}                                      \


#define YM_ADDR_TY0(scope, name, ty)   \
static void *                          \
YM_ADDR(scope, name)(void *data)       \
{                                      \
    return data;                       \
}                                      \



/*
 * boolean
 */
#define YM_INIT_BOOL(scope, name, n)                           \
static int                                                     \
YM_INIT(scope, name)(void *data, yaml_node_t *node)            \
{                                                              \
    int res;                                                   \
    YM_CONFIG_TYPE *root = data;                               \
    __typeof__(&root->n) v = &root->n;                         \
    if ((res = ym_can_cast_tag(node, YAML_BOOL_TAG)) != 0) {   \
        TRACE("expected %s found %s",                          \
              YAML_BOOL_TAG,                                   \
              node->tag);                                      \
        return res;                                            \
    }                                                          \
    /*TRACE("ptr=%s", node->data.scalar.value); */             \
    if (strcmp((char *)node->tag, YAML_STR_TAG) == 0) {        \
        if (strncasecmp((char *)node->data.scalar.value,       \
                        "true",                                \
                        node->data.scalar.length + 1) == 0 ||  \
            strncasecmp((char *)node->data.scalar.value,       \
                        "yes",                                 \
                        node->data.scalar.length + 1) == 0 ||  \
            strncasecmp((char *)node->data.scalar.value,       \
                        "on",                                  \
                        node->data.scalar.length + 1) == 0) {  \
            *v = 1;                                            \
        } else {                                               \
            *v = 0;                                            \
        }                                                      \
    }                                                          \
    /*TRACE("v=%ld", (intmax_t)*v); */                         \
    return 0;                                                  \
}                                                              \


#define YM_FINI_BOOL(scope, name, n)                           \
static int                                                     \
YM_FINI(scope, name)(void *data)                               \
{                                                              \
    YM_CONFIG_TYPE *root = data;                               \
    __typeof__(&root->n) v = &root->n;                         \
/*    TRACE("f=%p v=%p (bool)", YM_FINI(scope, name), v);      \
    TRACE("v=%ld (bool)", (intmax_t)*v) */;                    \
    *v = 0;                                                    \
    return 0;                                                  \
}                                                              \


#define YM_STR_BOOL(scope, name, n)                    \
static ssize_t                                         \
YM_STR(scope, name)(mnbytestream_t *bs, void *data)      \
{                                                      \
    YM_CONFIG_TYPE *root = data;                       \
    __typeof__(&root->n) v = &root->n;                 \
    return bytestream_nprintf(bs,                      \
                              16,                      \
                              "%s",                    \
                              *v ? "on" : "off");      \
}                                                      \


#ifdef YM_CMP_DEBUG
#define YM_CMP_DEBUG_BOOL(n) if (ra->n != rb->n) TRACE("%d/%d", ra->n, rb->n);
#else
#define YM_CMP_DEBUG_BOOL(n)
#endif

#define YM_CMP_BOOL(scope, name, n)    \
static int                             \
YM_CMP(scope, name)(void *a, void *b)  \
{                                      \
    YM_CONFIG_TYPE *ra = a;            \
    YM_CONFIG_TYPE *rb = b;            \
    YM_CMP_DEBUG_BOOL(n)               \
    return MNCMP(ra->n, rb->n);        \
}                                      \


/*
 * int0
 */
#define YM_INIT_INT0(scope, name, ty)                          \
static int YM_INIT(scope, name)(void *data, yaml_node_t *node) \
{                                                              \
    int res;                                                   \
    intmax_t v = 0;                                            \
    ty *vv = data;                                             \
    char *ptr, *endptr;                                        \
    if ((res = ym_can_cast_tag(node, YAML_INT_TAG)) != 0) {    \
        TRACE("expected %s found %s",                          \
              YAML_INT_TAG,                                    \
              node->tag);                                      \
        return res;                                            \
    }                                                          \
    /*TRACE("ptr=%s", node->data.scalar.value) */;             \
    ptr = (char *)node->data.scalar.value;                     \
    v = strtoimax(ptr, &endptr, 0);                            \
    if (ptr == endptr) {                                       \
        TRACE("invalid int '%s'", ptr);                        \
        return YM_INIT_INVALID;                                \
    }                                                          \
    /*TRACE("v=%ld", v); */                                    \
    *vv = (ty)v;                                               \
    return 0;                                                  \
}                                                              \


#define YM_FINI_INT0(scope, name, ty)                                  \
static int YM_FINI(scope, name)(void *data)                            \
{                                                                      \
    ty *v = data;                                                      \
/*    TRACE("v=%p (int0)", v);                                         \
    TRACE("v=%ld (int0)", (intmax_t)*v); */                            \
    *v = 0;                                                            \
    return 0;                                                          \
}                                                                      \


#define YM_STR_INT0(scope, name, ty)                   \
static ssize_t                                         \
YM_STR(scope, name)(mnbytestream_t *bs, void *data)      \
{                                                      \
    ty *v = data;                                      \
    return bytestream_nprintf(bs,                      \
                              32,                      \
                              "%ld",                   \
                              (intmax_t)*v);           \
}                                                      \


#ifdef YM_CMP_DEBUG
#define YM_CMP_DEBUG_INT0 if (*va != *vb) TRACE("%ld/%ld", (intmax_t)*va, (intmax_t)*vb);
#else
#define YM_CMP_DEBUG_INT0
#endif

#define YM_CMP_INT0(scope, name, ty)   \
static int                             \
YM_CMP(scope, name)(void *a, void *b)  \
{                                      \
    ty *va = a;                        \
    ty *vb = b;                        \
    YM_CMP_DEBUG_INT0                  \
    return MNCMP(*va, *vb);            \
}                                      \


/*
 * int, enum
 */
#define YM_INIT_INT(scope, name, n)                            \
static int YM_INIT(scope, name)(void *data, yaml_node_t *node) \
{                                                              \
    int res;                                                   \
    intmax_t v = 0;                                            \
    YM_CONFIG_TYPE *root = data;                               \
    __typeof__(&root->n) vv = &root->n;                        \
    char *ptr, *endptr;                                        \
    if ((res = ym_can_cast_tag(node, YAML_INT_TAG)) != 0) {    \
        TRACE("expected %s found %s",                          \
              YAML_INT_TAG,                                    \
              node->tag);                                      \
        return res;                                            \
    }                                                          \
    /*TRACE("ptr=%s", node->data.scalar.value); */             \
    ptr = (char *)node->data.scalar.value;                     \
    v = strtoimax(ptr, &endptr, 0);                            \
    if (ptr == endptr) {                                       \
        TRACE("invalid int '%s'", ptr);                        \
        return YM_INIT_INVALID;                                \
    }                                                          \
    /*TRACE("v=%ld", v); */                                    \
    *vv = (__typeof__(root->n))v;                              \
    return 0;                                                  \
}                                                              \


#define YM_INIT_INT_CEHCKRANGE(scope, name, n, a, b)           \
static int YM_INIT(scope, name)(void *data, yaml_node_t *node) \
{                                                              \
    int res;                                                   \
    intmax_t v = 0;                                            \
    YM_CONFIG_TYPE *root = data;                               \
    __typeof__(&root->n) vv = &root->n;                        \
    char *ptr, *endptr;                                        \
    if ((res = ym_can_cast_tag(node, YAML_INT_TAG)) != 0) {    \
        TRACE("expected %s found %s",                          \
              YAML_INT_TAG,                                    \
              node->tag);                                      \
        return res;                                            \
    }                                                          \
    /*TRACE("ptr=%s", node->data.scalar.value); */             \
    ptr = (char *)node->data.scalar.value;                     \
    v = strtoimax(ptr, &endptr, 0);                            \
    if (ptr == endptr) {                                       \
        TRACE("invalid int '%s'", ptr);                        \
        return YM_INIT_INVALID;                                \
    }                                                          \
    if ((v < a) || (v > b)) {                                  \
        TRACE("int '%s' is out of range %ld..%ld",             \
              ptr,                                             \
              (intmax_t)a,                                     \
              (intmax_t)b);                                    \
        return YM_INIT_INVALID;                                \
    }                                                          \
    /*TRACE("v=%ld", v); */                                    \
    *vv = (__typeof__(root->n))v;                              \
    return 0;                                                  \
}                                                              \


#define YM_INIT_ENUM(scope, name, n, en)                       \
static int YM_INIT(scope, name)(void *data, yaml_node_t *node) \
{                                                              \
    int res;                                                   \
    YM_CONFIG_TYPE *root = data;                               \
    __typeof__(&root->n) vv = &root->n;                        \
    char *ptr;                                                 \
    size_t i;                                                  \
    if ((res = ym_can_cast_tag(node, YAML_INT_TAG)) != 0) {    \
        TRACE("expected %s found %s",                          \
              YAML_INT_TAG,                                    \
              node->tag);                                      \
        return res;                                            \
    }                                                          \
    /*TRACE("ptr=%s", node->data.scalar.value); */             \
    ptr = (char *)node->data.scalar.value;                     \
    for (i = 0; i < countof(en); ++i) {                        \
        ym_enum_t *e;                                          \
        e = &en[i];                                            \
        if (strcmp(ptr, e->_name) == 0) {                      \
            *vv = (__typeof__(root->n))e->_value;              \
            return 0;                                          \
        }                                                      \
    }                                                          \
    TRACE("enumeration is not known: %s", ptr);                \
    return YM_INIT_INVALID;                                    \
}                                                              \


#define YM_FINI_INT(scope, name, n)                                    \
static int YM_FINI(scope, name)(void *data)                            \
{                                                                      \
    YM_CONFIG_TYPE *root = data;                                       \
    __typeof__(&root->n) v = &root->n;                                 \
/*    TRACE("v=%p (int)", v);                                          \
    TRACE("v=%ld (int)", (intmax_t)*v) */;                             \
    *v = 0;                                                            \
    return 0;                                                          \
}                                                                      \


#define YM_STR_INT(scope, name, n)                     \
static ssize_t                                         \
YM_STR(scope, name)(mnbytestream_t *bs, void *data)      \
{                                                      \
    YM_CONFIG_TYPE *root = data;                       \
    __typeof__(&root->n) v = &root->n;                 \
    return bytestream_nprintf(bs,                      \
                              32,                      \
                              "%ld",                   \
                              (intmax_t)*v);           \
}                                                      \


#define YM_STR_ENUM(scope, name, n, en)                        \
static ssize_t                                                 \
YM_STR(scope, name)(mnbytestream_t *bs, void *data)              \
{                                                              \
    YM_CONFIG_TYPE *root = data;                               \
    __typeof__(&root->n) v = &root->n;                         \
    size_t i;                                                  \
    for (i = 0; i < countof(en); ++i) {                        \
        ym_enum_t *e;                                          \
        e = &en[i];                                            \
        if ((int)(*v) == e->_value) {                          \
            return bytestream_nprintf(bs,                      \
                                      32 + strlen(e->_name),   \
                                      "'%s'",                  \
                                      e->_name);               \
        }                                                      \
    }                                                          \
    TRACE("enumeration is not known: %ld", (intmax_t)*v);      \
    return 0;                                                  \
}                                                              \


#ifdef YM_CMP_DEBUG
#define YM_CMP_DEBUG_INT(n) if (ra->n != rb->n) TRACE("%ld/%ld", (intmax_t)ra->n, (intmax_t)rb->n);
#else
#define YM_CMP_DEBUG_INT(n)
#endif

#define YM_CMP_INT(scope, name, n)     \
static int                             \
YM_CMP(scope, name)(void *a, void *b)  \
{                                      \
    YM_CONFIG_TYPE *ra = a;            \
    YM_CONFIG_TYPE *rb = b;            \
    YM_CMP_DEBUG_INT(n)                \
    return MNCMP(ra->n, rb->n);        \
}                                      \


/*
 * float
 */
#define YM_INIT_FLOAT(scope, name, n)                          \
static int YM_INIT(scope, name)(void *data, yaml_node_t *node) \
{                                                              \
    int res;                                                   \
    double v;                                                  \
    YM_CONFIG_TYPE *root = data;                               \
    __typeof__(&root->n) vv = &root->n;                        \
    char *ptr, *endptr;                                        \
    if ((res = ym_can_cast_tag(node, YAML_FLOAT_TAG)) != 0) {  \
        TRACE("expected %s found %s",                          \
              YAML_FLOAT_TAG,                                  \
              node->tag);                                      \
        return res;                                            \
    }                                                          \
    /*TRACE("ptr=%s", node->data.scalar.value); */             \
    ptr = (char *)node->data.scalar.value;                     \
    v = strtod(ptr, &endptr);                                  \
    if (ptr == endptr) {                                       \
        TRACE("invalid float '%s'", ptr);                      \
        return YM_INIT_INVALID;                                \
    }                                                          \
    /*TRACE("v=%lf", v); */                                    \
    *vv = (__typeof__(root->n))v;                              \
    return 0;                                                  \
}                                                              \


#define YM_INIT_FLOAT_CHECKRANGE(scope, name, n, a, b)         \
static int YM_INIT(scope, name)(void *data, yaml_node_t *node) \
{                                                              \
    int res;                                                   \
    double v;                                                  \
    YM_CONFIG_TYPE *root = data;                               \
    __typeof__(&root->n) vv = &root->n;                        \
    char *ptr, *endptr;                                        \
    if ((res = ym_can_cast_tag(node, YAML_FLOAT_TAG)) != 0) {  \
        TRACE("expected %s found %s",                          \
              YAML_FLOAT_TAG,                                  \
              node->tag);                                      \
        return res;                                            \
    }                                                          \
    /*TRACE("ptr=%s", node->data.scalar.value); */             \
    ptr = (char *)node->data.scalar.value;                     \
    v = strtod(ptr, &endptr);                                  \
    if (ptr == endptr) {                                       \
        TRACE("invalid float '%s'", ptr);                      \
        return YM_INIT_INVALID;                                \
    }                                                          \
    if ((v < a) || (v > b)) {                                  \
        TRACE("float '%s' is out of range %lf..%lf",           \
              ptr,                                             \
              (double)a,                                       \
              (double)b);                                      \
        return YM_INIT_INVALID;                                \
    }                                                          \
    /*TRACE("v=%lf", v); */                                    \
    *vv = (__typeof__(root->n))v;                              \
    return 0;                                                  \
}                                                              \


#define YM_FINI_FLOAT(scope, name, n)                                  \
static int YM_FINI(scope, name)(void *data)                            \
{                                                                      \
    YM_CONFIG_TYPE *root = data;                                       \
    __typeof__(&root->n) v = &root->n;                                 \
/*    TRACE("v=%p (float)", v);                                        \
    TRACE("v=%lf (float)", (double)*v) */;                             \
    *v = 0.0;                                                          \
    return 0;                                                          \
}                                                                      \


#define YM_STR_FLOAT(scope, name, n)                   \
static ssize_t                                         \
YM_STR(scope, name)(mnbytestream_t *bs, void *data)      \
{                                                      \
    YM_CONFIG_TYPE *root = data;                       \
    __typeof__(&root->n) v = &root->n;                 \
    return bytestream_nprintf(bs,                      \
                              32,                      \
                              "%lf",                   \
                              (double)*v);             \
}                                                      \


#ifdef YM_CMP_DEBUG
#define YM_CMP_DEBUG_FLOAT(n) if (ra->n != rb->n) TRACE("%lf/%lf", (double)ra->n, (double)rb->n);
#else
#define YM_CMP_DEBUG_FLOAT(n)
#endif

#define YM_CMP_FLOAT(scope, name, n)   \
static int                             \
YM_CMP(scope, name)(void *a, void *b)  \
{                                      \
    YM_CONFIG_TYPE *ra = a;            \
    YM_CONFIG_TYPE *rb = b;            \
    YM_CMP_DEBUG_FLOAT(n)              \
    return MNCMP(ra->n, rb->n);        \
}                                      \


/*
 * str0 (mnbytes_t *)
 */
#define YM_INIT_STR0(scope, name)                              \
static int YM_INIT(scope, name)(void *data, yaml_node_t *node) \
{                                                              \
    int res;                                                   \
    mnbytes_t **v = data;                                        \
    char *ptr;                                                 \
    if ((res = ym_can_cast_tag(node, YAML_STR_TAG)) != 0) {    \
        TRACE("expected %s found %s",                          \
              YAML_STR_TAG,                                    \
              node->tag);                                      \
        return res;                                            \
    }                                                          \
    /*TRACE("ptr=%s", node->data.scalar.value); */             \
    ptr = (char *)node->data.scalar.value;                     \
    BYTES_DECREF(v);                                           \
    *v = bytes_new_from_str(ptr);                              \
    BYTES_INCREF(*v);                                          \
    /*TRACE("v=%s", BDATA(*v)); */                             \
    return 0;                                                  \
}                                                              \


#define YM_FINI_STR0(scope, name)              \
static int YM_FINI(scope, name)(void *data)    \
{                                              \
    mnbytes_t **v = data;                        \
/*    TRACE("v=%p (str)", v);                  \
    TRACE("v=%s (str)", BDATA(*v)) */;         \
    BYTES_DECREF(v);                           \
    return 0;                                  \
}                                              \


#define YM_STR_STR0(scope, name)                       \
static ssize_t                                         \
YM_STR(scope, name)(mnbytestream_t *bs, void *data)      \
{                                                      \
    mnbytes_t **v = data;                                \
    if (*v == NULL) {                                  \
        return 0;                                      \
    }                                                  \
    return bytestream_nprintf(bs,                      \
                              32 + (*v)->sz,           \
                              "'%s'",                  \
                              BDATA(*v));              \
}                                                      \


#ifdef YM_CMP_DEBUG
#define YM_CMP_DEBUG_STR0(n, res) if(res) TRACE("%s/%s", BDATA(*va), BDATA(*vb));
#else
#define YM_CMP_DEBUG_STR0(n, res)
#endif


#define YM_CMP_STR0(scope, name)       \
static int                             \
YM_CMP(scope, name)(void *a, void *b)  \
{                                      \
    mnbytes_t **va = a;                  \
    mnbytes_t **vb = b;                  \
    if (*va == NULL) {                 \
        if (*vb == NULL) {             \
            return 0;                  \
        } else {                       \
            YM_CMP_DEBUG_STR0(n, 1)    \
            return -1;                 \
        }                              \
    } else {                           \
        if (*vb == NULL) {             \
            YM_CMP_DEBUG_STR0(n, 1)    \
            return 1;                  \
        } else {                       \
            int res;                   \
            res = bytes_cmp(*va, *vb); \
            YM_CMP_DEBUG_STR0(n, res)  \
            return res;                \
        }                              \
    }                                  \
    return 0;                          \
}                                      \


/*
 * str
 */
#define YM_INIT_STR(scope, name, n)                            \
static int YM_INIT(scope, name)(void *data, yaml_node_t *node) \
{                                                              \
    int res;                                                   \
    YM_CONFIG_TYPE *root = data;                               \
    __typeof__(&root->n) v = &root->n;                         \
    char *ptr;                                                 \
    if ((res = ym_can_cast_tag(node, YAML_STR_TAG)) != 0) {    \
        TRACE("expected %s found %s",                          \
              YAML_STR_TAG,                                    \
              node->tag);                                      \
        return res;                                            \
    }                                                          \
    /*TRACE("ptr=%s", node->data.scalar.value); */             \
    ptr = (char *)node->data.scalar.value;                     \
    BYTES_DECREF(v);                                           \
    *v = bytes_new_from_str(ptr);                              \
    BYTES_INCREF(*v);                                          \
    /*TRACE("v=%s", BDATA(*v)); */                             \
    return 0;                                                  \
}                                                              \


#define YM_INIT_STR_CHECKDIRNAME(scope, name, n)               \
static int YM_INIT(scope, name)(void *data, yaml_node_t *node) \
{                                                              \
    int res;                                                   \
    struct stat sb;                                            \
    YM_CONFIG_TYPE *root = data;                               \
    __typeof__(&root->n) v = &root->n;                         \
    char *ptr, *d;                                             \
    if ((res = ym_can_cast_tag(node, YAML_STR_TAG)) != 0) {    \
        TRACE("expected %s found %s",                          \
              YAML_STR_TAG,                                    \
              node->tag);                                      \
        return res;                                            \
    }                                                          \
    ptr = (char *)node->data.scalar.value;                     \
    /*TRACE("ptr=%s", ptr); */                                 \
    if (*ptr != '\0') {                                        \
        char *tmp;                                             \
        tmp = strdup(ptr);                                     \
        if ((d = dirname(tmp)) == NULL) {                      \
            TRACE("Could not get dirname from %s", ptr);       \
            free(tmp);                                         \
            return YM_INIT_INVALID;                            \
        }                                                      \
        if (stat(d, &sb) != 0) {                               \
            TRACE("Could not stat %s", d);                     \
            free(tmp);                                         \
            return YM_INIT_INVALID;                            \
        }                                                      \
        if (!S_ISDIR(sb.st_mode)) {                            \
            TRACE("Not a directory %s", d);                    \
            free(tmp);                                         \
            return YM_INIT_INVALID;                            \
        }                                                      \
        free(tmp);                                             \
    }                                                          \
    BYTES_DECREF(v);                                           \
    *v = bytes_new_from_str(ptr);                              \
    BYTES_INCREF(*v);                                          \
    /*TRACE("v=%s", BDATA(*v)); */                             \
    return 0;                                                  \
}                                                              \


#define YM_INIT_STR_CHECKDIRNAME_AUTOCREATE(scope, name, n)    \
static int YM_INIT(scope, name)(void *data, yaml_node_t *node) \
{                                                              \
    int res;                                                   \
    struct stat sb;                                            \
    YM_CONFIG_TYPE *root = data;                               \
    __typeof__(&root->n) v = &root->n;                         \
    char *ptr, *d;                                             \
    char *tmp;                                                 \
    if ((res = ym_can_cast_tag(node, YAML_STR_TAG)) != 0) {    \
        TRACE("expected %s found %s",                          \
              YAML_STR_TAG,                                    \
              node->tag);                                      \
        return res;                                            \
    }                                                          \
    ptr = (char *)node->data.scalar.value;                     \
    tmp = strdup(ptr);                                         \
    /*TRACE("ptr=%s", ptr); */                                 \
    if (*ptr != '\0') {                                        \
        if ((d = dirname(tmp)) == NULL) {                      \
            TRACE("Could not get dirname from %s", ptr);       \
            free(tmp);                                         \
            return YM_INIT_INVALID;                            \
        }                                                      \
        if (stat(d, &sb) == 0) {                               \
            if (S_ISDIR(sb.st_mode)) {                         \
                goto end;                                      \
            } else {                                           \
                TRACE("Not a directory %s", d);                \
                free(tmp);                                     \
                return YM_INIT_INVALID;                        \
            }                                                  \
        } else {                                               \
            if (mkdir(d, 0755) == 0) {                         \
                goto end;                                      \
            } else {                                           \
                TRACE("Could not mkdir %s", d);                \
                free(tmp);                                     \
                return YM_INIT_INVALID;                        \
            }                                                  \
        }                                                      \
    }                                                          \
end:                                                           \
    free(tmp);                                                 \
    BYTES_DECREF(v);                                           \
    *v = bytes_new_from_str(ptr);                              \
    BYTES_INCREF(*v);                                          \
    /*TRACE("v=%s", BDATA(*v)); */                             \
    return 0;                                                  \
}                                                              \


#define YM_INIT_STR_CHECKFILE(scope, name, n)                  \
static int YM_INIT(scope, name)(void *data, yaml_node_t *node) \
{                                                              \
    int res;                                                   \
    struct stat sb;                                            \
    YM_CONFIG_TYPE *root = data;                               \
    __typeof__(&root->n) v = &root->n;                         \
    char *ptr;                                                 \
    if ((res = ym_can_cast_tag(node, YAML_STR_TAG)) != 0) {    \
        TRACE("expected %s found %s",                          \
              YAML_STR_TAG,                                    \
              node->tag);                                      \
        return res;                                            \
    }                                                          \
    ptr = (char *)node->data.scalar.value;                     \
    /*TRACE("ptr=%s", ptr); */                                 \
    if (*ptr != '\0') {                                        \
        if (stat(ptr, &sb) != 0) {                             \
            TRACE("Could not stat %s", ptr);                   \
            return YM_INIT_INVALID;                            \
        }                                                      \
        if (!S_ISREG(sb.st_mode)) {                            \
            TRACE("Not a regular file %s", ptr);               \
            return YM_INIT_INVALID;                            \
        }                                                      \
    }                                                          \
    BYTES_DECREF(v);                                           \
    *v = bytes_new_from_str(ptr);                              \
    BYTES_INCREF(*v);                                          \
    /*TRACE("v=%s", BDATA(*v)); */                             \
    return 0;                                                  \
}                                                              \


#define YM_INIT_STR_CHECKDIR(scope, name, n)                   \
static int YM_INIT(scope, name)(void *data, yaml_node_t *node) \
{                                                              \
    int res;                                                   \
    struct stat sb;                                            \
    YM_CONFIG_TYPE *root = data;                               \
    __typeof__(&root->n) v = &root->n;                         \
    char *ptr;                                                 \
    if ((res = ym_can_cast_tag(node, YAML_STR_TAG)) != 0) {    \
        TRACE("expected %s found %s",                          \
              YAML_STR_TAG,                                    \
              node->tag);                                      \
        return res;                                            \
    }                                                          \
    ptr = (char *)node->data.scalar.value;                     \
    /*TRACE("ptr=%s", ptr); */                                 \
    if (*ptr != '\0') {                                        \
        if (stat(ptr, &sb) != 0) {                             \
            TRACE("Could not stat %s", ptr);                   \
            return YM_INIT_INVALID;                            \
        }                                                      \
        if (!S_ISDIR(sb.st_mode)) {                            \
            TRACE("Not a directory %s", ptr);                  \
            return YM_INIT_INVALID;                            \
        }                                                      \
    }                                                          \
    BYTES_DECREF(v);                                           \
    *v = bytes_new_from_str(ptr);                              \
    BYTES_INCREF(*v);                                          \
    /*TRACE("v=%s", BDATA(*v)); */                             \
    return 0;                                                  \
}                                                              \


#define YM_INIT_STR_CHECKDIR_AUTOCREATE(scope, name, n)        \
static int YM_INIT(scope, name)(void *data, yaml_node_t *node) \
{                                                              \
    int res;                                                   \
    struct stat sb;                                            \
    YM_CONFIG_TYPE *root = data;                               \
    __typeof__(&root->n) v = &root->n;                         \
    char *ptr;                                                 \
    if ((res = ym_can_cast_tag(node, YAML_STR_TAG)) != 0) {    \
        TRACE("expected %s found %s",                          \
              YAML_STR_TAG,                                    \
              node->tag);                                      \
        return res;                                            \
    }                                                          \
    ptr = (char *)node->data.scalar.value;                     \
    /*TRACE("ptr=%s", ptr); */                                 \
    if (*ptr != '\0') {                                        \
        if (stat(ptr, &sb) == 0) {                             \
            if (S_ISDIR(sb.st_mode)) {                         \
                goto end;                                      \
            } else {                                           \
                TRACE("Not a directory %s", ptr);              \
                return YM_INIT_INVALID;                        \
            }                                                  \
        } else {                                               \
            if (mkdir(ptr, 0755) == 0) {                       \
                goto end;                                      \
            } else {                                           \
                TRACE("Could not mkdir %s", ptr);              \
                return YM_INIT_INVALID;                        \
            }                                                  \
        }                                                      \
    }                                                          \
end:                                                           \
    BYTES_DECREF(v);                                           \
    *v = bytes_new_from_str(ptr);                              \
    BYTES_INCREF(*v);                                          \
    /*TRACE("v=%s", BDATA(*v)); */                             \
    return 0;                                                  \
}                                                              \


#define YM_FINI_STR(scope, name, n)            \
static int YM_FINI(scope, name)(void *data)    \
{                                              \
    YM_CONFIG_TYPE *root = data;               \
    __typeof__(&root->n) v = &root->n;         \
/*    TRACE("v=%p (str)", v);                  \
    TRACE("v=%s (str)", BDATA(*v)) */;         \
    BYTES_DECREF(v);                           \
    return 0;                                  \
}                                              \


#define YM_STR_STR(scope, name, n)                     \
static ssize_t                                         \
YM_STR(scope, name)(mnbytestream_t *bs, void *data)      \
{                                                      \
    YM_CONFIG_TYPE *root = data;                       \
    __typeof__(&root->n) v = &root->n;                 \
    if (*v == NULL) {                                  \
        return 0;                                      \
    }                                                  \
    return bytestream_nprintf(bs,                      \
                              32 + (*v)->sz,           \
                              "'%s'",                  \
                              BDATA(*v));              \
}                                                      \


#ifdef YM_CMP_DEBUG
#define YM_CMP_DEBUG_STR(n, res) if(res) TRACE("%s/%s", BDATA(ra->n), BDATA(rb->n));
#else
#define YM_CMP_DEBUG_STR(n, res)
#endif


#define YM_CMP_STR(scope, name, n)             \
static int                                     \
YM_CMP(scope, name)(void *a, void *b)          \
{                                              \
    YM_CONFIG_TYPE *ra = a;                    \
    YM_CONFIG_TYPE *rb = b;                    \
    if (ra->n == NULL) {                       \
        if (rb->n == NULL) {                   \
            return 0;                          \
        } else {                               \
            YM_CMP_DEBUG_STR(n, 1)             \
            return -1;                         \
        }                                      \
    } else {                                   \
        if (rb->n == NULL) {                   \
            YM_CMP_DEBUG_STR(n, 1)             \
            return 1;                          \
        } else {                               \
            int res;                           \
            res = bytes_cmp(ra->n, rb->n);     \
            YM_CMP_DEBUG_STR(n, res)           \
            return res;                        \
        }                                      \
    }                                          \
    return 0;                                  \
}                                              \


/*
 * seq
 */
#define YM_INIT_SEQ(scope, name, n, sz, init, fini)            \
static int YM_INIT(scope, name)(void *data, yaml_node_t *node) \
{                                                              \
    int res;                                                   \
    YM_CONFIG_TYPE *root = data;                               \
    __typeof__(&root->n) v = &root->n;                         \
    if ((res = ym_can_cast_tag(node, YAML_SEQ_TAG)) != 0) {    \
        TRACE("expected %s found %s",                          \
              YAML_SEQ_TAG,                                    \
              node->tag);                                      \
        return res;                                            \
    }                                                          \
    /*TRACE(FRED("a(0)=%p"), v); */                            \
/*    TRACE("v=%p init=%p fini=%p", v, init, fini); */         \
    array_init(v,                                              \
               sz,                                             \
               0,                                              \
               (array_initializer_t)init,                      \
               (array_finalizer_t)fini);                       \
    return 0;                                                  \
}                                                              \


#define YM_FINI_SEQ(scope, name, n)                                    \
static int YM_FINI(scope, name)(void *data)                            \
{                                                                      \
    YM_CONFIG_TYPE *root = data;                                       \
    __typeof__(&root->n) v = &root->n;                                 \
/*    TRACE("v=%p init=%p fini=%p (seq)", v, v->init, v->fini); */     \
    array_fini(v);                                                     \
    return 0;                                                          \
}                                                                      \


#define YM_STR_SEQ(scope, name, n)                             \
static ssize_t                                                 \
YM_STR(scope, name)(UNUSED mnbytestream_t *bs, UNUSED void *data)\
{                                                              \
    return 0;                                                  \
}                                                              \


#define YM_CMP_SEQ(scope, name, n)                     \
static int                                             \
YM_CMP(scope, name)(UNUSED void *a, UNUSED void *b)    \
{                                                      \
    return 0;                                          \
}                                                      \


/*
 * map
 */
#define YM_INIT_MAP(scope, name, n)                                            \
static int YM_INIT(scope, name)(UNUSED void *data, UNUSED yaml_node_t *node)   \
{                                                                              \
    return 0;                                                                  \
}                                                                              \


#define YM_FINI_MAP(scope, name, n)                                            \
static int YM_FINI(scope, name)(UNUSED void *data)                             \
{                                                                              \
/*    TRACE("v=%p (map)", data);                                               \
    TRACE("(map)") */;                                                         \
    return 0;                                                                  \
}                                                                              \


#define YM_STR_MAP(scope, name, n)                             \
static ssize_t                                                 \
YM_STR(scope, name)(UNUSED mnbytestream_t *bs, UNUSED void *data)\
{                                                              \
    return 0;                                                  \
}                                                              \


#define YM_CMP_MAP(scope, name, n)                     \
static int                                             \
YM_CMP(scope, name)(UNUSED void *a, UNUSED void *b)    \
{                                                      \
    return 0;                                          \
}                                                      \


/*
 * pair definitions
 */
#define YM_PAIR_TY0(scope, t, name, flags, ...)        \
UNUSED static ym_node_info_t YM_NAME(scope, name) = {  \
    t,                                                 \
    #name,                                             \
    NULL,                                              \
    NULL,                                              \
    NULL,                                              \
    NULL,                                              \
    NULL,                                              \
    flags,                                             \
    {__VA_ARGS__, NULL}                                \
}                                                      \


#define YM_PAIR_EXT(scope, t, name, flags, init, fini, str, cmp, addr, ...)    \
UNUSED static ym_node_info_t YM_NAME(scope, name) = {                          \
    t,                                                                         \
    #name,                                                                     \
    init,                                                                      \
    fini,                                                                      \
    str,                                                                       \
    cmp,                                                                       \
    addr,                                                                      \
    flags,                                                                     \
    {__VA_ARGS__, NULL}                                                        \
}                                                                              \


#define YM_PAIR_TY(scope, t, name, flags, ...)         \
UNUSED static ym_node_info_t YM_NAME(scope, name) = {  \
    t,                                                 \
    #name,                                             \
    YM_INIT(scope, name),                              \
    YM_FINI(scope, name),                              \
    YM_STR(scope, name),                               \
    YM_CMP(scope, name),                               \
    YM_ADDR(scope, name),                              \
    flags,                                             \
    {__VA_ARGS__, NULL}                                \
}                                                      \


#define YM_PAIR_TY1(scope, t, name, flags, init, ...)  \
UNUSED static ym_node_info_t YM_NAME(scope, name) = {  \
    t,                                                 \
    #name,                                             \
    init,                                              \
    YM_FINI(scope, name),                              \
    YM_STR(scope, name),                               \
    YM_CMP(scope, name),                               \
    YM_ADDR(scope, name),                              \
    flags,                                             \
    {__VA_ARGS__, NULL}                                \
}                                                      \


#define YM_PAIR_NULL(scope, name, flags, init)                 \
    YM_PAIR_TY0(scope, YAML_NULL_TAG, name, flags, NULL)       \


#define YM_PAIR_BOOL(scope, name, flags, n)            \
YM_INIT_BOOL(scope, name, n)                           \
YM_FINI_BOOL(scope, name, n)                           \
YM_STR_BOOL(scope, name, n)                            \
YM_CMP_BOOL(scope, name, n)                            \
YM_ADDR_TY(scope, name, n)                             \
YM_PAIR_TY(scope, YAML_BOOL_TAG, name, flags, NULL)    \


#define YM_PAIR_INT0(scope, name, flags, ty)           \
YM_INIT_INT0(scope, name, ty)                          \
YM_FINI_INT0(scope, name, ty)                          \
YM_STR_INT0(scope, name, ty)                           \
YM_CMP_INT0(scope, name, ty)                           \
YM_ADDR_TY0(scope, name, ty)                           \
YM_PAIR_TY(scope, YAML_INT_TAG, name, flags, NULL)     \


#define YM_PAIR_INT(scope, name, flags, n)             \
YM_INIT_INT(scope, name, n)                            \
YM_FINI_INT(scope, name, n)                            \
YM_STR_INT(scope, name, n)                             \
YM_CMP_INT(scope, name, n)                             \
YM_ADDR_TY(scope, name, n)                             \
YM_PAIR_TY(scope, YAML_INT_TAG, name, flags, NULL)     \


#define YM_PAIR_INT_CHECKRANGE(scope, name, flags, n, a, b)    \
YM_INIT_INT_CHECKRANGE(scope, name, n, a, b)                   \
YM_FINI_INT(scope, name, n)                                    \
YM_STR_INT(scope, name, n)                                     \
YM_CMP_INT(scope, name, n)                                     \
YM_ADDR_TY(scope, name, n)                                     \
YM_PAIR_TY(scope, YAML_INT_TAG, name, flags, NULL)             \


#define YM_PAIR_ENUM(scope, name, flags, n, en)        \
YM_INIT_ENUM(scope, name, n, en)                       \
YM_FINI_INT(scope, name, n)                            \
YM_STR_ENUM(scope, name, n, en)                        \
YM_CMP_INT(scope, name, n)                             \
YM_ADDR_TY(scope, name, n)                             \
YM_PAIR_TY(scope, YAML_INT_TAG, name, flags, NULL)     \


#define YM_PAIR_INT_EXT(scope, name, flags, n, init)           \
YM_FINI_INT(scope, name, n)                                    \
YM_STR_INT(scope, name, n)                                     \
YM_CMP_INT(scope, name, n)                                     \
YM_ADDR_TY(scope, name, n)                                     \
YM_PAIR_TY1(scope, YAML_INT_TAG, name, flags, init, NULL)      \


#define YM_PAIR_FLOAT(scope, name, flags, n)           \
YM_INIT_FLOAT(scope, name, n)                          \
YM_FINI_FLOAT(scope, name, n)                          \
YM_STR_FLOAT(scope, name, n)                           \
YM_CMP_FLOAT(scope, name, n)                           \
YM_ADDR_TY(scope, name, n)                             \
YM_PAIR_TY(scope, YAML_FLOAT_TAG, name, flags, NULL)   \


#define YM_PAIR_FLOAT_CHECKRANGE(scope, name, flags, n, a, b)  \
YM_INIT_FLOAT_CHECKRANGE(scope, name, n, a, b)                 \
YM_FINI_FLOAT(scope, name, n)                                  \
YM_STR_FLOAT(scope, name, n)                                   \
YM_CMP_FLOAT(scope, name, n)                                   \
YM_ADDR_TY(scope, name, n)                                     \
YM_PAIR_TY(scope, YAML_FLOAT_TAG, name, flags, NULL)           \


#define YM_PAIR_STR0(scope, name, flags)               \
YM_INIT_STR0(scope, name)                              \
YM_FINI_STR0(scope, name)                              \
YM_STR_STR0(scope, name)                               \
YM_CMP_STR0(scope, name)                               \
YM_ADDR_TY0(scope, name, mnbytes_t *)                    \
YM_PAIR_TY(scope, YAML_STR_TAG, name, flags, NULL)     \


#define YM_PAIR_STR(scope, name, flags, n)             \
YM_INIT_STR(scope, name, n)                            \
YM_FINI_STR(scope, name, n)                            \
YM_STR_STR(scope, name, n)                             \
YM_CMP_STR(scope, name, n)                             \
YM_ADDR_TY(scope, name, n)                             \
YM_PAIR_TY(scope, YAML_STR_TAG, name, flags, NULL)     \


#define YM_PAIR_STR_CHECKDIRNAME(scope, name, flags, n)\
YM_INIT_STR_CHECKDIRNAME(scope, name, n)               \
YM_FINI_STR(scope, name, n)                            \
YM_STR_STR(scope, name, n)                             \
YM_CMP_STR(scope, name, n)                             \
YM_ADDR_TY(scope, name, n)                             \
YM_PAIR_TY(scope, YAML_STR_TAG, name, flags, NULL)     \


#define YM_PAIR_STR_CHECKDIRNAME_AUTOCREATE(scope, name, flags, n)     \
YM_INIT_STR_CHECKDIRNAME_AUTOCREATE(scope, name, n)                    \
YM_FINI_STR(scope, name, n)                                            \
YM_STR_STR(scope, name, n)                                             \
YM_CMP_STR(scope, name, n)                                             \
YM_ADDR_TY(scope, name, n)                                             \
YM_PAIR_TY(scope, YAML_STR_TAG, name, flags, NULL)                     \


#define YM_PAIR_STR_CHECKFILE(scope, name, flags, n)   \
YM_INIT_STR_CHECKFILE(scope, name, n)                  \
YM_FINI_STR(scope, name, n)                            \
YM_STR_STR(scope, name, n)                             \
YM_CMP_STR(scope, name, n)                             \
YM_ADDR_TY(scope, name, n)                             \
YM_PAIR_TY(scope, YAML_STR_TAG, name, flags, NULL)     \


#define YM_PAIR_STR_CHECKDIR(scope, name, flags, n)    \
YM_INIT_STR_CHECKDIR(scope, name, n)                   \
YM_FINI_STR(scope, name, n)                            \
YM_STR_STR(scope, name, n)                             \
YM_CMP_STR(scope, name, n)                             \
YM_ADDR_TY(scope, name, n)                             \
YM_PAIR_TY(scope, YAML_STR_TAG, name, flags, NULL)     \


#define YM_PAIR_STR_CHECKDIR_AUTOCREATE(scope, name, flags, n) \
YM_INIT_STR_CHECKDIR_AUTOCREATE(scope, name, n)                \
YM_FINI_STR(scope, name, n)                                    \
YM_STR_STR(scope, name, n)                                     \
YM_CMP_STR(scope, name, n)                                     \
YM_ADDR_TY(scope, name, n)                                     \
YM_PAIR_TY(scope, YAML_STR_TAG, name, flags, NULL)             \


#define YM_PAIR_SEQ(scope, name, flags, n, sz, init, fini, ...)\
YM_INIT_SEQ(scope, name, n, sz, init, fini)                    \
YM_FINI_SEQ(scope, name, n)                                    \
YM_STR_SEQ(scope, name, n)                                     \
YM_CMP_SEQ(scope, name, n)                                     \
YM_ADDR_TY(scope, name, n)                                     \
YM_PAIR_TY(scope, YAML_SEQ_TAG, name, flags, __VA_ARGS__)      \



#define YM_PAIR_TIMESTAMP(scope, name, flags)                  \
    YM_PAIR_TY0(scope, YAML_TIMESTAMP_TAG, name, flags, NULL)  \


#define YM_PAIR_MAP(scope, name, flags, n, ...)                \
YM_INIT_MAP(scope, name, n)                                    \
YM_FINI_MAP(scope, name, n)                                    \
YM_STR_MAP(scope, name, n)                                     \
YM_CMP_MAP(scope, name, n)                                     \
YM_ADDR_TY(scope, name, n)                                     \
YM_PAIR_TY(scope, YAML_MAP_TAG, name, flags, __VA_ARGS__)      \


#define YM_PAIR_MAP0(scope, name, flags, ...)                  \
    YM_PAIR_TY0(scope, YAML_MAP_TAG, name, flags, __VA_ARGS__) \


#define YM_PAIR_MAP_EXT(scope, name, flags, init, fini, str, cmp, addr, ...)   \
UNUSED static ym_node_info_t YM_NAME(scope, name) = {                          \
    YAML_MAP_TAG,                                                              \
    #name,                                                                     \
    init,                                                                      \
    fini,                                                                      \
    str,                                                                       \
    cmp,                                                                       \
    addr,                                                                      \
    flags,                                                                     \
    {__VA_ARGS__, NULL}                                                        \
}                                                                              \




#define YM_PARSE_INTO(name, ni, ty)                                    \
static int                                                             \
name ## _doc_cb(yaml_document_t *doc,                                  \
                yaml_node_t *node,                                     \
                void *udata)                                           \
{                                                                      \
    int res;                                                           \
    struct {                                                           \
        ym_node_info_t *ninfo;                                         \
        ty *config;                                                    \
    } *params = udata;                                                 \
    /*                                                                 \
     * top-level node is mapping                                       \
     */                                                                \
    assert(strcmp(params->ninfo->tag, YAML_MAP_TAG) == 0);             \
    if ((res = ym_traverse_nodes(doc,                                  \
                                  params->ninfo,                       \
                                  node,                                \
                                  params->config)) != 0) {             \
        return res;                                                    \
    }                                                                  \
    return 0;                                                          \
                                                                       \
}                                                                      \
UNUSED static int                                                      \
name ## _parse_into(ty *co, yaml_read_handler_t cb, void *udata)       \
{                                                                      \
    int res;                                                           \
    yaml_parser_t p;                                                   \
    yaml_document_t doc;                                               \
                                                                       \
    if (!yaml_parser_initialize(&p)) {                                 \
        FAIL("yaml_parser_initialize");                                \
    }                                                                  \
    yaml_parser_set_input(&p, cb, udata);                              \
    if (yaml_parser_load(&p, &doc)) {                                  \
        struct {                                                       \
            ym_node_info_t *ninfo;                                     \
            ty *config;                                                \
        } params = { ni, co, };                                        \
                                                                       \
        res = traverse_yaml_document(&doc, name ## _doc_cb, &params);  \
        yaml_document_delete(&doc);                                    \
    } else {                                                           \
        res = YM_PARSE_INTO_ERROR;                                     \
        TRACE("error=%s %s: %s (line %ld column %ld)",                 \
              YAML_ERROR_TYPE_STR(p.error),                            \
              p.context,                                               \
              p.problem,                                               \
              p.problem_mark.line,                                     \
              p.problem_mark.column);                                  \
    }                                                                  \
    yaml_parser_delete(&p);                                            \
    return res;                                                        \
}                                                                      \




typedef int (*ym_node_traverser_t)(yaml_document_t *,
                                     yaml_node_t *,
                                     void *);


int ym_can_cast_tag(yaml_node_t *, char *);
int ym_traverse_nodes(yaml_document_t *,
                       ym_node_info_t *,
                       yaml_node_t *,
                       void *);

int ym_node_info_fini_data(ym_node_info_t *, void *);
int traverse_yaml_document(yaml_document_t *,
                       ym_node_traverser_t,
                       void *);
void ym_node_info_traverse_ctx_init(ym_node_info_traverse_ctx_t *,
                                    const char *,
                                    const char *,
                                    const char *,
                                    const char *);
void ym_node_info_traverse_ctx_fini(ym_node_info_traverse_ctx_t *);

int ym_node_info_traverse(ym_node_info_traverse_ctx_t *,
                          ym_node_info_t *,
                          void *,
                          ym_node_info_traverser_t,
                          void *);
int ym_node_info_traverse2(ym_node_info_traverse_ctx_t *,
                           ym_node_info_t *,
                           void *,
                           void *,
                           ym_node_info_traverser2_t,
                           void *);
int ym_node_info_cmp_data(ym_node_info_t *, void *, void *);

typedef struct _ym_read_params {
    mnbytestream_t bs;
    int fd;
} ym_read_params_t;
int ym_readfd(void *, unsigned char *, size_t, size_t *);
int ym_readbs(void *, unsigned char *, size_t, size_t *);

#ifdef __cplusplus
}
#endif
#endif /* MRKYAML_H_DEFINED */
