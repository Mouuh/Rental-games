#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "struct.h"
#include "rental.h"
#include "Customer.h"
#include "game.h"
#include "Tools.h"
#include "index.h"

RentalD defaultRental = {0}; // Default rental object

// Basic file operations
RentalD createRental(int rID, int cID, int gID, Date renDate, Date retDate, float rentalPrice, TOF *tof) {
    RentalD rental;
    rental.rentalID = rID;
    rental.customerID = cID;
    rental.gameID = gID;
    rental.rentalDate = renDate;
    rental.returnDate = retDate;
    rental.rentalPrice = rentalPrice;

    // Validate the rental before creating
    OperationResult validation = validateRental(rental);
    if (validation.code != SUCCESS) {
        printf("Rental validation failed: %s\n", validation.message);
        return defaultRental;
    }

    // Write to file and get address
    long address;
    writeRental(tof, rental, &address);
    
    return rental;
}

void writeRental(TOF *tof, RentalD rental, long* address) {
    if (!tof || !tof->f) return;
    
    // Get current position for index
    *address = ftell(tof->f);
    
    // Write rental record
    fwrite(&rental, sizeof(RentalD), 1, tof->f);
    
    // Update TOF header if needed
    tof->ent.nb_bloc++;
    write_entete(tof);
}

RentalD readRental(TOF tof, long address) {
    RentalD rental = defaultRental;
    if (!tof.f) return rental;
    
    // Seek to address and read
    fseek(tof.f, address, SEEK_SET);
    fread(&rental, sizeof(RentalD), 1, tof.f);
    
    return rental;
}

// List operations
void loadRentals(TOF* tof, RentalNode** head) {
    if (!tof || !tof->f || !head) return;
    
    // Clear existing list
    while (*head) {
        RentalNode* temp = *head;
        *head = (*head)->next;
        free(temp);
    }
    
    // Go to start of data
    fseek(tof->f, sizeof(Entete), SEEK_SET);
    
    // Read all rentals
    RentalD rental;
    RentalNode* tail = NULL;
    
    while (fread(&rental, sizeof(RentalD), 1, tof->f) == 1) {
        RentalNode* newNode = malloc(sizeof(RentalNode));
        if (!newNode) break;
        
        newNode->inf = rental;
        newNode->next = NULL;
        
        if (!*head) {
            *head = newNode;
        } else {
            tail->next = newNode;
        }
        tail = newNode;
    }
}

// Search operations
Rental searchByRentalID(int rentalID, RentalIndex* rentalRoot, TOF rTof, TOF cTof, TOF gTof) {
    Rental result = {0};
    
    // Search in index
    RentalIndex* current = rentalRoot;
    while (current && current->rentalID != rentalID) {
        current = current->next;
    }
    
    if (!current) {
        printf("Rental ID %d not found\n", rentalID);
        return result;
    }
    
    // Read rental record
    RentalD rentalD = readRental(rTof, current->blockNumber * sizeof(RentalD) + sizeof(Entete));
    
    // Build complete rental record
    result.rentalID = rentalD.rentalID;
    result.rentalDate = rentalD.rentalDate;
    result.returnDate = rentalD.returnDate;
    result.rentalPrice = rentalD.rentalPrice;
    
    // Get customer and game details using TOF
    result.customer = search_Customer(cTof, rentalD.customerID, 1);
    result.game = search_game(gTof, rentalD.gameID, 1);
    
    return result;
}

// Display functions
void displayRental(Rental rental) {
    printf("\nRental ID: %d\n", rental.rentalID);
    printf("Customer: %s %s\n", rental.customer.firstName, rental.customer.lastName);
    printf("Game: %s\n", rental.game.title);
    printf("Rental Date: %d/%d/%d\n", rental.rentalDate.day, rental.rentalDate.month, rental.rentalDate.year);
    printf("Return Date: %d/%d/%d\n", rental.returnDate.day, rental.returnDate.month, rental.returnDate.year);
    printf("Price: %.2f\n", rental.rentalPrice);
}

void displayRentalDetailed(Rental rental) {
    displayRental(rental);
    printf("Customer Details:\n");
    printf("  ID: %d\n", rental.customer.customerID);
    printf("  Contact: %s\n", rental.customer.contactInfo);
    printf("Game Details:\n");
    printf("  ID: %d\n", rental.game.gameID);
    printf("  Price per day: %.2f\n", rental.game.rentalPrice);
}

