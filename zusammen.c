#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int istGueltigeNummer(char *text, long *ergebnis)
{
    char *endptr;
    *ergebnis = strtol(text, &endptr, 10);
    if (endptr == text || *endptr != '\0' || *ergebnis <= 0)
    {
        return 0;
    }
    return 1;
}

FILE *oeffneDatei(char *dateiname, char *modus)
{
    return fopen(dateiname, modus);
}

int loescheItem(long itemNummer)
{
    FILE *datei, *tempDatei;
    char zeile[256];
    int zeilenNummer = 0;
    int gefunden = 0;

    datei = oeffneDatei("todo.txt", "r");

    if (datei == NULL)
    {
        printf("Datei konnte nicht geöffnet werden !\n");
        return 0;
    }
    tempDatei = oeffneDatei("temp.txt", "w");

    if (tempDatei == NULL)
    {
        fclose(datei);
        return 0;
    }
    while (fgets(zeile, 256, datei) != NULL)
    {
        zeilenNummer++;

        if (zeilenNummer == itemNummer)
        {
            gefunden = 1;
        }

        if (zeilenNummer != itemNummer)
        {
            fprintf(tempDatei, "%s", zeile);
        }
    }
    fclose(datei);
    fclose(tempDatei);

    if (gefunden == 0)
    {
        remove("temp.txt");
        printf("Item %ld existiert nicht!\n", itemNummer);
        return 0;
    }
    remove("todo.txt");
    rename("temp.txt", "todo.txt");
    printf("Item %ld wurde gelöscht!\n", itemNummer);
    return 1;
}

int aendereStatus(long itemNummer, int erledigt)
{
    FILE *datei, *tempDatei;
    char zeile[256];
    int zeilenNummer = 0;
    int gefunden = 0;

    datei = oeffneDatei("todo.txt", "r");

    if (datei == NULL)
    {
        printf("Datei konnte nicht geöffnet werden !\n");
        return 0;
    }

    tempDatei = oeffneDatei("temp.txt", "w");

    if (tempDatei == NULL)
    {
        fclose(datei);
        return 0;
    }
    while (fgets(zeile, 256, datei) != NULL)
    {
        zeilenNummer++;

        if (zeilenNummer == itemNummer)
        {
            gefunden = 1;

            if (erledigt == 1)
            {
                fprintf(tempDatei, "[ERLEDIGT] %s", zeile);
            }
            else
            {
                if (strncmp(zeile, "[ERLEDIGT] ", 11) == 0)
                {
                    fprintf(tempDatei, "%s", zeile + 11);
                }
                else
                {
                    fprintf(tempDatei, "%s", zeile);
                }
            }
        }
        else
        {
            fprintf(tempDatei, "%s", zeile);
        }
    }
    fclose(datei);
    fclose(tempDatei);
    if (gefunden == 0)
    {
        remove("temp.txt");
        printf("Item %ld existiert nicht !\n", itemNummer);
        return 0;
    }
    remove("todo.txt");
    rename("temp.txt", "todo.txt");
    if (erledigt == 1)
    {
        printf("Item %ld wurde als erledigt markiert !\n", itemNummer);
    }
    else
    {
        printf("Item %ld wurde als nicht erledigt markiert !\n", itemNummer);
    }
    return 1;
}

int main(int argc, char *argv[])
{
    int options;
    int gefunden;
    long itemNummer;
    char zeile[256];
    int zeilenNummer = 0;
    FILE *datei;
    FILE *tempDatei;
    // Lässt nur meinen default als fehler auswerfen und lässt
    // die Fehlermeldung von getopt() nicht mehr auswerfen:
    opterr = 0;
    while ((options = getopt(argc, argv, ":hla:d:c:u:")) != -1)
    {

        switch (options)
        {
        default:
            printf("Ungültiger Itembefehl\n");
            break;

        case ':':
            printf("Bekannte Option aber fehlendes Argument !\n");
            break;

        case '?':
            printf("Unbekannte Option !\n");
            break;

        case 'h':
            printf("Hilfe wurde abgerufen !\n");
            break;

        case 'l':
            datei = oeffneDatei("todo.txt", "r");
            if (datei == NULL)
            {
                printf("Es befinden sich aktuell keine Aufgaben im Ordner 'todo.txt' !");
                return 1;
            }
            while (fgets(zeile, 256, datei) != NULL)
            {
                zeilenNummer++;
                printf("%d: %s", zeilenNummer, zeile);
            }
            fclose(datei);
            break;

        case 'a':
            datei = oeffneDatei("todo.txt", "a");
            if (datei == NULL)
            {
                printf("Datei konnte nicht geöffnet werden !");
                return 1;
            }
            fprintf(datei, "%s\n", optarg);
            fclose(datei);
            printf("Neue Aufgabe: %s\n", optarg);
            break;

        case 'd':
            if (istGueltigeNummer(optarg, &itemNummer) == 0)
            {
                printf("Ungültige Item-Nummer !\n");
                break;
            }
            loescheItem(itemNummer);
            break;
         

        case 'c':

            if (istGueltigeNummer(optarg, &itemNummer) == 0)
            {
                printf("Ungültige Item-Nummer !\n");
                break;
            }

            aendereStatus(itemNummer, 1);
            break;

        case 'u':

            if (istGueltigeNummer(optarg, &itemNummer) == 0)
            {
                printf("Ungültige Item-Nummer !\n");
                break;
            }

            aendereStatus(itemNummer, 0);
            break;
        }

        return 0;
    }
}
