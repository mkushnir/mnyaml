#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <inttypes.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/types.h>

#include <yaml.h>

#include <mrkcommon/array.h>
#include <mrkcommon/bytes.h>
#include <mrkcommon/bytestream.h>
#include <mrkcommon/dumpm.h>
#include <mrkcommon/util.h>

#include <mrkyaml.h>

#include "diag.h"


#ifndef NDEBUG
const char *_malloc_options = "AJ";
#endif


static int ym_check_node_subs(yaml_document_t *,
                              ym_node_info_t *,
                              yaml_node_t *,
                              yaml_node_t *,
                              void *out);

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
ym_check_node_subs(yaml_document_t *doc,
                   ym_node_info_t *ninfo,
                   yaml_node_t *key,
                   yaml_node_t *value,
                   void *out)
{
    ym_node_info_t **nsub;

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
        if (strncmp((char *)key->data.scalar.value,
                    (*nsub)->name,
                    key->data.scalar.length + 1) == 0) {
            if ((res = ym_traverse_nodes(doc, *nsub, value, out)) == 0 ||
                res == YM_CHECK_NODE_NF ||
                res == YM_TRAVERSE_NODES_NF ||
                res == YM_CHECK_NODE_SEQ_NF ||
                res == YM_INIT_INVALID) {
                return res;
            }
        }
    }
    TRACEN("expect one of:");
    for (nsub = ninfo->subs; *nsub != NULL; ++nsub) {
        TRACEC(" '%s'", (*nsub)->name);
    }
    TRACEC(", found '%s'\n", key->data.scalar.value);
    return YM_CHECK_NODE_NF;
}


int
ym_can_cast_tag(yaml_node_t *node, char *tag)
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
    return 1;
}


int
ym_traverse_nodes(yaml_document_t *doc,
                   ym_node_info_t *ninfo,
                   yaml_node_t *node,
                   void *out)
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
            int res;

            deftag = YAML_DEFAULT_SEQUENCE_TAG;
            if (node->tag == NULL) {
                TRACE("node tag issue:");
                dump_yaml_node(doc, node);
                return 1;
            }
            if (strcmp(deftag, ninfo->tag) != 0) {
                if (ym_can_cast_tag(node, ninfo->tag) != 0) {
                    TRACE("expect tag: %s, found %s", ninfo->tag, node->tag);
                    return YM_TRAVERSE_NODES_NF;
                }
            }
            if (ninfo->init != NULL) {
                if ((res = ninfo->init(out, node)) != 0) {
                    return res;
                }
            }
            if (strcmp(deftag, (char *)node->tag) == 0) {
                if (strcmp(ninfo->tag, (char *)node->tag) == 0) {
                    yaml_node_item_t *p;
                    ym_node_info_t *ninfo_item;

                    ninfo_item = ninfo->subs[0];
                    if (ninfo_item == NULL) {
                        TRACE("expect sequence node info for %s, got NULL",
                              ninfo->name);
                        return YM_CHECK_NODE_SEQ_NF;
                    }

                    for (p = node->data.sequence.items.start;
                         p < node->data.sequence.items.top;
                         ++p) {
                        yaml_node_t *item;
                        array_t *a;
                        void *out_item;
                        int res;

                        item = yaml_document_get_node(doc, *p);
                        a = ninfo->addr(out);
                        //TRACE(FRED("a(%d)=%p"), *p, a);
                        if ((out_item = array_incr(a)) == NULL) {
                            FAIL("array_incr");
                        }

                        //TRACE("ni=%p p=%d oi=%p", ninfo_item, *p, out_item);

                        if ((res = ym_traverse_nodes(doc,
                                                      ninfo_item,
                                                      item,
                                                      out_item)) != 0) {

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

                        if ((res = ym_check_node_subs(doc,
                                                      ninfo,
                                                      key,
                                                      value,
                                                      out)) != 0) {

                            return res;
                        }
                    }
                }
            }
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
        if (ym_can_cast_tag(node, ninfo->tag) != 0) {
            TRACE("expect tag: %s, found %s", ninfo->tag, node->tag);
            return YM_TRAVERSE_NODES_NF;
        }
    }
    //TRACE("deftag=%s ninfo->tag=%s node->tag=%s", deftag, ninfo->tag, node->tag);
    if (ninfo->init != NULL) {
        return ninfo->init(out, node);
    }
    return 0;
}


int
ym_node_info_fini_data(ym_node_info_t *ninfo, void *out)
{
    int res;
    ym_node_info_t **nsub;

    //TRACE("ninfo=%p fini(%p) for %s out=%p", ninfo, ninfo->fini, ninfo->name, out);
    if (strcmp(ninfo->tag, YAML_SEQ_TAG) != 0) {
        for (nsub = ninfo->subs; *nsub != NULL; ++nsub) {
            if ((res = ym_node_info_fini_data(*nsub, out)) != 0) {
                return res;
            }
        }
    } else {
        /* special case */
    }
    res = 0;
    if (ninfo->fini != NULL) {
        res = ninfo->fini(out, NULL);
    } else {
        //TRACE("NULL fini for %s", ninfo->name);
    }
    return res;
}


int
traverse_yaml_document(yaml_document_t *doc,
                       ym_node_traverser_t cb,
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


