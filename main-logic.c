// Cast Vote Function
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
    if(scanf("%d", &candidateId) != 1) {
        clearInputBuffer();
        printError("Invalid input!");
        return;
    }
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

// Show All Candidates Function
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

// View Candidate Details Function
void showCandidateDetails() {
    int id;
    printf("\nEnter Candidate ID to view details: ");
    if(scanf("%d", &id) != 1) {
        clearInputBuffer();
        printError("Invalid input!");
        return;
    }
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

// Search Candidate Function
void searchCandidate() {
    char searchTerm[MAX_NAME_LENGTH];
    int found = 0;
    
    printf("\nEnter candidate name or party to search: ");
    if(fgets(searchTerm, MAX_NAME_LENGTH, stdin) == NULL) {
        printError("Error reading input!");
        return;
    }
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

// Show Results Function
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

// View Statistics Function
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

// Supporting Helper Functions for Voter Section

int isElectionActive() {
    time_t currentTime;
    time(&currentTime);
    
    if(difftime(currentTime, electionStartTime) < 0) {
        return 0;  // Not started
    }
    if(difftime(currentTime, electionEndTime) > 0) {
        return -1;  // Ended
    }
    return 1;  // Active
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
    
    // Save election configuration to text file
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