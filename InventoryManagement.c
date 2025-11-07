#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PRODUCTS 100
#define MAX_NAME_LEN 50
#define MIN_ID 1
#define MAX_ID 10000
#define MIN_PRICE 0.0
#define MAX_PRICE 100000.0
#define MIN_QTY 0
#define MAX_QTY 1000000

typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    float price;
    int quantity;
} Product;

void enterProductDetails(Product *, int, int);
void addProduct(Product **, int *);
void displayAllProducts(Product *, int);
void updateProductQuantity(Product *, int);
void deleteProduct(Product **, int *);
void searchById(Product *, int);
void searchByName(Product *, int);
void searchByPriceRange(Product *, int);
int getValidId(Product *, int);
char *getValidName();
float getValidPrice();
int getValidQuantity();

int main() {
    int productCount;

    do {
        printf("Enter number of initial products (1-%d): ", MAX_PRODUCTS);
        scanf("%d", &productCount);
        if (productCount < 1 || productCount > MAX_PRODUCTS) {
            printf("Invalid count. Please try again.\n");
        }
    } while (productCount < 1 || productCount > MAX_PRODUCTS);
    getchar();

    Product *productList = (Product *)calloc(productCount, sizeof(Product));

    for (int i = 0; i < productCount; i++) {
        printf("\nEnter details for product %d:\n", i + 1);
        enterProductDetails(productList, i, i);
    }

    int userChoice;
    do {
        printf("\n========== INVENTORY SYSTEM ==========\n");
        printf("1. Add New Product\n");
        printf("2. Display All Products\n");
        printf("3. Update Product Quantity\n");
        printf("4. Search by Product ID\n");
        printf("5. Search by Product Name\n");
        printf("6. Search by Price Range\n");
        printf("7. Delete a Product\n");
        printf("8. Exit\n");
        printf("=====================================\n");
        printf("Enter your choice: ");
        scanf("%d", &userChoice);
        getchar();

        switch (userChoice) {
            case 1:
                addProduct(&productList, &productCount);
                break;
            case 2:
                displayAllProducts(productList, productCount);
                break;
            case 3:
                updateProductQuantity(productList, productCount);
                break;
            case 4:
                searchById(productList, productCount);
                break;
            case 5:
                searchByName(productList, productCount);
                break;
            case 6:
                searchByPriceRange(productList, productCount);
                break;
            case 7:
                deleteProduct(&productList, &productCount);
                break;
            case 8:
                free(productList);
                printf("\nExiting system. Memory freed successfully.\n");
                break;
            default:
                printf("\nInvalid choice. Try again.\n");
        }
    } while (userChoice != 8);

    return 0;
}

void enterProductDetails(Product *list, int index, int count) {
    list[index].id = getValidId(list, count);

    printf("Enter Product Name: ");
    getchar();  // clear leftover newline before reading name
    char *tempName = getValidName();
    strcpy(list[index].name, tempName);
    free(tempName);

    printf("Enter Product Price: ");
    list[index].price = getValidPrice();

    printf("Enter Product Quantity: ");
    list[index].quantity = getValidQuantity();
}

