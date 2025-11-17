// Refactored version with renamed variables and improved styling
// Logic remains exactly the same as provided

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "Cricket_data.h"

#define MAX_STR_LEN 50
#define MAX_TEAM_COUNT 10

typedef enum
{
    ROLE_BATSMAN = 1,
    ROLE_BOWLER = 2,
    ROLE_ALLROUNDER = 3
} RoleType;

const char* getRoleName(RoleType role)
{
    switch(role)
    {
        case ROLE_BATSMAN: return "Batsman";
        case ROLE_BOWLER: return "Bowler";
        case ROLE_ALLROUNDER: return "All-rounder";
        default: return "Unknown";
    }
}

typedef struct PlayerNode
{
    int id;
    char fullName[50];
    char teamLabel[50];
    RoleType role;
    int runs;
    float avg;
    float strikeRate;
    int wickets;
    float ecoRate;
    float perfIndex;
    struct PlayerNode *next;
} PlayerNode;

typedef struct TeamInfo
{
    int id;
    char teamName[MAX_STR_LEN];
    int playerCount;
    float accumulatedSR;
    float avgStrikeRate;
    int batCount;
    PlayerNode *batList;
    PlayerNode *bowlList;
    PlayerNode *allList;
} TeamInfo;

typedef struct TempWrapper
{
    PlayerNode *player;
    struct TempWrapper *next;
} TempWrapper;

TeamInfo TeamTable[MAX_TEAM_COUNT];

// Function declarations
void loadInitialData();
float evaluatePerformance(PlayerNode *p);
void addToTeam(TeamInfo *t, PlayerNode *p);
void sortedInsert(PlayerNode **head, PlayerNode *p);
void printHeader();
void printRecord(PlayerNode *p);
void showTeamPlayers();
int getTeamIndex(int id);
void createPlayer();
void listTeamsByAvgSR();
void showTopK();
void listPlayersGlobal();
void insertSortedTemp(TempWrapper **head, PlayerNode *p);

int main()
{
    loadInitialData();
    bool running = true;

    while (running)
    {
        int choice;
        printf("ICC ODI Player Performance Analyzer\n");
        printf("1. Add Player to Team\n");
        printf("2. Display Players of a Specific Team\n");
        printf("3. Display Teams by Avg Batting SR\n");
        printf("4. Display Top K Players of Team by Role\n");
        printf("5. Display All Players of a Role Across Teams\n");
        printf("6. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1: createPlayer(); break;
            case 2: showTeamPlayers(); break;
            case 3: listTeamsByAvgSR(); break;
            case 4: showTopK(); break;
            case 5: listPlayersGlobal(); break;
            case 6: running = false; printf("Exiting...\n"); break;
            default: printf("Invalid choice!\n");
        }
    }
}

void loadInitialData()
{
    for (int i = 0; i < teamCount; i++)
    {
        TeamTable[i].id = i + 1;
        strcpy(TeamTable[i].teamName, teams[i]);
        TeamTable[i].playerCount = 0;
        TeamTable[i].accumulatedSR = 0;
        TeamTable[i].batCount = 0;
        TeamTable[i].avgStrikeRate = 0;
        TeamTable[i].batList = NULL;
        TeamTable[i].bowlList = NULL;
        TeamTable[i].allList = NULL;
    }

    for (int i = 0; i < playerCount; i++)
    {
        PlayerNode *p = (PlayerNode*)malloc(sizeof(PlayerNode));

        p->id = players[i].id;
        strcpy(p->fullName, players[i].name);
        strcpy(p->teamLabel, players[i].team);

        if(strcmp(players[i].role, "Batsman") == 0)
            p->role = ROLE_BATSMAN;
        else if(strcmp(players[i].role, "Bowler") == 0)
            p->role = ROLE_BOWLER;
        else
            p->role = ROLE_ALLROUNDER;

        p->runs = players[i].totalRuns;
        p->avg = players[i].battingAverage;
        p->strikeRate = players[i].strikeRate;
        p->wickets = players[i].wickets;
        p->ecoRate = players[i].economyRate;
        p->perfIndex = evaluatePerformance(p);
        p->next = NULL;

        for (int j = 0; j < teamCount; j++)
        {
            if (strcmp(TeamTable[j].teamName, p->teamLabel) == 0)
            {
                addToTeam(&TeamTable[j], p);
                break;
            }
        }
    }
}

