#ifndef STRUCT_H
#define STRUCT_H

#include <stdio.h>
#include <stdlib.h>

#define max1CUSTOMER  20
#define max2GAME 39
#define max3RENTAL  12
#define MAX_ENREG 10  // Maximum records per block

// Error handling structures
typedef enum {
    SUCCESS = 0,
    ERROR_INVALID_ID = -1,
    ERROR_NULL_POINTER = -2,
    ERROR_FILE_ACCESS = -3,
    ERROR_INVALID_DATE = -4,
    ERROR_INDEX_CORRUPTED = -5
} ErrorCode;

typedef struct {
    ErrorCode code;
    char message[256];
} OperationResult;

// Basic data structures
typedef struct Date {
    int day;
    int month;
    int year;
} Date;

typedef struct Customer {
    int customerID;
    char firstName[50];
    char lastName[50];
    char contactInfo[100];
} Customer;

typedef struct Game {
    int gameID;
    char title[100];
    float rentalPrice;
} Game;

typedef struct Rental {  //RAM
    int rentalID;
    Customer customer;
    Game game;
    Date rentalDate;
    Date returnDate;
    float rentalPrice;
} Rental;

typedef struct RentalD {  //DISK
    int rentalID;
    int customerID;
    int gameID;
    Date rentalDate;
    Date returnDate;
    float rentalPrice;
} RentalD;

// Linked list node structures
typedef struct CNode {
    Customer inf;
    struct CNode *next;
} CNode;

typedef struct GameNode {
    Game inf;
    struct GameNode *next;
} GameNode;

typedef struct RentalNode {
    RentalD inf;
    struct RentalNode *next;
} RentalNode;

// Index structures
typedef struct RentalIndex {
    int rentalID;
    int blockNumber;
    struct RentalIndex* next;
} RentalIndex;

typedef struct CustomerNameIndex {
    char fullName[100];
    int customerID;
    int blockNumber;
    struct CustomerNameIndex* next;
} CustomerNameIndex;

// Statistics structures
typedef struct {
    int totalEntries;
    int height;
    int balanceFactor;
} IndexStats;

typedef struct {
    IndexStats rentalIndexStats;
    IndexStats customerNameIndexStats;
    OperationResult result;
} IndexManagerStats;

// Main index manager
typedef struct IndexManager {
    RentalIndex* rentalRoot;
    CustomerNameIndex* customerNameRoot;
    int rentalCount;
    int customerCount;
    IndexManagerStats stats;
} IndexManager;

// File structures
typedef struct entete {
    int nb_bloc;
} Entete;

typedef struct TOF {
    FILE* f;
    Entete ent;
    char fichier[200];
} TOF;

typedef struct Bloc {
    RentalD tab_enreg[max3RENTAL];
    int nb;
} Bloc;

typedef struct Blocgame {
    Game tab_enreg[max2GAME];
    int nb;
} Blocgame;

typedef struct BlocC {
    Customer tab_enreg[max1CUSTOMER];
    int nb;
} BlocC;

// Search result structure
typedef struct {
    Rental* rentals;
    int count;
    int capacity;
    OperationResult result;
} RentalSearchResult;

#endif
