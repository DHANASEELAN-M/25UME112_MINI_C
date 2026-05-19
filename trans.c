// Bank-account program (Ultra-Efficient In-Memory Version)
// Reads a random-access file into memory sequentially,
// updates data in RAM, creates new data in RAM, and deletes data in RAM.
// Persists the entire RAM array back to disk sequentially on program exit.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RECORDS 100

// clientData structure definition
struct clientData
{
    unsigned int acctNum; // account number
    char lastName[15];    // account last name
    char firstName[10];   // account first name
    double balance;       // account balance
};

// prototypes
unsigned int enterChoice(void);
void textFile(const struct clientData clients[]);
void updateRecord(struct clientData clients[]);
void newRecord(struct clientData clients[]);
void deleteRecord(struct clientData clients[]);
void listAccounts(const struct clientData clients[]);
void searchByLastName(const struct clientData clients[]);
void calculateTotalBalance(const struct clientData clients[]);
void clearInputBuffer(void);

int main(int argc, char *argv[])
{
    struct clientData clients[MAX_RECORDS];
    FILE *cfPtr;
    unsigned int choice;

    // Load database into memory (Extremely efficient - 1 bulk read)
    memset(clients, 0, sizeof(clients));
    if ((cfPtr = fopen("credit.dat", "rb")) != NULL)
    {
        fread(clients, sizeof(struct clientData), MAX_RECORDS, cfPtr);
        fclose(cfPtr);
        printf("Database loaded successfully into memory.\n");
    }
    else
    {
        printf("File could not be opened, starting with a fresh in-memory database.\n");
    }

    // enable user to specify action in-memory
    while ((choice = enterChoice()) != 8)
    {
        switch (choice)
        {
        case 1:
            textFile(clients);
            break;
        case 2:
            updateRecord(clients);
            break;
        case 3:
            newRecord(clients);
            break;
        case 4:
            deleteRecord(clients);
            break;
        case 5:
            listAccounts(clients);
            break;
        case 6:
            searchByLastName(clients);
            break;
        case 7:
            calculateTotalBalance(clients);
            break;
        case 0:
            puts("Incorrect choice. Please enter a number.");
            break;
        default:
            puts("Incorrect choice");
            break;
        } // end switch
    }     // end while

    // Save database back to disk on exit (Extremely efficient - 1 bulk write)
    if ((cfPtr = fopen("credit.dat", "wb")) != NULL)
    {
        fwrite(clients, sizeof(struct clientData), MAX_RECORDS, cfPtr);
        fclose(cfPtr);
        printf("Database successfully saved to disk.\n");
    }
    else
    {
        printf("Error: Could not save database to disk.\n");
    }

    return 0;
} // end main

// utility to clear input buffer
void clearInputBuffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// create formatted text file for printing
void textFile(const struct clientData clients[])
{
    FILE *writePtr; // accounts.txt file pointer

    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        puts("File could not be opened.");
    }
    else
    {
        fprintf(writePtr, "%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");

        for (size_t i = 0; i < MAX_RECORDS; ++i)
        {
            if (clients[i].acctNum != 0)
            {
                fprintf(writePtr, "%-6u%-16s%-11s%10.2f\n", clients[i].acctNum, clients[i].lastName, clients[i].firstName, clients[i].balance);
            }
        }

        fclose(writePtr); // fclose closes the file
        printf("Data successfully exported to accounts.txt\n");
    }
} // end function textFile

// update balance in record
void updateRecord(struct clientData clients[])
{
    unsigned int account; // account number
    double transaction;   // transaction amount

    printf("Enter account to update ( 1 - 100 ): ");
    if (scanf("%u", &account) != 1) {
        clearInputBuffer();
        printf("Invalid input. Please enter a valid number.\n");
        return;
    }

    if (account < 1 || account > MAX_RECORDS)
    {
        printf("Invalid account number.\n");
        return;
    }

    if (clients[account - 1].acctNum == 0)
    {
        printf("Account #%u has no information.\n", account);
    }
    else
    {
        printf("%-6u%-16s%-11s%10.2f\n\n", clients[account - 1].acctNum, clients[account - 1].lastName, clients[account - 1].firstName, clients[account - 1].balance);

        printf("Enter charge ( + ) or payment ( - ): ");
        if (scanf("%lf", &transaction) != 1) {
            clearInputBuffer();
            printf("Invalid transaction amount.\n");
            return;
        }
        clients[account - 1].balance += transaction; // update record balance

        printf("%-6u%-16s%-11s%10.2f\n", clients[account - 1].acctNum, clients[account - 1].lastName, clients[account - 1].firstName, clients[account - 1].balance);
        printf("Account updated successfully.\n");
    }
} // end function updateRecord

