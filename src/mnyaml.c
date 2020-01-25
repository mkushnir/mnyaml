#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <inttypes.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>

#include <yaml.h>

#include <mncommon/array.h>
#include <mncommon/bytes.h>
#include <mncommon/bytestream.h>
#include <mncommon/dumpm.h>
#include <mncommon/util.h>

#include <mnyaml.h>

#include "diag.h"


static int mny_check_node_subs(yaml_document_t *,
                               mny_node_info_t *,
                               yaml_node_t *,
                               yaml_node_t *,
                               void *);

static int
mny_name_cmp(const char *a, const char *b, size_t sz)
{
    size_t i;
    int diff;

    for (i = 0; i < sz; ++i) {
        char ca, cb;
        if (a[i] == '-' || a[i] == '.') {
            ca = '_';
        } else {
            ca = a[i];
        }
        if (b[i] == '-' || b[i] == '.') {
            cb = '_';
        } else {
            cb = b[i];
        }
        diff = ca - cb;
        if (diff != 0) {
            return diff;
        }
    }
    return 0;
}


static void
dump_yaml_node(yaml_document_t *doc, yaml_node_t *node)
{
    TRACE("%s %s", YAML_NODE_TYPE_STR(node->type), node->tag);

    if (node->type == YAML_SCALAR_NODE) {
        D8(node->data.scalar.value, node->data.scalar.length);

    } else if (node->type == YAML_MAPPING_NODE) {
        yaml_node_pair_t *p;

        for (p = node->data.mapping.pairs.start;
             p < node->data.mapping.pairs.top;
             ++p) {
            yaml_node_t *key, *value;

            key = yaml_document_get_node(doc, p->key);
            value = yaml_document_get_node(doc, p->value);
            dump_yaml_node(doc, key);
            dump_yaml_node(doc, value);
        }

    } else if (node->type == YAML_SEQUENCE_NODE) {
        yaml_node_item_t *it;

        for (it = node->data.sequence.items.start;
             it < node->data.sequence.items.top;
             ++it) {
            yaml_node_t *value;
            value = yaml_document_get_node(doc, *it);
            dump_yaml_node(doc, value);
        }
    }
}


static int
mny_check_node_subs(yaml_document_t *doc,
                    mny_node_info_t *ninfo,
                    yaml_node_t *key,
                    yaml_node_t *value,
                    void *data)
{
    mny_node_info_t **nsub;
    int res;

    //TRACE("checking subs in %s", ninfo->name);
    if (key->type != YAML_SCALAR_NODE ||
            strcmp((char *)key->tag, YAML_STR_TAG) != 0) {
        TRACE("key is not a string:");
        dump_yaml_node(doc, key);
        return 1;
    }

    for (nsub = ninfo->subs; *nsub != NULL; ++nsub) {
        int res;

        //TRACE("%s/%s", key->data.scalar.value, (*nsub)->name);
        // XXX map "-" to "_"
        if (mny_name_cmp((char *)key->data.scalar.value,
                        (*nsub)->name,
                        key->data.scalar.length + 1) == 0) {
            if ((res = mny_traverse_nodes(doc, *nsub, value, data)) == 0 ||
                res == MNY_CAST_TAG_NF ||
                res == MNY_CHECK_NODE_NF ||
                res == MNY_TRAVERSE_NODES_NF ||
                res == MNY_CHECK_NODE_SEQ_NF ||
                res == MNY_INIT_INVALID) {
                return res;
            }
        }
    }
    TRACEN("expected one of '%s' sub-nodes: ", ninfo->name);
    for (nsub = ninfo->subs; *nsub != NULL; ++nsub) {
        TRACEC("'%s', ", (*nsub)->name);
    }
    if (ninfo->flags & MNY_FLAG_IGNORE_UNKNOWN_SUBNODES) {
        res = 0;
        TRACEC("found '%s' -- will be ignored\n", key->data.scalar.value);
    } else {
        res = MNY_CHECK_NODE_NF;
        TRACEC("found '%s' -- stopped\n", key->data.scalar.value);
    }
    return res;
}


