#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "struct.h"
#include "Customer.h"
#include "Tools.h"

Customer defaultCustomer = {0, "\0", "\0", "\0"};

BlocC createCustBloc(Customer c) {
    BlocC temp;
    temp.tab_enreg[0] = c;
    temp.nb = 1;
    return temp;
}

void read_bloc_Customer(TOF f, BlocC *bc, int bIndx) {
    FILE *fp = fopen(f.fichier, "rb");
    if (fp == NULL) {
        perror("Failed to open file for reading");
        return;
    }
    fseek(fp, sizeof(Entete) + ((bIndx - 1) * sizeof(BlocC)), SEEK_SET);
    fread(bc, sizeof(BlocC), 1, fp);
    fclose(fp);
}

void write_bloc_Customer(TOF *f, BlocC bc, int bIndx) {
    FILE *fp = fopen(f->fichier, "rb+");
    if (fp == NULL) {
        perror("Failed to open file for writing");
        return;
    }
    fseek(fp, (sizeof(Entete) + ((bIndx - 1) * sizeof(BlocC))), SEEK_SET);
    fwrite(&bc, sizeof(BlocC), 1, fp);
    fclose(fp);
}

Customer createCustomer(int cID, char cName[], char cLname[], char cContInf[], TOF *f) {
    // Input validation
    if (cID <= 0 || !f || !f->f) {
        printf("Error: Invalid customer ID or file handle\n");
        return defaultCustomer;
    }
    
    // Check if customer ID already exists
    Customer existing = search_Customer(*f, cID, 1);
    if (existing.customerID != 0) {
        printf("Error: Customer ID %d already exists\n", cID);
        return defaultCustomer;
    }

    Customer c;
    c.customerID = cID;
    strncpy(c.firstName, cName, sizeof(c.firstName) - 1);
    c.firstName[sizeof(c.firstName) - 1] = '\0';
    strncpy(c.lastName, cLname, sizeof(c.lastName) - 1);
    c.lastName[sizeof(c.lastName) - 1] = '\0';
    strncpy(c.contactInfo, cContInf, sizeof(c.contactInfo) - 1);
    c.contactInfo[sizeof(c.contactInfo) - 1] = '\0';

    // Check for new file with no blocs
    if (f->ent.nb_bloc == 0) {
        BlocC t = createCustBloc(c);
        f->ent.nb_bloc++;
        write_entete(f);
        write_bloc_Customer(f, t, f->ent.nb_bloc);
        return c;
    }

    BlocC t;
    read_bloc_Customer(*f, &t, f->ent.nb_bloc);
    
    // Check if bloc is full
    if (t.nb == max1CUSTOMER) {
        BlocC temp = createCustBloc(c);
        f->ent.nb_bloc++;
        write_entete(f);
        write_bloc_Customer(f, temp, f->ent.nb_bloc);
        return c;
    }

    // Bloc isn't full
    t.tab_enreg[t.nb++] = c;
    write_bloc_Customer(f, t, f->ent.nb_bloc);
    return c;
}

Customer search_Customer(TOF tof, int scID, int index) {
    Customer defaultCustomer = {0}; // Initialize empty customer
    read_entete(&tof);
    
    // Search through all blocks
    for (int currentBlock = 1; currentBlock <= tof.ent.nb_bloc; currentBlock++) {
        BlocC temp;
        read_bloc_Customer(tof, &temp, currentBlock);
        
        // Search within the current block
        for (int i = 0; i < temp.nb; i++) {
            if (temp.tab_enreg[i].customerID == scID) {
                return temp.tab_enreg[i]; // Return the customer if found
            }
        }
    }
    
    return defaultCustomer; // Return empty customer if not found
}

void loadFile(TOF *f, CNode **strt) {
    CNode *ptr, *tail = NULL;
    int i = 1;

    read_entete(f); // Directly read the header from f
    while (i <= f->ent.nb_bloc) {
        BlocC tmp;
        read_bloc_Customer(*f, &tmp, i);
        
        for (int j = 0; j < tmp.nb; j++) {
            ptr = malloc(sizeof(CNode));
            if (!ptr) {
                perror("Memory allocation failed");
                return; // Handle memory allocation failure
            }

            // Copy customer data from tmp.tab_enreg[j] into the new node
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
}

int sortCustList(CNode **strt) {
    if (!strt || !*strt) return -1;  // Handle empty list
    
    int swapped;
    CNode *ptr;
    CNode *lptr = NULL;
    
    do {
        swapped = 0;
        ptr = *strt;
        
        while (ptr->next != lptr) {
            if (ptr->inf.customerID > ptr->next->inf.customerID) {
                // Swap customer data
                Customer tmp = ptr->inf;
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

void saveFile(TOF f, CNode *strt) {
    if (!strt) return;
    
    // Reset file header
    f.ent.nb_bloc = 0;
    write_entete(&f);
    
    BlocC currentBloc;
    currentBloc.nb = 0;
    CNode *ptr = strt;
    
    while (ptr) {
        // If current block is full, write it and start a new one
        if (currentBloc.nb >= MAX_ENREG) {
            write_bloc_Customer(&f, currentBloc, f.ent.nb_bloc + 1);
            f.ent.nb_bloc++;
            write_entete(&f);
            currentBloc.nb = 0;
        }
        
        // Add customer to current block
        currentBloc.tab_enreg[currentBloc.nb] = ptr->inf;
        currentBloc.nb++;
        
        ptr = ptr->next;
    }
    
    // Write final block if it contains any customers
    if (currentBloc.nb > 0) {
        write_bloc_Customer(&f, currentBloc, f.ent.nb_bloc + 1);
        f.ent.nb_bloc++;
        write_entete(&f);
    }
}

void displayCustomer(Customer customer){
    printf("Customer ID: %d\n", customer.customerID);
    printf("Name: %s %s\n", customer.firstName, customer.lastName);
    printf("Contact: %s\n", customer.contactInfo);
}

void display_all_Customers(TOF tof) {
    // Read the header to get the number of blocs
    read_entete(&tof);
    int count = 0;
    
    printf("\n=== All Customers ===\n");
    for (int i = 1; i <= tof.ent.nb_bloc; i++) {
        BlocC temp;
        read_bloc_Customer(tof, &temp, i);
        for (int j = 0; j < temp.nb; j++) {
            count++;
            printf("\nCustomer #%d:\n", count);
            displayCustomer(temp.tab_enreg[j]);
            printf("-------------------\n");
        }
    }
    
    if (count == 0) {
        printf("No customers found.\n");
    } else {
        printf("\nTotal customers: %d\n", count);
    }
}
