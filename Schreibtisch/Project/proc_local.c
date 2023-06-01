#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static
void print_pid_files(char *filename, char *header, int max_lines, FILE *fpo) {
    char buffer[1001];
    int n;
    FILE *fp;
    if (max_lines == 0) return;
    fp = fopen(filename, "r");
    if (!fp) {
        perror("open status failed");
        exit(-1);
    }
    fprintf(stdout, "%s\n", header);
    fprintf(stdout, "---------------------------------------------------------\n");
    fprintf(fpo, "%s\n", header);
    fprintf(fpo, "---------------------------------------------------------\n");
    n = 0;
    while(feof(fp) == 0) {
        fgets(buffer,1000,fp);
        fprintf(stdout, "%s\n", buffer);
        fprintf(fpo, "%s\n", buffer);
	n++;
	if (max_lines > 0 && n == max_lines) break;
    }
    fclose(fp);
}

int main(int argc, char *argv[]) {
    int pid = getpid();
    char filename[100];
    char output[100];
    sprintf(output, "%s", "./project_out.txt");
    FILE *fpo = fopen(output, "a");
    if (!fpo) {
        perror("open output file failed");
        exit(-1);
    }
    sprintf(filename, "/proc/%d/status", pid);
    print_pid_files(filename, "status", -1, fpo);

    sprintf(filename, "/proc/%d/maps", pid);
    print_pid_files(filename, "maps", -1, fpo);
        
    sprintf(filename, "/proc/%d/statm", pid);
    print_pid_files(filename, "statm", 1, fpo);

    fprintf(stdout, "---------------------------------------------------------\n");
    fprintf(stdout, "---------------------------------------------------------\n\n");
    fprintf(fpo, "---------------------------------------------------------\n");
    fprintf(fpo, "---------------------------------------------------------\n\n");
    fclose(fpo);

    sleep(20);
    exit(0);
}