int
mny_can_cast_tag(yaml_node_t *node, char *tag)
{
    if (strcmp((char *)node->tag, tag) == 0) {
        return 0;
    }
    if (strcmp((char *)node->tag, YAML_DEFAULT_SCALAR_TAG) == 0) {
        const char *tags[] = {
            YAML_BOOL_TAG,
            YAML_STR_TAG,
            YAML_INT_TAG,
            YAML_FLOAT_TAG,
            YAML_TIMESTAMP_TAG
        };
        size_t i;

        for (i = 0; i < countof(tags); ++i) {
            if (strcmp(tags[i], tag) == 0) {
                return 0;
            }
        }
    }
    return MNY_CAST_TAG_NF;
}


int
mny_traverse_nodes(yaml_document_t *doc,
                   mny_node_info_t *ninfo,
                   yaml_node_t *node,
                   void *data)
{
    char *deftag;

    //TRACE("checking node:");
    //dump_yaml_node(doc, node);

    switch (node->type) {
    case YAML_SCALAR_NODE:
        deftag = YAML_DEFAULT_SCALAR_TAG;
        break;

    case YAML_SEQUENCE_NODE:
        {
            UNUSED int res;

            deftag = YAML_DEFAULT_SEQUENCE_TAG;
            if (node->tag == NULL) {
                TRACE("node tag issue:");
                dump_yaml_node(doc, node);
                return 1;
            }
            if (strcmp(deftag, ninfo->tag) != 0) {
                if (mny_can_cast_tag(node, ninfo->tag) != 0) {
                    TRACE("expected tag: %s, found %s", ninfo->tag, node->tag);
                    return MNY_TRAVERSE_NODES_NF;
                }
            }
            /*
             * XXX should not so it here
             */
#if 0
            if (ninfo->init != NULL) {
                if ((res = ninfo->init(data, node)) != 0) {
                    return res;
                }
            }
#endif
            if (strcmp(deftag, (char *)node->tag) == 0) {
                if (strcmp(ninfo->tag, (char *)node->tag) == 0) {
                    yaml_node_item_t *p;
                    mny_node_info_t *ninfo_item;
                    mnarray_t *a;

                    ninfo_item = ninfo->subs[0];
                    if (ninfo_item == NULL) {
                        TRACE("expected sequence node info for %s, got NULL",
                              ninfo->name);
                        return MNY_CHECK_NODE_SEQ_NF;
                    }

                    a = ninfo->addr(data);
                    for (p = node->data.sequence.items.start;
                         p < node->data.sequence.items.top;
                         ++p) {
                        yaml_node_t *item;
                        void *data_item;
                        int res;

                        item = yaml_document_get_node(doc, *p);
                        //TRACE(FRED("a(%d)=%p"), *p, a);
                        if ((data_item = array_incr(a)) == NULL) {
                            FAIL("array_incr");
                        }

                        //TRACE("ni=%p p=%d oi=%p", ninfo_item, *p, data_item);

                        if ((res = mny_traverse_nodes(doc,
                                                     ninfo_item,
                                                     item,
                                                     data_item)) != 0) {

                            return res;
                        }
                    }
                }
            }
        }
        return 0;

    case YAML_MAPPING_NODE:
        {
            deftag = YAML_DEFAULT_MAPPING_TAG;
            if (node->tag == NULL) {
                TRACE("node tag issue:");
                dump_yaml_node(doc, node);
                return 1;
            }
            if (strcmp(deftag, (char *)node->tag) == 0) {
                if (strcmp(ninfo->tag, (char *)node->tag) == 0) {
                    yaml_node_pair_t *p;

                    for (p = node->data.mapping.pairs.start;
                         p < node->data.mapping.pairs.top;
                         ++p) {
                        yaml_node_t *key, *value;
                        int res;

                        key = yaml_document_get_node(doc, p->key);
                        value = yaml_document_get_node(doc, p->value);

                        if ((res = mny_check_node_subs(doc,
                                                      ninfo,
                                                      key,
                                                      value,
                                                      data)) != 0) {

                            return res;
                        }
                    }
                }
            }
        }
        if (ninfo->init != NULL) {
            int res;
            if ((res = ninfo->init(data, node)) != 0) {
                TRACE("Error in '%s' initialization", ninfo->name);
            }
            return res;
        }
        return 0;

    default:
        deftag = NULL;
    }

    if (deftag == NULL || node->tag == NULL) {
        TRACE("null node tag issue:");
        dump_yaml_node(doc, node);
        return 1;
    }

    if (strcmp(deftag, ninfo->tag) != 0) {
        if (mny_can_cast_tag(node, ninfo->tag) != 0) {
            TRACE("node %s expected tag: %s, found %s", ninfo->name, ninfo->tag, node->tag);
            return MNY_TRAVERSE_NODES_NF;
        }
    }
    //TRACE("deftag=%s ninfo->tag=%s node->tag=%s", deftag, ninfo->tag, node->tag);
    if (ninfo->init != NULL) {
        int res;
        if ((res = ninfo->init(data, node)) != 0) {
            TRACE("Error in '%s' initialization", ninfo->name);
        }
        return res;
    }
    return 0;
}


