#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>
#include <time.h>

// Define structures for items in the inventory
struct Item {
    char id[20] ;
    char name[50] ;
    double price ;
    int quantity ;
} ;

// Define structures for customers
struct Customer {
    int id ;
    char name[50] ;
    char phoneNumber[20] ; 
    int pin ;
} ;

// Define structure for discounts
struct Discount {
    int customerId ;
    double discountPercentage ;
} ;


// Define structure for admin
struct Admin {
    char username[50];
    char password[50];
};

void gotoxy(int x, int y) ;

// Inventory management Functions
void displayInventory(void);
void addItem(struct Item newItem) ;
void removeItem(char *itemId) ;
void modifyItem(char *itemId) ;
void manageInventory(void) ;
void loadInventoryFromFile(const char *fileName) ;
void updateInventoryFile(const char *fileName) ;
void checkTransactions(void) ;

// Customer management Functions
void addCustomer(struct Customer newCustomer) ;
void removeCustomer(int customerId) ;
void displayCustomers(void) ;
void manageCustomers(void);
void loadCustomersWithDiscountsFromFile(void);
void writeCustomersWithDiscountsToFile(void) ;

// Discounts management Functions
void addDiscount(int customerId, double discountPercentage) ;
void removeDiscount(int customerId) ;
double getDiscount(int customerId) ;
void displayDiscounts(void) ;
void manageDiscounts(void) ;

// Admin Functions
void adminMenu(void) ;
void changeAdminKeys(void) ;
int authenticateAdmin(char *username, char *password) ;

// Shopping Functions
void storeCheckoutRecords(struct Item *cart, int cartItemCount, char *name, int customerId, double *checkoutPrices) ;
void shopping(struct Customer customer) ;

// Functions for managing inventory items
#define MAX_ITEMS 100 

struct Item inventory[MAX_ITEMS]; // Array to hold all inventory items
int itemCount = 0; // To track the current number of items in inventory

// Was given in OLD Turbo C but declared for new 64-bit compilers as ConsoleCursor function is part of windows.h
void gotoxy(int x, int y) {
    HANDLE hConsoleOutput;
    COORD dwCursorPosition;

    hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    dwCursorPosition.X = x;
    dwCursorPosition.Y = y;

    SetConsoleCursorPosition(hConsoleOutput, dwCursorPosition);
}

// Function to print formatted headings
void printMenu( char name[ ] ) {
    printf("\033[1;36m") ;
    for(int i = 0; i < 16 ; ++i ){
        Sleep(50);
        printf("\xDB");
    }

    for(int i = 0; i < strlen(name) ; ++i ){
        Sleep(60);
        printf(" %c", name[i] );
    }

    for(int i = 0; i < 16; i++){
        Sleep(50);
        printf("\xDB");
    }
    printf("\033[0m") ; // To reset to default color.
}

void loadInventoryFromFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        gotoxy(0 , 0) ;
        printf("\033[1;31mError opening file %s.\n\033[0m", filename);
        return;
    }

// !feof(file) checks if the end-of-file indicator is not set at the current position in the file,
    while (!feof(file) && itemCount < MAX_ITEMS) {
        fscanf(file, "%s %s %lf %d", inventory[itemCount].id, inventory[itemCount].name,
               &inventory[itemCount].price, &inventory[itemCount].quantity);
        itemCount++;
    }

    fclose(file);
}

void displayInventory(void) {
    if (itemCount == 0) {
        gotoxy(20, 20);
        printf("\033[1;31mInventory is empty.\033[0m\n");
        return;
    }

    int row = 5 ;
    gotoxy( 29, 1 ) ;
    printMenu("INVENTORY-DASHBOARD ") ;
    // printf("Inventory-Dashboard:\n");
    // gotoxy( 55, 2 ) ;
    // printf("********************\n") ;
    gotoxy( 45 , row ) ;
    printf("ID     |     Name     |  Price  | Quantity\n");
    for (int i = 0; i < itemCount; i++) {
        gotoxy( 45, ++row ) ; // As previously used so I pre-incremented for not to be on same line
        printf("%-6s | %-12s | %-7.2lf | %-3d\n", inventory[i].id, inventory[i].name, inventory[i].price, inventory[i].quantity);
    }
}

void updateInventoryFile(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        gotoxy( 20, 22) ;
        printf("\033[1;31mError opening file %s for writing.\033[0m\n", filename);
        return;
    }

    // Write the updated inventory to the file
    for (int i = 0; i < itemCount; ++i) {
        if( i == itemCount - 1 )
            fprintf(file, "%s %s %.2lf %d", inventory[i].id, inventory[i].name, inventory[i].price, inventory[i].quantity);
        else
            fprintf(file, "%s %s %.2lf %d\n", inventory[i].id, inventory[i].name, inventory[i].price, inventory[i].quantity);
    }

    fclose(file);
}

void addItem(struct Item newItem) {
    int found = 0 ;
    if (itemCount >= MAX_ITEMS) {
        gotoxy( 20 , 22 ) ;
        printf("\033[1;31mInventory is full. Cannot add more items\033[0m\n\n");
        Sleep(2000) ;
        return;
    }

    // Check if the item ID already exists
    for (int i = 0; i < itemCount; ++i) {
        if ( strcmp (inventory[i].id , newItem.id ) == 0 ) {
            gotoxy( 20, 22 );
            printf("\033[1;31mAn item with the same ID already exists. Item not added.\033[0m\n");
            Sleep(2000) ;
            return;
        }
        else if( newItem.id[0] == 'D' || newItem.id[0] == 'G'|| newItem.id[0] == 'C'|| newItem.id[0] == 'S'|| newItem.id[0] == 'B' || newItem.id[0] == 'O') {
            found = 1 ; // Don't break as need to check similiar IDs
        }
    }

    if( ! found ) {
        gotoxy( 20, 24 ) ;
        printf("\033[1;31mItem unable to add cause of invalid ID-format\nTry Again!\033[0m\n") ;
        Sleep(2000) ;
        return ;
    }
        inventory[itemCount] = newItem;
        itemCount++;
        gotoxy( 20 , 22 );
        printf("\033[1;32mItem added to inventory!\033[0m\n") ;
        Sleep(1750) ;
        updateInventoryFile("inventory_data.txt") ;
}

