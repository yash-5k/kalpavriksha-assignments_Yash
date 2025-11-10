#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BLOCKS 1024
#define BLOCK_SIZE 512
#define MAX_NAME 50
#define MAX_INPUT 2048

typedef struct FreeBlock {
    int index;
    struct FreeBlock *next;
} FreeBlock;

typedef struct FileNode {
    char name[MAX_NAME];
    int isDirectory;
    int size;
    int blockCount;
    struct FileNode *parent;
    struct FileNode *child;
    struct FileNode *next;
    struct FileNode *prev;
    int blockPointers[100];
} FileNode;

unsigned char virtualDisk[MAX_BLOCKS][BLOCK_SIZE];
FreeBlock *freeList = NULL;
int usedBlocks = 0;
FileNode *root = NULL;
FileNode *currentDir = NULL;

void initializeFreeList() {
    for (int i = 0; i < MAX_BLOCKS; i++) {
        FreeBlock *newBlock = (FreeBlock *)malloc(sizeof(FreeBlock));
        newBlock->index = i;
        newBlock->next = freeList;
        freeList = newBlock;
    }
}

FreeBlock *allocateBlock() {
    if (freeList == NULL) {
        return NULL;
    }
    FreeBlock *block = freeList;
    freeList = freeList->next;
    usedBlocks++;
    return block;
}

void freeBlock(int index) {
    FreeBlock *block = (FreeBlock *)malloc(sizeof(FreeBlock));
    block->index = index;
    block->next = freeList;
    freeList = block;
    usedBlocks--;
}

FileNode *createNode(const char *name, int isDirectory, FileNode *parent) {
    FileNode *node = (FileNode *)malloc(sizeof(FileNode));
    strcpy(node->name, name);
    node->isDirectory = isDirectory;
    node->size = 0;
    node->blockCount = 0;
    node->parent = parent;
    node->child = NULL;
    node->next = node->prev = NULL;
    for (int i = 0; i < 100; i++)
        node->blockPointers[i] = -1;
    return node;
}

void insertChild(FileNode *parent, FileNode *child) {
    if (parent->child == NULL) {
        parent->child = child;
        child->next = child->prev = child;
    } else {
        FileNode *first = parent->child;
        FileNode *last = first->prev;
        last->next = child;
        child->prev = last;
        child->next = first;
        first->prev = child;
    }
}

FileNode *findChild(FileNode *parent, const char *name) {
    if (!parent->child) return NULL;
    FileNode *temp = parent->child;
    do {
        if (strcmp(temp->name, name) == 0)
            return temp;
        temp = temp->next;
    } while (temp != parent->child);
    return NULL;
}

void makeDirectory(const char *name) {
    if (findChild(currentDir, name)) {
        printf("Directory already exists.\n");
        return;
    }
    FileNode *dir = createNode(name, 1, currentDir);
    insertChild(currentDir, dir);
    printf("Directory '%s' created.\n", name);
}

void listDirectory() {
    if (!currentDir->child) {
        printf("(empty)\n");
        return;
    }
    FileNode *temp = currentDir->child;
    do {
        printf("%s%s\n", temp->name, temp->isDirectory ? "/" : "");
        temp = temp->next;
    } while (temp != currentDir->child);
}

void changeDirectory(const char *name) {
    if (strcmp(name, "..") == 0) {
        if (currentDir->parent)
            currentDir = currentDir->parent;
        else
            printf("Already at root.\n");
        return;
    }

    FileNode *target = findChild(currentDir, name);
    if (target && target->isDirectory) {
        currentDir = target;
    } else {
        printf("Directory not found.\n");
    }
}

void showPath(FileNode *node) {
    if (node->parent != NULL)
        showPath(node->parent);
    printf("/%s", node->name);
}

void printWorkingDir() {
    if (currentDir == root)
        printf("/\n");
    else {
        showPath(currentDir);
        printf("\n");
    }
}

void makeFile(const char *name) {
    if (findChild(currentDir, name)) {
        printf("File already exists.\n");
        return;
    }
    FileNode *file = createNode(name, 0, currentDir);
    insertChild(currentDir, file);
    printf("File '%s' created.\n", name);
}

