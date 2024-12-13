#ifndef INDEX_MANAGER_H
#define INDEX_MANAGER_H

#include "struct.h"
#include "index.h"

// Index manager functions
void initializeIndexes(IndexManager* manager);
void updateIndexes(RentalD rental, Customer customer, int blockNumber, IndexManager* manager);
void buildRentalIndex(RentalNode* head, IndexManager* manager);
void loadIndexes(IndexManager* manager, const char* rentalIndexFile, const char* customerNameIndexFile);
void saveIndexes(IndexManager* manager, const char* rentalIndexFile, const char* customerNameIndexFile);
void cleanupIndexes(IndexManager* manager);

#endif // INDEX_MANAGER_H