void addProduct(Product **list, int *count) {
    if (*count >= MAX_PRODUCTS) {
        printf("Cannot add more products. Maximum limit reached.\n");
        return;
    }

    *list = realloc(*list, (*count + 1) * sizeof(Product));
    if (*list == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    printf("\nEnter details for new product:\n");
    enterProductDetails(*list, *count, *count);
    (*count)++;

    printf("\nProduct added successfully.\n");
}

void displayAllProducts(Product *list, int count) {
    if (count == 0) {
        printf("\nNo products to display.\n");
        return;
    }

    printf("\n========= PRODUCT LIST =========\n");
    for (int i = 0; i < count; i++) {
        printf("ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
               list[i].id, list[i].name, list[i].price, list[i].quantity);
    }
}

void updateProductQuantity(Product *list, int count) {
    int searchId;
    printf("Enter Product ID to update quantity: ");
    scanf("%d", &searchId);

    for (int i = 0; i < count; i++) {
        if (list[i].id == searchId) {
            printf("Enter new quantity: ");
            list[i].quantity = getValidQuantity();
            printf("\nQuantity updated successfully.\n");
            return;
        }
    }
    printf("\nProduct with ID %d not found.\n", searchId);
}

void deleteProduct(Product **list, int *count) {
    if (*count == 0) {
        printf("\nNo products to delete.\n");
        return;
    }

    int deleteId;
    printf("Enter Product ID to delete: ");
    scanf("%d", &deleteId);

    int found = 0;
    for (int i = 0; i < *count; i++) {
        if ((*list)[i].id == deleteId) {
            for (int j = i; j < *count - 1; j++) {
                (*list)[j] = (*list)[j + 1];
            }
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("\nProduct with ID %d not found.\n", deleteId);
        return;
    }

    (*count)--;
    *list = realloc(*list, (*count) * sizeof(Product));
    printf("\nProduct deleted successfully.\n");
}

void searchById(Product *list, int count) {
    int searchId;
    printf("Enter Product ID to search: ");
    scanf("%d", &searchId);

    for (int i = 0; i < count; i++) {
        if (list[i].id == searchId) {
            printf("\nFound Product:\n");
            printf("ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
                   list[i].id, list[i].name, list[i].price, list[i].quantity);
            return;
        }
    }
    printf("\nProduct with ID %d not found.\n", searchId);
}

void searchByName(Product *list, int count) {
    char searchName[MAX_NAME_LEN];
    printf("Enter Product Name to search: ");
    getchar();
    fgets(searchName, MAX_NAME_LEN, stdin);
    searchName[strcspn(searchName, "\n")] = 0;

    int found = 0;
    for (int i = 0; i < count; i++) {
        if (strstr(list[i].name, searchName) != NULL) {
            printf("ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
                   list[i].id, list[i].name, list[i].price, list[i].quantity);
            found = 1;
        }
    }

    if (!found)
        printf("\nNo products found matching \"%s\".\n", searchName);
}

void searchByPriceRange(Product *list, int count) {
    float minPrice, maxPrice;

    do {
        printf("Enter price range (min max): ");
        scanf("%f %f", &minPrice, &maxPrice);
        if (minPrice < MIN_PRICE || maxPrice > MAX_PRICE || minPrice > maxPrice) {
            printf("Invalid range. Ensure %.2f <= min <= max <= %.2f\n", MIN_PRICE, MAX_PRICE);
        }
    } while (minPrice < MIN_PRICE || maxPrice > MAX_PRICE || minPrice > maxPrice);

    int found = 0;
    for (int i = 0; i < count; i++) {
        if (list[i].price >= minPrice && list[i].price <= maxPrice) {
            printf("ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
                   list[i].id, list[i].name, list[i].price, list[i].quantity);
            found = 1;
        }
    }

    if (!found)
        printf("\nNo products found in the range %.2f - %.2f\n", minPrice, maxPrice);
}

int getValidId(Product *list, int count) {
    int id;
    while (1) {
        printf("Enter Product ID: ");
        scanf("%d", &id);
        if (id < MIN_ID || id > MAX_ID) {
            printf("Invalid ID. Must be between %d and %d.\n", MIN_ID, MAX_ID);
            continue;
        }
        int unique = 1;
        for (int i = 0; i < count; i++) {
            if (list[i].id == id) {
                unique = 0;
                break;
            }
        }
        if (!unique) {
            printf("ID already exists. Enter a unique ID.\n");
            continue;
        }
        return id;
    }
}

char *getValidName() {
    char *name = malloc(MAX_NAME_LEN);
    while (1) {
        fgets(name, MAX_NAME_LEN, stdin);
        name[strcspn(name, "\n")] = 0;
        if (strlen(name) == 0) {
            printf("Name cannot be empty. Enter again: ");
            continue;
        }
        return name;
    }
}

float getValidPrice() {
    float price;
    while (1) {
        scanf("%f", &price);
        if (price < MIN_PRICE || price > MAX_PRICE) {
            printf("Invalid price. Must be between %.2f and %.2f: ", MIN_PRICE, MAX_PRICE);
            continue;
        }
        return price;
    }
}

int getValidQuantity() {
    int quantity;
    while (1) {
        scanf("%d", &quantity);
        if (quantity < MIN_QTY || quantity > MAX_QTY) {
            printf("Invalid quantity. Must be between %d and %d: ", MIN_QTY, MAX_QTY);
            continue;
        }
        return quantity;
    }
}
