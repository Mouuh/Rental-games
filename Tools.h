#ifndef TOOLS_H
#define TOOLS_H
#include <stdio.h>
#include <string.h>
#include "struct.h"
#include <stdlib.h>

// File operations
TOF *open_file(char *chemin, char mode);
void write_entete(TOF *f);
void read_entete(TOF *f);
void close_file(TOF *f);

#endif