int
mny_node_info_fini_data(mny_node_info_t *ninfo, void *data)
{
    int res;
    mny_node_info_t **nsub;

    //TRACE("ninfo=%p fini(%p) for %s data=%p", ninfo, ninfo->fini, ninfo->name, data);
    if (strcmp(ninfo->tag, YAML_SEQ_TAG) != 0) {
        for (nsub = ninfo->subs; *nsub != NULL; ++nsub) {
            if ((res = mny_node_info_fini_data(*nsub, data)) != 0) {
                return res;
            }
        }
    } else {
        /* special case */
    }
    res = 0;
    if (ninfo->fini != NULL) {
        res = ninfo->fini(data);
    } else {
        //TRACE("NULL fini for %s", ninfo->name);
    }
    return res;
}


void
mny_node_info_traverse_ctx_init(mny_node_info_traverse_ctx_t *tctx,
                                const char *nsep,
                                const char *sub0,
                                const char *sub1,
                                const char *prefix)
{
    assert(prefix != NULL);
    tctx->nsep = nsep;
    tctx->sub0 = sub0;
    tctx->sub1 = sub1;
    tctx->prefix = bytes_new_from_str(prefix);
}


void
mny_node_info_traverse_ctx_fini(mny_node_info_traverse_ctx_t *tctx)
{
    BYTES_DECREF(&tctx->prefix);
}


int
mny_node_info_traverse(mny_node_info_traverse_ctx_t *tctx,
                       mny_node_info_t *ninfo,
                       void *data,
                       mny_node_info_traverser_t cb,
                       void *udata)
{
    int res;
    mny_node_info_t **ni;
    mnbytes_t *tmp;
    mnbytes_t *saved_prefix;

    assert(tctx->prefix != NULL);
    saved_prefix = tctx->prefix;

    res = 0;
    if (strcmp(ninfo->tag, YAML_SEQ_TAG) == 0) {
        mnarray_t *a;
        void *pdata;
        mnarray_iter_t it;


        ni = &ninfo->subs[0];
        a = ninfo->addr(data);
        for (pdata = array_first(a, &it);
             pdata != NULL;
             pdata = array_next(a, &it)) {


            if (ninfo->name == NULL) {
                tmp = bytes_new_from_bytes(tctx->prefix);
            } else {
                if (tctx->prefix == NULL ||
                    tctx->prefix->data[0] == '\0') {
                    tmp = bytes_printf("%s%s%d%s",
                                       ninfo->name,
                                       tctx->sub0,
                                       it.iter,
                                       tctx->sub1);
                } else {
                    tmp = bytes_printf("%s%s%s%s%d%s",
                                       tctx->prefix->data,
                                       tctx->nsep,
                                       ninfo->name,
                                       tctx->sub0,
                                       it.iter,
                                       tctx->sub1);
                }
            }
            tctx->prefix = tmp;

            if ((res = mny_node_info_traverse(tctx,
                                             *ni,
                                             pdata,
                                             cb,
                                             udata)) != 0) {
                tctx->prefix = saved_prefix;
                BYTES_DECREF(&tmp);
                break;
            }

            tctx->prefix = saved_prefix;
            BYTES_DECREF(&tmp);
        }

    } else {
        if (ninfo->name == NULL) {
            tmp = bytes_new_from_bytes(tctx->prefix);
        } else {
            if (tctx->prefix == NULL ||
                tctx->prefix->data[0] == '\0') {
                tmp = bytes_printf("%s",
                                   ninfo->name);
            } else {
                tmp = bytes_printf("%s%s%s",
                                   tctx->prefix->data,
                                   tctx->nsep,
                                   ninfo->name);
            }
        }
        tctx->prefix = tmp;

        for (ni = ninfo->subs; *ni != NULL; ++ni) {
            if ((res = mny_node_info_traverse(tctx,
                                             *ni,
                                             data,
                                             cb,
                                             udata)) != 0) {
                break;
            }
        }

        tctx->prefix = saved_prefix;
        BYTES_DECREF(&tmp);
    }

    if (res == 0) {
        if (ninfo->name == NULL) {
            tmp = bytes_new_from_bytes(tctx->prefix);
        } else {
            if (tctx->prefix == NULL ||
                tctx->prefix->data[0] == '\0') {
                tmp = bytes_printf("%s",
                                   ninfo->name);
            } else {
                tmp = bytes_printf("%s%s%s",
                                   tctx->prefix->data,
                                   tctx->nsep,
                                   ninfo->name);
            }
        }
        tctx->prefix = tmp;
        res = cb(tctx, ninfo, data, udata);
        tctx->prefix = saved_prefix;
        BYTES_DECREF(&tmp);
    }

    return res;
}