float evaluatePerformance(PlayerNode *p)
{
    if(p->role == ROLE_BATSMAN)
        return (p->avg * p->strikeRate) / 100.0f;

    if(p->role == ROLE_BOWLER)
        return (p->wickets * 2) + (100 - p->ecoRate);

    return (p->avg * p->strikeRate) / 100.0f + (p->wickets * 2);
}

void addToTeam(TeamInfo *t, PlayerNode *p)
{
    t->playerCount++;

    if(p->role == ROLE_BATSMAN || p->role == ROLE_ALLROUNDER)
    {
        t->accumulatedSR += p->strikeRate;
        t->batCount++;
        t->avgStrikeRate = t->accumulatedSR / t->batCount;
    }

    if(p->role == ROLE_BATSMAN)
        sortedInsert(&t->batList, p);
    else if(p->role == ROLE_BOWLER)
        sortedInsert(&t->bowlList, p);
    else
        sortedInsert(&t->allList, p);
}

void sortedInsert(PlayerNode **head, PlayerNode *p)
{
    if(*head == NULL || (*head)->perfIndex < p->perfIndex)
    {
        p->next = *head;
        *head = p;
        return;
    }

    PlayerNode *cur = *head;
    while(cur->next && cur->next->perfIndex >= p->perfIndex)
        cur = cur->next;

    p->next = cur->next;
    cur->next = p;
}

