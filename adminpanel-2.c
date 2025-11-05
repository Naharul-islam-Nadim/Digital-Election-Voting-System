#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_USERS 1000
#define MAX_CANDIDATES 10
#define MAX_NAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 30
#define NID_LENGTH 20
#define ADMIN_PASSWORD "admin123"

// Structure for User
typedef struct {
    char fullName[MAX_NAME_LENGTH];
    char nidNumber[NID_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    int hasVoted;
    time_t voteTime;
} User;

// Structure for Candidate
typedef struct {
    int id;
    char name[MAX_NAME_LENGTH];
    char party[MAX_NAME_LENGTH];
    char education[100];
    int age;
    char manifesto[200];
    int votes;
} Candidate;

// Global variables
User users[MAX_USERS];
int userCount = 0;
Candidate candidates[MAX_CANDIDATES];
int candidateCount = 0;
int currentUserIndex = -1;
time_t electionStartTime;
time_t electionEndTime;

// Function prototypes for Admin Panel Part 2
void removeCandidate();
void createBackup();
void setElectionPeriod();

// Helper function prototypes
void showCandidates();
void clearInputBuffer();
void saveData();
void logActivity(char* activity);
void printHeader(char* title);
void printSuccess(char* message);
void printError(char* message);
void printInfo(char* message);

// ============================================
// FUNCTION 5: REMOVE CANDIDATE
// ============================================

void removeCandidate() {
    int id;
    
    if(candidateCount == 0) {
        printError("No candidates available to remove!");
        return;
    }
    
    showCandidates();
    printf("\nEnter Candidate ID to remove: ");
    scanf("%d", &id);
    clearInputBuffer();
    
    if(id < 1 || id > candidateCount) {
        printError("Invalid candidate ID!");
        return;
    }
    
    // Show confirmation
    printf("\n[!] CONFIRMATION REQUIRED\n");
    printf("You are about to remove:\n");
    printf("=> %s (%s)\n", candidates[id-1].name, candidates[id-1].party);
    printf("=> Current votes: %d\n", candidates[id-1].votes);
    printf("\nAre you sure? (Y/N): ");
    
    char confirm;
    scanf(" %c", &confirm);
    clearInputBuffer();
    
    if(confirm != 'Y' && confirm != 'y') {
        printInfo("Removal cancelled.");
        return;
    }
    
    // Store removed candidate info for logging
    char removedName[MAX_NAME_LENGTH];
    strcpy(removedName, candidates[id-1].name);
    
    // Shift all candidates after the removed one
    for(int i = id-1; i < candidateCount-1; i++) {
        candidates[i] = candidates[i+1];
        candidates[i].id = i + 1; // Re-assign IDs
    }
    candidateCount--;
    
    printSuccess("Candidate removed successfully!");
    printf("[+] Removed: %s\n", removedName);
    printf("[+] Total candidates now: %d\n", candidateCount);
    
    logActivity("Candidate removed by admin");
    saveData();
}

// ============================================
// FUNCTION 6: CREATE BACKUP
// ============================================

void createBackup() {
    char backupUsers[50], backupCandidates[50];
    time_t now;
    time(&now);
    struct tm *timeInfo = localtime(&now);
    
    // Generate backup filenames with timestamp
    sprintf(backupUsers, "backup_users_%04d%02d%02d_%02d%02d%02d.txt",
            timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday,
            timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);
    
    sprintf(backupCandidates, "backup_candidates_%04d%02d%02d_%02d%02d%02d.txt",
            timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday,
            timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);
    
    FILE *fp;
    int success = 1;
    
    printHeader("CREATE BACKUP");
    printf("Creating backup files...\n\n");
    
    // ========== Backup Users ==========
    fp = fopen(backupUsers, "w");
    if(fp != NULL) {
        fprintf(fp, "===================================================\n");
        fprintf(fp, "          USERS DATABASE BACKUP\n");
        fprintf(fp, "===================================================\n");
        fprintf(fp, "Backup Created: %s\n", ctime(&now));
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
        
        fprintf(fp, "===================================================\n");
        fprintf(fp, "End of Users Backup\n");
        fprintf(fp, "===================================================\n");
        fclose(fp);
        printf("[+] Users backup created: %s\n", backupUsers);
    } else {
        printError("Failed to create users backup!");
        success = 0;
    }
    
    // ========== Backup Candidates ==========
    fp = fopen(backupCandidates, "w");
    if(fp != NULL) {
        fprintf(fp, "===================================================\n");
        fprintf(fp, "        CANDIDATES DATABASE BACKUP\n");
        fprintf(fp, "===================================================\n");
        fprintf(fp, "Backup Created: %s\n", ctime(&now));
        fprintf(fp, "TOTAL_CANDIDATES=%d\n\n", candidateCount);
        
        for(int i = 0; i < candidateCount; i++) {
            fprintf(fp, "CANDIDATE_%d_START\n", i+1);
            fprintf(fp, "ID=%d\n", candidates[i].id);
            fprintf(fp, "Name=%s\n", candidates[i].name);
            fprintf(fp, "Party=%s\n", candidates[i].party);
            fprintf(fp, "Education=%s\n", candidates[i].education);
            fprintf(fp, "Age=%d\n", candidates[i].age);
            fprintf(fp, "Manifesto=%s\n", candidates[i].manifesto);
            fprintf(fp, "Votes=%d\n", candidates[i].votes);
            fprintf(fp, "CANDIDATE_%d_END\n\n", i+1);
        }
        
        fprintf(fp, "===================================================\n");
        fprintf(fp, "End of Candidates Backup\n");
        fprintf(fp, "===================================================\n");
        fclose(fp);
        printf("[+] Candidates backup created: %s\n", backupCandidates);
    } else {
        printError("Failed to create candidates backup!");
        success = 0;
    }
    
    // ========== Summary ==========
    if(success) {
        printSuccess("Backup completed successfully!");
        printf("\n========================================\n");
        printf("        BACKUP SUMMARY\n");
        printf("========================================\n");
        printf(" Users backed up:      %d\n", userCount);
        printf(" Candidates backed up: %d\n", candidateCount);
        printf(" Files created:        2\n");
        printf("========================================\n");
        logActivity("Backup created");
    } else {
        printError("Backup completed with errors!");
    }
}

// ============================================
// FUNCTION 7: SET ELECTION PERIOD
// ============================================

void setElectionPeriod() {
    int choice;
    int days, hours;
    time_t currentTime;
    time(&currentTime);
    
    printHeader("SET ELECTION PERIOD");
    
    printf("Choose configuration method:\n");
    printf("1. Set duration in days\n");
    printf("2. Set custom start and end date/time\n");
    printf("3. Start immediately for X days\n");
    printf("\nEnter your choice: ");
    scanf("%d", &choice);
    clearInputBuffer();
    
    switch(choice) {
        case 1: {
            // Set duration from now
            printf("\nEnter election duration in days (1-365): ");
            scanf("%d", &days);
            clearInputBuffer();
            
            if(days < 1 || days > 365) {
                printError("Invalid duration! Must be 1-365 days.");
                return;
            }
            
            time(&electionStartTime);
            electionEndTime = electionStartTime + (days * 24 * 60 * 60);
            break;
        }
        
        case 2: {
            // Custom start and end
            int startYear, startMonth, startDay, startHour, startMin;
            int endYear, endMonth, endDay, endHour, endMin;
            
            printf("\n--- SET START DATE & TIME ---\n");
            printf("Enter year (YYYY): ");
            scanf("%d", &startYear);
            printf("Enter month (1-12): ");
            scanf("%d", &startMonth);
            printf("Enter day (1-31): ");
            scanf("%d", &startDay);
            printf("Enter hour (0-23): ");
            scanf("%d", &startHour);
            printf("Enter minute (0-59): ");
            scanf("%d", &startMin);
            
            printf("\n--- SET END DATE & TIME ---\n");
            printf("Enter year (YYYY): ");
            scanf("%d", &endYear);
            printf("Enter month (1-12): ");
            scanf("%d", &endMonth);
            printf("Enter day (1-31): ");
            scanf("%d", &endDay);
            printf("Enter hour (0-23): ");
            scanf("%d", &endHour);
            printf("Enter minute (0-59): ");
            scanf("%d", &endMin);
            
            clearInputBuffer();
            
            // Create time structures
            struct tm startTm = {0}, endTm = {0};
            startTm.tm_year = startYear - 1900;
            startTm.tm_mon = startMonth - 1;
            startTm.tm_mday = startDay;
            startTm.tm_hour = startHour;
            startTm.tm_min = startMin;
            
            endTm.tm_year = endYear - 1900;
            endTm.tm_mon = endMonth - 1;
            endTm.tm_mday = endDay;
            endTm.tm_hour = endHour;
            endTm.tm_min = endMin;
            
            electionStartTime = mktime(&startTm);
            electionEndTime = mktime(&endTm);
            
            if(electionEndTime <= electionStartTime) {
                printError("End time must be after start time!");
                return;
            }
            break;
        }
        
        case 3: {
            // Start immediately
            printf("\nEnter election duration in days (1-365): ");
            scanf("%d", &days);
            clearInputBuffer();
            
            if(days < 1 || days > 365) {
                printError("Invalid duration! Must be 1-365 days.");
                return;
            }
            
            time(&electionStartTime);
            electionEndTime = electionStartTime + (days * 24 * 60 * 60);
            
            printInfo("Election will start immediately!");
            break;
        }
        
        default:
            printError("Invalid choice!");
            return;
    }
    
    // Display configured period
    char startStr[100], endStr[100];
    struct tm *timeInfo;
    
    timeInfo = localtime(&electionStartTime);
    strftime(startStr, sizeof(startStr), "%Y-%m-%d %H:%M:%S", timeInfo);
    
    timeInfo = localtime(&electionEndTime);
    strftime(endStr, sizeof(endStr), "%Y-%m-%d %H:%M:%S", timeInfo);
    
    // Calculate duration
    double durationSeconds = difftime(electionEndTime, electionStartTime);
    int durationDays = (int)(durationSeconds / (24 * 60 * 60));
    int durationHours = (int)((durationSeconds - (durationDays * 24 * 60 * 60)) / 3600);
    
    printSuccess("Election period set successfully!");
    printf("\n========================================\n");
    printf("      ELECTION PERIOD DETAILS\n");
    printf("========================================\n");
    printf(" Start:    %s\n", startStr);
    printf(" End:      %s\n", endStr);
    printf(" Duration: %d days, %d hours\n", durationDays, durationHours);
    printf("========================================\n");
    
    // Check current status
    time_t now;
    time(&now);
    if(difftime(now, electionStartTime) < 0) {
        printf("\n[STATUS] Election has NOT started yet\n");
        double timeToStart = difftime(electionStartTime, now);
        int daysToStart = (int)(timeToStart / (24 * 60 * 60));
        printf("         Starts in %d days\n", daysToStart);
    } else if(difftime(now, electionEndTime) > 0) {
        printf("\n[STATUS] Election has ENDED\n");
    } else {
        printf("\n[STATUS] Election is ACTIVE\n");
        double timeRemaining = difftime(electionEndTime, now);
        int daysRemaining = (int)(timeRemaining / (24 * 60 * 60));
        printf("         %d days remaining\n", daysRemaining);
    }
    
    logActivity("Election period updated");
    saveData();
}

// ============================================
// HELPER FUNCTIONS
// ============================================

void showCandidates() {
    printHeader("LIST OF CANDIDATES");
    printf("%-4s %-25s %-25s %-10s\n", "ID", "Name", "Party", "Votes");
    printf("========================================================================\n");
    
    for(int i = 0; i < candidateCount; i++) {
        printf("%-4d %-25s %-25s %-10d\n", 
               candidates[i].id,
               candidates[i].name, 
               candidates[i].party,
               candidates[i].votes);
    }
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

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
    
    // Save candidates to text file
    fp = fopen("candidates.txt", "w");
    if(fp != NULL) {
        fprintf(fp, "TOTAL_CANDIDATES=%d\n\n", candidateCount);
        for(int i = 0; i < candidateCount; i++) {
            fprintf(fp, "CANDIDATE_%d_START\n", i+1);
            fprintf(fp, "ID=%d\n", candidates[i].id);
            fprintf(fp, "Name=%s\n", candidates[i].name);
            fprintf(fp, "Party=%s\n", candidates[i].party);
            fprintf(fp, "Education=%s\n", candidates[i].education);
            fprintf(fp, "Age=%d\n", candidates[i].age);
            fprintf(fp, "Manifesto=%s\n", candidates[i].manifesto);
            fprintf(fp, "Votes=%d\n", candidates[i].votes);
            fprintf(fp, "CANDIDATE_%d_END\n\n", i+1);
        }
        fclose(fp);
    }
    
    // Save election configuration
    fp = fopen("election_config.txt", "w");
    if(fp != NULL) {
        fprintf(fp, "ElectionStartTime=%ld\n", (long)electionStartTime);
        fprintf(fp, "ElectionEndTime=%ld\n", (long)electionEndTime);
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
    
    // Initialize some test data
    time(&electionStartTime);
    electionEndTime = electionStartTime + (7 * 24 * 60 * 60); // 7 days
    
    // Add some test candidates
    candidateCount = 5;
    
    strcpy(candidates[0].name, "John Smith");
    strcpy(candidates[0].party, "Democratic Party");
    strcpy(candidates[0].education, "MBA from Harvard");
    candidates[0].age = 52;
    strcpy(candidates[0].manifesto, "Healthcare reform");
    candidates[0].id = 1;
    candidates[0].votes = 15;
    
    strcpy(candidates[1].name, "Sarah Johnson");
    strcpy(candidates[1].party, "Republican Party");
    strcpy(candidates[1].education, "Law Degree from Yale");
    candidates[1].age = 48;
    strcpy(candidates[1].manifesto, "Economic growth");
    candidates[1].id = 2;
    candidates[1].votes = 12;
    
    strcpy(candidates[2].name, "Michael Brown");
    strcpy(candidates[2].party, "Independent");
    strcpy(candidates[2].education, "PhD in Economics");
    candidates[2].age = 45;
    strcpy(candidates[2].manifesto, "Environmental protection");
    candidates[2].id = 3;
    candidates[2].votes = 8;
    
    strcpy(candidates[3].name, "Emily Davis");
    strcpy(candidates[3].party, "Green Party");
    strcpy(candidates[3].education, "MS in Environmental Science");
    candidates[3].age = 42;
    strcpy(candidates[3].manifesto, "Climate action");
    candidates[3].id = 4;
    candidates[3].votes = 5;
    
    strcpy(candidates[4].name, "Robert Wilson");
    strcpy(candidates[4].party, "Libertarian Party");
    strcpy(candidates[4].education, "BA in Political Science");
    candidates[4].age = 55;
    strcpy(candidates[4].manifesto, "Individual freedom");
    candidates[4].id = 5;
    candidates[4].votes = 3;
    
    // Add some test users
    userCount = 50;
    for(int i = 0; i < 35; i++) {
        users[i].hasVoted = 1;
    }
    
    printf("\n========================================\n");
    printf("   ADMIN PANEL MODULE - PART 2\n");
    printf("   (Developer 3 - Remaining 3 Functions)\n");
    printf("========================================\n");
    
    while(1) {
        printf("\n---------------------------------\n");
        printf("         TEST MENU\n");
        printf("---------------------------------\n");
        printf("1. Test Remove Candidate\n");
        printf("2. Test Create Backup\n");
        printf("3. Test Set Election Period\n");
        printf("4. Show All Candidates\n");
        printf("5. Exit\n");
        printf("\nEnter your choice: ");
        scanf("%d", &choice);
        clearInputBuffer();
        
        switch(choice) {
            case 1:
                removeCandidate();
                break;
            case 2:
                createBackup();
                break;
            case 3:
                setElectionPeriod();
                break;
            case 4:
                showCandidates();
                break;
            case 5:
                printSuccess("Thank you for testing!");
                exit(0);
            default:
                printError("Invalid choice!");
        }
    }
    
    return 0;
}