void removeItem(char *itemId) {
    int found = 0;
    for (int i = 0; i < itemCount; ++i) {
        if ( strcmp (inventory[i].id , itemId ) == 0 ) {
            // Shift elements to fill the gap caused by removing the item
            for (int j = i; j < itemCount - 1; ++j) {
                inventory[j] = inventory[j + 1];
            }
            itemCount--;
            found = 1;
            gotoxy( 20 , 20);
            printf("\033[1;32mItem removed from inventory!\033[0m\n");
            Sleep(1750) ;
            updateInventoryFile("inventory_data.txt") ;
            break;
        }
    }

    if (!found) {
        gotoxy( 20 , 20 );
        printf("\033[1;31mItem with ID %s not found in inventory. Removal failed.\033[0m\n", itemId);
        Sleep(2000) ;
    }
}

void modifyItem(char *itemId) {
    int itemIndex = -1;

    for (int i = 0; i < itemCount; ++i) {
        if (strcmp(inventory[i].id, itemId) == 0) {
            itemIndex = i;
            break;
        }
    }

    if (itemIndex == -1) {
        gotoxy( 20 , 20 ) ;
        printf("\033[1;31mItem with ID %s not found in inventory. Update failed\033[0m\n", itemId);
        Sleep(2000);
        return;
    }

    char userChoice[20];
    gotoxy( 20 , 20 ) ;
    printf("Enter which thing to update (Quantity or Price or Both): ");
    fgets(userChoice, sizeof(userChoice), stdin);
    userChoice[strcspn(userChoice, "\n")] = '\0';

    if (strcasecmp(userChoice, "quantity") == 0) {
        int newQuantity;
        gotoxy( 20 , 22 ) ;
        printf("Enter new quantity: ");
        if (scanf("%d", &newQuantity) != 1) {
            fflush(stdin) ;
            gotoxy( 20 , 24 ) ;
            printf("\033[1;31mInvalid input for quantity. Update failed\033[0m\n");
            Sleep(2000);
            return;
        }
        inventory[itemIndex].quantity = newQuantity;
    } 
    else if (strcasecmp(userChoice, "price") == 0) {
        double newPrice;
        gotoxy( 20 , 22 ) ;
        printf("Enter new price: ");
        if (scanf("%lf", &newPrice) != 1) {
            fflush(stdin) ;
            gotoxy( 20 , 24 ) ;
            printf("\033[1;31mInvalid input for price. Update failed\033[0m\n");
            Sleep(2000);
            return;
        }
        inventory[itemIndex].price = newPrice;
    } 
    else if (strcasecmp(userChoice, "both") == 0) {
        int newQuantity;
        gotoxy( 20 , 22 ) ;
        printf("Enter new quantity: ");
        if (scanf("%d", &newQuantity) != 1) {
            fflush(stdin) ;
            gotoxy( 20 , 24 ) ;
            printf("\033[1;31mInvalid input for quantity. Update failed\033[0m\n");
            Sleep(2000);
            return;
        }
        inventory[itemIndex].quantity = newQuantity;

        double newPrice;
        gotoxy( 20 , 24 ) ;
        printf("Enter new price: ");
        if (scanf("%lf", &newPrice) != 1) {
            fflush(stdin) ;
            gotoxy( 20 , 26 ) ;
            printf("\033[1;31mInvalid input for price. Update failed\033[0m\n");
            Sleep(2000);
            return;
        }
        inventory[itemIndex].price = newPrice;
    } 
    else {
        gotoxy( 20 , 22 ) ;
        printf("\033[1;31mWrong Choice Entered!\033[0m\n");
        return;
    }

    gotoxy( 18 , 26 ) ;
    printf("\033[1;32mItem modified to %d quantity and %.2lf price!\033[0m\n", inventory[itemIndex].quantity, inventory[itemIndex].price);
    Sleep(1750);
    updateInventoryFile("inventory_data.txt");
}

// Functions for managing customers
#define MAX_CUSTOMERS 150 

struct Customer customers[MAX_CUSTOMERS]; // Array to hold all customer details
int customerCount = 0; // Variable to track the current number of customers

// Functions for managing discounted customers
#define MAX_DISCOUNTS 100 
struct Discount discounts[MAX_DISCOUNTS]; // Array to hold all discounts
int discountCount = 0; // Variable to track the current number of discounts

void loadCustomersWithDiscountsFromFile(void) {
    FILE *file = fopen("customers_data.bin", "rb");
    if (file == NULL) {
        printf("\033[1;31mError opening file %s for reading.\033[0m\n", "customers_data.bin");
        return;
    }

    fread(&customerCount, sizeof(int), 1, file); // Read the total number of customers first

    for (int i = 0; i < customerCount; ++i) {
        fread(&customers[i], sizeof(struct Customer), 1, file); // Read each customer structure
    }

    fread(&discountCount, sizeof(int), 1, file) ;

    for (int i = 0; i < customerCount; ++i) {
        fread(&discounts[i], sizeof(struct Discount), 1, file); // Read each discount structure then in sequence
    }

    fclose(file);
}

void writeCustomersWithDiscountsToFile(void) {
    FILE *file = fopen("customers_data.bin", "wb");
    if (file == NULL) {
        printf("\033[1;31mError opening file %s for writing.\033[0m\n", "customers_data.bin");
        return;
    }

    fwrite(&customerCount, sizeof(int), 1, file); // Write the total number of customers first

    for (int i = 0; i < customerCount; ++i) {
        fwrite(&customers[i], sizeof(struct Customer), 1, file); // Write each customer structure
    }

    fwrite(&discountCount, sizeof(int), 1, file) ;

    for (int i = 0; i < customerCount; ++i) {
        fwrite(&discounts[i], sizeof(struct Discount), 1, file); // Write each discount structure then in sequence
    }

    fclose(file);
}

void addCustomer(struct Customer newCustomer) {
    if (customerCount >= MAX_CUSTOMERS) {
        gotoxy(20,23);
        printf("\033[1;31mMaximum customers reached. Cannot add more customers\033[0m\n");
        Sleep(2000) ;
        return;
    }

    customers[customerCount] = newCustomer;
    customerCount++;
    gotoxy(20,23);
    printf("\033[1;32m%s Customer is allotted with ID-Number %d.\033[0m\n", newCustomer.name, newCustomer.id);
    Sleep(1750) ;
    // After adding the customer, save the updated data to the file. Otherwise it was not properly reading .
    writeCustomersWithDiscountsToFile( ); // This function should be called separately not integrated with in it otherwise not properly updating
}

