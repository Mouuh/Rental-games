#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "struct.h"
#include "Game.h"
#include "Tools.h"

Game defaultGame = {0, "\0", 0.0f}; // Default game object

Blocgame createGameBloc(Game g) {
    Blocgame temp;
    temp.tab_enreg[0] = g;
    temp.nb = 1;
    return temp;
}

void read_bloc_game(TOF f, Blocgame *bg, int bIndx) {
    FILE *fp = fopen(f.fichier, "rb");
    if (fp == NULL) {
        perror("Failed to open file for reading");
        return;
    }
    fseek(fp, sizeof(Entete) + ((bIndx - 1) * sizeof(Blocgame)), SEEK_SET);
    fread(bg, sizeof(Blocgame), 1, fp);
    fclose(fp);
}

void write_bloc_game(TOF *f, Blocgame bg, int bIndx) {
    FILE *fp = fopen(f->fichier, "rb+");
    if (fp == NULL) {
        perror("Failed to open file for writing");
        return;
    }
    fseek(fp, (sizeof(Entete) + ((bIndx - 1) * sizeof(Blocgame))), SEEK_SET);
    fwrite(&bg, sizeof(Blocgame), 1, fp);
    fclose(fp);
}

Game createGame(int gID, char title[], float rentalPrice, TOF *f) {
    Game g;
    g.gameID = gID;
    strcpy(g.title, title);
    g.rentalPrice = rentalPrice;

    // File operations
    TOF tmpH;
    strcpy(tmpH.fichier, f->fichier);
    
    // Reading the header
    read_entete(&tmpH);
    
    // Check for new file with no blocs
    if (tmpH.ent.nb_bloc == 0) {
        Blocgame t = createGameBloc(g);
        tmpH.ent.nb_bloc++;
        write_entete(&tmpH);
        write_bloc_game(&tmpH, t, tmpH.ent.nb_bloc);
        return g;
    }

    Blocgame t;
    read_bloc_game(tmpH, &t, tmpH.ent.nb_bloc);
    
    // Check if bloc is full
    if (t.nb == max2GAME) {
        Blocgame temp = createGameBloc(g);
        tmpH.ent.nb_bloc++;
        write_entete(&tmpH);
        write_bloc_game(&tmpH, temp, tmpH.ent.nb_bloc);
        return g;
    }

    // Bloc isn't full
    t.tab_enreg[t.nb++] = g;
    write_bloc_game(&tmpH, t, tmpH.ent.nb_bloc);
    return g;
}

Game search_game(TOF tof, int gID, int index) {
    read_entete(&tof);
    
    // Search through all blocks
    for (int currentBlock = 1; currentBlock <= tof.ent.nb_bloc; currentBlock++) {
        Blocgame temp;
        read_bloc_game(tof, &temp, currentBlock);
        
        // Search within the current block
        for (int i = 0; i < temp.nb; i++) {
            if (temp.tab_enreg[i].gameID == gID) {
                return temp.tab_enreg[i]; // Return the game if found
            }
        }
    }
    
    return defaultGame; // Return default game if not found
}

void loadGameFile(TOF *f, GameNode **strt) {
    FILE * file = fopen(f->fichier,"rb");
    if (file == NULL) {
        perror("Failed to open file for reading");
        return;
    }
    GameNode *ptr, *tail = NULL;
    int i = 1;

    read_entete(f); // Directly read the header from f
    while (i <= f->ent.nb_bloc) {
        Blocgame tmp;
        read_bloc_game(*f, &tmp, i);
        
        for (int j = 0; j < tmp.nb; j++) {
            ptr = malloc(sizeof(GameNode));
            if (!ptr) {
                perror("Memory allocation failed");
                return; // Handle memory allocation failure
            }

            // Copy game data from tmp.tab_enreg[j] into the new node
            ptr->inf = tmp.tab_enreg[j];
            ptr->next = NULL;

            // Append to the list in original order
            if (*strt == NULL) {
                *strt = ptr; // First node in the list
            } else {
                tail->next = ptr; // Link to the end of the list
            }
            tail = ptr; // Update tail to the new node
        }
        i++;
    }
    fclose(file);
}

int sortGameList(GameNode **strt) {
    if (!strt || !*strt) return -1;  // Handle empty list
    
    int swapped;
    GameNode *ptr;
    GameNode *lptr = NULL;
    
    do {
        swapped = 0;
        ptr = *strt;
        
        while (ptr->next != lptr) {
            if (ptr->inf.gameID > ptr->next->inf.gameID) {
                // Swap game data
                Game tmp = ptr->inf;
                ptr->inf = ptr->next->inf;
                ptr->next->inf = tmp;
                swapped = 1;
            }
            ptr = ptr->next;
        }
        lptr = ptr;
    } while (swapped);
    
    return 0;
}

void saveGameFile(TOF f, GameNode *strt) {
    if (!strt) return;
    
    // Reset file header
    f.ent.nb_bloc = 0;
    write_entete(&f);
    
    Blocgame currentBloc;
    currentBloc.nb = 0;
    GameNode *ptr = strt;
    
    while (ptr) {
        // If current block is full, write it and start a new one
        if (currentBloc.nb >= MAX_ENREG) {
            write_bloc_game(&f, currentBloc, f.ent.nb_bloc + 1);
            f.ent.nb_bloc++;
            write_entete(&f);
            currentBloc.nb = 0;
        }
        
        // Add game to current block
        currentBloc.tab_enreg[currentBloc.nb] = ptr->inf;
        currentBloc.nb++;
        
        ptr = ptr->next;
    }
    
    // Write final block if it contains any games
    if (currentBloc.nb > 0) {
        write_bloc_game(&f, currentBloc, f.ent.nb_bloc + 1);
        f.ent.nb_bloc++;
        write_entete(&f);
    }
}

void displayGame(Game game) {
    printf("Game ID: %d\n", game.gameID);
    printf("Title: %s\n", game.title);
    printf("Price: %.2f\n", game.rentalPrice);
}

void display_all_games(TOF tof) {
    // Read the header to get the number of blocs
    read_entete(&tof);
    int count = 0;
    
    printf("\n=== All Games ===\n");
    for (int i = 1; i <= tof.ent.nb_bloc; i++) {
        Blocgame temp;
        read_bloc_game(tof, &temp, i);
        for (int j = 0; j < temp.nb; j++) {
            count++;
            printf("\nGame #%d:\n", count);
            displayGame(temp.tab_enreg[j]);
            printf("-------------------\n");
        }
    }
    
    if (count == 0) {
        printf("No games found.\n");
    } else {
        printf("\nTotal games: %d\n", count);
    }
}