// delete an existing record
void deleteRecord(struct clientData clients[])
{
    unsigned int accountNum;                        

    printf("Enter account number to delete ( 1 - 100 ): ");
    if (scanf("%u", &accountNum) != 1) {
        clearInputBuffer();
        printf("Invalid input. Please enter a valid number.\n");
        return;
    }

    if (accountNum < 1 || accountNum > MAX_RECORDS)
    {
        printf("Invalid account number.\n");
        return;
    }

    if (clients[accountNum - 1].acctNum == 0)
    {
        printf("Account %u does not exist.\n", accountNum);
    }
    else
    {
        // wipe the struct memory clean
        memset(&clients[accountNum - 1], 0, sizeof(struct clientData));
        printf("Account %u successfully deleted.\n", accountNum);
    }
} // end function deleteRecord

// create and insert record
void newRecord(struct clientData clients[])
{
    unsigned int accountNum;

    printf("Enter new account number ( 1 - 100 ): ");
    if (scanf("%u", &accountNum) != 1) {
        clearInputBuffer();
        printf("Invalid input. Please enter a valid number.\n");
        return;
    }

    if (accountNum < 1 || accountNum > MAX_RECORDS)
    {
        printf("Invalid account number.\n");
        return;
    }

    if (clients[accountNum - 1].acctNum != 0)
    {
        printf("Account #%u already contains information.\n", accountNum);
    }
    else
    {
        printf("Enter lastname, firstname, balance\n? ");
        if (scanf("%14s%9s%lf", clients[accountNum - 1].lastName, clients[accountNum - 1].firstName, &clients[accountNum - 1].balance) != 3) {
            clearInputBuffer();
            printf("Invalid input details provided.\n");
            return;
        }

        clients[accountNum - 1].acctNum = accountNum;
        printf("Account %u successfully created.\n", accountNum);
    }
} // end function newRecord

// list all accounts
void listAccounts(const struct clientData clients[])
{
    printf("\n%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");
    printf("---------------------------------------------\n");

    for (size_t i = 0; i < MAX_RECORDS; ++i)
    {
        if (clients[i].acctNum != 0)
        {
            printf("%-6u%-16s%-11s%10.2f\n", clients[i].acctNum, clients[i].lastName, clients[i].firstName, clients[i].balance);
        }
    }
} // end function listAccounts

// search for accounts by last name
void searchByLastName(const struct clientData clients[])
{
    char searchName[15];
    int found = 0;

    printf("Enter last name to search: ");
    if (scanf("%14s", searchName) != 1) {
        clearInputBuffer();
        printf("Invalid input.\n");
        return;
    }

    printf("\n%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");
    printf("---------------------------------------------\n");

    for (size_t i = 0; i < MAX_RECORDS; ++i)
    {
        if (clients[i].acctNum != 0 && strcmp(clients[i].lastName, searchName) == 0)
        {
            printf("%-6u%-16s%-11s%10.2f\n", clients[i].acctNum, clients[i].lastName, clients[i].firstName, clients[i].balance);
            found = 1;
        }
    }

    if (!found) {
        printf("No accounts found with last name '%s'.\n", searchName);
    }
}

// calculate total balance of all accounts
void calculateTotalBalance(const struct clientData clients[])
{
    double total = 0.0;
    int count = 0;

    for (size_t i = 0; i < MAX_RECORDS; ++i)
    {
        if (clients[i].acctNum != 0)
        {
            total += clients[i].balance;
            count++;
        }
    }

    printf("\nTotal Bank Balance across %d active accounts: $%.2f\n", count, total);
}

// enable user to input menu choice
unsigned int enterChoice(void)
{
    unsigned int menuChoice;
    printf("\nEnter your choice\n"
                 "1 - store a formatted text file of accounts called \"accounts.txt\"\n"
                 "2 - update an account\n"
                 "3 - add a new account\n"
                 "4 - delete an account\n"
                 "5 - list all accounts to console\n"
                 "6 - search account by last name\n"
                 "7 - calculate total bank balance\n"
                 "8 - end program\n? ");

    if (scanf("%u", &menuChoice) != 1) {
        clearInputBuffer();
        return 0; // return invalid choice to trigger default case
    }
    return menuChoice;
} // end function enterChoice