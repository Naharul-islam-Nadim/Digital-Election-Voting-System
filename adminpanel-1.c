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

// Function prototypes
void adminPanel();
void showStatistics();
void exportResults();
void resetElection();
void addCandidate();
void loadData();

// Helper function prototypes
void clearInputBuffer();
void saveData();
void logActivity(char* activity);
void printHeader(char* title);
void printSuccess(char* message);
void printError(char* message);
void printInfo(char* message);
int isElectionActive();

// ============================================
// ADMIN PANEL MAIN FUNCTION
// ============================================

void adminPanel() {
    char password[MAX_PASSWORD_LENGTH];
    int choice;
    
    printHeader("ADMIN PANEL");
    printf("Enter Admin Password: ");
    fgets(password, MAX_PASSWORD_LENGTH, stdin);
    password[strcspn(password, "\n")] = 0;
    
    if(strcmp(password, ADMIN_PASSWORD) != 0) {
        printError("Invalid admin password!");
        logActivity("Failed admin login attempt");
        return;
    }
    
    printSuccess("Admin access granted!");
    logActivity("Admin logged in");
    
    while(1) {
        printHeader("ADMIN PANEL");
        printf("1. View All Statistics\n");
        printf("2. Export Results\n");
        printf("3. Reset Election\n");
        printf("4. Add Candidate\n");
        printf("5. Exit Admin Panel\n");
        printf("========================================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clearInputBuffer();
        
        switch(choice) {
            case 1:
                showStatistics();
                break;
            case 2:
                exportResults();
                break;
            case 3:
                resetElection();
                break;
            case 4:
                addCandidate();
                break;
            case 5:
                printInfo("Exiting admin panel...");
                return;
            default:
                printError("Invalid choice!");
        }
    }
}

// ============================================
// FUNCTION 1: SHOW STATISTICS
// ============================================

void showStatistics() {
    int totalVotes = 0;
    int votedUsers = 0;
    
    // Calculate total votes from all candidates
    for(int i = 0; i < candidateCount; i++) {
        totalVotes += candidates[i].votes;
    }
    
    // Count users who have voted
    for(int i = 0; i < userCount; i++) {
        if(users[i].hasVoted) votedUsers++;
    }
    
    // Calculate voter turnout percentage
    float turnout = (userCount > 0) ? (votedUsers * 100.0 / userCount) : 0;
    
    printHeader("ELECTION STATISTICS");
    printf("Total Registered Users:  %d\n", userCount);
    printf("Users Who Voted:         %d\n", votedUsers);
    printf("Users Who Haven't Voted: %d\n", userCount - votedUsers);
    printf("Voter Turnout:           %.2f%%\n", turnout);
    printf("Total Votes Cast:        %d\n", totalVotes);
    printf("Total Candidates:        %d\n", candidateCount);
    
    // Display election period information
    char startStr[100], endStr[100];
    struct tm *timeInfo;
    
    timeInfo = localtime(&electionStartTime);
    strftime(startStr, sizeof(startStr), "%Y-%m-%d %H:%M", timeInfo);
    
    timeInfo = localtime(&electionEndTime);
    strftime(endStr, sizeof(endStr), "%Y-%m-%d %H:%M", timeInfo);
    
    printf("\nElection Period:\n");
    printf("   Start: %s\n", startStr);
    printf("   End:   %s\n", endStr);
    
    // Show election status
    int status = isElectionActive();
    if(status == 1) {
        printf("   Status: ACTIVE\n");
    } else if(status == 0) {
        printf("   Status: NOT STARTED\n");
    } else {
        printf("   Status: ENDED\n");
    }
    
    printf("========================================\n");
}

// ============================================
// FUNCTION 2: EXPORT RESULTS
// ============================================

void exportResults() {
    FILE *fp = fopen("election_results.txt", "w");
    if(fp == NULL) {
        printError("Failed to export results!");
        return;
    }
    
    int totalVotes = 0;
    for(int i = 0; i < candidateCount; i++) {
        totalVotes += candidates[i].votes;
    }
    
    // Write header
    fprintf(fp, "===================================================\n");
    fprintf(fp, "          GENERAL ELECTION RESULTS\n");
    fprintf(fp, "===================================================\n\n");
    
    // Write report generation time
    time_t now;
    time(&now);
    fprintf(fp, "Report Generated: %s\n", ctime(&now));
    
    // Write candidate results
    fprintf(fp, "\nCandidate Results:\n");
    fprintf(fp, "---------------------------------------------------\n");
    
    for(int i = 0; i < candidateCount; i++) {
        float percentage = (totalVotes > 0) ? (candidates[i].votes * 100.0 / totalVotes) : 0;
        fprintf(fp, "%d. %-25s (%-20s) : %d votes (%.2f%%)\n", 
                i+1, candidates[i].name, candidates[i].party, 
                candidates[i].votes, percentage);
    }
    
    // Write summary
    fprintf(fp, "\n---------------------------------------------------\n");
    fprintf(fp, "Total Votes Cast: %d\n", totalVotes);
    fprintf(fp, "Total Registered Users: %d\n", userCount);
    
    // Find and write winner
    int maxVotes = -1;
    int winnerIndex = -1;
    for(int i = 0; i < candidateCount; i++) {
        if(candidates[i].votes > maxVotes) {
            maxVotes = candidates[i].votes;
            winnerIndex = i;
        }
    }
    
    if(winnerIndex != -1 && maxVotes > 0) {
        fprintf(fp, "\nWINNER: %s (%s) with %d votes\n", 
                candidates[winnerIndex].name,
                candidates[winnerIndex].party,
                maxVotes);
    }
    
    fprintf(fp, "\n===================================================\n");
    
    fclose(fp);
    printSuccess("Results exported to 'election_results.txt'");
    logActivity("Results exported");
}

// ============================================
// FUNCTION 3: RESET ELECTION
// ============================================

void resetElection() {
    char confirm[10];
    
    printf("\n[WARNING] This will reset all votes and voting status!\n");
    printf("Type 'RESET' to confirm: ");
    fgets(confirm, 10, stdin);
    confirm[strcspn(confirm, "\n")] = 0;
    
    if(strcmp(confirm, "RESET") != 0) {
        printInfo("Reset cancelled.");
        return;
    }
    
    // Reset all candidate votes to zero
    for(int i = 0; i < candidateCount; i++) {
        candidates[i].votes = 0;
    }
    
    // Reset all user voting status
    for(int i = 0; i < userCount; i++) {
        users[i].hasVoted = 0;
        users[i].voteTime = 0;
    }
    
    printSuccess("Election reset successfully!");
    printf("All votes cleared\n");
    printf("All voting status reset\n");
    printf("Users can now vote again\n");
    
    logActivity("Election reset by admin");
    saveData();
}

// ============================================
// FUNCTION 4: ADD CANDIDATE
// ============================================

void addCandidate() {
    if(candidateCount >= MAX_CANDIDATES) {
        printError("Maximum candidate limit reached!");
        printf("[!] Maximum %d candidates allowed.\n", MAX_CANDIDATES);
        return;
    }
    
    printHeader("ADD NEW CANDIDATE");
    
    Candidate newCandidate;
    newCandidate.id = candidateCount + 1;
    newCandidate.votes = 0;
    
    // Get candidate name
    printf("Enter Candidate Name: ");
    fgets(newCandidate.name, MAX_NAME_LENGTH, stdin);
    newCandidate.name[strcspn(newCandidate.name, "\n")] = 0;
    
    if(strlen(newCandidate.name) == 0) {
        printError("Candidate name cannot be empty!");
        return;
    }
    
    // Get party name
    printf("Enter Party Name: ");
    fgets(newCandidate.party, MAX_NAME_LENGTH, stdin);
    newCandidate.party[strcspn(newCandidate.party, "\n")] = 0;
    
    if(strlen(newCandidate.party) == 0) {
        printError("Party name cannot be empty!");
        return;
    }
    
    // Get age
    printf("Enter Age: ");
    scanf("%d", &newCandidate.age);
    clearInputBuffer();
    
    if(newCandidate.age < 18 || newCandidate.age > 100) {
        printError("Invalid age! Must be between 18 and 100.");
        return;
    }
    
    // Get education
    printf("Enter Education: ");
    fgets(newCandidate.education, 100, stdin);
    newCandidate.education[strcspn(newCandidate.education, "\n")] = 0;
    
    // Get manifesto
    printf("Enter Manifesto: ");
    fgets(newCandidate.manifesto, 200, stdin);
    newCandidate.manifesto[strcspn(newCandidate.manifesto, "\n")] = 0;
    
    // Add candidate to array
    candidates[candidateCount] = newCandidate;
    candidateCount++;
    
    printSuccess("Candidate added successfully!");
    printf("\n========================================\n");
    printf("        CANDIDATE DETAILS\n");
    printf("========================================\n");
    printf(" ID:        %d\n", newCandidate.id);
    printf(" Name:      %s\n", newCandidate.name);
    printf(" Party:     %s\n", newCandidate.party);
    printf(" Age:       %d\n", newCandidate.age);
    printf(" Education: %s\n", newCandidate.education);
    printf(" Manifesto: %s\n", newCandidate.manifesto);
    printf("========================================\n");
    
    logActivity("Candidate added by admin");
    saveData();
}

// ============================================
// HELPER FUNCTIONS
// ============================================

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int isElectionActive() {
    time_t currentTime;
    time(&currentTime);
    
    if(difftime(currentTime, electionStartTime) < 0) {
        return 0; // Not started
    }
    if(difftime(currentTime, electionEndTime) > 0) {
        return -1; // Ended
    }
    return 1; // Active
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
    
    // Load candidates from text file
    fp = fopen("candidates.txt", "r");
    if(fp != NULL) {
        if(fgets(line, sizeof(line), fp)) {
            sscanf(line, "TOTAL_CANDIDATES=%d", &candidateCount);
        }
        
        int idx = 0;
        while(fgets(line, sizeof(line), fp) && idx < candidateCount) {
            if(strstr(line, "CANDIDATE_") && strstr(line, "_START")) {
                // Read candidate data
                if(fgets(line, sizeof(line), fp)) {
                    sscanf(line, "ID=%d", &candidates[idx].id);
                }
                if(fgets(line, sizeof(line), fp)) {
                    sscanf(line, "Name=%[^\n]", candidates[idx].name);
                }
                if(fgets(line, sizeof(line), fp)) {
                    sscanf(line, "Party=%[^\n]", candidates[idx].party);
                }
                if(fgets(line, sizeof(line), fp)) {
                    sscanf(line, "Education=%[^\n]", candidates[idx].education);
                }
                if(fgets(line, sizeof(line), fp)) {
                    sscanf(line, "Age=%d", &candidates[idx].age);
                }
                if(fgets(line, sizeof(line), fp)) {
                    sscanf(line, "Manifesto=%[^\n]", candidates[idx].manifesto);
                }
                if(fgets(line, sizeof(line), fp)) {
                    sscanf(line, "Votes=%d", &candidates[idx].votes);
                }
                idx++;
            }
        }
        fclose(fp);
    }
    
    // Load election configuration from text file
    fp = fopen("election_config.txt", "r");
    if(fp != NULL) {
        long startTime, endTime;
        if(fgets(line, sizeof(line), fp)) {
            sscanf(line, "ElectionStartTime=%ld", &startTime);
            electionStartTime = (time_t)startTime;
        }
        if(fgets(line, sizeof(line), fp)) {
            sscanf(line, "ElectionEndTime=%ld", &endTime);
            electionEndTime = (time_t)endTime;
        }
        fclose(fp);
    }
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
// MAIN FUNCTION
// ============================================

int main() {
    // Initialize election times
    time(&electionStartTime);
    electionEndTime = electionStartTime + (7 * 24 * 60 * 60); // 7 days from now
    
    // Load existing data from files
    loadData();
    
    printf("\n========================================\n");
    printf("   ADMIN PANEL MODULE\n");
    printf("========================================\n");
    
    adminPanel();
    
    return 0;
}