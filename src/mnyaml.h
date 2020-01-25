#ifndef MNYAML_H_DEFINED
#define MNYAML_H_DEFINED

/*
 * https://yaml.org/type/
 */

#include <libgen.h>
#include <yaml.h>

#include <mncommon/array.h>
#include <mncommon/bytes.h>
#include <mncommon/bytestream.h>
#include <mncommon/dumpm.h>
#include <mncommon/util.h>

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


#define MNY_CAST_TAG_NF (-2)
#define MNY_CHECK_NODE_NF (-3)
#define MNY_TRAVERSE_NODES_NF (-4)
#define MNY_CHECK_NODE_SEQ_NF (-5)
#define MNY_INIT_INVALID (-6)
#define MNY_PARSE_INTO_ERROR (-7)

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _mny_node_info {
    char *tag;
    char *name;
    int (*init)(void *, yaml_node_t *);
    int (*fini)(void *);
    ssize_t (*str)(mnbytestream_t *, void *);
    int (*cmp)(void *, void *);
    void *(*addr)(void *);
#define MNY_FLAG_IGNORE_UNKNOWN_SUBNODES (0x01)
    uintptr_t flags;
    struct _mny_node_info *subs[];
} mny_node_info_t;


typedef struct _mny_node_info_traverse_ctx {
    const char *nsep;
    const char *sub0;
    const char *sub1;
    mnbytes_t *prefix;
} mny_node_info_traverse_ctx_t;


typedef struct _mny_enum {
    char *_name;
    int _value;
} mny_enum_t;

typedef int (*mny_node_info_traverser_t)(mny_node_info_traverse_ctx_t *,
                                         mny_node_info_t *,
                                         void *,
                                         void *);

typedef int (*mny_node_info_traverser2_t)(mny_node_info_traverse_ctx_t *,
                                          mny_node_info_t *,
                                          void *,
                                          void *,
                                          void *);

#define MNY_NAME(scope, name) _mny_ ## scope ## name
#define MNY_INIT(scope, name) _mny_ ## scope ## name ## _init
#define MNY_FINI(scope, name) _mny_ ## scope ## name ## _fini
#define MNY_STR(scope, name) _mny_ ## scope ## name ## _str
#define MNY_CMP(scope, name) _mny_ ## scope ## name ## _cmp
#define MNY_ADDR(scope, name) _mny_ ## scope ## name ## _addr


#define MNY_ADDR_TY(scope, name, n)    \
static void *                          \
MNY_ADDR(scope, name)(void *data)      \
{                                      \
    MNY_CONFIG_TYPE *root = data;      \
    __typeof__(&root->n) v = &root->n; \
    return (void *)v;                  \
}                                      \


#define MNY_ADDR_TY0(scope, name, ty)  \
static void *                          \
MNY_ADDR(scope, name)(void *data)      \
{                                      \
    return data;                       \
}                                      \



/*
 * boolean
 */
