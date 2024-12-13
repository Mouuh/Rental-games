#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "index_manager.h"

void initializeIndexes(IndexManager* manager) {
    if (!manager) return;
    manager->rentalRoot = NULL;
    manager->customerNameRoot = NULL;
    manager->rentalCount = 0;
    manager->customerCount = 0;
    memset(&manager->stats, 0, sizeof(IndexManagerStats));
}

void updateIndexes(RentalD rental, Customer customer, int blockNumber, IndexManager* manager) {
    if (!manager) return;

    // Update rental index
    RentalIndex* newRental = malloc(sizeof(RentalIndex));
    if (newRental) {
        newRental->rentalID = rental.rentalID;
        newRental->blockNumber = blockNumber;
        newRental->next = manager->rentalRoot;
        manager->rentalRoot = newRental;
        manager->rentalCount++;
    }

    // Update customer name index
    CustomerNameIndex* newCustomer = malloc(sizeof(CustomerNameIndex));
    if (newCustomer) {
        snprintf(newCustomer->fullName, sizeof(newCustomer->fullName), "%s %s", 
                customer.firstName, customer.lastName);
        newCustomer->customerID = customer.customerID;
        newCustomer->blockNumber = blockNumber;
        newCustomer->next = manager->customerNameRoot;
        manager->customerNameRoot = newCustomer;
        manager->customerCount++;
    }
}

void buildRentalIndex(RentalNode* head, IndexManager* manager) {
    if (!manager) return;
    
    // Clear existing index
    cleanupIndexes(manager);
    initializeIndexes(manager);
    
    // Rebuild from linked list
    RentalNode* current = head;
    int blockNumber = 0;
    
    while (current) {
        RentalIndex* newRental = malloc(sizeof(RentalIndex));
        if (newRental) {
            newRental->rentalID = current->inf.rentalID;
            newRental->blockNumber = blockNumber++;
            newRental->next = manager->rentalRoot;
            manager->rentalRoot = newRental;
            manager->rentalCount++;
        }
        current = current->next;
    }
}

void loadIndexes(IndexManager* manager, const char* rentalIndexFile, const char* customerNameIndexFile) {
    if (!manager) return;
    
    // Load rental index
    FILE* rf = fopen(rentalIndexFile, "rb");
    if (rf) {
        RentalIndex temp;
        while (fread(&temp, sizeof(RentalIndex), 1, rf) == 1) {
            RentalIndex* newNode = malloc(sizeof(RentalIndex));
            if (newNode) {
                *newNode = temp;
                newNode->next = manager->rentalRoot;
                manager->rentalRoot = newNode;
                manager->rentalCount++;
            }
        }
        fclose(rf);
    }
    
    // Load customer name index
    FILE* cf = fopen(customerNameIndexFile, "rb");
    if (cf) {
        CustomerNameIndex temp;
        while (fread(&temp, sizeof(CustomerNameIndex), 1, cf) == 1) {
            CustomerNameIndex* newNode = malloc(sizeof(CustomerNameIndex));
            if (newNode) {
                *newNode = temp;
                newNode->next = manager->customerNameRoot;
                manager->customerNameRoot = newNode;
                manager->customerCount++;
            }
        }
        fclose(cf);
    }
}

void saveIndexes(IndexManager* manager, const char* rentalIndexFile, const char* customerNameIndexFile) {
    if (!manager) return;
    
    // Save rental index
    FILE* rf = fopen(rentalIndexFile, "wb");
    if (rf) {
        RentalIndex* current = manager->rentalRoot;
        while (current) {
            fwrite(current, sizeof(RentalIndex), 1, rf);
            current = current->next;
        }
        fclose(rf);
    }
    
    // Save customer name index
    FILE* cf = fopen(customerNameIndexFile, "wb");
    if (cf) {
        CustomerNameIndex* current = manager->customerNameRoot;
        while (current) {
            fwrite(current, sizeof(CustomerNameIndex), 1, cf);
            current = current->next;
        }
        fclose(cf);
    }
}

void cleanupIndexes(IndexManager* manager) {
    if (!manager) return;
    
    // Cleanup rental index
    while (manager->rentalRoot) {
        RentalIndex* temp = manager->rentalRoot;
        manager->rentalRoot = manager->rentalRoot->next;
        free(temp);
    }
    
    // Cleanup customer name index
    while (manager->customerNameRoot) {
        CustomerNameIndex* temp = manager->customerNameRoot;
        manager->customerNameRoot = manager->customerNameRoot->next;
        free(temp);
    }
    
    manager->rentalCount = 0;
    manager->customerCount = 0;
    memset(&manager->stats, 0, sizeof(IndexManagerStats));
}
