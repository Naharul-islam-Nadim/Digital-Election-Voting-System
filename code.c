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
#define SESSION_TIMEOUT 300

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
time_t lastActivityTime;
time_t electionStartTime;
time_t electionEndTime;

// Function prototypes
void initializeCandidates();
void registerUser();
int loginUser();
void mainMenu();
void adminPanel();
void castVote();
void showCandidates();
void showCandidateDetails();
void searchCandidate();
void showResults();
void showStatistics();
void exportResults();
void resetElection();
void addCandidate();
void removeCandidate();
void clearInputBuffer();
int findUserByNID(char* nid);
void saveData();
void loadData();
void createBackup();
void logActivity(char* activity);
int validateNID(char* nid);
int validatePassword(char* password);
void hashPassword(char* password, char* hashedPassword);
int checkSession();
void setElectionPeriod();
int isElectionActive();
void printHeader(char* title);
void printSuccess(char* message);
void printError(char* message);
void printInfo(char* message);

int main() {
    int choice;
    
    time(&lastActivityTime);
    time(&electionStartTime);
    electionEndTime = electionStartTime + (7 * 24 * 60 * 60);
    
    initializeCandidates();
    loadData();
    
    printf("\n========================================\n");
    printf("   GENERAL ELECTION VOTING SYSTEM\n");
    printf("========================================\n");
    
    while(1) {
        printf("\n---------------------------------\n");
        printf("         MAIN OPTIONS\n");
        printf("---------------------------------\n");
        printf("1. Register\n");
        printf("2. Login\n");
        printf("3. Admin Panel\n");
        printf("4. Exit\n");
        printf("\nEnter your choice: ");
        scanf("%d", &choice);
        clearInputBuffer();
        
        switch(choice) {
            case 1:
                registerUser();
                break;
            case 2:
                if(loginUser()) {
                    mainMenu();
                }
                break;
            case 3:
                adminPanel();
                break;
            case 4:
                createBackup();
                saveData();
                printSuccess("Thank you for using the Voting System!");
                logActivity("System shutdown");
                exit(0);
            default:
                printError("Invalid choice! Please try again.");
        }
    }
    
    return 0;
}

void initializeCandidates() {
    candidateCount = 5;
    
    strcpy(candidates[0].name, "John Smith");
    strcpy(candidates[0].party, "Democratic Party");
    strcpy(candidates[0].education, "MBA from Harvard University");
    candidates[0].age = 52;
    strcpy(candidates[0].manifesto, "Focus on healthcare reform and education");
    candidates[0].id = 1;
    candidates[0].votes = 0;
    
    strcpy(candidates[1].name, "Sarah Johnson");
    strcpy(candidates[1].party, "Republican Party");
    strcpy(candidates[1].education, "Law Degree from Yale");
    candidates[1].age = 48;
    strcpy(candidates[1].manifesto, "Economic growth and tax reforms");
    candidates[1].id = 2;
    candidates[1].votes = 0;
    
    strcpy(candidates[2].name, "Michael Brown");
    strcpy(candidates[2].party, "Independent");
    strcpy(candidates[2].education, "PhD in Economics");
    candidates[2].age = 45;
    strcpy(candidates[2].manifesto, "Environmental protection and sustainability");
    candidates[2].id = 3;
    candidates[2].votes = 0;
    
    strcpy(candidates[3].name, "Emily Davis");
    strcpy(candidates[3].party, "Green Party");
    strcpy(candidates[3].education, "MS in Environmental Science");
    candidates[3].age = 42;
    strcpy(candidates[3].manifesto, "Climate action and renewable energy");
    candidates[3].id = 4;
    candidates[3].votes = 0;
    
    strcpy(candidates[4].name, "Robert Wilson");
    strcpy(candidates[4].party, "Libertarian Party");
    strcpy(candidates[4].education, "BA in Political Science");
    candidates[4].age = 55;
    strcpy(candidates[4].manifesto, "Individual freedom and limited government");
    candidates[4].id = 5;
    candidates[4].votes = 0;
}