#define MNY_INIT_BOOL(scope, name, n)                          \
static int                                                     \
MNY_INIT(scope, name)(void *data, yaml_node_t *node)           \
{                                                              \
    int res;                                                   \
    MNY_CONFIG_TYPE *root = data;                              \
    __typeof__(&root->n) v = &root->n;                         \
    if ((res = mny_can_cast_tag(node, YAML_BOOL_TAG)) != 0) {  \
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


#define MNY_FINI_BOOL(scope, name, n)                          \
static int                                                     \
MNY_FINI(scope, name)(void *data)                              \
{                                                              \
    MNY_CONFIG_TYPE *root = data;                              \
    __typeof__(&root->n) v = &root->n;                         \
/*    TRACE("f=%p v=%p (bool)", MNY_FINI(scope, name), v);     \
    TRACE("v=%ld (bool)", (intmax_t)*v) */;                    \
    *v = 0;                                                    \
    return 0;                                                  \
}                                                              \


#define MNY_STR_BOOL(scope, name, n)                   \
static ssize_t                                         \
MNY_STR(scope, name)(mnbytestream_t *bs, void *data)   \
{                                                      \
    MNY_CONFIG_TYPE *root = data;                      \
    __typeof__(&root->n) v = &root->n;                 \
    return bytestream_nprintf(bs,                      \
                              16,                      \
                              "%s",                    \
                              *v ? "on" : "off");      \
}                                                      \


#ifdef MNY_CMP_DEBUG
#define MNY_CMP_DEBUG_BOOL(n) if (ra->n != rb->n) TRACE("%d/%d", ra->n, rb->n);
#else
#define MNY_CMP_DEBUG_BOOL(n)
#endif

#define MNY_CMP_BOOL(scope, name, n)   \
static int                             \
MNY_CMP(scope, name)(void *a, void *b) \
{                                      \
    MNY_CONFIG_TYPE *ra = a;           \
    MNY_CONFIG_TYPE *rb = b;           \
    MNY_CMP_DEBUG_BOOL(n)              \
    return MNCMP(ra->n, rb->n);        \
}                                      \


/*
 * int0
 */
#define MNY_INIT_INT0(scope, name, ty)                         \
static int MNY_INIT(scope, name)(void *data, yaml_node_t *node)\
{                                                              \
    int res;                                                   \
    intmax_t v = 0;                                            \
    ty *vv = data;                                             \
    char *ptr, *endptr;                                        \
    if ((res = mny_can_cast_tag(node, YAML_INT_TAG)) != 0) {   \
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
        return MNY_INIT_INVALID;                               \
    }                                                          \
    /*TRACE("v=%ld", v); */                                    \
    *vv = (ty)v;                                               \
    return 0;                                                  \
}                                                              \


#define MNY_FINI_INT0(scope, name, ty)         \
static int MNY_FINI(scope, name)(void *data)   \
{                                              \
    ty *v = data;                              \
/*    TRACE("v=%p (int0)", v);                 \
    TRACE("v=%ld (int0)", (intmax_t)*v); */    \
    *v = 0;                                    \
    return 0;                                  \
}                                              \


#define MNY_STR_INT0(scope, name, ty)                  \
static ssize_t                                         \
MNY_STR(scope, name)(mnbytestream_t *bs, void *data)   \
{                                                      \
    ty *v = data;                                      \
    return bytestream_nprintf(bs,                      \
                              32,                      \
                              "%ld",                   \
                              (intmax_t)*v);           \
}                                                      \


#ifdef MNY_CMP_DEBUG
#define MNY_CMP_DEBUG_INT0 if (*va != *vb) TRACE("%ld/%ld", (intmax_t)*va, (intmax_t)*vb);
#else
#define MNY_CMP_DEBUG_INT0
#endif

#define MNY_CMP_INT0(scope, name, ty)  \
static int                             \
MNY_CMP(scope, name)(void *a, void *b) \
{                                      \
    ty *va = a;                        \
    ty *vb = b;                        \
    MNY_CMP_DEBUG_INT0                 \
    return MNCMP(*va, *vb);            \
}                                      \


/*
 * int, enum
 */
#define MNY_INIT_INT(scope, name, n)                           \
static int MNY_INIT(scope, name)(void *data, yaml_node_t *node)\
{                                                              \
    int res;                                                   \
    intmax_t v = 0;                                            \
    MNY_CONFIG_TYPE *root = data;                              \
    __typeof__(&root->n) vv = &root->n;                        \
    char *ptr, *endptr;                                        \
    if ((res = mny_can_cast_tag(node, YAML_INT_TAG)) != 0) {   \
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
        return MNY_INIT_INVALID;                               \
    }                                                          \
    /*TRACE("v=%ld", v); */                                    \
    *vv = (__typeof__(root->n))v;                              \
    return 0;                                                  \
}                                                              \


#define MNY_INIT_INT_CHECKRANGE(scope, name, n, a, b)          \
static int MNY_INIT(scope, name)(void *data, yaml_node_t *node)\
{                                                              \
    int res;                                                   \
    intmax_t v = 0;                                            \
    MNY_CONFIG_TYPE *root = data;                              \
    __typeof__(&root->n) vv = &root->n;                        \
    char *ptr, *endptr;                                        \
    if ((res = mny_can_cast_tag(node, YAML_INT_TAG)) != 0) {   \
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
        return MNY_INIT_INVALID;                               \
    }                                                          \
    if ((v < a) || (v > b)) {                                  \
        TRACE("int '%s' is out of range %ld..%ld",             \
              ptr,                                             \
              (intmax_t)a,                                     \
              (intmax_t)b);                                    \
        return MNY_INIT_INVALID;                               \
    }                                                          \
    /*TRACE("v=%ld", v); */                                    \
    *vv = (__typeof__(root->n))v;                              \
    return 0;                                                  \
}                                                              \


#define MNY_INIT_ENUM(scope, name, n, en)                      \
static int MNY_INIT(scope, name)(void *data, yaml_node_t *node)\
{                                                              \
    int res;                                                   \
    MNY_CONFIG_TYPE *root = data;                              \
    __typeof__(&root->n) vv = &root->n;                        \
    char *ptr;                                                 \
    size_t i;                                                  \
    if ((res = mny_can_cast_tag(node, YAML_INT_TAG)) != 0) {   \
        TRACE("expected %s found %s",                          \
              YAML_INT_TAG,                                    \
              node->tag);                                      \
        return res;                                            \
    }                                                          \
    /*TRACE("ptr=%s", node->data.scalar.value); */             \
    ptr = (char *)node->data.scalar.value;                     \
    for (i = 0; i < countof(en); ++i) {                        \
        mny_enum_t *e;                                         \
        e = &en[i];                                            \
        if (strcmp(ptr, e->_name) == 0) {                      \
            *vv = (__typeof__(root->n))e->_value;              \
            return 0;                                          \
        }                                                      \
    }                                                          \
    TRACE("enumeration is not known: %s", ptr);                \
    return MNY_INIT_INVALID;                                   \
}                                                              \


#define MNY_FINI_INT(scope, name, n)           \
static int MNY_FINI(scope, name)(void *data)   \
{                                              \
    MNY_CONFIG_TYPE *root = data;              \
    __typeof__(&root->n) v = &root->n;         \
/*    TRACE("v=%p (int)", v);                  \
    TRACE("v=%ld (int)", (intmax_t)*v) */;     \
    *v = 0;                                    \
    return 0;                                  \
}                                              \


#define MNY_STR_INT(scope, name, n)                    \
static ssize_t                                         \
MNY_STR(scope, name)(mnbytestream_t *bs, void *data)   \
{                                                      \
    MNY_CONFIG_TYPE *root = data;                      \
    __typeof__(&root->n) v = &root->n;                 \
    return bytestream_nprintf(bs,                      \
                              32,                      \
                              "%ld",                   \
                              (intmax_t)*v);           \
}                                                      \


#define MNY_STR_ENUM(scope, name, n, en)                       \
static ssize_t                                                 \
MNY_STR(scope, name)(mnbytestream_t *bs, void *data)           \
{                                                              \
    MNY_CONFIG_TYPE *root = data;                              \
    __typeof__(&root->n) v = &root->n;                         \
    size_t i;                                                  \
    for (i = 0; i < countof(en); ++i) {                        \
        mny_enum_t *e;                                         \
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


#ifdef MNY_CMP_DEBUG
#define MNY_CMP_DEBUG_INT(n) if (ra->n != rb->n) TRACE("%ld/%ld", (intmax_t)ra->n, (intmax_t)rb->n);
#else
#define MNY_CMP_DEBUG_INT(n)
#endif

#define MNY_CMP_INT(scope, name, n)    \
static int                             \
MNY_CMP(scope, name)(void *a, void *b) \
{                                      \
    MNY_CONFIG_TYPE *ra = a;           \
    MNY_CONFIG_TYPE *rb = b;           \
    MNY_CMP_DEBUG_INT(n)               \
    return MNCMP(ra->n, rb->n);        \
}                                      \


/*
 * float
 */
#define MNY_INIT_FLOAT(scope, name, n)                         \
static int MNY_INIT(scope, name)(void *data, yaml_node_t *node)\
{                                                              \
    int res;                                                   \
    double v;                                                  \
    MNY_CONFIG_TYPE *root = data;                              \
    __typeof__(&root->n) vv = &root->n;                        \
    char *ptr, *endptr;                                        \
    if ((res = mny_can_cast_tag(node, YAML_FLOAT_TAG)) != 0) { \
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
        return MNY_INIT_INVALID;                               \
    }                                                          \
    /*TRACE("v=%lf", v); */                                    \
    *vv = (__typeof__(root->n))v;                              \
    return 0;                                                  \
}                                                              \


#define MNY_INIT_FLOAT_CHECKRANGE(scope, name, n, a, b)        \
static int MNY_INIT(scope, name)(void *data, yaml_node_t *node)\
{                                                              \
    int res;                                                   \
    double v;                                                  \
    MNY_CONFIG_TYPE *root = data;                              \
    __typeof__(&root->n) vv = &root->n;                        \
    char *ptr, *endptr;                                        \
    if ((res = mny_can_cast_tag(node, YAML_FLOAT_TAG)) != 0) { \
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
        return MNY_INIT_INVALID;                               \
    }                                                          \
    if ((v < a) || (v > b)) {                                  \
        TRACE("float '%s' is out of range %lf..%lf",           \
              ptr,                                             \
              (double)a,                                       \
              (double)b);                                      \
        return MNY_INIT_INVALID;                               \
    }                                                          \
    /*TRACE("v=%lf", v); */                                    \
    *vv = (__typeof__(root->n))v;                              \
    return 0;                                                  \
}                                                              \


#define MNY_FINI_FLOAT(scope, name, n)         \
static int MNY_FINI(scope, name)(void *data)   \
{                                              \
    MNY_CONFIG_TYPE *root = data;              \
    __typeof__(&root->n) v = &root->n;         \
/*    TRACE("v=%p (float)", v);                \
    TRACE("v=%lf (float)", (double)*v) */;     \
    *v = 0.0;                                  \
    return 0;                                  \
}                                              \


#define MNY_STR_FLOAT(scope, name, n)                  \
static ssize_t                                         \
MNY_STR(scope, name)(mnbytestream_t *bs, void *data)   \
{                                                      \
    MNY_CONFIG_TYPE *root = data;                      \
    __typeof__(&root->n) v = &root->n;                 \
    return bytestream_nprintf(bs,                      \
                              32,                      \
                              "%lf",                   \
                              (double)*v);             \
}                                                      \


#ifdef MNY_CMP_DEBUG
#define MNY_CMP_DEBUG_FLOAT(n) if (ra->n != rb->n) TRACE("%lf/%lf", (double)ra->n, (double)rb->n);
#else
#define MNY_CMP_DEBUG_FLOAT(n)
#endif

#define MNY_CMP_FLOAT(scope, name, n)  \
static int                             \
MNY_CMP(scope, name)(void *a, void *b) \
{                                      \
    MNY_CONFIG_TYPE *ra = a;           \
    MNY_CONFIG_TYPE *rb = b;           \
    MNY_CMP_DEBUG_FLOAT(n)             \
    return MNCMP(ra->n, rb->n);        \
}                                      \


/*
 * str0 (mnbytes_t *)
 */
#define MNY_INIT_STR0(scope, name)                             \
static int MNY_INIT(scope, name)(void *data, yaml_node_t *node)\
{                                                              \
    int res;                                                   \
    mnbytes_t **v = data;                                      \
    char *ptr;                                                 \
    if ((res = mny_can_cast_tag(node, YAML_STR_TAG)) != 0) {   \
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


#define MNY_FINI_STR0(scope, name)             \
static int MNY_FINI(scope, name)(void *data)   \
{                                              \
    mnbytes_t **v = data;                      \
/*    TRACE("v=%p (str)", v);                  \
    TRACE("v=%s (str)", BDATA(*v)) */;         \
    BYTES_DECREF(v);                           \
    return 0;                                  \
}                                              \


#define MNY_STR_STR0(scope, name)                      \
static ssize_t                                         \
MNY_STR(scope, name)(mnbytestream_t *bs, void *data)   \
{                                                      \
    mnbytes_t **v = data;                              \
    if (*v == NULL) {                                  \
        return 0;                                      \
    }                                                  \
    return bytestream_nprintf(bs,                      \
                              32 + (*v)->sz,           \
                              "'%s'",                  \
                              BDATA(*v));              \
}                                                      \


#ifdef MNY_CMP_DEBUG
#define MNY_CMP_DEBUG_STR0(n, res) if(res) TRACE("%s/%s", BDATA(*va), BDATA(*vb));
#else
#define MNY_CMP_DEBUG_STR0(n, res)
#endif


#define MNY_CMP_STR0(scope, name)      \
static int                             \
MNY_CMP(scope, name)(void *a, void *b) \
{                                      \
    mnbytes_t **va = a;                \
    mnbytes_t **vb = b;                \
    if (*va == NULL) {                 \
        if (*vb == NULL) {             \
            return 0;                  \
        } else {                       \
            MNY_CMP_DEBUG_STR0(n, 1)   \
            return -1;                 \
        }                              \
    } else {                           \
        if (*vb == NULL) {             \
            MNY_CMP_DEBUG_STR0(n, 1)   \
            return 1;                  \
        } else {                       \
            int res;                   \
            res = bytes_cmp(*va, *vb); \
            MNY_CMP_DEBUG_STR0(n, res) \
            return res;                \
        }                              \
    }                                  \
    return 0;                          \
}                                      \


/*
 * str
 */
#define MNY_INIT_STR(scope, name, n)                           \
static int MNY_INIT(scope, name)(void *data, yaml_node_t *node)\
{                                                              \
    int res;                                                   \
    MNY_CONFIG_TYPE *root = data;                              \
    __typeof__(&root->n) v = &root->n;                         \
    char *ptr;                                                 \
    if ((res = mny_can_cast_tag(node, YAML_STR_TAG)) != 0) {   \
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


#define MNY_INIT_STR_CHECKDIRNAME(scope, name, n)              \
static int MNY_INIT(scope, name)(void *data, yaml_node_t *node)\
{                                                              \
    int res;                                                   \
    struct stat sb;                                            \
    MNY_CONFIG_TYPE *root = data;                              \
    __typeof__(&root->n) v = &root->n;                         \
    char *ptr, *d;                                             \
    if ((res = mny_can_cast_tag(node, YAML_STR_TAG)) != 0) {   \
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
            return MNY_INIT_INVALID;                           \
        }                                                      \
        if (stat(d, &sb) != 0) {                               \
            TRACE("Could not stat %s", d);                     \
            free(tmp);                                         \
            return MNY_INIT_INVALID;                           \
        }                                                      \
        if (!S_ISDIR(sb.st_mode)) {                            \
            TRACE("Not a directory %s", d);                    \
            free(tmp);                                         \
            return MNY_INIT_INVALID;                           \
        }                                                      \
        free(tmp);                                             \
    }                                                          \
    BYTES_DECREF(v);                                           \
    *v = bytes_new_from_str(ptr);                              \
    BYTES_INCREF(*v);                                          \
    /*TRACE("v=%s", BDATA(*v)); */                             \
    return 0;                                                  \
}                                                              \


#define MNY_INIT_STR_CHECKDIRNAME_AUTOCREATE(scope, name, n)   \
static int MNY_INIT(scope, name)(void *data, yaml_node_t *node)\
{                                                              \
    int res;                                                   \
    struct stat sb;                                            \
    MNY_CONFIG_TYPE *root = data;                              \
    __typeof__(&root->n) v = &root->n;                         \
    char *ptr, *d;                                             \
    char *tmp;                                                 \
    if ((res = mny_can_cast_tag(node, YAML_STR_TAG)) != 0) {   \
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
            return MNY_INIT_INVALID;                           \
        }                                                      \
        if (stat(d, &sb) == 0) {                               \
            if (S_ISDIR(sb.st_mode)) {                         \
                goto end;                                      \
            } else {                                           \
                TRACE("Not a directory %s", d);                \
                free(tmp);                                     \
                return MNY_INIT_INVALID;                       \
            }                                                  \
        } else {                                               \
            if (mkdir(d, 0755) == 0) {                         \
                goto end;                                      \
            } else {                                           \
                TRACE("Could not mkdir %s", d);                \
                free(tmp);                                     \
                return MNY_INIT_INVALID;                       \
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


#define MNY_INIT_STR_CHECKFILE(scope, name, n)                 \
static int MNY_INIT(scope, name)(void *data, yaml_node_t *node)\
{                                                              \
    int res;                                                   \
    struct stat sb;                                            \
    MNY_CONFIG_TYPE *root = data;                              \
    __typeof__(&root->n) v = &root->n;                         \
    char *ptr;                                                 \
    if ((res = mny_can_cast_tag(node, YAML_STR_TAG)) != 0) {   \
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
            return MNY_INIT_INVALID;                           \
        }                                                      \
        if (!S_ISREG(sb.st_mode)) {                            \
            TRACE("Not a regular file %s", ptr);               \
            return MNY_INIT_INVALID;                           \
        }                                                      \
    }                                                          \
    BYTES_DECREF(v);                                           \
    *v = bytes_new_from_str(ptr);                              \
    BYTES_INCREF(*v);                                          \
    /*TRACE("v=%s", BDATA(*v)); */                             \
    return 0;                                                  \
}                                                              \


#define MNY_INIT_STR_CHECKDIR(scope, name, n)                  \
static int MNY_INIT(scope, name)(void *data, yaml_node_t *node)\
{                                                              \
    int res;                                                   \
    struct stat sb;                                            \
    MNY_CONFIG_TYPE *root = data;                              \
    __typeof__(&root->n) v = &root->n;                         \
    char *ptr;                                                 \
    if ((res = mny_can_cast_tag(node, YAML_STR_TAG)) != 0) {   \
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
            return MNY_INIT_INVALID;                           \
        }                                                      \
        if (!S_ISDIR(sb.st_mode)) {                            \
            TRACE("Not a directory %s", ptr);                  \
            return MNY_INIT_INVALID;                           \
        }                                                      \
    }                                                          \
    BYTES_DECREF(v);                                           \
    *v = bytes_new_from_str(ptr);                              \
    BYTES_INCREF(*v);                                          \
    /*TRACE("v=%s", BDATA(*v)); */                             \
    return 0;                                                  \
}                                                              \


#define MNY_INIT_STR_CHECKDIR_AUTOCREATE(scope, name, n)       \
static int MNY_INIT(scope, name)(void *data, yaml_node_t *node)\
{                                                              \
    int res;                                                   \
    struct stat sb;                                            \
    MNY_CONFIG_TYPE *root = data;                              \
    __typeof__(&root->n) v = &root->n;                         \
    char *ptr;                                                 \
    if ((res = mny_can_cast_tag(node, YAML_STR_TAG)) != 0) {   \
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
                return MNY_INIT_INVALID;                       \
            }                                                  \
        } else {                                               \
            if (mkdir(ptr, 0755) == 0) {                       \
                goto end;                                      \
            } else {                                           \
                TRACE("Could not mkdir %s", ptr);              \
                return MNY_INIT_INVALID;                       \
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


#define MNY_FINI_STR(scope, name, n)           \
static int MNY_FINI(scope, name)(void *data)   \
{                                              \
    MNY_CONFIG_TYPE *root = data;              \
    __typeof__(&root->n) v = &root->n;         \
/*    TRACE("v=%p (str)", v);                  \
    TRACE("v=%s (str)", BDATA(*v)) */;         \
    BYTES_DECREF(v);                           \
    return 0;                                  \
}                                              \


#define MNY_STR_STR(scope, name, n)                    \
static ssize_t                                         \
MNY_STR(scope, name)(mnbytestream_t *bs, void *data)   \
{                                                      \
    MNY_CONFIG_TYPE *root = data;                      \
    __typeof__(&root->n) v = &root->n;                 \
    if (*v == NULL) {                                  \
        return 0;                                      \
    }                                                  \
    return bytestream_nprintf(bs,                      \
                              32 + (*v)->sz,           \
                              "'%s'",                  \
                              BDATA(*v));              \
}                                                      \


#ifdef MNY_CMP_DEBUG
#define MNY_CMP_DEBUG_STR(n, res) if(res) TRACE("%s/%s", BDATA(ra->n), BDATA(rb->n));
#else
#define MNY_CMP_DEBUG_STR(n, res)
#endif


#define MNY_CMP_STR(scope, name, n)            \
static int                                     \
MNY_CMP(scope, name)(void *a, void *b)         \
{                                              \
    MNY_CONFIG_TYPE *ra = a;                   \
    MNY_CONFIG_TYPE *rb = b;                   \
    if (ra->n == NULL) {                       \
        if (rb->n == NULL) {                   \
            return 0;                          \
        } else {                               \
            MNY_CMP_DEBUG_STR(n, 1)            \
            return -1;                         \
        }                                      \
    } else {                                   \
        if (rb->n == NULL) {                   \
            MNY_CMP_DEBUG_STR(n, 1)            \
            return 1;                          \
        } else {                               \
            int res;                           \
            res = bytes_cmp(ra->n, rb->n);     \
            MNY_CMP_DEBUG_STR(n, res)          \
            return res;                        \
        }                                      \
    }                                          \
    return 0;                                  \
}                                              \


/*
 * seq
 */
#define MNY_INIT_SEQ(scope, name, n, sz, init, fini)           \
static int MNY_INIT(scope, name)(void *data, yaml_node_t *node)\
{                                                              \
    int res;                                                   \
    MNY_CONFIG_TYPE *root = data;                              \
    __typeof__(&root->n) v = &root->n;                         \
    if ((res = mny_can_cast_tag(node, YAML_SEQ_TAG)) != 0) {   \
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


#define MNY_FINI_SEQ(scope, name, n)                                   \
static int MNY_FINI(scope, name)(void *data)                           \
{                                                                      \
    MNY_CONFIG_TYPE *root = data;                                      \
    __typeof__(&root->n) v = &root->n;                                 \
/*    TRACE("v=%p init=%p fini=%p (seq)", v, v->init, v->fini); */     \
    array_fini(v);                                                     \
    return 0;                                                          \
}                                                                      \


#define MNY_STR_SEQ(scope, name, n)                                    \
static ssize_t                                                         \
MNY_STR(scope, name)(UNUSED mnbytestream_t *bs, UNUSED void *data)     \
{                                                                      \
    return 0;                                                          \
}                                                                      \


#define MNY_CMP_SEQ(scope, name, n)                    \
static int                                             \
MNY_CMP(scope, name)(UNUSED void *a, UNUSED void *b)   \
{                                                      \
    return 0;                                          \
}                                                      \


/*
 * map
 */
#define MNY_INIT_MAP(scope, name, n)                                           \
static int MNY_INIT(scope, name)(UNUSED void *data, UNUSED yaml_node_t *node)  \
{                                                                              \
    return 0;                                                                  \
}                                                                              \


#define MNY_FINI_MAP(scope, name, n)                   \
static int MNY_FINI(scope, name)(UNUSED void *data)    \
{                                                      \
/*    TRACE("v=%p (map)", data);                       \
    TRACE("(map)") */;                                 \
    return 0;                                          \
}                                                      \


#define MNY_STR_MAP(scope, name, n)                                    \
static ssize_t                                                         \
MNY_STR(scope, name)(UNUSED mnbytestream_t *bs, UNUSED void *data)     \
{                                                                      \
    return 0;                                                          \
}                                                                      \


#define MNY_CMP_MAP(scope, name, n)                    \
static int                                             \
MNY_CMP(scope, name)(UNUSED void *a, UNUSED void *b)   \
{                                                      \
    return 0;                                          \
}                                                      \


/*
 * pair definitions
 */
#define MNY_PAIR_TY0(scope, t, name, flags, ...)       \
UNUSED static mny_node_info_t MNY_NAME(scope, name) = {\
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


#define MNY_PAIR_EXT(scope, t, name, flags, init, fini, str, cmp, addr, ...)   \
UNUSED static mny_node_info_t MNY_NAME(scope, name) = {                        \
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


#define MNY_PAIR_TY(scope, t, name, flags, ...)        \
UNUSED static mny_node_info_t MNY_NAME(scope, name) = {\
    t,                                                 \
    #name,                                             \
    MNY_INIT(scope, name),                             \
    MNY_FINI(scope, name),                             \
    MNY_STR(scope, name),                              \
    MNY_CMP(scope, name),                              \
    MNY_ADDR(scope, name),                             \
    flags,                                             \
    {__VA_ARGS__, NULL}                                \
}                                                      \


#define MNY_PAIR_TY1(scope, t, name, flags, init, ...) \
UNUSED static mny_node_info_t MNY_NAME(scope, name) = {\
    t,                                                 \
    #name,                                             \
    init,                                              \
    MNY_FINI(scope, name),                             \
    MNY_STR(scope, name),                              \
    MNY_CMP(scope, name),                              \
    MNY_ADDR(scope, name),                             \
    flags,                                             \
    {__VA_ARGS__, NULL}                                \
}                                                      \


#define MNY_PAIR_NULL(scope, name, flags, init)                \
    MNY_PAIR_TY0(scope, YAML_NULL_TAG, name, flags, NULL)      \


#define MNY_PAIR_BOOL(scope, name, flags, n)           \
MNY_INIT_BOOL(scope, name, n)                          \
MNY_FINI_BOOL(scope, name, n)                          \
MNY_STR_BOOL(scope, name, n)                           \
MNY_CMP_BOOL(scope, name, n)                           \
MNY_ADDR_TY(scope, name, n)                            \
MNY_PAIR_TY(scope, YAML_BOOL_TAG, name, flags, NULL)   \


#define MNY_PAIR_INT0(scope, name, flags, ty)          \
MNY_INIT_INT0(scope, name, ty)                         \
MNY_FINI_INT0(scope, name, ty)                         \
MNY_STR_INT0(scope, name, ty)                          \
MNY_CMP_INT0(scope, name, ty)                          \
MNY_ADDR_TY0(scope, name, ty)                          \
MNY_PAIR_TY(scope, YAML_INT_TAG, name, flags, NULL)    \


#define MNY_PAIR_INT(scope, name, flags, n)            \
MNY_INIT_INT(scope, name, n)                           \
MNY_FINI_INT(scope, name, n)                           \
MNY_STR_INT(scope, name, n)                            \
MNY_CMP_INT(scope, name, n)                            \
MNY_ADDR_TY(scope, name, n)                            \
MNY_PAIR_TY(scope, YAML_INT_TAG, name, flags, NULL)    \


#define MNY_PAIR_INT_CHECKRANGE(scope, name, flags, n, a, b)   \
MNY_INIT_INT_CHECKRANGE(scope, name, n, a, b)                  \
MNY_FINI_INT(scope, name, n)                                   \
MNY_STR_INT(scope, name, n)                                    \
MNY_CMP_INT(scope, name, n)                                    \
MNY_ADDR_TY(scope, name, n)                                    \
MNY_PAIR_TY(scope, YAML_INT_TAG, name, flags, NULL)            \


#define MNY_PAIR_ENUM(scope, name, flags, n, en)       \
MNY_INIT_ENUM(scope, name, n, en)                      \
MNY_FINI_INT(scope, name, n)                           \
MNY_STR_ENUM(scope, name, n, en)                       \
MNY_CMP_INT(scope, name, n)                            \
MNY_ADDR_TY(scope, name, n)                            \
MNY_PAIR_TY(scope, YAML_INT_TAG, name, flags, NULL)    \


#define MNY_PAIR_INT_EXT(scope, name, flags, n, init)          \
MNY_FINI_INT(scope, name, n)                                   \
MNY_STR_INT(scope, name, n)                                    \
MNY_CMP_INT(scope, name, n)                                    \
MNY_ADDR_TY(scope, name, n)                                    \
MNY_PAIR_TY1(scope, YAML_INT_TAG, name, flags, init, NULL)     \


#define MNY_PAIR_FLOAT(scope, name, flags, n)          \
MNY_INIT_FLOAT(scope, name, n)                         \
MNY_FINI_FLOAT(scope, name, n)                         \
MNY_STR_FLOAT(scope, name, n)                          \
MNY_CMP_FLOAT(scope, name, n)                          \
MNY_ADDR_TY(scope, name, n)                            \
MNY_PAIR_TY(scope, YAML_FLOAT_TAG, name, flags, NULL)  \


#define MNY_PAIR_FLOAT_CHECKRANGE(scope, name, flags, n, a, b) \
MNY_INIT_FLOAT_CHECKRANGE(scope, name, n, a, b)                \
MNY_FINI_FLOAT(scope, name, n)                                 \
MNY_STR_FLOAT(scope, name, n)                                  \
MNY_CMP_FLOAT(scope, name, n)                                  \
MNY_ADDR_TY(scope, name, n)                                    \
MNY_PAIR_TY(scope, YAML_FLOAT_TAG, name, flags, NULL)          \


#define MNY_PAIR_STR0(scope, name, flags)              \
MNY_INIT_STR0(scope, name)                             \
MNY_FINI_STR0(scope, name)                             \
MNY_STR_STR0(scope, name)                              \
MNY_CMP_STR0(scope, name)                              \
MNY_ADDR_TY0(scope, name, mnbytes_t *)                 \
MNY_PAIR_TY(scope, YAML_STR_TAG, name, flags, NULL)    \


#define MNY_PAIR_STR(scope, name, flags, n)            \
MNY_INIT_STR(scope, name, n)                           \
MNY_FINI_STR(scope, name, n)                           \
MNY_STR_STR(scope, name, n)                            \
MNY_CMP_STR(scope, name, n)                            \
MNY_ADDR_TY(scope, name, n)                            \
MNY_PAIR_TY(scope, YAML_STR_TAG, name, flags, NULL)    \


#define MNY_PAIR_STR_CHECKDIRNAME(scope, name, flags, n)       \
MNY_INIT_STR_CHECKDIRNAME(scope, name, n)                      \
MNY_FINI_STR(scope, name, n)                                   \
MNY_STR_STR(scope, name, n)                                    \
MNY_CMP_STR(scope, name, n)                                    \
MNY_ADDR_TY(scope, name, n)                                    \
MNY_PAIR_TY(scope, YAML_STR_TAG, name, flags, NULL)            \


#define MNY_PAIR_STR_CHECKDIRNAME_AUTOCREATE(scope, name, flags, n)    \
MNY_INIT_STR_CHECKDIRNAME_AUTOCREATE(scope, name, n)                   \
MNY_FINI_STR(scope, name, n)                                           \
MNY_STR_STR(scope, name, n)                                            \
MNY_CMP_STR(scope, name, n)                                            \
MNY_ADDR_TY(scope, name, n)                                            \
MNY_PAIR_TY(scope, YAML_STR_TAG, name, flags, NULL)                    \


#define MNY_PAIR_STR_CHECKFILE(scope, name, flags, n)  \
MNY_INIT_STR_CHECKFILE(scope, name, n)                 \
MNY_FINI_STR(scope, name, n)                           \
MNY_STR_STR(scope, name, n)                            \
MNY_CMP_STR(scope, name, n)                            \
MNY_ADDR_TY(scope, name, n)                            \
MNY_PAIR_TY(scope, YAML_STR_TAG, name, flags, NULL)    \


#define MNY_PAIR_STR_CHECKDIR(scope, name, flags, n)   \
MNY_INIT_STR_CHECKDIR(scope, name, n)                  \
MNY_FINI_STR(scope, name, n)                           \
MNY_STR_STR(scope, name, n)                            \
MNY_CMP_STR(scope, name, n)                            \
MNY_ADDR_TY(scope, name, n)                            \
MNY_PAIR_TY(scope, YAML_STR_TAG, name, flags, NULL)    \


#define MNY_PAIR_STR_CHECKDIR_AUTOCREATE(scope, name, flags, n)\
MNY_INIT_STR_CHECKDIR_AUTOCREATE(scope, name, n)               \
MNY_FINI_STR(scope, name, n)                                   \
MNY_STR_STR(scope, name, n)                                    \
MNY_CMP_STR(scope, name, n)                                    \
MNY_ADDR_TY(scope, name, n)                                    \
MNY_PAIR_TY(scope, YAML_STR_TAG, name, flags, NULL)            \


#define MNY_PAIR_SEQ(scope, name, flags, n, sz, init, fini, ...)       \
MNY_INIT_SEQ(scope, name, n, sz, init, fini)                           \
MNY_FINI_SEQ(scope, name, n)                                           \
MNY_STR_SEQ(scope, name, n)                                            \
MNY_CMP_SEQ(scope, name, n)                                            \
MNY_ADDR_TY(scope, name, n)                                            \
MNY_PAIR_TY(scope, YAML_SEQ_TAG, name, flags, __VA_ARGS__)             \



#define MNY_PAIR_TIMESTAMP(scope, name, flags)                 \
    MNY_PAIR_TY0(scope, YAML_TIMESTAMP_TAG, name, flags, NULL) \


#define MNY_PAIR_MAP(scope, name, flags, n, ...)               \
MNY_INIT_MAP(scope, name, n)                                   \
MNY_FINI_MAP(scope, name, n)                                   \
MNY_STR_MAP(scope, name, n)                                    \
MNY_CMP_MAP(scope, name, n)                                    \
MNY_ADDR_TY(scope, name, n)                                    \
MNY_PAIR_TY(scope, YAML_MAP_TAG, name, flags, __VA_ARGS__)     \


#define MNY_PAIR_MAP0(scope, name, flags, ...)                 \
    MNY_PAIR_TY0(scope, YAML_MAP_TAG, name, flags, __VA_ARGS__)\


#define MNY_PAIR_MAP_EXT(scope, name, flags, init, fini, str, cmp, addr, ...)  \
UNUSED static mny_node_info_t MNY_NAME(scope, name) = {                        \
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




#define MNY_PARSE_INTO(name, ni, ty)                                   \
static int                                                             \
name ## _doc_cb(yaml_document_t *doc,                                  \
                yaml_node_t *node,                                     \
                void *udata)                                           \
{                                                                      \
    int res;                                                           \
    struct {                                                           \
        mny_node_info_t *ninfo;                                        \
        ty *config;                                                    \
    } *params = udata;                                                 \
    /*                                                                 \
     * top-level node is mapping                                       \
     */                                                                \
    assert(strcmp(params->ninfo->tag, YAML_MAP_TAG) == 0);             \
    if ((res = mny_traverse_nodes(doc,                                 \
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
            mny_node_info_t *ninfo;                                    \
            ty *config;                                                \
        } params = { ni, co, };                                        \
                                                                       \
        res = traverse_yaml_document(&doc, name ## _doc_cb, &params);  \
        yaml_document_delete(&doc);                                    \
    } else {                                                           \
        res = MNY_PARSE_INTO_ERROR;                                    \
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




typedef int (*mny_node_traverser_t)(yaml_document_t *,
                                    yaml_node_t *,
                                    void *);


int mny_can_cast_tag(yaml_node_t *, char *);
int mny_traverse_nodes(yaml_document_t *,
                       mny_node_info_t *,
                       yaml_node_t *,
                       void *);

int mny_node_info_fini_data(mny_node_info_t *, void *);
int traverse_yaml_document(yaml_document_t *,
                       mny_node_traverser_t,
                       void *);
void mny_node_info_traverse_ctx_init(mny_node_info_traverse_ctx_t *,
                                     const char *,
                                     const char *,
                                     const char *,
                                     const char *);
void mny_node_info_traverse_ctx_fini(mny_node_info_traverse_ctx_t *);

int mny_node_info_traverse(mny_node_info_traverse_ctx_t *,
                           mny_node_info_t *,
                           void *,
                           mny_node_info_traverser_t,
                           void *);
int mny_node_info_traverse2(mny_node_info_traverse_ctx_t *,
                            mny_node_info_t *,
                            void *,
                            void *,
                            mny_node_info_traverser2_t,
                            void *);
int mny_node_info_cmp_data(mny_node_info_t *, void *, void *);


typedef struct _mny_read_params {
    mnbytestream_t bs;
    int fd;
} mny_read_params_t;

int mny_readfd(void *, unsigned char *, size_t, size_t *);
int mny_readbs(void *, unsigned char *, size_t, size_t *);

#ifdef __cplusplus
}
#endif
#endif /* MNYAML_H_DEFINED */
