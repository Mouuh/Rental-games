#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "Customer.h"
#include "rental.h"
#include "game.h"
#include "struct.h"
#include "Tools.h"
#include "index_manager.h"

// Nodes and structs
CNode *cHead = NULL;
GameNode *gHead = NULL;
RentalNode *rHead = NULL;
Customer tempCustomer, tempCust;
Game tempGame, tmpGame;
RentalD tempRent;
RentalD temprentId;

// Indexes
IndexManager indexManager;
const char* RENTAL_INDEX_FILE = "rental_index.bin";
const char* CUSTOMER_NAME_INDEX_FILE = "customer_name_index.bin";

// Files and file-variables
TOF *cTof = NULL;
TOF *gTof = NULL;
TOF *rTof = NULL;
char opMode[2]; // N for New file, A for existing file

void displayCustomerMenu() {
    int choice = -1;
    while (choice != 0) {
        printf("\n=========== Customer Menu ===========\n");
        printf("1  - Register a customer\n");
        printf("2  - Search for a customer\n");
        printf("3  - Display all customers\n");
        printf("0  - Back to Main Menu\n");
        printf("=====================================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                printf("\n=== Create New Customer ===\n");
                printf("Enter customer ID (positive number): ");
                if (scanf("%d", &tempCustomer.customerID) != 1 || tempCustomer.customerID <= 0) {
                    printf("Error: Invalid customer ID. Please enter a positive number.\n");
                    while (getchar() != '\n'); // Clear input buffer
                    break;
                }
                
                printf("Enter first name: ");
                while (getchar() != '\n'); // Clear input buffer
                if (fgets(tempCustomer.firstName, sizeof(tempCustomer.firstName), stdin)) {
                    tempCustomer.firstName[strcspn(tempCustomer.firstName, "\n")] = 0; // Remove newline
                }
                
                printf("Enter last name: ");
                if (fgets(tempCustomer.lastName, sizeof(tempCustomer.lastName), stdin)) {
                    tempCustomer.lastName[strcspn(tempCustomer.lastName, "\n")] = 0; // Remove newline
                }
                
                printf("Enter phone number (+213): ");
                if (fgets(tempCustomer.contactInfo, sizeof(tempCustomer.contactInfo), stdin)) {
                    tempCustomer.contactInfo[strcspn(tempCustomer.contactInfo, "\n")] = 0; // Remove newline
                }
                
                tempCust = createCustomer(tempCustomer.customerID, tempCustomer.firstName, 
                                        tempCustomer.lastName, tempCustomer.contactInfo, cTof);
                
                if (tempCust.customerID != 0) {
                    printf("\nCustomer created successfully!\n");
                    
                    // Update customer list
                    CNode* newNode = malloc(sizeof(CNode));
                    if (newNode) {
                        newNode->inf = tempCust;
                        newNode->next = cHead;
                        cHead = newNode;
                    } else {
                        printf("Warning: Could not update customer list\n");
                    }
                }
                break;
            }
            case 2:
                printf("Search for a customer!\n");
                int searchID;
                printf("Enter customer ID to search: ");
                scanf("%d", &searchID);
                read_entete(cTof);
                Customer foundCustomer = search_Customer(*cTof, searchID, 1);
                if (foundCustomer.customerID != 0) {
                    printf("Customer found\n");
                    displayCustomer(foundCustomer);         
                } else {
                    printf("Customer not found!\n");
                }
                break;
            case 3:
                printf("Displaying all customers information:\n");
                display_all_Customers(*cTof);
                break;
            case 0:
                printf("Returning to Main Menu...\n");
                break;
            default:
                printf("Invalid choice! Please try again.\n");
                break;
        }
    }
}

void displayGameMenu() {
    int choice = -1;
    while (choice != 0) {
        printf("\n============ Game Menu =============\n");
        printf("1  - Register a game\n");
        printf("2  - Search for a game\n");
        printf("3  - Display all games\n");
        printf("0  - Back to Main Menu\n");
        printf("====== ==============================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Create game!\n");
                printf("ID: ");
                scanf("%d", &tmpGame.gameID);
                printf("Game title: ");
                scanf(" %[^\n]s", tmpGame.title);
                printf("Game price: ");
                scanf("%f", &tmpGame.rentalPrice);
                tempGame = createGame(tmpGame.gameID, tmpGame.title, tmpGame.rentalPrice, gTof);
                break;
            case 2:
                printf("Search for a game!\n");
                int searchGameID;
                printf("Enter game ID to search: ");
                scanf("%d", &searchGameID);
                read_entete(gTof);
                Game foundGame = search_game(*gTof, searchGameID, 1);
                if (foundGame.gameID != 0) {
                    printf("Game found\n");
                    displayGame(foundGame);
                } else {
                    printf("Game not found!\n");
                }
                break;
            case 3:
                printf("Displaying all games information:\n");
                display_all_games(*gTof);
                break;
            case 0:
                printf("Returning to Main Menu...\n");
                break;
            default:
                printf("Invalid choice! Please try again.\n");
                break;
        }
    }
}