void removeCustomer(int customerId) {
    int found = 0;
    for (int i = 0; i < customerCount; ++i) {
        if (customers[i].id == customerId) {
            // Shift elements to fill the gap caused by removing the customer
            for (int j = i; j < customerCount - 1; ++j) {
                customers[j] = customers[j + 1];
                customers[j].id -- ; // -1 from IDs also
            }
            customerCount--;
            found = 1;
            gotoxy( 20 , 20);
            printf("\033[1;32mCustomer with %d-ID removed!\033[0m\n", customerId );
            Sleep(1750) ;
            writeCustomersWithDiscountsToFile( ) ;
            break;
        }
    }

    if (!found) {
        gotoxy(20,20);
        printf("\033[1;31mCustomer with ID %d not found. Removal failed.\033[0m\n", customerId);
        Sleep(2000) ;
    }
}

void displayCustomers(void) {
    if (customerCount == 0) {
        gotoxy(20,16);
        printf("\033[1;31mNo customers added in the list.\033[0m\n");
        Sleep(2000) ;
        return;
    }
    int row = 5;
    gotoxy(15,2);
    printMenu("CUSTOMERS-DASHBOARD ") ;
    // printf("==========Customers Dasboard==========\n");
    // gotoxy(15,3);
    // printf("\t\t ******************\n") ;
    gotoxy(20,4);
    printf("ID |     Name     | Phone_Num\n");
    for (int i = 0; i < customerCount; ++i) {
        gotoxy( 20 , ++row );
        printf("%d  | %-12s | %s\n", customers[i].id, customers[i].name, customers[i].phoneNumber);
    }
}

// Functions for managing discounts
void addDiscount(int customerId, double discountPercentage) {
    if (discountCount >= MAX_DISCOUNTS) {
        gotoxy(20,18);
        printf("\033[1;31mMaximum number of discounts reached. Cannot add more discounts\033[0m\n");
        Sleep(2000) ;
        return; 
    }
    else if ( customerId > customerCount ) {
        gotoxy(20,18);
        printf("\033[1;31mCustomer with %d-ID not exists\033[0m\n", customerId ) ;
        Sleep(2000) ;
        return ;
    }

    // Check if the discount for the customer already exists
    for (int i = 0; i < discountCount; ++i) {
        if (discounts[i].customerId == customerId) {
            gotoxy(20,18);
            printf("\033[1;31mA discount for the same customer already exists. Discount not added\033[0m\n");
            Sleep(2000) ;
            return ;
        }
    }

    discounts[discountCount].customerId = customerId;
    discounts[discountCount].discountPercentage = discountPercentage;
    discountCount++;
    gotoxy(20,18);
    printf("\033[1;32mDiscount added for the %d-ID Customer!\033[0m\n", customerId ) ;
    Sleep(1750) ;
    writeCustomersWithDiscountsToFile( ) ;
}

void removeDiscount(int customerId) {
    int found = 0;
    for (int i = 0; i < discountCount; ++i) {
        if (discounts[i].customerId == customerId) {
            // Shift elements to fill the gap caused by removing the discount
            for (int j = i; j < discountCount - 1; ++j) {
                discounts[j] = discounts[j + 1];
            }
            discountCount--;
            found = 1;
            gotoxy( 20 , 16 );
            printf("\033[1;32mDiscount removed for the %d-ID customer!\033[0m\n", customerId );
            Sleep( 1750 ) ;
            writeCustomersWithDiscountsToFile( ) ;
            break;
        }
    }

    if (!found) {
        gotoxy(20,16);
        printf("\033[1;31mNo discount found for customer ID %d. Removal failed\033[0m\n", customerId);
        Sleep( 2000 ) ;
    }
}


double getDiscount(int customerId) {
    double discount = 0.0;
    for (int i = 0; i < discountCount; ++i) {
        if (discounts[i].customerId == customerId) {
            discount = discounts[i].discountPercentage;
            break;
        }
    }
    return discount;
}

void displayDiscounts(void) {
    int row = 10 ;
    gotoxy( 32, 5 ) ;
    printMenu("ALL DISCOUNTEES ") ;
    // printf("--- All Discounts ---\n");
    // gotoxy( 50, 6 ) ;
    // printf("    ************\n") ;

    gotoxy( 50, 8 ) ;
    printf("|Customer ID|Discount %%|\n");
    for (int i = 0; i < discountCount; ++i) {
        gotoxy( 50, row++ ) ; // Now here since not previously used so post-incremenr is better
        printf("| %-9d | %-7.2lf%% |\n", discounts[i].customerId, discounts[i].discountPercentage);
    }
}

void checkTransactions(void) {
    FILE *file = fopen("Records.txt", "r") ;

    if(file == NULL) {
        gotoxy( 20, 18 ) ;
        printf("\033[1;31mError opening file %s for writing.\033[0m\n", "Records.bin");
        return;
    }

    char info ;
    while( ( info = fgetc(file) ) != EOF ) {
        printf("%c", info ) ;
    }
}

// Functions for admin interface
struct Admin adminUser = {"admin", "pass123"}; // Sample password


int authenticateAdmin(char *username, char *password) {
    if (strcmp(username, adminUser.username) == 0 && strcmp(password, adminUser.password) == 0) 
        return 1; 
    else
        return 0; 
}