void display_all_rentals(TOF rTof, TOF cTof, TOF gTof, RentalIndex* rentalRoot) {
    if (!rentalRoot) {
        printf("\nNo rentals found in the system.\n");
        return;
    }
    
    if (!rTof.f || !cTof.f || !gTof.f) {
        printf("\nError: Database files not properly opened.\n");
        return;
    }
    
    printf("\n=== All Rentals ===\n");
    int count = 0;
    RentalIndex* current = rentalRoot;
    
    while (current) {
        // Calculate the correct file offset
        long address = current->blockNumber * sizeof(RentalD) + sizeof(Entete);
        
        // Read the rental record
        RentalD rentalD = readRental(rTof, address);
        
        if (rentalD.rentalID != 0) {  // Valid rental record
            // Build complete rental record
            Rental rental;
            rental.rentalID = rentalD.rentalID;
            rental.rentalDate = rentalD.rentalDate;
            rental.returnDate = rentalD.returnDate;
            rental.rentalPrice = rentalD.rentalPrice;
            
            // Get customer and game details
            Customer cust = search_Customer(cTof, rentalD.customerID, 1);
            Game game = search_game(gTof, rentalD.gameID, 1);
            
            if (cust.customerID != 0 && game.gameID != 0) {
                rental.customer = cust;
                rental.game = game;
                displayRental(rental);
                count++;
            }
        }
        
        current = current->next;
    }
    
    if (count == 0) {
        printf("\nNo valid rentals found.\n");
    } else {
        printf("\nTotal rentals: %d\n", count);
    }
}

void displayRentalStatistics(TOF rTof, RentalIndex* rentalRoot) {
    if (!rentalRoot || !rTof.f) return;
    
    int count = 0;
    float totalRevenue = 0.0;
    RentalIndex* current = rentalRoot;
    
    while (current) {
        long address = current->blockNumber * sizeof(RentalD) + sizeof(Entete);
        RentalD rental = readRental(rTof, address);
        totalRevenue += rental.rentalPrice;
        count++;
        current = current->next;
    }
    
    printf("\n=== Rental Statistics ===\n");
    printf("Total rentals: %d\n", count);
    printf("Total revenue: $%.2f\n", totalRevenue);
    printf("Average rental price: $%.2f\n", count > 0 ? totalRevenue / count : 0.0);
}

// Keep existing validation functions
OperationResult validateRental(RentalD rental) {
    OperationResult result;
    result.code = SUCCESS;
    strcpy(result.message, "Rental validation successful");

    // Validate rental ID
    if (rental.rentalID <= 0) {
        result.code = ERROR_INVALID_ID;
        strcpy(result.message, "Invalid rental ID");
        return result;
    }

    // Validate customer ID
    if (rental.customerID <= 0) {
        result.code = ERROR_INVALID_ID;
        strcpy(result.message, "Invalid customer ID");
        return result;
    }

    // Validate game ID
    if (rental.gameID <= 0) {
        result.code = ERROR_INVALID_ID;
        strcpy(result.message, "Invalid game ID");
        return result;
    }

    // Validate rental date
    OperationResult dateValidation = validateDate(rental.rentalDate);
    if (dateValidation.code != SUCCESS) {
        return dateValidation;
    }

    // Validate return date
    dateValidation = validateDate(rental.returnDate);
    if (dateValidation.code != SUCCESS) {
        return dateValidation;
    }

    // Validate return date is after rental date
    int totalDays = (rental.returnDate.year - rental.rentalDate.year) * 365 +
                    (rental.returnDate.month - rental.rentalDate.month) * 30 +
                    (rental.returnDate.day - rental.rentalDate.day);
    
    if (totalDays < 0) {
        result.code = ERROR_INVALID_DATE;
        strcpy(result.message, "Return date must be after rental date");
        return result;
    }

    // Validate rental price
    if (rental.rentalPrice < 0) {
        result.code = ERROR_INVALID_ID;
        strcpy(result.message, "Invalid rental price");
        return result;
    }

    return result;
}

OperationResult validateDate(Date date) {
    OperationResult result;
    result.code = SUCCESS;
    strcpy(result.message, "Date validation successful");

    // Check year
    if (date.year < 2000 || date.year > 2100) {
        result.code = ERROR_INVALID_DATE;
        strcpy(result.message, "Invalid year (must be between 2000 and 2100)");
        return result;
    }

    // Check month
    if (date.month < 1 || date.month > 12) {
        result.code = ERROR_INVALID_DATE;
        strcpy(result.message, "Invalid month (must be between 1 and 12)");
        return result;
    }

    // Check day
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    // Adjust February for leap years
    if (date.year % 4 == 0 && (date.year % 100 != 0 || date.year % 400 == 0)) {
        daysInMonth[1] = 29;
    }

    if (date.day < 1 || date.day > daysInMonth[date.month - 1]) {
        result.code = ERROR_INVALID_DATE;
        strcpy(result.message, "Invalid day for the given month");
        return result;
    }

    return result;
}