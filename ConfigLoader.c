#include "ConfigLoader.h"
#include "HashMap.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>



HashMap_t *ReadConfigFile(const char *file)
{
    #define MAX_LINE_LEN (256)
    HashMap_t *m;
    map_init(m);
    FILE *fd;
    int line_cnt = 0, num_pars = 0;
    char line[MAX_LINE_LEN];

    fd = fopen(file, "r");
    if(!fd)
        return NULL;

    while(fgets(line, MAX_LINE_LEN, fd) != NULL)
    {
        strtrim(line);
        ++line_cnt;
        size_t len = strlen(line);

        char key[KEY_LEN], value[VAL_LEN];

        if((len <= 0) || (line[0] == '#') || (line[0] == ';'))
            continue;

        /*
         * looks for equal sign in line and assigns
         * the string in front of it to 'key' and
         * everything following the equal sign to 'value'.
         * Spaces in search string account for different
         * whitespace configurations (k=v, k =v, k= v, k = v)
         */
        num_pars = sscanf(line, " %[^= ] = %s", key, value);
        if(num_pars != 2)
            /*
             * failed to read key value pair with equal sign
             * try with whitepace seperation
             */
            num_pars = sscanf(line, " %s %s", key, value);
        if(num_pars != 2) {
            printf("... Unknown key, value pair format in file <%s> on line <%d>. "
                "Aborting...", file, line_cnt);
            fclose(fd);
            // return par;
            exit(123);
        }
        map_set(m, key, value);
    }
    fclose(fd);
    return m;
}

void InitConfig(Config_t *c, const char *configFile)
{
    HashMap_t *m = ReadConfigFile(configFile);

    #define SETTING(STR, DEFAULT)       \
        _Generic(typeof(DEFAULT),       \
            int:       _ValToInt,       \
            float:     _ValToFloat,     \
            long long: _ValToLongLong,  \
            double:    _ValToDouble,    \
            char[VAL_LEN]: _ValToString \
        ) (m, (#STR), &(c->STR));

    #include "Settings.def"
    #undef SETTING

    map_free(m);
    return;
}


static int _ValToInt(HashMap_t *m, const char *key, int *result)
{
    Entry_t *e = map_get(m, key);
    char *endptr;

    if(!e) { return 1; }

    if(e->value[0] == '\0') { return 1; }

    *result = strtol(e->value, &endptr, 10);

    if((*endptr) != 0)
        return 1; /* did not parse entire string */

    return 0;
}

static int _ValToLongLong(HashMap_t *m, const char *key, long long *result)
{
    Entry_t *e = map_get(m, key);
    char *endptr;

    if(!e) { return 1; }

    if(e->value[0] == '\0') { return 1; }

    *result = strtoll(e->value, &endptr, 10);

    if((*endptr) != 0)
        return 1; /* did not parse entire string */

    return 0;
}

static int _ValToFloat(HashMap_t *m, const char *key, float *result)
{
    Entry_t *e = map_get(m, key);
    char *endptr;

    if(!e) { return 1; }

    if(e->value[0] == '\0') { return 1; }

    *result = strtof(e->value, &endptr);

    if((*endptr) != 0)
        return 1; /* did not parse entire string */

    return 0;
}

static int _ValToDouble(HashMap_t *m, const char *key, double *result)
{
    Entry_t *e = map_get(m, key);
    char *endptr;

    if(!e) { return 1; }

    if(e->value[0] == '\0') { return 1; }

    *result = strtod(e->value, &endptr);

    if((*endptr) != 0)
        return 1; /* did not parse entire string */

    return 0;
}

static int _ValToString(HashMap_t *m, const char *key, char *result)
{
    Entry_t *e = map_get(m, key);
    char *endptr;

    if(!e) { return 1; }

    // if(e->value[0] == '\0') { return 1; }

    strncpy(result, e->value, VAL_LEN);

    return 0;
}


int main(int argc, char **argv)
{
    Config_t settings;
    InitConfig(&settings, argv[1]);
    return 0;
}