#include <stdlib.h>

typedef struct {
    #define SETTING(STR, DEFAULT) typeof(DEFAULT) STR;
    #include "Settings.def"
    #undef SETTING
} Config_t;

// void InitConfig(Config_t* c);

// void ConfigReader(Config_t* c, const char *file, void (*func)(const char*));