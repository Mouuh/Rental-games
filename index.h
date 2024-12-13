#ifndef INDEX_H
#define INDEX_H

#include <stdio.h>
#include <stdlib.h>
#include "struct.h"

// Index record structure (key-address pair)
typedef struct {
    int key;           // Record ID (rental ID, customer ID, or game ID)
    long address;      // Physical address in the file
} IndexRecord;

// Index table structure
typedef struct {
    IndexRecord* records;  // Array of index records
    int size;             // Current size of index
    int capacity;         // Maximum capacity
    char type;           // Type of index ('R' for rental, 'C' for customer, 'G' for game)
} IndexTable;

// Function declarations
IndexTable* createIndexTable(int initialCapacity, char type);
void insertIntoIndex(IndexTable* table, int key, long address);
long searchInIndex(IndexTable* table, int key);
void sortIndex(IndexTable* table);
void saveIndex(IndexTable* table, const char* filename);
IndexTable* loadIndex(const char* filename, char type);
void freeIndexTable(IndexTable* table);

// Binary search in index
int binarySearch(IndexTable* table, int key);

// Index maintenance
void rebuildIndex(IndexTable* table, const char* dataFilename, char type);

#endif
