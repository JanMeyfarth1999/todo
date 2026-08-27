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

char *leseZeile(FILE *datei)
{
    char *zeile;
    char *temp;
    int groesse = 64;
    int laenge = 0;
    int zeichen;
    zeile = malloc(64 * sizeof(char));
    if (zeile == NULL)
    {
        printf("Speicher kann nicht reserviert werden !\n");
        return NULL;
    }
    zeichen = fgetc(datei);
    if (zeichen == EOF)
    {
        free(zeile);
        return NULL;
    }
    while (zeichen != '\n' && zeichen != EOF)
    {
        if (laenge == groesse)
        {
            groesse = groesse * 2;
            temp = realloc(zeile, groesse * sizeof(char));

            if (temp == NULL)
            {
                free(zeile);
                printf("Speicher konnte nicht erweitert werden !\n");
                return NULL;
            }

            zeile = temp;
        }

        zeile[laenge] = zeichen;
        laenge++;
        zeichen = fgetc(datei);
    }

    if (laenge == groesse)
    {
        groesse = groesse + 1;
        temp = realloc(zeile, groesse * sizeof(char));

        if (temp == NULL)
        {
            free(zeile);
            printf("Speicher konnte nicht erweitert werden !\n");
            return NULL;
        }
        zeile = temp;
    }
    zeile[laenge] = '\0';
    return zeile;
}

int zeigeListe(int filter)
{
    FILE *datei;
    char *zeile;
    int zeilenNummer = 0;

    datei = oeffneDatei("todo.txt", "r");
    if (datei == NULL)
    {
        printf("Datei konnte nicht geöffnet werden !\n");
        return 0;
    }
    zeile = leseZeile(datei);
    while (zeile != NULL)
    {
        zeilenNummer++;
        if (filter == 0)
        {
            printf("%d: %s\n", zeilenNummer, zeile);
        }
        else if (filter == 1 && strncmp(zeile, "[ERLEDIGT] ", 11) != 0)
        {
            printf("%d: %s\n", zeilenNummer, zeile);
        }
        else if (filter == 2 && strncmp(zeile, "[ERLEDIGT] ", 11) == 0)
        {
            printf("%d: %s\n", zeilenNummer, zeile);
        }
        free(zeile);
        zeile = leseZeile(datei);
    }

    fclose(datei);
    return 1;
}

int loescheItem(long itemNummer)
{
    FILE *datei, *tempDatei;
    char *zeile;
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
    zeile = leseZeile(datei);
    while (zeile != NULL)
    {
        zeilenNummer++;

        if (zeilenNummer == itemNummer)
        {
            gefunden = 1;
        }

        if (zeilenNummer != itemNummer)
        {
            fprintf(tempDatei, "%s\n", zeile);
        }
        free(zeile);
        zeile = leseZeile(datei);
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
    printf("Item %ld wurde gelöscht!\n", itemNummer);
    return 1;
}

int aendereStatus(long itemNummer, int erledigt)
{
    FILE *datei, *tempDatei;
    char *zeile;
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
    zeile = leseZeile(datei);
    while (zeile != NULL)
    {
        zeilenNummer++;

        if (zeilenNummer == itemNummer)
        {
            gefunden = 1;

            if (erledigt == 1)
            {
                fprintf(tempDatei, "[ERLEDIGT] %s\n", zeile);
            }
            else
            {
                if (strncmp(zeile, "[ERLEDIGT] ", 11) == 0)
                {
                    fprintf(tempDatei, "%s\n", zeile + 11);
                }
                else
                {
                    fprintf(tempDatei, "%s\n", zeile);
                }
            }
        }
        else
        {
            fprintf(tempDatei, "%s\n", zeile);
        }
        free(zeile);
        zeile = leseZeile(datei);
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
int aendereText(long itemNummer, char *neuerText)
{
    FILE *datei, *tempDatei;
    char *zeile;
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
    zeile = leseZeile(datei);
    while (zeile != NULL)
    {
        zeilenNummer++;

        if (zeilenNummer == itemNummer)
        {
            gefunden = 1;
            fprintf(tempDatei, "%s\n", neuerText);
        }

        if (zeilenNummer != itemNummer)
        {
            fprintf(tempDatei, "%s\n", zeile);
        }
        free(zeile);
        zeile = leseZeile(datei);
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
    printf("Item %ld wurde geändert !\n", itemNummer);
    return 1;
}
int main(int argc, char *argv[])
{
    int options;
    long itemNummer;
    int filter = 0;
    int listeAnzeigen = 0;
    FILE *datei;

    if (argc == 1)
    {
        printf("Gebe '-h' ein um die Befehle für die Todo-Liste abzurufen\n");
    }

    while ((options = getopt(argc, argv, ":hla:d:c:u:e:UC")) != -1)
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
            printf("-l = Listet alle Items Nummeriert auf.\n"
                   "-a = Fügt ein neues Item mit dem angegebenen Text hinzu.\n"
                   "-e = Ersetzt den Text von einem Item mit Nr... durch einen neuen Text\n"
                   "-d = Löscht das Item mit einer eingebenen Nr...\n"
                   "-c = Markiert ein Item mit eingebener Nr... als [ERLEDIGT]\n"
                   "-u = Entfernt die Markierung [ERLEDIGT] wieder bei einem Item mit eingegebener Nr...\n"
                   "-U = Zeigt zusammen mit -l (-l -U) nicht erledigte Items\n"
                   "-C = Zeigt zusammen mit -l (-l -C) erledigte Items\n" 
                  );
            break;

        case 'l':
            listeAnzeigen = 1;
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

        case 'e':
            if (optind >= argc)
            {
                printf("Item-Nummer fehlt !\n");
                break;
            }

            if (istGueltigeNummer(argv[optind], &itemNummer) == 0)
            {
                printf("Ungültige Item-Nummer  !\n");
                break;
            }
            aendereText(itemNummer, optarg);
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

        case 'U':
            filter = 1;
            break;

        case 'C':
            filter = 2;
            break;
        }
    }
    if (listeAnzeigen == 1)
    {
        zeigeListe(filter);
    }
    return 0;
}