int validateNID(char* nid) {
    int len = strlen(nid);
    
    if(len < 10 || len > 17) {
        return 0;
    }
    
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
    
    if(len < 8) {
        return 0;
    }
    
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
    
    printf("1. Enter your Full Name: ");
    fgets(fullName, MAX_NAME_LENGTH, stdin);
    fullName[strcspn(fullName, "\n")] = 0;
    
    if(strlen(fullName) == 0) {
        printError("Full name cannot be empty!");
        return;
    }
    
    printf("2. Enter your NID Number (10-17 digits): ");
    fgets(nidNumber, NID_LENGTH, stdin);
    nidNumber[strcspn(nidNumber, "\n")] = 0;
    
    if(!validateNID(nidNumber)) {
        printError("Invalid NID! Must be 10-17 digits only.");
        return;
    }
    
    if(findUserByNID(nidNumber) != -1) {
        printError("This NID is already registered!");
        return;
    }
    
    printf("3. Enter a Password (min 8 chars, 1 uppercase, 1 lowercase, 1 digit): ");
    fgets(password, MAX_PASSWORD_LENGTH, stdin);
    password[strcspn(password, "\n")] = 0;
    
    if(!validatePassword(password)) {
        printError("Weak password! Must have 8+ chars, uppercase, lowercase, and digit.");
        return;
    }
    
    hashPassword(password, hashedPassword);
    
    strcpy(users[userCount].fullName, fullName);
    strcpy(users[userCount].nidNumber, nidNumber);
    strcpy(users[userCount].password, hashedPassword);
    users[userCount].hasVoted = 0;
    users[userCount].voteTime = 0;
    userCount++;
    
    printSuccess("Registration successful!");
    printf("Name: %s\n", fullName);
    printf("NID: %s\n", nidNumber);
    printInfo("You can now login with your NID number.");
    
    logActivity("User registered");
    saveData();
}

int loginUser() {
    char nidNumber[NID_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char hashedPassword[MAX_PASSWORD_LENGTH];
    
    printHeader("USER LOGIN");
    
    printf("1. Enter your NID Number: ");
    fgets(nidNumber, NID_LENGTH, stdin);
    nidNumber[strcspn(nidNumber, "\n")] = 0;
    
    printf("2. Enter your Password: ");
    fgets(password, MAX_PASSWORD_LENGTH, stdin);
    password[strcspn(password, "\n")] = 0;
    
    hashPassword(password, hashedPassword);
    int userIndex = findUserByNID(nidNumber);
    
    if(userIndex != -1 && strcmp(users[userIndex].password, hashedPassword) == 0) {
        currentUserIndex = userIndex;
        time(&lastActivityTime);
        printSuccess("Login successful!");
        printf("Welcome, %s!\n", users[userIndex].fullName);
        logActivity("User logged in");
        return 1;
    } else {
        printError("Invalid NID number or password!");
        logActivity("Failed login attempt");
        return 0;
    }
}

int checkSession() {
    time_t currentTime;
    time(&currentTime);
    
    if(difftime(currentTime, lastActivityTime) > SESSION_TIMEOUT) {
        printError("Session timeout! Please login again.");
        currentUserIndex = -1;
        return 0;
    }
    
    time(&lastActivityTime);
    return 1;
}

int isElectionActive() {
    time_t currentTime;
    time(&currentTime);
    
    if(difftime(currentTime, electionStartTime) < 0) {
        return 0;
    }
    if(difftime(currentTime, electionEndTime) > 0) {
        return -1;
    }
    return 1;
}

void mainMenu() {
    int choice;
    
    while(1) {
        if(!checkSession()) {
            return;
        }
        
        printHeader("MAIN MENU");
        printf("Logged in as: %s\n", users[currentUserIndex].fullName);
        printf("========================================\n");
        printf("1. Cast Vote\n");
        printf("2. Show All Candidates\n");
        printf("3. View Candidate Details\n");
        printf("4. Search Candidate\n");
        printf("5. Show Results\n");
        printf("6. View Statistics\n");
        printf("7. Logout\n");
        printf("========================================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clearInputBuffer();
        
        switch(choice) {
            case 1:
                castVote();
                break;
            case 2:
                showCandidates();
                break;
            case 3:
                showCandidateDetails();
                break;
            case 4:
                searchCandidate();
                break;
            case 5:
                showResults();
                break;
            case 6:
                showStatistics();
                break;
            case 7:
                printSuccess("Logged out successfully!");
                printf("Goodbye, %s!\n", users[currentUserIndex].fullName);
                logActivity("User logged out");
                currentUserIndex = -1;
                return;
            default:
                printError("Invalid choice! Please try again.");
        }
    }
}

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
        printf("5. Remove Candidate\n");
        printf("6. Create Backup\n");
        printf("7. Set Election Period\n");
        printf("8. Exit Admin Panel\n");
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
                removeCandidate();
                break;
            case 6:
                createBackup();
                break;
            case 7:
                setElectionPeriod();
                break;
            case 8:
                printInfo("Exiting admin panel...");
                return;
            default:
                printError("Invalid choice!");
        }
    }
}