void displayRentalMenu() {
    int choice = -1;
    RentalD tempRent, createdRental;
    
    while (choice != 0) {
        printf("\n=========== Rental Menu ============\n");
        printf("1  - Register a rental operation\n");
        printf("2  - Search for a rental by ID\n");
        printf("3  - Search rentals by date range\n");
        printf("4  - Search rentals by customer\n");
        printf("5  - Display rental statistics\n");
        printf("6  - Display all rentals\n");
        printf("0  - Back to Main Menu\n");
        printf("====================================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                printf("\n=== Create New Rental ===\n");
                printf("Enter rental ID: ");
                scanf("%d", &tempRent.rentalID);
                
                printf("Enter customer ID: ");
                scanf("%d", &tempCustomer.customerID);
                tempCust = search_Customer(*cTof, tempCustomer.customerID, 1);
                
                if (tempCust.customerID == 0) {
                    printf("\nCustomer not found! Please add customer details:\n");
                    printf("First name: ");
                    scanf("%s", tempCustomer.firstName);
                    printf("Last name: ");
                    scanf("%s", tempCustomer.lastName);
                    printf("Phone number: +213");
                    scanf("%s", tempCustomer.contactInfo);
                    tempCust = createCustomer(tempCustomer.customerID, tempCustomer.firstName, 
                                           tempCustomer.lastName, tempCustomer.contactInfo, cTof);
                }
                
                printf("Enter game ID: ");
                scanf("%d", &tmpGame.gameID);
                tempGame = search_game(*gTof, tmpGame.gameID, 1);
                
                if (tempGame.gameID == 0) {
                    printf("\nGame not found! Please add game details:\n");
                    printf("Game title: ");
                    scanf(" %[^\n]s", tmpGame.title);
                    printf("Rental price per day: ");
                    scanf("%f", &tmpGame.rentalPrice);
                    tempGame = createGame(tmpGame.gameID, tmpGame.title, tmpGame.rentalPrice, gTof);
                }
                
                printf("Enter rental date (YYYY MM DD): ");
                while (scanf("%d %d %d", &tempRent.rentalDate.year, 
                           &tempRent.rentalDate.month, 
                           &tempRent.rentalDate.day) != 3) {
                    printf("Invalid date format. Please enter year month day (e.g., 2023 12 25): ");
                    while (getchar() != '\n'); // Clear input buffer
                }
                
                printf("Enter return date (YYYY MM DD): ");
                while (scanf("%d %d %d", &tempRent.returnDate.year, 
                           &tempRent.returnDate.month, 
                           &tempRent.returnDate.day) != 3) {
                    printf("Invalid date format. Please enter year month day (e.g., 2023 12 25): ");
                    while (getchar() != '\n'); // Clear input buffer
                }
                
                // Calculate rental price
                int numberDays = (tempRent.returnDate.year - tempRent.rentalDate.year) * 365 +
                               (tempRent.returnDate.month - tempRent.rentalDate.month) * 30 +
                               (tempRent.returnDate.day - tempRent.rentalDate.day);
                tempRent.rentalPrice = (float)numberDays * tempGame.rentalPrice;
                
                // Create the rental
                createdRental = createRental(tempRent.rentalID, tempCust.customerID, tempGame.gameID, 
                                           tempRent.rentalDate, tempRent.returnDate, tempRent.rentalPrice, rTof);
                
                if (createdRental.rentalID > 0) {
                    printf("\nRental created successfully!\n");
                    printf("Total rental price: $%.2f\n", createdRental.rentalPrice);
                    
                    // Update linked list
                    RentalNode* newNode = malloc(sizeof(RentalNode));
                    if (newNode) {
                        newNode->inf = createdRental;
                        newNode->next = rHead;
                        rHead = newNode;
                        
                        // Update indexes
                        updateIndexes(createdRental, tempCust, rTof->ent.nb_bloc, &indexManager);
                    } else {
                        printf("\nWarning: Could not update rental list\n");
                    }
                } else {
                    printf("\nError creating rental. Please try again.\n");
                }
                break;
            }
            case 2: {
                printf("\n=== Search Rental by ID ===\n");
                int searchID;
                printf("Enter rental ID to search: ");
                if (scanf("%d", &searchID) != 1 || searchID <= 0) {
                    printf("Error: Invalid rental ID. Please enter a positive number.\n");
                    while (getchar() != '\n'); // Clear input buffer
                    break;
                }
                
                if (rTof == NULL || cTof == NULL || gTof == NULL) {
                    printf("Error: Database files not properly initialized\n");
                    break;
                }
                
                // Search using the rental index
                Rental foundRental = searchByRentalID(searchID, indexManager.rentalRoot, *rTof, *cTof, *gTof);
                if (foundRental.rentalID != 0) {
                    printf("\nRental found!\n");
                    displayRental(foundRental);
                } else {
                    printf("\nRental not found.\n");
                }
                break;
            }
            case 3: {
                printf("\n=== Search Rentals by Date Range ===\n");
                printf("This feature is not implemented yet.\n");
                break;
            }
            case 4: {
                printf("\n=== Search Rentals by Customer ===\n");
                printf("This feature is not implemented yet.\n");
                break;
            }
            case 5: {
                printf("\n=== Rental Statistics ===\n");
                printf("Total rentals: %d\n", indexManager.rentalCount);
                break;
            }
            case 6: {
                printf("\n=== All Rentals ===\n");
                display_all_rentals(*rTof, *cTof, *gTof, indexManager.rentalRoot);
                break;
            }
            case 0:
                printf("Returning to Main Menu...\n");
                break;
            default:
                printf("Invalid choice! Please try again.\n");
                break;
        }
    }
}