// Functions for managing inventory in the admin menu
void manageInventory(void) {
    int choice;
    do {
        gotoxy(15,3);
        printf("========Inventory Management========\n");
        gotoxy(20,4);
        printf("1. Add Item\n");
        gotoxy(20,6);
        printf("2. Remove Item\n");
        gotoxy(20,8); 
        printf("3. Modify Item\n");
        gotoxy(20,10); 
        printf("4. Display Inventory\n");
        gotoxy(20,12); 
        printf("5. Display All Transactions\n");
        gotoxy(20,14); 
        printf("0. Back to Admin Menu\n");
        gotoxy(20,16); 
        printf("Enter your choice: ");
        scanf("%d", &choice);
        fflush( stdin ) ;
        
        switch (choice) {
            case 1: {
                system("cls");
                struct Item newItem;
                gotoxy(12,3);
                printf("\033[1;31mInstructions:-\033[0m");
                gotoxy(15,4); 
                printf("ID must follow these prefixes for specific sections in the format: 1stFixedChar(NUM or CHAR)_NUM ") ;
                gotoxy(15,5); 
                printf("For Dairy (D) section: ID should be like DD_188\n");
                gotoxy(15,6);
                printf("For Cooking Products (G or C) section: ID should be like G/C_NUM\n");
                gotoxy(15,7);
                printf("For Snacks (S) section: ID should be like S_NUM\n");
                gotoxy(15,8);
                printf("For Beverages (B) section: ID should be like B_NUM\n");
                gotoxy(15,9);
                printf("For Other Products (O) section: ID should be like O_NUM\n") ;
                gotoxy(20,14);
                printf("Enter item ID: ");
                fgets(newItem.id, sizeof(newItem.id), stdin ) ;
                newItem.id[ strcspn( newItem.id, "\n" ) ] = '\0' ;
                gotoxy(20,16);
                printf("Enter item name: ");
                fgets(newItem.name, sizeof(newItem.name), stdin ) ;
                newItem.name[ strcspn( newItem.name, "\n" ) ] = '\0' ;
                gotoxy(20,18);
                printf("Enter item price: ");
                scanf("%lf", &newItem.price);
                fflush(stdin) ;
                gotoxy(20,20);
                printf("Enter item quantity: ");
                scanf("%d", &newItem.quantity);
                fflush(stdin) ;
                addItem(newItem) ;
                system("cls") ;
                break;
            }
            case 2: {
                char itemId[20] ;
                gotoxy(20,18); 
                printf("Enter item ID to remove: ");
                fgets(itemId, sizeof(itemId), stdin) ;
                itemId[ strcspn( itemId, "\n" ) ] = '\0' ;
                removeItem(itemId);
                system("cls") ;
                break;
            }
            case 3: {
                char itemId[20] ;
                int newQuantity ;
                double newPrice ;
                gotoxy(20,18);
                printf("Enter item ID to update: ");
                fgets(itemId, sizeof(itemId), stdin) ;
                itemId[ strcspn( itemId, "\n" ) ] = '\0' ;
                modifyItem(itemId);
                system("cls") ;
                break;
            }
            case 4:
                system("cls") ;
                displayInventory();
                printf("\n\033[1;33;5mPress any key to exit viewing.\033[0m\n") ;
                getch( ) ;
                system("cls") ;
                break;
            case 5: 
                system("cls") ;
                checkTransactions() ;
                printf("\n\033[1;33;5mPress any key to go back.\033[0m\n") ;
                getch( ) ;
                fflush(stdin) ;
                system("cls") ;
            case 0:
                gotoxy(20,19);
                printf("Returning to Admin Menu...\n");
                Sleep(1750) ;
                system("cls");
                break;
            default:
                fflush(stdin) ;
                gotoxy(20,19);
                printf("\033[1;31mInvalid choice! Please enter a valid option..\033[0m\n");
                break ;
        }
    } while (choice != 0);
}

// Functions for managing customers in the admin menu
void manageCustomers(void) {
    int choice;
    do {
        gotoxy(15,3);
        printf("========Customer Management========");
        gotoxy(20,6);
        printf("1. Add Customer\n");
        gotoxy(20,8);
        printf("2. Remove Customer\n");
        gotoxy(20,10);
        printf("3. Display Customers\n");
        gotoxy(20,12);
        printf("0. Back to Admin Menu\n");
        gotoxy(20,14);
        printf("Enter your choice: ") ;
        scanf("%d", &choice);
        fflush(stdin) ;

        switch (choice) {
            case 1: {
                struct Customer newCustomer;
                newCustomer.id = customerCount + 1 ;
                gotoxy(20,17);
                printf("Enter customer name: ");
                gotoxy(20,19);
                fgets(newCustomer.name, sizeof(newCustomer.name), stdin ) ;
                newCustomer.name[ strcspn( newCustomer.name,"\n" ) ] = '\0' ;
                gotoxy( 20 , 21 ) ;
                printf("Enter customer phone_number: ") ;
                fgets(newCustomer.phoneNumber, sizeof(newCustomer.phoneNumber), stdin ) ;
                newCustomer.phoneNumber[ strcspn( newCustomer.phoneNumber, "\n" ) ] = '\0' ;

                for(int i = 0; i < customerCount; ++i) {
                    if( strcmp ( newCustomer.phoneNumber , customers[i].phoneNumber ) == 0 ) {
                        gotoxy(25, 24) ;
                        printf("\033[1;31m%s Phone_Number is already registered.\n\t\t\t\tPlease Try Again!\033[0m\n", newCustomer.phoneNumber ) ;
                        Sleep( 2000 ) ;
                        system("cls") ;
                        return ;
                    }
                }
                addCustomer(newCustomer);
                system("cls") ;
                break;
            }
            case 2: {
                int customerId;
                gotoxy(20,17);
                printf("Enter customer ID to remove: ");
                scanf("%d", &customerId);
                fflush(stdin) ;
                removeCustomer(customerId);
                system("cls") ;
                break;
            }
            case 3:
                system("cls") ;
                displayCustomers();
                printf("\n\033[1;33;5mPress Any Key to Go back to Management menu:\033[0m\n");
                getch( ) ;
                system("cls") ;
                break;
            case 0:
                gotoxy(20,17);
                printf("Returning to Admin Menu.\n");
                Sleep(1750) ;
                system("cls") ;
                break;
            default:
                fflush(stdin) ;
                gotoxy(20,17);
                system("cls") ;
                printf("\033[1;31mInvalid choice! Please enter a valid option.\033[0m\n");
        }
    } while (choice != 0);
}

// Functions for managing discounts in the admin menu
void manageDiscounts(void) {
    int choice;
    do {
        gotoxy(12,2);
        printf("========Discount Management========");
        gotoxy(20,4);
        printf("1. Add Discount\n");
        gotoxy(20,6);
        printf("2. Remove Discount\n");
        gotoxy(20,8);
        printf("3. Display Customer Discount List\n");
        gotoxy(20, 10);
        printf("0. Back to Admin Menu\n");
        gotoxy(20,12);
        printf("Enter your choice: ");
        scanf("%d", &choice);
        fflush( stdin ) ;

        switch (choice) {
            case 1: {
                int customerId;
                double discountPercentage;
                gotoxy(20,14);
                printf("Enter customer ID for discount: ");
                scanf("%d", &customerId);
                fflush( stdin ) ;
                gotoxy(20,16);
                printf("Enter discount percentage: ");
                scanf("%lf", &discountPercentage);
                fflush( stdin ) ;
                addDiscount(customerId, discountPercentage);
                system("cls") ;
                break;
            }
            case 2: {
                int customerId;
                gotoxy(20,14);
                printf("Enter customer ID to remove discount: ");
                scanf("%d", &customerId);
                fflush( stdin ) ;
                removeDiscount(customerId);
                system("cls") ;
                break;
            }
            case 3:
                system("cls") ;
                displayDiscounts();
                printf("\n\033[1;33;5mPress any key to go back.\033[0m\n") ;
                getch( ) ;
                system("cls") ;
                break;
            case 0:
                gotoxy(20,14);
                printf("Returning to Admin Menu.\n") ;
                Sleep(1750) ;
                system("cls") ;
                break;
            default:
                fflush( stdin ) ;
                gotoxy(20,14);
                printf("\033[1;31mInvalid choice! Please enter a valid option.\033[0m\n");
                Sleep(2000) ;
                system("cls") ;
        }
    } while (choice != 0);
}

