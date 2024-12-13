#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "Tools.h"

BlocC createCustBloc(Customer c);
void read_bloc_Customer(TOF f, BlocC *bc, int bIndx);
void write_bloc_Customer(TOF *f, BlocC bc, int bIndx);

// Core operations
Customer createCustomer(int cID, char cName[], char cLname[], char cContInf[], TOF *f);
Customer search_Customer(TOF tof, int scID, int index);

// List operations
void loadFile(TOF *f, CNode **strt);
void saveFile(TOF f, CNode *strt);

// Display functions
void displayCustomer(Customer customer);
void display_all_Customers(TOF tof);

#endif