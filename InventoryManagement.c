#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PRODUCTS 100
#define MAX_NAME_LEN 50
#define MAX_ID 10000
#define MIN_ID 1
#define MIN_PRICE 0.0
#define MAX_PRICE 100000.0
#define MIN_QTY 0
#define MAX_QTY 1000000

typedef struct {
    int productID;
    char productName[MAX_NAME_LEN];
    float productPrice;
    int stockQty;
} Product;

// Function prototypes
void inputProduct(Product *, int, int);
void addNewProduct(Product **, int *);
void showAllProducts(Product *, int);
void modifyQuantity(Product *, int);
void removeProduct(Product **, int *);
void findByID(Product *, int);
void findByName(Product *, int);
void findByPrice(Product *, int);
int getValidID(Product *, int);
float getValidPrice();
int getValidQty();
char *getValidName();

int main() {
    int productCount = 0;

    printf("Enter number of initial products: ");
    scanf("%d", &productCount);
    getchar(); // consume newline

    if (productCount < 1 || productCount > MAX_PRODUCTS) {
        printf("Invalid count. Please restart.\n");
        return 0;
    }

    Product *products = (Product *)calloc(productCount, sizeof(Product));

    for (int i = 0; i < productCount; i++) {
        printf("\nEnter details for product %d:\n", i + 1);
        inputProduct(products, i, i);
    }

    int choice;
    do {
        printf("\n========== INVENTORY SYSTEM ==========\n");
        printf("1. Add New Product\n");
        printf("2. Display All Products\n");
        printf("3. Update Quantity\n");
        printf("4. Search by ID\n");
        printf("5. Search by Name\n");
        printf("6. Search by Price Range\n");
        printf("7. Delete a Product\n");
        printf("8. Exit\n");
        printf("=====================================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // consume newline

        switch (choice) {
            case 1:
                addNewProduct(&products, &productCount);
                break;
            case 2:
                showAllProducts(products, productCount);
                break;
            case 3:
                modifyQuantity(products, productCount);
                break;
            case 4:
                findByID(products, productCount);
                break;
            case 5:
                findByName(products, productCount);
                break;
            case 6:
                findByPrice(products, productCount);
                break;
            case 7:
                removeProduct(&products, &productCount);
                break;
            case 8:
                free(products);
                products = NULL;
                printf("\nExiting system. Memory freed successfully.\n");
                break;
            default:
                printf("\nInvalid option. Try again.\n");
        }
    } while (choice != 8);

    return 0;
}

// Function definitions

void inputProduct(Product *products, int index, int count) {
    products[index].productID = getValidID(products, count);
    printf("Enter Product Name: ");
    char *name = getValidName();
    strcpy(products[index].productName, name);
    free(name);
    printf("Enter Product Price: ");
    products[index].productPrice = getValidPrice();
    printf("Enter Product Quantity: ");
    products[index].stockQty = getValidQty();
}

void addNewProduct(Product **products, int *count) {
    if (*count >= MAX_PRODUCTS) {
        printf("Cannot add more products. Max limit reached.\n");
        return;
    }
    *products = (Product *)realloc(*products, (*count + 1) * sizeof(Product));
    printf("\nEnter new product details:\n");
    inputProduct(*products, *count, *count);
    (*count)++;
    printf("\nProduct added successfully!\n");
}

void showAllProducts(Product *products, int count) {
    if (count == 0) {
        printf("\nNo products to display.\n");
        return;
    }

    printf("\n========= PRODUCT LIST =========\n");
    for (int i = 0; i < count; i++) {
        printf("ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
               products[i].productID, products[i].productName,
               products[i].productPrice, products[i].stockQty);
    }
}

void modifyQuantity(Product *products, int count) {
    int id;
    printf("Enter Product ID to modify quantity: ");
    scanf("%d", &id);
    for (int i = 0; i < count; i++) {
        if (products[i].productID == id) {
            printf("Enter new quantity: ");
            products[i].stockQty = getValidQty();
            printf("\nQuantity updated successfully!\n");
            return;
        }
    }
    printf("\nProduct with ID %d not found.\n", id);
}

void removeProduct(Product **products, int *count) {
    if (*count == 0) {
        printf("\nNo products to delete.\n");
        return;
    }

    int id;
    printf("Enter Product ID to delete: ");
    scanf("%d", &id);

    int found = 0;
    for (int i = 0; i < *count; i++) {
        if ((*products)[i].productID == id) {
            for (int j = i; j < *count - 1; j++) {
                (*products)[j] = (*products)[j + 1];
            }
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("\nProduct with ID %d not found.\n", id);
        return;
    }

    (*count)--;
    *products = (Product *)realloc(*products, (*count) * sizeof(Product));
    printf("\nProduct deleted successfully!\n");
}

void findByID(Product *products, int count) {
    int id;
    printf("Enter Product ID to search: ");
    scanf("%d", &id);
    for (int i = 0; i < count; i++) {
        if (products[i].productID == id) {
            printf("Found: ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
                   products[i].productID, products[i].productName,
                   products[i].productPrice, products[i].stockQty);
            return;
        }
    }
    printf("Product with ID %d not found.\n", id);
}

void findByName(Product *products, int count) {
    char name[MAX_NAME_LEN];
    printf("Enter Product Name to search: ");
    getchar(); // consume leftover newline
    fgets(name, MAX_NAME_LEN, stdin);
    name[strcspn(name, "\n")] = 0;

    int found = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(products[i].productName, name) == 0) {
            printf("Found: ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
                   products[i].productID, products[i].productName,
                   products[i].productPrice, products[i].stockQty);
            found = 1;
        }
    }
    if (!found) {
        printf("No product found with name \"%s\".\n", name);
    }
}

void findByPrice(Product *products, int count) {
    float low, high;
    printf("Enter price range (min max): ");
    scanf("%f %f", &low, &high);
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (products[i].productPrice >= low && products[i].productPrice <= high) {
            printf("ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
                   products[i].productID, products[i].productName,
                   products[i].productPrice, products[i].stockQty);
            found = 1;
        }
    }
    if (!found) {
        printf("No products found in the price range %.2f - %.2f\n", low, high);
    }
}

// Input validation functions
int getValidID(Product *products, int count) {
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
            if (products[i].productID == id) {
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
        int c;
        while ((c = getchar()) != '\n' && c != EOF); 
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
            printf("Invalid price. Must be between %.2f and %.2f\n", MIN_PRICE, MAX_PRICE);
            continue;
        }
        return price;
    }
}

int getValidQty() {
    int qty;
    while (1) {
        scanf("%d", &qty);
        if (qty < MIN_QTY || qty > MAX_QTY) {
            printf("Invalid quantity. Must be between %d and %d\n", MIN_QTY, MAX_QTY);
            continue;
        }
        return qty;
    }
}
