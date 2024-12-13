#ifndef RENTAL_H
#define RENTAL_H

#include "Tools.h"
#include "game.h"
#include "index.h"

// Basic operations
RentalD createRental(int rID, int cID, int gID, Date renDate, Date retDate, float rentalPrice, TOF *tof);
void writeRental(TOF *tof, RentalD rental, long* address);
RentalD readRental(TOF tof, long address);

// List operations
void loadRentals(TOF* tof, RentalNode** head);

// Enhanced search operations
Rental searchByRentalID(int rentalID, RentalIndex* rentalRoot, TOF rTof, TOF cTof, TOF gTof);

// Display operations
void displayRental(Rental rental);
void displayRentalDetailed(Rental rental);
void display_all_rentals(TOF rTof, TOF cTof, TOF gTof, RentalIndex* rentalRoot);
void displayRentalStatistics(TOF rTof, RentalIndex* rentalRoot);

// Index maintenance
void initializeRentalIndexes(TOF tof, RentalIndex** rentalRoot);
void updateRentalIndex(RentalIndex* rentalRoot, RentalD rental, long address);

// Validation and cleanup
OperationResult validateRental(RentalD rental);
OperationResult validateDate(Date date);

#endif // RENTAL_H
