#include <stdio.h>      // Standard-Ein-/Ausgabe-Funktionen
#include <stdlib.h>     // Allgemeine Funktionen (z.B. Speicherallokation)
#include <unistd.h>     // POSIX-Betriebssystem-API

// Funktion zum Drucken von PID-Dateien
static void print_pid_files(char *filename, char *header, int max_lines, FILE *fpo) {
    char buffer[1001];
    int n;
    FILE *fp;
    
    if (max_lines == 0) return; // Abbruch, wenn max_lines 0 ist
    
    fp = fopen(filename, "r");
    if (!fp) {
        perror("Fehler beim Öffnen der Statusdatei"); // Fehlermeldung, falls Statusdatei nicht geöffnet werden kann
        exit(-1);
    }
    
    fprintf(stdout, "%s\n", header);
    fprintf(stdout, "---------------------------------------------------------\n");
    fprintf(fpo, "%s\n", header);
    fprintf(fpo, "---------------------------------------------------------\n");
    
    n = 0;
    while(feof(fp) == 0) {
        fgets(buffer, 1000, fp); // Zeile aus Datei lesen
        fprintf(stdout, "%s\n", buffer); // Zeile auf der Konsole ausgeben
        fprintf(fpo, "%s\n", buffer); // Zeile in der Ausgabedatei speichern
        n++;
        if (max_lines > 0 && n == max_lines) break; // Abbruch nach max_lines Zeilen
    }
    
    fclose(fp);
}

int main(int argc, char *argv[]) {
    int pid = getpid(); // Aktuelle Prozess-ID
    char filename[100];
    char output[100];
    sprintf(output, "%s", "./project_out.txt"); // Ausgabedatei festlegen
    FILE *fpo = fopen(output, "a");
    if (!fpo) {
        perror("Fehler beim Öffnen der Ausgabedatei"); // Fehlermeldung, falls Ausgabedatei nicht geöffnet werden kann
        exit(-1);
    }
    
    sprintf(filename, "/proc/%d/status", pid); // Pfad zur Statusdatei des aktuellen Prozesses erstellen
    print_pid_files(filename, "status", -1, fpo); // Statusdatei ausgeben (-1 für alle Zeilen)
    
    sprintf(filename, "/proc/%d/maps", pid); // Pfad zur Maps-Datei des aktuellen Prozesses erstellen
    print_pid_files(filename, "maps", -1, fpo); // Maps-Datei ausgeben (-1 für alle Zeilen)
        
    sprintf(filename, "/proc/%d/statm", pid); // Pfad zur Statm-Datei des aktuellen Prozesses erstellen
    print_pid_files(filename, "statm", 1, fpo); // Statm-Datei ausgeben (nur eine Zeile)
    
    fprintf(stdout, "---------------------------------------------------------\n");
    fprintf(stdout, "---------------------------------------------------------\n\n");
    fprintf(fpo, "---------------------------------------------------------\n");
    fprintf(fpo, "---------------------------------------------------------\n\n");
    fclose(fpo);

    sleep(20); // 20 Sekunden warten
    exit(0);
}
