#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ConfigLoader.h"
#include "HashMap.h"

HashMap_t *ReadConfigFile(const char *file)
{
#define MAX_LINE_LEN (256)
    HashMap_t *m = {0};
    map_init(&m, 0);

    FILE *fd;
    int line_cnt = 0, num_pars = 0;
    char line[MAX_LINE_LEN];

    fd = fopen(file, "r");
    if (!fd)
    {
        return NULL;
    }

    while (fgets(line, MAX_LINE_LEN, fd) != NULL)
    {
        // strtrim(line);
        ++line_cnt;
        size_t len = strlen(line);

        char key[KEY_LEN], value[VAL_LEN];

        if ((len <= 6) || (line[0] == '#') || (line[0] == ';'))
            continue;

        /*
         * looks for equal sign in line and assigns
         * the string in front of it to 'key' and
         * everything following the equal sign to 'value'.
         * Spaces in search string account for different
         * whitespace configurations (k=v, k =v, k= v, k = v)
         */
        num_pars = sscanf(line, " %[^= ] = %s", key, value);
        if (num_pars != 2)
            /*
             * failed to read key value pair with equal sign
             * try with whitepace seperation
             */
            num_pars = sscanf(line, " %s %s", key, value);
        if (num_pars != 2)
        {
            printf("... Unknown key, value pair format in file <%s> on line <%d>\n", file, line_cnt);
            // fclose(fd);
            // exit(123);
        }
        map_set(&m, key, value);
    }
    fclose(fd);
    return m;
}

static int _ValToInt(HashMap_t *m, const char *key, int *result)
{
    printf("Get key <%s> ...", key);
    fflush(stdout);
    Entry_t *e = map_get(m, key);
    char *endptr;

    if (!e)
    {
        printf("Failed\n");
        fflush(stdout);
        return 0;
    }

    if (e->value[0] == '\0')
    {
        return 1;
    }

    *result = strtol(e->value, &endptr, 10);

    if ((*endptr) != 0)
        return 0; /* did not parse entire string */

    printf("Success\n");
    fflush(stdout);
    return 1;
}

static int _ValToUnsignedInt(HashMap_t *m, const char *key, unsigned int *result)
{
    printf("Get key <%s> ...", key);
    fflush(stdout);
    Entry_t *e = map_get(m, key);
    char *endptr;

    if (!e)
    {
        printf("Failed\n");
        fflush(stdout);
        return 0;
    }

    if (e->value[0] == '\0')
    {
        return 1;
    }

    *result = strtoul(e->value, &endptr, 10);

    if ((*endptr) != 0)
        return 0; /* did not parse entire string */

    printf("Success\n");
    fflush(stdout);
    return 1;
}

static int _ValToLongLong(HashMap_t *m, const char *key, long long *result)
{
    printf("Get key <%s> ...", key);
    fflush(stdout);
    Entry_t *e = map_get(m, key);
    char *endptr;

    if (!e)
    {
        printf("Failed\n");
        fflush(stdout);
        return 0;
    }

    if (e->value[0] == '\0')
    {
        return 1;
    }

    *result = strtoll(e->value, &endptr, 10);

    if ((*endptr) != 0)
        return 0; /* did not parse entire string */

    printf("Success\n");
    fflush(stdout);
    return 1;
}

static int _ValToUnsignedLongLong(HashMap_t *m, const char *key, unsigned long long *result)
{
    printf("Get key <%s> ...", key);
    fflush(stdout);
    Entry_t *e = map_get(m, key);
    char *endptr;

    if (!e)
    {
        printf("Failed\n");
        fflush(stdout);
        return 0;
    }

    if (e->value[0] == '\0')
    {
        return 1;
    }

    *result = strtoull(e->value, &endptr, 10);

    if ((*endptr) != 0)
        return 0; /* did not parse entire string */

    printf("Success\n");
    fflush(stdout);
    return 1;
}

static int _ValToFloat(HashMap_t *m, const char *key, float *result)
{
    printf("Get key <%s> ...", key);
    fflush(stdout);
    Entry_t *e = map_get(m, key);
    char *endptr;

    if (!e)
    {
        printf("Failed\n");
        fflush(stdout);
        return 0;
    }

    if (e->value[0] == '\0')
    {
        return 1;
    }

    *result = strtof(e->value, &endptr);

    if ((*endptr) != 0)
        return 0; /* did not parse entire string */

    printf("Success\n");
    fflush(stdout);
    return 1;
}

static int _ValToDouble(HashMap_t *m, const char *key, double *result)
{
    printf("Get key <%s> ...", key);
    fflush(stdout);
    Entry_t *e = map_get(m, key);
    char *endptr;

    if (!e)
    {
        printf("Failed\n");
        fflush(stdout);
        return 0;
    }

    if (e->value[0] == '\0')
    {
        return 1;
    }

    *result = strtod(e->value, &endptr);

    if ((*endptr) != 0)
        return 0; /* did not parse entire string */

    printf("Success\n");
    fflush(stdout);
    return 1;
}

static int _ValToString(HashMap_t *m, const char *key, char **result)
{
    printf("Get key <%s> ...", key);
    fflush(stdout);
    Entry_t *e = map_get(m, key);

    if (!e)
    {
        printf("Failed\n");
        fflush(stdout);
        return 0;
    }

    // if(e->value[0] == '\0') { return 1; }
    *result = realloc(*result, VAL_LEN);
    if (!(*result))
    {
        printf("Failed\n");
        fflush(stdout);
        return 0;
    }

    strncpy(*result, e->value, VAL_LEN);
    printf("Success\n");
    fflush(stdout);

    return 1;
}

void PrintConfig(Config_t *c)
{
    if (!c)
    {
        return;
    }

#define SETTING(STR, DEF)                       \
    printf(                                     \
        _Generic((DEF),                         \
            int: "'%*s' %d\n",                  \
            unsigned int: "'%*s' %u\n",         \
            long long: "'%*s' %lld\n",          \
            unsigned long long: "'%*s' %llu\n", \
            float: "'%*s' %f\n",                \
            double: "'%*s' %f\n",               \
            default: "'%*s' %s\n"),             \
        KEY_LEN, #STR, c->STR);

#include "Settings.def"
#undef SETTING
    fflush(stdout);
}

void InitConfig(Config_t *c, const char *configFile)
{
    // Intermediate structure to store key/vlaue pairs
    HashMap_t *m = ReadConfigFile(configFile);
    map_print(m);

// Conversion of char* value to expected type based
// on type of the default value in 'Settings.def'
// Only searches for the values defined in 'Settings.def'
#define SETTING(STR, DEFAULT)                           \
    if (!_Generic((DEFAULT),                            \
            int: _ValToInt,                             \
            unsigned int: _ValToUnsignedInt,            \
            float: _ValToFloat,                         \
            long long: _ValToLongLong,                  \
            unsigned long long: _ValToUnsignedLongLong, \
            double: _ValToDouble,                       \
            default: _ValToString)(m, #STR, &(c->STR))) \
        (c->STR) = DEFAULT;

#include "Settings.def"
#undef SETTING

    map_free(m);
    return;
}

int main(int argc, char **argv)
{
    if (argv[1] == NULL)
        return 0;

    Config_t settings;
    printf("argv[1] = %s\n", argv[1]);
    InitConfig(&settings, argv[1]);
    PrintConfig(&settings);

    return 0;
}