int main() {
    char opMode[2];
    
    // Initialize file structures
    cTof = malloc(sizeof(TOF));
    gTof = malloc(sizeof(TOF));
    rTof = malloc(sizeof(TOF));
    
    if (!rTof || !cTof || !gTof) {
        printf("Error: Memory allocation failed\n");
        return 1;
    }
    
    // Initialize index manager
    memset(&indexManager, 0, sizeof(IndexManager));  // Clear the structure
    initializeIndexes(&indexManager);
    
    // Initialize linked list heads
    rHead = NULL;
    cHead = NULL;
    gHead = NULL;
    
    // Open customer file
    printf("Opening mode for customer file (N for new file, A for existing): ");
    scanf("%1s", opMode);
    cTof = open_file("cust.bin", opMode[0]);
    if (!cTof) {
        printf("Error opening customer file\n");
        return 1;
    }
    
    // Open game file
    printf("Opening mode for game file (N for new file, A for existing): ");
    scanf("%1s", opMode);
    gTof = open_file("game.bin", opMode[0]);
    if (!gTof) {
        printf("Error opening game file\n");
        free(cTof);
        return 1;
    }
    
    // Open rental file
    printf("Opening mode for rental file (N for new file, A for existing): ");
    scanf("%1s", opMode);
    rTof = open_file("rent.bin", opMode[0]);
    if (!rTof) {
        printf("Error opening rental file\n");
        free(cTof);
        free(gTof);
        return 1;
    }
    
    // Load data if files exist
    if (opMode[0] == 'A') {
        // Load existing data into memory first
        loadFile(cTof, &cHead);
        loadGameFile(gTof, &gHead);
        loadRentals(rTof, &rHead);
        
        // Then load indexes
        loadIndexes(&indexManager, RENTAL_INDEX_FILE, CUSTOMER_NAME_INDEX_FILE);
        
        // If no rental index exists, rebuild it
        if (!indexManager.rentalRoot) {
            buildRentalIndex(rHead, &indexManager);
            saveIndexes(&indexManager, RENTAL_INDEX_FILE, CUSTOMER_NAME_INDEX_FILE);
        }
    } else {
        // For new files, initialize empty structures
        initializeIndexes(&indexManager);
    }
    
    int mainChoice = -1;
    while (mainChoice != 0) {
        printf("\n========== Main Menu ==========\n");
        printf("1 - Customer Management\n");
        printf("2 - Game Management\n");
        printf("3 - Rental Management\n");
        printf("0 - Exit\n");
        printf("==============================\n");
        printf("Enter your choice: ");
        scanf("%d", &mainChoice);
        
        switch (mainChoice) {
            case 1:
                displayCustomerMenu();
                break;
            case 2:
                displayGameMenu();
                break;
            case 3:
                displayRentalMenu();
                break;
            case 0:
                printf("Saving and exiting...\n");
                // Save indexes before exit
                saveIndexes(&indexManager, RENTAL_INDEX_FILE, CUSTOMER_NAME_INDEX_FILE);
                break;
            default:
                printf("Invalid choice! Please try again.\n");
                break;
        }
    }
    
    // Cleanup
    if (rTof) {
        close_file(rTof);
        free(rTof);
    }
    if (cTof) {
        close_file(cTof);
        free(cTof);
    }
    if (gTof) {
        close_file(gTof);
        free(gTof);
    }
    cleanupIndexes(&indexManager);
    
    return 0;
}