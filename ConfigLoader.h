#pragma once

typedef struct {
    #define SETTING(STR, DEFAULT) typeof(DEFAULT) STR;
    #include "Settings.def"
    #undef SETTING
} Config_t;

void InitConfig(Config_t* c, const char* configfile);
void PrintConfig(Config_t *c);