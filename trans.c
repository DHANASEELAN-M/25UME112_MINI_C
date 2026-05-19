// Bank-account program reads a random-access file sequentially,
// updates data already written to the file, creates new data to
// be placed in the file, and deletes data previously in the file.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// clientData structure definition
struct clientData
{
    unsigned int acctNum; // account number
    char lastName[15];    // account last name
    char firstName[10];   // account first name
    double balance;       // account balance
};                        // end structure clientData

// prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void listAccounts(FILE *fPtr);
void searchByLastName(FILE *fPtr);
void calculateTotalBalance(FILE *fPtr);
void clearInputBuffer(void);
int readRecord(FILE *fPtr, unsigned int accountNum, struct clientData *client);
void writeRecord(FILE *fPtr, unsigned int accountNum, const struct clientData *client);

int main(int argc, char *argv[])
{
    FILE *cfPtr;         // credit.dat file pointer
    unsigned int choice; // user's choice

    // fopen opens the file; exits if file cannot be opened
    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
    {
        printf("File could not be opened, attempting to create...\n");
        if ((cfPtr = fopen("credit.dat", "wb+")) == NULL)
        {
            printf("%s: File could not be created.\n", argv[0]);
            exit(-1);
        }
        else
        {
            // Initialize the file with 100 blank records
            struct clientData blankClient = {0, "", "", 0.0};
            for (unsigned int i = 1; i <= 100; ++i)
            {
                fwrite(&blankClient, sizeof(struct clientData), 1, cfPtr);
            }
            rewind(cfPtr); // sets pointer to beginning of file
        }
    }

    // enable user to specify action
    while ((choice = enterChoice()) != 8)
    {
        switch (choice)
        {
        case 1:
            textFile(cfPtr);
            break;
        case 2:
            updateRecord(cfPtr);
            break;
        case 3:
            newRecord(cfPtr);
            break;
        case 4:
            deleteRecord(cfPtr);
            break;
        case 5:
            listAccounts(cfPtr);
            break;
        case 6:
            searchByLastName(cfPtr);
            break;
        case 7:
            calculateTotalBalance(cfPtr);
            break;
        case 0:
            // 0 is returned when scanf fails to read an integer
            puts("Incorrect choice. Please enter a number.");
            break;
        default:
            puts("Incorrect choice");
            break;
        } // end switch
    }     // end while

    fclose(cfPtr); // fclose closes the file
    return 0;
} // end main

// utility to clear input buffer
void clearInputBuffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// read a record from file safely
int readRecord(FILE *fPtr, unsigned int accountNum, struct clientData *client)
{
    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    return fread(client, sizeof(struct clientData), 1, fPtr);
}

// write a record to file safely
void writeRecord(FILE *fPtr, unsigned int accountNum, const struct clientData *client)
{
    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(client, sizeof(struct clientData), 1, fPtr);
}

// create formatted text file for printing
void textFile(FILE *readPtr)
{
    FILE *writePtr; // accounts.txt file pointer
    struct clientData client = {0, "", "", 0.0};

    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        puts("File could not be opened.");
    }
    else
    {
        rewind(readPtr); // sets pointer to beginning of file
        fprintf(writePtr, "%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");

        while (fread(&client, sizeof(struct clientData), 1, readPtr) == 1)
        {
            if (client.acctNum != 0)
            {
                fprintf(writePtr, "%-6u%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName, client.balance);
            }
        }

        fclose(writePtr); // fclose closes the file
        printf("Data successfully exported to accounts.txt\n");
    }
} // end function textFile

// update balance in record
void updateRecord(FILE *fPtr)
{
    unsigned int account; // account number
    double transaction;   // transaction amount
    struct clientData client = {0, "", "", 0.0};

    printf("Enter account to update ( 1 - 100 ): ");
    if (scanf("%u", &account) != 1) {
        clearInputBuffer();
        printf("Invalid input. Please enter a valid number.\n");
        return;
    }

    if (account < 1 || account > 100)
    {
        printf("Invalid account number.\n");
        return;
    }

    readRecord(fPtr, account, &client);

    if (client.acctNum == 0)
    {
        printf("Account #%u has no information.\n", account);
    }
    else
    {
        printf("%-6u%-16s%-11s%10.2f\n\n", client.acctNum, client.lastName, client.firstName, client.balance);

        printf("Enter charge ( + ) or payment ( - ): ");
        if (scanf("%lf", &transaction) != 1) {
            clearInputBuffer();
            printf("Invalid transaction amount.\n");
            return;
        }
        client.balance += transaction; // update record balance

        printf("%-6u%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName, client.balance);

        writeRecord(fPtr, account, &client);
        printf("Account updated successfully.\n");
    }
} // end function updateRecord

// delete an existing record
void deleteRecord(FILE *fPtr)
{
    struct clientData client;                       
    struct clientData blankClient = {0, "", "", 0}; 
    unsigned int accountNum;                        

    printf("Enter account number to delete ( 1 - 100 ): ");
    if (scanf("%u", &accountNum) != 1) {
        clearInputBuffer();
        printf("Invalid input. Please enter a valid number.\n");
        return;
    }

    if (accountNum < 1 || accountNum > 100)
    {
        printf("Invalid account number.\n");
        return;
    }

    readRecord(fPtr, accountNum, &client);

    if (client.acctNum == 0)
    {
        printf("Account %u does not exist.\n", accountNum);
    }
    else
    {
        writeRecord(fPtr, accountNum, &blankClient);
        printf("Account %u successfully deleted.\n", accountNum);
    }
} // end function deleteRecord

// create and insert record
void newRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};
    unsigned int accountNum;

    printf("Enter new account number ( 1 - 100 ): ");
    if (scanf("%u", &accountNum) != 1) {
        clearInputBuffer();
        printf("Invalid input. Please enter a valid number.\n");
        return;
    }

    if (accountNum < 1 || accountNum > 100)
    {
        printf("Invalid account number.\n");
        return;
    }

    readRecord(fPtr, accountNum, &client);

    if (client.acctNum != 0)
    {
        printf("Account #%u already contains information.\n", client.acctNum);
    }
    else
    {
        printf("Enter lastname, firstname, balance\n? ");
        if (scanf("%14s%9s%lf", client.lastName, client.firstName, &client.balance) != 3) {
            clearInputBuffer();
            printf("Invalid input details provided.\n");
            return;
        }

        client.acctNum = accountNum;
        writeRecord(fPtr, accountNum, &client);
        printf("Account %u successfully created.\n", accountNum);
    }
} // end function newRecord

// list all accounts
void listAccounts(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};

    printf("\n%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");
    printf("---------------------------------------------\n");

    rewind(fPtr); // start from beginning
    while (fread(&client, sizeof(struct clientData), 1, fPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            printf("%-6u%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName, client.balance);
        }
    }
} // end function listAccounts

// search for accounts by last name
void searchByLastName(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};
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

    rewind(fPtr);
    while (fread(&client, sizeof(struct clientData), 1, fPtr) == 1)
    {
        if (client.acctNum != 0 && strcmp(client.lastName, searchName) == 0)
        {
            printf("%-6u%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName, client.balance);
            found = 1;
        }
    }

    if (!found) {
        printf("No accounts found with last name '%s'.\n", searchName);
    }
}

// calculate total balance of all accounts
void calculateTotalBalance(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};
    double total = 0.0;
    int count = 0;

    rewind(fPtr);
    while (fread(&client, sizeof(struct clientData), 1, fPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            total += client.balance;
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