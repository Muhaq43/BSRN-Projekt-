#include <stdio.h>      // Standard-Ein-/Ausgabe-Funktionen
#include <stdlib.h>     // Allgemeine Funktionen (z.B. Speicherallokation)
#include <unistd.h>     // POSIX-Betriebssystem-API
#include <signal.h>     // Signal-Handling-Funktionen
#include <sys/types.h>  // Systemdatentypen (z.B. pid_t)

// Funktion zum Erzeugen eines Kindprozesses
static int do_fork() {
    int pid;

    if ((pid = fork()) < 0) { // Forken des aktuellen Prozesses
        perror("Fehler bei fork"); // Fehlermeldung, falls Fork fehlschlägt
        return 2;
    }
    if (pid == 0) { // Kindprozess
        // Stdout auf die Datei umleiten
        freopen("./project_out.txt", "a", stdout);

        // Programm 'proc_local' ausführen
        execlp("./proc_local", "proc_local", (char *) 0);
        perror("proc_local fehlgeschlagen"); // Fehlermeldung, falls Ausführung fehlschlägt
        return 3;
    }
    return pid; // Rückgabe der PID des Kindprozesses
}

// Funktion zum Drucken des Inhalts einer Datei
void printFileContents(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Fehler beim Öffnen der Datei"); // Fehlermeldung, falls Datei nicht geöffnet werden kann
        return;
    }

    printf("Inhalt von %s:\n", filename);
    printf("--------------------\n");

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("%s", buffer); // Zeile aus Datei drucken
    }

    fclose(file);
}

// Funktion zum Beenden aller Kindprozesse
void killAllChildProcesses() {
    pid_t myPid = getpid(); // PID des aktuellen Prozesses
    pid_t pgrp = getpgrp(); // Gruppen-ID des aktuellen Prozesses

    // Array zum Speichern der Kindprozess-IDs erstellen
    pid_t childPids[100];
    int childCount = 0;

    // Alle Kindprozess-IDs finden
    pid_t pid;
    for (pid = 1; pid < myPid; pid++) {
        if (getpgid(pid) == pgrp) { // Prüfen, ob Prozess zur gleichen Gruppe gehört
            childPids[childCount++] = pid; // Kindprozess-ID speichern
        }
    }

    // Jeden Kindprozess beenden
    int i;
    for (i = 0; i < childCount; i++) {
        if (kill(childPids[i], SIGTERM) == -1) { // Kindprozess mit SIGTERM beenden
            perror("Fehler beim Beenden des Kindprozesses"); // Fehlermeldung, falls Beenden fehlschlägt
        }
    }

    printf("Alle Kindprozesse beendet.\n");
}

int main() {
    int n_process = 0;
    int i;
    int ret;
    char output[100];
    sprintf(output, "%s", "./project_out.txt"); // Ausgabedatei festlegen
    FILE *fpo = fopen(output, "w");
    if (!fpo) {
        perror("Fehler beim Öffnen der Ausgabedatei"); // Fehlermeldung, falls Ausgabedatei nicht geöffnet werden kann
        exit(-1);
    }
    ret = 0;

    fprintf(fpo, "Prozesserzeugung und Verzweigung:\n");

    // Menüschleife
    while (1) {
        printf("1. Prozesse erstellen\n");
        printf("2. project_out.txt lesen\n");
        printf("3. Beenden\n");
        printf("Geben Sie Ihre Wahl ein: ");
        int choice;
        scanf("%d", &choice); // Benutzereingabe auslesen

        switch (choice) {
            case 1: {
                // Prozesse erstellen
                if (n_process > 0) {
                    printf("Prozesse bereits erstellt.\n"); // Fehlermeldung, falls Prozesse bereits erstellt wurden
                } else {
                    printf("Geben Sie die Anzahl der zu erstellenden Prozesse ein (größer als 0): ");
                    scanf("%d", &n_process); // Anzahl der zu erstellenden Prozesse auslesen

                    if (n_process <= 0) {
                        printf("Ungültige Anzahl von Prozessen.\n"); // Fehlermeldung, falls ungültige Anzahl
                        n_process = 0;
                    } else {
                        printf("%d Prozess(e) wird/werden erstellt...\n", n_process);
                        printf("Elternprozess (PID: %d) verzweigt in:\n", getpid()); // PID des Elternprozesses ausgeben
                        fprintf(fpo, "Elternprozess (PID: %d) verzweigt in:\n", getpid()); // PID des Elternprozesses in Ausgabedatei schreiben
                        for (i = 0; i < n_process; i++) {
                            ret = do_fork(); // Kindprozess erzeugen
                            if (ret <= 0) {
                                printf("Fehler beim Erstellen des %d. Prozesses.\n", i + 1); // Fehlermeldung, falls Erzeugung fehlschlägt
                                exit(ret);
                            }
                            if (ret > 0) {
                                printf("  |- Kindprozess %d (PID: %d)\n", i + 1, ret); // PID des Kindprozesses ausgeben
                                fprintf(fpo, "Kindprozess %d (PID: %d)\n", i + 1, ret); // PID des Kindprozesses in Ausgabedatei schreiben
                            }
                            sleep(2); // Kurze Verzögerung zwischen den Kindprozessen
                        }
                        printf("Prozesse erfolgreich erstellt.\n");
                    }
                }
                break;
            }
            case 2: {
                // project_out.txt lesen
                printFileContents(output); // Dateiinhalt ausgeben
                break;
            }
            case 3: {
                // Beenden
                if (n_process > 0) {
                    killAllChildProcesses(); // Alle Kindprozesse beenden
                }
                printf("Programm wird beendet.\n");
                fclose(fpo); // Ausgabedatei schließen
                exit(0); // Programm beenden
                break;
            }
            default:
                printf("Ungültige Auswahl. Bitte versuchen Sie es erneut.\n"); // Fehlermeldung, falls ungültige Auswahl
        }
    }
}
