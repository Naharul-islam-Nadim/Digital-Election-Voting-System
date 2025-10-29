#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_USERS 1000
#define MAX_NAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 30
#define NID_LENGTH 20

// Structure for User
typedef struct {
    char fullName[MAX_NAME_LENGTH];
    char nidNumber[NID_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    int hasVoted;
    time_t voteTime;
} User;

// Global variables
User users[MAX_USERS];
int userCount = 0;
int currentUserIndex = -1;
time_t lastActivityTime;

// Function prototypes
void registerUser();
int loginUser();
void clearInputBuffer();
int findUserByNID(char* nid);
int validateNID(char* nid);
int validatePassword(char* password);
void hashPassword(char* password, char* hashedPassword);
void saveData();
void loadData();
void logActivity(char* activity);
void printHeader(char* title);
void printSuccess(char* message);
void printError(char* message);
void printInfo(char* message);

// ============================================
// VALIDATION FUNCTIONS
// ============================================

int validateNID(char* nid) {
    int len = strlen(nid);
    
    // Check length (10-17 digits)
    if(len < 10 || len > 17) {
        return 0;
    }
    
    // Check if all characters are digits
    for(int i = 0; i < len; i++) {
        if(!isdigit(nid[i])) {
            return 0;
        }
    }
    
    return 1;
}

int validatePassword(char* password) {
    int len = strlen(password);
    int hasUpper = 0, hasLower = 0, hasDigit = 0;
    
    // Check minimum length
    if(len < 8) {
        return 0;
    }
    
    // Check for uppercase, lowercase, and digit
    for(int i = 0; i < len; i++) {
        if(isupper(password[i])) hasUpper = 1;
        if(islower(password[i])) hasLower = 1;
        if(isdigit(password[i])) hasDigit = 1;
    }
    
    return (hasUpper && hasLower && hasDigit);
}

void hashPassword(char* password, char* hashedPassword) {
    int hash = 0;
    for(int i = 0; password[i] != '\0'; i++) {
        hash = (hash * 31 + password[i]) % 100000;
    }
    sprintf(hashedPassword, "H%d", hash);
}

// ============================================
// REGISTRATION FUNCTION
// ============================================

void registerUser() {
    if(userCount >= MAX_USERS) {
        printError("Registration limit reached! Maximum 1000 users allowed.");
        return;
    }
    
    char fullName[MAX_NAME_LENGTH];
    char nidNumber[NID_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char hashedPassword[MAX_PASSWORD_LENGTH];
    
    printHeader("USER REGISTRATION");
    
    // Get Full Name
    printf("1. Enter your Full Name: ");
    fgets(fullName, MAX_NAME_LENGTH, stdin);
    fullName[strcspn(fullName, "\n")] = 0;
    
    if(strlen(fullName) == 0) {
        printError("Full name cannot be empty!");
        return;
    }
    
    // Get NID Number
    printf("2. Enter your NID Number (10-17 digits): ");
    fgets(nidNumber, NID_LENGTH, stdin);
    nidNumber[strcspn(nidNumber, "\n")] = 0;
    
    if(!validateNID(nidNumber)) {
        printError("Invalid NID! Must be 10-17 digits only.");
        return;
    }
    
    // Check if NID already exists
    if(findUserByNID(nidNumber) != -1) {
        printError("This NID is already registered!");
        return;
    }
    
    // Get Password
    printf("3. Enter a Password (min 8 chars, 1 uppercase, 1 lowercase, 1 digit): ");
    fgets(password, MAX_PASSWORD_LENGTH, stdin);
    password[strcspn(password, "\n")] = 0;
    
    if(!validatePassword(password)) {
        printError("Weak password! Must have 8+ chars, uppercase, lowercase, and digit.");
        return;
    }
    
    // Hash the password for security
    hashPassword(password, hashedPassword);
    
    // Store user data
    strcpy(users[userCount].fullName, fullName);
    strcpy(users[userCount].nidNumber, nidNumber);
    strcpy(users[userCount].password, hashedPassword);
    users[userCount].hasVoted = 0;
    users[userCount].voteTime = 0;
    userCount++;
    
    // Success message
    printSuccess("Registration successful!");
    printf("[+] Name: %s\n", fullName);
    printf("[+] NID: %s\n", nidNumber);
    printInfo("You can now login with your NID number.");
    
    logActivity("User registered");
    saveData();
}

// ============================================
// LOGIN FUNCTION
// ============================================

int loginUser() {
    char nidNumber[NID_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char hashedPassword[MAX_PASSWORD_LENGTH];
    
    printHeader("USER LOGIN");
    
    // Get NID Number
    printf("1. Enter your NID Number: ");
    fgets(nidNumber, NID_LENGTH, stdin);
    nidNumber[strcspn(nidNumber, "\n")] = 0;
    
    // Get Password
    printf("2. Enter your Password: ");
    fgets(password, MAX_PASSWORD_LENGTH, stdin);
    password[strcspn(password, "\n")] = 0;
    
    // Hash the entered password
    hashPassword(password, hashedPassword);
    
    // Find user by NID
    int userIndex = findUserByNID(nidNumber);
    
    // Verify credentials
    if(userIndex != -1 && strcmp(users[userIndex].password, hashedPassword) == 0) {
        currentUserIndex = userIndex;
        time(&lastActivityTime);
        
        printSuccess("Login successful!");
        printf("Welcome, %s!\n", users[userIndex].fullName);
        
        logActivity("User logged in");
        return 1; // Login successful
    } else {
        printError("Invalid NID number or password!");
        logActivity("Failed login attempt");
        return 0; // Login failed
    }
}

// ============================================
// UTILITY FUNCTIONS
// ============================================

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int findUserByNID(char* nid) {
    for(int i = 0; i < userCount; i++) {
        if(strcmp(users[i].nidNumber, nid) == 0) {
            return i;
        }
    }
    return -1; // User not found
}

// ============================================
// DATA PERSISTENCE FUNCTIONS
// ============================================

void saveData() {
    FILE *fp;
    
    // Save users to text file
    fp = fopen("users.txt", "w");
    if(fp != NULL) {
        fprintf(fp, "TOTAL_USERS=%d\n\n", userCount);
        for(int i = 0; i < userCount; i++) {
            fprintf(fp, "USER_%d_START\n", i+1);
            fprintf(fp, "FullName=%s\n", users[i].fullName);
            fprintf(fp, "NID=%s\n", users[i].nidNumber);
            fprintf(fp, "Password=%s\n", users[i].password);
            fprintf(fp, "HasVoted=%d\n", users[i].hasVoted);
            fprintf(fp, "VoteTime=%ld\n", (long)users[i].voteTime);
            fprintf(fp, "USER_%d_END\n\n", i+1);
        }
        fclose(fp);
    }
}

void loadData() {
    FILE *fp;
    char line[500];
    
    // Load users from text file
    fp = fopen("users.txt", "r");
    if(fp != NULL) {
        if(fgets(line, sizeof(line), fp)) {
            sscanf(line, "TOTAL_USERS=%d", &userCount);
        }
        
        int idx = 0;
        while(fgets(line, sizeof(line), fp) && idx < userCount) {
            if(strstr(line, "USER_") && strstr(line, "_START")) {
                // Read user data
                if(fgets(line, sizeof(line), fp)) {
                    sscanf(line, "FullName=%[^\n]", users[idx].fullName);
                }
                if(fgets(line, sizeof(line), fp)) {
                    sscanf(line, "NID=%s", users[idx].nidNumber);
                }
                if(fgets(line, sizeof(line), fp)) {
                    sscanf(line, "Password=%s", users[idx].password);
                }
                if(fgets(line, sizeof(line), fp)) {
                    sscanf(line, "HasVoted=%d", &users[idx].hasVoted);
                }
                if(fgets(line, sizeof(line), fp)) {
                    long voteTime;
                    sscanf(line, "VoteTime=%ld", &voteTime);
                    users[idx].voteTime = (time_t)voteTime;
                }
                idx++;
            }
        }
        fclose(fp);
    }
}

void logActivity(char* activity) {
    FILE *fp = fopen("activity_log.txt", "a");
    if(fp != NULL) {
        time_t now;
        time(&now);
        char timeStr[100];
        struct tm *timeInfo = localtime(&now);
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeInfo);
        
        fprintf(fp, "[%s] %s", timeStr, activity);
        if(currentUserIndex != -1) {
            fprintf(fp, " - User: %s (NID: %s)", 
                    users[currentUserIndex].fullName,
                    users[currentUserIndex].nidNumber);
        }
        fprintf(fp, "\n");
        fclose(fp);
    }
}

// ============================================
// UI HELPER FUNCTIONS
// ============================================

void printHeader(char* title) {
    printf("\n========================================\n");
    printf("  %s\n", title);
    printf("========================================\n");
}

void printSuccess(char* message) {
    printf("\n[SUCCESS] %s\n", message);
}

void printError(char* message) {
    printf("\n[ERROR] %s\n", message);
}

void printInfo(char* message) {
    printf("\n[INFO] %s\n", message);
}

// ============================================
// MAIN FUNCTION (FOR TESTING)
// ============================================

int main() {
    int choice;
    
    time(&lastActivityTime);
    loadData();
    
    printf("\n========================================\n");
    printf("   USER AUTHENTICATION MODULE\n");
    printf("========================================\n");
    
    while(1) {
        printf("\n---------------------------------\n");
        printf("         MAIN OPTIONS\n");
        printf("---------------------------------\n");
        printf("1. Register New User\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("\nEnter your choice: ");
        scanf("%d", &choice);
        clearInputBuffer();
        
        switch(choice) {
            case 1:
                registerUser();
                break;
            case 2:
                if(loginUser()) {
                    printf("\n[INFO] Login successful! Redirecting to main menu...\n");
                    // Here you would call mainMenu() in the complete system
                }
                break;
            case 3:
                saveData();
                printSuccess("Thank you for using the system!");
                exit(0);
            default:
                printError("Invalid choice! Please try again.");
        }
    }
    
    return 0;
}