void changeAdminKeys( void ) {
    char existingPass[50] ;
    char newUsername[50] ;
    char newPassword[50] ;
    gotoxy( 20, 4 );
    printf("Enter the previous password for admin : ") ;
    
    int i = 0;
        while (i < 49) { 
            if (_kbhit()) {
                char ch = _getch() ;
                if (ch == 13) { 
                    existingPass[i] = '\0'; 
                    break;
                } else if (ch == 8) { 
                    if (i > 0) {
                        printf("\b \b"); 
                        i--;
                    }
                } else {
                    existingPass[i] = ch;
                    printf("*"); 
                    i++;
                }
            }
        }

    gotoxy( 0 , 7 ) ;
    printf("PLEASE WAIT!!\n\n\t\t\033[1;33;5mVALIDATING CREDENTIALS...\033[0m\n") ;
    Sleep(2250) ;
    if ( strcmp (existingPass, adminUser.password) == 0 ) {
        gotoxy(15, 9) ;
        printf("\033[1;31mNote:-\033[0mChange Password At your own risk.\n") ;
        gotoxy(15, 11) ;
        printf("Enter new admin username: ");
        fgets(newUsername, sizeof(newUsername), stdin ) ;
        newUsername[ strcspn(newUsername, "\n") ] = '\0' ;
        gotoxy(15, 13) ;
        printf("Enter new admin password: ");
        fgets(newPassword, sizeof(newPassword), stdin ) ;
        newPassword[ strcspn(newPassword, "\n") ] = '\0' ;

        // Update admin credentials
        strcpy(adminUser.username, newUsername);
        strcpy(adminUser.password, newPassword);

        gotoxy(20, 18) ;
        printf("\033[1;32mAdmin credentials updated successfully!\033[0m\n");
    } 
    else {
        gotoxy(20, 18) ;
        printf("\n\033[1;31mSecurity-Alert Sent to Server!\033[0m") ;
        // Playing siren like sound via Beep
        int initialFrequency = 1000 ;
        for (int i = 0; i < 20; ++i) {
            Beep(initialFrequency, 200);
            initialFrequency += 50; 
            Sleep(100);             
        }
        exit(1) ;
    }
}

void adminMenu(void) {
    char username[50];
    char password[50];
    int choice;

    system("cls");
    gotoxy(20,5);
    printMenu( "Authentication For Admin ") ;
    // printf("========Authentication For Admin========");
    // gotoxy(18,6);
    // printf("*********************************************\n");
    gotoxy(24,8);
    printf("Enter Admin Username: ");
    fgets(username, sizeof(username), stdin) ;
    username[ strcspn( username, "\n" ) ] = '\0'; 
    fflush(stdin);
    gotoxy(24,10);
    printf("Enter Admin Password: ");
    
    int i = 0;
    while (i < 49) { 
        if (_kbhit()) {
            char ch = _getch() ;
            if (ch == 13) { // If character is not ENTER KEY (\r For Conio) then
                password[i] = '\0';
                break;
            } else if (ch == 8) { // Backspace
                if (i > 0) {
                    printf("\b \b"); // Move cursor back, erase character, move cursor back again
                    i--;
                }
            } else {
                password[i] = ch;
                printf("*"); // Masking the input
                i++;
            }
        }
    }

    gotoxy( 0, 14 ) ;
    printf("PLEASE WAIT....\n\nYOUR DATA IS BEING PROCESSED....");
    Sleep( 1500 ) ;

    if (authenticateAdmin(username, password)) {
        system("cls");
        gotoxy(10,3);
        printf("\033[1;32m========Authentication successful! Welcome, Admin========\033[0m\n");
        do {
           gotoxy(20,5);
            printf("========Admin Menu========");
            gotoxy(20,7);
            printf("1. Manage Inventory");
            gotoxy(20,9);
            printf("2. Manage Customers\n");
            gotoxy(20,11);
            printf("3. Manage Discounts\n");
            gotoxy(20,13);
            printf("4. Change Admin Credentials\n");
            gotoxy(20,15);
            printf("5. Exit Admin Menu\n");
            gotoxy(20,17); 
            printf("Enter your choice: ");
            scanf("%d", &choice);
            fflush( stdin ) ;

            switch (choice) {
                case 1:
                    system("cls") ;
                    manageInventory() ;
                    break;
                case 2:
                    system("cls") ;
                    manageCustomers() ;
                    break;
                case 3:
                    system("cls") ;
                    manageDiscounts() ;
                    break;
                case 4:
                    system("cls") ;
                    changeAdminKeys() ;
                    Sleep(1750) ;
                    system("cls") ;
                    break ;
                case 5:
                    gotoxy(20,20);
                    printf("Exiting Admin Menu.\n");
                    Sleep(1750) ;
                    system("cls") ;
                    break;
                default:
                    fflush(stdin) ;
                    gotoxy(20,20);
                    printf("\033[1;31mInvalid choice! Please enter a valid option\033[0m\n");
                    break ;
            }
        } while (choice != 5);
    } 
    else {
        gotoxy(15,18);
        printf("\033[1;31m!!!!!Authentication failed! Access denied!!!!!.\033[0m\n") ;
        Sleep(1750) ;
        system("cls");
    }
}