void writeFile(const char *name, const char *data) {
    FileNode *file = findChild(currentDir, name);
    if (!file || file->isDirectory) {
        printf("File not found.\n");
        return;
    }

    int len = strlen(data);
    int blocksNeeded = (len + BLOCK_SIZE - 1) / BLOCK_SIZE;

    if (blocksNeeded > (MAX_BLOCKS - usedBlocks)) {
        printf("Not enough space.\n");
        return;
    }

    // clear old blocks
    for (int i = 0; i < file->blockCount; i++) {
        if (file->blockPointers[i] != -1)
            freeBlock(file->blockPointers[i]);
    }

    file->blockCount = 0;
    file->size = len;

    int dataIndex = 0;
    for (int i = 0; i < blocksNeeded; i++) {
        FreeBlock *block = allocateBlock();
        if (!block) {
            printf("Disk full.\n");
            return;
        }
        file->blockPointers[i] = block->index;
        for (int j = 0; j < BLOCK_SIZE && dataIndex < len; j++) {
            virtualDisk[block->index][j] = data[dataIndex++];
        }
        file->blockCount++;
        free(block);
    }
    printf("Data written to '%s' (%d bytes).\n", name, len);
}

void readFile(const char *name) {
    FileNode *file = findChild(currentDir, name);
    if (!file || file->isDirectory) {
        printf("File not found.\n");
        return;
    }
    if (file->size == 0) {
        printf("(empty file)\n");
        return;
    }

    int bytesRead = 0;
    for (int i = 0; i < file->blockCount; i++) {
        int idx = file->blockPointers[i];
        for (int j = 0; j < BLOCK_SIZE && bytesRead < file->size; j++) {
            printf("%c", virtualDisk[idx][j]);
            bytesRead++;
        }
    }
    printf("\n");
}

void deleteFile(const char *name) {
    FileNode *file = findChild(currentDir, name);
    if (!file || file->isDirectory) {
        printf("File not found.\n");
        return;
    }

    for (int i = 0; i < file->blockCount; i++) {
        if (file->blockPointers[i] != -1)
            freeBlock(file->blockPointers[i]);
    }

    FileNode *next = file->next;
    FileNode *prev = file->prev;

    if (next == file) {
        currentDir->child = NULL;
    } else {
        prev->next = next;
        next->prev = prev;
        if (currentDir->child == file)
            currentDir->child = next;
    }

    free(file);
    printf("File deleted.\n");
}

void removeDir(const char *name) {
    FileNode *dir = findChild(currentDir, name);
    if (!dir || !dir->isDirectory) {
        printf("Directory not found.\n");
        return;
    }
    if (dir->child != NULL) {
        printf("Directory not empty.\n");
        return;
    }

    FileNode *next = dir->next;
    FileNode *prev = dir->prev;

    if (next == dir)
        currentDir->child = NULL;
    else {
        prev->next = next;
        next->prev = prev;
        if (currentDir->child == dir)
            currentDir->child = next;
    }

    free(dir);
    printf("Directory removed.\n");
}

void showDiskInfo() {
    int freeBlocks = MAX_BLOCKS - usedBlocks;
    double usedPercent = (usedBlocks * 100.0) / MAX_BLOCKS;
    printf("Total blocks: %d\nUsed blocks: %d\nFree blocks: %d\nUsage: %.2f%%\n",
           MAX_BLOCKS, usedBlocks, freeBlocks, usedPercent);
}

// ---------------------- MAIN LOOP ----------------------

int main() {
    initializeFreeList();
    root = createNode("", 1, NULL);
    currentDir = root;

    printf("Simple Virtual File System Ready.\n");
    char input[MAX_INPUT];
    while (1) {
        printf("%s> ", currentDir == root ? "/" : currentDir->name);
        fgets(input, MAX_INPUT, stdin);
        input[strcspn(input, "\n")] = '\0';

        if (strlen(input) == 0) continue;

        char cmd[50], arg1[100], arg2[1000];
        int count = sscanf(input, "%s %s %[^\n]", cmd, arg1, arg2);

        if (strcmp(cmd, "exit") == 0)
            break;
        else if (strcmp(cmd, "mkdir") == 0 && count >= 2)
            makeDirectory(arg1);
        else if (strcmp(cmd, "rmdir") == 0 && count >= 2)
            removeDir(arg1);
        else if (strcmp(cmd, "ls") == 0)
            listDirectory();
        else if (strcmp(cmd, "cd") == 0 && count >= 2)
            changeDirectory(arg1);
        else if (strcmp(cmd, "pwd") == 0)
            printWorkingDir();
        else if (strcmp(cmd, "create") == 0 && count >= 2)
            makeFile(arg1);
        else if (strcmp(cmd, "delete") == 0 && count >= 2)
            deleteFile(arg1);
        else if (strcmp(cmd, "write") == 0 && count >= 3)
            writeFile(arg1, arg2);
        else if (strcmp(cmd, "read") == 0 && count >= 2)
            readFile(arg1);
        else if (strcmp(cmd, "df") == 0)
            showDiskInfo();
        else
            printf("Unknown or invalid command.\n");
    }

    printf("Exiting VFS...\n");
    return 0;
}
