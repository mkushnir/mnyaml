#include <assert.h>
#include <fcntl.h>
#include <inttypes.h>
#include <unistd.h>


#include <mrkcommon/bytestream.h>
#include <mrkcommon/dumpm.h>
#include <mrkcommon/util.h>

#include <mrkyaml.h>

#include "diag.h"
#include "unittest.h"

#ifndef NDEBUG
const char *_malloc_options = "AJ";
#endif



#define YM_CONFIG_TYPE si_t

typedef struct {
    int abc;
    int bcd;
} si_t;

YM_PAIR_INT(si_, abc, abc);
YM_PAIR_INT(si_, bcd, bcd);

YM_PAIR_MAP0(, _si,
    &YM_NAME(si_, abc),
    &YM_NAME(si_, bcd)
);


static int
si_init(si_t *si)
{
    //TRACE("si=%p", si);
    si->abc = -1;
    si->bcd = -2;
    return 0;
}


static int
si_fini(UNUSED si_t *si)
{
    //TRACE("si=%p abc=%d bcd=%d", si, si->abc, si->bcd);
    return 0;
}


#undef YM_CONFIG_TYPE
#define YM_CONFIG_TYPE myconfig_t

typedef struct {
    struct {
        struct {
            bytes_t *qwe;
            intmax_t asd;
            //char *b;
            array_t zxc;
            struct {
                char bnm;
                array_t sources;
            } vbn;
        } input;
        struct {
            char aaa;
            double c;
        } output;
    } engine;
} myconfig_t;


static int
zxc_item_init(int *i)
{
    *i = 0;
    //TRACE();
    return 0;
}

static int
zxc_item_fini(int *i)
{
    //TRACE("v=%d", *i);
    *i = 0;
    return 0;
}

static void
myconfig_init(myconfig_t *config)
{
    array_init(&config->engine.input.vbn.sources,
               sizeof(si_t),
               0,
               (array_initializer_t)si_init,
               (array_finalizer_t)si_fini);
    array_init(&config->engine.input.zxc,
               sizeof(int),
               0,
               (array_initializer_t)zxc_item_init,
               (array_finalizer_t)zxc_item_fini);
}


static void
myconfig_fini(myconfig_t *config)
{
    array_fini(&config->engine.input.vbn.sources);
    array_fini(&config->engine.input.zxc);
}



YM_PAIR_SEQ(vbn_, sources, engine.input.vbn.sources,
            sizeof(si_t),
            si_init,
            si_fini,
    &YM_NAME(, _si)
);
YM_PAIR_INT(vbn_, bnm, engine.input.vbn.bnm);

YM_PAIR_MAP(input_, vbn, engine.input.vbn,
    &YM_NAME(vbn_, bnm),
    &YM_NAME(vbn_, sources)
);
YM_PAIR_STR(input_, qwe, engine.input.qwe);
YM_PAIR_INT(input_, asd, engine.input.asd);

YM_PAIR_INT0(, _zxc, int);
YM_PAIR_SEQ(input_, zxc, engine.input.zxc,
            sizeof(int),
            zxc_item_init,
            zxc_item_fini,
            &YM_NAME(, _zxc)
);

YM_PAIR_MAP(engine_, input, engine.input,
    &YM_NAME(input_, qwe),
    &YM_NAME(input_, asd),
    &YM_NAME(input_, zxc),
    &YM_NAME(input_, vbn)
);

YM_PAIR_BOOL(output_, aaa, engine.output.aaa);
YM_PAIR_FLOAT(output_, c, engine.output.c);
YM_PAIR_MAP(engine_, output, engine.output,
    &YM_NAME(output_, aaa),
    &YM_NAME(output_, c),
    NULL);

YM_PAIR_MAP(config_, engine, engine,
    &YM_NAME(engine_, input),
    &YM_NAME(engine_, output)
);

YM_PAIR_MAP0(, config,
    &YM_NAME(config_, engine)
);


myconfig_t the_config;


static int
mycb(void *udata, unsigned char *buf, size_t sz, size_t *nread)
{
    ssize_t n;
    int res;

    struct {
        int fd;
        bytestream_t bs;
    } *params = udata;

    //TRACE("need %ld to read", sz);
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


YM_PARSE_INTO(myconfig, mycb, &YM_NAME(, config), &the_config)


static bytes_t _all = BYTES_INITIALIZER("all");

static int
mycb1(ym_node_info_traverse_ctx_t *tctx,
      ym_node_info_t *ninfo,
      void *data,
      void *udata)
{
        struct {
            bytestream_t bs;
            array_t prefixes;
        } *params;

    if (ninfo->str != NULL) {
        params = udata;
        UNUSED ssize_t res;
        bytes_t **s;
        array_iter_t it;
        //TRACE("%s=%ld", tctx->prefix->data, ninfo->str(&params->bs, data));
        for (s = array_first(&params->prefixes, &it);
             s != NULL;
             s = array_next(&params->prefixes, &it)) {
            if (bytes_startswith(tctx->prefix, *s) || bytes_cmp(&_all, *s) == 0) {
                bytestream_nprintf(&params->bs, 8 + tctx->prefix->sz, "%s=", tctx->prefix->data);
                res = ninfo->str(&params->bs, data);
                bytestream_cat(&params->bs, 2, ";\n");
                break;
            }
        }
    } else {
        //TRACE("%s=...", tctx->prefix->data);
    }
    return 0;
}


static int
prefixes_fini_item(bytes_t **s)
{
    BYTES_DECREF(s);
    return 0;
}


static void
test1(int argc, char **argv)
{
    int fd;
    if (argc > 1) {
        ym_node_info_traverse_ctx_t tctx;
        struct {
            bytestream_t bs;
            array_t prefixes;
        } params;
        int i;

        if ((fd = open(argv[1], O_RDONLY)) == -1) {
            FAIL("open");
        }

        myconfig_init(&the_config);
        myconfig_parse_into(fd);
        close(fd);

        //ym_node_info_traverse_ctx_init(&tctx, ".", "[", "]", "");
        ym_node_info_traverse_ctx_init(&tctx, "_", "_", "_", "");
        bytestream_init(&params.bs, 1024);
        *SDATA(&params.bs, 0) = '\0';
        array_init(&params.prefixes,
                   sizeof(bytes_t *),
                   0,
                   NULL,
                   (array_finalizer_t)prefixes_fini_item);
        for (i = 2; i < argc; ++i) {
            bytes_t **s;
            if ((s = array_incr(&params.prefixes)) == NULL) {
                FAIL("array_incr");
            }
            *s = bytes_new_from_str(argv[i]);
        }
        (void)ym_node_info_traverse(&tctx,
                                    //&YM_NAME(, config),
                                    &YM_NAME(config_, engine),
                                    &the_config,
                                    mycb1,
                                    &params);
        TRACEC("%s", SDATA(&params.bs, 0));
        array_fini(&params.prefixes);
        bytestream_fini(&params.bs);
        ym_node_info_traverse_ctx_fini(&tctx);

        ym_node_info_fini_data(&YM_NAME(, config), &the_config);
        myconfig_fini(&the_config);
    }
}

int
main(int argc, char **argv)
{
    test1(argc, argv);
    return 0;
}
