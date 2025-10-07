#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define FILE_NAME "users.txt"
#define NAME_MAX_LENGTH 50

// A struct to hold the data for one person.
typedef struct {
    int userId;
    char fullName[NAME_MAX_LENGTH];
    int userAge;
} PersonRecord;

// A necessary function to clear the input buffer.
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void addPerson() {
    FILE *fp = fopen(FILE_NAME, "a");
    if (!fp) {
        perror("Error opening file");
        return;
    }

    PersonRecord person;
    printf("Enter user ID: ");
    scanf("%d", &person.userId);
    clearInputBuffer();

    printf("Enter full name: ");
    fgets(person.fullName, NAME_MAX_LENGTH, stdin);
  

    printf("Enter age: ");
    scanf("%d", &person.userAge);
    clearInputBuffer();

    fprintf(fp, "%d#%s#%d\n", person.userId, person.fullName, person.userAge);
    fclose(fp);

    printf("Person record saved.\n");
}

void displayAllRecords() {
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) {
        printf("No records found or file cannot be opened.\n");
        return;
    }

    char buffer[256];
    printf("\nID\tName\t\t\tAge\n-------------------------------------------\n");

    while (fgets(buffer, sizeof(buffer), fp)) {
        PersonRecord person;
        if (sscanf(buffer, "%d#%49[^#]#%d", &person.userId, person.fullName, &person.userAge) == 3) {
             printf("%d\t%-20s\t%d\n", person.userId, person.fullName, person.userAge);
        }
    }
    fclose(fp);
}

void modifyRecord() {
    int idToModify;
    printf("Enter ID to modify: ");
    scanf("%d", &idToModify);
    clearInputBuffer();

    FILE *fp = fopen(FILE_NAME, "r");
    FILE *temp = fopen("temp.txt", "w");

    if (!fp || !temp) {
        perror("Error opening files");
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        return;
    }

    char buffer[256];
    bool found = false;

    while (fgets(buffer, sizeof(buffer), fp)) {
        PersonRecord person;
        if (sscanf(buffer, "%d#%49[^#]#%d", &person.userId, person.fullName, &person.userAge) != 3) {
            fputs(buffer, temp);
            continue;
        }

        if (person.userId == idToModify) {
            found = true;
            printf("Enter new name: ");
            fgets(person.fullName, NAME_MAX_LENGTH, stdin);
            

            printf("Enter new age: ");
            scanf("%d", &person.userAge);
            clearInputBuffer();
        }

        fprintf(temp, "%d#%s#%d\n", person.userId, person.fullName, person.userAge);
    }

    fclose(fp);
    fclose(temp);

    if (!found) {
        printf("Record with ID %d not found.\n", idToModify);
        remove("temp.txt");
    } else {
        remove(FILE_NAME);
        rename("temp.txt", FILE_NAME);
        printf("Record updated successfully.\n");
    }
}

void removePerson() {
    int idToRemove;
    printf("Enter ID to remove: ");
    scanf("%d", &idToRemove);
    clearInputBuffer();

    FILE *fp = fopen(FILE_NAME, "r");
    FILE *temp = fopen("temp.txt", "w");

    if (!fp || !temp) {
        perror("Error opening files");
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        return;
    }

    char buffer[256];
    bool deleted = false;
    int currentId;

    while (fgets(buffer, sizeof(buffer), fp)) {
        if (sscanf(buffer, "%d#", &currentId) == 1 && currentId == idToRemove) {
            deleted = true;
        } else {
            fputs(buffer, temp);
        }
    }

    fclose(fp);
    fclose(temp);

    if (!deleted) {
        printf("User ID %d not found.\n", idToRemove);
        remove("temp.txt");
    } else {
        remove(FILE_NAME);
        rename("temp.txt", FILE_NAME);
        printf("Record deleted.\n");
    }
}

int main() {
    int option;
    bool isRunning = true;

    while (isRunning) {
        printf("\n==== Manage Users ====\n");
        printf("1. Add Record\n");
        printf("2. Display All Records\n");
        printf("3. Update Record\n");
        printf("4. Delete Record\n");
        printf("5. Exit\n");
        printf("Select an option: ");

        if (scanf("%d", &option) != 1) {
            printf("Invalid input. Please enter a number.\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        switch (option) {
            case 1:
                addPerson();
                break;
            case 2:
                displayAllRecords();
                break;
            case 3:
                modifyRecord();
                break;
            case 4:
                removePerson();
                break;
            case 5:
                isRunning = false;
                break;
            default:
                printf("Option not valid. Please try again.\n");
        }
    }

    printf("Goodbye!\n");
    return 0;
}