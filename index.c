#include "index.h"
#include <string.h>

// Create a new index table
IndexTable* createIndexTable(int initialCapacity, char type) {
    IndexTable* table = (IndexTable*)malloc(sizeof(IndexTable));
    if (!table) return NULL;

    table->records = (IndexRecord*)malloc(sizeof(IndexRecord) * initialCapacity);
    if (!table->records) {
        free(table);
        return NULL;
    }

    table->size = 0;
    table->capacity = initialCapacity;
    table->type = type;
    return table;
}

// Compare function for qsort
static int compareIndexRecords(const void* a, const void* b) {
    return ((IndexRecord*)a)->key - ((IndexRecord*)b)->key;
}

// Insert a new record into the index
void insertIntoIndex(IndexTable* table, int key, long address) {
    if (table->size >= table->capacity) {
        int newCapacity = table->capacity * 2;
        IndexRecord* newRecords = (IndexRecord*)realloc(table->records, 
                                                      sizeof(IndexRecord) * newCapacity);
        if (!newRecords) return;

        table->records = newRecords;
        table->capacity = newCapacity;
    }

    table->records[table->size].key = key;
    table->records[table->size].address = address;
    table->size++;

    // Keep index sorted for binary search
    sortIndex(table);
}

// Sort the index table
void sortIndex(IndexTable* table) {
    qsort(table->records, table->size, sizeof(IndexRecord), compareIndexRecords);
}

// Binary search implementation
int binarySearch(IndexTable* table, int key) {
    int left = 0;
    int right = table->size - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (table->records[mid].key == key)
            return mid;

        if (table->records[mid].key < key)
            left = mid + 1;
        else
            right = mid - 1;
    }

    return -1; // Not found
}

// Search for a key in the index
long searchInIndex(IndexTable* table, int key) {
    int index = binarySearch(table, key);
    if (index != -1)
        return table->records[index].address;
    return -1;
}

// Save index table to file
void saveIndex(IndexTable* table, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (!file) return;

    // Write table metadata
    fwrite(&table->size, sizeof(int), 1, file);
    fwrite(&table->type, sizeof(char), 1, file);

    // Write index records
    fwrite(table->records, sizeof(IndexRecord), table->size, file);

    fclose(file);
}

// Load index table from file
IndexTable* loadIndex(const char* filename, char type) {
    FILE* file = fopen(filename, "rb");
    if (!file) return NULL;

    int size;
    char storedType;

    // Read metadata
    fread(&size, sizeof(int), 1, file);
    fread(&storedType, sizeof(char), 1, file);

    if (storedType != type) {
        fclose(file);
        return NULL;
    }

    IndexTable* table = createIndexTable(size, type);
    if (!table) {
        fclose(file);
        return NULL;
    }

    // Read index records
    fread(table->records, sizeof(IndexRecord), size, file);
    table->size = size;

    fclose(file);
    return table;
}

// Free index table memory
void freeIndexTable(IndexTable* table) {
    if (table) {
        if (table->records) {
            free(table->records);
        }
        free(table);
    }
}

// Rebuild index from data file
void rebuildIndex(IndexTable* table, const char* dataFilename, char type) {
    FILE* file = fopen(dataFilename, "rb");
    if (!file) return;

    // Clear existing index
    table->size = 0;

    long address;
    int key;

    // Read file header if exists
    fseek(file, sizeof(Entete), SEEK_SET);

    // Read records and build index
    while (1) {
        address = ftell(file);
        
        // Read key based on record type
        size_t readSize;
        switch (type) {
            case 'R': {
                RentalD rental;
                readSize = fread(&rental, sizeof(RentalD), 1, file);
                key = rental.rentalID;
                break;
            }
            case 'C': {
                Customer customer;
                readSize = fread(&customer, sizeof(Customer), 1, file);
                key = customer.customerID;
                break;
            }
            case 'G': {
                Game game;
                readSize = fread(&game, sizeof(Game), 1, file);
                key = game.gameID;
                break;
            }
            default:
                readSize = 0;
        }

        if (readSize != 1) break;
        insertIntoIndex(table, key, address);
    }

    fclose(file);
    sortIndex(table);
}
