#include <assert.h>
#include <fcntl.h>
#include <inttypes.h>
#include <unistd.h>


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

YM_PAIR_MAP0(, si,
    &YM_NAME(si_, abc),
    &YM_NAME(si_, bcd)
);


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

YM_PAIR_SEQ(vbn_, sources, engine.input.vbn.sources,
            sizeof(si_t),
            si_init,
            si_fini,
    &YM_NAME(, si)
);
YM_PAIR_INT(vbn_, bnm, engine.input.vbn.bnm);

YM_PAIR_MAP(input_, vbn, engine.input.vbn,
    &YM_NAME(vbn_, bnm),
    &YM_NAME(vbn_, sources)
);
YM_PAIR_STR(input_, qwe, engine.input.qwe);
YM_PAIR_INT(input_, asd, engine.input.asd);

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

YM_PAIR_INT0(, zxci, int);
YM_PAIR_SEQ(input_, zxc, engine.input.zxc,
            sizeof(int),
            zxc_item_init,
            zxc_item_fini,
            &YM_NAME(, zxci)
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


static void
test1(int argc, char **argv)
{
    int fd;
    if (argc > 1) {
        UNUSED yaml_token_t tok;
        UNUSED yaml_event_t ev;

        if ((fd = open(argv[1], O_RDONLY)) == -1) {
            FAIL("open");
        }

        myconfig_parse_into(fd);
        close(fd);

        ym_node_info_fini_data(&YM_NAME(, config), &the_config);
    }
}

int
main(int argc, char **argv)
{
    test1(argc, argv);
    return 0;
}
