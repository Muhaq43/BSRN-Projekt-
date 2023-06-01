
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

static int do_fork() {
    int pid;

    if ((pid = fork()) < 0) {
        perror("fork failed");
        return 2;
    }
    if (pid == 0) {
        // Redirect stdout to the file
        freopen("./project_out.txt", "a", stdout);

        execlp("./proc_local", "proc_local", (char *) 0);
        perror("proc_local failed");
        return 3;
    }
    return pid;
}

void printFileContents(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("open file failed");
        return;
    }

    printf("Contents of %s:\n", filename);
    printf("--------------------\n");

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("%s", buffer);
    }

    fclose(file);
}

// Function to kill all child processes
void killAllChildProcesses() {
    pid_t myPid = getpid();
    pid_t pgrp = getpgrp();

    // Create an array to store child process IDs
    pid_t childPids[100];
    int childCount = 0;

    // Find all child process IDs
    pid_t pid;
    for (pid = 1; pid < myPid; pid++) {
        if (getpgid(pid) == pgrp) {
            childPids[childCount++] = pid;
        }
    }

    // Terminate each child process
    int i;
    for (i = 0; i < childCount; i++) {
        if (kill(childPids[i], SIGTERM) == -1) {
            perror("Error killing child process");
        }
    }

    printf("All child processes killed.\n");
}


int main() {
    int n_process = 0;
    int i;
    int ret;
    char output[100];
    sprintf(output, "%s", "./project_out.txt");
    FILE *fpo = fopen(output, "w");
    if (!fpo) {
        perror("open output file failed");
        exit(-1);
    }
    ret = 0;

    fprintf(fpo, "Process Generation and Branching:\n");

    // Menu loop
    while (1) {
        printf("1. Create processes\n");
        printf("2. Read project_out.txt\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        int choice;
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                // Create processes
                if (n_process > 0) {
                    printf("Processes already created.\n");
                } else {
                    printf("Enter the number of processes to create (greater than 0): ");
                    scanf("%d", &n_process);

                    if (n_process <= 0) {
                        printf("Invalid number of processes.\n");
                        n_process = 0;
                    } else {
                        printf("Creating %d processes...\n", n_process);
                        printf("Parent Process (PID: %d) forks into:\n", getpid());
                        fprintf(fpo, "Parent Process (PID: %d) forks into:\n", getpid());
                        for (i = 0; i < n_process; i++) {
                            ret = do_fork();
                            if (ret <= 0) {
                                printf("Failed to create process %d.\n", i + 1);
                                exit(ret);
                            }
                            if (ret > 0) {
                                printf("  |- Child Process %d (PID: %d)\n", i + 1, ret);
                                fprintf(fpo, "Child Process %d (PID: %d)\n", i + 1, ret);
                            }
                            sleep(2);
                        }
                        printf("Processes created successfully.\n");
                    }
                }
                break;
            }
            case 2: {
                // Read project_out.txt
                printFileContents(output);
                break;
            }
            case 3: {
                // Exit
                if (n_process > 0) {
                    killAllChildProcesses();
                }
                printf("Exiting the program.\n");
                fclose(fpo);
                exit(0);
                break;
            }
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}