void castVote() {
    if(!checkSession()) return;
    
    int status = isElectionActive();
    if(status == 0) {
        printError("Election has not started yet!");
        return;
    } else if(status == -1) {
        printError("Election has ended!");
        return;
    }
    
    if(users[currentUserIndex].hasVoted) {
        printError("You have already cast your vote!");
        printf("[!] One person can only vote once.\n");
        
        char timeStr[100];
        struct tm *timeInfo = localtime(&users[currentUserIndex].voteTime);
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeInfo);
        printf("[INFO] You voted on: %s\n", timeStr);
        return;
    }
    
    printHeader("CAST YOUR VOTE");
    showCandidates();
    
    int candidateId;
    printf("\nEnter the ID of the candidate you want to vote for: ");
    scanf("%d", &candidateId);
    clearInputBuffer();
    
    if(candidateId < 1 || candidateId > candidateCount) {
        printError("Invalid candidate ID!");
        return;
    }
    
    printf("\n[!] CONFIRMATION REQUIRED\n");
    printf("You are about to vote for:\n");
    printf("=> %s (%s)\n", candidates[candidateId-1].name, candidates[candidateId-1].party);
    printf("\nAre you sure? (Y/N): ");
    
    char confirm;
    scanf(" %c", &confirm);
    clearInputBuffer();
    
    if(confirm != 'Y' && confirm != 'y') {
        printInfo("Vote cancelled.");
        return;
    }
    
    candidates[candidateId - 1].votes++;
    users[currentUserIndex].hasVoted = 1;
    time(&users[currentUserIndex].voteTime);
    
    printSuccess("Vote cast successfully!");
    printf("\n========================================\n");
    printf("        VOTING RECEIPT\n");
    printf("========================================\n");
    printf(" Voter: %s\n", users[currentUserIndex].fullName);
    printf(" Candidate: %s\n", candidates[candidateId-1].name);
    printf(" Party: %s\n", candidates[candidateId-1].party);
    
    char timeStr[100];
    struct tm *timeInfo = localtime(&users[currentUserIndex].voteTime);
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeInfo);
    printf(" Time: %s\n", timeStr);
    printf("========================================\n");
    
    printf("\nThank you for voting, %s!\n", users[currentUserIndex].fullName);
    
    logActivity("Vote cast");
    saveData();
}

void showCandidates() {
    printHeader("LIST OF CANDIDATES");
    printf("%-4s %-25s %-25s\n", "ID", "Name", "Party");
    printf("========================================\n");
    
    for(int i = 0; i < candidateCount; i++) {
        printf("%-4d %-25s %-25s\n", 
               candidates[i].id,
               candidates[i].name, 
               candidates[i].party);
    }
}

void showCandidateDetails() {
    int id;
    printf("\nEnter Candidate ID to view details: ");
    scanf("%d", &id);
    clearInputBuffer();
    
    if(id < 1 || id > candidateCount) {
        printError("Invalid candidate ID!");
        return;
    }
    
    Candidate c = candidates[id-1];
    
    printf("\n========================================\n");
    printf("        CANDIDATE PROFILE\n");
    printf("========================================\n");
    printf(" Name:      %s\n", c.name);
    printf(" Party:     %s\n", c.party);
    printf(" Age:       %d\n", c.age);
    printf(" Education: %s\n", c.education);
    printf(" Manifesto: %s\n", c.manifesto);
    printf("========================================\n");
}

void searchCandidate() {
    char searchTerm[MAX_NAME_LENGTH];
    int found = 0;
    
    printf("\nEnter candidate name or party to search: ");
    fgets(searchTerm, MAX_NAME_LENGTH, stdin);
    searchTerm[strcspn(searchTerm, "\n")] = 0;
    
    for(int i = 0; searchTerm[i]; i++) {
        searchTerm[i] = tolower(searchTerm[i]);
    }
    
    printHeader("SEARCH RESULTS");
    
    for(int i = 0; i < candidateCount; i++) {
        char name[MAX_NAME_LENGTH], party[MAX_NAME_LENGTH];
        strcpy(name, candidates[i].name);
        strcpy(party, candidates[i].party);
        
        for(int j = 0; name[j]; j++) name[j] = tolower(name[j]);
        for(int j = 0; party[j]; j++) party[j] = tolower(party[j]);
        
        if(strstr(name, searchTerm) || strstr(party, searchTerm)) {
            printf("[+] [%d] %s - %s\n", 
                   candidates[i].id, 
                   candidates[i].name, 
                   candidates[i].party);
            found = 1;
        }
    }
    
    if(!found) {
        printError("No candidates found matching your search.");
    }
}