// Function to store checkout records in a file with date and time
void storeCheckoutRecords(struct Item *cart, int cartItemCount, char *name, int customerId , double *checkoutPrices ) {
    FILE *file = fopen("Records.txt", "a");
    if (file == NULL) {
        printf("Error opening file for storing records.\n");
        return;
    }

    // Get current date and time
    time_t currentTime;
    struct tm *localTime;
    time(&currentTime);
    localTime = localtime(&currentTime);

    double totalPrice = 0.0 ;
    if ( getDiscount( customerId ) > 0 ) {
        fprintf(file, "\n  (Old SignedUp Customer) Name : %s", name ) ;
        // Write timestamp to file
        fprintf(file, "\t\tCheckout Date and Time: %s", asctime(localTime));

        // Write cart details to file
        fprintf(file, "ID     |     Name     |  Price  | Quantity\n");
        for (int i = 0; i < cartItemCount; ++i) {
            fprintf(file, "%-6s | %-12s | %-7.2lf | %-3d\n", cart[i].id, cart[i].name, checkoutPrices[i] , cart[i].quantity);
            totalPrice += checkoutPrices[i] ;
        }
            fprintf(file, "\t\t\tTotal Purchase Price = %lf (With %lf%% Discount)\n", totalPrice, getDiscount(customerId) ) ;
    } 
    else {
        fprintf(file, "\n  (New Customer ) Name : %s", name ) ;
        fprintf(file, "\t\tCheckout Date and Time: %s", asctime(localTime));

        fprintf(file, "ID     |     Name     |  Price  | Quantity\n");
        for (int i = 0; i < cartItemCount; ++i) {
            fprintf(file, "%-6s | %-12s | %-7.2lf | %-3d\n", cart[i].id, cart[i].name, cart[i].price, cart[i].quantity);
            totalPrice += checkoutPrices[i] ;
        }
            fprintf(file, "\t\t\tTotal Purchase Price = %lf\n", totalPrice ) ;
    }
    fprintf(file, "------------------------------------\n");

    fclose(file);
}