int
mny_node_info_traverse2(mny_node_info_traverse_ctx_t *tctx,
                        mny_node_info_t *ninfo,
                        void *a,
                        void *b,
                        mny_node_info_traverser2_t cb,
                        void *udata) {
    int res;
    mny_node_info_t **ni;

    res = 0;
    if (strcmp(ninfo->tag, YAML_SEQ_TAG) == 0) {
        mnarray_t *ara, *arb;
        void *pa, *pb;
        mnarray_iter_t it;

        ni = &ninfo->subs[0];
        ara = ninfo->addr(a);
        arb = ninfo->addr(b);

        for (pa = array_first(ara, &it);
             pa != NULL;
             pa = array_next(ara, &it)) {
            pb = array_get(arb, it.iter);
            if (pb == NULL) {
                break;
            }
            if ((res = mny_node_info_traverse2(tctx,
                                              *ni,
                                              pa,
                                              pb,
                                              cb,
                                              udata)) != 0) {
                break;
            }
        }
    } else {
        for (ni = ninfo->subs; *ni != NULL; ++ni) {
            if ((res = mny_node_info_traverse2(tctx,
                                              *ni,
                                              a,
                                              b,
                                              cb,
                                              udata)) != 0) {
                break;
            }
        }
    }

    if (res == 0) {
        res = cb(tctx, ninfo, a, b, udata);
    }

    return res;
}

static int
cmp_cb(UNUSED mny_node_info_traverse_ctx_t *tctx,
       mny_node_info_t *ninfo,
       void *a,
       void *b,
       UNUSED void *udata)
{
    if (ninfo->cmp != NULL) {
#ifdef MNY_CMP_DEBUG
        int res;
        res = ninfo->cmp(a, b);
        if (res != 0) {
            TRACE("diff at %s", ninfo->name);
        }
        return res;
#else
        return ninfo->cmp(a, b);
#endif
    }
    return 0;
}


int
mny_node_info_cmp_data(mny_node_info_t *ninfo, void *a, void *b)
{
    mny_node_info_traverse_ctx_t tctx;
    int res;

    mny_node_info_traverse_ctx_init(&tctx, ".", "[", "]", "");
    res = mny_node_info_traverse2(&tctx, ninfo, a, b, cmp_cb, NULL);
    mny_node_info_traverse_ctx_fini(&tctx);
    return res;
}




int
traverse_yaml_document(yaml_document_t *doc,
                       mny_node_traverser_t cb,
                       void *udata)
{
    yaml_node_t *node;

    if ((node = yaml_document_get_root_node(doc)) != NULL) {
        int res;

        if ((res = cb(doc, node, udata)) != 0) {
            return res;
        }
    }
    return 0;
}


int
mny_readfd(void *udata, unsigned char *buf, size_t sz, size_t *nread)
{
    mny_read_params_t *params = udata;
    ssize_t n;
    int res;

    n = read(params->fd, buf, sz);
    if (n >= 0) {
        res = 1;
        *nread = n;
    } else {
        res = 0;
        *nread = 0;
    }
    return res;
}


int
mny_readbs(void *udata, unsigned char *buf, size_t sz, size_t *nread)
{
    mny_read_params_t *params = udata;
    ssize_t n;

    n = params->bs.read_more(&params->bs, (void *)(intptr_t)params->fd, sz);
    if (n > 0) {
        memcpy(buf, SPDATA(&params->bs), n);
        SADVANCEPOS(&params->bs, n);
        *nread = n;
    } else {
        *nread = 0;
    }
    return 1;
}