void showResults() {
    if(!checkSession()) return;
    
    int totalVotes = 0;
    for(int i = 0; i < candidateCount; i++) {
        totalVotes += candidates[i].votes;
    }
    
    printHeader("ELECTION RESULTS");
    printf("%-25s %-25s %-10s %-12s\n", "Candidate", "Party", "Votes", "Percentage");
    printf("========================================================================\n");
    
    for(int i = 0; i < candidateCount; i++) {
        float percentage = (totalVotes > 0) ? (candidates[i].votes * 100.0 / totalVotes) : 0;
        printf("%-25s %-25s %-10d %.2f%%\n", 
               candidates[i].name, 
               candidates[i].party,
               candidates[i].votes,
               percentage);
    }
    
    printf("========================================================================\n");
    printf("Total Votes Cast: %d\n", totalVotes);
    
    int maxVotes = -1;
    int winnerIndex = -1;
    for(int i = 0; i < candidateCount; i++) {
        if(candidates[i].votes > maxVotes) {
            maxVotes = candidates[i].votes;
            winnerIndex = i;
        }
    }
    
    if(winnerIndex != -1 && maxVotes > 0) {
        printf("\n[CURRENT LEADER]\n");
        printf("   %s (%s) with %d votes\n",
               candidates[winnerIndex].name,
               candidates[winnerIndex].party,
               maxVotes);
    } else {
        printInfo("No votes cast yet.");
    }
}

void showStatistics() {
    int totalVotes = 0;
    int votedUsers = 0;
    
    for(int i = 0; i < candidateCount; i++) {
        totalVotes += candidates[i].votes;
    }
    
    for(int i = 0; i < userCount; i++) {
        if(users[i].hasVoted) votedUsers++;
    }
    
    float turnout = (userCount > 0) ? (votedUsers * 100.0 / userCount) : 0;
    
    printHeader("ELECTION STATISTICS");
    printf("Total Registered Users:  %d\n", userCount);
    printf("Users Who Voted:         %d\n", votedUsers);
    printf("Users Who Haven't Voted: %d\n", userCount - votedUsers);
    printf("Voter Turnout:           %.2f%%\n", turnout);
    printf("Total Votes Cast:        %d\n", totalVotes);
    printf("Total Candidates:        %d\n", candidateCount);
    
    char startStr[100], endStr[100];
    struct tm *timeInfo;
    
    timeInfo = localtime(&electionStartTime);
    strftime(startStr, sizeof(startStr), "%Y-%m-%d %H:%M", timeInfo);
    
    timeInfo = localtime(&electionEndTime);
    strftime(endStr, sizeof(endStr), "%Y-%m-%d %H:%M", timeInfo);
    
    printf("\nElection Period:\n");
    printf("   Start: %s\n", startStr);
    printf("   End:   %s\n", endStr);
    
    int status = isElectionActive();
    if(status == 1) {
        printf("   Status: ACTIVE\n");
    } else if(status == 0) {
        printf("   Status: NOT STARTED\n");
    } else {
        printf("   Status: ENDED\n");
    }
}

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
    
    fprintf(fp, "===================================================\n");
    fprintf(fp, "          GENERAL ELECTION RESULTS\n");
    fprintf(fp, "===================================================\n\n");
    
    time_t now;
    time(&now);
    fprintf(fp, "Report Generated: %s\n", ctime(&now));
    
    fprintf(fp, "\nCandidate Results:\n");
    fprintf(fp, "---------------------------------------------------\n");
    
    for(int i = 0; i < candidateCount; i++) {
        float percentage = (totalVotes > 0) ? (candidates[i].votes * 100.0 / totalVotes) : 0;
        fprintf(fp, "%d. %-25s (%-20s) : %d votes (%.2f%%)\n", 
                i+1, candidates[i].name, candidates[i].party, 
                candidates[i].votes, percentage);
    }
    
    fprintf(fp, "\n---------------------------------------------------\n");
    fprintf(fp, "Total Votes Cast: %d\n", totalVotes);
    fprintf(fp, "Total Registered Users: %d\n", userCount);
    
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
    
    for(int i = 0; i < candidateCount; i++) {
        candidates[i].votes = 0;
    }
    
    for(int i = 0; i < userCount; i++) {
        users[i].hasVoted = 0;
        users[i].voteTime = 0;
    }
    
    printSuccess("Election reset successfully!");
    logActivity("Election reset by admin");
    saveData();
}