void printHeader()
{
    printf("------------------------------------------------------------------------------------"\n");
    printf("printf("ID   | Name                    | Role         | Runs | Avg  | SR   | Wkts | ER  | PerfIdx\n");
    printf("====================================================================================\n");
}

void printRecord(PlayerNode *p)
{
    printf("%-4d %-24s %-14s %-6d %-6.1f %-6.1f %-5d %-5.1f %-7.2f\n",
        p->id, p->fullName, getRoleName(p->role), p->runs,
        p->avg, p->strikeRate, p->wickets, p->ecoRate, p->perfIndex);
}

void showTeamPlayers()
{
    int id;
    printf("Enter Team ID: ");
    scanf("%d", &id);

    int idx = getTeamIndex(id);
    if(idx == -1)
    {
        printf("Invalid Team ID.\n");
        return;
    }

    TeamInfo *t = &TeamTable[idx];
    printHeader();

    PlayerNode *p = t->batList;
    while(p) { printRecord(p); p = p->next; }

    p = t->bowlList;
    while(p) { printRecord(p); p = p->next; }

    p = t->allList;
    while(p) { printRecord(p); p = p->next; }

    printf("====================================================================================\n");
    printf("Total Players: %d\n", t->playerCount);
    printf("Average Batting Strike Rate: %.2f\n", t->avgStrikeRate);
}

int getTeamIndex(int id)
{
    int low = 0, high = teamCount - 1;

    while(low <= high)
    {
        int mid = (low + high) / 2;
        if(TeamTable[mid].id == id) return mid;
        if(TeamTable[mid].id < id) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

void createPlayer()
{
    int tid;
    printf("Enter Team ID (1-%d): ", teamCount);
    scanf("%d", &tid);

    int idx = getTeamIndex(tid);
    if(idx == -1)
    {
        printf("Invalid Team\n");
        return;
    }

    PlayerNode *p = (PlayerNode*)malloc(sizeof(PlayerNode));

    printf("Player ID: "); scanf("%d", &p->id); getchar();

    printf("Name: ");
    fgets(p->fullName, MAX_STR_LEN, stdin);
    p->fullName[strcspn(p->fullName, "\n")] = '\0';

    strcpy(p->teamLabel, TeamTable[idx].teamName);

    int r;
    printf("Role (1=Batsman, 2=Bowler, 3=All-rounder): ");
    scanf("%d", &r);
    p->role = (RoleType)r;

    printf("Total Runs: "); scanf("%d", &p->runs);
    printf("Batting Average: "); scanf("%f", &p->avg);
    printf("Strike Rate: "); scanf("%f", &p->strikeRate);
    printf("Wickets: "); scanf("%d", &p->wickets);
    printf("Economy Rate: "); scanf("%f", &p->ecoRate);

    p->perfIndex = evaluatePerformance(p);
    p->next = NULL;

    addToTeam(&TeamTable[idx], p);

    printf("Player Added Successfully!\n");
}

void listTeamsByAvgSR()
{
    TeamInfo temp[MAX_TEAM_COUNT];
    for(int i = 0; i < teamCount; i++) temp[i] = TeamTable[i];

    for(int i = 0; i < teamCount - 1; i++)
    {
        for(int j = 0; j < teamCount - i - 1; j++)
        {
            if(temp[j].avgStrikeRate < temp[j+1].avgStrikeRate)
            {
                TeamInfo swap = temp[j];
                temp[j] = temp[j+1];
                temp[j+1] = swap;
            }
        }
    }

    printf("ID   Team Name           Avg Bat SR   Total Players\n");
    printf("===================================================\n");

    for(int i = 0; i < teamCount; i++)
    {
        printf("%-4d %-18s %-15.2f %-5d\n",
            temp[i].id, temp[i].teamName, temp[i].avgStrikeRate, temp[i].playerCount);
    }
    printf("===================================================\n");
}

void showTopK()
{
    int tid, k, role;
    printf("Enter Team ID: "); scanf("%d", &tid);

    int idx = getTeamIndex(tid);
    if(idx == -1)
    {
        printf("Invalid Team ID\n");
        return;
    }

    printf("Enter Role (1=Batsman, 2=Bowler, 3=All-rounder): "); scanf("%d", &role);
    printf("Enter K: "); scanf("%d", &k);

    PlayerNode *head = NULL;
    if(role == ROLE_BATSMAN) head = TeamTable[idx].batList;
    else if(role == ROLE_BOWLER) head = TeamTable[idx].bowlList;
    else head = TeamTable[idx].allList;

    if(!head)
    {
        printf("No players for this role\n");
        return;
    }

    printHeader();

    int count = 0;
    PlayerNode *p = head;
    while(p && count < k)
    {
        printRecord(p);
        p = p->next;
        count++;
    }

    printf("====================================================================================\n");
}

void listPlayersGlobal()
{
    int role;
    char roleStr[20];

    printf("Enter Role (1=Batsman, 2=Bowler, 3=All-rounder): ");
    scanf("%d", &role);

    if(role == ROLE_BATSMAN) strcpy(roleStr, "Batsman");
    else if(role == ROLE_BOWLER) strcpy(roleStr, "Bowler");
    else strcpy(roleStr, "All-rounder");

    TempWrapper *head = NULL;

    for(int i = 0; i < teamCount; i++)
    {
        PlayerNode *p = NULL;

        if(role == ROLE_BATSMAN) p = TeamTable[i].batList;
        else if(role == ROLE_BOWLER) p = TeamTable[i].bowlList;
        else p = TeamTable[i].allList;

        while(p)
        {
            insertSortedTemp(&head, p);
            p = p->next;
        }
    }

    if(!head)
    {
        printf("No players found for role %s\n", roleStr);
        return;
    }

    printf("Players Across All Teams (Role = %s)\n", roleStr);
    printf("==============================================================================================\n");
    printf("ID   Name                 Team              Role           Runs  Avg   SR    Wkts  ER   PerfIdx\n");
    printf("==============================================================================================\n");

    TempWrapper *cur = head;
    while(cur)
    {
        PlayerNode *p = cur->player;
        printf("%-4d %-20s %-16s %-15s %-5d %-5.1f %-5.1f %-4d %-4.1f %-7.2f\n",
            p->id, p->fullName, p->teamLabel, getRoleName(p->role),
            p->runs, p->avg, p->strikeRate, p->wickets, p->ecoRate, p->perfIndex);
        cur = cur->next;
    }

    while(head)
    {
        TempWrapper *tmp = head;
        head = head->next;
        free(tmp);
    }
}

void insertSortedTemp(TempWrapper **head, PlayerNode *p)
{
    TempWrapper *node = (TempWrapper*)malloc(sizeof(TempWrapper));
    node->player = p;
    node->next = NULL;

    if(*head == NULL || (*head)->player->perfIndex < p->perfIndex)
    {
        node->next = *head;
        *head = node;
        return;
    }

    TempWrapper *cur = *head;
    while(cur->next && cur->next->player->perfIndex >= p->perfIndex)
        cur = cur->next;

    node->next = cur->next;
    cur->next = node;
}