// Function for shopping
void shopping(struct Customer customer) {
    int choice;
    int customerId = customer.id;
    double discount = getDiscount(customerId); // Get customer's discount
    struct Item cart[MAX_ITEMS]; // Array to hold items in the cart
    int cartItemCount = 0; // Variable to track the number of items in the cart
    char *NewCustomer = customer.name ;

    do {
        gotoxy(30, 3);
        printMenu("SHOPPING MENU ") ;
        // printf("Shopping-Menu:\n");
        // gotoxy(25, 4);
	    // printf("_____________") ;
        gotoxy( 20, 7 ) ;
        printf("1. Display Whole-Inventory\n");
        gotoxy( 20, 9 ) ;
        printf("2. Display Section Wise Items\n") ;
        gotoxy( 20, 11 ) ;
        printf("3. Add Item to Cart\n");
        gotoxy( 20, 13 ) ;
        printf("4. Remove Item from Cart\n");
        gotoxy( 20, 15 ) ;
        printf("5. Display Cart\n");
        gotoxy( 20, 17 ) ;
        printf("6. Checkout\n");
        gotoxy( 20, 19 ) ;
        printf("0. Exit Shopping\n");
        gotoxy( 20, 21 ) ;
        printf("\nEnter your choice: ");
        scanf("%d", &choice);
        fflush(stdin);

        switch (choice) {
            case 1:
                system("cls") ;
                displayInventory();
                // 33 for yellow color and 5 for blinking effect
                printf("\n\033[1;33;5mPress any key to exit viewing.\033[0m\n") ;
                getch( ) ;
                system("cls") ;
                break;
            case 2: {
                system("cls") ;
                int category_choice ;
                gotoxy( 10, 1 ) ;
                printMenu("SECTION-WISE INVENTORY ") ;
                // printf("Section-Wise Inventory\n") ;
                // gotoxy( 10, 2 ) ;
	            // printf("______________________\n\n") ;
                printf("\n\n1. Dairy\n\n") ;
                printf("2. Grains_and_cooking_products\n\n") ;
                printf("3. Snacks\n\n") ;
                printf("4. Beverages\n\n") ;
                printf("5. Others Section\n\n") ;
                printf("Enter the option_number: ") ;
                scanf("%d", &category_choice ) ;
                fflush( stdin ) ;
                if( category_choice == 1 ) {
                    printf("\n\tDairy Inventory:\n") ;
                    printf("ID     |     Name     |  Price  | Quantity\n") ;
                    for (int i = 0; i < itemCount; i++) {
                        if ( inventory[i].id[0] == 'D')
                        printf("%-6s | %-12s | %-7.2lf | %-3d\n", inventory[i].id, inventory[i].name, inventory[i].price, inventory[i].quantity);
                    }
                }
                else if( category_choice == 2 ) {
                    printf("\n\tGrains Inventory:\n") ;
                    printf("ID     |     Name     |  Price  | Quantity\n") ;
                    for (int i = 0; i < itemCount; i++) {
                        if ( inventory[i].id[0] == 'G' || inventory[i].id[0] == 'C' )
                            printf("%-6s | %-12s | %-7.2lf | %-3d\n", inventory[i].id, inventory[i].name, inventory[i].price, inventory[i].quantity);
                    }
                }
                else if( category_choice == 3 ) {
                    printf("\n\tSnacks Inventory:\n") ;
                    printf("ID     |     Name     |  Price  | Quantity\n") ;
                    for (int i = 0; i < itemCount; i++) {
                        if ( inventory[i].id[0] == 'S')
                            printf("%-6s | %-12s | %-7.2lf | %-3d\n", inventory[i].id, inventory[i].name, inventory[i].price, inventory[i].quantity);
                    }
                }
                else if( category_choice == 4 ) {
                    printf("\n\tBeverages Inventory:\n") ;
                    printf("ID     |     Name     |  Price  | Quantity\n") ;
                    for (int i = 0; i < itemCount; i++) {
                        if ( inventory[i].id[0] == 'B')
                            printf("%-6s | %-12s | %-7.2lf | %-3d\n", inventory[i].id, inventory[i].name, inventory[i].price, inventory[i].quantity);
                    }
                }
                else if( category_choice == 5 ) {
                    printf("\n\tOthers Inventory:\n") ;
                    printf("ID     |     Name     |  Price  | Quantity\n") ;
                    for (int i = 0; i < itemCount; i++) {
                        if ( inventory[i].id[0] == 'O')
                            printf("%-6s | %-12s | %-7.2lf | %-3d\n", inventory[i].id, inventory[i].name, inventory[i].price, inventory[i].quantity);
                    }
                }
                else {
                    printf("\033[1;31mInvalid choice!\033[0m\n ") ;
                }
                printf("\n\033[1;33;5mEnter any key to go back to shopping...\033[0m\n") ;
                getch( ) ;
                system("cls") ;
                break ;
            }
            case 3: { // Can declare varaibles in case-switch with delimitters
                char itemId[25] ; 
                int quantity;
                printf("\nEnter item ID to add to cart: ");
                fgets( itemId, sizeof(itemId), stdin ) ;
                itemId[ strcspn( itemId, "\n" ) ] = '\0' ;

                printf("\nEnter quantity: ");
                scanf("%d", &quantity);
                fflush( stdin ) ;
                // Find the item in inventory
                int itemIndex = -1;
                for (int i = 0; i < itemCount; ++i) {
                    if ( strcmp ( inventory[i].id , itemId ) == 0 ) {
                        itemIndex = i;
                        break;
                    }
                }

                if (itemIndex != -1 && inventory[itemIndex].quantity >= quantity) {
                    double totalPrice;
                    if ( discount > 0  ) {
                        // Calculate discounted price for existing customer
                        double discountedPrice = inventory[itemIndex].price * (1.0 - (discount / 100.0));
                        totalPrice = discountedPrice * quantity;
                    } 
                    else {
                        // Regular price for new customers (no discount)
                        totalPrice = inventory[itemIndex].price * quantity;
                    }

                    // Add item to the cart
                    cart[cartItemCount] = inventory[itemIndex];
                    cart[cartItemCount].quantity = quantity;
                    cartItemCount++;

                    // Display item details and total price
                    printf("\nAdded to Cart:\n");
                    printf("Item: %s | Quantity: %d | Total Price: %.2lf\n", inventory[itemIndex].name, quantity, totalPrice);
                    getch( ) ;
                    system("cls") ;

                    // Previously Used to Update inventory quantity after individual purchases
                    // inventory[itemIndex].quantity -= quantity;

                } 
                else {
                    system("cls") ;
                    printf("\033[1;31mItem not found or insufficient quantity in inventory.\033[3;32m(Max-%d Quantity In Stock).\033[0m\n", inventory[itemIndex].quantity);
                }
                break;
            }

            case 4: {
                if (cartItemCount == 0) {
                    system("cls") ;
                    printf("\n\033[1;31mCart is empty. Unable to remove.\033[0m\n");
                } 
                else {
                    char itemIdToRemove[25];
                    printf("\nEnter item ID to remove from cart: ");
                    fgets(itemIdToRemove, sizeof(itemIdToRemove), stdin);
                    itemIdToRemove[strcspn(itemIdToRemove, "\n")] = '\0';

                    int removeIndex = -1;
                    for (int i = 0; i < cartItemCount; ++i) {
                        if (strcmp(cart[i].id, itemIdToRemove) == 0) {
                            removeIndex = i;
                            break;
                        }
                    }

                    system("cls") ;
                    if (removeIndex != -1) {
                        // Shift elements to remove the specified item from the cart
                        for (int i = removeIndex; i < cartItemCount - 1; ++i) {
                            cart[i] = cart[i + 1];
                        }
                        cartItemCount--;

                        printf("\n\033[1;32m%s-Item removed from cart successfully.\033[0m\n", itemIdToRemove );
                    } 
                    else {
                        printf("\n\033[1;31m%s Item not found in the cart.\033[0m\n", itemIdToRemove );
                    }
                }
                break;
            }

            case 5: {
                if (cartItemCount == 0) {
                    printf("\n\n\033[1;31mCart is empty.\033[0m\n");
                } 
                else {
                    int row = 29 ;
                    gotoxy( 55, 25 ) ;
                    printf("Cart Contents:\n");
                    gotoxy( 55, 26 ) ;
                    printf("______________\n") ;
                    gotoxy( 45, row ) ;
                    printf("ID     |     Name     |  Price  | Quantity\n") ;
                    double totalCartPrice = 0.0;
                    for (int i = 0; i < cartItemCount; ++i) {
                        gotoxy( 45, ++row ) ;
                        double itemPrice ;
                        if ( discount > 0 ) {
                            double discountedPrice = cart[i].price * (1.0 - (discount / 100.0));
                            itemPrice = discountedPrice * cart[i].quantity ;
                            printf("%-6s | %-12s | %-7.2lf | %-3d\n", cart[i].id, cart[i].name, itemPrice, cart[i].quantity);
                            totalCartPrice += itemPrice ;
                        }
                        else {
                            itemPrice = cart[i].price * cart[i].quantity;
                            printf("%-6s | %-12s | %-7.2lf | %-3d\n", cart[i].id, cart[i].name, cart[i].price, cart[i].quantity);
                            totalCartPrice += cart[i].price ;
                        }
                    }
                    printf("\033[1;33;5mTotal Cart Price: %.2lf\nPress any key to move back.\033[0m\n", totalCartPrice) ;
                    getch( ) ;
                    system("cls") ;
                }
                break;
            }
            case 6: {
                system("cls") ;
                if (cartItemCount == 0) {
                    printf("\033[1;31mCart is empty. Unable to checkout.\033[0m\n");
                } 
                else {
                    int row = 5 ;
                    printf("\nCheckout Process:\n");
                    printf("\nItems Purchased are:\n");
                    gotoxy( 15, row ) ;
                    printf("ID     |     Name     |  Price  | Quantity\n") ;
                    double totalCheckoutPrice = 0.0;
                    double checkoutPrices[cartItemCount] ;
                    for (int i = 0; i < cartItemCount; ++i) {
                        gotoxy( 15 , ++row ) ;
                        if ( discount > 0 ) {
                            double discountedPrice = cart[i].price * (1.0 - (discount / 100.0));
                            checkoutPrices[i] = discountedPrice * cart[i].quantity ;
                            printf("%-6s | %-12s | %-7.2lf | %-3d\n", cart[i].id, cart[i].name, checkoutPrices[i], cart[i].quantity);
                        }
                        else {
                            checkoutPrices[i] = cart[i].price * cart[i].quantity;
                            printf("%-6s | %-12s | %-7.2lf | %-3d\n", cart[i].id, cart[i].name, cart[i].price, cart[i].quantity);
                        }
                        totalCheckoutPrice += checkoutPrices[i] ;
                    }
                    gotoxy(0 , ++row ) ;
                    if( discount > 0 ) 
                        printf("\n\033[1;31mTotal Checkout Price(With %.2lf%%-Discount): %.2lf\033[0m\n",discount, totalCheckoutPrice) ;
                    else 
                        printf("\n\033[1;31mTotal Checkout Price: %.2lf\033[0m\n", totalCheckoutPrice) ;

                    // Perform checkout - Reset the cart
                    gotoxy(0 , ++row ) ;
                    printf("\n\033[1;33;5mCheckout completed. Thanks you for shopping!\nWe hope to see you again soon!\033[0m(Regards Developer Obaid)\n") ;
                    getch( ) ;
                    system("cls") ;
                    for (int i = 0; i < cartItemCount; ++i) {
                        // Update quantity in inventory after purchase
                        for (int j = 0; j < itemCount; ++j) {
                            if ( strcmp( cart[i].id , inventory[j].id ) == 0 ) {
                                inventory[j].quantity -= cart[i].quantity;
                                break;
                            }
                        }
                    }
                    // Update the inventory file with the modified quantities
                    updateInventoryFile("inventory_data.txt");
                    // Store checkout records with date and time into Records.txt
                    storeCheckoutRecords(cart, cartItemCount, NewCustomer, customerId , checkoutPrices );

                    cartItemCount = 0; // Reset cart
                }
                break;
            }
            case 0:
                printf("\033[1;32mExiting Shopping...\033[0m\n");
                Sleep( 1750 ) ;
                system("cls") ;
                break;
            default:
                fflush(stdin) ;
                system("cls") ;
                printf("\n\033[1;31mInvalid choice! Please enter a valid option.\033[0m\n");
        }
    } while (choice != 0);
}

