#include "Tools.h"

TOF *open_file(char* path, char mode) {
    TOF* tof = malloc(sizeof(TOF));
    if (!tof) {
        return NULL;
    }
    
    strcpy(tof->fichier, path);
    tof->f = (mode == 'N') ? fopen(path, "wb+") : fopen(path, "rb+");

    if (!tof->f) {
        free(tof);
        return NULL;
    }

    if (mode == 'N') {
        tof->ent.nb_bloc = 0;
        write_entete(tof);  
    } else {
        read_entete(tof); 
    }

    return tof;
}

void read_entete(TOF* tof) {
    if (!tof || !tof->f) return;
    long currentPos = ftell(tof->f);
    fseek(tof->f, 0, SEEK_SET);
    fread(&(tof->ent), sizeof(Entete), 1, tof->f);
    fseek(tof->f, currentPos, SEEK_SET);
}

void write_entete(TOF* tof) {
    if (!tof || !tof->f) return;
    long currentPos = ftell(tof->f);
    fseek(tof->f, 0, SEEK_SET);
    fwrite(&(tof->ent), sizeof(Entete), 1, tof->f);
    fseek(tof->f, currentPos, SEEK_SET);
}

void close_file(TOF *f) {
    if (f) {
        if (f->f) {
            fclose(f->f);
            f->f = NULL;
        }
        free(f);
    }
}