void addCandidate() {
    if(candidateCount >= MAX_CANDIDATES) {
        printError("Maximum candidate limit reached!");
        return;
    }
    
    printHeader("ADD NEW CANDIDATE");
    
    Candidate newCandidate;
    newCandidate.id = candidateCount + 1;
    newCandidate.votes = 0;
    
    printf("Enter Candidate Name: ");
    fgets(newCandidate.name, MAX_NAME_LENGTH, stdin);
    newCandidate.name[strcspn(newCandidate.name, "\n")] = 0;
    
    printf("Enter Party Name: ");
    fgets(newCandidate.party, MAX_NAME_LENGTH, stdin);
    newCandidate.party[strcspn(newCandidate.party, "\n")] = 0;
    
    printf("Enter Age: ");
    scanf("%d", &newCandidate.age);
    clearInputBuffer();
    
    printf("Enter Education: ");
    fgets(newCandidate.education, 100, stdin);
    newCandidate.education[strcspn(newCandidate.education, "\n")] = 0;
    
    printf("Enter Manifesto: ");
    fgets(newCandidate.manifesto, 200, stdin);
    newCandidate.manifesto[strcspn(newCandidate.manifesto, "\n")] = 0;
    
    candidates[candidateCount] = newCandidate;
    candidateCount++;
    
    printSuccess("Candidate added successfully!");
    logActivity("Candidate added by admin");
    saveData();
}

void removeCandidate() {
    int id;
    
    showCandidates();
    printf("\nEnter Candidate ID to remove: ");
    scanf("%d", &id);
    clearInputBuffer();
    
    if(id < 1 || id > candidateCount) {
        printError("Invalid candidate ID!");
        return;
    }
    
    printf("\n[!] Remove %s? (Y/N): ", candidates[id-1].name);
    char confirm;
    scanf(" %c", &confirm);
    clearInputBuffer();
    
    if(confirm != 'Y' && confirm != 'y') {
        printInfo("Removal cancelled.");
        return;
    }
    
    for(int i = id-1; i < candidateCount-1; i++) {
        candidates[i] = candidates[i+1];
        candidates[i].id = i + 1;
    }
    candidateCount--;
    
    printSuccess("Candidate removed successfully!");
    logActivity("Candidate removed by admin");
    saveData();
}

void createBackup() {
    char backupUsers[50], backupCandidates[50];
    time_t now;
    time(&now);
    struct tm *timeInfo = localtime(&now);
    
    sprintf(backupUsers, "backup_users_%04d%02d%02d_%02d%02d%02d.txt",
            timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday,
            timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);
    
    sprintf(backupCandidates, "backup_candidates_%04d%02d%02d_%02d%02d%02d.txt",
            timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday,
            timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);
    
    FILE *fp;
    
    // Backup users
    fp = fopen(backupUsers, "w");
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
    
    // Backup candidates
    fp = fopen(backupCandidates, "w");
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
    
    printSuccess("Backup created successfully!");
    printf("Files: %s, %s\n", backupUsers, backupCandidates);
    logActivity("Backup created");
}

void setElectionPeriod() {
    int days;
    
    printHeader("SET ELECTION PERIOD");
    printf("Enter election duration in days: ");
    scanf("%d", &days);
    clearInputBuffer();
    
    if(days < 1 || days > 365) {
        printError("Invalid duration! Must be 1-365 days.");
        return;
    }
    
    time(&electionStartTime);
    electionEndTime = electionStartTime + (days * 24 * 60 * 60);
    
    char startStr[100], endStr[100];
    struct tm *timeInfo;
    
    timeInfo = localtime(&electionStartTime);
    strftime(startStr, sizeof(startStr), "%Y-%m-%d %H:%M:%S", timeInfo);
    
    timeInfo = localtime(&electionEndTime);
    strftime(endStr, sizeof(endStr), "%Y-%m-%d %H:%M:%S", timeInfo);
    
    printSuccess("Election period set successfully!");
    printf("Start: %s\n", startStr);
    printf("End:   %s\n", endStr);
    
    logActivity("Election period updated");
    saveData();
}

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
    return -1;
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
    
    // Save election configuration to text file
    fp = fopen("election_config.txt", "w");
    if(fp != NULL) {
        fprintf(fp, "ElectionStartTime=%ld\n", (long)electionStartTime);
        fprintf(fp, "ElectionEndTime=%ld\n", (long)electionEndTime);
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