int main() {
    struct Customer newCustomer;
    char customerPhoneNum[20] ;
    int pin;
    int choice;
    loadCustomersWithDiscountsFromFile( ) ;
    loadInventoryFromFile("inventory_data.txt");

    do {
        // gotoxy(20, 3);
        // printf("========= GENERAL STORE MANAGEMENT SYSTEM =========\n");
        // gotoxy(20, 4);
	    // printf("\t      ********************************\n");
        gotoxy(13, 2) ;
        printMenu("GENERAL STORE MANAGEMENT SYSTEM ") ;
        gotoxy(35, 7) ;
	    printf("DEVELOPED BY M.OBAID");
        gotoxy(23, 10);
        printf("1. New Customer Shopping\n");
        gotoxy(23, 12);
        printf("2. Existing Customer Shopping\n");
        gotoxy(23, 14);
        printf("3. Admin Menu\n");
        gotoxy(23, 16);
        printf("0. Exit\n");
        gotoxy(23, 18);
        printf("\nEnter your choice: ");
        scanf("%d", &choice);
        fflush(stdin) ; // Used flush buffer where necessary to remove white spaces or new lines wrongly given in integer to avoid code breakage

        switch (choice) {
            case 1:
                system("cls") ;
                printf("\n\n!!CREATING NEW CUSTOMER ACCOUNT!!") ;
                newCustomer.id = customerCount + 1 ;
                printf("\n\nEnter customer name: ");
                fgets(newCustomer.name, sizeof(newCustomer.name), stdin ) ;
                newCustomer.name[ strcspn( newCustomer.name, "\n" ) ] = '\0' ;
                int check = 1 ;
                printf("\n\nEnter customer phone number: ");
                fgets(newCustomer.phoneNumber, sizeof(newCustomer.phoneNumber), stdin ) ;
                newCustomer.phoneNumber[ strcspn( newCustomer.phoneNumber, "\n" ) ] = '\0' ;
                fflush( stdin ) ;
                for(int i = 0; i < strlen( newCustomer.phoneNumber); ++i) {
                    if( ! ( newCustomer.phoneNumber[i] >= '0' && newCustomer.phoneNumber[i] <= '9' || ( newCustomer.phoneNumber[0] == '+' || newCustomer.phoneNumber[i] == '-' ) ) ) { 
                        check = 0  ;
                        break ;
                    }
                }

                if( check == 1 ) {
                    char c ;
                    printf("\n\nSet PIN for customer authentication(Only Numbers): ");
                    while( ( scanf("%d", &newCustomer.pin) ) != 1 ) {
                        printf("\033[1;31mInvalid input.Please enter a valid number.\033[0m\n") ;
                        while(( c = getchar() )!= '\n' && c != EOF ) ;
                        fflush( stdin );
                    }

                    for(int i = 0; i < customerCount; ++i) {
                        if( strcmp ( newCustomer.phoneNumber , customers[i].phoneNumber ) == 0 ) {
                            gotoxy(15, 18) ;
                            // ANSI escape code for red text
                            printf("\033[1;31m%s Phone_Number is already registered.\n\t\t\t\tPlease Try Again!\033[0m\n", newCustomer.phoneNumber ) ;
                            return 1 ;
                        }
                    }

                    system("cls");
                    printf("PLEASE WAIT....\n\nYOUR DATA IS BEING PROCESSED....");
                    Sleep( 1500 ) ;

                    gotoxy(30, 10);

                    printf("ACCOUNT CREATED SUCCESSFULLY....");
                    gotoxy(0, 20);
                    addCustomer(newCustomer);
                    printf("Press enter to login");

                    getch();
                    // Perform shopping for the new customer
                    system("cls") ;
                    shopping(newCustomer) ;
                }
                else {
                    printf("\033[1;31mPhone Number Format is wrong !\nCan start with (+ code) and contain hypens(-) but now other alphabets or characters.\033[0m\n") ;
                    Sleep(2250) ;
                    system("cls") ;
                }
                break;
            case 2:
                system("cls") ;
                printf("\n\n!!LOGGING IN CURRENT CUSTOMER!!") ;
                printf("\n\nEnter customer phone number: ");
                fgets(customerPhoneNum, sizeof(customerPhoneNum), stdin) ;
                customerPhoneNum[ strcspn( customerPhoneNum, "\n" ) ] = '\0' ;
                fflush( stdin ) ;
                printf("\n\nEnter PIN for authentication: ");
                scanf("%d", &pin);
                fflush( stdin );

                // Authenticate existing customer
                int customerIndex = -1;
                for (int i = 0; i < customerCount; ++i) {
                    if ( ( strcmp ( customers[i].phoneNumber , customerPhoneNum ) == 0 ) && customers[i].pin == pin) {
                        customerIndex = i;
                        break;
                    }
                }

                system("cls");
                printf("PLEASE WAIT....\n\nFETCHING ACCOUNT DETAILS....");
                Sleep( 1500 ) ;
                if (customerIndex != -1) {
                    gotoxy(30, 10);
                    printf("\033[1;32mAuthentication successful! Welcome back, %s.\033[0m\n", customers[customerIndex].name);
                    gotoxy(0 , 20) ;
                    printf("Enter any key to login ...\n") ;
                    getch() ;
                    system("cls") ;
                    // Perform shopping for the existing customer
                    shopping(customers[customerIndex]);
                } 
                else {
                    system("cls") ;
                    gotoxy(0, 21) ;
                    printf("\033[1;31mAuthentication failed! Access denied.\033[0m\n");
                }
                break;
            case 3:
                adminMenu();
                break;
            case 0:
                printf("\nThanks For Using Our App.\nRegards Developer Obaid and Admin Interface Formatter Akhyar\nExiting...\n");
                break;
            default:
                system("cls") ;
                fflush(stdin) ;
                gotoxy(0, 21) ;
                printf("\033[1;31mInvalid choice! Please enter a valid option.\033[0m\n");
        }
    } while (choice != 0);

    return 0;
}