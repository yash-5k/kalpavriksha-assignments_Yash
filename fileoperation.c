#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "users.txt"
#define NAME_MAX_LENGTH 50

typedef struct {
    int userId;
    char fullName[NAME_MAX_LENGTH];
    int userAge;
} PersonRecord;

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void addPerson() {
    FILE *fp = fopen(FILE_NAME, "a");
    if (!fp) {
        perror("File open error");
        return;
    }

    PersonRecord person;
    printf("Enter user ID: ");
    scanf("%d", &person.userId);
    clearInputBuffer();

    printf("Enter full name: ");
    fgets(person.fullName, NAME_MAX_LENGTH, stdin);
    person.fullName[strcspn(person.fullName, "\n")] = '\0';  // Trim newline

    printf("Enter age: ");
    scanf("%d", &person.userAge);
    clearInputBuffer();

    fprintf(fp, "%d#%s#%d\n", person.userId, person.fullName, person.userAge);
    fclose(fp);

    printf("Person record saved.\n");
}

void showAll() {
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) {
        printf("No records found.\n");
        return;
    }

    char buffer[256];
    printf("ID\tName\t\tAge\n----------------------------\n");
    while (fgets(buffer, sizeof(buffer), fp)) {
        PersonRecord person;
        char *token = strtok(buffer, "#");
        if (token) person.userId = atoi(token);

        token = strtok(NULL, "#");
        if (token) strncpy(person.fullName, token, NAME_MAX_LENGTH);

        token = strtok(NULL, "#");
        if (token) person.userAge = atoi(token);

        printf("%d\t%s\t\t%d\n", person.userId, person.fullName, person.userAge);
    }
    fclose(fp);
}

void modifyRecord() {
    printf("Enter ID to modify: ");
    int idToModify;
    scanf("%d", &idToModify);
    clearInputBuffer();

    FILE *fp = fopen(FILE_NAME, "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!fp || !temp) {
        perror("Unable to read/write files");
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        return;
    }

    char buffer[256];
    int found = 0;
    while (fgets(buffer, sizeof(buffer), fp)) {
        PersonRecord person;
        char *token = strtok(buffer, "#");
        if (token) person.userId = atoi(token);

        token = strtok(NULL, "#");
        if (token) strncpy(person.fullName, token, NAME_MAX_LENGTH);
        person.fullName[strcspn(person.fullName, "\n")] = '\0';

        token = strtok(NULL, "#");
        if (token) person.userAge = atoi(token);

        if (person.userId == idToModify) {
            found = 1;
            printf("Enter new name: ");
            fgets(person.fullName, NAME_MAX_LENGTH, stdin);
            person.fullName[strcspn(person.fullName, "\n")] = '\0';

            printf("Enter new age: ");
            scanf("%d", &person.userAge);
            clearInputBuffer();
        }

        fprintf(temp, "%d#%s#%d\n", person.userId, person.fullName, person.userAge);
    }

    fclose(fp);
    fclose(temp);

    if (!found) {
        printf("No matching record found for ID %d.\n", idToModify);
        remove("temp.txt");
        return;
    }

    remove(FILE_NAME);
    rename("temp.txt", FILE_NAME);
    printf("Record updated successfully.\n");
}

void removePerson() {
    printf("Enter ID to remove: ");
    int idToRemove;
    scanf("%d", &idToRemove);
    clearInputBuffer();

    FILE *fp = fopen(FILE_NAME, "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!fp || !temp) {
        perror("File read/write error");
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        return;
    }

    char buffer[256];
    int deleted = 0;
    while (fgets(buffer, sizeof(buffer), fp)) {
        PersonRecord person;
        char *token = strtok(buffer, "#");
        if (token) person.userId = atoi(token);

        token = strtok(NULL, "#");
        if (token) strncpy(person.fullName, token, NAME_MAX_LENGTH);
        person.fullName[strcspn(person.fullName, "\n")] = '\0';

        token = strtok(NULL, "#");
        if (token) person.userAge = atoi(token);

        if (person.userId == idToRemove) {
            deleted = 1;
            continue;
        }

        fprintf(temp, "%d#%s#%d\n", person.userId, person.fullName, person.userAge);
    }

    fclose(fp);
    fclose(temp);

    if (!deleted) {
        printf("User ID %d not found.\n", idToRemove);
        remove("temp.txt");
        return;
    }

    remove(FILE_NAME);
    rename("temp.txt", FILE_NAME);
    printf("Record deleted.\n");
}

int main() {
    int option;
    while (1) {
        printf("\n==== Manage Users ====\n");
        printf("1. Add Record\n2. Show Records\n3. Update Record\n4. Delete Record\n5. Exit\nSelect: ");
        if (scanf("%d", &option) != 1) {
            printf("Please enter a valid number.\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        switch (option) {
            case 1: addPerson(); break;
            case 2: showAll(); break;
            case 3: modifyRecord(); break;
            case 4: removePerson(); break;
            case 5: printf("Goodbye!\n"); return 0;
            default: printf("Option not valid, try again.\n");
        }
    }
}