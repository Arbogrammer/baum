#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "miniz.c"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <math.h>

#if defined __WIN32__
#include <dir.h>
#include <windows.h>
#define MODUS )
#define EINSTELLUNGSPFAD getenv("appdata") //"\\Anwendungsdaten\\"
#define EINSTELLUNGSPFADUNIX
#define DATEIART "Dateiarten.txt"
#define ODTEINSTELLUNGSDATEI "odtEinstellungen.txt"
#define SVGEINSTELLUNGSDATEI "svgEinstellungen.txt"
#define HILFSVERZEICHNIS "\\baumtmp"
#define CONTENTXML "\\content.xml"
#define MANIFESTXML "\\META-INF\\manifest.xml"
#define STYLESXML "\\styles.xml"
#define ZIPDATEINAME "\\baum.zip"
#define FORMELVERZEICHNIS "\\Formel"
#define PFADTRENNER "\\"
#define WURZELVERZEICHNIS char hilfspfad[4097] = ""
#define HOMEVERZEICHNIS sprintf(homeverzeichnis,"%s",getenv("HOMEDRIVE"));\
                            strcat(homeverzeichnis,getenv("HOMEPATH"));
#define OEFFNEN     char libreoffice[300] = "";\
                    char aufruf2[300] = "";\
                    if(strlen(odtProgramm) != 1)\
                    {\
                      strcat(libreoffice, odtProgramm);\
                      char *progname = strrchr(libreoffice,92);\
                      *progname = 0;\
                      /*printf("%i\n",chdir(libreoffice))*/;\
                      strcat(aufruf2, progname+1);\
                      strcat(aufruf2, " \"");\
                    }\
                    else\
                    {\
                      strcat(libreoffice,getenv("ProgramFiles"));\
                      /*printf("%s\n",libreoffice);*/\
                      strcat(libreoffice,"\\");\
                      /*printf("%s\n",libreoffice);*/\
                      WIN32_FIND_DATA dir;\
                      HANDLE fhandle;\
                      char directory[256];\
                      sprintf(directory,"%s\\*.*",libreoffice);\
                      if ((fhandle=FindFirstFile(directory,&dir)) != INVALID_HANDLE_VALUE)\
                      {\
                        do\
                        {\
                          /*printf("%s\n", dir.cFileName)*/;\
                          if(strstr(dir.cFileName,"LibreOffice") || strstr(dir.cFileName,"libreoffice") || strstr(dir.cFileName,"Libreoffice") || strstr(dir.cFileName,"OpenOffice") || strstr(dir.cFileName,"openoffice") || strstr(dir.cFileName,"Openoffice"))\
                          {\
                            strcat(libreoffice,dir.cFileName);\
                            break;\
                          }\
                        }\
                        while(FindNextFile(fhandle,&dir));\
                      }\
                      /*printf("%s\n",libreoffice)*/;\
                      FindClose(fhandle);\
                      /*printf("%s\n",libreoffice);*/\
                      strcat(libreoffice,"\\program");\
                      /*printf("%s\n",libreoffice);*/\
                      /*printf("%i\n",chdir(libreoffice))*/;\
                      /*printf("%s\n",libreoffice)*/;\
                      strcpy(aufruf2, "swriter.exe \"");\
                    }\
                    strcat(aufruf2,odtdateiname);\
                    strcat(aufruf2,"\"");
#define AUSGABEUMLEITEN
#else
#include <unistd.h>
#define MODUS ,0755)
#define EINSTELLUNGSPFAD "/.config"
#define EINSTELLUNGSPFADUNIX snprintf(einstellungspfad,4095,"%s",homeverzeichnis);
#define DATEIART "Dateiarten"
#define ODTEINSTELLUNGSDATEI "odtEinstellungen"
#define SVGEINSTELLUNGSDATEI "svgEinstellungen"
#define HILFSVERZEICHNIS "/.baum"
#define CONTENTXML "/content.xml"
#define MANIFESTXML "/META-INF/manifest.xml"
#define STYLESXML "/styles.xml"
#define ZIPDATEINAME "/baum.zip"
#define FORMELVERZEICHNIS "/Formel"
#define PFADTRENNER "/"
#define WURZELVERZEICHNIS char hilfspfad[4097] = "/"
#define HOMEVERZEICHNIS sprintf(homeverzeichnis,"%s",getenv("HOME"));
#define OEFFNEN char aufruf2[4200] = "";\
                if(strlen(odtProgramm) == 1)\
                {\
                  strcpy(aufruf2, "soffice \"");\
                }\
                else\
                {\
                  strcpy(aufruf2, odtProgramm);\
                  strcat(aufruf2, " \"");\
                }\
                /*printf("%s\n",aufruf2);*/\
                strcat(aufruf2,odtdateiname);\
                strcat(aufruf2,"\"");\
                /*printf("%s\n",aufruf2);*/\
                strcat(aufruf2,">>/dev/null 2> /dev/null &");\
                //printf("%s\n",aufruf2);
#endif
#define CMINPX 37.79527559055118     /* Umrechnung cm in Pixel */
#define PXINCM 0.026458333333333     /* Umrechnung Pixel in cm */
#define PI 3.14159265359
#define SVGM (svgyl-svgyr)/(svgxr-svgxl)
#define SVGMS (svgxl-svgxr)/(svgyl-svgyr)
#define SCHRIFTFAKTOR 0.75
#define SCHRIFTBREITE 2.384


/* Funktionsprototypen */

int zeichenzaehlen(char *string, int zeichen);
char *knotenfinder(char *string, int nummer);
unsigned long int potenziere(unsigned long int basis, unsigned long int exponent);
unsigned long int ggT(unsigned long int zahl1, unsigned long int zahl2);
char *bruchmultiplikation(char *bruch1, char *bruch2, char *ergebnis);
double bruchzufloat(char *bruch);
char *floatstringzubruch(char *zahlstring, char *bruch);
char *floatzubruch(float zahl, char *bruch);
double wurzel(double zahl);
double arctan(double zahl);
double schriftbreite(char *Schriftart);


/* Los gehts mit der Hauptfunktion! */

int main(int ArgumentAnzahl, char *Argument[])
{
  int i=0, j=0, k=0, l=0, m=0, n=0;
  if(ArgumentAnzahl == 1)
  {
    fprintf(stderr,"Fehler: Keine Argumente angegeben!\n");
    return 1;
  }

  FILE *fp;                                /* Dateizeiger, um Dateien erstellen und lesen zu können */


  /* Einlesen der Einstellungsvariablen - falls vorhanden. Ansonsten Einstellungsdatei erstellen. */

  char homeverzeichnis[4097] = "";
  HOMEVERZEICHNIS
  //printf("Homeverzeichnis: %s\n",homeverzeichnis);

  char einstellungspfad[4097] = "", einstellungsdatei[4097] = "", hilfseinstellungspfad[4097] = "";
  EINSTELLUNGSPFADUNIX
  strncat(einstellungspfad,EINSTELLUNGSPFAD,4093-strlen(einstellungspfad));
  strcat(einstellungspfad,PFADTRENNER);
  strcat(einstellungspfad,"Baumdiagramme");
  strcat(einstellungspfad,PFADTRENNER);
  printf("Einstellungspfad: %s\n",einstellungspfad);

  strcpy(einstellungsdatei,einstellungspfad);
  strncat(einstellungsdatei,DATEIART,4095-strlen(einstellungsdatei));
  strcpy(hilfseinstellungspfad, einstellungspfad);

  char *zeiger;
  zeiger = strtok(hilfseinstellungspfad,PFADTRENNER);
  WURZELVERZEICHNIS;
  while(zeiger)
  {
    //printf("%s\n",zeiger);
    strcat(hilfspfad,zeiger);
    strcat(hilfspfad,PFADTRENNER);
    //printf("%s\n",hilfspfad);
    if(chdir(hilfspfad) == -1)
    {
      mkdir(hilfspfad MODUS;
    }
    zeiger = strtok(NULL,PFADTRENNER);
  }

  char odteinstellungsdatei[4097] = "";
  strcpy(odteinstellungsdatei,einstellungspfad);
  strncat(odteinstellungsdatei,ODTEINSTELLUNGSDATEI,4095-strlen(odteinstellungsdatei));

  char svgeinstellungsdatei[4097] = "";
  strcpy(svgeinstellungsdatei,einstellungspfad);
  strncat(svgeinstellungsdatei,SVGEINSTELLUNGSDATEI,4095-strlen(svgeinstellungsdatei));

  //printf("Einstellungspfad: %s\nEinstellungsdatei: %s\nsvgEInst: %s\nodteinst: %s\n",einstellungspfad,einstellungsdatei,svgeinstellungsdatei,odteinstellungsdatei);

  fp = fopen(einstellungsdatei,"r");
  if(fp == NULL)
  {
    fp = fopen(einstellungsdatei,"w");
    fprintf(fp, "Hinweis: Die Reihenfolge der Dateiarten darf nicht verändert werden. Sie können hier einstellen, welche Art von Dateien Sie erstellen möchten:\nOpenDocumentText (odt). Diese Dateien können zum Beispiel mit LibreOffice Writer oder OpenOffice Writer geöffnet werden.\nScalable Vector Graphics (svg). Diese skalierbaren Vektorgrafiken können zum Beispiel mit Google Chrome, Mozilla Firefox oder anderen Programmen geöffnet werden. Leider erfüllen nicht alle Programme, die svg darstellen können, den svg-Standard komplett, so dass es gerade bei den Textelementen zu Darstellungsfehlern kommen kann.\n1 bedeutet, dass die Datei erstellt wird. 0, dass sie nicht erstellt wird. Bitte sonst nichts an der Datei ändern. Bitte keine Leerzeichen nach den Gleichheitszeichen hinzufügen.\n\nodt=0\nsvg=1\n");
    fclose(fp);
  }
  else
  {
    fclose(fp);
  }

  fp = fopen(odteinstellungsdatei,"r");
  if(fp == NULL)
  {
    fp = fopen(odteinstellungsdatei,"w");
    fprintf(fp, "[Allgemeine Einstellungen]\n\nDateipfad=0\t\t\tPfad wo die Baumdiagrammdateien gespeichert werden sollen. Standardwert (0) ist das eigene Benutzerverzeichnis.\nProgramm=0\t\t\tProgramm inklusive Pfad, mit dem die Baumdiagramme (als odt-Dateien) geöffnet werden sollen. Z.B. C:\\Programme\\LibreOffice 4.2\\program\\swriter.exe. Muss nur angegeben werden, wenn Libreoffice/Openoffice nicht standardmäßig im übichen Programme-Pfad liegen. Mit 0 wird einfach Libreoffice bzw. OpenOffice geöffnet, sofern es gefunden wird. Wenn nichts geöffnet werden sollen, einfach leer lassen.\nSeitenHoehe=0\t\t\tEinheit cm. DIN A4 Format ist 29,7 mal 21,0 cm DIN A3 42,0 mal 29,7 cm. 0 bedeutet automatische Größenanpassung an das Baumdiagramm.\nSeitenBreite=0\t\t\tEinheit cm. DIN A4 Format ist 29,7 mal 21,0 cm DIN A3 42,0 mal 29,7 cm. 0 bedeutet automatische Größenanpassung an das Baumdiagramm.\nSeitenRandLinks=2\t\tEinheit cm.\nSeitenRandRechts=2\t\tEinheit cm.\nSeitenRandOben=2\t\tEinheit cm.\nSeitenRandUnten=2\t\tEinheit cm.\nGruppierung=0\t\t\t0: Alles gruppiert. 1: Keine Gruppierung. 2: Alles außer Bruch-Formeln in der Ergebnisspalte wird gruppiert.\nOptimierung=0\t\t\tMögliche Werte: 0: Die obigen beiden Werte gelten. 1: Das Baumdiagramm nutzt eine komplette DIN A4-Seite im Hochformat. 2: Wie 1 nur im Querformat. 3 oder 4: wie 1 oder 2 nur im DIN A3-Format.\nUrneNull=0\t\t\tMögliche Werte: 0, um beim Ziehen aus einer Urne ohne Zurücklegen Pfade mit Wahrscheinlichkeit 0 nicht anzuzeigen, 1 um diese anzuzeigen.\nUrneKuerzen=1\t\t\tMögliche Werte: 0 um die Zweig-Wahrscheinlichkeiten beim Urnenmodell nicht zu kürzen, 1 um sie zu kürzen.\nErgebnisSpalteAnzeigen=1\tMögliche Werte: 1 für anzeigen, 0 für nicht anzeigen.\nErgebnisSpalteWskAnzeigen=1\tMögliche Werte: 1 für anzeigen, 0 für nicht anzeigen. Diese wird natürlich nur angezeigt, wenn Wahrscheinlichkeiten angegeben werden.\nGenauigkeit=-1\t\t\tAnzahl der Nachkommastellen der Wahrscheinlichkeiten. -1 bedeutet so genau wie möglich. Mit -1 werden zum Beispiel 0,1 oder 0,25 oder 0,125 dargestellt, bei 3 würde 0,100 oder 0,250 oder 0,125 angezeigt werden. Nur relevant, wenn nicht \"Bruch\" eingestellt ist.\n\n\n[Eigenschaften der Zweige des Baumdiagramms]\n\nHorizontalerKnotenAbstand=3\t\tSo weit in cm sollen die einzelnen \"Spalten\" des Baumdiagramms auseinander liegen: Entspricht der \"Breite\" (Länge in x-Richtung) der Zweige.\nZweig=solid\t\t\t\tAnzeigeform der Zweige: none (keine), solid (Linie), dash (gestrichelte Linie).\n\nZweigStrichelungStrichAnzahl1=1\t\tBei Strichelungen mit zwei verschiedenen Strichen, kann hier die Anzahl der ersten Art von Strichen gesetzt werden. Macht nur Sinn, wenn dash bei Zweig aktiviert ist.\nZweigStrichelungStrichLaenge1=1\t\tDie Länge der Striche 1. Art. 0 bedeutet punktförmig.\nZweigStrichelungStrichAnzahl2=1\t\tAnzahl Striche 2. Art.\nZweigStrichelungStrichLaenge2=1\t\tDie Länge der Striche 2. Art. 0 bedeutet punktförmig.\nZweigStrichelungAbstand=1\t\tAbstand der Striche.\n\nZweigDicke=0\t\t\t\tWerte zwischen 0 und 5 in cm.\nZweigFarbe=000000\t\t\tHexadezimal RGB. Die erste zwei Stellen stehen für den Rotanteil, die zweiten zwei für den Grünanteil und die letzten für den Blauanteil. 000000 z.B. ist schwarz, 0000FF blau, 00FF00 grün und FF0000 rot. 808080 wäre ein mittlerer Grau-Ton. Etc.\nZweigAnfangForm=none\t\t\tMögliche Werte: none, Square, Arrow, Small_20_Arrow, Dimension_20_Lines, Double_20_Arrow, Rounded_20_short_20_Arrow, Symmetric_20_Arrow, Line_20_Arrow, Rounded_20_large_20_Arrow, Circle, Square_20_45, Arrow_20_concave, Short_20_line_20_Arrow, Triangle_20_unfilled, Diamond_20_unfilled, Diamond, Circle_20_unfilled, Square_20_45_20_unfilled, Square_20_unfilled, Half_20_Circle_20_unfilled\nZweigAnfangDicke=0\t\t\tMögliche Werte 0 bis 5 in cm.\nZweigEndeForm=none\t\t\tMögliche Werte: none, Square, Arrow, Small_20_Arrow, Dimension_20_Lines, Double_20_Arrow, Rounded_20_short_20_Arrow, Symmetric_20_Arrow, Line_20_Arrow, Rounded_20_large_20_Arrow, Circle, Square_20_45, Arrow_20_concave, Short_20_line_20_Arrow, Triangle_20_unfilled, Diamond_20_unfilled, Diamond, Circle_20_unfilled, Square_20_45_20_unfilled, Square_20_unfilled, Half_20_Circle_20_unfilled\nZweigEndeDicke=0\t\t\tMögliche Werte 0 bis 5 in cm.\nZweigUndurchsichtigkeit=100\t\tWerte von 0 bis 100 in Prozent. 0 bedeutet völlig transparent (durchsichtig), 100 völlig opak (undurchsichtig).\n\n\n[Eigenschaften der Zweigwahrscheinlichkeiten]\n\nBruchZweige=0\t\t\t\t\tWie oben: 0, Dezimalzahlen auf den Zweigen, 1, Brüche auf den Zweigen.\nBruchZweigeFormel=1\t\t\t\t0, Die Brüche auf den Zweigen werden nebeneinander mit / dargestellt, z.B. 5/7. 1, die Brüche werden wie üblich \"Zähler über Nenner\" dargestellt.\nLeerzeichenAnzahl=0\t\t\t\tHiermit können die Wahrscheinlichkeiten auf den Zweigen noch etwas nach rechts geschoben werden. Alternativ lässt sich auch die Variable VerschiebungWskNachRechts nutzen.\nWahrscheinlichkeitsPositionAufZweig=center\tMögliche Werte: left, center, right.\nVerschiebungWskNachUnten=0.05\t\t\tDie Position der Zweigwahrscheinlichkeiten anpassen (Einheit cm).\nVerschiebungWskNachOben=0.05\t\t\tDie Position der Zweigwahrscheinlichkeiten anpassen (Einheit cm).\nVerschiebungWskNachRechts=0.8\t\t\tDie Position der Zweigwahrscheinlichkeiten anpassen (Einheit cm).\nVerschiebungWskNachLinks=0.05\t\t\tDie Position der Zweigwahrscheinlichkeiten anpassen (Einheit cm).\nFettWsk=\t\t\t\t\tWerte none oder bold\nKursivWsk=\t\t\t\t\tKursivdruck ja oder nein: Werte none oder italic\nSchriftfarbeWsk=black\t\t\t\tSchriftfarbe in hexadezimaler Schreibweise\nSchriftartWsk=Times New Roman\t\t\tSchriftart einstellen. Exakte Schreibweise ist notwendig.\nSchriftgroesseWsk=12\t\t\t\tSchriftgröße der Zweigwahrscheinlichkeiten in pt.\n\n\n[Eigenschaften der Knotenpunkte des Baumdiagramms]\n\nKnotenBreite=0\t\t\t\t\tSo breit in cm sollen die Knotenpunkte sein. 0 bedeutet automatische Anpassung an die Schrfitgröße und -art.\nKnotenHoehe=0\t\t\t\t\tSo hoch in cm sollen die Knotenpunkte sein. 0 bedeutet automatische Anpassung an die Schrfitgröße und -art.\nVertikalerKnotenAbstand=2\t\t\tSo viele cm sollen die letzten (am weitesten rechts liegenden) Knotenpunkte auseinander liegen.\nKnotenForm=0\t\t\t\t\t0 entspricht Rechteck, 1 entspricht Ellipse (Kreis). Macht nur Sinn, wenn Rahmen oder Fuellung aktiviert wurde.\nRahmenKnoten=none\t\t\t\tZeichnet einen Rahmen um die Knotenpunkte. Mögliche Werte: none (kein Rahmen), solid (Linie) oder dash (gestrichelt).\n\nRahmenKnotenStrichelungStrichAnzahl1=1\t\tBei Strichelungen mit zwei verschiedenen Strichen, kann hier die Anzahl der ersten Art von Strichen gesetzt werden. Macht nur Sinn, wenn dash bei Rahmen aktiviert ist.\nRahmenKnotenStrichelungStrichLaenge1=0\t\tDie Länge der Striche 1. Art. 0 bedeutet punktförmig.\nRahmenKnotenStrichelungStrichAnzahl2=0\t\tAnzahl Striche 2. Art.\nRahmenKnotenStrichelungStrichLaenge2=0\t\tDie Länge der Striche 2. Art. 0 bedeutet punktförmig.\nRahmenKnotenStrichelungAbstand=0.1\t\tAbstand der Striche.\n\nRahmenKnotenDicke=0.0\t\t\t\tZwischen 0 und 5 cm.\nRahmenKnotenFarbe=000000\t\t\tHexadezimal RGB. Die erste zwei Stellen stehen für den Rotanteil, die zweiten zwei für den Grünanteil und die letzten für den Blauanteil. 000000 z.B. ist schwarz, 0000FF blau, 00FF00 grün und FF0000 rot. 808080 wäre ein mittlerer Grau-Ton. Etc.\nRahmenKnotenUndurchsichtigkeit=100\t\tMögliche Werte: 0 bis 100 in Prozent. 0 ist völlig transparent, 100 völlig opak.\n\nFuellungKnoten=0\t\t\t\tMögliche Werte: 0 (keine Füllung), 1 (einfarbig), 2 (Farbverlauf), 3 (Schraffur), 4 (Hintergrundbild).\nFuellfarbeKnoten=FFFFFF\t\t\t\tHexadezimale Farbe für Knotenpunkte\nFuellundurchsichtigkeitKnoten=100\t\tIn Prozent. s.o. 0 ist völlig transparent, 100 völlig undurchsichtig.\n\nFuellungFarbverlaufArtKnoten=ellipsoid\t\tArt des Farbverlaufs. Mögliche Werte: linear, axial, radial, ellipsoid, square, rectangular\nFuellungFarbverlaufZentrumXKnoten=50\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100. Bei linear und axial hinfällig.\nFuellungFarbverlaufZentrumYKnoten=50\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100. Bei linear und axial hinfällig.\nFuellungFarbverlaufAnfangsfarbeKnoten=FF0000\tHexadezimale Farbe.\nFuellungFarbverlaufEndfarbeKnoten=0000FF\tHexadezimale Farbe.\nFuellungFarbverlaufAnfangsintensitaetKnoten=0\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungFarbverlaufEndintensitaetKnoten=100\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungFarbverlaufWinkelKnoten=450\t\tWinkel in Zehntel-Grad. 450 bedeutet 45°.\nFuellungFarbverlaufRandKnoten=1\t\t\tAngaben in Prozent. Mögliche Werte zwischen 0 und 100.\n\nFuellungTransparenzverlaufArtKnoten=ellipsoid\tArt des Transparenzverlaufs. Mögliche Werte: linear, axial, radial, ellipsoid, square, rectangular, oder 0 für keinen Verlauf.\nFuellungTransparenzverlaufZentrumXKnoten=50\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100. Bei linear und axial hinfällig.\nFuellungTransparenzverlaufZentrumYKnoten=50\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100. Bei linear und axial hinfällig.\nFuellungTransparenzverlaufAnfangKnoten=0\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufEndKnoten=100\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufWinkelKnoten=450\tWinkel in Zehntel-Grad. 450 bedeutet 45°.\nFuellungTransparenzverlaufRandKnoten=1\t\tAngaben in Prozent. Mögliche Werte zwischen 0 und 100.\n\nFuellungSchraffurArtKnoten=1\t\t\tAnzahl der überkreuzten Linien. Mögliche Werte 1 (nur parallele), 2 (Parallelogramme) oder 3.\nFuellungSchraffurFarbeKnoten=000000\t\tHexadezimale Farbe der Linien.\nFuellungSchraffurAbstandKnoten=0.05\t\tAbstand der Linien in cm.\nFuellungSchraffurRotationKnoten=450\t\tDrehung der Linien in Zehntel-Grad. 0 bedeutet waagrecht, 900 senkrecht. 450 steht für 45°.\n\nFuellungBildKnoten=Sky\t\t\t\tBild als Knotenfüllung. Mögliche Werte: Sky, Aqua, Coarse, Space, Metal, Wet, Marble, Linen, Stone, Space_20_Metal, Pebbles, Wall, Red_20_Wall, Pattern, Leaves, Lawn_20_Artificial, Daisy, Orange, Fiery, Roses\n\nSchattenKnoten=invisible\t\t\tSchattenwurf aktivieren (visible) oder deaktivieren (invisible)\nSchattenVerschiebungXKnoten=0.2\t\t\tVerschiebung des Schattens nach rechts in cm. Für Verschiebungen nach links negative Werte wählen.\nSchattenVerschiebungYKnoten=0.2\t\t\tVerschiebung des Schattens nach unten in cm. Für Verschiebungen nach oben negative Werte wählen.\nSchattenFarbeKnoten=000000\t\t\tHexadezimale Farbe des Schattens. s.o.\nSchattenUndurchsichtigkeitKnoten=65\t\tOpazität (Undurchsichtigkeit) des Schattens in Prozent. 0 bedeutet voll transparent, 100 überhaupt nicht.\n\nFettKnoten=none\t\t\t\t\tSchrift der Knotenpunkte Fett. Werte: none oder bold.\nKursivKnoten=none\t\t\t\tKursivdruck ja oder nein: Werte none oder italic\nSchriftfarbeKnoten=000000\t\t\tSchriftfarbe in hexadezimaler Schreibweise\nSchriftartKnoten=Times New Roman\t\tSchriftart einstellen. Exakte Schreibweise ist notwendig.\nSchriftgroesseKnoten=12\t\t\t\tSchriftgröße der Ergebnisspaltenüberschrift in pt.\n\n\n[Eigenschaften der Ergebnisspaltenüberschrift des Baumdiagramms]\n\nRahmenErgebnisTitel=none\t\t\t\t\tZeichnet einen Rahmen um die Überschrift der Ergebnisspalte. Mögliche Werte: none (kein Rahmen), solid (Linie) oder dash (gestrichelt).\n\nRahmenErgebnisTitelStrichelungStrichAnzahl1=1\t\t\tBei Strichelungen mit zwei verschiedenen Strichen, kann hier die Anzahl der ersten Art von Strichen gesetzt werden. Macht nur Sinn, wenn dash bei Rahmen aktiviert ist.\nRahmenErgebnisTitelStrichelungStrichLaenge1=0\t\t\tDie Länge der Striche 1. Art. 0 bedeutet punktförmig.\nRahmenErgebnisTitelStrichelungStrichAnzahl2=0\t\t\tAnzahl Striche 2. Art.\nRahmenErgebnisTitelStrichelungStrichLaenge2=0\t\t\tDie Länge der Striche 2. Art. 0 bedeutet punktförmig.\nRahmenErgebnisTitelStrichelungAbstand=0.1\t\t\tAbstand der Striche.\n\nRahmenErgebnisTitelDicke=0.0\t\t\t\t\tZwischen 0 und 5 cm.\nRahmenErgebnisTitelFarbe=000000\t\t\t\t\tHexadezimale Farbe für Ergebnisspalten-Überschrifts-Rahmen.\nRahmenErgebnisTitelUndurchsichtigkeit=100\t\t\tMögliche Werte: 0 bis 100 in Prozent. 0 ist völlig transparent, 100 völlig opak.\n\nFuellungErgebnisTitel=0\t\t\t\t\t\tMögliche Werte: 0 (keine Füllung), 1 (einfarbig), 2 (Farbverlauf), 3 (Schraffur), 4 (Hintergrundbild).\nFuellfarbeErgebnisTitel=FFFFFF\t\t\t\t\tHexadezimale Farbe für Ergebnisspaltenüberschrift\nFuellundurchsichtigkeitErgebnisTitel=100\t\t\ts.o. 0 ist völlig transparent, 100 völlig undurchsichtig.\n\nFuellungFarbverlaufArtErgebnisTitel=ellipsoid\t\t\tArt des Farbverlaufs, z.B. rectangular.\nFuellungFarbverlaufZentrumXErgebnisTitel=0\t\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungFarbverlaufZentrumYErgebnisTitel=0\t\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungFarbverlaufAnfangsfarbeErgebnisTitel=000000\t\tHexadezimale Farbe.\nFuellungFarbverlaufEndfarbeErgebnisTitel=FFFFFF\t\t\tHexadezimale Farbe.\nFuellungFarbverlaufAnfangsintensitaetErgebnisTitel=0\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungFarbverlaufEndintensitaetErgebnisTitel=0\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungFarbverlaufWinkelErgebnisTitel=450\t\t\tWinkel in Zehntel-Grad. 450 bedeutet 45°.\nFuellungFarbverlaufRandErgebnisTitel=20\t\t\t\tAngaben in Prozent. Mögliche Werte zwischen 0 und 100.\n\nFuellungTransparenzverlaufArtErgebnisTitel=ellipsoid\t\tArt des Transparenzverlaufs, z.B. rectangular.\nFuellungTransparenzverlaufZentrumXErgebnisTitel=0\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufZentrumYErgebnisTitel=0\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufAnfangErgebnisTitel=50\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufEndErgebnisTitelErgebnisTitel=80\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufWinkelErgebnisTitel=450\t\tWinkel in Zehntel-Grad. 450 bedeutet 45°.\nFuellungTransparenzverlaufRandErgebnisTitel=20\t\t\tAngaben in Prozent. Mögliche Werte zwischen 0 und 100.\n\nFuellungSchraffurArtErgebnisTitel=2\t\t\t\tAnzahl der überkreuzten Linien. Mögliche Werte 1 (nur parallele), 2 (Parallelogramme) oder 3.\nFuellungSchraffurFarbeErgebnisTitel=10FF00\t\t\tHexadezimale Farbe der Linien.\nFuellungSchraffurAbstandErgebnisTitel=0.03\t\t\tAbstand der Linien in cm.\nFuellungSchraffurRotationErgebnisTitel=200\t\t\tDrehung der Linien in Zehntel-Grad. 0 bedeutet waagrecht, 900 senkrecht. 450 steht für 45°.\n\nFuellungBildErgebnisTitel=Sky\t\t\t\t\tBild als Füllung der Ergebnisspalten-Überschrift. Mögliche Werte: Sky, Aqua, Coarse, Space, Metal, Wet, Marble, Linen, Stone, Space_20_Metal, Pebbles, Wall, Red_20_Wall, Pattern, Leaves, Lawn_20_Artificial, Daisy, Orange, Fiery, Roses\n\nSchattenErgebnisTitel=invisible\t\t\t\t\tSchattenwurf aktivieren (visible) oder deaktivieren (invisible)\nSchattenVerschiebungXErgebnisTitel=0.2\t\t\t\tVerschiebung des Schattens nach rechts in cm. Für Verschiebungen nach links negative Werte wählen.\nSchattenVerschiebungYErgebnisTitel=0.2\t\t\t\tVerschiebung des Schattens nach unten in cm. Für Verschiebungen nach oben negative Werte wählen.\nSchattenFarbeErgebnisTitel=000000\t\t\t\tHexadezimale Farbe des Schattens. s.o.\nSchattenUndurchsichtigkeitErgebnisTitel=65\t\t\tOpazität (Undurchsichtigkeit) des Schattens in Prozent. 0 bedeutet voll transparent, 100 überhaupt nicht.\n\nFettErgebnisTitel=none\t\t\t\t\t\tWerte none oder bold\nKursivErgebnisTitel=none\t\t\t\t\tKursivdruck ja oder nein: Werte none oder italic\nSchriftfarbeErgebnisTitel=000000\t\t\t\tSchriftfarbe in hexadezimaler Schreibweise\nSchriftartErgebnisTitel=Times New Roman\t\t\t\tSchriftart einstellen. Exakte Schreibweise ist notwendig.\nSchriftgroesseErgebnisTitel=12\t\t\t\t\tSchriftgröße der Ergebnisspaltenüberschrift in pt.\n\n\n[Eigenschaften der Ergebnisspalte des Baumdiagramms]\n\nErgebnisBreite=0\t\t\t\t\tBreite der Ergebnisspalte in cm. 0 bedeutet, dass diese automatisch angepasst wird.\nRahmenErgebnis=none\t\t\t\t\tZeichnet einen Rahmen um die Pfadergebnisse. Mögliche Werte: none (kein Rahmen), solid (Linie) oder dash (gestrichelt).\n\nRahmenErgebnisStrichelungStrichAnzahl1=1\t\tBei Strichelungen mit zwei verschiedenen Strichen, kann hier die Anzahl der ersten Art von Strichen gesetzt werden. Macht nur Sinn, wenn dash bei Rahmen aktiviert ist.\nRahmenErgebnisStrichelungStrichLaenge1=0\t\tDie Länge der Striche 1. Art. 0 bedeutet punktförmig.\nRahmenErgebnisStrichelungStrichAnzahl2=0\t\tAnzahl Striche 2. Art.\nRahmenErgebnisStrichelungStrichLaenge2=0\t\tDie Länge der Striche 2. Art. 0 bedeutet punktförmig.\nRahmenErgebnisStrichelungAbstand=0.1\t\t\tAbstand der Striche.\n\nRahmenErgebnisDicke=0.0\t\t\t\t\tZwischen 0 und 5 cm.\nRahmenErgebnisFarbe=000000\t\t\t\tHexadezimale Farbe für Ergebnisspalten-Überschrifts-Rahmen.\nRahmenErgebnisUndurchsichtigkeit=100\t\t\tMögliche Werte: 0 bis 100 in Prozent. 0 ist völlig transparent, 100 völlig opak.\n\nFuellungErgebnis=0\t\t\t\t\tMögliche Werte: 0 (keine Füllung), 1 (einfarbig), 2 (Farbverlauf), 3 (Schraffur), 4 (Hintergrundbild).\nFuellfarbeErgebnis=FFFFFF\t\t\t\tHexadezimale Farbe für Ergebnisspaltenüberschrift\nFuellundurchsichtigkeitErgebnis=100\t\t\ts.o. 0 ist völlig transparent, 100 völlig undurchsichtig.\n\nFuellungFarbverlaufArtErgebnis=ellipsoid\t\tArt des Farbverlaufs, z.B. rectangular.\nFuellungFarbverlaufZentrumXErgebnis=0\t\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungFarbverlaufZentrumYErgebnis=0\t\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungFarbverlaufAnfangsfarbeErgebnis=000000\t\tHexadezimale Farbe.\nFuellungFarbverlaufEndfarbeErgebnis=FFFFFF\t\tHexadezimale Farbe.\nFuellungFarbverlaufAnfangsintensitaetErgebnis=000000\tHexadezimale Farbe.\nFuellungFarbverlaufEndintensitaetErgebnis=FFFFFF\tHexadezimale Farbe.\nFuellungFarbverlaufWinkelErgebnis=450\t\t\tWinkel in Zehntel-Grad. 450 bedeutet 45°.\nFuellungFarbverlaufRandErgebnis=20\t\t\tAngaben in Prozent. Mögliche Werte zwischen 0 und 100.\n\nFuellungTransparenzverlaufArtErgebnis=ellipsoid\t\tArt des Transparenzverlaufs, z.B. rectangular.\nFuellungTransparenzverlaufZentrumXErgebnis=0\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufZentrumYErgebnis=0\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufAnfangErgebnis=50\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufEndErgebnis=80\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufWinkelErgebnis=450\t\tWinkel in Zehntel-Grad. 450 bedeutet 45°.\nFuellungTransparenzverlaufRandErgebnis=20\t\tAngaben in Prozent. Mögliche Werte zwischen 0 und 100.\n\nFuellungSchraffurArtErgebnis=1\t\t\t\tAnzahl der überkreuzten Linien. Mögliche Werte 1 (nur parallele), 2 (Parallelogramme) oder 3.\nFuellungSchraffurFarbeErgebnis=000000\t\t\tHexadezimale Farbe der Linien.\nFuellungSchraffurAbstandErgebnis=0.05\t\t\tAbstand der Linien in cm.\nFuellungSchraffurRotationErgebnis=450\t\t\tDrehung der Linien in Zehntel-Grad. 0 bedeutet waagrecht, 900 senkrecht. 450 steht für 45°.\n\nFuellungBildErgebnis=Sky\t\t\t\tBild als Füllung der Ergebnisspalte. Mögliche Werte: Sky, Aqua, Coarse, Space, Metal, Wet, Marble, Linen, Stone, Space_20_Metal, Pebbles, Wall, Red_20_Wall, Pattern, Leaves, Lawn_20_Artificial, Daisy, Orange, Fiery, Roses\n\nSchattenErgebnis=invisible\t\t\t\tSchattenwurf aktivieren (visible) oder deaktivieren (invisible)\nSchattenVerschiebungXErgebnis=0.2\t\t\tVerschiebung des Schattens nach rechts in cm. Für Verschiebungen nach links negative Werte wählen.\nSchattenVerschiebungYErgebnis=0.2\t\t\tVerschiebung des Schattens nach unten in cm. Für Verschiebungen nach oben negative Werte wählen.\nSchattenFarbeErgebnis=000000\t\t\t\tHexadezimale Farbe des Schattens. s.o.\nSchattenUndurchsichtigkeitErgebnis=65\t\t\tOpazität (Undurchsichtigkeit) des Schattens in Prozent. 0 bedeutet voll transparent, 100 überhaupt nicht.\n\nFettErgebnis=none\t\t\t\t\tWerte none oder bold\nKursivErgebnis=none\t\t\t\t\tKursivdruck ja oder nein: Werte none oder italic\nSchriftfarbeErgebnis=000000\t\t\t\tSchriftfarbe in hexadezimaler Schreibweise\nSchriftartErgebnis=Times New Roman\t\t\tSchriftart einstellen. Exakte Schreibweise ist notwendig.\nSchriftgroesseErgebnis=12\t\t\t\tSchriftgröße der Ergebnisspalte in pt.\n\n\n[Eigenschaften der Überschrift der Wahrscheinlichkeits-Ergebnisspalte]\n\nRahmenWskErgebnisTitel=none\t\t\t\t\tZeichnet einen Rahmen um die Überschrift der Pfadwahrscheinlichkeiten-Spalte. Mögliche Werte: none (kein Rahmen), solid (Linie) oder dash (gestrichelt).\n\nRahmenWskErgebnisTitelStrichelungStrichAnzahl1=1\t\tBei Strichelungen mit zwei verschiedenen Strichen, kann hier die Anzahl der ersten Art von Strichen gesetzt werden. Macht nur Sinn, wenn dash bei Rahmen aktiviert ist.\nRahmenWskErgebnisTitelStrichelungStrichLaenge1=0\t\tDie Länge der Striche 1. Art. 0 bedeutet punktförmig.\nRahmenWskErgebnisTitelStrichelungStrichAnzahl2=0\t\tAnzahl Striche 2. Art.\nRahmenWskErgebnisTitelStrichelungStrichLaenge2=0\t\tDie Länge der Striche 2. Art. 0 bedeutet punktförmig.\nRahmenWskErgebnisTitelStrichelungAbstand=0.1\t\t\tAbstand der Striche.\n\nRahmenWskErgebnisTitelDicke=0.0\t\t\t\t\tZwischen 0 und 5 cm.\nRahmenWskErgebnisTitelFarbe=000000\t\t\t\tHexadezimale Farbe für Ergebnisspalten-Überschrifts-Rahmen.\nRahmenWskErgebnisTitelUndurchsichtigkeit=100\t\t\tMögliche Werte: 0 bis 100 in Prozent. 0 ist völlig transparent, 100 völlig opak.\n\nFuellungWskErgebnisTitel=0\t\t\t\t\tMögliche Werte: 0 (keine Füllung), 1 (einfarbig), 2 (Farbverlauf), 3 (Schraffur), 4 (Hintergrundbild).\nFuellfarbeWskErgebnisTitel=FFFFFF\t\t\t\tHexadezimale Farbe für Ergebnisspaltenüberschrift\nFuellundurchsichtigkeitWskErgebnisTitel=100\t\t\ts.o. 0 ist völlig transparent, 100 völlig undurchsichtig.\n\nFuellungFarbverlaufArtWskErgebnisTitel=ellipsoid\t\tArt des Farbverlaufs, z.B. rectangular.\nFuellungFarbverlaufZentrumXWskErgebnisTitel=0\t\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungFarbverlaufZentrumYWskErgebnisTitel=0\t\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungFarbverlaufAnfangsfarbeWskErgebnisTitel=000000\t\tHexadezimale Farbe.\nFuellungFarbverlaufEndfarbeWskErgebnisTitel=FFFFFF\t\tHexadezimale Farbe.\nFuellungFarbverlaufAnfangsintensitaetWskErgebnisTitel=000000\tHexadezimale Farbe.\nFuellungFarbverlaufEndintensitaetWskErgebnisTitel=FFFFFF\tHexadezimale Farbe.\nFuellungFarbverlaufWinkelWskErgebnisTitel=450\t\t\tWinkel in Zehntel-Grad. 450 bedeutet 45°.\nFuellungFarbverlaufRandWskErgebnisTitel=20\t\t\tAngaben in Prozent. Mögliche Werte zwischen 0 und 100.\n\nFuellungTransparenzverlaufArtWskErgebnisTitel=ellipsoid\t\tArt des Transparenzverlaufs, z.B. rectangular.\nFuellungTransparenzverlaufZentrumXWskErgebnisTitel=0\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufZentrumYWskErgebnisTitel=0\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufAnfangWskErgebnisTitel=50\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufEndWskErgebnisTitel=80\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufWinkelWskErgebnisTitel=450\t\tWinkel in Zehntel-Grad. 450 bedeutet 45°.\nFuellungTransparenzverlaufRandWskErgebnisTitel=20\t\tAngaben in Prozent. Mögliche Werte zwischen 0 und 100.\n\nFuellungSchraffurArtWskErgebnisTitel=1\t\t\t\tAnzahl der überkreuzten Linien. Mögliche Werte 1 (nur parallele), 2 (Parallelogramme) oder 3.\nFuellungSchraffurFarbeWskErgebnisTitel=000000\t\t\tHexadezimale Farbe der Linien.\nFuellungSchraffurAbstandWskErgebnisTitel=0.05\t\t\tAbstand der Linien in cm.\nFuellungSchraffurRotationWskErgebnisTitel=450\t\t\tDrehung der Linien in Zehntel-Grad. 0 bedeutet waagrecht, 900 senkrecht. 450 steht für 45°.\n\nFuellungBildWskErgebnisTitel=Sky\t\t\t\tBild als Füllung der Wahrscheinlichkeits-Ergebnisspalten-Überschrift. Mögliche Werte: Sky, Aqua, Coarse, Space, Metal, Wet, Marble, Linen, Stone, Space_20_Metal, Pebbles, Wall, Red_20_Wall, Pattern, Leaves, Lawn_20_Artificial, Daisy, Orange, Fiery, Roses\n\nSchattenWskErgebnisTitel=invisible\t\t\t\tSchattenwurf aktivieren (visible) oder deaktivieren (invisible)\nSchattenVerschiebungXWskErgebnisTitel=0.2\t\t\tVerschiebung des Schattens nach rechts in cm. Für Verschiebungen nach links negative Werte wählen.\nSchattenVerschiebungYWskErgebnisTitel=0.2\t\t\tVerschiebung des Schattens nach unten in cm. Für Verschiebungen nach oben negative Werte wählen.\nSchattenFarbeWskErgebnisTitel=000000\t\t\t\tHexadezimale Farbe des Schattens. s.o.\nSchattenUndurchsichtigkeitWskErgebnisTitel=65\t\t\tOpazität (Undurchsichtigkeit) des Schattens in Prozent. 0 bedeutet voll transparent, 100 überhaupt nicht.\n\nFettWskErgebnisTitel=none\t\t\t\t\tWerte none oder bold\nKursivWskErgebnisTitel=none\t\t\t\t\tKursivdruck ja oder nein: Werte none oder italic\nSchriftfarbeWskErgebnisTitel=000000\t\t\t\tSchriftfarbe in hexadezimaler Schreibweise\nSchriftartWskErgebnisTitel=Times New Roman\t\t\tSchriftart einstellen. Exakte Schreibweise ist notwendig.\nSchriftgroesseWskErgebnisTitel=12\t\t\t\tSchriftgröße der Wahrscheinlichkeits-Ergebnisspaltenüberschrift in pt.\n\n\n[Eigenschaften der Wahrscheinlichkeits-Ergebnisspalte]\n\nWskBreite=1.5\t\t\t\t\t\t\tBreite der Wahrscheinlichkeitsspalte ganz rechts in cm. Für die meisten Baumdiagramme ist der voreingestellte Wert passend.\nBruchErgebnis=0\t\t\t\t\t\t\tMögliche Werte: 0: Wahrscheinlichkeiten in der Ergebnisspalte werden als Dezimalzahlen angegeben. 1: oder als Bruch\nBruchErgebnisFormel=1\t\t\t\t\t\tMögliche Werte: 0, Bruch wird mit / dargestellt, zum Beispiel 5/7. 1, Bruch wird als Formelobjekt übereinander dargestellt.\n\nRahmenWsk=none\t\t\t\t\t\t\tZeichnet einen Rahmen um die Pfadwahrscheinlichkeiten. Mögliche Werte: none (kein Rahmen), solid (Linie) oder dash (gestrichelt).\n\nRahmenWskErgebnisStrichelungStrichAnzahl1=1\t\t\tBei Strichelungen mit zwei verschiedenen Strichen, kann hier die Anzahl der ersten Art von Strichen gesetzt werden. Macht nur Sinn, wenn dash bei Rahmen aktiviert ist.\nRahmenWskErgebnisStrichelungStrichLaenge1=0\t\t\tDie Länge der Striche 1. Art. 0 bedeutet punktförmig.\nRahmenWskErgebnisStrichelungStrichAnzahl2=0\t\t\tAnzahl Striche 2. Art.\nRahmenWskErgebnisStrichelungStrichLaenge2=0\t\t\tDie Länge der Striche 2. Art. 0 bedeutet punktförmig.\nRahmenWskErgebnisStrichelungAbstand=0.1\t\t\t\tAbstand der Striche.\n\nRahmenWskErgebnisDicke=0.0\t\t\t\t\tZwischen 0 und 5 cm.\nRahmenWskErgebnisFarbe=000000\t\t\t\t\tHexadezimale Farbe für Ergebnisspalten-Überschrifts-Rahmen.\nRahmenWskErgebnisUndurchsichtigkeit=100\t\t\t\tMögliche Werte: 0 bis 100 in Prozent. 0 ist völlig transparent, 100 völlig opak.\nFuellungWskErgebnis=0\t\t\t\t\t\tMögliche Werte: 0 (keine Füllung), 1 (einfarbig), 2 (Farbverlauf), 3 (Schraffur), 4 (Hintergrundbild).\nFuellfarbeWskErgebnis=00FF00\t\t\t\t\tHexadezimale Farbe für Ergebnisspaltenüberschrift\nFuellundurchsichtigkeitWskErgebnis=100\t\t\t\ts.o. 0 ist völlig transparent, 100 völlig undurchsichtig.\n\nFuellungFarbverlaufArtWskErgebnis=ellipsoid\t\t\tArt des Farbverlaufs, z.B. rectangular.\nFuellungFarbverlaufZentrumXWskErgebnis=0\t\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungFarbverlaufZentrumYWskErgebnis=0\t\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungFarbverlaufAnfangsfarbeWskErgebnis=000000\t\tHexadezimale Farbe.\nFuellungFarbverlaufEndfarbeWskErgebnis=FFFFFF\t\t\tHexadezimale Farbe.\nFuellungFarbverlaufAnfangsintensitaetWskErgebnis=000000\t\tHexadezimale Farbe.\nFuellungFarbverlaufEndintensitaetWskErgebnis=FFFFFF\t\tHexadezimale Farbe.\nFuellungFarbverlaufWinkelWskErgebnis=450\t\t\tWinkel in Zehntel-Grad. 450 bedeutet 45°.\nFuellungFarbverlaufRandWskErgebnis=20\t\t\t\tAngaben in Prozent. Mögliche Werte zwischen 0 und 100.\n\nFuellungTransparenzverlaufArtWskErgebnis=ellipsoid\t\tArt des Transparenzverlaufs, z.B. rectangular.\nFuellungTransparenzverlaufZentrumXWskErgebnis=0\t\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufZentrumYWskErgebnis=0\t\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufAnfangWskErgebnis=50\t\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufEndWskErgebnis=80\t\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufWinkelWskErgebnis=450\t\t\tWinkel in Zehntel-Grad. 450 bedeutet 45°.\nFuellungTransparenzverlaufRandWskErgebnis=20\t\t\tAngaben in Prozent. Mögliche Werte zwischen 0 und 100.\n\nFuellungSchraffurArtWskErgebnis=1\t\t\t\tAnzahl der überkreuzten Linien. Mögliche Werte 1 (nur parallele), 2 (Parallelogramme) oder 3.\nFuellungSchraffurFarbeWskErgebnis=000000\t\t\tHexadezimale Farbe der Linien.\nFuellungSchraffurAbstandWskErgebnis=0.05\t\t\tAbstand der Linien in cm.\nFuellungSchraffurRotationWskErgebnis=450\t\t\tDrehung der Linien in Zehntel-Grad. 0 bedeutet waagrecht, 900 senkrecht. 450 steht für 45°.\n\nFuellungBildWskErgebnis=Sky\t\t\t\t\tBild als Füllung der Wahrscheinlichkeits-Ergebnisspalte. Mögliche Werte: Sky, Aqua, Coarse, Space, Metal, Wet, Marble, Linen, Stone, Space_20_Metal, Pebbles, Wall, Red_20_Wall, Pattern, Leaves, Lawn_20_Artificial, Daisy, Orange, Fiery, Roses\n\nSchattenWskErgebnis=invisible\t\t\t\t\tSchattenwurf aktivieren (visible) oder deaktivieren (invisible)\nSchattenVerschiebungXWskErgebnis=0.2\t\t\t\tVerschiebung des Schattens nach rechts in cm. Für Verschiebungen nach links negative Werte wählen.\nSchattenVerschiebungYWskErgebnis=0.2\t\t\t\tVerschiebung des Schattens nach unten in cm. Für Verschiebungen nach oben negative Werte wählen.\nSchattenFarbeWskErgebnis=000000\t\t\t\t\tHexadezimale Farbe des Schattens. s.o.\nSchattenUndurchsichtigkeitWskErgebnis=65\t\t\tOpazität (Undurchsichtigkeit) des Schattens in Prozent. 0 bedeutet voll transparent, 100 überhaupt nicht.\n\nFettWskErgebnis=none\t\t\t\t\t\tWerte none oder bold. Falls Formeln aktiviert sind, bold oder leer lassen.\nKursivWskErgebnis=none\t\t\t\t\t\tKursivdruck ja oder nein: Werte none oder italic. Falls Formeln aktiviert sind, italic oder leer lassen.\nSchriftfarbeWskErgebnis=000000\t\t\t\t\tSchriftfarbe in hexadezimaler Schreibweise. Falls Formeln aktiviert sind, muss hier black, white, cyan, magenta, yellow, red oder green eingetragen werden.\nSchriftartWskErgebnis=Times New Roman\t\t\t\tSchriftart einstellen. Exakte Schreibweise ist notwendig.\nSchriftgroesseWskErgebnis=12\t\t\t\t\tSchriftgröße der Wahrscheinlichkeits-Ergebnisspalte in pt.\n");
    fclose(fp);
  }
  else
  {
    fclose(fp);
  }

  fp = fopen(svgeinstellungsdatei,"r");
  if(fp == NULL)
  {
    fp = fopen(svgeinstellungsdatei,"w");
    fprintf(fp, "[Allgemeine Einstellungen]\n\nDateipfad=0\t\t\tPfad wo die Baumdiagrammdateien gespeichert werden sollen. Standardwert (0) ist das eigene Benutzerverzeichnis.\nProgramm=0\t\t\tProgramm inklusive Pfad, mit dem die Baumdiagramme (als svg-Dateien) geöffnet werden sollen. Recht gut funktioniert zum Beispiel Chromium bzw. Chrome oder Mozilla Firefox. 0 bedeutet nichts öffnen.\nSeitenHoehe=0\t\t\tEinheit cm. DIN A4 Format ist 29.7 mal 21.0 cm DIN A3 42.0 mal 29.7 cm. 0 bedeutet automatische Größenanpassung an das Baumdiagramm.\nSeitenBreite=0\t\t\tEinheit cm. DIN A4 Format ist 29.7 mal 21.0 cm DIN A3 42.0 mal 29.7 cm. 0 bedeutet automatische Größenanpassung an das Baumdiagramm.\nSeitenRandLinks=2\t\tEinheit cm.\nSeitenRandRechts=2\t\tEinheit cm.\nSeitenRandOben=2\t\tEinheit cm.\nSeitenrandUnten=2\t\tEinheit cm.\nOptimierung=0\t\t\tMögliche Werte: 0: Die obigen beiden Werte gelten. 1: Das Baumdiagramm nutzt eine komplette DIN A4-Seite im Hochformat. 2: Wie 1 nur im Querformat. 3 oder 4: wie 1 oder 2 nur im DIN A3-Format.\nUrneNull=0\t\t\tMögliche Werte: 0, um beim Ziehen aus einer Urne ohne Zurücklegen Pfade mit Wahrscheinlichkeit 0 nicht anzuzeigen, 1 um diese anzuzeigen.\nUrneKuerzen=1\t\t\tMögliche Werte: 0 um die Zweig-Wahrscheinlichkeiten beim Urnenmodell nicht zu kürzen, 1 um sie zu kürzen.\nErgebnisSpalteAnzeigen=1\tMögliche Werte: 1 für anzeigen, 0 für nicht anzeigen.\nErgebnisSpalteWskAnzeigen=1\tMögliche Werte: 1 für anzeigen, 0 für nicht anzeigen. Diese wird natürlich nur angezeigt, wenn Wahrscheinlichkeiten angegeben werden.\nGenauigkeit=-1\t\t\tAnzahl der Nachkommastellen der Wahrscheinlichkeiten. -1 bedeutet so genau wie möglich. Mit -1 werden zum Beispiel 0,1 oder 0,25 oder 0,125 dargestellt, bei 3 würde 0,100 oder 0,250 oder 0,125 angezeigt werden. Nur relevant, wenn nicht \"Bruch\" eingestellt ist.\n\n\n[Eigenschaften der Zweige des Baumdiagramms]\n\nHorizontalerKnotenAbstand=3\t\tSo weit in cm sollen die einzelnen \"Spalten\" des Baumdiagramms auseinander liegen: Entspricht der \"Breite\" (Länge in x-Richtung) der Zweige.\nZweigFarbe=000000\t\t\tAnzeigefarbe: none (Zweige sind unsichtbar) oder Farbe in hexadezimaler Schreibweise, oder in svg-Farbschlüsselwörtern (black, maroon, green, olive, navy, purple, teal, silver, gray, red, lime, yellow, blue, fuchsia, aqua, white). Bei der hexadezimalen Schreibweise stehen die erste zwei Stellen für den Rotanteil, die zweiten zwei für den Grünanteil und die letzten für den Blauanteil. 000000 z.B. ist schwarz, 0000FF blau, 00FF00 grün und FF0000 rot. 808080 wäre ein mittlerer Grau-Ton. Etc.\nZweigStrichelung=none\t\t\tEine gerade Anzahl an mit Komma getrennten Zahlen mit folgender Bedeutung: Länge in Pixel des ersten Strichs, Abstand in Pixel zum nächsten Strich, Länge in Pixel des zweiten Strichs, etc. Es können beliebig viele Werte gesetzt werden.\nZweigDicke=1\t\t\t\tDicke in px (Pixel).\nZweigUndurchsichtigkeit=100\t\tWerte von 0 bis 100 in Prozent. 0 bedeutet völlig transparent (durchsichtig), 100 völlig opak (undurchsichtig).\n\n\n[Eigenschaften der Zweigwahrscheinlichkeiten]\n\nBruchZweige=0\t\t\t\tWie oben: 0, Dezimalzahlen auf den Zweigen, 1, Brüche auf den Zweigen.\nBruchDarstellungZweig=1\t\t\t0 bedeutet Brüche werden mit Schrägstrich von links nach rechts geschrieben, z.B. 5/7. 1 bedeutet die normale Schreibweise, Zähler über Nenner.\nBruchDrehung=1\t\t\t\tWird nur beachtet, wenn BruchdarstellungZweig gleich 1 ist. 0 bedeutet, die Bruchstriche verlaufen parallel zu den Zweigen, 1 heißt, die Bruchstriche verlaufen horizontal (parallel zum Rand).\nWskVerschiebung=0.5\t\t\tVerschiebung der Wahrscheinlichkeit nach rechts. Negative Werte sorgen für eine Verschiebung nach links.\nZweigWskAbstand=5\t\t\tAbstand der Wahrscheinlichkeiten vom Zweig in Pixel (px).\nSchriftartWsk=Times New Roman\t\tSchriftart einstellen. Exakte Schreibweise ist notwendig. Wird von den meisten Anzeigeprogrammen leider nicht unterstützt.\nSchriftgroesseWsk=12\t\t\tSchriftgröße der Zweigwahrscheinlichkeiten in pt.\n\n\n[Eigenschaften der Knotenpunkte des Baumdiagramms]\n\nKnotenBreite=0\t\t\t\t\tSo breit in cm sollen die Knotenpunkte sein. 0 passt die Breite automatisch an die Schriftgröße an.\nKnotenHoehe=0\t\t\t\t\tSo hoch in cm sollen die Knotenpunkte sein. 0 passt die Höhe automatisch an die Schriftgröße an.\nVertikalerKnotenAbstand=2\t\t\tSo viele cm sollen die letzten (am weitesten rechts liegenden) Knotenpunkte auseinander liegen.\nRahmenKnotenFarbe=none\t\t\t\tZeichnet einen Rahmen um die Knotenpunkte. Mögliche Werte: none (kein Rahmen) oder hexadezimale Farbe, z.B. ff0000 für rot, oder die Farbwörter black, maroon, green, olive, navy, purple, teal, silver, gray, red, lime, yellow, blue, fuchsia, aqua, white.\nRahmenKnotenStrichelung=7,3,5,4\t\t\tEine gerade Anzahl an mit Komma getrennten Zahlen mit folgender Bedeutung: Länge in Pixel des ersten Strichs, Abstand in Pixel zum nächsten Strich, Länge in Pixel des zweiten Strichs, etc. Es können beliebig viele Werte gesetzt werden.\nRahmenKnotenDicke=1\t\t\t\tDicke in px (Pixel).\nRahmenKnotenUndurchsichtigkeit=100\t\tMögliche Werte: 0 bis 100 in Prozent. 0 ist völlig transparent, 100 völlig opak.\nFuellfarbeKnoten=none\t\t\t\tHexadezimale Farbe für Knotenpunkte.\nFuellundurchsichtigkeitKnoten=100\t\tIn Prozent. s.o. 0 ist völlig transparent, 100 völlig undurchsichtig.\nSchriftartKnoten=Times New Roman\t\tSchriftart einstellen. Exakte Schreibweise ist notwendig.\nSchriftgroesseKnoten=12\t\t\t\tSchriftgröße der Ergebnisspaltenüberschrift in pt.\n\n\n[Eigenschaften der Ergebnisspaltenüberschrift des Baumdiagramms]\n\nRahmenErgebnisTitelFarbe=none\t\t\t\t\tZeichnet einen Rahmen um die Überschrift der Ergebnisspalte. Mögliche Werte: none (kein Rahmen) oder hexadezimale Farbe, z.B. ff0000 für rot, oder die Farbwörter black, maroon, green, olive, navy, purple, teal, silver, gray, red, lime, yellow, blue, fuchsia, aqua, white.\nRahmenErgebnisTitelStrichelung=1,2,3,4\t\t\t\tEine gerade Anzahl an mit Komma getrennten Zahlen mit folgender Bedeutung: Länge in Pixel des ersten Strichs, Abstand in Pixel zum nächsten Strich, Länge in Pixel des zweiten Strichs, etc. Es können beliebig viele Werte gesetzt werden.\nRahmenErgebnisTitelDicke=0.0\t\t\t\t\tDicke in px (Pixel).\nRahmenErgebnisTitelUndurchsichtigkeit=100\t\t\tMögliche Werte: 0 bis 100 in Prozent. 0 ist völlig transparent, 100 völlig opak.\nFuellfarbeErgebnisTitel=none\t\t\t\t\tHexadezimale Farbe für Ergebnisspaltenüberschrift\nFuellundurchsichtigkeitErgebnisTitel=100\t\t\ts.o. 0 ist völlig transparent, 100 völlig undurchsichtig.\nSchriftartErgebnisTitel=Times New Roman\t\t\t\tSchriftart einstellen. Exakte Schreibweise ist notwendig.\nSchriftgroesseErgebnisTitel=12\t\t\t\t\tSchriftgröße der Ergebnisspaltenüberschrift in pt.\n\n\n[Eigenschaften der Ergebnisspalte des Baumdiagramms]\n\nErgebnisBreite=0\t\t\t\t\tBreite der Ergebnisspalte in cm. 0 bedeutet, dass diese automatisch angepasst wird.\nRahmenErgebnisFarbe=none\t\t\t\tZeichnet einen Rahmen um die Pfadergebnisse. Mögliche Werte: none (kein Rahmen) oder hexadezimale Farbe, z.B. ff0000 für rot, oder die Farbwörter black, maroon, green, olive, navy, purple, teal, silver, gray, red, lime, yellow, blue, fuchsia, aqua, white.\nRahmenErgebnisStrichelung=5,5,6,6,7,7\t\t\tEine gerade Anzahl an mit Komma getrennten Zahlen mit folgender Bedeutung: Länge in Pixel des ersten Strichs, Abstand in Pixel zum nächsten Strich, Länge in Pixel des zweiten Strichs, etc. Es können beliebig viele Werte gesetzt werden.\nRahmenErgebnisDicke=1\t\t\t\t\tDicke in px (Pixel).\nRahmenErgebnisUndurchsichtigkeit=100\t\t\tMögliche Werte: 0 bis 100 in Prozent. 0 ist völlig transparent, 100 völlig opak.\nFuellfarbeErgebnis=none\t\t\t\t\tHexadezimale Farbe für Ergebnisspalteneinträge.\nFuellundurchsichtigkeitErgebnis=100\t\t\ts.o. 0 ist völlig transparent, 100 völlig undurchsichtig.\nSchriftartErgebnis=Times New Roman\t\t\tSchriftart einstellen. Exakte Schreibweise ist notwendig.\nSchriftgroesseErgebnis=12\t\t\t\tSchriftgröße der Ergebnisspalte in pt.\n\n\n[Eigenschaften der Überschrift der Wahrscheinlichkeits-Ergebnisspalte]\n\nRahmenWskErgebnisTitelFarbe=none\t\t\tZeichnet einen Rahmen um die Überschrift der Pfadwahrscheinlichkeiten-Spalte. Mögliche Werte: none (kein Rahmen) oder hexadezimale Farbe, z.B. ff0000 für rot, oder die Farbwörter black, maroon, green, olive, navy, purple, teal, silver, gray, red, lime, yellow, blue, fuchsia, aqua, white.\nRahmenWskErgebnisTitelStrichelung=1,4,5,3\t\tEine gerade Anzahl an mit Komma getrennten Zahlen mit folgender Bedeutung: Länge in Pixel des ersten Strichs, Abstand in Pixel zum nächsten Strich, Länge in Pixel des zweiten Strichs, etc. Es können beliebig viele Werte gesetzt werden.\nRahmenWskErgebnisTitelDicke=0.0\t\t\t\tDicke in Pixel.\nRahmenWskErgebnisTitelUndurchsichtigkeit=100\t\tMögliche Werte: 0 bis 100 in Prozent. 0 ist völlig transparent, 100 völlig opak.\nFuellfarbeWskErgebnisTitel=none\t\t\t\tHexadezimale Farbe für Wahrscheinlichkeits-Ergebnisspaltenüberschrift.\nFuellundurchsichtigkeitWskErgebnisTitel=100\t\ts.o. 0 ist völlig transparent, 100 völlig undurchsichtig.\nSchriftartWskErgebnisTitel=Times New Roman\t\tSchriftart einstellen. Exakte Schreibweise ist notwendig.\nSchriftgroesseWskErgebnisTitel=12\t\t\tSchriftgröße der Wahrscheinlichkeits-Ergebnisspaltenüberschrift in pt.\n\n\n[Eigenschaften der Wahrscheinlichkeits-Ergebnisspalte]\n\nWskBreite=1.5\t\t\t\t\t\tBreite der Wahrscheinlichkeitsspalte ganz rechts in cm. Für die meisten Baumdiagramme ist der voreingestellte Wert passend.\nBruchErgebnis=0\t\t\t\t\t\tMögliche Werte: 0: Wahrscheinlichkeiten in der Ergebnisspalte werden als Dezimalzahlen angegeben. 1: oder als Bruch\nBruchDarstellungErgebnis=1\t\t\t\t0 bedeutet Brüche werden mit Schrägstrich von links nach rechts geschrieben, z.B. 5/7. 1 bedeutet die normale Schreibweise, Zähler über Nenner.\n\nRahmenWskErgebnisFarbe=none\t\t\t\tZeichnet einen Rahmen um die Pfadwahrscheinlichkeiten. Mögliche Werte: none (kein Rahmen) oder hexadezimale Farbe, z.B. ff0000 für rot, oder die Farbwörter black, maroon, green, olive, navy, purple, teal, silver, gray, red, lime, yellow, blue, fuchsia, aqua, white.\nRahmenWskErgebnisStrichelung=7,5,8,9\t\t\tEine gerade Anzahl an mit Komma getrennten Zahlen mit folgender Bedeutung: Länge in Pixel des ersten Strichs, Abstand in Pixel zum nächsten Strich, Länge in Pixel des zweiten Strichs, etc. Es können beliebig viele Werte gesetzt werden.\nRahmenWskErgebnisDicke=0.0\t\t\t\tDicke in px (Pixel).\nRahmenWskErgebnisUndurchsichtigkeit=100\t\t\tMögliche Werte: 0 bis 100 in Prozent. 0 ist völlig transparent, 100 völlig opak.\nFuellfarbeWskErgebnis=none\t\t\t\tHexadezimale Farbe für Ergebnisspaltenüberschrift\nFuellundurchsichtigkeitWskErgebnis=100\t\t\ts.o. 0 ist völlig transparent, 100 völlig undurchsichtig.\nSchriftartWskErgebnis=Times New Roman\t\t\tSchriftart einstellen. Exakte Schreibweise ist notwendig.\nSchriftgroesseWskErgebnis=12\t\t\t\tSchriftgröße der Wahrscheinlichkeits-Ergebnisspalte in pt.\n");
    fclose(fp);
  }
  else
  {
    fclose(fp);
  }

  short odt=0;
  short svg=1;

  char Dateipfad[4097]="0";
  char odtProgramm[4097]="0";
  float SeitenHoehe=0;
  float SeitenBreite=0;
  float SeitenRandLinks=2;
  float SeitenRandRechts=2;
  float SeitenRandOben=2;
  float SeitenRandUnten=2;
  short Gruppierung=0;
  short Optimierung=0;
  short UrneNull=0;
  short UrneKuerzen=1;
  short ErgebnisSpalteAnzeigen=1;
  short ErgebnisSpalteWskAnzeigen=1;
  short Genauigkeit=-1;
  float HorizontalerKnotenAbstand=3;
  char Zweig[6]="solid";
  int ZweigStrichelungStrichAnzahl1=1;
  float ZweigStrichelungStrichLaenge1=1;
  int ZweigStrichelungStrichAnzahl2=1;
  float ZweigStrichelungStrichLaenge2=1;
  float ZweigStrichelungAbstand=1;
  float ZweigDicke=0;
  char ZweigFarbe[7]="000000";
  char ZweigAnfangForm[27]="none";
  float ZweigAnfangDicke=0;
  char ZweigEndeForm[27]="none";
  float ZweigEndeDicke=0;
  short ZweigUndurchsichtigkeit=100;
  short BruchZweige=1;
  short BruchZweigeFormel=1;
  short LeerzeichenAnzahl=8;
  char WahrscheinlichkeitsPositionAufZweig[7]="center";
  float VerschiebungWskNachUnten=0.05;
  float VerschiebungWskNachOben=0.05;
  float VerschiebungWskNachRechts=0.05;
  float VerschiebungWskNachLinks=0.05;
  char FettWsk[5]="none";
  char KursivWsk[7]="none";
  char SchriftfarbeWsk[7]="000000";
  char SchriftartWsk[40]="Times New Roman";
  short SchriftgroesseWsk=12;
  float KnotenBreite=0.5;
  float KnotenHoehe=0.5;
  float VertikalerKnotenAbstand=0.5;
  short KnotenForm=0;
  char RahmenKnoten[6]="none";
  int RahmenKnotenStrichelungStrichAnzahl1=1;
  float RahmenKnotenStrichelungStrichLaenge1=0;
  int RahmenKnotenStrichelungStrichAnzahl2=0;
  float RahmenKnotenStrichelungStrichLaenge2=0;
  float RahmenKnotenStrichelungAbstand=0.1;
  float RahmenKnotenDicke=0.0;
  char RahmenKnotenFarbe[7]="FFFFFF";
  short RahmenKnotenUndurchsichtigkeit=100;
  short FuellungKnoten=0;
  char FuellfarbeKnoten[7]="FFFFFF";
  short FuellundurchsichtigkeitKnoten=100;
  char FuellungFarbverlaufArtKnoten[12]="ellipsoid";
  short FuellungFarbverlaufZentrumXKnoten=0;
  short FuellungFarbverlaufZentrumYKnoten=0;
  char FuellungFarbverlaufAnfangsfarbeKnoten[7]="000000";
  char FuellungFarbverlaufEndfarbeKnoten[7]="FFFFFF";
  short FuellungFarbverlaufAnfangsintensitaetKnoten=0;
  short FuellungFarbverlaufEndintensitaetKnoten=0;
  short FuellungFarbverlaufWinkelKnoten=450;
  short FuellungFarbverlaufRandKnoten=20;
  char FuellungTransparenzverlaufArtKnoten[12]="ellipsoid";
  short FuellungTransparenzverlaufZentrumXKnoten=0;
  short FuellungTransparenzverlaufZentrumYKnoten=0;
  short FuellungTransparenzverlaufAnfangKnoten=50;
  short FuellungTransparenzverlaufEndKnoten=80;
  short FuellungTransparenzverlaufWinkelKnoten=450;
  short FuellungTransparenzverlaufRandKnoten=20;
  short FuellungSchraffurArtKnoten=1;
  char FuellungSchraffurFarbeKnoten[7]="000000";
  float FuellungSchraffurAbstandKnoten=0.05;
  short FuellungSchraffurRotationKnoten=450;
  char FuellungBildKnoten[19]="Sky";
  char SchattenKnoten[10]="invisible";
  float SchattenVerschiebungXKnoten=0.2;
  float SchattenVerschiebungYKnoten=0.2;
  char SchattenFarbeKnoten[7]="000000";
  short SchattenUndurchsichtigkeitKnoten=65;
  char FettKnoten[5]="none";
  char KursivKnoten[7]="none";
  char SchriftfarbeKnoten[7]="000000";
  char SchriftartKnoten[40]="Times New Roman";
  short SchriftgroesseKnoten=12;
  char RahmenErgebnisTitel[6]="none";
  int RahmenErgebnisTitelStrichelungStrichAnzahl1=1;
  float RahmenErgebnisTitelStrichelungStrichLaenge1=0;
  int RahmenErgebnisTitelStrichelungStrichAnzahl2=0;
  float RahmenErgebnisTitelStrichelungStrichLaenge2=0;
  float RahmenErgebnisTitelStrichelungAbstand=0.1;
  float RahmenErgebnisTitelDicke=0.0;
  char RahmenErgebnisTitelFarbe[7]="FFFFFF";
  short RahmenErgebnisTitelUndurchsichtigkeit=100;
  short FuellungErgebnisTitel=0;
  char FuellfarbeErgebnisTitel[7]="FFFFFF";
  short FuellundurchsichtigkeitErgebnisTitel=100;
  char FuellungFarbverlaufArtErgebnisTitel[12]="ellipsoid";
  short FuellungFarbverlaufZentrumXErgebnisTitel=0;
  short FuellungFarbverlaufZentrumYErgebnisTitel=0;
  char FuellungFarbverlaufAnfangsfarbeErgebnisTitel[7]="000000";
  char FuellungFarbverlaufEndfarbeErgebnisTitel[7]="FFFFFF";
  short FuellungFarbverlaufAnfangsintensitaetErgebnisTitel=0;
  short FuellungFarbverlaufEndintensitaetErgebnisTitel=0;
  short FuellungFarbverlaufWinkelErgebnisTitel=450;
  short FuellungFarbverlaufRandErgebnisTitel=20;
  char FuellungTransparenzverlaufArtErgebnisTitel[12]="ellipsoid";
  short FuellungTransparenzverlaufZentrumXErgebnisTitel=0;
  short FuellungTransparenzverlaufZentrumYErgebnisTitel=0;
  short FuellungTransparenzverlaufAnfangErgebnisTitel=50;
  short FuellungTransparenzverlaufEndErgebnisTitel=80;
  short FuellungTransparenzverlaufWinkelErgebnisTitel=450;
  short FuellungTransparenzverlaufRandErgebnisTitel=20;
  short FuellungSchraffurArtErgebnisTitel=1;
  char FuellungSchraffurFarbeErgebnisTitel[7]="000000";
  float FuellungSchraffurAbstandErgebnisTitel=0.05;
  short FuellungSchraffurRotationErgebnisTitel=450;
  char FuellungBildErgebnisTitel[19]="Sky";
  char SchattenErgebnisTitel[10]="invisible";
  float SchattenVerschiebungXErgebnisTitel=0.2;
  float SchattenVerschiebungYErgebnisTitel=0.2;
  char SchattenFarbeErgebnisTitel[7]="000000";
  short SchattenUndurchsichtigkeitErgebnisTitel=65;
  char FettErgebnisTitel[5]="none";
  char KursivErgebnisTitel[7]="none";
  char SchriftfarbeErgebnisTitel[7]="000000";
  char SchriftartErgebnisTitel[40]="Times New Roman";
  short SchriftgroesseErgebnisTitel=12;
  float ErgebnisBreite=0;
  char RahmenErgebnis[6]="none";
  int RahmenErgebnisStrichelungStrichAnzahl1=1;
  float RahmenErgebnisStrichelungStrichLaenge1=0;
  int RahmenErgebnisStrichelungStrichAnzahl2=0;
  float RahmenErgebnisStrichelungStrichLaenge2=0;
  float RahmenErgebnisStrichelungAbstand=0.1;
  float RahmenErgebnisDicke=0.0;
  char RahmenErgebnisFarbe[7]="FFFFFF";
  short RahmenErgebnisUndurchsichtigkeit=100;
  short FuellungErgebnis=0;
  char FuellfarbeErgebnis[7]="FFFFFF";
  short FuellundurchsichtigkeitErgebnis=100;
  char FuellungFarbverlaufArtErgebnis[12]="ellipsoid";
  short FuellungFarbverlaufZentrumXErgebnis=0;
  short FuellungFarbverlaufZentrumYErgebnis=0;
  char FuellungFarbverlaufAnfangsfarbeErgebnis[7]="000000";
  char FuellungFarbverlaufEndfarbeErgebnis[7]="FFFFFF";
  short FuellungFarbverlaufAnfangsintensitaetErgebnis=0;
  short FuellungFarbverlaufEndintensitaetErgebnis=0;
  short FuellungFarbverlaufWinkelErgebnis=450;
  short FuellungFarbverlaufRandErgebnis=20;
  char FuellungTransparenzverlaufArtErgebnis[12]="ellipsoid";
  short FuellungTransparenzverlaufZentrumXErgebnis=0;
  short FuellungTransparenzverlaufZentrumYErgebnis=0;
  short FuellungTransparenzverlaufAnfangErgebnis=50;
  short FuellungTransparenzverlaufEndErgebnis=80;
  short FuellungTransparenzverlaufWinkelErgebnis=450;
  short FuellungTransparenzverlaufRandErgebnis=20;
  short FuellungSchraffurArtErgebnis=1;
  char FuellungSchraffurFarbeErgebnis[7]="000000";
  float FuellungSchraffurAbstandErgebnis=0.05;
  short FuellungSchraffurRotationErgebnis=450;
  char FuellungBildErgebnis[19]="Sky";
  char SchattenErgebnis[10]="invisible";
  float SchattenVerschiebungXErgebnis=0.2;
  float SchattenVerschiebungYErgebnis=0.2;
  char SchattenFarbeErgebnis[7]="000000";
  short SchattenUndurchsichtigkeitErgebnis=65;
  char FettErgebnis[5]="none";
  char KursivErgebnis[7]="none";
  char SchriftfarbeErgebnis[7]="000000";
  char SchriftartErgebnis[40]="Times New Roman";
  short SchriftgroesseErgebnis=12;
  char RahmenWskErgebnisTitel[6]="none";
  int RahmenWskErgebnisTitelStrichelungStrichAnzahl1=1;
  float RahmenWskErgebnisTitelStrichelungStrichLaenge1=0;
  int RahmenWskErgebnisTitelStrichelungStrichAnzahl2=0;
  float RahmenWskErgebnisTitelStrichelungStrichLaenge2=0;
  float RahmenWskErgebnisTitelStrichelungAbstand=0.1;
  float RahmenWskErgebnisTitelDicke=0.0;
  char RahmenWskErgebnisTitelFarbe[7]="FFFFFF";
  short RahmenWskErgebnisTitelUndurchsichtigkeit=100;
  short FuellungWskErgebnisTitel=0;
  char FuellfarbeWskErgebnisTitel[7]="FFFFFF";
  short FuellundurchsichtigkeitWskErgebnisTitel=100;
  char FuellungFarbverlaufArtWskErgebnisTitel[12]="ellipsoid";
  short FuellungFarbverlaufZentrumXWskErgebnisTitel=0;
  short FuellungFarbverlaufZentrumYWskErgebnisTitel=0;
  char FuellungFarbverlaufAnfangsfarbeWskErgebnisTitel[7]="000000";
  char FuellungFarbverlaufEndfarbeWskErgebnisTitel[7]="FFFFFF";
  short FuellungFarbverlaufAnfangsintensitaetWskErgebnisTitel=0;
  short FuellungFarbverlaufEndintensitaetWskErgebnisTitel=0;
  short FuellungFarbverlaufWinkelWskErgebnisTitel=450;
  short FuellungFarbverlaufRandWskErgebnisTitel=20;
  char FuellungTransparenzverlaufArtWskErgebnisTitel[12]="ellipsoid";
  short FuellungTransparenzverlaufZentrumXWskErgebnisTitel=0;
  short FuellungTransparenzverlaufZentrumYWskErgebnisTitel=0;
  short FuellungTransparenzverlaufAnfangWskErgebnisTitel=50;
  short FuellungTransparenzverlaufEndWskErgebnisTitel=80;
  short FuellungTransparenzverlaufWinkelWskErgebnisTitel=450;
  short FuellungTransparenzverlaufRandWskErgebnisTitel=20;
  short FuellungSchraffurArtWskErgebnisTitel=1;
  char FuellungSchraffurFarbeWskErgebnisTitel[7]="000000";
  float FuellungSchraffurAbstandWskErgebnisTitel=0.05;
  short FuellungSchraffurRotationWskErgebnisTitel=450;
  char FuellungBildWskErgebnisTitel[19]="Sky";
  char SchattenWskErgebnisTitel[10]="invisible";
  float SchattenVerschiebungXWskErgebnisTitel=0.2;
  float SchattenVerschiebungYWskErgebnisTitel=0.2;
  char SchattenFarbeWskErgebnisTitel[7]="000000";
  short SchattenUndurchsichtigkeitWskErgebnisTitel=65;
  char FettWskErgebnisTitel[5]="none";
  char KursivWskErgebnisTitel[7]="none";
  char SchriftfarbeWskErgebnisTitel[7]="000000";
  char SchriftartWskErgebnisTitel[40]="Times New Roman";
  short SchriftgroesseWskErgebnisTitel=12;
  float WskBreite=1.5;
  short BruchErgebnis=1;
  short BruchErgebnisFormel=1;
  char RahmenWskErgebnis[6]="none";
  int RahmenWskErgebnisStrichelungStrichAnzahl1=1;
  float RahmenWskErgebnisStrichelungStrichLaenge1=0;
  int RahmenWskErgebnisStrichelungStrichAnzahl2=0;
  float RahmenWskErgebnisStrichelungStrichLaenge2=0;
  float RahmenWskErgebnisStrichelungAbstand=0.1;
  float RahmenWskErgebnisDicke=0.0;
  char RahmenWskErgebnisFarbe[7]="FFFFFF";
  short RahmenWskErgebnisUndurchsichtigkeit=100;
  short FuellungWskErgebnis=0;
  char FuellfarbeWskErgebnis[7]="FFFFFF";
  short FuellundurchsichtigkeitWskErgebnis=100;
  char FuellungFarbverlaufArtWskErgebnis[12]="ellipsoid";
  short FuellungFarbverlaufZentrumXWskErgebnis=0;
  short FuellungFarbverlaufZentrumYWskErgebnis=0;
  char FuellungFarbverlaufAnfangsfarbeWskErgebnis[7]="000000";
  char FuellungFarbverlaufEndfarbeWskErgebnis[7]="FFFFFF";
  short FuellungFarbverlaufAnfangsintensitaetWskErgebnis=0;
  short FuellungFarbverlaufEndintensitaetWskErgebnis=0;
  short FuellungFarbverlaufWinkelWskErgebnis=450;
  short FuellungFarbverlaufRandWskErgebnis=20;
  char FuellungTransparenzverlaufArtWskErgebnis[12]="ellipsoid";
  short FuellungTransparenzverlaufZentrumXWskErgebnis=0;
  short FuellungTransparenzverlaufZentrumYWskErgebnis=0;
  short FuellungTransparenzverlaufAnfangWskErgebnis=50;
  short FuellungTransparenzverlaufEndWskErgebnis=80;
  short FuellungTransparenzverlaufWinkelWskErgebnis=450;
  short FuellungTransparenzverlaufRandWskErgebnis=20;
  short FuellungSchraffurArtWskErgebnis=1;
  char FuellungSchraffurFarbeWskErgebnis[7]="000000";
  float FuellungSchraffurAbstandWskErgebnis=0.05;
  short FuellungSchraffurRotationWskErgebnis=450;
  char FuellungBildWskErgebnis[19]="Sky";
  char SchattenWskErgebnis[10]="invisible";
  float SchattenVerschiebungXWskErgebnis=0.2;
  float SchattenVerschiebungYWskErgebnis=0.2;
  char SchattenFarbeWskErgebnis[7]="000000";
  short SchattenUndurchsichtigkeitWskErgebnis=65;
  char FettWskErgebnis[5]="none";
  char KursivWskErgebnis[7]="none";
  char SchriftfarbeWskErgebnis[7]="000000";
  char SchriftartWskErgebnis[40]="Times New Roman";
  short SchriftgroesseWskErgebnis=12;

  char svgDateipfad[4097]="0";
  char svgProgramm[4097]="0";
  float svgSeitenHoehe=0;
  float svgSeitenBreite=0;
  float svgSeitenRandLinks=2;
  float svgSeitenRandRechts=2;
  float svgSeitenRandOben=2;
  float svgSeitenRandUnten=2;
  short svgOptimierung=0;
  short svgUrneNull=0;
  short svgUrneKuerzen=1;
  short svgErgebnisSpalteAnzeigen=1;
  short svgErgebnisSpalteWskAnzeigen=1;
  short svgGenauigkeit=-1;
  float svgHorizontalerKnotenAbstand=3;
  char svgZweigFarbe[9]="black";
  char *svgZweigStrichelung;
  float svgZweigDicke=1;
  short svgZweigUndurchsichtigkeit=100;
  short svgBruchZweige=1;
  short svgBruchDarstellungZweig=1;
  short svgBruchDrehung=0;
  float svgWskVerschiebung=0;
  float svgZweigWskAbstand=5;
  char svgSchriftartWsk[40]="Times New Roman";
  short svgSchriftgroesseWsk=12;
  float svgKnotenBreite=0.5;
  float svgKnotenHoehe=0.5;
  float svgVertikalerKnotenAbstand=0.5;
  char svgRahmenKnotenFarbe[9]="none";
  char *svgRahmenKnotenStrichelung;
  float svgRahmenKnotenDicke=1;
  short svgRahmenKnotenUndurchsichtigkeit=100;
  char svgFuellfarbeKnoten[9]="none";
  short svgFuellundurchsichtigkeitKnoten=100;
  char svgSchriftartKnoten[40]="Times New Roman";
  short svgSchriftgroesseKnoten=12;
  char svgRahmenErgebnisTitelFarbe[9]="none";
  char *svgRahmenErgebnisTitelStrichelung;
  float svgRahmenErgebnisTitelDicke=1;
  short svgRahmenErgebnisTitelUndurchsichtigkeit=100;
  char svgFuellfarbeErgebnisTitel[9]="none";
  short svgFuellundurchsichtigkeitErgebnisTitel=100;
  char svgSchriftartErgebnisTitel[40]="Times New Roman";
  short svgSchriftgroesseErgebnisTitel=12;
  float svgErgebnisBreite=0;
  char svgRahmenErgebnisFarbe[9]="none";
  char *svgRahmenErgebnisStrichelung;
  float svgRahmenErgebnisDicke=1;
  short svgRahmenErgebnisUndurchsichtigkeit=100;
  char svgFuellfarbeErgebnis[9]="none";
  short svgFuellundurchsichtigkeitErgebnis=100;
  char svgSchriftartErgebnis[40]="Times New Roman";
  short svgSchriftgroesseErgebnis=12;
  char svgRahmenWskErgebnisTitelFarbe[9]="none";
  char *svgRahmenWskErgebnisTitelStrichelung;
  float svgRahmenWskErgebnisTitelDicke=1;
  short svgRahmenWskErgebnisTitelUndurchsichtigkeit=100;
  char svgFuellfarbeWskErgebnisTitel[9]="none";
  short svgFuellundurchsichtigkeitWskErgebnisTitel=100;
  char svgSchriftartWskErgebnisTitel[40]="Times New Roman";
  short svgSchriftgroesseWskErgebnisTitel=12;
  float svgWskBreite=1.5;
  short svgBruchErgebnis=1;
  short svgBruchDarstellungErgebnis=1;
  char svgRahmenWskErgebnisFarbe[9]="none";
  char *svgRahmenWskErgebnisStrichelung;
  float svgRahmenWskErgebnisDicke=1;
  short svgRahmenWskErgebnisUndurchsichtigkeit=100;
  char svgFuellfarbeWskErgebnis[9]="none";
  short svgFuellundurchsichtigkeitWskErgebnis=100;
  char svgSchriftartWskErgebnis[40]="Times New Roman";
  short svgSchriftgroesseWskErgebnis=12;


  char *Suchergebnis;
  int temp = 0;

  fp = fopen(einstellungsdatei,"r");
  fseek(fp,0L,SEEK_END);
  long int dateigroesse = ftell(fp);
  //printf("Dateigroesse = %ld\n",dateigroesse);
  rewind(fp);
  char *Einstellungen=calloc(dateigroesse+2,sizeof(char));
  j = 0;
  while((temp = fgetc(fp))!=EOF)
  {
    if(!temp)
    {
      break;
    }
    Einstellungen[j]=temp;
    j++;
  }
  fclose(fp);
  Einstellungen[j] = 0;

  if(zeichenzaehlen(Einstellungen, 61) > 2)
  {
    fprintf(stderr,"\n\tWarnung: Die Datei Dateiarten ist möglicherweise fehlerhaft.\n\tEs wurden mehr =-Zeichen gefunden als erwartet.\n\n");
  }
  if(zeichenzaehlen(Einstellungen, 61) < 2 || strstr(Einstellungen,"==") != NULL)    // es gibt 2 =-Zeichen (ASCII 61) in der Einstellungsdatei. Wenn es weniger sind, ist die Datei unvollständg, und wird neu erstellt.
  {
    fprintf(stderr,"\n\tWarnung: Die Datei Dateiarten ist fehlerhaft!\n\tSie wird nun neu erstellt.\n\tVon der bisherigen Datei wird eine Sicherungskopie angelegt.\n\n");
    char sicherungsdatei[4097];
    strcpy(sicherungsdatei,einstellungsdatei);
    strcat(sicherungsdatei,".bak");
    fp = fopen(sicherungsdatei,"r");
    i=0;
    while(fp != NULL)
    {
      //printf("Sicherungsdatei existiert schon.\n");
      fclose(fp);
      i++;
      sprintf(sicherungsdatei,"%s%i.bak",einstellungsdatei,i);
      //printf("%s\n",sicherungsdatei);
      fp = fopen(sicherungsdatei,"r");
    }
    rename(einstellungsdatei,sicherungsdatei);     // Die alten Einstellungen werden gesichert.
    fp = fopen(einstellungsdatei,"w");
    fprintf(fp, "Hinweis: Die Reihenfolge der Dateiarten darf nicht verändert werden. Sie können hier einstellen, welche Art von Dateien Sie erstellen möchten:\nOpenDocumentText (odt). Diese Dateien können zum Beispiel mit LibreOffice Writer oder OpenOffice Writer geöffnet werden.\nScalable Vector Graphics (svg). Diese skalierbaren Vektorgrafiken können zum Beispiel mit Google Chrome, Mozilla Firefox oder anderen Programmen geöffnet werden. Leider erfüllen nicht alle Programme, die svg darstellen können, den svg-Standard komplett, so dass es gerade bei den Textelementen zu Darstellungsfehlern kommen kann.\n1 bedeutet, dass die Datei erstellt wird. 0, dass sie nicht erstellt wird. Bitte sonst nichts an der Datei ändern. Bitte keine Leerzeichen nach den Gleichheitszeichen hinzufügen.\n\nodt=0\nsvg=1\n");
    fclose(fp);
  }
  else
  {
    Suchergebnis = strtok(Einstellungen,"=");
    Suchergebnis = strtok(NULL,"=");
    if(Suchergebnis[0] == 48)
    {
      odt = 0;
    }
    else
    {
      odt = 1;
    }
    Suchergebnis = strtok(NULL,"=");
    if(Suchergebnis[0] == 48)
    {
      svg = 0;
    }
    else
    {
      svg = 1;
    }
  }
  free(Einstellungen);
  if(!svg && !odt)
  {
    fprintf(stderr,"Wenn Sie weder odt-Dateien noch svg-Dateien erstellen wollen, brauchen Sie kein Programm!");
    return 2;
  }


  if(odt)
  {
    fp = fopen(odteinstellungsdatei,"r");
    fseek(fp,0L,SEEK_END);
    dateigroesse = ftell(fp);
    //printf("Dateigroesse = %ld\n",dateigroesse);
    rewind(fp);
    Einstellungen=calloc(dateigroesse+2,sizeof(char));
    j = 0;
    while((temp = fgetc(fp))!=EOF)
    {
      if(!temp)
      {
        break;
      }
      Einstellungen[j]=temp;
      j++;
    }
    fclose(fp);
    Einstellungen[j] = 0;
    if(zeichenzaehlen(Einstellungen, 61) > 265)
    {
      fprintf(stderr,"\n\tWarnung: Die Datei odtEinstellungen ist möglicherweise fehlerhaft.\n\tEs wurden mehr =-Zeichen gefunden als erwartet.\n\n");
    }
    if(zeichenzaehlen(Einstellungen, 61) < 265 || strstr(Einstellungen,"==") != NULL)    // es gibt 265 =-Zeichen (ASCII 61) in der Einstellungsdatei. Wenn es weniger sind, ist die Datei unvollständg, und wird neu erstellt.
    {
      fprintf(stderr,"\n\tWarnung: Die Datei odtEinstellungen ist fehlerhaft!\n\tSie wird nun neu erstellt.\n\tVon der bisherigen Datei wird eine Sicherungskopie angelegt.\n\n");
      char sicherungsdatei[4097];
      strcpy(sicherungsdatei,odteinstellungsdatei);
      strcat(sicherungsdatei,".bak");
      fp = fopen(sicherungsdatei,"r");
      i=0;
      while(fp != NULL)
      {
        //printf("Sicherungsdatei existiert schon.\n");
        fclose(fp);
        i++;
        sprintf(sicherungsdatei,"%s%i.bak",einstellungsdatei,i);
        //printf("%s\n",sicherungsdatei);
        fp = fopen(sicherungsdatei,"r");
      }
      rename(odteinstellungsdatei,sicherungsdatei);     // Die alten Einstellungen werden gesichert.
      fp = fopen(odteinstellungsdatei,"w");
      fprintf(fp, "[Allgemeine Einstellungen]\n\nDateipfad=0\t\t\tPfad wo die Baumdiagrammdateien gespeichert werden sollen. Standardwert (0) ist das eigene Benutzerverzeichnis.\nProgramm=0\t\t\tProgramm inklusive Pfad, mit dem die Baumdiagramme (als odt-Dateien) geöffnet werden sollen. Z.B. C:\\Programme\\LibreOffice 4.2\\program\\swriter.exe. Muss nur angegeben werden, wenn Libreoffice/Openoffice nicht standardmäßig im übichen Programme-Pfad liegen. Mit 0 wird einfach Libreoffice bzw. OpenOffice geöffnet, sofern es gefunden wird. Wenn nichts geöffnet werden sollen, einfach leer lassen.\nSeitenHoehe=0\t\t\tEinheit cm. DIN A4 Format ist 29,7 mal 21,0 cm DIN A3 42,0 mal 29,7 cm. 0 bedeutet automatische Größenanpassung an das Baumdiagramm.\nSeitenBreite=0\t\t\tEinheit cm. DIN A4 Format ist 29,7 mal 21,0 cm DIN A3 42,0 mal 29,7 cm. 0 bedeutet automatische Größenanpassung an das Baumdiagramm.\nSeitenRandLinks=2\t\tEinheit cm.\nSeitenRandRechts=2\t\tEinheit cm.\nSeitenRandOben=2\t\tEinheit cm.\nSeitenRandUnten=2\t\tEinheit cm.\nGruppierung=0\t\t\t0: Alles gruppiert. 1: Keine Gruppierung. 2: Alles außer Bruch-Formeln in der Ergebnisspalte wird gruppiert.\nOptimierung=0\t\t\tMögliche Werte: 0: Die obigen beiden Werte gelten. 1: Das Baumdiagramm nutzt eine komplette DIN A4-Seite im Hochformat. 2: Wie 1 nur im Querformat. 3 oder 4: wie 1 oder 2 nur im DIN A3-Format.\nUrneNull=0\t\t\tMögliche Werte: 0, um beim Ziehen aus einer Urne ohne Zurücklegen Pfade mit Wahrscheinlichkeit 0 nicht anzuzeigen, 1 um diese anzuzeigen.\nUrneKuerzen=1\t\t\tMögliche Werte: 0 um die Zweig-Wahrscheinlichkeiten beim Urnenmodell nicht zu kürzen, 1 um sie zu kürzen.\nErgebnisSpalteAnzeigen=1\tMögliche Werte: 1 für anzeigen, 0 für nicht anzeigen.\nErgebnisSpalteWskAnzeigen=1\tMögliche Werte: 1 für anzeigen, 0 für nicht anzeigen. Diese wird natürlich nur angezeigt, wenn Wahrscheinlichkeiten angegeben werden.\nGenauigkeit=-1\t\t\tAnzahl der Nachkommastellen der Wahrscheinlichkeiten. -1 bedeutet so genau wie möglich. Mit -1 werden zum Beispiel 0,1 oder 0,25 oder 0,125 dargestellt, bei 3 würde 0,100 oder 0,250 oder 0,125 angezeigt werden. Nur relevant, wenn nicht \"Bruch\" eingestellt ist.\n\n\n[Eigenschaften der Zweige des Baumdiagramms]\n\nHorizontalerKnotenAbstand=3\t\tSo weit in cm sollen die einzelnen \"Spalten\" des Baumdiagramms auseinander liegen: Entspricht der \"Breite\" (Länge in x-Richtung) der Zweige.\nZweig=solid\t\t\t\tAnzeigeform der Zweige: none (keine), solid (Linie), dash (gestrichelte Linie).\n\nZweigStrichelungStrichAnzahl1=1\t\tBei Strichelungen mit zwei verschiedenen Strichen, kann hier die Anzahl der ersten Art von Strichen gesetzt werden. Macht nur Sinn, wenn dash bei Zweig aktiviert ist.\nZweigStrichelungStrichLaenge1=1\t\tDie Länge der Striche 1. Art. 0 bedeutet punktförmig.\nZweigStrichelungStrichAnzahl2=1\t\tAnzahl Striche 2. Art.\nZweigStrichelungStrichLaenge2=1\t\tDie Länge der Striche 2. Art. 0 bedeutet punktförmig.\nZweigStrichelungAbstand=1\t\tAbstand der Striche.\n\nZweigDicke=0\t\t\t\tWerte zwischen 0 und 5 in cm.\nZweigFarbe=000000\t\t\tHexadezimal RGB. Die erste zwei Stellen stehen für den Rotanteil, die zweiten zwei für den Grünanteil und die letzten für den Blauanteil. 000000 z.B. ist schwarz, 0000FF blau, 00FF00 grün und FF0000 rot. 808080 wäre ein mittlerer Grau-Ton. Etc.\nZweigAnfangForm=none\t\t\tMögliche Werte: none, Square, Arrow, Small_20_Arrow, Dimension_20_Lines, Double_20_Arrow, Rounded_20_short_20_Arrow, Symmetric_20_Arrow, Line_20_Arrow, Rounded_20_large_20_Arrow, Circle, Square_20_45, Arrow_20_concave, Short_20_line_20_Arrow, Triangle_20_unfilled, Diamond_20_unfilled, Diamond, Circle_20_unfilled, Square_20_45_20_unfilled, Square_20_unfilled, Half_20_Circle_20_unfilled\nZweigAnfangDicke=0\t\t\tMögliche Werte 0 bis 5 in cm.\nZweigEndeForm=none\t\t\tMögliche Werte: none, Square, Arrow, Small_20_Arrow, Dimension_20_Lines, Double_20_Arrow, Rounded_20_short_20_Arrow, Symmetric_20_Arrow, Line_20_Arrow, Rounded_20_large_20_Arrow, Circle, Square_20_45, Arrow_20_concave, Short_20_line_20_Arrow, Triangle_20_unfilled, Diamond_20_unfilled, Diamond, Circle_20_unfilled, Square_20_45_20_unfilled, Square_20_unfilled, Half_20_Circle_20_unfilled\nZweigEndeDicke=0\t\t\tMögliche Werte 0 bis 5 in cm.\nZweigUndurchsichtigkeit=100\t\tWerte von 0 bis 100 in Prozent. 0 bedeutet völlig transparent (durchsichtig), 100 völlig opak (undurchsichtig).\n\n\n[Eigenschaften der Zweigwahrscheinlichkeiten]\n\nBruchZweige=0\t\t\t\t\tWie oben: 0, Dezimalzahlen auf den Zweigen, 1, Brüche auf den Zweigen.\nBruchZweigeFormel=1\t\t\t\t0, Die Brüche auf den Zweigen werden nebeneinander mit / dargestellt, z.B. 5/7. 1, die Brüche werden wie üblich \"Zähler über Nenner\" dargestellt.\nLeerzeichenAnzahl=0\t\t\t\tHiermit können die Wahrscheinlichkeiten auf den Zweigen noch etwas nach rechts geschoben werden. Alternativ lässt sich auch die Variable VerschiebungWskNachRechts nutzen.\nWahrscheinlichkeitsPositionAufZweig=center\tMögliche Werte: left, center, right.\nVerschiebungWskNachUnten=0.05\t\t\tDie Position der Zweigwahrscheinlichkeiten anpassen (Einheit cm).\nVerschiebungWskNachOben=0.05\t\t\tDie Position der Zweigwahrscheinlichkeiten anpassen (Einheit cm).\nVerschiebungWskNachRechts=0.8\t\t\tDie Position der Zweigwahrscheinlichkeiten anpassen (Einheit cm).\nVerschiebungWskNachLinks=0.05\t\t\tDie Position der Zweigwahrscheinlichkeiten anpassen (Einheit cm).\nFettWsk=\t\t\t\t\tWerte none oder bold\nKursivWsk=\t\t\t\t\tKursivdruck ja oder nein: Werte none oder italic\nSchriftfarbeWsk=black\t\t\t\tSchriftfarbe in hexadezimaler Schreibweise\nSchriftartWsk=Times New Roman\t\t\tSchriftart einstellen. Exakte Schreibweise ist notwendig.\nSchriftgroesseWsk=12\t\t\t\tSchriftgröße der Zweigwahrscheinlichkeiten in pt.\n\n\n[Eigenschaften der Knotenpunkte des Baumdiagramms]\n\nKnotenBreite=0\t\t\t\t\tSo breit in cm sollen die Knotenpunkte sein. 0 bedeutet automatische Anpassung an die Schrfitgröße und -art.\nKnotenHoehe=0\t\t\t\t\tSo hoch in cm sollen die Knotenpunkte sein. 0 bedeutet automatische Anpassung an die Schrfitgröße und -art.\nVertikalerKnotenAbstand=2\t\t\tSo viele cm sollen die letzten (am weitesten rechts liegenden) Knotenpunkte auseinander liegen.\nKnotenForm=0\t\t\t\t\t0 entspricht Rechteck, 1 entspricht Ellipse (Kreis). Macht nur Sinn, wenn Rahmen oder Fuellung aktiviert wurde.\nRahmenKnoten=none\t\t\t\tZeichnet einen Rahmen um die Knotenpunkte. Mögliche Werte: none (kein Rahmen), solid (Linie) oder dash (gestrichelt).\n\nRahmenKnotenStrichelungStrichAnzahl1=1\t\tBei Strichelungen mit zwei verschiedenen Strichen, kann hier die Anzahl der ersten Art von Strichen gesetzt werden. Macht nur Sinn, wenn dash bei Rahmen aktiviert ist.\nRahmenKnotenStrichelungStrichLaenge1=0\t\tDie Länge der Striche 1. Art. 0 bedeutet punktförmig.\nRahmenKnotenStrichelungStrichAnzahl2=0\t\tAnzahl Striche 2. Art.\nRahmenKnotenStrichelungStrichLaenge2=0\t\tDie Länge der Striche 2. Art. 0 bedeutet punktförmig.\nRahmenKnotenStrichelungAbstand=0.1\t\tAbstand der Striche.\n\nRahmenKnotenDicke=0.0\t\t\t\tZwischen 0 und 5 cm.\nRahmenKnotenFarbe=000000\t\t\tHexadezimal RGB. Die erste zwei Stellen stehen für den Rotanteil, die zweiten zwei für den Grünanteil und die letzten für den Blauanteil. 000000 z.B. ist schwarz, 0000FF blau, 00FF00 grün und FF0000 rot. 808080 wäre ein mittlerer Grau-Ton. Etc.\nRahmenKnotenUndurchsichtigkeit=100\t\tMögliche Werte: 0 bis 100 in Prozent. 0 ist völlig transparent, 100 völlig opak.\n\nFuellungKnoten=0\t\t\t\tMögliche Werte: 0 (keine Füllung), 1 (einfarbig), 2 (Farbverlauf), 3 (Schraffur), 4 (Hintergrundbild).\nFuellfarbeKnoten=FFFFFF\t\t\t\tHexadezimale Farbe für Knotenpunkte\nFuellundurchsichtigkeitKnoten=100\t\tIn Prozent. s.o. 0 ist völlig transparent, 100 völlig undurchsichtig.\n\nFuellungFarbverlaufArtKnoten=ellipsoid\t\tArt des Farbverlaufs. Mögliche Werte: linear, axial, radial, ellipsoid, square, rectangular\nFuellungFarbverlaufZentrumXKnoten=50\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100. Bei linear und axial hinfällig.\nFuellungFarbverlaufZentrumYKnoten=50\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100. Bei linear und axial hinfällig.\nFuellungFarbverlaufAnfangsfarbeKnoten=FF0000\tHexadezimale Farbe.\nFuellungFarbverlaufEndfarbeKnoten=0000FF\tHexadezimale Farbe.\nFuellungFarbverlaufAnfangsintensitaetKnoten=0\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungFarbverlaufEndintensitaetKnoten=100\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungFarbverlaufWinkelKnoten=450\t\tWinkel in Zehntel-Grad. 450 bedeutet 45°.\nFuellungFarbverlaufRandKnoten=1\t\t\tAngaben in Prozent. Mögliche Werte zwischen 0 und 100.\n\nFuellungTransparenzverlaufArtKnoten=ellipsoid\tArt des Transparenzverlaufs. Mögliche Werte: linear, axial, radial, ellipsoid, square, rectangular, oder 0 für keinen Verlauf.\nFuellungTransparenzverlaufZentrumXKnoten=50\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100. Bei linear und axial hinfällig.\nFuellungTransparenzverlaufZentrumYKnoten=50\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100. Bei linear und axial hinfällig.\nFuellungTransparenzverlaufAnfangKnoten=0\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufEndKnoten=100\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufWinkelKnoten=450\tWinkel in Zehntel-Grad. 450 bedeutet 45°.\nFuellungTransparenzverlaufRandKnoten=1\t\tAngaben in Prozent. Mögliche Werte zwischen 0 und 100.\n\nFuellungSchraffurArtKnoten=1\t\t\tAnzahl der überkreuzten Linien. Mögliche Werte 1 (nur parallele), 2 (Parallelogramme) oder 3.\nFuellungSchraffurFarbeKnoten=000000\t\tHexadezimale Farbe der Linien.\nFuellungSchraffurAbstandKnoten=0.05\t\tAbstand der Linien in cm.\nFuellungSchraffurRotationKnoten=450\t\tDrehung der Linien in Zehntel-Grad. 0 bedeutet waagrecht, 900 senkrecht. 450 steht für 45°.\n\nFuellungBildKnoten=Sky\t\t\t\tBild als Knotenfüllung. Mögliche Werte: Sky, Aqua, Coarse, Space, Metal, Wet, Marble, Linen, Stone, Space_20_Metal, Pebbles, Wall, Red_20_Wall, Pattern, Leaves, Lawn_20_Artificial, Daisy, Orange, Fiery, Roses\n\nSchattenKnoten=invisible\t\t\tSchattenwurf aktivieren (visible) oder deaktivieren (invisible)\nSchattenVerschiebungXKnoten=0.2\t\t\tVerschiebung des Schattens nach rechts in cm. Für Verschiebungen nach links negative Werte wählen.\nSchattenVerschiebungYKnoten=0.2\t\t\tVerschiebung des Schattens nach unten in cm. Für Verschiebungen nach oben negative Werte wählen.\nSchattenFarbeKnoten=000000\t\t\tHexadezimale Farbe des Schattens. s.o.\nSchattenUndurchsichtigkeitKnoten=65\t\tOpazität (Undurchsichtigkeit) des Schattens in Prozent. 0 bedeutet voll transparent, 100 überhaupt nicht.\n\nFettKnoten=none\t\t\t\t\tSchrift der Knotenpunkte Fett. Werte: none oder bold.\nKursivKnoten=none\t\t\t\tKursivdruck ja oder nein: Werte none oder italic\nSchriftfarbeKnoten=000000\t\t\tSchriftfarbe in hexadezimaler Schreibweise\nSchriftartKnoten=Times New Roman\t\tSchriftart einstellen. Exakte Schreibweise ist notwendig.\nSchriftgroesseKnoten=12\t\t\t\tSchriftgröße der Ergebnisspaltenüberschrift in pt.\n\n\n[Eigenschaften der Ergebnisspaltenüberschrift des Baumdiagramms]\n\nRahmenErgebnisTitel=none\t\t\t\t\tZeichnet einen Rahmen um die Überschrift der Ergebnisspalte. Mögliche Werte: none (kein Rahmen), solid (Linie) oder dash (gestrichelt).\n\nRahmenErgebnisTitelStrichelungStrichAnzahl1=1\t\t\tBei Strichelungen mit zwei verschiedenen Strichen, kann hier die Anzahl der ersten Art von Strichen gesetzt werden. Macht nur Sinn, wenn dash bei Rahmen aktiviert ist.\nRahmenErgebnisTitelStrichelungStrichLaenge1=0\t\t\tDie Länge der Striche 1. Art. 0 bedeutet punktförmig.\nRahmenErgebnisTitelStrichelungStrichAnzahl2=0\t\t\tAnzahl Striche 2. Art.\nRahmenErgebnisTitelStrichelungStrichLaenge2=0\t\t\tDie Länge der Striche 2. Art. 0 bedeutet punktförmig.\nRahmenErgebnisTitelStrichelungAbstand=0.1\t\t\tAbstand der Striche.\n\nRahmenErgebnisTitelDicke=0.0\t\t\t\t\tZwischen 0 und 5 cm.\nRahmenErgebnisTitelFarbe=000000\t\t\t\t\tHexadezimale Farbe für Ergebnisspalten-Überschrifts-Rahmen.\nRahmenErgebnisTitelUndurchsichtigkeit=100\t\t\tMögliche Werte: 0 bis 100 in Prozent. 0 ist völlig transparent, 100 völlig opak.\n\nFuellungErgebnisTitel=0\t\t\t\t\t\tMögliche Werte: 0 (keine Füllung), 1 (einfarbig), 2 (Farbverlauf), 3 (Schraffur), 4 (Hintergrundbild).\nFuellfarbeErgebnisTitel=FFFFFF\t\t\t\t\tHexadezimale Farbe für Ergebnisspaltenüberschrift\nFuellundurchsichtigkeitErgebnisTitel=100\t\t\ts.o. 0 ist völlig transparent, 100 völlig undurchsichtig.\n\nFuellungFarbverlaufArtErgebnisTitel=ellipsoid\t\t\tArt des Farbverlaufs, z.B. rectangular.\nFuellungFarbverlaufZentrumXErgebnisTitel=0\t\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungFarbverlaufZentrumYErgebnisTitel=0\t\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungFarbverlaufAnfangsfarbeErgebnisTitel=000000\t\tHexadezimale Farbe.\nFuellungFarbverlaufEndfarbeErgebnisTitel=FFFFFF\t\t\tHexadezimale Farbe.\nFuellungFarbverlaufAnfangsintensitaetErgebnisTitel=0\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungFarbverlaufEndintensitaetErgebnisTitel=0\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungFarbverlaufWinkelErgebnisTitel=450\t\t\tWinkel in Zehntel-Grad. 450 bedeutet 45°.\nFuellungFarbverlaufRandErgebnisTitel=20\t\t\t\tAngaben in Prozent. Mögliche Werte zwischen 0 und 100.\n\nFuellungTransparenzverlaufArtErgebnisTitel=ellipsoid\t\tArt des Transparenzverlaufs, z.B. rectangular.\nFuellungTransparenzverlaufZentrumXErgebnisTitel=0\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufZentrumYErgebnisTitel=0\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufAnfangErgebnisTitel=50\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufEndErgebnisTitelErgebnisTitel=80\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufWinkelErgebnisTitel=450\t\tWinkel in Zehntel-Grad. 450 bedeutet 45°.\nFuellungTransparenzverlaufRandErgebnisTitel=20\t\t\tAngaben in Prozent. Mögliche Werte zwischen 0 und 100.\n\nFuellungSchraffurArtErgebnisTitel=2\t\t\t\tAnzahl der überkreuzten Linien. Mögliche Werte 1 (nur parallele), 2 (Parallelogramme) oder 3.\nFuellungSchraffurFarbeErgebnisTitel=10FF00\t\t\tHexadezimale Farbe der Linien.\nFuellungSchraffurAbstandErgebnisTitel=0.03\t\t\tAbstand der Linien in cm.\nFuellungSchraffurRotationErgebnisTitel=200\t\t\tDrehung der Linien in Zehntel-Grad. 0 bedeutet waagrecht, 900 senkrecht. 450 steht für 45°.\n\nFuellungBildErgebnisTitel=Sky\t\t\t\t\tBild als Füllung der Ergebnisspalten-Überschrift. Mögliche Werte: Sky, Aqua, Coarse, Space, Metal, Wet, Marble, Linen, Stone, Space_20_Metal, Pebbles, Wall, Red_20_Wall, Pattern, Leaves, Lawn_20_Artificial, Daisy, Orange, Fiery, Roses\n\nSchattenErgebnisTitel=invisible\t\t\t\t\tSchattenwurf aktivieren (visible) oder deaktivieren (invisible)\nSchattenVerschiebungXErgebnisTitel=0.2\t\t\t\tVerschiebung des Schattens nach rechts in cm. Für Verschiebungen nach links negative Werte wählen.\nSchattenVerschiebungYErgebnisTitel=0.2\t\t\t\tVerschiebung des Schattens nach unten in cm. Für Verschiebungen nach oben negative Werte wählen.\nSchattenFarbeErgebnisTitel=000000\t\t\t\tHexadezimale Farbe des Schattens. s.o.\nSchattenUndurchsichtigkeitErgebnisTitel=65\t\t\tOpazität (Undurchsichtigkeit) des Schattens in Prozent. 0 bedeutet voll transparent, 100 überhaupt nicht.\n\nFettErgebnisTitel=none\t\t\t\t\t\tWerte none oder bold\nKursivErgebnisTitel=none\t\t\t\t\tKursivdruck ja oder nein: Werte none oder italic\nSchriftfarbeErgebnisTitel=000000\t\t\t\tSchriftfarbe in hexadezimaler Schreibweise\nSchriftartErgebnisTitel=Times New Roman\t\t\t\tSchriftart einstellen. Exakte Schreibweise ist notwendig.\nSchriftgroesseErgebnisTitel=12\t\t\t\t\tSchriftgröße der Ergebnisspaltenüberschrift in pt.\n\n\n[Eigenschaften der Ergebnisspalte des Baumdiagramms]\n\nErgebnisBreite=0\t\t\t\t\tBreite der Ergebnisspalte in cm. 0 bedeutet, dass diese automatisch angepasst wird.\nRahmenErgebnis=none\t\t\t\t\tZeichnet einen Rahmen um die Pfadergebnisse. Mögliche Werte: none (kein Rahmen), solid (Linie) oder dash (gestrichelt).\n\nRahmenErgebnisStrichelungStrichAnzahl1=1\t\tBei Strichelungen mit zwei verschiedenen Strichen, kann hier die Anzahl der ersten Art von Strichen gesetzt werden. Macht nur Sinn, wenn dash bei Rahmen aktiviert ist.\nRahmenErgebnisStrichelungStrichLaenge1=0\t\tDie Länge der Striche 1. Art. 0 bedeutet punktförmig.\nRahmenErgebnisStrichelungStrichAnzahl2=0\t\tAnzahl Striche 2. Art.\nRahmenErgebnisStrichelungStrichLaenge2=0\t\tDie Länge der Striche 2. Art. 0 bedeutet punktförmig.\nRahmenErgebnisStrichelungAbstand=0.1\t\t\tAbstand der Striche.\n\nRahmenErgebnisDicke=0.0\t\t\t\t\tZwischen 0 und 5 cm.\nRahmenErgebnisFarbe=000000\t\t\t\tHexadezimale Farbe für Ergebnisspalten-Überschrifts-Rahmen.\nRahmenErgebnisUndurchsichtigkeit=100\t\t\tMögliche Werte: 0 bis 100 in Prozent. 0 ist völlig transparent, 100 völlig opak.\n\nFuellungErgebnis=0\t\t\t\t\tMögliche Werte: 0 (keine Füllung), 1 (einfarbig), 2 (Farbverlauf), 3 (Schraffur), 4 (Hintergrundbild).\nFuellfarbeErgebnis=FFFFFF\t\t\t\tHexadezimale Farbe für Ergebnisspaltenüberschrift\nFuellundurchsichtigkeitErgebnis=100\t\t\ts.o. 0 ist völlig transparent, 100 völlig undurchsichtig.\n\nFuellungFarbverlaufArtErgebnis=ellipsoid\t\tArt des Farbverlaufs, z.B. rectangular.\nFuellungFarbverlaufZentrumXErgebnis=0\t\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungFarbverlaufZentrumYErgebnis=0\t\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungFarbverlaufAnfangsfarbeErgebnis=000000\t\tHexadezimale Farbe.\nFuellungFarbverlaufEndfarbeErgebnis=FFFFFF\t\tHexadezimale Farbe.\nFuellungFarbverlaufAnfangsintensitaetErgebnis=000000\tHexadezimale Farbe.\nFuellungFarbverlaufEndintensitaetErgebnis=FFFFFF\tHexadezimale Farbe.\nFuellungFarbverlaufWinkelErgebnis=450\t\t\tWinkel in Zehntel-Grad. 450 bedeutet 45°.\nFuellungFarbverlaufRandErgebnis=20\t\t\tAngaben in Prozent. Mögliche Werte zwischen 0 und 100.\n\nFuellungTransparenzverlaufArtErgebnis=ellipsoid\t\tArt des Transparenzverlaufs, z.B. rectangular.\nFuellungTransparenzverlaufZentrumXErgebnis=0\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufZentrumYErgebnis=0\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufAnfangErgebnis=50\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufEndErgebnis=80\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufWinkelErgebnis=450\t\tWinkel in Zehntel-Grad. 450 bedeutet 45°.\nFuellungTransparenzverlaufRandErgebnis=20\t\tAngaben in Prozent. Mögliche Werte zwischen 0 und 100.\n\nFuellungSchraffurArtErgebnis=1\t\t\t\tAnzahl der überkreuzten Linien. Mögliche Werte 1 (nur parallele), 2 (Parallelogramme) oder 3.\nFuellungSchraffurFarbeErgebnis=000000\t\t\tHexadezimale Farbe der Linien.\nFuellungSchraffurAbstandErgebnis=0.05\t\t\tAbstand der Linien in cm.\nFuellungSchraffurRotationErgebnis=450\t\t\tDrehung der Linien in Zehntel-Grad. 0 bedeutet waagrecht, 900 senkrecht. 450 steht für 45°.\n\nFuellungBildErgebnis=Sky\t\t\t\tBild als Füllung der Ergebnisspalte. Mögliche Werte: Sky, Aqua, Coarse, Space, Metal, Wet, Marble, Linen, Stone, Space_20_Metal, Pebbles, Wall, Red_20_Wall, Pattern, Leaves, Lawn_20_Artificial, Daisy, Orange, Fiery, Roses\n\nSchattenErgebnis=invisible\t\t\t\tSchattenwurf aktivieren (visible) oder deaktivieren (invisible)\nSchattenVerschiebungXErgebnis=0.2\t\t\tVerschiebung des Schattens nach rechts in cm. Für Verschiebungen nach links negative Werte wählen.\nSchattenVerschiebungYErgebnis=0.2\t\t\tVerschiebung des Schattens nach unten in cm. Für Verschiebungen nach oben negative Werte wählen.\nSchattenFarbeErgebnis=000000\t\t\t\tHexadezimale Farbe des Schattens. s.o.\nSchattenUndurchsichtigkeitErgebnis=65\t\t\tOpazität (Undurchsichtigkeit) des Schattens in Prozent. 0 bedeutet voll transparent, 100 überhaupt nicht.\n\nFettErgebnis=none\t\t\t\t\tWerte none oder bold\nKursivErgebnis=none\t\t\t\t\tKursivdruck ja oder nein: Werte none oder italic\nSchriftfarbeErgebnis=000000\t\t\t\tSchriftfarbe in hexadezimaler Schreibweise\nSchriftartErgebnis=Times New Roman\t\t\tSchriftart einstellen. Exakte Schreibweise ist notwendig.\nSchriftgroesseErgebnis=12\t\t\t\tSchriftgröße der Ergebnisspalte in pt.\n\n\n[Eigenschaften der Überschrift der Wahrscheinlichkeits-Ergebnisspalte]\n\nRahmenWskErgebnisTitel=none\t\t\t\t\tZeichnet einen Rahmen um die Überschrift der Pfadwahrscheinlichkeiten-Spalte. Mögliche Werte: none (kein Rahmen), solid (Linie) oder dash (gestrichelt).\n\nRahmenWskErgebnisTitelStrichelungStrichAnzahl1=1\t\tBei Strichelungen mit zwei verschiedenen Strichen, kann hier die Anzahl der ersten Art von Strichen gesetzt werden. Macht nur Sinn, wenn dash bei Rahmen aktiviert ist.\nRahmenWskErgebnisTitelStrichelungStrichLaenge1=0\t\tDie Länge der Striche 1. Art. 0 bedeutet punktförmig.\nRahmenWskErgebnisTitelStrichelungStrichAnzahl2=0\t\tAnzahl Striche 2. Art.\nRahmenWskErgebnisTitelStrichelungStrichLaenge2=0\t\tDie Länge der Striche 2. Art. 0 bedeutet punktförmig.\nRahmenWskErgebnisTitelStrichelungAbstand=0.1\t\t\tAbstand der Striche.\n\nRahmenWskErgebnisTitelDicke=0.0\t\t\t\t\tZwischen 0 und 5 cm.\nRahmenWskErgebnisTitelFarbe=000000\t\t\t\tHexadezimale Farbe für Ergebnisspalten-Überschrifts-Rahmen.\nRahmenWskErgebnisTitelUndurchsichtigkeit=100\t\t\tMögliche Werte: 0 bis 100 in Prozent. 0 ist völlig transparent, 100 völlig opak.\n\nFuellungWskErgebnisTitel=0\t\t\t\t\tMögliche Werte: 0 (keine Füllung), 1 (einfarbig), 2 (Farbverlauf), 3 (Schraffur), 4 (Hintergrundbild).\nFuellfarbeWskErgebnisTitel=FFFFFF\t\t\t\tHexadezimale Farbe für Ergebnisspaltenüberschrift\nFuellundurchsichtigkeitWskErgebnisTitel=100\t\t\ts.o. 0 ist völlig transparent, 100 völlig undurchsichtig.\n\nFuellungFarbverlaufArtWskErgebnisTitel=ellipsoid\t\tArt des Farbverlaufs, z.B. rectangular.\nFuellungFarbverlaufZentrumXWskErgebnisTitel=0\t\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungFarbverlaufZentrumYWskErgebnisTitel=0\t\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungFarbverlaufAnfangsfarbeWskErgebnisTitel=000000\t\tHexadezimale Farbe.\nFuellungFarbverlaufEndfarbeWskErgebnisTitel=FFFFFF\t\tHexadezimale Farbe.\nFuellungFarbverlaufAnfangsintensitaetWskErgebnisTitel=000000\tHexadezimale Farbe.\nFuellungFarbverlaufEndintensitaetWskErgebnisTitel=FFFFFF\tHexadezimale Farbe.\nFuellungFarbverlaufWinkelWskErgebnisTitel=450\t\t\tWinkel in Zehntel-Grad. 450 bedeutet 45°.\nFuellungFarbverlaufRandWskErgebnisTitel=20\t\t\tAngaben in Prozent. Mögliche Werte zwischen 0 und 100.\n\nFuellungTransparenzverlaufArtWskErgebnisTitel=ellipsoid\t\tArt des Transparenzverlaufs, z.B. rectangular.\nFuellungTransparenzverlaufZentrumXWskErgebnisTitel=0\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufZentrumYWskErgebnisTitel=0\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufAnfangWskErgebnisTitel=50\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufEndWskErgebnisTitel=80\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufWinkelWskErgebnisTitel=450\t\tWinkel in Zehntel-Grad. 450 bedeutet 45°.\nFuellungTransparenzverlaufRandWskErgebnisTitel=20\t\tAngaben in Prozent. Mögliche Werte zwischen 0 und 100.\n\nFuellungSchraffurArtWskErgebnisTitel=1\t\t\t\tAnzahl der überkreuzten Linien. Mögliche Werte 1 (nur parallele), 2 (Parallelogramme) oder 3.\nFuellungSchraffurFarbeWskErgebnisTitel=000000\t\t\tHexadezimale Farbe der Linien.\nFuellungSchraffurAbstandWskErgebnisTitel=0.05\t\t\tAbstand der Linien in cm.\nFuellungSchraffurRotationWskErgebnisTitel=450\t\t\tDrehung der Linien in Zehntel-Grad. 0 bedeutet waagrecht, 900 senkrecht. 450 steht für 45°.\n\nFuellungBildWskErgebnisTitel=Sky\t\t\t\tBild als Füllung der Wahrscheinlichkeits-Ergebnisspalten-Überschrift. Mögliche Werte: Sky, Aqua, Coarse, Space, Metal, Wet, Marble, Linen, Stone, Space_20_Metal, Pebbles, Wall, Red_20_Wall, Pattern, Leaves, Lawn_20_Artificial, Daisy, Orange, Fiery, Roses\n\nSchattenWskErgebnisTitel=invisible\t\t\t\tSchattenwurf aktivieren (visible) oder deaktivieren (invisible)\nSchattenVerschiebungXWskErgebnisTitel=0.2\t\t\tVerschiebung des Schattens nach rechts in cm. Für Verschiebungen nach links negative Werte wählen.\nSchattenVerschiebungYWskErgebnisTitel=0.2\t\t\tVerschiebung des Schattens nach unten in cm. Für Verschiebungen nach oben negative Werte wählen.\nSchattenFarbeWskErgebnisTitel=000000\t\t\t\tHexadezimale Farbe des Schattens. s.o.\nSchattenUndurchsichtigkeitWskErgebnisTitel=65\t\t\tOpazität (Undurchsichtigkeit) des Schattens in Prozent. 0 bedeutet voll transparent, 100 überhaupt nicht.\n\nFettWskErgebnisTitel=none\t\t\t\t\tWerte none oder bold\nKursivWskErgebnisTitel=none\t\t\t\t\tKursivdruck ja oder nein: Werte none oder italic\nSchriftfarbeWskErgebnisTitel=000000\t\t\t\tSchriftfarbe in hexadezimaler Schreibweise\nSchriftartWskErgebnisTitel=Times New Roman\t\t\tSchriftart einstellen. Exakte Schreibweise ist notwendig.\nSchriftgroesseWskErgebnisTitel=12\t\t\t\tSchriftgröße der Wahrscheinlichkeits-Ergebnisspaltenüberschrift in pt.\n\n\n[Eigenschaften der Wahrscheinlichkeits-Ergebnisspalte]\n\nWskBreite=1.5\t\t\t\t\t\t\tBreite der Wahrscheinlichkeitsspalte ganz rechts in cm. Für die meisten Baumdiagramme ist der voreingestellte Wert passend.\nBruchErgebnis=0\t\t\t\t\t\t\tMögliche Werte: 0: Wahrscheinlichkeiten in der Ergebnisspalte werden als Dezimalzahlen angegeben. 1: oder als Bruch\nBruchErgebnisFormel=1\t\t\t\t\t\tMögliche Werte: 0, Bruch wird mit / dargestellt, zum Beispiel 5/7. 1, Bruch wird als Formelobjekt übereinander dargestellt.\n\nRahmenWsk=none\t\t\t\t\t\t\tZeichnet einen Rahmen um die Pfadwahrscheinlichkeiten. Mögliche Werte: none (kein Rahmen), solid (Linie) oder dash (gestrichelt).\n\nRahmenWskErgebnisStrichelungStrichAnzahl1=1\t\t\tBei Strichelungen mit zwei verschiedenen Strichen, kann hier die Anzahl der ersten Art von Strichen gesetzt werden. Macht nur Sinn, wenn dash bei Rahmen aktiviert ist.\nRahmenWskErgebnisStrichelungStrichLaenge1=0\t\t\tDie Länge der Striche 1. Art. 0 bedeutet punktförmig.\nRahmenWskErgebnisStrichelungStrichAnzahl2=0\t\t\tAnzahl Striche 2. Art.\nRahmenWskErgebnisStrichelungStrichLaenge2=0\t\t\tDie Länge der Striche 2. Art. 0 bedeutet punktförmig.\nRahmenWskErgebnisStrichelungAbstand=0.1\t\t\t\tAbstand der Striche.\n\nRahmenWskErgebnisDicke=0.0\t\t\t\t\tZwischen 0 und 5 cm.\nRahmenWskErgebnisFarbe=000000\t\t\t\t\tHexadezimale Farbe für Ergebnisspalten-Überschrifts-Rahmen.\nRahmenWskErgebnisUndurchsichtigkeit=100\t\t\t\tMögliche Werte: 0 bis 100 in Prozent. 0 ist völlig transparent, 100 völlig opak.\nFuellungWskErgebnis=0\t\t\t\t\t\tMögliche Werte: 0 (keine Füllung), 1 (einfarbig), 2 (Farbverlauf), 3 (Schraffur), 4 (Hintergrundbild).\nFuellfarbeWskErgebnis=00FF00\t\t\t\t\tHexadezimale Farbe für Ergebnisspaltenüberschrift\nFuellundurchsichtigkeitWskErgebnis=100\t\t\t\ts.o. 0 ist völlig transparent, 100 völlig undurchsichtig.\n\nFuellungFarbverlaufArtWskErgebnis=ellipsoid\t\t\tArt des Farbverlaufs, z.B. rectangular.\nFuellungFarbverlaufZentrumXWskErgebnis=0\t\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungFarbverlaufZentrumYWskErgebnis=0\t\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungFarbverlaufAnfangsfarbeWskErgebnis=000000\t\tHexadezimale Farbe.\nFuellungFarbverlaufEndfarbeWskErgebnis=FFFFFF\t\t\tHexadezimale Farbe.\nFuellungFarbverlaufAnfangsintensitaetWskErgebnis=000000\t\tHexadezimale Farbe.\nFuellungFarbverlaufEndintensitaetWskErgebnis=FFFFFF\t\tHexadezimale Farbe.\nFuellungFarbverlaufWinkelWskErgebnis=450\t\t\tWinkel in Zehntel-Grad. 450 bedeutet 45°.\nFuellungFarbverlaufRandWskErgebnis=20\t\t\t\tAngaben in Prozent. Mögliche Werte zwischen 0 und 100.\n\nFuellungTransparenzverlaufArtWskErgebnis=ellipsoid\t\tArt des Transparenzverlaufs, z.B. rectangular.\nFuellungTransparenzverlaufZentrumXWskErgebnis=0\t\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufZentrumYWskErgebnis=0\t\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufAnfangWskErgebnis=50\t\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufEndWskErgebnis=80\t\t\tAngaben in Prozent. Mögliche Werte: Zwischen 0 und 100.\nFuellungTransparenzverlaufWinkelWskErgebnis=450\t\t\tWinkel in Zehntel-Grad. 450 bedeutet 45°.\nFuellungTransparenzverlaufRandWskErgebnis=20\t\t\tAngaben in Prozent. Mögliche Werte zwischen 0 und 100.\n\nFuellungSchraffurArtWskErgebnis=1\t\t\t\tAnzahl der überkreuzten Linien. Mögliche Werte 1 (nur parallele), 2 (Parallelogramme) oder 3.\nFuellungSchraffurFarbeWskErgebnis=000000\t\t\tHexadezimale Farbe der Linien.\nFuellungSchraffurAbstandWskErgebnis=0.05\t\t\tAbstand der Linien in cm.\nFuellungSchraffurRotationWskErgebnis=450\t\t\tDrehung der Linien in Zehntel-Grad. 0 bedeutet waagrecht, 900 senkrecht. 450 steht für 45°.\n\nFuellungBildWskErgebnis=Sky\t\t\t\t\tBild als Füllung der Wahrscheinlichkeits-Ergebnisspalte. Mögliche Werte: Sky, Aqua, Coarse, Space, Metal, Wet, Marble, Linen, Stone, Space_20_Metal, Pebbles, Wall, Red_20_Wall, Pattern, Leaves, Lawn_20_Artificial, Daisy, Orange, Fiery, Roses\n\nSchattenWskErgebnis=invisible\t\t\t\t\tSchattenwurf aktivieren (visible) oder deaktivieren (invisible)\nSchattenVerschiebungXWskErgebnis=0.2\t\t\t\tVerschiebung des Schattens nach rechts in cm. Für Verschiebungen nach links negative Werte wählen.\nSchattenVerschiebungYWskErgebnis=0.2\t\t\t\tVerschiebung des Schattens nach unten in cm. Für Verschiebungen nach oben negative Werte wählen.\nSchattenFarbeWskErgebnis=000000\t\t\t\t\tHexadezimale Farbe des Schattens. s.o.\nSchattenUndurchsichtigkeitWskErgebnis=65\t\t\tOpazität (Undurchsichtigkeit) des Schattens in Prozent. 0 bedeutet voll transparent, 100 überhaupt nicht.\n\nFettWskErgebnis=none\t\t\t\t\t\tWerte none oder bold. Falls Formeln aktiviert sind, bold oder leer lassen.\nKursivWskErgebnis=none\t\t\t\t\t\tKursivdruck ja oder nein: Werte none oder italic. Falls Formeln aktiviert sind, italic oder leer lassen.\nSchriftfarbeWskErgebnis=000000\t\t\t\t\tSchriftfarbe in hexadezimaler Schreibweise. Falls Formeln aktiviert sind, muss hier black, white, cyan, magenta, yellow, red oder green eingetragen werden.\nSchriftartWskErgebnis=Times New Roman\t\t\t\tSchriftart einstellen. Exakte Schreibweise ist notwendig.\nSchriftgroesseWskErgebnis=12\t\t\t\t\tSchriftgröße der Wahrscheinlichkeits-Ergebnisspalte in pt.\n");
      fclose(fp);
    }
    else
    {
      char seitenhoehe[7] = "", seitenbreite[7] = "", seitenrandlinks[7] = "", seitenrandrechts[7] = "", seitenrandoben[7] = "", seitenrandunten[7] = "", gruppierung[2] = "", optimierung[2] = "", urnenull[2] = "", urnekuerzen[2] = "", ergebnisspalteanzeigen[2] = "", ergebnisspaltewskanzeigen[2] = "", genauigkeit[7] = "", horizontalerknotenabstand[7] = "", zweigstrichelungstrichanzahl1[7] = "", zweigstrichelungstrichlaenge1[7] = "", zweigstrichelungstrichanzahl2[7] = "", zweigstrichelungstrichlaenge2[7] = "", zweigstrichelungstrichabstand[7] = "", zweigdicke[7] = "", zweiganfangdicke[7] = "", zweigendedicke[7] = "", zweigundurchsichtigkeit[7] = "", bruchzweige[2] = "", bruchzweigeformel[2] = "", leerzeichenanzahl[7] = "", verschiebungwsknachunten[7] = "", verschiebungwsknachoben[7] = "", verschiebungwsknachrechts[7] = "", verschiebungwsknachlinks[7] = "", schriftgroessewsk[7] = "", knotenbreite[7] = "", knotenhoehe[7] = "", vertikalerknotenabstand[7] = "", knotenform[2] = "", rahmenknotenstrichelungstrichanzahl1[7] = "", rahmenknotenstrichelungstrichlaenge1[7] = "", rahmenknotenstrichelungstrichanzahl2[7] = "", rahmenknotenstrichelungstrichlaenge2[7] = "", rahmenknotenstrichelungabstand[7] = "", rahmenknotendicke[7] = "", rahmenknotenundurchsichtigkeit[7] = "", fuellundurchsichtigkeitknoten[7] = "", fuellungfarbverlaufzentrumxknoten[7] = "", fuellungfarbverlaufzentrumyknoten[7] = "", fuellungfarbverlaufanfangsintensitaetknoten[7] = "", fuellungfarbverlaufendintensitaetknoten[7] = "", fuellungfarbverlaufwinkelknoten[7] = "", fuellungfarbverlaufrandknoten[7] = "", fuellungtransparenzverlaufzentrumxknoten[7] = "", fuellungtransparenzverlaufzentrumyknoten[7] = "", fuellungtransparenzverlaufanfangknoten[7] = "", fuellungtransparenzverlaufendknoten[7] = "", fuellungtransparenzverlaufwinkelknoten[7] = "", fuellungtransparenzverlaufrandknoten[7] = "", fuellungschraffurartknoten[7] = "", fuellungschraffurabstandknoten[7] = "", fuellungschraffurrotationknoten[7] = "", schattenverschiebungxknoten[7] = "", schattenverschiebungyknoten[7] = "", schattenundurchsichtigkeitknoten[7] = "", schriftgroesseknoten[7] = "", rahmenergebnistitelstrichelungstrichanzahl1[7] = "", rahmenergebnistitelstrichelungstrichlaenge1[7] = "", rahmenergebnistitelstrichelungstrichanzahl2[7] = "", rahmenergebnistitelstrichelungstrichlaenge2[7] = "", rahmenergebnistitelstrichelungabstand[7] = "", rahmenergebnistiteldicke[7] = "", rahmenergebnistitelundurchsichtigkeit[7] = "", fuellundurchsichtigkeitergebnistitel[7] = "", fuellungfarbverlaufzentrumxergebnistitel[7] = "", fuellungfarbverlaufzentrumyergebnistitel[7] = "", fuellungfarbverlaufanfangsintensitaetergebnistitel[7] = "", fuellungfarbverlaufendintensitaetergebnistitel[7] = "", fuellungfarbverlaufwinkelergebnistitel[7] = "", fuellungfarbverlaufrandergebnistitel[7] = "", fuellungtransparenzverlaufzentrumxergebnistitel[7] = "", fuellungtransparenzverlaufzentrumyergebnistitel[7] = "", fuellungtransparenzverlaufanfangergebnistitel[7] = "", fuellungtransparenzverlaufendergebnistitel[7] = "", fuellungtransparenzverlaufwinkelergebnistitel[7] = "", fuellungtransparenzverlaufrandergebnistitel[7] = "", fuellungschraffurartergebnistitel[7] = "", fuellungschraffurabstandergebnistitel[7] = "", fuellungschraffurrotationergebnistitel[7] = "", schattenverschiebungxergebnistitel[7] = "", schattenverschiebungyergebnistitel[7] = "", schattenundurchsichtigkeitergebnistitel[7] = "", schriftgroesseergebnistitel[7] = "", ergebnisbreite[7] = "", rahmenergebnisstrichelungstrichanzahl1[7] = "", rahmenergebnisstrichelungstrichlaenge1[7] = "", rahmenergebnisstrichelungstrichanzahl2[7] = "", rahmenergebnisstrichelungstrichlaenge2[7] = "", rahmenergebnisstrichelungabstand[7] = "", rahmenergebnisdicke[7] = "", rahmenergebnisundurchsichtigkeit[7] = "", fuellundurchsichtigkeitergebnis[7] = "", fuellungfarbverlaufzentrumxergebnis[7] = "", fuellungfarbverlaufzentrumyergebnis[7] = "", fuellungfarbverlaufanfangsintensitaetergebnis[7] = "", fuellungfarbverlaufendintensitaetergebnis[7] = "", fuellungfarbverlaufwinkelergebnis[7] = "", fuellungfarbverlaufrandergebnis[7] = "", fuellungtransparenzverlaufzentrumxergebnis[7] = "", fuellungtransparenzverlaufzentrumyergebnis[7] = "", fuellungtransparenzverlaufanfangergebnis[7] = "", fuellungtransparenzverlaufendergebnis[7] = "", fuellungtransparenzverlaufwinkelergebnis[7] = "", fuellungtransparenzverlaufrandergebnis[7] = "", fuellungschraffurartergebnis[7] = "", fuellungschraffurabstandergebnis[7] = "", fuellungschraffurrotationergebnis[7] = "", schattenverschiebungxergebnis[7] = "", schattenverschiebungyergebnis[7] = "", schattenundurchsichtigkeitergebnis[7] = "", schriftgroesseergebnis[7] = "", rahmenwskergebnistitelstrichelungstrichanzahl1[7] = "", rahmenwskergebnistitelstrichelungstrichlaenge1[7] = "", rahmenwskergebnistitelstrichelungstrichanzahl2[7] = "", rahmenwskergebnistitelstrichelungstrichlaenge2[7] = "", rahmenwskergebnistitelstrichelungabstand[7] = "", rahmenwskergebnistiteldicke[7] = "", rahmenwskergebnistitelundurchsichtigkeit[7] = "", fuellundurchsichtigkeitwskergebnistitel[7] = "", fuellungfarbverlaufzentrumxwskergebnistitel[7] = "", fuellungfarbverlaufzentrumywskergebnistitel[7] = "", fuellungfarbverlaufanfangsintensitaetwskergebnistitel[7] = "", fuellungfarbverlaufendintensitaetwskergebnistitel[7] = "", fuellungfarbverlaufwinkelwskergebnistitel[7] = "", fuellungfarbverlaufrandwskergebnistitel[7] = "", fuellungtransparenzverlaufzentrumxwskergebnistitel[7] = "", fuellungtransparenzverlaufzentrumywskergebnistitel[7] = "", fuellungtransparenzverlaufanfangwskergebnistitel[7] = "", fuellungtransparenzverlaufendwskergebnistitel[7] = "", fuellungtransparenzverlaufwinkelwskergebnistitel[7] = "", fuellungtransparenzverlaufrandwskergebnistitel[7] = "", fuellungschraffurartwskergebnistitel[7] = "", fuellungschraffurabstandwskergebnistitel[7] = "", fuellungschraffurrotationwskergebnistitel[7] = "", schattenverschiebungxwskergebnistitel[7] = "", schattenverschiebungywskergebnistitel[7] = "", schattenundurchsichtigkeitwskergebnistitel[7] = "", schriftgroessewskergebnistitel[7] = "", wskbreite[7] = "", bruchergebnis[2] = "", bruchergebnisformel[2] = "", rahmenwskergebnisstrichelungstrichanzahl1[7] = "", rahmenwskergebnisstrichelungstrichlaenge1[7] = "", rahmenwskergebnisstrichelungstrichanzahl2[7] = "", rahmenwskergebnisstrichelungstrichlaenge2[7] = "", rahmenwskergebnisstrichelungabstand[7] = "", rahmenwskergebnisdicke[7] = "", rahmenwskergebnisundurchsichtigkeit[7] = "", fuellundurchsichtigkeitwskergebnis[7] = "", fuellungfarbverlaufzentrumxwskergebnis[7] = "", fuellungfarbverlaufzentrumywskergebnis[7] = "", fuellungfarbverlaufanfangsintensitaetwskergebnis[7] = "", fuellungfarbverlaufendintensitaetwskergebnis[7] = "", fuellungfarbverlaufwinkelwskergebnis[7] = "", fuellungfarbverlaufrandwskergebnis[7] = "", fuellungtransparenzverlaufzentrumxwskergebnis[7] = "", fuellungtransparenzverlaufzentrumywskergebnis[7] = "", fuellungtransparenzverlaufanfangwskergebnis[7] = "", fuellungtransparenzverlaufendwskergebnis[7] = "", fuellungtransparenzverlaufwinkelwskergebnis[7] = "", fuellungtransparenzverlaufrandwskergebnis[7] = "", fuellungschraffurartwskergebnis[7] = "", fuellungschraffurabstandwskergebnis[7] = "", fuellungschraffurrotationwskergebnis[7] = "", schattenverschiebungxwskergebnis[7] = "", schattenverschiebungywskergebnis[7] = "", schattenundurchsichtigkeitwskergebnis[7] = "", schriftgroessewskergebnis[7] = "";
      Suchergebnis = strtok(Einstellungen,"=");

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 4096)
      {
        Dateipfad[i] = Suchergebnis[i];
        i++;
      }
      //printf("%s\n",Dateipfad);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 4096)
      {
        odtProgramm[i] = Suchergebnis[i];
        i++;
      }
      //printf("%s\n",odtProgramm);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        seitenhoehe[i] = Suchergebnis[i];
        i++;
      }
      SeitenHoehe = atof(seitenhoehe);
      //printf("%s\n",seitenhoehe);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        seitenbreite[i] = Suchergebnis[i];
        i++;
      }
      SeitenBreite = atof(seitenbreite);
      //printf("%s\n",seitenbreite);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        seitenrandlinks[i] = Suchergebnis[i];
        i++;
      }
      SeitenRandLinks = atof(seitenrandlinks);
      //printf("%s\n",seitenrandlinks);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        seitenrandrechts[i] = Suchergebnis[i];
        i++;
      }
      SeitenRandRechts = atof(seitenrandrechts);
      //printf("%s\n",seitenrandrechts);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        seitenrandoben[i] = Suchergebnis[i];
        i++;
      }
      SeitenRandOben = atof(seitenrandoben);
      //printf("%s\n",seitenrandoben);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        seitenrandunten[i] = Suchergebnis[i];
        i++;
      }
      SeitenRandUnten = atof(seitenrandunten);
      //printf("%s\n",seitenrandunten);

      Suchergebnis = strtok(NULL,"=");
      gruppierung[0] = Suchergebnis[0];
      Gruppierung = atoi(gruppierung);
      //printf("%s\n",gruppierung);

      Suchergebnis = strtok(NULL,"=");
      optimierung[0] = Suchergebnis[0];
      Optimierung = atoi(optimierung);
      //printf("%s\n",optimierung);

      Suchergebnis = strtok(NULL,"=");
      urnenull[0] = Suchergebnis[0];
      UrneNull = atoi(urnenull);
      //printf("%s\n",urnenull);

      Suchergebnis = strtok(NULL,"=");
      urnekuerzen[0] = Suchergebnis[0];
      UrneKuerzen = atoi(urnekuerzen);
      //printf("%s\n",urnekuerzen);

      Suchergebnis = strtok(NULL,"=");
      ergebnisspalteanzeigen[0] = Suchergebnis[0];
      ErgebnisSpalteAnzeigen = atoi(ergebnisspalteanzeigen);
      //printf("%s\n",ergebnisspalteanzeigen);

      Suchergebnis = strtok(NULL,"=");
      ergebnisspaltewskanzeigen[0] = Suchergebnis[0];
      ErgebnisSpalteWskAnzeigen = atoi(ergebnisspaltewskanzeigen);
      //printf("%s\n",ergebnisspaltewskanzeigen);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && i < 6)
      {
        genauigkeit[i] = Suchergebnis[i];
        i++;
      }
      Genauigkeit = atoi(genauigkeit);
      //printf("%s\n",genauigkeit);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        horizontalerknotenabstand[i] = Suchergebnis[i];
        i++;
      }
      HorizontalerKnotenAbstand = atof(horizontalerknotenabstand);
      //printf("%s\n",horizontalerknotenabstand);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        Zweig[i] = Suchergebnis[i];
        i++;
      }
      Zweig[i] = 0;
      //printf("%s\n",Zweig);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        zweigstrichelungstrichanzahl1[i] = Suchergebnis[i];
        i++;
      }
      ZweigStrichelungStrichAnzahl1 = atoi(zweigstrichelungstrichanzahl1);
      //printf("%s\n",zweigstrichelungstrichanzahl1);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        zweigstrichelungstrichlaenge1[i] = Suchergebnis[i];
        i++;
      }
      ZweigStrichelungStrichLaenge1 = atof(zweigstrichelungstrichlaenge1);
      //printf("%s\n",zweigstrichelungstrichlaenge1);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        zweigstrichelungstrichanzahl2[i] = Suchergebnis[i];
        i++;
      }
      ZweigStrichelungStrichAnzahl2 = atoi(zweigstrichelungstrichanzahl2);
      //printf("%s\n",zweigstrichelungstrichanzahl2);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        zweigstrichelungstrichlaenge2[i] = Suchergebnis[i];
        i++;
      }
      ZweigStrichelungStrichLaenge2 = atof(zweigstrichelungstrichlaenge2);
      //printf("%s\n",zweigstrichelungstrichlaenge2);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        zweigstrichelungstrichabstand[i] = Suchergebnis[i];
        i++;
      }
      ZweigStrichelungAbstand = atof(zweigstrichelungstrichabstand);
      //printf("%s\n",zweigstrichelungstrichabstand);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        zweigdicke[i] = Suchergebnis[i];
        i++;
      }
      ZweigDicke = atof(zweigdicke);
      //printf("%s\n%g\n",zweigdicke,ZweigDicke);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        ZweigFarbe[i] = Suchergebnis[i];
        i++;
      }
      ZweigFarbe[i] = 0;
      //printf("%s\n",ZweigFarbe);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 26)
      {
        ZweigAnfangForm[i] = Suchergebnis[i];
        i++;
      }
      ZweigAnfangForm[i] = 0;
      //printf("%s\n",ZweigAnfangForm);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        zweiganfangdicke[i] = Suchergebnis[i];
        i++;
      }
      ZweigAnfangDicke = atof(zweiganfangdicke);
      //printf("%s\n%g\n",zweiganfangdicke,ZweigAnfangDicke);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 26)
      {
        ZweigEndeForm[i] = Suchergebnis[i];
        i++;
      }
      ZweigEndeForm[i] = 0;
      //printf("%s\n",ZweigEndeForm);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        zweigendedicke[i] = Suchergebnis[i];
        i++;
      }
      ZweigEndeDicke = atof(zweigendedicke);
      //printf("%s\n%g\n",zweigendedicke,ZweigEndeDicke);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        zweigundurchsichtigkeit[i] = Suchergebnis[i];
        i++;
      }
      ZweigUndurchsichtigkeit = atoi(zweigundurchsichtigkeit);
      //printf("%s\n",zweigundurchsichtigkeit);

      Suchergebnis = strtok(NULL,"=");
      bruchzweige[0] = Suchergebnis[0];
      BruchZweige = atoi(bruchzweige);
      //printf("%s\n%i\n",bruchzweige,BruchZweige);

      Suchergebnis = strtok(NULL,"=");
      bruchzweigeformel[0] = Suchergebnis[0];
      BruchZweigeFormel = atoi(bruchzweigeformel);
      //printf("%s\n%i\n",bruchzweigeformel,BruchZweigeFormel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        leerzeichenanzahl[i] = Suchergebnis[i];
        i++;
      }
      LeerzeichenAnzahl = atoi(leerzeichenanzahl);
      //printf("%s\n",leerzeichenanzahl);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        WahrscheinlichkeitsPositionAufZweig[i] = Suchergebnis[i];
        i++;
      }
      WahrscheinlichkeitsPositionAufZweig[i] = 0;
      //printf("%s\n",WahrscheinlichkeitsPositionAufZweig);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        verschiebungwsknachunten[i] = Suchergebnis[i];
        i++;
      }
      VerschiebungWskNachUnten = atof(verschiebungwsknachunten);
      //printf("%s\n%g\n",verschiebungwsknachunten,VerschiebungWskNachUnten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        verschiebungwsknachoben[i] = Suchergebnis[i];
        i++;
      }
      VerschiebungWskNachOben = atof(verschiebungwsknachoben);
      //printf("%s\n%g\n",verschiebungwsknachoben,VerschiebungWskNachOben);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        verschiebungwsknachrechts[i] = Suchergebnis[i];
        i++;
      }
      VerschiebungWskNachRechts = atof(verschiebungwsknachrechts);
      //printf("%s\n%g\n",verschiebungwsknachrechts,VerschiebungWskNachRechts);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        verschiebungwsknachlinks[i] = Suchergebnis[i];
        i++;
      }
      VerschiebungWskNachLinks = atof(verschiebungwsknachlinks);
      //printf("%s\n%g\n",verschiebungwsknachlinks,VerschiebungWskNachLinks);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 4)
      {
        FettWsk[i] = Suchergebnis[i];
        i++;
      }
      FettWsk[i] = 0;
      //printf("%s\n",FettWsk);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        KursivWsk[i] = Suchergebnis[i];
        i++;
      }
      KursivWsk[i] = 0;
      //printf("%s\n",KursivWsk);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        SchriftfarbeWsk[i] = Suchergebnis[i];
        i++;
      }
      SchriftfarbeWsk[i] = 0;
      //printf("%s\n",SchriftfarbeWsk);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 39)
      {
        SchriftartWsk[i] = Suchergebnis[i];
        i++;
      }
      SchriftartWsk[i] = 0;
      //printf("%s\n",SchriftartWsk);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        schriftgroessewsk[i] = Suchergebnis[i];
        i++;
      }
      SchriftgroesseWsk = atoi(schriftgroessewsk);
      //printf("%s\n",schriftgroessewsk);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        knotenbreite[i] = Suchergebnis[i];
        i++;
      }
      KnotenBreite = atof(knotenbreite);
      //printf("knotenbreite: %s\n%g\n",knotenbreite,KnotenBreite);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        knotenhoehe[i] = Suchergebnis[i];
        i++;
      }
      KnotenHoehe = atof(knotenhoehe);
      //printf("knotenhoehe: %s\n%g\n",knotenhoehe,KnotenHoehe);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        vertikalerknotenabstand[i] = Suchergebnis[i];
        i++;
      }
      VertikalerKnotenAbstand = atof(vertikalerknotenabstand);
      //printf("vertikalerknotenabstand: %s\n%g\n",vertikalerknotenabstand,VertikalerKnotenAbstand);

      Suchergebnis = strtok(NULL,"=");
      knotenform[0] = Suchergebnis[0];
      KnotenForm = atoi(knotenform);
      //printf("knotenform: %s\n%i\n",knotenform,KnotenForm);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 5)
      {
        RahmenKnoten[i] = Suchergebnis[i];
        i++;
      }
      RahmenKnoten[i] = 0;
      //printf("%s\n",RahmenKnoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenknotenstrichelungstrichanzahl1[i] = Suchergebnis[i];
        i++;
      }
      RahmenKnotenStrichelungStrichAnzahl1 = atoi(rahmenknotenstrichelungstrichanzahl1);
      //printf("%s\n",rahmenknotenstrichelungstrichanzahl1);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenknotenstrichelungstrichlaenge1[i] = Suchergebnis[i];
        i++;
      }
      RahmenKnotenStrichelungStrichLaenge1 = atof(rahmenknotenstrichelungstrichlaenge1);
      //printf("%s\n",rahmenknotenstrichelungstrichlaenge1);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenknotenstrichelungstrichanzahl2[i] = Suchergebnis[i];
        i++;
      }
      RahmenKnotenStrichelungStrichAnzahl2 = atoi(rahmenknotenstrichelungstrichanzahl2);
      //printf("%s\n",rahmenknotenstrichelungstrichanzahl2);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenknotenstrichelungstrichlaenge2[i] = Suchergebnis[i];
        i++;
      }
      RahmenKnotenStrichelungStrichLaenge2 = atof(rahmenknotenstrichelungstrichlaenge2);
      //printf("%s\n",rahmenknotenstrichelungstrichlaenge2);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenknotenstrichelungabstand[i] = Suchergebnis[i];
        i++;
      }
      RahmenKnotenStrichelungAbstand = atof(rahmenknotenstrichelungabstand);
      //printf("%s\n",rahmenknotenstrichelungabstand);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenknotendicke[i] = Suchergebnis[i];
        i++;
      }
      RahmenKnotenDicke = atof(rahmenknotendicke);
      //printf("%s\n",rahmenknotendicke);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        RahmenKnotenFarbe[i] = Suchergebnis[i];
        i++;
      }
      RahmenKnotenFarbe[i] = 0;
      //printf("%s\n",RahmenKnotenFarbe);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenknotenundurchsichtigkeit[i] = Suchergebnis[i];
        i++;
      }
      RahmenKnotenUndurchsichtigkeit = atoi(rahmenknotenundurchsichtigkeit);
      //printf("%s\n",rahmenknotenundurchsichtigkeit);

      Suchergebnis = strtok(NULL,"=");
      FuellungKnoten = Suchergebnis[0]-48;
      //printf("FuellungKnoten:%d\n",FuellungKnoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        FuellfarbeKnoten[i] = Suchergebnis[i];
        i++;
      }
      FuellfarbeKnoten[i] = 0;
      //printf("%s\n",FuellfarbeKnoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellundurchsichtigkeitknoten[i] = Suchergebnis[i];
        i++;
      }
      FuellundurchsichtigkeitKnoten = atoi(fuellundurchsichtigkeitknoten);
      //printf("%s\n",fuellundurchsichtigkeitknoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 11)
      {
        FuellungFarbverlaufArtKnoten[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufArtKnoten[i] = 0;
      //printf("%s\n",FuellungFarbverlaufArtKnoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungfarbverlaufzentrumxknoten[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufZentrumXKnoten = atoi(fuellungfarbverlaufzentrumxknoten);
      //printf("%s\n",fuellungfarbverlaufzentrumxknoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungfarbverlaufzentrumyknoten[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufZentrumYKnoten = atoi(fuellungfarbverlaufzentrumyknoten);
      //printf("%s\n",fuellungfarbverlaufzentrumyknoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        FuellungFarbverlaufAnfangsfarbeKnoten[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufAnfangsfarbeKnoten[i] = 0;
      //printf("%s\n",FuellungFarbverlaufAnfangsfarbeKnoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        FuellungFarbverlaufEndfarbeKnoten[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufEndfarbeKnoten[i] = 0;
      //printf("%s\n",FuellungFarbverlaufEndfarbeKnoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungfarbverlaufanfangsintensitaetknoten[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufAnfangsintensitaetKnoten = atoi(fuellungfarbverlaufanfangsintensitaetknoten);
      //printf("%s\n",fuellungfarbverlaufanfangsintensitaetknoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungfarbverlaufendintensitaetknoten[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufEndintensitaetKnoten= atoi(fuellungfarbverlaufendintensitaetknoten);
      //printf("%s\n",fuellungfarbverlaufendintensitaetknoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungfarbverlaufwinkelknoten[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufWinkelKnoten = atoi(fuellungfarbverlaufwinkelknoten);
      //printf("%s\n",fuellungfarbverlaufwinkelknoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungfarbverlaufrandknoten[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufRandKnoten = atoi(fuellungfarbverlaufrandknoten);
      //printf("%s\n",fuellungfarbverlaufrandknoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 11)
      {
        FuellungTransparenzverlaufArtKnoten[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufArtKnoten[i] = 0;
      //printf("%s\n",FuellungTransparenzverlaufArtKnoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungtransparenzverlaufzentrumxknoten[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufZentrumXKnoten = atoi(fuellungtransparenzverlaufzentrumxknoten);
      //printf("%s\n",fuellungtransparenzverlaufzentrumxknoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungtransparenzverlaufzentrumyknoten[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufZentrumYKnoten = atoi(fuellungtransparenzverlaufzentrumyknoten);
      //printf("%s\n",fuellungtransparenzverlaufzentrumyknoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungtransparenzverlaufanfangknoten[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufAnfangKnoten = atoi(fuellungtransparenzverlaufanfangknoten);
      //printf("%s\n",fuellungtransparenzverlaufanfangknoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungtransparenzverlaufendknoten[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufEndKnoten = atoi(fuellungtransparenzverlaufendknoten);
      //printf("%s\n",fuellungtransparenzverlaufendknoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungtransparenzverlaufwinkelknoten[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufWinkelKnoten = atoi(fuellungtransparenzverlaufwinkelknoten);
      //printf("%s\n",fuellungtransparenzverlaufwinkelknoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungtransparenzverlaufrandknoten[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufRandKnoten = atoi(fuellungtransparenzverlaufrandknoten);
      //printf("%s\n",fuellungtransparenzverlaufrandknoten);

      Suchergebnis = strtok(NULL,"=");
      fuellungschraffurartknoten[0] = Suchergebnis[0];
      FuellungSchraffurArtKnoten = atoi(fuellungschraffurartknoten);
      //printf("%s\n%i\n",fuellungschraffurartknoten,FuellungSchraffurArtKnoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        FuellungSchraffurFarbeKnoten[i] = Suchergebnis[i];
        i++;
      }
      FuellungSchraffurFarbeKnoten[i] = 0;
      //printf("%s\n",FuellungSchraffurFarbeKnoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungschraffurabstandknoten[i] = Suchergebnis[i];
        i++;
      }
      FuellungSchraffurAbstandKnoten = atof(fuellungschraffurabstandknoten);
      //printf("%s\n",fuellungschraffurabstandknoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungschraffurrotationknoten[i] = Suchergebnis[i];
        i++;
      }
      FuellungSchraffurRotationKnoten = atoi(fuellungschraffurrotationknoten);
      //printf("%s\n",fuellungschraffurrotationknoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 18)
      {
        FuellungBildKnoten[i] = Suchergebnis[i];
        i++;
      }
      FuellungBildKnoten[i] = 0;
      //printf("%s\n",FuellungBildKnoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 9)
      {
        SchattenKnoten[i] = Suchergebnis[i];
        i++;
      }
      SchattenKnoten[i] = 0;
      //printf("%s\n",SchattenKnoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        schattenverschiebungxknoten[i] = Suchergebnis[i];
        i++;
      }
      SchattenVerschiebungXKnoten = atof(schattenverschiebungxknoten);
      //printf("%s\n",schattenverschiebungxknoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        schattenverschiebungyknoten[i] = Suchergebnis[i];
        i++;
      }
      SchattenVerschiebungYKnoten = atof(schattenverschiebungyknoten);
      //printf("%s\n",schattenverschiebungyknoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        SchattenFarbeKnoten[i] = Suchergebnis[i];
        i++;
      }
      SchattenFarbeKnoten[i] = 0;
      //printf("%s\n",SchattenFarbeKnoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        schattenundurchsichtigkeitknoten[i] = Suchergebnis[i];
        i++;
      }
      SchattenUndurchsichtigkeitKnoten = atoi(schattenundurchsichtigkeitknoten);
      //printf("%s\n",schattenundurchsichtigkeitknoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 4)
      {
        FettKnoten[i] = Suchergebnis[i];
        i++;
      }
      FettKnoten[i] = 0;
      //printf("%s\n",FettKnoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        KursivKnoten[i] = Suchergebnis[i];
        i++;
      }
      KursivKnoten[i] = 0;
      //printf("%s\n",KursivKnoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        SchriftfarbeKnoten[i] = Suchergebnis[i];
        i++;
      }
      SchriftfarbeKnoten[i] = 0;
      //printf("%s\n",SchriftfarbeKnoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 39)
      {
        SchriftartKnoten[i] = Suchergebnis[i];
        i++;
      }
      SchriftartKnoten[i] = 0;
      //printf("%s\n",SchriftartKnoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        schriftgroesseknoten[i] = Suchergebnis[i];
        i++;
      }
      SchriftgroesseKnoten = atoi(schriftgroesseknoten);
      //printf("%s\n",schriftgroesseknoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 5)
      {
        RahmenErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      RahmenErgebnisTitel[i] = 0;
      //printf("%s\n",RahmenErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenergebnistitelstrichelungstrichanzahl1[i] = Suchergebnis[i];
        i++;
      }
      RahmenErgebnisTitelStrichelungStrichAnzahl1 = atoi(rahmenergebnistitelstrichelungstrichanzahl1);
      //printf("%s\n",rahmenergebnistitelstrichelungstrichanzahl1);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenergebnistitelstrichelungstrichlaenge1[i] = Suchergebnis[i];
        i++;
      }
      RahmenErgebnisTitelStrichelungStrichLaenge1 = atof(rahmenergebnistitelstrichelungstrichlaenge1);
      //printf("%s\n",rahmenergebnistitelstrichelungstrichlaenge1);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenergebnistitelstrichelungstrichanzahl2[i] = Suchergebnis[i];
        i++;
      }
      RahmenErgebnisTitelStrichelungStrichAnzahl2 = atoi(rahmenergebnistitelstrichelungstrichanzahl2);
      //printf("%s\n",rahmenergebnistitelstrichelungstrichanzahl2);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenergebnistitelstrichelungstrichlaenge2[i] = Suchergebnis[i];
        i++;
      }
      RahmenErgebnisTitelStrichelungStrichLaenge2 = atof(rahmenergebnistitelstrichelungstrichlaenge2);
      //printf("%s\n",rahmenergebnistitelstrichelungstrichlaenge2);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenergebnistitelstrichelungabstand[i] = Suchergebnis[i];
        i++;
      }
      RahmenErgebnisTitelStrichelungAbstand = atof(rahmenergebnistitelstrichelungabstand);
      //printf("%s\n",rahmenergebnistitelstrichelungabstand);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenergebnistiteldicke[i] = Suchergebnis[i];
        i++;
      }
      RahmenErgebnisTitelDicke = atof(rahmenergebnistiteldicke);
      //printf("%s\n",rahmenergebnistiteldicke);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        RahmenErgebnisTitelFarbe[i] = Suchergebnis[i];
        i++;
      }
      RahmenErgebnisTitelFarbe[i] = 0;
      //printf("%s\n",RahmenErgebnisTitelFarbe);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenergebnistitelundurchsichtigkeit[i] = Suchergebnis[i];
        i++;
      }
      RahmenErgebnisTitelUndurchsichtigkeit = atoi(rahmenergebnistitelundurchsichtigkeit);
      //printf("%s\n",rahmenergebnistitelundurchsichtigkeit);

      Suchergebnis = strtok(NULL,"=");
      FuellungErgebnisTitel = Suchergebnis[0]-48;
      //printf("%d\n",FuellungErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        FuellfarbeErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      FuellfarbeErgebnisTitel[i] = 0;
      //printf("%s\n",FuellfarbeErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellundurchsichtigkeitergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      FuellundurchsichtigkeitErgebnisTitel = atoi(fuellundurchsichtigkeitergebnistitel);
      //printf("%s\n",fuellundurchsichtigkeitergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 11)
      {
        FuellungFarbverlaufArtErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufArtErgebnisTitel[i] = 0;
      //printf("%s\n",FuellungFarbverlaufArtErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungfarbverlaufzentrumxergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufZentrumXErgebnisTitel = atoi(fuellungfarbverlaufzentrumxergebnistitel);
      //printf("%s\n",fuellungfarbverlaufzentrumxergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungfarbverlaufzentrumyergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufZentrumYErgebnisTitel = atoi(fuellungfarbverlaufzentrumyergebnistitel);
      //printf("%s\n",fuellungfarbverlaufzentrumyergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        FuellungFarbverlaufAnfangsfarbeErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufAnfangsfarbeErgebnisTitel[i] = 0;
      //printf("%s\n",FuellungFarbverlaufAnfangsfarbeErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        FuellungFarbverlaufEndfarbeErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufEndfarbeErgebnisTitel[i] = 0;
      //printf("%s\n",FuellungFarbverlaufEndfarbeErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungfarbverlaufanfangsintensitaetergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufAnfangsintensitaetErgebnisTitel = atoi(fuellungfarbverlaufanfangsintensitaetergebnistitel);
      //printf("%s\n",fuellungfarbverlaufanfangsintensitaetergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungfarbverlaufendintensitaetergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufEndintensitaetErgebnisTitel= atoi(fuellungfarbverlaufendintensitaetergebnistitel);
      //printf("%s\n",fuellungfarbverlaufendintensitaetergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungfarbverlaufwinkelergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufWinkelErgebnisTitel = atoi(fuellungfarbverlaufwinkelergebnistitel);
      //printf("%s\n",fuellungfarbverlaufwinkelergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungfarbverlaufrandergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufRandErgebnisTitel = atoi(fuellungfarbverlaufrandergebnistitel);
      //printf("%s\n",fuellungfarbverlaufrandergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 11)
      {
        FuellungTransparenzverlaufArtErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufArtErgebnisTitel[i] = 0;
      //printf("%s\n",FuellungTransparenzverlaufArtErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungtransparenzverlaufzentrumxergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufZentrumXErgebnisTitel = atoi(fuellungtransparenzverlaufzentrumxergebnistitel);
      //printf("%s\n",fuellungtransparenzverlaufzentrumxergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungtransparenzverlaufzentrumyergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufZentrumYErgebnisTitel = atoi(fuellungtransparenzverlaufzentrumyergebnistitel);
      //printf("%s\n",fuellungtransparenzverlaufzentrumyergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungtransparenzverlaufanfangergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufAnfangErgebnisTitel = atoi(fuellungtransparenzverlaufanfangergebnistitel);
      //printf("%s\n",fuellungtransparenzverlaufanfangergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungtransparenzverlaufendergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufEndErgebnisTitel = atoi(fuellungtransparenzverlaufendergebnistitel);
      //printf("%s\n",fuellungtransparenzverlaufendergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungtransparenzverlaufwinkelergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufWinkelErgebnisTitel = atoi(fuellungtransparenzverlaufwinkelergebnistitel);
      //printf("%s\n",fuellungtransparenzverlaufwinkelergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungtransparenzverlaufrandergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufRandErgebnisTitel = atoi(fuellungtransparenzverlaufrandergebnistitel);
      //printf("%s\n",fuellungtransparenzverlaufrandergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      fuellungschraffurartergebnistitel[0] = Suchergebnis[0];
      FuellungSchraffurArtErgebnisTitel = atoi(fuellungschraffurartergebnistitel);
      //printf("%s\n%i\n",fuellungschraffurartergebnistitel,FuellungSchraffurArtErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        FuellungSchraffurFarbeErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungSchraffurFarbeErgebnisTitel[i] = 0;
      //printf("%s\n",FuellungSchraffurFarbeErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungschraffurabstandergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungSchraffurAbstandErgebnisTitel = atof(fuellungschraffurabstandergebnistitel);
      //printf("%s\n",fuellungschraffurabstandergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungschraffurrotationergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungSchraffurRotationErgebnisTitel = atoi(fuellungschraffurrotationergebnistitel);
      //printf("%s\n",fuellungschraffurrotationergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 18)
      {
        FuellungBildErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungBildErgebnisTitel[i] = 0;
      //printf("%s\n",FuellungBildErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 9)
      {
        SchattenErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      SchattenErgebnisTitel[i] = 0;
      //printf("%s\n",SchattenErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        schattenverschiebungxergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      SchattenVerschiebungXErgebnisTitel = atof(schattenverschiebungxergebnistitel);
      //printf("%s\n",schattenverschiebungxergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        schattenverschiebungyergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      SchattenVerschiebungYErgebnisTitel = atof(schattenverschiebungyergebnistitel);
      //printf("%s\n",schattenverschiebungyergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        SchattenFarbeErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      SchattenFarbeErgebnisTitel[i] = 0;
      //printf("%s\n",SchattenFarbeErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        schattenundurchsichtigkeitergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      SchattenUndurchsichtigkeitErgebnisTitel = atoi(schattenundurchsichtigkeitergebnistitel);
      //printf("%s\n",schattenundurchsichtigkeitergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 4)
      {
        FettErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      FettErgebnisTitel[i] = 0;
      //printf("%s\n",FettErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        KursivErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      KursivErgebnisTitel[i] = 0;
      //printf("%s\n",KursivErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        SchriftfarbeErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      SchriftfarbeErgebnisTitel[i] = 0;
      //printf("%s\n",SchriftfarbeErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 39)
      {
        SchriftartErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      SchriftartErgebnisTitel[i] = 0;
      //printf("%s\n",SchriftartErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        schriftgroesseergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      SchriftgroesseErgebnisTitel = atoi(schriftgroesseergebnistitel);
      //printf("%s\n",schriftgroesseergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        ergebnisbreite[i] = Suchergebnis[i];
        i++;
      }
      ErgebnisBreite = atof(ergebnisbreite);
      //printf("%s\n",ergebnisbreite);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 5)
      {
        RahmenErgebnis[i] = Suchergebnis[i];
        i++;
      }
      RahmenErgebnis[i] = 0;
      //printf("%s\n",RahmenErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenergebnisstrichelungstrichanzahl1[i] = Suchergebnis[i];
        i++;
      }
      RahmenErgebnisStrichelungStrichAnzahl1 = atoi(rahmenergebnisstrichelungstrichanzahl1);
      //printf("%s\n",rahmenergebnisstrichelungstrichanzahl1);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenergebnisstrichelungstrichlaenge1[i] = Suchergebnis[i];
        i++;
      }
      RahmenErgebnisStrichelungStrichLaenge1 = atof(rahmenergebnisstrichelungstrichlaenge1);
      //printf("%s\n",rahmenergebnisstrichelungstrichlaenge1);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenergebnisstrichelungstrichanzahl2[i] = Suchergebnis[i];
        i++;
      }
      RahmenErgebnisStrichelungStrichAnzahl2 = atoi(rahmenergebnisstrichelungstrichanzahl2);
      //printf("%s\n",rahmenergebnisstrichelungstrichanzahl2);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenergebnisstrichelungstrichlaenge2[i] = Suchergebnis[i];
        i++;
      }
      RahmenErgebnisStrichelungStrichLaenge2 = atof(rahmenergebnisstrichelungstrichlaenge2);
      //printf("%s\n",rahmenergebnisstrichelungstrichlaenge2);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenergebnisstrichelungabstand[i] = Suchergebnis[i];
        i++;
      }
      RahmenErgebnisStrichelungAbstand = atof(rahmenergebnisstrichelungabstand);
      //printf("%s\n",rahmenergebnisstrichelungabstand);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenergebnisdicke[i] = Suchergebnis[i];
        i++;
      }
      RahmenErgebnisDicke = atof(rahmenergebnisdicke);
      //printf("%s\n",rahmenergebnisdicke);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        RahmenErgebnisFarbe[i] = Suchergebnis[i];
        i++;
      }
      RahmenErgebnisFarbe[i] = 0;
      //printf("%s\n",RahmenErgebnisFarbe);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenergebnisundurchsichtigkeit[i] = Suchergebnis[i];
        i++;
      }
      RahmenErgebnisUndurchsichtigkeit = atoi(rahmenergebnisundurchsichtigkeit);
      //printf("%s\n",rahmenergebnisundurchsichtigkeit);

      Suchergebnis = strtok(NULL,"=");
      FuellungErgebnis = Suchergebnis[0]-48;
      //printf("%d\n",FuellungErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        FuellfarbeErgebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellfarbeErgebnis[i] = 0;
      //printf("%s\n",FuellfarbeErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellundurchsichtigkeitergebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellundurchsichtigkeitErgebnis = atoi(fuellundurchsichtigkeitergebnis);
      //printf("%s\n",fuellundurchsichtigkeitergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 11)
      {
        FuellungFarbverlaufArtErgebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufArtErgebnis[i] = 0;
      //printf("%s\n",FuellungFarbverlaufArtErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungfarbverlaufzentrumxergebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufZentrumXErgebnis = atoi(fuellungfarbverlaufzentrumxergebnis);
      //printf("%s\n",fuellungfarbverlaufzentrumxergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungfarbverlaufzentrumyergebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufZentrumYErgebnis = atoi(fuellungfarbverlaufzentrumyergebnis);
      //printf("%s\n",fuellungfarbverlaufzentrumyergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        FuellungFarbverlaufAnfangsfarbeErgebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufAnfangsfarbeErgebnis[i] = 0;
      //printf("%s\n",FuellungFarbverlaufAnfangsfarbeErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        FuellungFarbverlaufEndfarbeErgebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufEndfarbeErgebnis[i] = 0;
      //printf("%s\n",FuellungFarbverlaufEndfarbeErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungfarbverlaufanfangsintensitaetergebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufAnfangsintensitaetErgebnis = atoi(fuellungfarbverlaufanfangsintensitaetergebnis);
      //printf("%s\n",fuellungfarbverlaufanfangsintensitaetergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungfarbverlaufendintensitaetergebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufEndintensitaetErgebnis= atoi(fuellungfarbverlaufendintensitaetergebnis);
      //printf("%s\n",fuellungfarbverlaufendintensitaetergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungfarbverlaufwinkelergebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufWinkelErgebnis = atoi(fuellungfarbverlaufwinkelergebnis);
      //printf("%s\n",fuellungfarbverlaufwinkelergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungfarbverlaufrandergebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufRandErgebnis = atoi(fuellungfarbverlaufrandergebnis);
      //printf("%s\n",fuellungfarbverlaufrandergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 11)
      {
        FuellungTransparenzverlaufArtErgebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufArtErgebnis[i] = 0;
      //printf("%s\n",FuellungTransparenzverlaufArtErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungtransparenzverlaufzentrumxergebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufZentrumXErgebnis = atoi(fuellungtransparenzverlaufzentrumxergebnis);
      //printf("%s\n",fuellungtransparenzverlaufzentrumxergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungtransparenzverlaufzentrumyergebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufZentrumYErgebnis = atoi(fuellungtransparenzverlaufzentrumyergebnis);
      //printf("%s\n",fuellungtransparenzverlaufzentrumyergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungtransparenzverlaufanfangergebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufAnfangErgebnis = atoi(fuellungtransparenzverlaufanfangergebnis);
      //printf("%s\n",fuellungtransparenzverlaufanfangergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungtransparenzverlaufendergebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufEndErgebnis = atoi(fuellungtransparenzverlaufendergebnis);
      //printf("%s\n",fuellungtransparenzverlaufendergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungtransparenzverlaufwinkelergebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufWinkelErgebnis = atoi(fuellungtransparenzverlaufwinkelergebnis);
      //printf("%s\n",fuellungtransparenzverlaufwinkelergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungtransparenzverlaufrandergebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufRandErgebnis = atoi(fuellungtransparenzverlaufrandergebnis);
      //printf("%s\n",fuellungtransparenzverlaufrandergebnis);

      Suchergebnis = strtok(NULL,"=");
      fuellungschraffurartergebnis[0] = Suchergebnis[0];
      FuellungSchraffurArtErgebnis = atoi(fuellungschraffurartergebnis);
      //printf("%s\n%i\n",fuellungschraffurartergebnis,FuellungSchraffurArtErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        FuellungSchraffurFarbeErgebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungSchraffurFarbeErgebnis[i] = 0;
      //printf("%s\n",FuellungSchraffurFarbeErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungschraffurabstandergebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungSchraffurAbstandErgebnis = atof(fuellungschraffurabstandergebnis);
      //printf("%s\n",fuellungschraffurabstandergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungschraffurrotationergebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungSchraffurRotationErgebnis = atoi(fuellungschraffurrotationergebnis);
      //printf("%s\n",fuellungschraffurrotationergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 18)
      {
        FuellungBildErgebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungBildErgebnis[i] = 0;
      //printf("%s\n",FuellungBildErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 9)
      {
        SchattenErgebnis[i] = Suchergebnis[i];
        i++;
      }
      SchattenErgebnis[i] = 0;
      //printf("%s\n",SchattenErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        schattenverschiebungxergebnis[i] = Suchergebnis[i];
        i++;
      }
      SchattenVerschiebungXErgebnis = atof(schattenverschiebungxergebnis);
      //printf("%s\n",schattenverschiebungxergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        schattenverschiebungyergebnis[i] = Suchergebnis[i];
        i++;
      }
      SchattenVerschiebungYErgebnis = atof(schattenverschiebungyergebnis);
      //printf("%s\n",schattenverschiebungyergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        SchattenFarbeErgebnis[i] = Suchergebnis[i];
        i++;
      }
      SchattenFarbeErgebnis[i] = 0;
      //printf("%s\n",SchattenFarbeErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        schattenundurchsichtigkeitergebnis[i] = Suchergebnis[i];
        i++;
      }
      SchattenUndurchsichtigkeitErgebnis = atoi(schattenundurchsichtigkeitergebnis);
      //printf("%s\n",schattenundurchsichtigkeitergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 4)
      {
        FettErgebnis[i] = Suchergebnis[i];
        i++;
      }
      FettErgebnis[i] = 0;
      //printf("%s\n",FettErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        KursivErgebnis[i] = Suchergebnis[i];
        i++;
      }
      KursivErgebnis[i] = 0;
      //printf("%s\n",KursivErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        SchriftfarbeErgebnis[i] = Suchergebnis[i];
        i++;
      }
      SchriftfarbeErgebnis[i] = 0;
      //printf("%s\n",SchriftfarbeErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 39)
      {
        SchriftartErgebnis[i] = Suchergebnis[i];
        i++;
      }
      SchriftartErgebnis[i] = 0;
      //printf("%s\n",SchriftartErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        schriftgroesseergebnis[i] = Suchergebnis[i];
        i++;
      }
      SchriftgroesseErgebnis = atoi(schriftgroesseergebnis);
      //printf("%s\n",schriftgroesseergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 5)
      {
        RahmenWskErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      RahmenWskErgebnisTitel[i] = 0;
      //printf("%s\n",RahmenWskErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenwskergebnistitelstrichelungstrichanzahl1[i] = Suchergebnis[i];
        i++;
      }
      RahmenWskErgebnisTitelStrichelungStrichAnzahl1 = atoi(rahmenwskergebnistitelstrichelungstrichanzahl1);
      //printf("%s\n",rahmenwskergebnistitelstrichelungstrichanzahl1);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenwskergebnistitelstrichelungstrichlaenge1[i] = Suchergebnis[i];
        i++;
      }
      RahmenWskErgebnisTitelStrichelungStrichLaenge1 = atof(rahmenwskergebnistitelstrichelungstrichlaenge1);
      //printf("%s\n",rahmenwskergebnistitelstrichelungstrichlaenge1);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenwskergebnistitelstrichelungstrichanzahl2[i] = Suchergebnis[i];
        i++;
      }
      RahmenWskErgebnisTitelStrichelungStrichAnzahl2 = atoi(rahmenwskergebnistitelstrichelungstrichanzahl2);
      //printf("%s\n",rahmenwskergebnistitelstrichelungstrichanzahl2);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenwskergebnistitelstrichelungstrichlaenge2[i] = Suchergebnis[i];
        i++;
      }
      RahmenWskErgebnisTitelStrichelungStrichLaenge2 = atof(rahmenwskergebnistitelstrichelungstrichlaenge2);
      //printf("%s\n",rahmenwskergebnistitelstrichelungstrichlaenge2);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenwskergebnistitelstrichelungabstand[i] = Suchergebnis[i];
        i++;
      }
      RahmenWskErgebnisTitelStrichelungAbstand = atof(rahmenwskergebnistitelstrichelungabstand);
      //printf("%s\n",rahmenwskergebnistitelstrichelungabstand);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenwskergebnistiteldicke[i] = Suchergebnis[i];
        i++;
      }
      RahmenWskErgebnisTitelDicke = atof(rahmenwskergebnistiteldicke);
      //printf("%s\n",rahmenwskergebnistiteldicke);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        RahmenWskErgebnisTitelFarbe[i] = Suchergebnis[i];
        i++;
      }
      RahmenWskErgebnisTitelFarbe[i] = 0;
      //printf("%s\n",RahmenWskErgebnisTitelFarbe);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenwskergebnistitelundurchsichtigkeit[i] = Suchergebnis[i];
        i++;
      }
      RahmenWskErgebnisTitelUndurchsichtigkeit = atoi(rahmenwskergebnistitelundurchsichtigkeit);
      //printf("%s\n",rahmenwskergebnistitelundurchsichtigkeit);

      Suchergebnis = strtok(NULL,"=");
      FuellungWskErgebnisTitel = Suchergebnis[0]-48;
      //printf("%d\n",FuellungWskErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        FuellfarbeWskErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      FuellfarbeWskErgebnisTitel[i] = 0;
      //printf("%s\n",FuellfarbeWskErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellundurchsichtigkeitwskergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      FuellundurchsichtigkeitWskErgebnisTitel = atoi(fuellundurchsichtigkeitwskergebnistitel);
      //printf("%s\n",fuellundurchsichtigkeitwskergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 11)
      {
        FuellungFarbverlaufArtWskErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufArtWskErgebnisTitel[i] = 0;
      //printf("%s\n",FuellungFarbverlaufArtWskErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungfarbverlaufzentrumxwskergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufZentrumXWskErgebnisTitel = atoi(fuellungfarbverlaufzentrumxwskergebnistitel);
      //printf("%s\n",fuellungfarbverlaufzentrumxwskergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungfarbverlaufzentrumywskergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufZentrumYWskErgebnisTitel = atoi(fuellungfarbverlaufzentrumywskergebnistitel);
      //printf("%s\n",fuellungfarbverlaufzentrumywskergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        FuellungFarbverlaufAnfangsfarbeWskErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufAnfangsfarbeWskErgebnisTitel[i] = 0;
      //printf("%s\n",FuellungFarbverlaufAnfangsfarbeWskErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        FuellungFarbverlaufEndfarbeWskErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufEndfarbeWskErgebnisTitel[i] = 0;
      //printf("%s\n",FuellungFarbverlaufEndfarbeWskErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungfarbverlaufanfangsintensitaetwskergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufAnfangsintensitaetWskErgebnisTitel = atoi(fuellungfarbverlaufanfangsintensitaetwskergebnistitel);
      //printf("%s\n",fuellungfarbverlaufanfangsintensitaetwskergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungfarbverlaufendintensitaetwskergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufEndintensitaetWskErgebnisTitel= atoi(fuellungfarbverlaufendintensitaetwskergebnistitel);
      //printf("%s\n",fuellungfarbverlaufendintensitaetwskergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungfarbverlaufwinkelwskergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufWinkelWskErgebnisTitel = atoi(fuellungfarbverlaufwinkelwskergebnistitel);
      //printf("%s\n",fuellungfarbverlaufwinkelwskergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungfarbverlaufrandwskergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufRandWskErgebnisTitel = atoi(fuellungfarbverlaufrandwskergebnistitel);
      //printf("%s\n",fuellungfarbverlaufrandwskergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 11)
      {
        FuellungTransparenzverlaufArtWskErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufArtWskErgebnisTitel[i] = 0;
      //printf("%s\n",FuellungTransparenzverlaufArtWskErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungtransparenzverlaufzentrumxwskergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufZentrumXWskErgebnisTitel = atoi(fuellungtransparenzverlaufzentrumxwskergebnistitel);
      //printf("%s\n",fuellungtransparenzverlaufzentrumxwskergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungtransparenzverlaufzentrumywskergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufZentrumYWskErgebnisTitel = atoi(fuellungtransparenzverlaufzentrumywskergebnistitel);
      //printf("%s\n",fuellungtransparenzverlaufzentrumywskergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungtransparenzverlaufanfangwskergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufAnfangWskErgebnisTitel = atoi(fuellungtransparenzverlaufanfangwskergebnistitel);
      //printf("%s\n",fuellungtransparenzverlaufanfangwskergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungtransparenzverlaufendwskergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufEndWskErgebnisTitel = atoi(fuellungtransparenzverlaufendwskergebnistitel);
      //printf("%s\n",fuellungtransparenzverlaufendwskergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungtransparenzverlaufwinkelwskergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufWinkelWskErgebnisTitel = atoi(fuellungtransparenzverlaufwinkelwskergebnistitel);
      //printf("%s\n",fuellungtransparenzverlaufwinkelwskergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungtransparenzverlaufrandwskergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufRandWskErgebnisTitel = atoi(fuellungtransparenzverlaufrandwskergebnistitel);
      //printf("%s\n",fuellungtransparenzverlaufrandwskergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      fuellungschraffurartwskergebnistitel[0] = Suchergebnis[0];
      FuellungSchraffurArtWskErgebnisTitel = atoi(fuellungschraffurartwskergebnistitel);
      //printf("%s\n%i\n",fuellungschraffurartwskergebnistitel,FuellungSchraffurArtWskErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        FuellungSchraffurFarbeWskErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungSchraffurFarbeWskErgebnisTitel[i] = 0;
      //printf("%s\n",FuellungSchraffurFarbeWskErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungschraffurabstandwskergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungSchraffurAbstandWskErgebnisTitel = atof(fuellungschraffurabstandwskergebnistitel);
      //printf("%s\n",fuellungschraffurabstandwskergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungschraffurrotationwskergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungSchraffurRotationWskErgebnisTitel = atoi(fuellungschraffurrotationwskergebnistitel);
      //printf("%s\n",fuellungschraffurrotationwskergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 18)
      {
        FuellungBildWskErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      FuellungBildWskErgebnisTitel[i] = 0;
      //printf("%s\n",FuellungBildWskErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 9)
      {
        SchattenWskErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      SchattenWskErgebnisTitel[i] = 0;
      //printf("%s\n",SchattenWskErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        schattenverschiebungxwskergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      SchattenVerschiebungXWskErgebnisTitel = atof(schattenverschiebungxwskergebnistitel);
      //printf("%s\n",schattenverschiebungxwskergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        schattenverschiebungywskergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      SchattenVerschiebungYWskErgebnisTitel = atof(schattenverschiebungywskergebnistitel);
      //printf("%s\n",schattenverschiebungywskergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        SchattenFarbeWskErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      SchattenFarbeWskErgebnisTitel[i] = 0;
      //printf("%s\n",SchattenFarbeWskErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        schattenundurchsichtigkeitwskergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      SchattenUndurchsichtigkeitWskErgebnisTitel = atoi(schattenundurchsichtigkeitwskergebnistitel);
      //printf("%s\n",schattenundurchsichtigkeitwskergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 4)
      {
        FettWskErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      FettWskErgebnisTitel[i] = 0;
      //printf("%s\n",FettWskErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        KursivWskErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      KursivWskErgebnisTitel[i] = 0;
      //printf("%s\n",KursivWskErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        SchriftfarbeWskErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      SchriftfarbeWskErgebnisTitel[i] = 0;
      //printf("%s\n",SchriftfarbeWskErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 39)
      {
        SchriftartWskErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      SchriftartWskErgebnisTitel[i] = 0;
      //printf("%s\n",SchriftartWskErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        schriftgroessewskergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      SchriftgroesseWskErgebnisTitel = atoi(schriftgroessewskergebnistitel);
      //printf("%s\n",schriftgroessewskergebnistitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        wskbreite[i] = Suchergebnis[i];
        i++;
      }
      WskBreite = atof(wskbreite);
      //printf("%s\n",wskbreite);

      Suchergebnis = strtok(NULL,"=");
      bruchergebnis[0] = Suchergebnis[0];
      BruchErgebnis = atoi(bruchergebnis);
      //printf("%s\n",bruchergebnis);

      Suchergebnis = strtok(NULL,"=");
      bruchergebnisformel[0] = Suchergebnis[0];
      BruchErgebnisFormel = atoi(bruchergebnisformel);
      //printf("%s\n",bruchergebnisformel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 5)
      {
        RahmenWskErgebnis[i] = Suchergebnis[i];
        i++;
      }
      RahmenWskErgebnis[i] = 0;
      //printf("%s\n",RahmenWskErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenwskergebnisstrichelungstrichanzahl1[i] = Suchergebnis[i];
        i++;
      }
      RahmenWskErgebnisStrichelungStrichAnzahl1 = atoi(rahmenwskergebnisstrichelungstrichanzahl1);
      //printf("%s\n",rahmenwskergebnisstrichelungstrichanzahl1);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenwskergebnisstrichelungstrichlaenge1[i] = Suchergebnis[i];
        i++;
      }
      RahmenWskErgebnisStrichelungStrichLaenge1 = atof(rahmenwskergebnisstrichelungstrichlaenge1);
      //printf("%s\n",rahmenwskergebnisstrichelungstrichlaenge1);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenwskergebnisstrichelungstrichanzahl2[i] = Suchergebnis[i];
        i++;
      }
      RahmenWskErgebnisStrichelungStrichAnzahl2 = atoi(rahmenwskergebnisstrichelungstrichanzahl2);
      //printf("%s\n",rahmenwskergebnisstrichelungstrichanzahl2);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenwskergebnisstrichelungstrichlaenge2[i] = Suchergebnis[i];
        i++;
      }
      RahmenWskErgebnisStrichelungStrichLaenge2 = atof(rahmenwskergebnisstrichelungstrichlaenge2);
      //printf("%s\n",rahmenwskergebnisstrichelungstrichlaenge2);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenwskergebnisstrichelungabstand[i] = Suchergebnis[i];
        i++;
      }
      RahmenWskErgebnisStrichelungAbstand = atof(rahmenwskergebnisstrichelungabstand);
      //printf("%s\n",rahmenwskergebnisstrichelungabstand);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenwskergebnisdicke[i] = Suchergebnis[i];
        i++;
      }
      RahmenWskErgebnisDicke = atof(rahmenwskergebnisdicke);
      //printf("%s\n",rahmenwskergebnisdicke);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        RahmenWskErgebnisFarbe[i] = Suchergebnis[i];
        i++;
      }
      RahmenWskErgebnisFarbe[i] = 0;
      //printf("%s\n",RahmenWskErgebnisFarbe);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        rahmenwskergebnisundurchsichtigkeit[i] = Suchergebnis[i];
        i++;
      }
      RahmenWskErgebnisUndurchsichtigkeit = atoi(rahmenwskergebnisundurchsichtigkeit);
      //printf("%s\n",rahmenwskergebnisundurchsichtigkeit);

      Suchergebnis = strtok(NULL,"=");
      FuellungWskErgebnis = Suchergebnis[0]-48;
      //printf("%d\n",FuellungWskErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        FuellfarbeWskErgebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellfarbeWskErgebnis[i] = 0;
      //printf("%s\n",FuellfarbeWskErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellundurchsichtigkeitwskergebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellundurchsichtigkeitWskErgebnis = atoi(fuellundurchsichtigkeitwskergebnis);
      //printf("%s\n",fuellundurchsichtigkeitwskergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 11)
      {
        FuellungFarbverlaufArtWskErgebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufArtWskErgebnis[i] = 0;
      //printf("%s\n",FuellungFarbverlaufArtWskErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungfarbverlaufzentrumxwskergebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufZentrumXWskErgebnis = atoi(fuellungfarbverlaufzentrumxwskergebnis);
      //printf("%s\n",fuellungfarbverlaufzentrumxwskergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungfarbverlaufzentrumywskergebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufZentrumYWskErgebnis = atoi(fuellungfarbverlaufzentrumywskergebnis);
      //printf("%s\n",fuellungfarbverlaufzentrumywskergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        FuellungFarbverlaufAnfangsfarbeWskErgebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufAnfangsfarbeWskErgebnis[i] = 0;
      //printf("%s\n",FuellungFarbverlaufAnfangsfarbeWskErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        FuellungFarbverlaufEndfarbeWskErgebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufEndfarbeWskErgebnis[i] = 0;
      //printf("%s\n",FuellungFarbverlaufEndfarbeWskErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungfarbverlaufanfangsintensitaetwskergebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufAnfangsintensitaetWskErgebnis = atoi(fuellungfarbverlaufanfangsintensitaetwskergebnis);
      //printf("%s\n",fuellungfarbverlaufanfangsintensitaetwskergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungfarbverlaufendintensitaetwskergebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufEndintensitaetWskErgebnis= atoi(fuellungfarbverlaufendintensitaetwskergebnis);
      //printf("%s\n",fuellungfarbverlaufendintensitaetwskergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungfarbverlaufwinkelwskergebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufWinkelWskErgebnis = atoi(fuellungfarbverlaufwinkelwskergebnis);
      //printf("%s\n",fuellungfarbverlaufwinkelwskergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungfarbverlaufrandwskergebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungFarbverlaufRandWskErgebnis = atoi(fuellungfarbverlaufrandwskergebnis);
      //printf("%s\n",fuellungfarbverlaufrandwskergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 11)
      {
        FuellungTransparenzverlaufArtWskErgebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufArtWskErgebnis[i] = 0;
      //printf("%s\n",FuellungTransparenzverlaufArtWskErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungtransparenzverlaufzentrumxwskergebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufZentrumXWskErgebnis = atoi(fuellungtransparenzverlaufzentrumxwskergebnis);
      //printf("%s\n",fuellungtransparenzverlaufzentrumxwskergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungtransparenzverlaufzentrumywskergebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufZentrumYWskErgebnis = atoi(fuellungtransparenzverlaufzentrumywskergebnis);
      //printf("%s\n",fuellungtransparenzverlaufzentrumywskergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungtransparenzverlaufanfangwskergebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufAnfangWskErgebnis = atoi(fuellungtransparenzverlaufanfangwskergebnis);
      //printf("%s\n",fuellungtransparenzverlaufanfangwskergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungtransparenzverlaufendwskergebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufEndWskErgebnis = atoi(fuellungtransparenzverlaufendwskergebnis);
      //printf("%s\n",fuellungtransparenzverlaufendwskergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungtransparenzverlaufwinkelwskergebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufWinkelWskErgebnis = atoi(fuellungtransparenzverlaufwinkelwskergebnis);
      //printf("%s\n",fuellungtransparenzverlaufwinkelwskergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungtransparenzverlaufrandwskergebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungTransparenzverlaufRandWskErgebnis = atoi(fuellungtransparenzverlaufrandwskergebnis);
      //printf("%s\n",fuellungtransparenzverlaufrandwskergebnis);

      Suchergebnis = strtok(NULL,"=");
      fuellungschraffurartwskergebnis[0] = Suchergebnis[0];
      FuellungSchraffurArtWskErgebnis = atoi(fuellungschraffurartwskergebnis);
      //printf("%s\n%i\n",fuellungschraffurartwskergebnis,FuellungSchraffurArtWskErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        FuellungSchraffurFarbeWskErgebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungSchraffurFarbeWskErgebnis[i] = 0;
      //printf("%s\n",FuellungSchraffurFarbeWskErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungschraffurabstandwskergebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungSchraffurAbstandWskErgebnis = atof(fuellungschraffurabstandwskergebnis);
      //printf("%s\n",fuellungschraffurabstandwskergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        fuellungschraffurrotationwskergebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungSchraffurRotationWskErgebnis = atoi(fuellungschraffurrotationwskergebnis);
      //printf("%s\n",fuellungschraffurrotationwskergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 18)
      {
        FuellungBildWskErgebnis[i] = Suchergebnis[i];
        i++;
      }
      FuellungBildWskErgebnis[i] = 0;
      //printf("%s\n",FuellungBildWskErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 9)
      {
        SchattenWskErgebnis[i] = Suchergebnis[i];
        i++;
      }
      SchattenWskErgebnis[i] = 0;
      //printf("%s\n",SchattenWskErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        schattenverschiebungxwskergebnis[i] = Suchergebnis[i];
        i++;
      }
      SchattenVerschiebungXWskErgebnis = atof(schattenverschiebungxwskergebnis);
      //printf("%s\n",schattenverschiebungxwskergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        schattenverschiebungywskergebnis[i] = Suchergebnis[i];
        i++;
      }
      SchattenVerschiebungYWskErgebnis = atof(schattenverschiebungywskergebnis);
      //printf("%s\n",schattenverschiebungywskergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        SchattenFarbeWskErgebnis[i] = Suchergebnis[i];
        i++;
      }
      SchattenFarbeWskErgebnis[i] = 0;
      //printf("%s\n",SchattenFarbeWskErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        schattenundurchsichtigkeitwskergebnis[i] = Suchergebnis[i];
        i++;
      }
      SchattenUndurchsichtigkeitWskErgebnis = atoi(schattenundurchsichtigkeitwskergebnis);
      //printf("%s\n",schattenundurchsichtigkeitwskergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 4)
      {
        FettWskErgebnis[i] = Suchergebnis[i];
        i++;
      }
      FettWskErgebnis[i] = 0;
      //printf("%s\n",FettWskErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        KursivWskErgebnis[i] = Suchergebnis[i];
        i++;
      }
      KursivWskErgebnis[i] = 0;
      //printf("%s\n",KursivWskErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
      {
        SchriftfarbeWskErgebnis[i] = Suchergebnis[i];
        i++;
      }
      SchriftfarbeWskErgebnis[i] = 0;
      //printf("%s\n",SchriftfarbeWskErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 39)
      {
        SchriftartWskErgebnis[i] = Suchergebnis[i];
        i++;
      }
      SchriftartWskErgebnis[i] = 0;
      //printf("%s\n",SchriftartWskErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      if(Suchergebnis != NULL)
      {
        while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 6)
        {
          schriftgroessewskergebnis[i] = Suchergebnis[i];
          i++;
        }
        SchriftgroesseWskErgebnis = atoi(schriftgroessewskergebnis);
        //printf("%s\n",schriftgroessewskergebnis);
      }
    }

    if(BruchZweigeFormel && BruchZweige && strcmp(FettWsk, "bold"))
    {
      FettWsk[0] = 0;
    }

    if(BruchErgebnisFormel && BruchErgebnis && strcmp(KursivWsk,"italic"))
    {
      KursivWsk[0] = 0;
    }

    if(BruchErgebnisFormel && BruchErgebnis && strcmp(FettWskErgebnis, "bold"))
    {
      FettWskErgebnis[0] = 0;
    }

    if(BruchErgebnisFormel && BruchErgebnis && strcmp(KursivWskErgebnis,"italic"))
    {
      KursivWskErgebnis[0] = 0;
    }

    if(BruchErgebnisFormel && BruchErgebnis && strcmp(SchriftfarbeWskErgebnis,"black") && strcmp(SchriftfarbeWskErgebnis,"white") && strcmp(SchriftfarbeWskErgebnis,"yellow") && strcmp(SchriftfarbeWskErgebnis,"red") && strcmp(SchriftfarbeWskErgebnis,"blue") && strcmp(SchriftfarbeWskErgebnis,"green") && strcmp(SchriftfarbeWskErgebnis,"magenta") && strcmp(SchriftfarbeWskErgebnis,"cyan"))
    {
      strcpy(SchriftfarbeWskErgebnis, "black");
    }

    if(BruchZweigeFormel && BruchZweige && strcmp(SchriftfarbeWsk,"black") && strcmp(SchriftfarbeWsk,"white") && strcmp(SchriftfarbeWsk,"yellow") && strcmp(SchriftfarbeWsk,"red") && strcmp(SchriftfarbeWsk,"blue") && strcmp(SchriftfarbeWsk,"green") && strcmp(SchriftfarbeWsk,"magenta") && strcmp(SchriftfarbeWsk,"cyan"))
    {
      strcpy(SchriftfarbeWsk, "black");
    }

    free(Einstellungen);
  }

  if(svg)
  {
    fp = fopen(svgeinstellungsdatei,"r");
    fseek(fp,0L,SEEK_END);
    dateigroesse = ftell(fp);
    //printf("Dateigroesse = %ld\n",dateigroesse);
    rewind(fp);
    Einstellungen=calloc(dateigroesse+2,sizeof(char));
    j = 0;
    while((temp = fgetc(fp))!=EOF)
    {
      if(!temp)
      {
        break;
      }
      Einstellungen[j]=temp;
      j++;
    }
    fclose(fp);
    Einstellungen[j] = 0;
    if(zeichenzaehlen(Einstellungen, 61) > 73)
    {
      fprintf(stderr,"\n\tWarnung: Die Datei svgEinstellungen ist möglicherweise fehlerhaft.\n\tEs wurden mehr =-Zeichen gefunden als erwartet.\n\n");
    }
    if(zeichenzaehlen(Einstellungen, 61) < 73 || strstr(Einstellungen,"==") != NULL)    // es gibt 69 =-Zeichen (ASCII 61) in der Einstellungsdatei. Wenn es weniger sind, ist die Datei unvollständg, und wird neu erstellt.
    {
      fprintf(stderr,"\n\tWarnung: Die Datei svgEinstellungen ist fehlerhaft!\n\tSie wird nun neu erstellt.\n\tVon der bisherigen Datei wird eine Sicherungskopie angelegt.\n\n");
      char sicherungsdatei[4097];
      strcpy(sicherungsdatei,svgeinstellungsdatei);
      strcat(sicherungsdatei,".bak");
      fp = fopen(sicherungsdatei,"r");
      i=0;
      while(fp != NULL)
      {
        //printf("Sicherungsdatei existiert schon.\n");
        fclose(fp);
        i++;
        sprintf(sicherungsdatei,"%s%i.bak",svgeinstellungsdatei,i);
        //printf("%s\n",sicherungsdatei);
        fp = fopen(sicherungsdatei,"r");
      }
      rename(svgeinstellungsdatei,sicherungsdatei);     // Die alten Einstellungen werden gesichert.
      fp = fopen(svgeinstellungsdatei,"w");
      fprintf(fp, "[Allgemeine Einstellungen]\n\nDateipfad=0\t\t\tPfad wo die Baumdiagrammdateien gespeichert werden sollen. Standardwert (0) ist das eigene Benutzerverzeichnis.\nProgramm=0\t\t\tProgramm inklusive Pfad, mit dem die Baumdiagramme (als svg-Dateien) geöffnet werden sollen. Recht gut funktioniert zum Beispiel Chromium bzw. Chrome oder Mozilla Firefox. 0 bedeutet nichts öffnen.\nSeitenHoehe=0\t\t\tEinheit cm. DIN A4 Format ist 29.7 mal 21.0 cm DIN A3 42.0 mal 29.7 cm. 0 bedeutet automatische Größenanpassung an das Baumdiagramm.\nSeitenBreite=0\t\t\tEinheit cm. DIN A4 Format ist 29.7 mal 21.0 cm DIN A3 42.0 mal 29.7 cm. 0 bedeutet automatische Größenanpassung an das Baumdiagramm.\nSeitenRandLinks=2\t\tEinheit cm.\nSeitenRandRechts=2\t\tEinheit cm.\nSeitenRandOben=2\t\tEinheit cm.\nSeitenrandUnten=2\t\tEinheit cm.\nOptimierung=0\t\t\tMögliche Werte: 0: Die obigen beiden Werte gelten. 1: Das Baumdiagramm nutzt eine komplette DIN A4-Seite im Hochformat. 2: Wie 1 nur im Querformat. 3 oder 4: wie 1 oder 2 nur im DIN A3-Format.\nUrneNull=0\t\t\tMögliche Werte: 0, um beim Ziehen aus einer Urne ohne Zurücklegen Pfade mit Wahrscheinlichkeit 0 nicht anzuzeigen, 1 um diese anzuzeigen.\nUrneKuerzen=1\t\t\tMögliche Werte: 0 um die Zweig-Wahrscheinlichkeiten beim Urnenmodell nicht zu kürzen, 1 um sie zu kürzen.\nErgebnisSpalteAnzeigen=1\tMögliche Werte: 1 für anzeigen, 0 für nicht anzeigen.\nErgebnisSpalteWskAnzeigen=1\tMögliche Werte: 1 für anzeigen, 0 für nicht anzeigen. Diese wird natürlich nur angezeigt, wenn Wahrscheinlichkeiten angegeben werden.\nGenauigkeit=-1\t\t\tAnzahl der Nachkommastellen der Wahrscheinlichkeiten. -1 bedeutet so genau wie möglich. Mit -1 werden zum Beispiel 0,1 oder 0,25 oder 0,125 dargestellt, bei 3 würde 0,100 oder 0,250 oder 0,125 angezeigt werden. Nur relevant, wenn nicht \"Bruch\" eingestellt ist.\n\n\n[Eigenschaften der Zweige des Baumdiagramms]\n\nHorizontalerKnotenAbstand=3\t\tSo weit in cm sollen die einzelnen \"Spalten\" des Baumdiagramms auseinander liegen: Entspricht der \"Breite\" (Länge in x-Richtung) der Zweige.\nZweigFarbe=000000\t\t\tAnzeigefarbe: none (Zweige sind unsichtbar) oder Farbe in hexadezimaler Schreibweise, oder in svg-Farbschlüsselwörtern (black, maroon, green, olive, navy, purple, teal, silver, gray, red, lime, yellow, blue, fuchsia, aqua, white). Bei der hexadezimalen Schreibweise stehen die erste zwei Stellen für den Rotanteil, die zweiten zwei für den Grünanteil und die letzten für den Blauanteil. 000000 z.B. ist schwarz, 0000FF blau, 00FF00 grün und FF0000 rot. 808080 wäre ein mittlerer Grau-Ton. Etc.\nZweigStrichelung=none\t\t\tEine gerade Anzahl an mit Komma getrennten Zahlen mit folgender Bedeutung: Länge in Pixel des ersten Strichs, Abstand in Pixel zum nächsten Strich, Länge in Pixel des zweiten Strichs, etc. Es können beliebig viele Werte gesetzt werden.\nZweigDicke=1\t\t\t\tDicke in px (Pixel).\nZweigUndurchsichtigkeit=100\t\tWerte von 0 bis 100 in Prozent. 0 bedeutet völlig transparent (durchsichtig), 100 völlig opak (undurchsichtig).\n\n\n[Eigenschaften der Zweigwahrscheinlichkeiten]\n\nBruchZweige=0\t\t\t\tWie oben: 0, Dezimalzahlen auf den Zweigen, 1, Brüche auf den Zweigen.\nBruchDarstellungZweig=1\t\t\t0 bedeutet Brüche werden mit Schrägstrich von links nach rechts geschrieben, z.B. 5/7. 1 bedeutet die normale Schreibweise, Zähler über Nenner.\nBruchDrehung=1\t\t\t\tWird nur beachtet, wenn BruchdarstellungZweig gleich 1 ist. 0 bedeutet, die Bruchstriche verlaufen parallel zu den Zweigen, 1 heißt, die Bruchstriche verlaufen horizontal (parallel zum Rand).\nWskVerschiebung=0.5\t\t\tVerschiebung der Wahrscheinlichkeit nach rechts. Negative Werte sorgen für eine Verschiebung nach links.\nZweigWskAbstand=5\t\t\tAbstand der Wahrscheinlichkeiten vom Zweig in Pixel (px).\nSchriftartWsk=Times New Roman\t\tSchriftart einstellen. Exakte Schreibweise ist notwendig. Wird von den meisten Anzeigeprogrammen leider nicht unterstützt.\nSchriftgroesseWsk=12\t\t\tSchriftgröße der Zweigwahrscheinlichkeiten in pt.\n\n\n[Eigenschaften der Knotenpunkte des Baumdiagramms]\n\nKnotenBreite=0\t\t\t\t\tSo breit in cm sollen die Knotenpunkte sein. 0 passt die Breite automatisch an die Schriftgröße an.\nKnotenHoehe=0\t\t\t\t\tSo hoch in cm sollen die Knotenpunkte sein. 0 passt die Höhe automatisch an die Schriftgröße an.\nVertikalerKnotenAbstand=2\t\t\tSo viele cm sollen die letzten (am weitesten rechts liegenden) Knotenpunkte auseinander liegen.\nRahmenKnotenFarbe=none\t\t\t\tZeichnet einen Rahmen um die Knotenpunkte. Mögliche Werte: none (kein Rahmen) oder hexadezimale Farbe, z.B. ff0000 für rot, oder die Farbwörter black, maroon, green, olive, navy, purple, teal, silver, gray, red, lime, yellow, blue, fuchsia, aqua, white.\nRahmenKnotenStrichelung=7,3,5,4\t\t\tEine gerade Anzahl an mit Komma getrennten Zahlen mit folgender Bedeutung: Länge in Pixel des ersten Strichs, Abstand in Pixel zum nächsten Strich, Länge in Pixel des zweiten Strichs, etc. Es können beliebig viele Werte gesetzt werden.\nRahmenKnotenDicke=1\t\t\t\tDicke in px (Pixel).\nRahmenKnotenUndurchsichtigkeit=100\t\tMögliche Werte: 0 bis 100 in Prozent. 0 ist völlig transparent, 100 völlig opak.\nFuellfarbeKnoten=none\t\t\t\tHexadezimale Farbe für Knotenpunkte.\nFuellundurchsichtigkeitKnoten=100\t\tIn Prozent. s.o. 0 ist völlig transparent, 100 völlig undurchsichtig.\nSchriftartKnoten=Times New Roman\t\tSchriftart einstellen. Exakte Schreibweise ist notwendig.\nSchriftgroesseKnoten=12\t\t\t\tSchriftgröße der Ergebnisspaltenüberschrift in pt.\n\n\n[Eigenschaften der Ergebnisspaltenüberschrift des Baumdiagramms]\n\nRahmenErgebnisTitelFarbe=none\t\t\t\t\tZeichnet einen Rahmen um die Überschrift der Ergebnisspalte. Mögliche Werte: none (kein Rahmen) oder hexadezimale Farbe, z.B. ff0000 für rot, oder die Farbwörter black, maroon, green, olive, navy, purple, teal, silver, gray, red, lime, yellow, blue, fuchsia, aqua, white.\nRahmenErgebnisTitelStrichelung=1,2,3,4\t\t\t\tEine gerade Anzahl an mit Komma getrennten Zahlen mit folgender Bedeutung: Länge in Pixel des ersten Strichs, Abstand in Pixel zum nächsten Strich, Länge in Pixel des zweiten Strichs, etc. Es können beliebig viele Werte gesetzt werden.\nRahmenErgebnisTitelDicke=0.0\t\t\t\t\tDicke in px (Pixel).\nRahmenErgebnisTitelUndurchsichtigkeit=100\t\t\tMögliche Werte: 0 bis 100 in Prozent. 0 ist völlig transparent, 100 völlig opak.\nFuellfarbeErgebnisTitel=none\t\t\t\t\tHexadezimale Farbe für Ergebnisspaltenüberschrift\nFuellundurchsichtigkeitErgebnisTitel=100\t\t\ts.o. 0 ist völlig transparent, 100 völlig undurchsichtig.\nSchriftartErgebnisTitel=Times New Roman\t\t\t\tSchriftart einstellen. Exakte Schreibweise ist notwendig.\nSchriftgroesseErgebnisTitel=12\t\t\t\t\tSchriftgröße der Ergebnisspaltenüberschrift in pt.\n\n\n[Eigenschaften der Ergebnisspalte des Baumdiagramms]\n\nErgebnisBreite=0\t\t\t\t\tBreite der Ergebnisspalte in cm. 0 bedeutet, dass diese automatisch angepasst wird.\nRahmenErgebnisFarbe=none\t\t\t\tZeichnet einen Rahmen um die Pfadergebnisse. Mögliche Werte: none (kein Rahmen) oder hexadezimale Farbe, z.B. ff0000 für rot, oder die Farbwörter black, maroon, green, olive, navy, purple, teal, silver, gray, red, lime, yellow, blue, fuchsia, aqua, white.\nRahmenErgebnisStrichelung=5,5,6,6,7,7\t\t\tEine gerade Anzahl an mit Komma getrennten Zahlen mit folgender Bedeutung: Länge in Pixel des ersten Strichs, Abstand in Pixel zum nächsten Strich, Länge in Pixel des zweiten Strichs, etc. Es können beliebig viele Werte gesetzt werden.\nRahmenErgebnisDicke=1\t\t\t\t\tDicke in px (Pixel).\nRahmenErgebnisUndurchsichtigkeit=100\t\t\tMögliche Werte: 0 bis 100 in Prozent. 0 ist völlig transparent, 100 völlig opak.\nFuellfarbeErgebnis=none\t\t\t\t\tHexadezimale Farbe für Ergebnisspalteneinträge.\nFuellundurchsichtigkeitErgebnis=100\t\t\ts.o. 0 ist völlig transparent, 100 völlig undurchsichtig.\nSchriftartErgebnis=Times New Roman\t\t\tSchriftart einstellen. Exakte Schreibweise ist notwendig.\nSchriftgroesseErgebnis=12\t\t\t\tSchriftgröße der Ergebnisspalte in pt.\n\n\n[Eigenschaften der Überschrift der Wahrscheinlichkeits-Ergebnisspalte]\n\nRahmenWskErgebnisTitelFarbe=none\t\t\tZeichnet einen Rahmen um die Überschrift der Pfadwahrscheinlichkeiten-Spalte. Mögliche Werte: none (kein Rahmen) oder hexadezimale Farbe, z.B. ff0000 für rot, oder die Farbwörter black, maroon, green, olive, navy, purple, teal, silver, gray, red, lime, yellow, blue, fuchsia, aqua, white.\nRahmenWskErgebnisTitelStrichelung=1,4,5,3\t\tEine gerade Anzahl an mit Komma getrennten Zahlen mit folgender Bedeutung: Länge in Pixel des ersten Strichs, Abstand in Pixel zum nächsten Strich, Länge in Pixel des zweiten Strichs, etc. Es können beliebig viele Werte gesetzt werden.\nRahmenWskErgebnisTitelDicke=0.0\t\t\t\tDicke in Pixel.\nRahmenWskErgebnisTitelUndurchsichtigkeit=100\t\tMögliche Werte: 0 bis 100 in Prozent. 0 ist völlig transparent, 100 völlig opak.\nFuellfarbeWskErgebnisTitel=none\t\t\t\tHexadezimale Farbe für Wahrscheinlichkeits-Ergebnisspaltenüberschrift.\nFuellundurchsichtigkeitWskErgebnisTitel=100\t\ts.o. 0 ist völlig transparent, 100 völlig undurchsichtig.\nSchriftartWskErgebnisTitel=Times New Roman\t\tSchriftart einstellen. Exakte Schreibweise ist notwendig.\nSchriftgroesseWskErgebnisTitel=12\t\t\tSchriftgröße der Wahrscheinlichkeits-Ergebnisspaltenüberschrift in pt.\n\n\n[Eigenschaften der Wahrscheinlichkeits-Ergebnisspalte]\n\nWskBreite=1.5\t\t\t\t\t\tBreite der Wahrscheinlichkeitsspalte ganz rechts in cm. Für die meisten Baumdiagramme ist der voreingestellte Wert passend.\nBruchErgebnis=0\t\t\t\t\t\tMögliche Werte: 0: Wahrscheinlichkeiten in der Ergebnisspalte werden als Dezimalzahlen angegeben. 1: oder als Bruch\nBruchDarstellungErgebnis=1\t\t\t\t0 bedeutet Brüche werden mit Schrägstrich von links nach rechts geschrieben, z.B. 5/7. 1 bedeutet die normale Schreibweise, Zähler über Nenner.\n\nRahmenWskErgebnisFarbe=none\t\t\t\tZeichnet einen Rahmen um die Pfadwahrscheinlichkeiten. Mögliche Werte: none (kein Rahmen) oder hexadezimale Farbe, z.B. ff0000 für rot, oder die Farbwörter black, maroon, green, olive, navy, purple, teal, silver, gray, red, lime, yellow, blue, fuchsia, aqua, white.\nRahmenWskErgebnisStrichelung=7,5,8,9\t\t\tEine gerade Anzahl an mit Komma getrennten Zahlen mit folgender Bedeutung: Länge in Pixel des ersten Strichs, Abstand in Pixel zum nächsten Strich, Länge in Pixel des zweiten Strichs, etc. Es können beliebig viele Werte gesetzt werden.\nRahmenWskErgebnisDicke=0.0\t\t\t\tDicke in px (Pixel).\nRahmenWskErgebnisUndurchsichtigkeit=100\t\t\tMögliche Werte: 0 bis 100 in Prozent. 0 ist völlig transparent, 100 völlig opak.\nFuellfarbeWskErgebnis=none\t\t\t\tHexadezimale Farbe für Ergebnisspaltenüberschrift\nFuellundurchsichtigkeitWskErgebnis=100\t\t\ts.o. 0 ist völlig transparent, 100 völlig undurchsichtig.\nSchriftartWskErgebnis=Times New Roman\t\t\tSchriftart einstellen. Exakte Schreibweise ist notwendig.\nSchriftgroesseWskErgebnis=12\t\t\t\tSchriftgröße der Wahrscheinlichkeits-Ergebnisspalte in pt.\n");
      fclose(fp);
      svgZweigStrichelung = calloc(5,sizeof(char));
      strcpy(svgZweigStrichelung, "none");
      svgRahmenKnotenStrichelung = calloc(5,sizeof(char));
      strcpy(svgRahmenKnotenStrichelung, "none");
      svgRahmenErgebnisTitelStrichelung = calloc(5,sizeof(char));
      strcpy(svgRahmenErgebnisTitelStrichelung, "none");
      svgRahmenErgebnisStrichelung = calloc(5,sizeof(char));
      strcpy(svgRahmenErgebnisStrichelung, "none");
      svgRahmenWskErgebnisTitelStrichelung = calloc(5,sizeof(char));
      strcpy(svgRahmenWskErgebnisTitelStrichelung, "none");
      svgRahmenWskErgebnisStrichelung = calloc(5,sizeof(char));
      strcpy(svgRahmenWskErgebnisStrichelung, "none");
    }
    else
    {
      char svgseitenhoehe[10]="",svgseitenbreite[10]="",svgseitenrandlinks[10]="",svgseitenrandrechts[10]="",svgseitenrandoben[10]="",svgseitenrandunten[10]="",svggenauigkeit[3]="",svghorizontalerknotenabstand[10]="",svgzweigdicke[10]="",svgzweigundurchsichtigkeit[4]="",svgwskverschiebung[10]="",svgzweigwskabstand[10]="",svgschriftgroessewsk[10]="",svgknotenbreite[10]="",svgknotenhoehe[10]="",svgvertikalerknotenabstand[10]="",svgrahmenknotendicke[10]="",svgrahmenknotenundurchsichtigkeit[4]="",svgfuellundurchsichtigkeitknoten[4]="",svgschriftgroesseknoten[10]="",svgrahmenergebnistiteldicke[10]="",svgrahmenergebnistitelundurchsichtigkeit[4]="",svgfuellundurchsichtigkeitergebnistitel[4]="",svgschriftgroesseergebnistitel[10]="",svgergebnisbreite[10]="",svgrahmenergebnisdicke[10]="",svgrahmenergebnisundurchsichtigkeit[4]="",svgfuellundurchsichtigkeitergebnis[4]="",svgschriftgroesseergebnis[10]="",svgrahmenwskergebnistiteldicke[10]="",svgrahmenwskergebnistitelundurchsichtigkeit[4]="",svgfuellundurchsichtigkeitwskergebnistitel[4]="",svgschriftgroessewskergebnistitel[10]="",svgwskbreite[10]="",svgrahmenwskergebnisdicke[10]="",svgrahmenwskergebnisundurchsichtigkeit[4]="",svgfuellundurchsichtigkeitwskergebnis[4]="",svgschriftgroessewskergebnis[10]="";
      Suchergebnis = strtok(Einstellungen,"=");

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 4096)
      {
        svgDateipfad[i] = Suchergebnis[i];
        i++;
      }
      svgDateipfad[i] = 0;
      //printf("%s\n",svgDateipfad);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 4096)
      {
        svgProgramm[i] = Suchergebnis[i];
        i++;
      }
      svgProgramm[i] = 0;
      //printf("%s\n",svgProgramm);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 9)
      {
        svgseitenhoehe[i] = Suchergebnis[i];
        i++;
      }
      svgSeitenHoehe = atof(svgseitenhoehe);
      //printf("%g\n",svgSeitenHoehe);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 9)
      {
        svgseitenbreite[i] = Suchergebnis[i];
        i++;
      }
      svgSeitenBreite = atof(svgseitenbreite);
      //printf("%g\n",svgSeitenBreite);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 9)
      {
        svgseitenrandlinks[i] = Suchergebnis[i];
        i++;
      }
      svgSeitenRandLinks = atof(svgseitenrandlinks);
      //printf("%g\n",svgSeitenRandLinks);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 9)
      {
        svgseitenrandrechts[i] = Suchergebnis[i];
        i++;
      }
      svgSeitenRandRechts = atof(svgseitenrandrechts);
      //printf("%g\n",svgSeitenRandRechts);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 9)
      {
        svgseitenrandoben[i] = Suchergebnis[i];
        i++;
      }
      svgSeitenRandOben = atof(svgseitenrandoben);
      //printf("%g\n",svgSeitenRandOben);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 9)
      {
        svgseitenrandunten[i] = Suchergebnis[i];
        i++;
      }
      svgSeitenRandUnten = atof(svgseitenrandunten);
      //printf("%g\n",svgSeitenRandUnten);

      Suchergebnis = strtok(NULL,"=");
      svgOptimierung = Suchergebnis[0] - 48;
      //printf("svgOptimierung: %d\n",svgOptimierung);

      Suchergebnis = strtok(NULL,"=");
      if(Suchergebnis[0] != 48)
      {
        svgUrneNull=1;
      }
      //printf("svgUrneNull: %d\n",svgUrneNull);

      Suchergebnis = strtok(NULL,"=");
      if(Suchergebnis[0] == 48)
      {
        svgUrneKuerzen=0;
      }
      //printf("svgUrneKuerzen: %d\n",svgUrneKuerzen);

      Suchergebnis = strtok(NULL,"=");
      if(Suchergebnis[0] == 48)
      {
        svgErgebnisSpalteAnzeigen=0;
      }
      //printf("svgErgebnisSpalteAnzeigen: %d\n",svgErgebnisSpalteAnzeigen);

      Suchergebnis = strtok(NULL,"=");
      if(Suchergebnis[0] == 48)
      {
        svgErgebnisSpalteWskAnzeigen=0;
      }
      //printf("svgErgebnisSpalteWskAnzeigen: %d\n",svgErgebnisSpalteWskAnzeigen);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 2)
      {
        svggenauigkeit[i] = Suchergebnis[i];
        i++;
      }
      svgGenauigkeit = atoi(svggenauigkeit);
      //printf("%d\n",svgGenauigkeit);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 9)
      {
        svghorizontalerknotenabstand[i] = Suchergebnis[i];
        i++;
      }
      svgHorizontalerKnotenAbstand = atof(svghorizontalerknotenabstand);
      //printf("%g\n",svgHorizontalerKnotenAbstand);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 8)
      {
        svgZweigFarbe[i] = Suchergebnis[i];
        i++;
      }
      svgZweigFarbe[i] = 0;
      //printf("%s\n",svgZweigFarbe);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 10)
      {
        i++;
      }
      Suchergebnis[i] = 0;
      svgZweigStrichelung = calloc(strlen(Suchergebnis)+1,sizeof(char));
      strcpy(svgZweigStrichelung, Suchergebnis);
      //printf("svgZweigStrichelung: %s\n",svgZweigStrichelung);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 9)
      {
        svgzweigdicke[i] = Suchergebnis[i];
        i++;
      }
      svgZweigDicke = atof(svgzweigdicke);
      //printf("svgZweigDicke = %g\n",svgZweigDicke);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 3)
      {
        svgzweigundurchsichtigkeit[i] = Suchergebnis[i];
        i++;
      }
      svgZweigUndurchsichtigkeit = atoi(svgzweigundurchsichtigkeit);
      //printf("%d\n",svgZweigUndurchsichtigkeit);

      Suchergebnis = strtok(NULL,"=");
      if(Suchergebnis[0] == 48)
      {
        svgBruchZweige = 0;
      }

      Suchergebnis = strtok(NULL,"=");
      if(Suchergebnis[0] == 48)
      {
        svgBruchDarstellungZweig = 0;
      }

      Suchergebnis = strtok(NULL,"=");
      if(Suchergebnis[0] != 48)
      {
        svgBruchDrehung = 1;
      }

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 9)
      {
        svgwskverschiebung[i] = Suchergebnis[i];
        i++;
      }
      svgWskVerschiebung = atof(svgwskverschiebung);
      //printf("%g\n",svgWskVerschiebung);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 9)
      {
        svgzweigwskabstand[i] = Suchergebnis[i];
        i++;
      }
      svgZweigWskAbstand = atof(svgzweigwskabstand);
      //printf("%g\n",svgZweigWskAbstand);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 39)
      {
        svgSchriftartWsk[i] = Suchergebnis[i];
        i++;
      }
      svgSchriftartWsk[i] = 0;
      //printf("%s\n",svgSchriftartWsk);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 9)
      {
        svgschriftgroessewsk[i] = Suchergebnis[i];
        i++;
      }
      svgSchriftgroesseWsk = atoi(svgschriftgroessewsk);
      //printf("%d\n",svgSchriftgroesseWsk);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 9)
      {
        svgknotenbreite[i] = Suchergebnis[i];
        i++;
      }
      svgKnotenBreite = atof(svgknotenbreite);
      //printf("%g\n",svgKnotenBreite);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 9)
      {
        svgknotenhoehe[i] = Suchergebnis[i];
        i++;
      }
      svgKnotenHoehe = atof(svgknotenhoehe);
      //printf("%g\n",svgKnotenHoehe);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 9)
      {
        svgvertikalerknotenabstand[i] = Suchergebnis[i];
        i++;
      }
      svgVertikalerKnotenAbstand = atof(svgvertikalerknotenabstand);
      //printf("%g\n",svgVertikalerKnotenAbstand);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 8)
      {
        svgRahmenKnotenFarbe[i] = Suchergebnis[i];
        i++;
      }
      svgRahmenKnotenFarbe[i] = 0;
      //printf("%s\n",svgRahmenKnotenFarbe);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 10)
      {
        i++;
      }
      Suchergebnis[i] = 0;
      svgRahmenKnotenStrichelung = calloc(strlen(Suchergebnis)+1,sizeof(char));
      strcpy(svgRahmenKnotenStrichelung, Suchergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 9)
      {
        svgrahmenknotendicke[i] = Suchergebnis[i];
        i++;
      }
      svgRahmenKnotenDicke = atof(svgrahmenknotendicke);
      //printf("%g\n",svgRahmenKnotenDicke);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 3)
      {
        svgrahmenknotenundurchsichtigkeit[i] = Suchergebnis[i];
        i++;
      }
      svgRahmenKnotenUndurchsichtigkeit = atoi(svgrahmenknotenundurchsichtigkeit);
      //printf("%d\n",svgRahmenKnotenUndurchsichtigkeit);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 8)
      {
        svgFuellfarbeKnoten[i] = Suchergebnis[i];
        i++;
      }
      svgFuellfarbeKnoten[i] = 0;
      //printf("%s\n",svgFuellfarbeKnoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 3)
      {
        svgfuellundurchsichtigkeitknoten[i] = Suchergebnis[i];
        i++;
      }
      svgFuellundurchsichtigkeitKnoten = atoi(svgfuellundurchsichtigkeitknoten);
      //printf("%d\n",svgFuellundurchsichtigkeitKnoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 39)
      {
        svgSchriftartKnoten[i] = Suchergebnis[i];
        i++;
      }
      svgSchriftartKnoten[i] = 0;
      //printf("%s\n",svgSchriftartKnoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 9)
      {
        svgschriftgroesseknoten[i] = Suchergebnis[i];
        i++;
      }
      svgSchriftgroesseKnoten = atoi(svgschriftgroesseknoten);
      //printf("%d\n",svgSchriftgroesseKnoten);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 8)
      {
        svgRahmenErgebnisTitelFarbe[i] = Suchergebnis[i];
        i++;
      }
      svgRahmenErgebnisTitelFarbe[i] = 0;
      //printf("%s\n",svgRahmenErgebnisTitelFarbe);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 10)
      {
        i++;
      }
      Suchergebnis[i] = 0;
      svgRahmenErgebnisTitelStrichelung = calloc(strlen(Suchergebnis)+1,sizeof(char));
      strcpy(svgRahmenErgebnisTitelStrichelung, Suchergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 9)
      {
        svgrahmenergebnistiteldicke[i] = Suchergebnis[i];
        i++;
      }
      svgRahmenErgebnisTitelDicke = atof(svgrahmenergebnistiteldicke);
      //printf("%g\n",svgRahmenErgebnisTitelDicke);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 3)
      {
        svgrahmenergebnistitelundurchsichtigkeit[i] = Suchergebnis[i];
        i++;
      }
      svgRahmenErgebnisTitelUndurchsichtigkeit = atoi(svgrahmenergebnistitelundurchsichtigkeit);
      //printf("%d\n",svgRahmenErgebnisTitelUndurchsichtigkeit);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 8)
      {
        svgFuellfarbeErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      svgFuellfarbeErgebnisTitel[i] = 0;
      //printf("%s\n",svgFuellfarbeErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 3)
      {
        svgfuellundurchsichtigkeitergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      svgFuellundurchsichtigkeitErgebnisTitel = atoi(svgfuellundurchsichtigkeitergebnistitel);
      //printf("%d\n",svgFuellundurchsichtigkeitErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 39)
      {
        svgSchriftartErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      svgSchriftartErgebnisTitel[i] = 0;
      //printf("%s\n",svgSchriftartErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 9)
      {
        svgschriftgroesseergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      svgSchriftgroesseErgebnisTitel = atoi(svgschriftgroesseergebnistitel);
      //printf("%d\n",svgSchriftgroesseErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 9)
      {
        svgergebnisbreite[i] = Suchergebnis[i];
        i++;
      }
      svgErgebnisBreite = atof(svgergebnisbreite);
      //printf("%g\n",svgErgebnisBreite);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 8)
      {
        svgRahmenErgebnisFarbe[i] = Suchergebnis[i];
        i++;
      }
      svgRahmenErgebnisFarbe[i] = 0;
      //printf("%s\n",svgRahmenErgebnisFarbe);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 10)
      {
        i++;
      }
      Suchergebnis[i] = 0;
      svgRahmenErgebnisStrichelung = calloc(strlen(Suchergebnis)+1,sizeof(char));
      strcpy(svgRahmenErgebnisStrichelung, Suchergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 9)
      {
        svgrahmenergebnisdicke[i] = Suchergebnis[i];
        i++;
      }
      svgRahmenErgebnisDicke = atof(svgrahmenergebnisdicke);
      //printf("%g\n",svgRahmenErgebnisDicke);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 3)
      {
        svgrahmenergebnisundurchsichtigkeit[i] = Suchergebnis[i];
        i++;
      }
      svgRahmenErgebnisUndurchsichtigkeit = atoi(svgrahmenergebnisundurchsichtigkeit);
      //printf("%d\n",svgRahmenErgebnisUndurchsichtigkeit);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 8)
      {
        svgFuellfarbeErgebnis[i] = Suchergebnis[i];
        i++;
      }
      svgFuellfarbeErgebnis[i] = 0;
      //printf("%s\n",svgFuellfarbeErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 3)
      {
        svgfuellundurchsichtigkeitergebnis[i] = Suchergebnis[i];
        i++;
      }
      svgFuellundurchsichtigkeitErgebnis = atoi(svgfuellundurchsichtigkeitergebnis);
      //printf("%d\n",svgFuellundurchsichtigkeitErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 39)
      {
        svgSchriftartErgebnis[i] = Suchergebnis[i];
        i++;
      }
      svgSchriftartErgebnis[i] = 0;
      //printf("%s\n",svgSchriftartErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 9)
      {
        svgschriftgroesseergebnis[i] = Suchergebnis[i];
        i++;
      }
      svgSchriftgroesseErgebnis = atoi(svgschriftgroesseergebnis);
      //printf("%d\n",svgSchriftgroesseErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 8)
      {
        svgRahmenWskErgebnisTitelFarbe[i] = Suchergebnis[i];
        i++;
      }
      svgRahmenWskErgebnisTitelFarbe[i] = 0;
      //printf("%s\n",svgRahmenWskErgebnisTitelFarbe);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 10)
      {
        i++;
      }
      Suchergebnis[i] = 0;
      svgRahmenWskErgebnisTitelStrichelung = calloc(strlen(Suchergebnis)+1,sizeof(char));
      strcpy(svgRahmenWskErgebnisTitelStrichelung, Suchergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 9)
      {
        svgrahmenwskergebnistiteldicke[i] = Suchergebnis[i];
        i++;
      }
      svgRahmenWskErgebnisTitelDicke = atof(svgrahmenwskergebnistiteldicke);
      //printf("%g\n",svgRahmenWskErgebnisTitelDicke);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 3)
      {
        svgrahmenwskergebnistitelundurchsichtigkeit[i] = Suchergebnis[i];
        i++;
      }
      svgRahmenWskErgebnisTitelUndurchsichtigkeit = atoi(svgrahmenwskergebnistitelundurchsichtigkeit);
      //printf("%d\n",svgRahmenWskErgebnisTitelUndurchsichtigkeit);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 8)
      {
        svgFuellfarbeWskErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      svgFuellfarbeWskErgebnisTitel[i] = 0;
      //printf("%s\n",svgFuellfarbeWskErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 3)
      {
        svgfuellundurchsichtigkeitwskergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      svgFuellundurchsichtigkeitWskErgebnisTitel = atoi(svgfuellundurchsichtigkeitwskergebnistitel);
      //printf("%d\n",svgFuellundurchsichtigkeitWskErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 39)
      {
        svgSchriftartWskErgebnisTitel[i] = Suchergebnis[i];
        i++;
      }
      svgSchriftartWskErgebnisTitel[i] = 0;
      //printf("%s\n",svgSchriftartWskErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 9)
      {
        svgschriftgroessewskergebnistitel[i] = Suchergebnis[i];
        i++;
      }
      svgSchriftgroesseWskErgebnisTitel = atoi(svgschriftgroessewskergebnistitel);
      //printf("%d\n",svgSchriftgroesseWskErgebnisTitel);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 9)
      {
        svgwskbreite[i] = Suchergebnis[i];
        i++;
      }
      svgWskBreite = atof(svgwskbreite);
      //printf("%g\n",svgWskBreite);

      Suchergebnis = strtok(NULL,"=");
      if(Suchergebnis[0] == 48)
      {
        svgBruchErgebnis = 0;
      }

      Suchergebnis = strtok(NULL,"=");
      if(Suchergebnis[0] == 48)
      {
        svgBruchDarstellungErgebnis = 0;
      }

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 8)
      {
        svgRahmenWskErgebnisFarbe[i] = Suchergebnis[i];
        i++;
      }
      svgRahmenWskErgebnisFarbe[i] = 0;
      //printf("%s\n",svgRahmenWskErgebnisFarbe);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 10)
      {
        i++;
      }
      Suchergebnis[i] = 0;
      svgRahmenWskErgebnisStrichelung = calloc(strlen(Suchergebnis)+1,sizeof(char));
      strcpy(svgRahmenWskErgebnisStrichelung, Suchergebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 9)
      {
        svgrahmenwskergebnisdicke[i] = Suchergebnis[i];
        i++;
      }
      svgRahmenWskErgebnisDicke = atof(svgrahmenwskergebnisdicke);
      //printf("%g\n",svgRahmenWskErgebnisDicke);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 3)
      {
        svgrahmenwskergebnisundurchsichtigkeit[i] = Suchergebnis[i];
        i++;
      }
      svgRahmenWskErgebnisUndurchsichtigkeit = atoi(svgrahmenwskergebnisundurchsichtigkeit);
      //printf("%d\n",svgRahmenWskErgebnisUndurchsichtigkeit);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 8)
      {
        svgFuellfarbeWskErgebnis[i] = Suchergebnis[i];
        i++;
      }
      svgFuellfarbeWskErgebnis[i] = 0;
      //printf("%s\n",svgFuellfarbeWskErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 3)
      {
        svgfuellundurchsichtigkeitwskergebnis[i] = Suchergebnis[i];
        i++;
      }
      svgFuellundurchsichtigkeitWskErgebnis = atoi(svgfuellundurchsichtigkeitwskergebnis);
      //printf("%d\n",svgFuellundurchsichtigkeitWskErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 39)
      {
        svgSchriftartWskErgebnis[i] = Suchergebnis[i];
        i++;
      }
      svgSchriftartWskErgebnis[i] = 0;
      //printf("%s\n",svgSchriftartWskErgebnis);

      Suchergebnis = strtok(NULL,"=");
      i=0;
      while(Suchergebnis[i] != 9 && Suchergebnis[i] != 0 && Suchergebnis[i] != 10 && i < 9)
      {
        svgschriftgroessewskergebnis[i] = Suchergebnis[i];
        i++;
      }
      svgSchriftgroesseWskErgebnis = atoi(svgschriftgroessewskergebnis);
      //printf("%d\n",svgSchriftgroesseWskErgebnis);
    }

    free(Einstellungen);
  }


  char rechteckoderellipse[7] = "";
  if(!KnotenForm && odt)
  {
    strcpy(rechteckoderellipse,"rect");
  }
  else
  {
    strcpy(rechteckoderellipse,"circle");
  }

  if(!BruchErgebnis && odt)
  {
    BruchErgebnisFormel = 0;
  }

  if(!BruchZweige && odt)
  {
    BruchZweigeFormel = 0;
  }

  if(!svgBruchErgebnis && svg)
  {
    svgBruchDarstellungErgebnis = 0;
  }

  if(!svgBruchZweige && svg)
  {
    svgBruchDarstellungZweig = 0;
  }

  char FuellungKnotenTyp[9] = "";
  char FuellungKnotenTypSpezifizierung[100] = "";
  char FuellungErgebnisTitelTyp[9] = "";
  char FuellungErgebnisTitelTypSpezifizierung[100] = "";
  char FuellungErgebnisTyp[9] = "";
  char FuellungErgebnisTypSpezifizierung[100] = "";
  char FuellungWskErgebnisTitelTyp[9] = "";
  char FuellungWskErgebnisTitelTypSpezifizierung[100] = "";
  char FuellungWskErgebnisTyp[9] = "";
  char FuellungWskErgebnisTypSpezifizierung[100] = "";
  char TransparenzVerlaufKnotenTyp[100] = "";
  char TransparenzVerlaufErgebnisTitelTyp[100] = "";
  char TransparenzVerlaufErgebnisTyp[100] = "";
  char TransparenzVerlaufWskErgebnisTitelTyp[100] = "";
  char TransparenzVerlaufWskErgebnisTyp[100] = "";
  char FuellungSchraffurArtKnotenString[7] = "";
  char FuellungSchraffurArtErgebnisTitelString[7] = "";
  char FuellungSchraffurArtErgebnisString[7] = "";
  char FuellungSchraffurArtWskErgebnisTitelString[7] = "";
  char FuellungSchraffurArtWskErgebnisString[7] = "";
  char shadowstylewskergebnis[60] = "";


  if(odt)
  {
    switch(FuellungKnoten)
    {
      case 0: strcat(FuellungKnotenTyp,"none"); break;
      case 2: strcat(FuellungKnotenTypSpezifizierung," draw:fill-gradient-name=\"FarbverlaufKnoten\""); strcat(FuellungKnotenTyp,"gradient"); break;
      case 3: strcat(FuellungKnotenTypSpezifizierung," draw:fill-hatch-name=\"SchraffurKnoten\""); strcat(FuellungKnotenTyp,"hatch"); break;
      case 4: strcat(FuellungKnotenTypSpezifizierung," draw:fill-image-name=\""); strcat(FuellungKnotenTypSpezifizierung,FuellungBildKnoten); strcat(FuellungKnotenTypSpezifizierung,"\""); strcat(FuellungKnotenTyp,"bitmap"); break;
      default: break;
    }

    switch(FuellungErgebnisTitel)
    {
      case 0: strcat(FuellungErgebnisTitelTyp,"none"); break;
      case 2: strcat(FuellungErgebnisTitelTypSpezifizierung," draw:fill-gradient-name=\"FarbverlaufErgebnisTitel\""); strcat(FuellungErgebnisTitelTyp,"gradient"); break;
      case 3: strcat(FuellungErgebnisTitelTypSpezifizierung," draw:fill-hatch-name=\"SchraffurErgebnisTitel\""); strcat(FuellungErgebnisTitelTyp,"hatch"); break;
      case 4: strcat(FuellungErgebnisTitelTypSpezifizierung," draw:fill-image-name=\""); strcat(FuellungErgebnisTitelTypSpezifizierung,FuellungBildErgebnisTitel); strcat(FuellungErgebnisTitelTypSpezifizierung,"\""); strcat(FuellungErgebnisTitelTyp,"bitmap"); break;
      default: break;
    }

    switch(FuellungErgebnis)
    {
      case 0: strcat(FuellungErgebnisTyp,"none"); break;
      case 2: strcat(FuellungErgebnisTypSpezifizierung," draw:fill-gradient-name=\"FarbverlaufErgebnis\""); strcat(FuellungErgebnisTyp,"gradient"); break;
      case 3: strcat(FuellungErgebnisTypSpezifizierung," draw:fill-hatch-name=\"SchraffurErgebnis\""); strcat(FuellungErgebnisTyp,"hatch"); break;
      case 4: strcat(FuellungErgebnisTypSpezifizierung," draw:fill-image-name=\""); strcat(FuellungErgebnisTypSpezifizierung,FuellungBildErgebnis); strcat(FuellungErgebnisTypSpezifizierung,"\""); strcat(FuellungErgebnisTyp,"bitmap"); break;
      default: break;
    }

    switch(FuellungWskErgebnisTitel)
    {
      case 0: strcat(FuellungWskErgebnisTitelTyp,"none"); break;
      case 2: strcat(FuellungWskErgebnisTitelTypSpezifizierung," draw:fill-gradient-name=\"FarbverlaufWskErgebnisTitel\""); strcat(FuellungWskErgebnisTitelTyp,"gradient"); break;
      case 3: strcat(FuellungWskErgebnisTitelTypSpezifizierung," draw:fill-hatch-name=\"SchraffurWskErgebnisTitel\""); strcat(FuellungWskErgebnisTitelTyp,"hatch"); break;
      case 4: strcat(FuellungWskErgebnisTitelTypSpezifizierung," draw:fill-image-name=\""); strcat(FuellungWskErgebnisTitelTypSpezifizierung,FuellungBildWskErgebnisTitel); strcat(FuellungWskErgebnisTitelTypSpezifizierung,"\""); strcat(FuellungWskErgebnisTitelTyp,"bitmap"); break;
      default: break;
    }

    switch(FuellungWskErgebnis)
    {
      case 0: strcat(FuellungWskErgebnisTyp,"none"); break;
      case 2: strcat(FuellungWskErgebnisTypSpezifizierung," draw:fill-gradient-name=\"FarbverlaufWskErgebnis\""); strcat(FuellungWskErgebnisTyp,"gradient"); break;
      case 3: strcat(FuellungWskErgebnisTypSpezifizierung," draw:fill-hatch-name=\"SchraffurWskErgebnis\""); strcat(FuellungWskErgebnisTyp,"hatch"); break;
      case 4: strcat(FuellungWskErgebnisTypSpezifizierung," draw:fill-image-name=\""); strcat(FuellungWskErgebnisTypSpezifizierung,FuellungBildWskErgebnis); strcat(FuellungWskErgebnisTypSpezifizierung,"\""); strcat(FuellungWskErgebnisTyp,"bitmap"); break;
      default: break;
    }

    if(FuellungTransparenzverlaufArtKnoten[0] == 48)
    {
      strcat(TransparenzVerlaufKnotenTyp, " draw-opacity=\"");
      char FuellundurchsichtigkeitKnotenString[4] = "";
      snprintf(FuellundurchsichtigkeitKnotenString,4,"%d",FuellundurchsichtigkeitKnoten);
      strcat(TransparenzVerlaufKnotenTyp, FuellundurchsichtigkeitKnotenString);
      strcat(TransparenzVerlaufKnotenTyp, "\"");
    }
    else
    {
      strcat(TransparenzVerlaufKnotenTyp, " draw-opacity=\"100%%\" draw:opacity-name=\"TransparenzverlaufKnoten\"");
    }

    if(FuellungTransparenzverlaufArtErgebnisTitel[0] == 48)
    {
      strcat(TransparenzVerlaufErgebnisTitelTyp, " draw-opacity=\"");
      char FuellundurchsichtigkeitErgebnisTitelString[4] = "";
      snprintf(FuellundurchsichtigkeitErgebnisTitelString,4,"%d",FuellundurchsichtigkeitErgebnisTitel);
      strcat(TransparenzVerlaufErgebnisTitelTyp, FuellundurchsichtigkeitErgebnisTitelString);
      strcat(TransparenzVerlaufErgebnisTitelTyp, "\"");
    }
    else
    {
      strcat(TransparenzVerlaufErgebnisTitelTyp, " draw-opacity=\"100%%\" draw:opacity-name=\"TransparenzverlaufErgebnisTitel\"");
    }

    if(FuellungTransparenzverlaufArtErgebnis[0] == 48)
    {
      strcat(TransparenzVerlaufErgebnisTyp, " draw-opacity=\"");
      char FuellundurchsichtigkeitErgebnisString[4] = "";
      snprintf(FuellundurchsichtigkeitErgebnisString,4,"%d",FuellundurchsichtigkeitErgebnis);
      strcat(TransparenzVerlaufErgebnisTyp, FuellundurchsichtigkeitErgebnisString);
      strcat(TransparenzVerlaufErgebnisTyp, "\"");
    }
    else
    {
      strcat(TransparenzVerlaufErgebnisTyp, " draw-opacity=\"100%%\" draw:opacity-name=\"TransparenzverlaufErgebnis\"");
    }

    if(FuellungTransparenzverlaufArtWskErgebnisTitel[0] == 48)
    {
      strcat(TransparenzVerlaufWskErgebnisTitelTyp, " draw-opacity=\"");
      char FuellundurchsichtigkeitWskErgebnisTitelString[4] = "";
      snprintf(FuellundurchsichtigkeitWskErgebnisTitelString,4,"%d",FuellundurchsichtigkeitWskErgebnisTitel);
      strcat(TransparenzVerlaufWskErgebnisTitelTyp, FuellundurchsichtigkeitWskErgebnisTitelString);
      strcat(TransparenzVerlaufWskErgebnisTitelTyp, "\"");
    }
    else
    {
      strcat(TransparenzVerlaufWskErgebnisTitelTyp, " draw-opacity=\"100%%\" draw:opacity-name=\"TransparenzverlaufWskErgebnisTitel\"");
    }

    if(FuellungTransparenzverlaufArtWskErgebnis[0] == 48)
    {
      strcat(TransparenzVerlaufWskErgebnisTyp, " draw-opacity=\"");
      char FuellundurchsichtigkeitWskErgebnisString[4] = "";
      snprintf(FuellundurchsichtigkeitWskErgebnisString,4,"%d",FuellundurchsichtigkeitWskErgebnis);
      strcat(TransparenzVerlaufWskErgebnisTyp, FuellundurchsichtigkeitWskErgebnisString);
      strcat(TransparenzVerlaufWskErgebnisTyp, "\"");
    }
    else
    {
      strcat(TransparenzVerlaufWskErgebnisTyp, " draw-opacity=\"100%%\" draw:opacity-name=\"TransparenzverlaufWskErgebnis\"");
    }

    switch(FuellungSchraffurArtKnoten)
    {
      case 2: strcat(FuellungSchraffurArtKnotenString, "double"); break;
      case 3: strcat(FuellungSchraffurArtKnotenString, "triple"); break;
      default:  strcat(FuellungSchraffurArtKnotenString, "single"); break;
    }

    switch(FuellungSchraffurArtErgebnisTitel)
    {
      case 2: strcat(FuellungSchraffurArtErgebnisTitelString, "double"); break;
      case 3: strcat(FuellungSchraffurArtErgebnisTitelString, "triple"); break;
      default:  strcat(FuellungSchraffurArtErgebnisTitelString, "single"); break;
    }

    switch(FuellungSchraffurArtErgebnis)
    {
      case 2: strcat(FuellungSchraffurArtErgebnisString, "double"); break;
      case 3: strcat(FuellungSchraffurArtErgebnisString, "triple"); break;
      default:  strcat(FuellungSchraffurArtErgebnisString, "single"); break;
    }

    switch(FuellungSchraffurArtWskErgebnisTitel)
    {
      case 2: strcat(FuellungSchraffurArtWskErgebnisTitelString, "double"); break;
      case 3: strcat(FuellungSchraffurArtWskErgebnisTitelString, "triple"); break;
      default:  strcat(FuellungSchraffurArtWskErgebnisTitelString, "single"); break;
    }

    switch(FuellungSchraffurArtWskErgebnis)
    {
      case 2: strcat(FuellungSchraffurArtWskErgebnisString, "double"); break;
      case 3: strcat(FuellungSchraffurArtWskErgebnisString, "triple"); break;
      default:  strcat(FuellungSchraffurArtWskErgebnisString, "single"); break;
    }

    if(strcmp(SchattenWskErgebnis,"visible"))
    {
      strcpy(shadowstylewskergebnis,"none");
    }
    else
    {
      char schattenverschiebungxwskergebnis[7] = "", schattenverschiebungywskergebnis[7] = "";
      snprintf(schattenverschiebungxwskergebnis,6,"%g",SchattenVerschiebungXWskErgebnis);
      snprintf(schattenverschiebungywskergebnis,6,"%g",SchattenVerschiebungYWskErgebnis);
      strcat(shadowstylewskergebnis,SchattenFarbeWskErgebnis);
      strcat(shadowstylewskergebnis," ");
      strcat(shadowstylewskergebnis,schattenverschiebungxwskergebnis);
      strcat(shadowstylewskergebnis,"cm ");
      strcat(shadowstylewskergebnis,schattenverschiebungxwskergebnis);
      strcat(shadowstylewskergebnis,"cm");
    }
  }

  char *svgzweigstrichelung, *svgrahmenknotenstrichelung, *svgrahmenergebnistitelstrichelung, *svgrahmenergebnisstrichelung, *svgrahmenwskergebnistitelstrichelung, *svgrahmenwskergebnisstrichelung;

  if(svg)
  {
    svgzweigstrichelung = calloc(strlen(svgZweigStrichelung)+21,sizeof(char));
    if(strcmp(svgZweigStrichelung,"none"))
    {
      if(zeichenzaehlen(svgZweigStrichelung,',') > 0 && zeichenzaehlen(svgZweigStrichelung,',') % 2 == 1)
      {
        strcat(svgzweigstrichelung," stroke-dasharray=\"");
        strcat(svgzweigstrichelung,svgZweigStrichelung);
        strcat(svgzweigstrichelung,"\"");
      }
    }
    //printf("Zweigstrichelung: %s\n",svgzweigstrichelung);
    free(svgZweigStrichelung);

    svgrahmenknotenstrichelung = calloc(strlen(svgRahmenKnotenStrichelung)+21,sizeof(char));
    if(strcmp(svgRahmenKnotenStrichelung,"none"))
    {
      if(zeichenzaehlen(svgRahmenKnotenStrichelung,',') > 0 && zeichenzaehlen(svgRahmenKnotenStrichelung,',') % 2 == 1)
      {
        strcat(svgrahmenknotenstrichelung," stroke-dasharray=\"");
        strcat(svgrahmenknotenstrichelung,svgRahmenKnotenStrichelung);
        strcat(svgrahmenknotenstrichelung,"\"");
      }
    }
    //printf("RahmenKnotenstrichelung: %s\n",svgrahmenknotenstrichelung);
    free(svgRahmenKnotenStrichelung);

    svgrahmenergebnistitelstrichelung = calloc(strlen(svgRahmenErgebnisTitelStrichelung)+21,sizeof(char));
    if(strcmp(svgRahmenErgebnisTitelStrichelung,"none"))
    {
      if(zeichenzaehlen(svgRahmenErgebnisTitelStrichelung,',') > 0 && zeichenzaehlen(svgRahmenErgebnisTitelStrichelung,',') % 2 == 1)
      {
        strcat(svgrahmenergebnistitelstrichelung," stroke-dasharray=\"");
        strcat(svgrahmenergebnistitelstrichelung,svgRahmenErgebnisTitelStrichelung);
        strcat(svgrahmenergebnistitelstrichelung,"\"");
      }
    }
    //printf("RahmenErgebnisTitelstrichelung: %s\n",svgrahmenergebnistitelstrichelung);
    free(svgRahmenErgebnisTitelStrichelung);

    svgrahmenergebnisstrichelung = calloc(strlen(svgRahmenErgebnisStrichelung)+21,sizeof(char));
    if(strcmp(svgRahmenErgebnisStrichelung,"none"))
    {
      if(zeichenzaehlen(svgRahmenErgebnisStrichelung,',') > 0 && zeichenzaehlen(svgRahmenErgebnisStrichelung,',') % 2 == 1)
      {
        strcat(svgrahmenergebnisstrichelung," stroke-dasharray=\"");
        strcat(svgrahmenergebnisstrichelung,svgRahmenErgebnisStrichelung);
        strcat(svgrahmenergebnisstrichelung,"\"");
      }
    }
    //printf("RahmenErgebnisstrichelung: %s\n",svgrahmenergebnisstrichelung);
    free(svgRahmenErgebnisStrichelung);

    svgrahmenwskergebnistitelstrichelung = calloc(strlen(svgRahmenWskErgebnisTitelStrichelung)+21,sizeof(char));
    if(strcmp(svgRahmenWskErgebnisTitelStrichelung,"none"))
    {
      if(zeichenzaehlen(svgRahmenWskErgebnisTitelStrichelung,',') > 0 && zeichenzaehlen(svgRahmenWskErgebnisTitelStrichelung,',') % 2 == 1)
      {
        strcat(svgrahmenwskergebnistitelstrichelung," stroke-dasharray=\"");
        strcat(svgrahmenwskergebnistitelstrichelung,svgRahmenWskErgebnisTitelStrichelung);
        strcat(svgrahmenwskergebnistitelstrichelung,"\"");
      }
    }
    //printf("RahmenWskErgebnisTitelstrichelung: %s\n",svgrahmenwskergebnistitelstrichelung);
    free(svgRahmenWskErgebnisTitelStrichelung);

    svgrahmenwskergebnisstrichelung = calloc(strlen(svgRahmenWskErgebnisStrichelung)+21,sizeof(char));
    if(strcmp(svgRahmenWskErgebnisStrichelung,"none"))
    {
      if(zeichenzaehlen(svgRahmenWskErgebnisStrichelung,',') > 0 && zeichenzaehlen(svgRahmenWskErgebnisStrichelung,',') % 2 == 1)
      {
        strcat(svgrahmenwskergebnisstrichelung," stroke-dasharray=\"");
        strcat(svgrahmenwskergebnisstrichelung,svgRahmenWskErgebnisStrichelung);
        strcat(svgrahmenwskergebnisstrichelung,"\"");
      }
    }
    //printf("RahmenWskErgebnisstrichelung: %s\n",svgrahmenwskergebnisstrichelung);
    free(svgRahmenWskErgebnisStrichelung);

    if(strcmp(svgZweigFarbe,"black") && strcmp(svgZweigFarbe,"maroon") && strcmp(svgZweigFarbe,"green") && strcmp(svgZweigFarbe,"olive") && strcmp(svgZweigFarbe,"navy") && strcmp(svgZweigFarbe,"purple") && strcmp(svgZweigFarbe,"teal") && strcmp(svgZweigFarbe,"silver") && strcmp(svgZweigFarbe,"gray") && strcmp(svgZweigFarbe,"red") && strcmp(svgZweigFarbe,"lime") && strcmp(svgZweigFarbe,"yellow") && strcmp(svgZweigFarbe,"blue") && strcmp(svgZweigFarbe,"fuchsia") && strcmp(svgZweigFarbe,"aqua") && strcmp(svgZweigFarbe,"white") && strcmp(svgZweigFarbe,"none"))
    {
    //printf("svgZweigFarbe: %s\n",svgZweigFarbe);
      memmove(svgZweigFarbe+1,svgZweigFarbe,strlen(svgZweigFarbe));
    //printf("svgZweigFarbe: %s\n",svgZweigFarbe);
      svgZweigFarbe[0] = 35;
    }
    //printf("svgZweigFarbe: %s\n",svgZweigFarbe);

    if(strcmp(svgRahmenKnotenFarbe,"black") && strcmp(svgRahmenKnotenFarbe,"maroon") && strcmp(svgRahmenKnotenFarbe,"green") && strcmp(svgRahmenKnotenFarbe,"olive") && strcmp(svgRahmenKnotenFarbe,"navy") && strcmp(svgRahmenKnotenFarbe,"purple") && strcmp(svgRahmenKnotenFarbe,"teal") && strcmp(svgRahmenKnotenFarbe,"silver") && strcmp(svgRahmenKnotenFarbe,"gray") && strcmp(svgRahmenKnotenFarbe,"red") && strcmp(svgRahmenKnotenFarbe,"lime") && strcmp(svgRahmenKnotenFarbe,"yellow") && strcmp(svgRahmenKnotenFarbe,"blue") && strcmp(svgRahmenKnotenFarbe,"fuchsia") && strcmp(svgRahmenKnotenFarbe,"aqua") && strcmp(svgRahmenKnotenFarbe,"white") && strcmp(svgRahmenKnotenFarbe,"none"))
    {
      memmove(svgRahmenKnotenFarbe+1,svgRahmenKnotenFarbe,strlen(svgRahmenKnotenFarbe));
      svgRahmenKnotenFarbe[0] = 35;
    }
    //printf("svgRahmenKnotenFarbe: %s\n",svgRahmenKnotenFarbe);

    if(strcmp(svgRahmenErgebnisTitelFarbe,"black") && strcmp(svgRahmenErgebnisTitelFarbe,"maroon") && strcmp(svgRahmenErgebnisTitelFarbe,"green") && strcmp(svgRahmenErgebnisTitelFarbe,"olive") && strcmp(svgRahmenErgebnisTitelFarbe,"navy") && strcmp(svgRahmenErgebnisTitelFarbe,"purple") && strcmp(svgRahmenErgebnisTitelFarbe,"teal") && strcmp(svgRahmenErgebnisTitelFarbe,"silver") && strcmp(svgRahmenErgebnisTitelFarbe,"gray") && strcmp(svgRahmenErgebnisTitelFarbe,"red") && strcmp(svgRahmenErgebnisTitelFarbe,"lime") && strcmp(svgRahmenErgebnisTitelFarbe,"yellow") && strcmp(svgRahmenErgebnisTitelFarbe,"blue") && strcmp(svgRahmenErgebnisTitelFarbe,"fuchsia") && strcmp(svgRahmenErgebnisTitelFarbe,"aqua") && strcmp(svgRahmenErgebnisTitelFarbe,"white") && strcmp(svgRahmenErgebnisTitelFarbe,"none"))
    {
      memmove(svgRahmenErgebnisTitelFarbe+1,svgRahmenErgebnisTitelFarbe,strlen(svgRahmenErgebnisTitelFarbe));
      svgRahmenErgebnisTitelFarbe[0] = 35;
    }
    //printf("svgRahmenErgebnisTitelFarbe: %s\n",svgRahmenErgebnisTitelFarbe);

    if(strcmp(svgRahmenErgebnisFarbe,"black") && strcmp(svgRahmenErgebnisFarbe,"maroon") && strcmp(svgRahmenErgebnisFarbe,"green") && strcmp(svgRahmenErgebnisFarbe,"olive") && strcmp(svgRahmenErgebnisFarbe,"navy") && strcmp(svgRahmenErgebnisFarbe,"purple") && strcmp(svgRahmenErgebnisFarbe,"teal") && strcmp(svgRahmenErgebnisFarbe,"silver") && strcmp(svgRahmenErgebnisFarbe,"gray") && strcmp(svgRahmenErgebnisFarbe,"red") && strcmp(svgRahmenErgebnisFarbe,"lime") && strcmp(svgRahmenErgebnisFarbe,"yellow") && strcmp(svgRahmenErgebnisFarbe,"blue") && strcmp(svgRahmenErgebnisFarbe,"fuchsia") && strcmp(svgRahmenErgebnisFarbe,"aqua") && strcmp(svgRahmenErgebnisFarbe,"white") && strcmp(svgRahmenErgebnisFarbe,"none"))
    {
      memmove(svgRahmenErgebnisFarbe+1,svgRahmenErgebnisFarbe,strlen(svgRahmenErgebnisFarbe));
      svgRahmenErgebnisFarbe[0] = 35;
    }
    //printf("svgRahmenErgebnisFarbe: %s\n",svgRahmenErgebnisFarbe);

    if(strcmp(svgRahmenWskErgebnisTitelFarbe,"black") && strcmp(svgRahmenWskErgebnisTitelFarbe,"maroon") && strcmp(svgRahmenWskErgebnisTitelFarbe,"green") && strcmp(svgRahmenWskErgebnisTitelFarbe,"olive") && strcmp(svgRahmenWskErgebnisTitelFarbe,"navy") && strcmp(svgRahmenWskErgebnisTitelFarbe,"purple") && strcmp(svgRahmenWskErgebnisTitelFarbe,"teal") && strcmp(svgRahmenWskErgebnisTitelFarbe,"silver") && strcmp(svgRahmenWskErgebnisTitelFarbe,"gray") && strcmp(svgRahmenWskErgebnisTitelFarbe,"red") && strcmp(svgRahmenWskErgebnisTitelFarbe,"lime") && strcmp(svgRahmenWskErgebnisTitelFarbe,"yellow") && strcmp(svgRahmenWskErgebnisTitelFarbe,"blue") && strcmp(svgRahmenWskErgebnisTitelFarbe,"fuchsia") && strcmp(svgRahmenWskErgebnisTitelFarbe,"aqua") && strcmp(svgRahmenWskErgebnisTitelFarbe,"white") && strcmp(svgRahmenWskErgebnisTitelFarbe,"none"))
    {
      memmove(svgRahmenWskErgebnisTitelFarbe+1,svgRahmenWskErgebnisTitelFarbe,strlen(svgRahmenWskErgebnisTitelFarbe));
      svgRahmenWskErgebnisTitelFarbe[0] = 35;
    }
    //printf("svgRahmenWskErgebnisTitelFarbe: %s\n",svgRahmenWskErgebnisTitelFarbe);

    if(strcmp(svgRahmenWskErgebnisFarbe,"black") && strcmp(svgRahmenWskErgebnisFarbe,"maroon") && strcmp(svgRahmenWskErgebnisFarbe,"green") && strcmp(svgRahmenWskErgebnisFarbe,"olive") && strcmp(svgRahmenWskErgebnisFarbe,"navy") && strcmp(svgRahmenWskErgebnisFarbe,"purple") && strcmp(svgRahmenWskErgebnisFarbe,"teal") && strcmp(svgRahmenWskErgebnisFarbe,"silver") && strcmp(svgRahmenWskErgebnisFarbe,"gray") && strcmp(svgRahmenWskErgebnisFarbe,"red") && strcmp(svgRahmenWskErgebnisFarbe,"lime") && strcmp(svgRahmenWskErgebnisFarbe,"yellow") && strcmp(svgRahmenWskErgebnisFarbe,"blue") && strcmp(svgRahmenWskErgebnisFarbe,"fuchsia") && strcmp(svgRahmenWskErgebnisFarbe,"aqua") && strcmp(svgRahmenWskErgebnisFarbe,"white") && strcmp(svgRahmenWskErgebnisFarbe,"none"))
    {
      memmove(svgRahmenWskErgebnisFarbe+1,svgRahmenWskErgebnisFarbe,strlen(svgRahmenWskErgebnisFarbe));
      svgRahmenWskErgebnisFarbe[0] = 35;
    }
    //printf("svgRahmenWskErgebnisFarbe: %s\n",svgRahmenWskErgebnisFarbe);

    if(strcmp(svgFuellfarbeKnoten,"black") && strcmp(svgFuellfarbeKnoten,"maroon") && strcmp(svgFuellfarbeKnoten,"green") && strcmp(svgFuellfarbeKnoten,"olive") && strcmp(svgFuellfarbeKnoten,"navy") && strcmp(svgFuellfarbeKnoten,"purple") && strcmp(svgFuellfarbeKnoten,"teal") && strcmp(svgFuellfarbeKnoten,"silver") && strcmp(svgFuellfarbeKnoten,"gray") && strcmp(svgFuellfarbeKnoten,"red") && strcmp(svgFuellfarbeKnoten,"lime") && strcmp(svgFuellfarbeKnoten,"yellow") && strcmp(svgFuellfarbeKnoten,"blue") && strcmp(svgFuellfarbeKnoten,"fuchsia") && strcmp(svgFuellfarbeKnoten,"aqua") && strcmp(svgFuellfarbeKnoten,"white") && strcmp(svgFuellfarbeKnoten,"none"))
    {
      memmove(svgFuellfarbeKnoten+1,svgFuellfarbeKnoten,strlen(svgFuellfarbeKnoten));
      svgFuellfarbeKnoten[0] = 35;
    }
    //printf("svgFuellfarbeKnoten: %s\n",svgFuellfarbeKnoten);

    if(strcmp(svgFuellfarbeErgebnisTitel,"black") && strcmp(svgFuellfarbeErgebnisTitel,"maroon") && strcmp(svgFuellfarbeErgebnisTitel,"green") && strcmp(svgFuellfarbeErgebnisTitel,"olive") && strcmp(svgFuellfarbeErgebnisTitel,"navy") && strcmp(svgFuellfarbeErgebnisTitel,"purple") && strcmp(svgFuellfarbeErgebnisTitel,"teal") && strcmp(svgFuellfarbeErgebnisTitel,"silver") && strcmp(svgFuellfarbeErgebnisTitel,"gray") && strcmp(svgFuellfarbeErgebnisTitel,"red") && strcmp(svgFuellfarbeErgebnisTitel,"lime") && strcmp(svgFuellfarbeErgebnisTitel,"yellow") && strcmp(svgFuellfarbeErgebnisTitel,"blue") && strcmp(svgFuellfarbeErgebnisTitel,"fuchsia") && strcmp(svgFuellfarbeErgebnisTitel,"aqua") && strcmp(svgFuellfarbeErgebnisTitel,"white") && strcmp(svgFuellfarbeErgebnisTitel,"none"))
    {
      memmove(svgFuellfarbeErgebnisTitel+1,svgFuellfarbeErgebnisTitel,strlen(svgFuellfarbeErgebnisTitel));
      svgFuellfarbeErgebnisTitel[0] = 35;
    }
    //printf("svgFuellfarbeErgebnisTitel: %s\n",svgFuellfarbeErgebnisTitel);

    if(strcmp(svgFuellfarbeErgebnis,"black") && strcmp(svgFuellfarbeErgebnis,"maroon") && strcmp(svgFuellfarbeErgebnis,"green") && strcmp(svgFuellfarbeErgebnis,"olive") && strcmp(svgFuellfarbeErgebnis,"navy") && strcmp(svgFuellfarbeErgebnis,"purple") && strcmp(svgFuellfarbeErgebnis,"teal") && strcmp(svgFuellfarbeErgebnis,"silver") && strcmp(svgFuellfarbeErgebnis,"gray") && strcmp(svgFuellfarbeErgebnis,"red") && strcmp(svgFuellfarbeErgebnis,"lime") && strcmp(svgFuellfarbeErgebnis,"yellow") && strcmp(svgFuellfarbeErgebnis,"blue") && strcmp(svgFuellfarbeErgebnis,"fuchsia") && strcmp(svgFuellfarbeErgebnis,"aqua") && strcmp(svgFuellfarbeErgebnis,"white") && strcmp(svgFuellfarbeErgebnis,"none"))
    {
      memmove(svgFuellfarbeErgebnis+1,svgFuellfarbeErgebnis,strlen(svgFuellfarbeErgebnis));
      svgFuellfarbeErgebnis[0] = 35;
    }
    //printf("svgFuellfarbeErgebnis: %s\n",svgFuellfarbeErgebnis);

    if(strcmp(svgFuellfarbeWskErgebnisTitel,"black") && strcmp(svgFuellfarbeWskErgebnisTitel,"maroon") && strcmp(svgFuellfarbeWskErgebnisTitel,"green") && strcmp(svgFuellfarbeWskErgebnisTitel,"olive") && strcmp(svgFuellfarbeWskErgebnisTitel,"navy") && strcmp(svgFuellfarbeWskErgebnisTitel,"purple") && strcmp(svgFuellfarbeWskErgebnisTitel,"teal") && strcmp(svgFuellfarbeWskErgebnisTitel,"silver") && strcmp(svgFuellfarbeWskErgebnisTitel,"gray") && strcmp(svgFuellfarbeWskErgebnisTitel,"red") && strcmp(svgFuellfarbeWskErgebnisTitel,"lime") && strcmp(svgFuellfarbeWskErgebnisTitel,"yellow") && strcmp(svgFuellfarbeWskErgebnisTitel,"blue") && strcmp(svgFuellfarbeWskErgebnisTitel,"fuchsia") && strcmp(svgFuellfarbeWskErgebnisTitel,"aqua") && strcmp(svgFuellfarbeWskErgebnisTitel,"white") && strcmp(svgFuellfarbeWskErgebnisTitel,"none"))
    {
      memmove(svgFuellfarbeWskErgebnisTitel+1,svgFuellfarbeWskErgebnisTitel,strlen(svgFuellfarbeWskErgebnisTitel));
      svgFuellfarbeWskErgebnisTitel[0] = 35;
    }
    //printf("svgFuellfarbeWskErgebnisTitel: %s\n",svgFuellfarbeWskErgebnisTitel);

    if(strcmp(svgFuellfarbeWskErgebnis,"black") && strcmp(svgFuellfarbeWskErgebnis,"maroon") && strcmp(svgFuellfarbeWskErgebnis,"green") && strcmp(svgFuellfarbeWskErgebnis,"olive") && strcmp(svgFuellfarbeWskErgebnis,"navy") && strcmp(svgFuellfarbeWskErgebnis,"purple") && strcmp(svgFuellfarbeWskErgebnis,"teal") && strcmp(svgFuellfarbeWskErgebnis,"silver") && strcmp(svgFuellfarbeWskErgebnis,"gray") && strcmp(svgFuellfarbeWskErgebnis,"red") && strcmp(svgFuellfarbeWskErgebnis,"lime") && strcmp(svgFuellfarbeWskErgebnis,"yellow") && strcmp(svgFuellfarbeWskErgebnis,"blue") && strcmp(svgFuellfarbeWskErgebnis,"fuchsia") && strcmp(svgFuellfarbeWskErgebnis,"aqua") && strcmp(svgFuellfarbeWskErgebnis,"white") && strcmp(svgFuellfarbeWskErgebnis,"none"))
    {
      memmove(svgFuellfarbeWskErgebnis+1,svgFuellfarbeWskErgebnis,strlen(svgFuellfarbeWskErgebnis));
      svgFuellfarbeWskErgebnis[0] = 35;
    }
    //printf("svgFuellfarbeWskErgebnis: %s\n",svgFuellfarbeWskErgebnis);
  }



  /* Bestimmung der maximalen Knotenlänge, die vorkommt */

  int GesamtLaengeArgument = 0;
  for(i=1;i<ArgumentAnzahl;i++)
  {
    GesamtLaengeArgument += strlen(Argument[i]) + 1;
  }
  char *GesamtArgument = calloc(GesamtLaengeArgument+1,sizeof(char));
  for(i=1;i<ArgumentAnzahl;i++)
  {
    strcat(GesamtArgument, Argument[i]);
    strcat(GesamtArgument, " ");
  }
  //printf("GesamtArgument: %s\n",GesamtArgument);
  short urnefail = 1;
  if(strchr(GesamtArgument, 43) || strchr(GesamtArgument, 44))
  {
    urnefail = 0;
  }
  i=2;
  while(i < ArgumentAnzahl)
  {
    if(atoi(Argument[i]) == 0)
    {
      urnefail = 0;
    }
    i += 2;
  }

  int MaximaleLaengeKnoten = 0;
  char *argzeiger = strtok(GesamtArgument,"+, ");
  while(argzeiger != NULL)
  {
    //printf("argzeiger: %s\n",argzeiger);
    if(MaximaleLaengeKnoten < strlen(argzeiger))
    {
      MaximaleLaengeKnoten = strlen(argzeiger);
      //printf("MAXKNOTEN: %i\n",MaximaleLaengeKnoten);
    }
    argzeiger = strtok(NULL,"+, ");
  }
  free(GesamtArgument);

  /* Erstellen eines eindeutigen Verzeichnisnamens und festlegen der Dateipfade für content.xml, styles.xml und manifest.xml */

/*  char arbeitsverzeichnis[200] = "";
  getcwd(arbeitsverzeichnis,sizeof(arbeitsverzeichnis));
  //printf("Aktuelles Arbeitsverzeichnis: %s\n",arbeitsverzeichnis);*/

  char verzeichnisname[4097]="";
  if(Dateipfad[0] == 48)
  {
    strcpy(verzeichnisname,homeverzeichnis);
    strcat(verzeichnisname,HILFSVERZEICHNIS);
  }
  else
  {
    if(chdir(Dateipfad) == -1)
    {
      char *zeiger;
      char hilfsdateipfad[4097] = "";
      WURZELVERZEICHNIS;
      strcpy(hilfsdateipfad, Dateipfad);
      zeiger = strtok(hilfsdateipfad,PFADTRENNER);
      while(zeiger)
      {
        //printf("%s\n",zeiger);
        strcat(hilfspfad,zeiger);
        strcat(hilfspfad,PFADTRENNER);
        //printf("%s\n",hilfsdateipfad);
        if(chdir(hilfspfad) == -1)
        {
          mkdir(hilfspfad MODUS;
        }
        zeiger = strtok(NULL,PFADTRENNER);
      }
      fprintf(stderr,"Warnung: Das Verzeichnis existiert nicht. Es wurde erstellt.");
      sprintf(verzeichnisname,"%s",Dateipfad);
      strcat(verzeichnisname,HILFSVERZEICHNIS);
    }
    else
    {
      strcpy(verzeichnisname,Dateipfad);
      strcat(verzeichnisname,HILFSVERZEICHNIS);
    }
  }
  //printf("%s\n",verzeichnisname);
  char hilfsverzeichnisname[4097] = "";
  strcpy(hilfsverzeichnisname, verzeichnisname);
  i=1;
  while(chdir(verzeichnisname) != -1)
  {
    //printf("Verzeichnis %s existiert schon!\n",verzeichnisname);
    sprintf(verzeichnisname,"%s%i",hilfsverzeichnisname,i);
    i++;
    //printf("Neuer Verzeichnisname: %s\n",verzeichnisname);
  }


  char CONTENT[100] = "";
  strcat(CONTENT,verzeichnisname);
  strcat(CONTENT,CONTENTXML);

  char MANIFEST[100] = "";
  strcat(MANIFEST,verzeichnisname);
  strcat(MANIFEST,MANIFESTXML);

  char STYLES[100] = "";
  strcat(STYLES,verzeichnisname);
  strcat(STYLES,STYLESXML);


  /* Hier kommen die Sonderfälle MULTINOMIAL und UNABHÄNGIG sowie URNE */

  /* Ziehen aus einer Urne mit Zurücklegen. Syntax: baum z 5 R 50 G 30 W 20 ... p */

  if((*Argument[1] == 90 || *Argument[1] == 122) && strlen(Argument[1]) == 1 && ArgumentAnzahl > 3 && atoi(Argument[2]) != 0 && urnefail)
  {
    unsigned long int gesamtzahlkugeln = 0;
    for(i=4;i<ArgumentAnzahl;i+=2)
    {
      gesamtzahlkugeln += atol(Argument[i]);
    }
    char *knotenargument = calloc((MaximaleLaengeKnoten+1)*(ArgumentAnzahl-3)/2+1,sizeof(char));
    strcpy(knotenargument,Argument[3]);
    for(i=5;i<ArgumentAnzahl-1;i+=2)
    {
      strcat(knotenargument,"+");
      strcat(knotenargument,Argument[i]);
    }
    short wsk = 0;
    char *wskargument;
    if(*Argument[ArgumentAnzahl-1] == 80 || *Argument[ArgumentAnzahl-1] == 112)
    {
      wsk = 1;
      char gesamtzahlkugelnstring[21];
      sprintf(gesamtzahlkugelnstring,"%lu",gesamtzahlkugeln);
      wskargument = calloc(202*(ArgumentAnzahl-3)/2+1,sizeof(char));
      strcat(wskargument,Argument[4]);
      strcat(wskargument,"/");
      strcat(wskargument,gesamtzahlkugelnstring);
      for(i=6;i<ArgumentAnzahl;i+=2)
      {
        strcat(wskargument,"+");
        strcat(wskargument,Argument[i]);
        strcat(wskargument,"/");
        strcat(wskargument,gesamtzahlkugelnstring);
      }
    }
    Argument[1][0] = 77;
    strcpy(Argument[3],knotenargument);
    if(wsk)
    {
      Argument[4] = Argument[3]+strlen(knotenargument)+1;
      strcpy(Argument[4],wskargument);
    }
    else
    {
      Argument[4] = NULL;
    }
    free(knotenargument);
    if(wsk)
    {
      free(wskargument);
    }
    ArgumentAnzahl = (wsk)?5:4;
  }


  /* MULTINOMIAL */

  short multinomial = 0;

  if((*Argument[1] == 77 || *Argument[1] == 109) && strlen(Argument[1]) == 1 && (ArgumentAnzahl == 4 || ArgumentAnzahl == 5) && atoi(Argument[2]) != 0)
  {
    multinomial = 1;
    char *knotengruppe = calloc(strlen(Argument[3])+1,sizeof(char));
    char *wahrscheinlichkeitengruppe, *hilfswahrscheinlichkeitengruppe;
    strcpy(knotengruppe,Argument[3]);
    int AnzahlKnotenInGruppe = zeichenzaehlen(knotengruppe,43)+1;
    int laengsteWahrscheinlichkeit = 0;
    short AnzahlStufen = atoi(Argument[2]);
    //printf("ANZAHLSTUFEN: %i\n",AnzahlStufen);
    short wsk = 0;
    if(Argument[4] != NULL)
    {
      wsk = 1;
      if((*Argument[4] == 71 || *Argument[4] == 103) && strlen(Argument[4]) == 1)
      {
        char einzelwahrscheinlichkeit[23] = "";
        sprintf(einzelwahrscheinlichkeit,"1/%i",AnzahlKnotenInGruppe);
        wahrscheinlichkeitengruppe = calloc(AnzahlKnotenInGruppe*(strlen(einzelwahrscheinlichkeit)+1)+1,sizeof(char));
        for(i=0;i<AnzahlKnotenInGruppe-1;i++)
        {
          strcat(wahrscheinlichkeitengruppe,einzelwahrscheinlichkeit);
          strcat(wahrscheinlichkeitengruppe,"+");
        }
        strcat(wahrscheinlichkeitengruppe,einzelwahrscheinlichkeit);
      }
      else
      {
        wahrscheinlichkeitengruppe = calloc(strlen(Argument[4])+1,sizeof(char));
        strcpy(wahrscheinlichkeitengruppe, Argument[4]);
      }
      hilfswahrscheinlichkeitengruppe = calloc(strlen(wahrscheinlichkeitengruppe)+1,sizeof(char));
      strcpy(hilfswahrscheinlichkeitengruppe, wahrscheinlichkeitengruppe);
      char *einzelwahrscheinlichkeit;
      einzelwahrscheinlichkeit = strtok(hilfswahrscheinlichkeitengruppe, "+");
      while(einzelwahrscheinlichkeit)
      {
        if(laengsteWahrscheinlichkeit < strlen(einzelwahrscheinlichkeit))
        {
          laengsteWahrscheinlichkeit = strlen(einzelwahrscheinlichkeit);
        }
        einzelwahrscheinlichkeit = strtok(NULL, "+");
      }
      //printf("längstewsk: %i\n",laengsteWahrscheinlichkeit);
      free(hilfswahrscheinlichkeitengruppe);
    }
    ArgumentAnzahl = AnzahlStufen + 1;
    Argument = (char **)calloc((wsk)?2*ArgumentAnzahl:ArgumentAnzahl+1,sizeof(char *));
    for(i=0;i<ArgumentAnzahl;i++)
    {
      Argument[i] = (char *)calloc((MaximaleLaengeKnoten+1)*potenziere(AnzahlKnotenInGruppe,i)+i+1,sizeof(char));
    }
    if(wsk)
    {
      for(i=ArgumentAnzahl;i<2*ArgumentAnzahl;i++)
      {
        Argument[i] = (char *)calloc((laengsteWahrscheinlichkeit+1)*potenziere(AnzahlKnotenInGruppe,i-ArgumentAnzahl)+i-ArgumentAnzahl+1,sizeof(char));
      }
    }
    for(j = 1;j <= AnzahlStufen;j++)
    {
      strcpy(Argument[j],knotengruppe);
      for(i = 1;i < potenziere(AnzahlKnotenInGruppe,j-1);i++)
      {
        strcat(Argument[j],",");
        strcat(Argument[j],knotengruppe);
      }
    }
    if(wsk)
    {
      *Argument[ArgumentAnzahl] = 80;
      for(j = ArgumentAnzahl+1;j <= ArgumentAnzahl + AnzahlStufen;j++)
      {
        strcpy(Argument[j],wahrscheinlichkeitengruppe);
        for(i = 1;i < potenziere(AnzahlKnotenInGruppe,j-ArgumentAnzahl-1);i++)
        {
          strcat(Argument[j],",");
          strcat(Argument[j],wahrscheinlichkeitengruppe);
        }
      }
      ArgumentAnzahl *= 2;
      free(wahrscheinlichkeitengruppe);
    }
    free(knotengruppe);
  }

  /* UNABHÄNGIG */

  short unabhaengig = 0;

  if((*Argument[1] == 85 || *Argument[1] == 117) && strlen(Argument[1]) == 1 && ArgumentAnzahl > 2)
  {
    unabhaengig = 1;
    int *KnotenAnzahlStufe = calloc(ArgumentAnzahl-2,sizeof(int)), i;
    for(i=2;i<ArgumentAnzahl;i++)
    {
      KnotenAnzahlStufe[i-2] = zeichenzaehlen(Argument[i],43)+1;
    }
    char **hilfsargument;
    hilfsargument = (char **)calloc(ArgumentAnzahl-2,sizeof(char *));
    for(i=0;i<ArgumentAnzahl-2;i++)
    {
      hilfsargument[i] = (char *)calloc(strlen(Argument[i+2])+1,sizeof(char));
    }
    for(i=0;i<ArgumentAnzahl-2;i++)
    {
      strcpy(hilfsargument[i],Argument[i+2]);
    }
    j = 1;
    Argument = (char **)calloc(ArgumentAnzahl-1,sizeof(char *));
    //printf("ARGUMENTANZAHL: %i\n",ArgumentAnzahl);
    for(i=1;i<ArgumentAnzahl-1;i++)
    {
      j *= KnotenAnzahlStufe[i-1];
      if((*hilfsargument[i-1] == 80 || *hilfsargument[i-1] == 112) && strlen(hilfsargument[i-1]) == 1 && ArgumentAnzahl > 2)
      {
        j=1;
      }
      Argument[i] = (char *)calloc((MaximaleLaengeKnoten+1)*j+1,sizeof(char));
    }
    int KnotenAnzahl=1;
    for(i=1;i<ArgumentAnzahl-1;i++)
    {
      strcpy(Argument[i],hilfsargument[i-1]);
      if((*Argument[i] == 80 || *Argument[i] == 112) && strlen(Argument[i]) == 1 && ArgumentAnzahl > 2)
      {
        KnotenAnzahl=1;
        continue;
      }
      for(j=2;j<=KnotenAnzahl;j++)
      {
        strcat(Argument[i],",");
        strcat(Argument[i],hilfsargument[i-1]);
      }
      KnotenAnzahl *= KnotenAnzahlStufe[i-1];
    }
    for(i=0;i<ArgumentAnzahl-2;i++)
    {
      free(hilfsargument[i]);
    }
    free(hilfsargument);
    free(KnotenAnzahlStufe);
    ArgumentAnzahl -= 1;
  }

  /* Ziehen aus einer Urne ohne Zurücklegen. Syntax: baum o 5 R 50 G 30 W 20 ... p */

  if((*Argument[1] == 79 || *Argument[1] == 111) && strlen(Argument[1]) == 1 && ArgumentAnzahl > 3 && atoi(Argument[2]) != 0 && urnefail)
  {
    unsigned long int *gesamtzahlkugeln, *urne;
    int anzahlstufen = atoi(Argument[2]);
    int anzahlfarben = (ArgumentAnzahl-3)/2;
    gesamtzahlkugeln = calloc(anzahlstufen,sizeof(unsigned long int));
    urne = calloc(anzahlfarben,sizeof(unsigned long int));
    for(i=4;i<ArgumentAnzahl;i+=2)
    {
      gesamtzahlkugeln[0] += atol(Argument[i]);
      urne[(i-4)/2] = atol(Argument[i]);
    }
    for(i=1;i<anzahlstufen;i++)
    {
      gesamtzahlkugeln[i]=gesamtzahlkugeln[0]-i;  // genauso möglich wäre gesamtzahlkugeln[i]=gesamtzahlkugeln[i-1]-1
    }
    char *knotenargument = calloc((MaximaleLaengeKnoten+1)*anzahlfarben+1,sizeof(char));
    strcpy(knotenargument,Argument[3]);
    for(i=5;i<ArgumentAnzahl-1;i+=2)
    {
      strcat(knotenargument,"+");
      strcat(knotenargument,Argument[i]);
    }
    short wsk = 0;
    char ***wskargument;
    if(*Argument[ArgumentAnzahl-1] == 80 || *Argument[ArgumentAnzahl-1] == 112)
    {
      wsk = 1;
      char **gesamtzahlkugelnstring;

      gesamtzahlkugelnstring = (char **)calloc(200,sizeof(char**));
      for(j=0; j < 200; j++)
      {
        gesamtzahlkugelnstring[j] = (char *)calloc(50,sizeof(char));
      }

      for(i=0;i<anzahlstufen;i++)
      {
        sprintf(gesamtzahlkugelnstring[i],"%lu",gesamtzahlkugeln[i]);
        //printf("gesamtzahlkugelnstring[%i]: %s\n",i,gesamtzahlkugelnstring[i]);
      }

      wskargument = (char ***)calloc(anzahlstufen+1,sizeof(char**));
      for(j=0; j < anzahlstufen+1; j++)
      {
        wskargument[j] = (char **)calloc(potenziere(anzahlfarben,anzahlstufen)+1,sizeof(char*));
        for(k=0; k < potenziere(anzahlfarben,anzahlstufen)+1; k++)
        {
          wskargument[j][k] = (char *)calloc(50*anzahlfarben+1,sizeof(char));
        }
      }

      for(k=0;k<anzahlstufen;k++)
      {
        for(j=0;j<potenziere(anzahlfarben,k);j++)
        {
          int anzahlfarbkugeln = atoi(Argument[4]);
          int knotenblocknummer;
          knotenblocknummer=j*anzahlfarben;
          for(l=k-1;l>=0;l--)
          {
            if(0 == (knotenblocknummer / anzahlfarben) % anzahlfarben)
            {
              anzahlfarbkugeln -= 1;
            }
            knotenblocknummer /= anzahlfarben;
          }
          if(anzahlfarbkugeln < 0)
          {
            anzahlfarbkugeln=0;
          }
          char anzahlfarbkugelnstring[300];
          sprintf(anzahlfarbkugelnstring,"%d",anzahlfarbkugeln);
          strcat(wskargument[k][j],anzahlfarbkugelnstring);
          strcat(wskargument[k][j],"/");
          strcat(wskargument[k][j],gesamtzahlkugelnstring[k]);
          for(i=6;i<ArgumentAnzahl;i+=2)
          {
            anzahlfarbkugeln = atoi(Argument[i]);
            knotenblocknummer=j*anzahlfarben+i/2-2;
            for(l=k-1;l>=0;l--)
            {
              if((j*anzahlfarben+i/2-2) % anzahlfarben == (knotenblocknummer / anzahlfarben) % anzahlfarben)
              {
                anzahlfarbkugeln -= 1;
              }
              knotenblocknummer /= anzahlfarben;
            }
            if(anzahlfarbkugeln < 0)
            {
              anzahlfarbkugeln=0;
            }
            strcat(wskargument[k][j],"+");
            sprintf(anzahlfarbkugelnstring,"%i",anzahlfarbkugeln);
            strcat(wskargument[k][j],anzahlfarbkugelnstring);
            strcat(wskargument[k][j],"/");
            strcat(wskargument[k][j],gesamtzahlkugelnstring[k]);
          }
          //printf("wskargument[%i][%i]=%s\n",k,j,wskargument[k][j]);
        }
      }
      for(j=0; j < 200; j++)
      {
        free(gesamtzahlkugelnstring[j]);
      }
      free(gesamtzahlkugelnstring);
    }
    free(gesamtzahlkugeln);
    multinomial = 1;

    char *knotengruppe = calloc(strlen(knotenargument)+1,sizeof(char));
    strcpy(knotengruppe,knotenargument);
    int AnzahlKnotenInGruppe = zeichenzaehlen(knotengruppe,43)+1;
    ArgumentAnzahl = anzahlstufen + 1;
    Argument = (char **)calloc((wsk)?2*ArgumentAnzahl:ArgumentAnzahl+1,sizeof(char *));
    for(i=0;i<ArgumentAnzahl;i++)
    {
      Argument[i] = (char *)calloc((MaximaleLaengeKnoten+1)*potenziere(AnzahlKnotenInGruppe,i)+i+1,sizeof(char));
    }
    if(wsk)
    {
      for(i=ArgumentAnzahl;i<2*ArgumentAnzahl;i++)
      {
        Argument[i] = (char *)calloc(202*potenziere(AnzahlKnotenInGruppe,i-ArgumentAnzahl)+i-ArgumentAnzahl+1,sizeof(char));
      }
    }
    for(j = 1;j <= anzahlstufen;j++)
    {
      strcpy(Argument[j],knotengruppe);
      for(i = 1;i < potenziere(AnzahlKnotenInGruppe,j-1);i++)
      {
        strcat(Argument[j],",");
        strcat(Argument[j],knotengruppe);
      }
    }
    if(wsk)
    {
      *Argument[ArgumentAnzahl] = 80;
      for(j = ArgumentAnzahl+1;j <= ArgumentAnzahl + anzahlstufen;j++)
      {
        strcpy(Argument[j],wskargument[j-ArgumentAnzahl-1][0]);
        for(i = 1;i < potenziere(AnzahlKnotenInGruppe,j-ArgumentAnzahl-1);i++)
        {
          strcat(Argument[j],",");
          strcat(Argument[j],wskargument[j-ArgumentAnzahl-1][i]);
        }
      }
      ArgumentAnzahl *= 2;
    }
    free(knotengruppe);
    if(wsk)
    {
      for(j=0; j < anzahlstufen+1; j++)
      {
        for(k=0; k < potenziere(anzahlfarben,anzahlstufen)+1; k++)
        {
          free(wskargument[j][k]);
        }
        free(wskargument[j]);
      }
      free(wskargument);
    }
    free(knotenargument);
//    //printf("UrneNull: %d\nUrneKuerzen: %d\n",UrneNull,UrneKuerzen);
    if((!UrneNull && odt) || (!svgUrneNull && svg))
    {
      if(!wsk)
      {
        for(i=ArgumentAnzahl-1;i>0;i--)
        {
          for(j=0;j<=zeichenzaehlen(Argument[i],'+')+zeichenzaehlen(Argument[i],',');j++)
          {
            int knotennummer = j % AnzahlKnotenInGruppe, knotenblock = j/AnzahlKnotenInGruppe;
            long long int farbzaehler = urne[knotennummer];
            char *hilfsargument = calloc(strlen(Argument[i])+1,sizeof(char));
            strcpy(hilfsargument,Argument[i]);
            char *gestutztesargument = calloc(strlen(Argument[i])+1,sizeof(char));
            strcpy(gestutztesargument,Argument[i]);
            char *knoten = knotenfinder(hilfsargument,j);
            int position = (knoten - hilfsargument) - 1;
            for(k=i-1;k>0;k--)
            {
              char *hilfsargument2 = calloc(strlen(Argument[k])+1,sizeof(char));
              strcpy(hilfsargument2,Argument[k]);
              char *knoten2 = knotenfinder(hilfsargument2,knotenblock);
              if(!strcmp(knoten2,knoten))
              {
                farbzaehler--;
              }
              knotenblock /= AnzahlKnotenInGruppe;
              if(farbzaehler <= 0)
              {
//              //printf("Knotenlänge: %d\n",strlen(knoten));
//              //printf("gestutztesargument: %s an Adresse %p\n",gestutztesargument,gestutztesargument);
//              //printf("Position: %d\n",position);
                for(l=0;l<strlen(knoten);l++)
                {
                  gestutztesargument[position+l+1] = 58;
                }
              }
              free(hilfsargument2);
            }
            strcpy(Argument[i],gestutztesargument);
            free(gestutztesargument);
            free(hilfsargument);
          }
//        //printf("Argument %d: %s\n",i,Argument[i]);
        }
        for(i=ArgumentAnzahl-1;i>0;i--)
        {
          for(j=0;j<=zeichenzaehlen(Argument[i],'+')+zeichenzaehlen(Argument[i],',');j++)
          {
            short loeschen = 0;
            int /*knotennummer = j % AnzahlKnotenInGruppe,*/ knotenblock = j/AnzahlKnotenInGruppe;
            char *hilfsargument = calloc(strlen(Argument[i])+1,sizeof(char));
            strcpy(hilfsargument,Argument[i]);
            char *knoten = knotenfinder(hilfsargument,j);
            int position = (knoten-hilfsargument)-1;
            char *gestutztesargument = calloc(strlen(Argument[i])+1,sizeof(char));
            strcpy(gestutztesargument,Argument[i]);
            for(k=i-1;k>0;k--)
            {
              char *hilfsargument2 = calloc(strlen(Argument[k])+1,sizeof(char));
              strcpy(hilfsargument2,Argument[k]);
              char *knoten2 = knotenfinder(hilfsargument2,(k<i)?knotenblock:j);
              if(knoten2[0] == 58)
              {
                loeschen = 1;
              }
              knotenblock /= AnzahlKnotenInGruppe;
              free(hilfsargument2);
            }
            if(loeschen)
            {
              for(l=0;l<strlen(knoten);l++)
              {
//                  //printf("Gestutztes Argument %d vorher: %s\n",i,gestutztesargument);
                gestutztesargument[position+l+1] = 58;
//                  //printf("Gestutztes Argument %d nachher: %s\n",i,gestutztesargument);
              }
              strcpy(Argument[i],gestutztesargument);
            }
            free(gestutztesargument);
            free(hilfsargument);
          }
        }
        for(i=ArgumentAnzahl-1;i>0;i--)
        {
          for(j=0;j<=strlen(Argument[i]);j++)
          {
            if(Argument[i][j] == 58)
            {
//              //printf("Argument %d vorher: %s\n",i,Argument[i]);
              memmove(Argument[i]+j,Argument[i]+j+1,strlen(Argument[i]+j+1));
              Argument[i][strlen(Argument[i])-1] = 0;
              j--;
//              //printf("Argument %d nachher: %s\n",i,Argument[i]);
            }
          }
        }
        for(i=ArgumentAnzahl-1;i>0;i--)
        {
          /* Anfangs-Plus-und-Kommazeichen löschen */
          j = 0;
          while(Argument[i][j] == 43 || Argument[i][j] == 44)
          {
            j++;
          }
          memmove(Argument[i],Argument[i]+j,strlen(Argument[i]+j));
          Argument[i][strlen(Argument[i])-j] = 0;
          /* End-Plus-und-Kommazeichen löschen */
          j = strlen(Argument[i])-1;
          while(Argument[i][j] == 43 || Argument[i][j] == 44)
          {
            j--;
          }
          Argument[i][j+1] = 0;
          /* Überschüssige Plus-und Kommazeichen in der Mitte löschen */
          for(j=1;j<strlen(Argument[i]);j++)
          {
            char vorgaenger = Argument[i][j-1];
            char zeichen = Argument[i][j];
            while(zeichen == 44 && vorgaenger == 43)
            {
              memmove(Argument[i]+j-1,Argument[i]+j,strlen(Argument[i]+j));
              Argument[i][strlen(Argument[i])-1] = 0;
              j--;
              vorgaenger = Argument[i][j-1];
              zeichen = Argument[i][j];
            }
            while(zeichen == 44 && vorgaenger == 44)
            {
              memmove(Argument[i]+j-1,Argument[i]+j,strlen(Argument[i]+j));
              Argument[i][strlen(Argument[i])-1] = 0;
              j--;
              vorgaenger = Argument[i][j-1];
              zeichen = Argument[i][j];
            }
            while(zeichen == 43 && vorgaenger == 43)
            {
              memmove(Argument[i]+j-1,Argument[i]+j,strlen(Argument[i]+j));
              Argument[i][strlen(Argument[i])-1] = 0;
              j--;
              vorgaenger = Argument[i][j-1];
              zeichen = Argument[i][j];
            }
            if(zeichen == 43 && vorgaenger == 44)
            {
              memmove(Argument[i]+j-1,Argument[i]+j,strlen(Argument[i]+j));
              Argument[i][j-1] = 44;
              Argument[i][strlen(Argument[i])-1] = 0;
              j--;
              vorgaenger = Argument[i][j-1];
              zeichen = Argument[i][j];
            }
          }
        }
      }
      else
      {
        for(i=ArgumentAnzahl/2-1;i>0;i--)
        {
          m=i+ArgumentAnzahl/2;
          for(j=0;j<=zeichenzaehlen(Argument[i],'+')+zeichenzaehlen(Argument[i],',');j++)
          {
            int knotennummer = j % AnzahlKnotenInGruppe, knotenblock = j/AnzahlKnotenInGruppe;
            long long int farbzaehler = urne[knotennummer];
            char *hilfsargument = calloc(strlen(Argument[i])+1,sizeof(char));
            strcpy(hilfsargument,Argument[i]);
            char *gestutztesargument = calloc(strlen(Argument[i])+1,sizeof(char));
            strcpy(gestutztesargument,Argument[i]);
            char *knoten = knotenfinder(hilfsargument,j);
            int position = (knoten - hilfsargument) - 1;

            char *wskhilfsargument = calloc(strlen(Argument[m])+1,sizeof(char));
            strcpy(wskhilfsargument,Argument[m]);
            char *wskgestutztesargument = calloc(strlen(Argument[m])+1,sizeof(char));
            strcpy(wskgestutztesargument,Argument[m]);
            char *wskknoten = knotenfinder(wskhilfsargument,j);
            int wskposition = (wskknoten - wskhilfsargument) - 1;

            for(k=i-1;k>0;k--)
            {
              char *hilfsargument2 = calloc(strlen(Argument[k])+1,sizeof(char));
              strcpy(hilfsargument2,Argument[k]);
              char *knoten2 = knotenfinder(hilfsargument2,knotenblock);
              if(!strcmp(knoten2,knoten))
              {
                farbzaehler--;
              }
              knotenblock /= AnzahlKnotenInGruppe;
              if(farbzaehler <= 0)
              {
//              //printf("Knotenlänge: %d\n",strlen(knoten));
//              //printf("gestutztesargument: %s an Adresse %p\n",gestutztesargument,gestutztesargument);
//              //printf("Position: %d\n",position);
                for(l=0;l<strlen(knoten);l++)
                {
                  gestutztesargument[position+l+1] = 58;
                }

                for(l=0;l<strlen(wskknoten);l++)
                {
                  wskgestutztesargument[wskposition+l+1] = 58;
                }

              }
              free(hilfsargument2);
            }
            strcpy(Argument[i],gestutztesargument);
            strcpy(Argument[m],wskgestutztesargument);
            free(gestutztesargument);
            free(hilfsargument);
            free(wskgestutztesargument);
            free(wskhilfsargument);
          }
//        //printf("Argument %d: %s\n Argument %d: %s\n",i,Argument[i],m,Argument[m]);
        }
        for(i=ArgumentAnzahl/2-1;i>0;i--)
        {
          m=i+ArgumentAnzahl/2;
          for(j=0;j<=zeichenzaehlen(Argument[i],'+')+zeichenzaehlen(Argument[i],',');j++)
          {
            short loeschen = 0;
            int /*knotennummer = j % AnzahlKnotenInGruppe,*/ knotenblock = j/AnzahlKnotenInGruppe;
            char *hilfsargument = calloc(strlen(Argument[i])+1,sizeof(char));
            strcpy(hilfsargument,Argument[i]);
            char *knoten = knotenfinder(hilfsargument,j);
            int position = (knoten-hilfsargument)-1;
            char *gestutztesargument = calloc(strlen(Argument[i])+1,sizeof(char));
            strcpy(gestutztesargument,Argument[i]);

            char *wskhilfsargument = calloc(strlen(Argument[m])+1,sizeof(char));
            strcpy(wskhilfsargument,Argument[m]);
            char *wskknoten = knotenfinder(wskhilfsargument,j);
            int wskposition = (wskknoten-wskhilfsargument)-1;
            char *wskgestutztesargument = calloc(strlen(Argument[m])+1,sizeof(char));
            strcpy(wskgestutztesargument,Argument[m]);

            for(k=i-1;k>0;k--)
            {
              char *hilfsargument2 = calloc(strlen(Argument[k])+1,sizeof(char));
              strcpy(hilfsargument2,Argument[k]);
              char *knoten2 = knotenfinder(hilfsargument2,(k<i)?knotenblock:j);
              if(knoten2[0] == 58)
              {
                loeschen = 1;
              }
              knotenblock /= AnzahlKnotenInGruppe;
              free(hilfsargument2);
            }

            if(loeschen)
            {
              for(l=0;l<strlen(knoten);l++)
              {
//                  //printf("Gestutztes Argument %d vorher: %s\n",i,gestutztesargument);
                gestutztesargument[position+l+1] = 58;
//                  //printf("Gestutztes Argument %d nachher: %s\n",i,gestutztesargument);
              }
              strcpy(Argument[i],gestutztesargument);

              for(l=0;l<strlen(wskknoten);l++)
              {
//                  //printf("Knotenlänge: %d\nGestutztes Argument %d vorher: %s\n",strlen(wskknoten),m,wskgestutztesargument);
                wskgestutztesargument[wskposition+l+1] = 58;
//                  //printf("Gestutztes Argument %d nachher: %s\n",m,wskgestutztesargument);
              }
              strcpy(Argument[m],wskgestutztesargument);

            }
            free(gestutztesargument);
            free(hilfsargument);
            free(wskgestutztesargument);
            free(wskhilfsargument);
          }
        }
        for(i=ArgumentAnzahl-1;i>0;i--)
        {
          for(j=0;j<=strlen(Argument[i]);j++)
          {
            if(Argument[i][j] == 58)
            {
//              //printf("Argument %d vorher: %s\n",i,Argument[i]);
              memmove(Argument[i]+j,Argument[i]+j+1,strlen(Argument[i]+j+1));
              Argument[i][strlen(Argument[i])-1] = 0;
              j--;
//              //printf("Argument %d nachher: %s\n",i,Argument[i]);
            }
          }
        }
        for(i=ArgumentAnzahl-1;i>0;i--)
        {
          /* Anfangs-Plus-und-Kommazeichen löschen */
          j = 0;
          while(Argument[i][j] == 43 || Argument[i][j] == 44)
          {
            j++;
          }
          memmove(Argument[i],Argument[i]+j,strlen(Argument[i]+j));
          Argument[i][strlen(Argument[i])-j] = 0;
          /* End-Plus-und-Kommazeichen löschen */
          j = strlen(Argument[i])-1;
          while(Argument[i][j] == 43 || Argument[i][j] == 44)
          {
            j--;
          }
          Argument[i][j+1] = 0;
          /* Überschüssige Plus-und Kommazeichen in der Mitte löschen */
          for(j=1;j<strlen(Argument[i]);j++)
          {
            char vorgaenger = Argument[i][j-1];
            char zeichen = Argument[i][j];
            while(zeichen == 44 && vorgaenger == 43)
            {
              memmove(Argument[i]+j-1,Argument[i]+j,strlen(Argument[i]+j));
              Argument[i][strlen(Argument[i])-1] = 0;
              j--;
              vorgaenger = Argument[i][j-1];
              zeichen = Argument[i][j];
            }
            while(zeichen == 44 && vorgaenger == 44)
            {
              memmove(Argument[i]+j-1,Argument[i]+j,strlen(Argument[i]+j));
              Argument[i][strlen(Argument[i])-1] = 0;
              j--;
              vorgaenger = Argument[i][j-1];
              zeichen = Argument[i][j];
            }
            while(zeichen == 43 && vorgaenger == 43)
            {
              memmove(Argument[i]+j-1,Argument[i]+j,strlen(Argument[i]+j));
              Argument[i][strlen(Argument[i])-1] = 0;
              j--;
              vorgaenger = Argument[i][j-1];
              zeichen = Argument[i][j];
            }
            if(zeichen == 43 && vorgaenger == 44)
            {
              memmove(Argument[i]+j-1,Argument[i]+j,strlen(Argument[i]+j));
              Argument[i][j-1] = 44;
              Argument[i][strlen(Argument[i])-1] = 0;
              j--;
              vorgaenger = Argument[i][j-1];
              zeichen = Argument[i][j];
            }
          }
        }
      }
    }
    free(urne);
  }


  /* Ziehen aus einer Urne mit Dazulegen. Syntax: baum d 5 R 50 G 30 W 20 ... p */

  if((*Argument[1] == 68 || *Argument[1] == 100) && strlen(Argument[1]) == 1 && ArgumentAnzahl > 3 && atoi(Argument[2]) != 0 && urnefail)
  {
    unsigned long int *gesamtzahlkugeln, *urne;
    int anzahlstufen = atoi(Argument[2]);
    int anzahlfarben = (ArgumentAnzahl-3)/2;
    gesamtzahlkugeln = calloc(anzahlstufen,sizeof(unsigned long int));
    urne = calloc(anzahlfarben,sizeof(unsigned long int));
    for(i=4;i<ArgumentAnzahl;i+=2)
    {
      gesamtzahlkugeln[0] += atol(Argument[i]);
      urne[(i-4)/2] = atol(Argument[i]);
    }
    for(i=1;i<anzahlstufen;i++)
    {
      gesamtzahlkugeln[i]=gesamtzahlkugeln[0]+i;  // genauso möglich wäre gesamtzahlkugeln[i]=gesamtzahlkugeln[i-1]+1
    }
    char *knotenargument = calloc((MaximaleLaengeKnoten+1)*anzahlfarben+1,sizeof(char));
    strcpy(knotenargument,Argument[3]);
    for(i=5;i<ArgumentAnzahl-1;i+=2)
    {
      strcat(knotenargument,"+");
      strcat(knotenargument,Argument[i]);
    }
    short wsk = 0;
    char ***wskargument;
    if(*Argument[ArgumentAnzahl-1] == 80 || *Argument[ArgumentAnzahl-1] == 112)
    {
      wsk = 1;
      char **gesamtzahlkugelnstring;

      gesamtzahlkugelnstring = (char **)calloc(200,sizeof(char**));
      for(j=0; j < 200; j++)
      {
        gesamtzahlkugelnstring[j] = (char *)calloc(50,sizeof(char));
      }

      for(i=0;i<anzahlstufen;i++)
      {
        sprintf(gesamtzahlkugelnstring[i],"%lu",gesamtzahlkugeln[i]);
        //printf("gesamtzahlkugelnstring[%i]: %s\n",i,gesamtzahlkugelnstring[i]);
      }

      wskargument = (char ***)calloc(anzahlstufen+1,sizeof(char**));
      for(j=0; j < anzahlstufen+1; j++)
      {
        wskargument[j] = (char **)calloc(potenziere(anzahlfarben,anzahlstufen)+1,sizeof(char*));
        for(k=0; k < potenziere(anzahlfarben,anzahlstufen)+1; k++)
        {
          wskargument[j][k] = (char *)calloc(50*anzahlfarben+1,sizeof(char));
        }
      }

      for(k=0;k<anzahlstufen;k++)
      {
        for(j=0;j<potenziere(anzahlfarben,k);j++)
        {
          int anzahlfarbkugeln = atoi(Argument[4]);
          int knotenblocknummer;
          knotenblocknummer=j*anzahlfarben;
          for(l=k-1;l>=0;l--)
          {
            if(0 == (knotenblocknummer / anzahlfarben) % anzahlfarben)
            {
              anzahlfarbkugeln += 1;
            }
            knotenblocknummer /= anzahlfarben;
          }
          if(anzahlfarbkugeln < 0)
          {
            anzahlfarbkugeln=0;
          }
          char anzahlfarbkugelnstring[300];
          sprintf(anzahlfarbkugelnstring,"%d",anzahlfarbkugeln);
          strcat(wskargument[k][j],anzahlfarbkugelnstring);
          strcat(wskargument[k][j],"/");
          strcat(wskargument[k][j],gesamtzahlkugelnstring[k]);
          for(i=6;i<ArgumentAnzahl;i+=2)
          {
            anzahlfarbkugeln = atoi(Argument[i]);
            knotenblocknummer=j*anzahlfarben+i/2-2;
            for(l=k-1;l>=0;l--)
            {
              if((j*anzahlfarben+i/2-2) % anzahlfarben == (knotenblocknummer / anzahlfarben) % anzahlfarben)
              {
                anzahlfarbkugeln += 1;
              }
              knotenblocknummer /= anzahlfarben;
            }
            if(anzahlfarbkugeln < 0)
            {
              anzahlfarbkugeln=0;
            }
            strcat(wskargument[k][j],"+");
            sprintf(anzahlfarbkugelnstring,"%i",anzahlfarbkugeln);
            strcat(wskargument[k][j],anzahlfarbkugelnstring);
            strcat(wskargument[k][j],"/");
            strcat(wskargument[k][j],gesamtzahlkugelnstring[k]);
          }
          //printf("wskargument[%i][%i]=%s\n",k,j,wskargument[k][j]);
        }
      }
      for(j=0; j < 200; j++)
      {
        free(gesamtzahlkugelnstring[j]);
      }
      free(gesamtzahlkugelnstring);
    }
    free(gesamtzahlkugeln);
    multinomial = 1;

    char *knotengruppe = calloc(strlen(knotenargument)+1,sizeof(char));
    strcpy(knotengruppe,knotenargument);
    int AnzahlKnotenInGruppe = zeichenzaehlen(knotengruppe,43)+1;
    ArgumentAnzahl = anzahlstufen + 1;
    Argument = (char **)calloc((wsk)?2*ArgumentAnzahl:ArgumentAnzahl+1,sizeof(char *));
    for(i=0;i<ArgumentAnzahl;i++)
    {
      Argument[i] = (char *)calloc((MaximaleLaengeKnoten+1)*potenziere(AnzahlKnotenInGruppe,i)+i+1,sizeof(char));
    }
    if(wsk)
    {
      for(i=ArgumentAnzahl;i<2*ArgumentAnzahl;i++)
      {
        Argument[i] = (char *)calloc(202*potenziere(AnzahlKnotenInGruppe,i-ArgumentAnzahl)+i-ArgumentAnzahl+1,sizeof(char));
      }
    }
    for(j = 1;j <= anzahlstufen;j++)
    {
      strcpy(Argument[j],knotengruppe);
      for(i = 1;i < potenziere(AnzahlKnotenInGruppe,j-1);i++)
      {
        strcat(Argument[j],",");
        strcat(Argument[j],knotengruppe);
      }
    }
    if(wsk)
    {
      *Argument[ArgumentAnzahl] = 80;
      for(j = ArgumentAnzahl+1;j <= ArgumentAnzahl + anzahlstufen;j++)
      {
        strcpy(Argument[j],wskargument[j-ArgumentAnzahl-1][0]);
        for(i = 1;i < potenziere(AnzahlKnotenInGruppe,j-ArgumentAnzahl-1);i++)
        {
          strcat(Argument[j],",");
          strcat(Argument[j],wskargument[j-ArgumentAnzahl-1][i]);
        }
      }
      ArgumentAnzahl *= 2;
    }
    free(knotengruppe);
    if(wsk)
    {
      for(j=0; j < anzahlstufen+1; j++)
      {
        for(k=0; k < potenziere(anzahlfarben,anzahlstufen)+1; k++)
        {
          free(wskargument[j][k]);
        }
        free(wskargument[j]);
      }
      free(wskargument);
    }
    free(knotenargument);
    free(urne);
  }




  /* Deklaration später verwendeter Variablen */

  int AnzahlKnoten = 0;
  float x = 0, y = 0, svgx = 0, svgy = 0;                      /* Koordinaten der Zeichnungsobjekte */
  short wsk = 0;                             /* Wahrscheinlichkeiten werden nicht angezeigt, außer wsk wird auf 1 gesetzt */
  char WahrscheinlichkeitsString[9];

  if((*Argument[ArgumentAnzahl/2] == 80 || *Argument[ArgumentAnzahl/2] == 112) && strlen(Argument[ArgumentAnzahl/2]) == 1 && ArgumentAnzahl > 2 && ArgumentAnzahl % 2 == 0)
  {
    wsk = 1;
    ArgumentAnzahl /= 2;
  }

  int AnzahlKnotenLetzteStufe = zeichenzaehlen(Argument[ArgumentAnzahl-1],'+')+zeichenzaehlen(Argument[ArgumentAnzahl-1],',')+1;

  int *AnzahlKnotenStufe;
  AnzahlKnotenStufe = calloc(ArgumentAnzahl+1,sizeof(int));
  int MaximaleAnzahlKnotenProStufe = 0;
  for(i=1;i<ArgumentAnzahl;i++)
  {
    AnzahlKnotenStufe[i] = zeichenzaehlen(Argument[i],'+')+zeichenzaehlen(Argument[i],',')+1;
    if(MaximaleAnzahlKnotenProStufe < AnzahlKnotenStufe[i])
    {
      MaximaleAnzahlKnotenProStufe = AnzahlKnotenStufe[i]+1;
    }
    //printf("ANZAHLKNOTEN%i: %i\nMAXIMALEANZAHL: %i\n",i,AnzahlKnotenStufe[i],MaximaleAnzahlKnotenProStufe);
  }


  int ii=0;
  for(ii=0;ii<ArgumentAnzahl;ii++)
  {
    //printf("Argument[%i] = %s\n",ii,Argument[ii]);
  }


  /* Nochmalige Bestimmung der maximalen Knotenlänge, die vorkommt */

  GesamtLaengeArgument = 0;
  for(i=1;i<ArgumentAnzahl;i++)
  {
    GesamtLaengeArgument += strlen(Argument[i]) + 1;
  }
  GesamtArgument = calloc(GesamtLaengeArgument+1,sizeof(char));
  for(i=1;i<ArgumentAnzahl;i++)
  {
    strcat(GesamtArgument, Argument[i]);
    strcat(GesamtArgument, " ");
  }
  //printf("GesamtArgument beim zweiten mal: %s\nGesamtlaenge %i\nargc %i\n",GesamtArgument,GesamtLaengeArgument,ArgumentAnzahl);

  MaximaleLaengeKnoten = 0;
  argzeiger = strtok(GesamtArgument,"+, ");
  while(argzeiger != NULL)
  {
    char *hilfszeiger = calloc(strlen(argzeiger)+1,sizeof(char));
    strcpy(hilfszeiger, argzeiger);
    //printf("argzeiger: %s\nargzeigerlänge: %i\ntildezahl %i\nzeichenanzahl minus anzahl tildezeichen: %i\nMAXKNOTEN: %i\n",argzeiger,(int)strlen(argzeiger),zeichenzaehlen(argzeiger,126),(int)(strlen(argzeiger)-zeichenzaehlen(argzeiger,126)),MaximaleLaengeKnoten);
    if(MaximaleLaengeKnoten < (int)(strlen(argzeiger))-zeichenzaehlen(hilfszeiger,126))
    {
      //printf("argzeigerlänge: %i\ttildezeichen: %i\n",strlen(argzeiger),zeichenzaehlen(hilfszeiger,126));
      MaximaleLaengeKnoten = (int)(strlen(argzeiger))-zeichenzaehlen(hilfszeiger,126);
      //printf("MAXKNOTEN: %i\n",MaximaleLaengeKnoten);
    }
    free(hilfszeiger);
    argzeiger = strtok(NULL,"+, ");
  }
  free(GesamtArgument);

  /* Bestimmung der längsten Wahrscheinlichkeit, die vorkommt */

  int GesamtLaengeWskArgument = 0;
  char *GesamtArgumentWsk;
  int laengsteWahrscheinlichkeit = 0;
  if(wsk)
  {
    for(i=ArgumentAnzahl+1;i<2*ArgumentAnzahl;i++)
    {
      GesamtLaengeWskArgument += strlen(Argument[i]) + 1;
    }
    GesamtArgumentWsk = calloc(GesamtLaengeWskArgument+1,sizeof(char));
    for(i=ArgumentAnzahl+1;i<2*ArgumentAnzahl;i++)
    {
      strcat(GesamtArgumentWsk, Argument[i]);
      strcat(GesamtArgumentWsk, " ");
    }
    //printf("GesamtArgumentWsk: %s\n",GesamtArgumentWsk);

    argzeiger = strtok(GesamtArgumentWsk,"+, ");
    while(argzeiger != NULL)
    {
      //printf("argzeiger: %s\n",argzeiger);
      if(laengsteWahrscheinlichkeit < strlen(argzeiger))
      {
        laengsteWahrscheinlichkeit = strlen(argzeiger);
        //printf("MAXKNOTEN: %i\n",MaximaleLaengeKnoten);
      }
      argzeiger = strtok(NULL,"+, ");
    }
    free(GesamtArgumentWsk);
  }


  if(!wsk)
  {
    if(odt)
    {
      ErgebnisSpalteWskAnzeigen = 0;
    }
    if(svg)
    {
      svgErgebnisSpalteWskAnzeigen = 0;
    }
  }

  if(odt)
  {
    if(ErgebnisBreite == 0)
    {
      ErgebnisBreite = (ArgumentAnzahl - 1) * MaximaleLaengeKnoten * schriftbreite(SchriftartErgebnis) * SchriftgroesseErgebnis * 4/3 * PXINCM;
    }
    if(KnotenBreite == 0)
    {
      KnotenBreite = MaximaleLaengeKnoten * schriftbreite(SchriftartKnoten) * SchriftgroesseKnoten * 4/3 * PXINCM;
    }
    if(KnotenHoehe == 0)
    {
      KnotenHoehe = SchriftgroesseKnoten * 4/3 * PXINCM;
    }
  }

  if(svg)
  {
    if(svgErgebnisBreite == 0)
    {
      //printf("MaximaleLaengeKnoten %d\n",MaximaleLaengeKnoten);
      svgErgebnisBreite = (ArgumentAnzahl - 1) * MaximaleLaengeKnoten * schriftbreite(svgSchriftartErgebnis) * svgSchriftgroesseErgebnis * 4/3 * PXINCM /*svgKnotenBreite*/;
    }
    if(svgKnotenBreite == 0)
    {
      svgKnotenBreite = MaximaleLaengeKnoten * schriftbreite(svgSchriftartKnoten) * svgSchriftgroesseKnoten * 4/3 * PXINCM;
    }
    if(svgKnotenHoehe == 0)
    {
      svgKnotenHoehe = svgSchriftgroesseKnoten * 4/3 * PXINCM;
    }
  }

  float FormelHoeheErgebnis, svgFormelHoeheErgebnis;

  if(odt)
  {
    FormelHoeheErgebnis = (float)SchriftgroesseWskErgebnis / 12;
  }

  if(svg)
  {
    svgFormelHoeheErgebnis = (float)svgSchriftgroesseWskErgebnis / 12;
  }

  if(odt)
  {
    if(FormelHoeheErgebnis > KnotenHoehe + VertikalerKnotenAbstand)
    {
      FormelHoeheErgebnis = KnotenHoehe + VertikalerKnotenAbstand;
      SchriftgroesseWskErgebnis = 12 * FormelHoeheErgebnis;
    }
  }

  if(svg)
  {
    if(svgFormelHoeheErgebnis > svgKnotenHoehe + svgVertikalerKnotenAbstand)
    {
      svgFormelHoeheErgebnis = svgKnotenHoehe + svgVertikalerKnotenAbstand;
      svgSchriftgroesseWskErgebnis = 12 * svgFormelHoeheErgebnis;
    }
  }

  float FormelHoeheZweig;

  if(odt)
  {
    FormelHoeheZweig = (float)SchriftgroesseWskErgebnis / 12;
  }

  if(odt)
  {
    switch(Optimierung)
    {
      case 1:
        SeitenHoehe = 29.7;
        SeitenBreite = 21;
        if(!BruchErgebnisFormel)
        {
          if(ErgebnisSpalteAnzeigen)
          {
            VertikalerKnotenAbstand = (29.7 - SeitenRandOben - SeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * KnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
          }
          else
          {
            VertikalerKnotenAbstand = (29.7 - SeitenRandOben - SeitenRandUnten - AnzahlKnotenLetzteStufe * KnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
          }
        }
        else
        {
          if(ErgebnisSpalteWskAnzeigen)
          {
            VertikalerKnotenAbstand = (29.7 - SeitenRandOben - SeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * KnotenHoehe - 0.5 - (FormelHoeheErgebnis - KnotenHoehe)) / (AnzahlKnotenLetzteStufe - 1);
          }
          else
          {
            if(ErgebnisSpalteAnzeigen)
            {
              VertikalerKnotenAbstand = (29.7 - SeitenRandOben - SeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * KnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
            }
            else
            {
              VertikalerKnotenAbstand = (29.7 - SeitenRandOben - SeitenRandUnten - AnzahlKnotenLetzteStufe * KnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
            }
          }
        }
        if(ErgebnisSpalteAnzeigen)
        {
          if(ErgebnisSpalteWskAnzeigen)
          {
            HorizontalerKnotenAbstand= (21 - SeitenRandLinks - SeitenRandRechts - ErgebnisBreite - WskBreite - 1 - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
          }
          else
          {
            HorizontalerKnotenAbstand= (21 - SeitenRandLinks - SeitenRandRechts - ErgebnisBreite - 0.5 - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
          }
        }
        else
        {
          if(ErgebnisSpalteWskAnzeigen)
          {
            HorizontalerKnotenAbstand= (21 - SeitenRandLinks - SeitenRandRechts - WskBreite - 0.5 - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
          }
          else
          {
            HorizontalerKnotenAbstand= (21 - SeitenRandLinks - SeitenRandRechts - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
          }
        }
        break;
      case 2:
        SeitenHoehe = 21;
        SeitenBreite = 29.7;
        if(!BruchErgebnisFormel)
        {
          if(ErgebnisSpalteAnzeigen)
          {
            VertikalerKnotenAbstand = (21 - SeitenRandOben - SeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * KnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
          }
          else
          {
            VertikalerKnotenAbstand = (21 - SeitenRandOben - SeitenRandUnten - AnzahlKnotenLetzteStufe * KnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
          }
        }
        else
        {
          if(ErgebnisSpalteWskAnzeigen)
          {
            VertikalerKnotenAbstand = ( 21 - SeitenRandOben - SeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * KnotenHoehe - 0.5 - (FormelHoeheErgebnis - KnotenHoehe)) / (AnzahlKnotenLetzteStufe - 1);
          }
          else
          {
            if(ErgebnisSpalteAnzeigen)
            {
              VertikalerKnotenAbstand = (21 - SeitenRandOben - SeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * KnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
            }
            else
            {
              VertikalerKnotenAbstand = (21 - SeitenRandOben - SeitenRandUnten - AnzahlKnotenLetzteStufe * KnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
            }
          }
        }
        if(ErgebnisSpalteAnzeigen)
        {
          if(ErgebnisSpalteWskAnzeigen)
          {
            HorizontalerKnotenAbstand= (29.7 - SeitenRandLinks - SeitenRandRechts - ErgebnisBreite - WskBreite - 1 - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
          }
          else
          {
            HorizontalerKnotenAbstand= (29.7 - SeitenRandLinks - SeitenRandRechts - ErgebnisBreite - 0.5 - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
          }
        }
        else
        {
          if(ErgebnisSpalteWskAnzeigen)
          {
            HorizontalerKnotenAbstand= (29.7 - SeitenRandLinks - SeitenRandRechts - WskBreite - 0.5 - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
          }
          else
          {
            HorizontalerKnotenAbstand= (29.7 - SeitenRandLinks - SeitenRandRechts - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
          }
        }
        break;
      case 3:
        SeitenHoehe = 42;
        SeitenBreite = 29.7;
        if(!BruchErgebnisFormel)
        {
          if(ErgebnisSpalteAnzeigen)
          {
            VertikalerKnotenAbstand = (42 - SeitenRandOben - SeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * KnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
          }
          else
          {
            VertikalerKnotenAbstand = (42 - SeitenRandOben - SeitenRandUnten - AnzahlKnotenLetzteStufe * KnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
          }
        }
        else
        {
          if(ErgebnisSpalteWskAnzeigen)
          {
            VertikalerKnotenAbstand = (42 - SeitenRandOben - SeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * KnotenHoehe - 0.5 - (FormelHoeheErgebnis - KnotenHoehe)) / (AnzahlKnotenLetzteStufe - 1);
          }
          else
          {
            if(ErgebnisSpalteAnzeigen)
            {
              VertikalerKnotenAbstand = (42 - SeitenRandOben - SeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * KnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
            }
            else
            {
              VertikalerKnotenAbstand = (42 - SeitenRandOben - SeitenRandUnten - AnzahlKnotenLetzteStufe * KnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
            }
          }
        }
        if(ErgebnisSpalteAnzeigen)
        {
          if(ErgebnisSpalteWskAnzeigen)
          {
            HorizontalerKnotenAbstand= (29.7 - SeitenRandLinks - SeitenRandRechts - ErgebnisBreite - WskBreite - 1 - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
          }
          else
          {
            HorizontalerKnotenAbstand= (29.7 - SeitenRandLinks - SeitenRandRechts - ErgebnisBreite - 0.5 - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
          }
        }
        else
        {
          if(ErgebnisSpalteWskAnzeigen)
          {
            HorizontalerKnotenAbstand= (29.7 - SeitenRandLinks - SeitenRandRechts - WskBreite - 0.5 - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
          }
          else
          {
            HorizontalerKnotenAbstand= (29.7 - SeitenRandLinks - SeitenRandRechts - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
          }
        }
        break;
      case 4:
        SeitenHoehe = 29.7;
        SeitenBreite = 42;
        if(!BruchErgebnisFormel)
        {
          if(ErgebnisSpalteAnzeigen)
          {
            VertikalerKnotenAbstand = (29.7 - SeitenRandOben - SeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * KnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
          }
          else
          {
            VertikalerKnotenAbstand = (29.7 - SeitenRandOben - SeitenRandUnten - AnzahlKnotenLetzteStufe * KnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
          }
        }
        else
        {
          if(ErgebnisSpalteWskAnzeigen)
          {
            VertikalerKnotenAbstand = ( 29.7 - SeitenRandOben - SeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * KnotenHoehe - 0.5 - (FormelHoeheErgebnis - KnotenHoehe)) / (AnzahlKnotenLetzteStufe - 1);
          }
          else
          {
            if(ErgebnisSpalteAnzeigen)
            {
              VertikalerKnotenAbstand = (29.7 - SeitenRandOben - SeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * KnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
            }
            else
            {
              VertikalerKnotenAbstand = (29.7 - SeitenRandOben - SeitenRandUnten - AnzahlKnotenLetzteStufe * KnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
            }
          }
        }
        if(ErgebnisSpalteAnzeigen)
        {
          if(ErgebnisSpalteWskAnzeigen)
          {
            HorizontalerKnotenAbstand= (42 - SeitenRandLinks - SeitenRandRechts - ErgebnisBreite - WskBreite - 1 - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
          }
          else
          {
            HorizontalerKnotenAbstand= (42 - SeitenRandLinks - SeitenRandRechts - ErgebnisBreite - 0.5 - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
          }
        }
        else
        {
          if(ErgebnisSpalteWskAnzeigen)
          {
            HorizontalerKnotenAbstand= (42 - SeitenRandLinks - SeitenRandRechts - WskBreite - 0.5 - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
          }
          else
          {
            HorizontalerKnotenAbstand= (42 - SeitenRandLinks - SeitenRandRechts - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
          }
        }
        break;
      default:
        break;
    }
  }

  if(svg)
  {
    switch(svgOptimierung)
    {
      case 1:
        svgSeitenHoehe = 29.7;
        svgSeitenBreite = 21;
        if(!svgBruchDarstellungErgebnis)
        {
          if(svgErgebnisSpalteAnzeigen)
          {
            svgVertikalerKnotenAbstand = (29.7 - svgSeitenRandOben - svgSeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * svgKnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
          }
          else
          {
            svgVertikalerKnotenAbstand = (29.7 - svgSeitenRandOben - svgSeitenRandUnten - AnzahlKnotenLetzteStufe * svgKnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
          }
        }
        else
        {
          if(svgErgebnisSpalteWskAnzeigen)
          {
            svgVertikalerKnotenAbstand = (29.7 - svgSeitenRandOben - svgSeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * svgKnotenHoehe - 0.5 - (svgFormelHoeheErgebnis - svgKnotenHoehe)) / (AnzahlKnotenLetzteStufe - 1);
          }
          else
          {
            if(svgErgebnisSpalteAnzeigen)
            {
              svgVertikalerKnotenAbstand = (29.7 - svgSeitenRandOben - svgSeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * svgKnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
            }
            else
            {
              svgVertikalerKnotenAbstand = (29.7 - svgSeitenRandOben - svgSeitenRandUnten - AnzahlKnotenLetzteStufe * svgKnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
            }
          }
        }
        if(svgErgebnisSpalteAnzeigen)
        {
          if(svgErgebnisSpalteWskAnzeigen)
          {
            svgHorizontalerKnotenAbstand= (21 - svgSeitenRandLinks - svgSeitenRandRechts - svgErgebnisBreite - svgWskBreite - 1 - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
          }
          else
          {
            svgHorizontalerKnotenAbstand= (21 - svgSeitenRandLinks - svgSeitenRandRechts - svgErgebnisBreite - 0.5 - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
          }
        }
        else
        {
          if(svgErgebnisSpalteWskAnzeigen)
          {
            svgHorizontalerKnotenAbstand= (21 - svgSeitenRandLinks - svgSeitenRandRechts - svgWskBreite - 0.5 - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
          }
          else
          {
            svgHorizontalerKnotenAbstand= (21 - svgSeitenRandLinks - svgSeitenRandRechts - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
          }
        }
        break;
      case 2:
        svgSeitenHoehe = 21;
        svgSeitenBreite = 29.7;
        if(!svgBruchDarstellungErgebnis)
        {
          if(svgErgebnisSpalteAnzeigen)
          {
            svgVertikalerKnotenAbstand = (21 - svgSeitenRandOben - svgSeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * svgKnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
          }
          else
          {
            svgVertikalerKnotenAbstand = (21 - svgSeitenRandOben - svgSeitenRandUnten - AnzahlKnotenLetzteStufe * svgKnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
          }
        }
        else
        {
          if(svgErgebnisSpalteWskAnzeigen)
          {
            svgVertikalerKnotenAbstand = (21 - svgSeitenRandOben - svgSeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * svgKnotenHoehe - 0.5 - (svgFormelHoeheErgebnis - svgKnotenHoehe)) / (AnzahlKnotenLetzteStufe - 1);
          }
          else
          {
            if(svgErgebnisSpalteAnzeigen)
            {
              svgVertikalerKnotenAbstand = (21 - svgSeitenRandOben - svgSeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * svgKnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
            }
            else
            {
              svgVertikalerKnotenAbstand = (21 - svgSeitenRandOben - svgSeitenRandUnten - AnzahlKnotenLetzteStufe * svgKnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
            }
          }
        }
        if(svgErgebnisSpalteAnzeigen)
        {
          if(svgErgebnisSpalteWskAnzeigen)
          {
            svgHorizontalerKnotenAbstand= (29.7 - svgSeitenRandLinks - svgSeitenRandRechts - svgErgebnisBreite - svgWskBreite - 1 - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
          }
          else
          {
            svgHorizontalerKnotenAbstand= (29.7 - svgSeitenRandLinks - svgSeitenRandRechts - svgErgebnisBreite - 0.5 - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
          }
        }
        else
        {
          if(svgErgebnisSpalteWskAnzeigen)
          {
            svgHorizontalerKnotenAbstand= (29.7 - svgSeitenRandLinks - svgSeitenRandRechts - svgWskBreite - 0.5 - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
          }
          else
          {
            svgHorizontalerKnotenAbstand= (29.7 - svgSeitenRandLinks - svgSeitenRandRechts - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
          }
        }
        break;
      case 3:
        svgSeitenHoehe = 42;
        svgSeitenBreite = 29.7;
        if(!svgBruchDarstellungErgebnis)
        {
          if(svgErgebnisSpalteAnzeigen)
          {
            svgVertikalerKnotenAbstand = (42 - svgSeitenRandOben - svgSeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * svgKnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
          }
          else
          {
            svgVertikalerKnotenAbstand = (42 - svgSeitenRandOben - svgSeitenRandUnten - AnzahlKnotenLetzteStufe * svgKnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
          }
        }
        else
        {
          if(svgErgebnisSpalteWskAnzeigen)
          {
            svgVertikalerKnotenAbstand = (42 - svgSeitenRandOben - svgSeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * svgKnotenHoehe - 0.5 - (svgFormelHoeheErgebnis - svgKnotenHoehe)) / (AnzahlKnotenLetzteStufe - 1);
          }
          else
          {
            if(svgErgebnisSpalteAnzeigen)
            {
              svgVertikalerKnotenAbstand = (42 - svgSeitenRandOben - svgSeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * svgKnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
            }
            else
            {
              svgVertikalerKnotenAbstand = (42 - svgSeitenRandOben - svgSeitenRandUnten - AnzahlKnotenLetzteStufe * svgKnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
            }
          }
        }
        if(svgErgebnisSpalteAnzeigen)
        {
          if(svgErgebnisSpalteWskAnzeigen)
          {
            svgHorizontalerKnotenAbstand= (29.7 - svgSeitenRandLinks - svgSeitenRandRechts - svgErgebnisBreite - svgWskBreite - 1 - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
          }
          else
          {
            svgHorizontalerKnotenAbstand= (29.7 - svgSeitenRandLinks - svgSeitenRandRechts - svgErgebnisBreite - 0.5 - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
          }
        }
        else
        {
          if(svgErgebnisSpalteWskAnzeigen)
          {
            svgHorizontalerKnotenAbstand= (29.7 - svgSeitenRandLinks - svgSeitenRandRechts - svgWskBreite - 0.5 - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
          }
          else
          {
            svgHorizontalerKnotenAbstand= (29.7 - svgSeitenRandLinks - svgSeitenRandRechts - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
          }
        }
        break;
      case 4:
        svgSeitenHoehe = 29.7;
        svgSeitenBreite = 42;
        if(!svgBruchDarstellungErgebnis)
        {
          if(svgErgebnisSpalteAnzeigen)
          {
            svgVertikalerKnotenAbstand = (29.7 - svgSeitenRandOben - svgSeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * svgKnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
          }
          else
          {
            svgVertikalerKnotenAbstand = (29.7 - svgSeitenRandOben - svgSeitenRandUnten - AnzahlKnotenLetzteStufe * svgKnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
          }
        }
        else
        {
          if(svgErgebnisSpalteWskAnzeigen)
          {
            svgVertikalerKnotenAbstand = ( 25.7 - (AnzahlKnotenLetzteStufe + 1) * svgKnotenHoehe - 0.5 - (svgFormelHoeheErgebnis - svgKnotenHoehe)) / (AnzahlKnotenLetzteStufe - 1);
          }
          else
          {
            if(svgErgebnisSpalteAnzeigen)
            {
              svgVertikalerKnotenAbstand = (29.7 - svgSeitenRandOben - svgSeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * svgKnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
            }
            else
            {
              svgVertikalerKnotenAbstand = (29.7 - svgSeitenRandOben - svgSeitenRandUnten - AnzahlKnotenLetzteStufe * svgKnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
            }
          }
        }
        if(svgErgebnisSpalteAnzeigen)
        {
          if(svgErgebnisSpalteWskAnzeigen)
          {
            svgHorizontalerKnotenAbstand= (42 - svgSeitenRandLinks - svgSeitenRandRechts - svgErgebnisBreite - svgWskBreite - 1 - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
          }
          else
          {
            svgHorizontalerKnotenAbstand= (42 - svgSeitenRandLinks - svgSeitenRandRechts - svgErgebnisBreite - 0.5 - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
          }
        }
        else
        {
          if(svgErgebnisSpalteWskAnzeigen)
          {
            svgHorizontalerKnotenAbstand= (42 - svgSeitenRandLinks - svgSeitenRandRechts - svgWskBreite - 0.5 - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
          }
          else
          {
            svgHorizontalerKnotenAbstand= (42 - svgSeitenRandLinks - svgSeitenRandRechts - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
          }
        }
        break;
      default:
        break;
    }
  }

  if(odt)
  {
    if(FormelHoeheErgebnis > KnotenHoehe + VertikalerKnotenAbstand)
    {
      FormelHoeheErgebnis = KnotenHoehe + VertikalerKnotenAbstand;
      SchriftgroesseWskErgebnis = 12 * FormelHoeheErgebnis;
      switch(Optimierung)
      {
        case 1:
          SeitenHoehe = 29.7;
          SeitenBreite = 21;
          if(!BruchErgebnisFormel)
          {
            if(ErgebnisSpalteAnzeigen)
            {
              VertikalerKnotenAbstand = (29.7 - SeitenRandOben - SeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * KnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
            }
            else
            {
              VertikalerKnotenAbstand = (29.7 - SeitenRandOben - SeitenRandUnten - AnzahlKnotenLetzteStufe * KnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
            }
          }
          else
          {
            if(ErgebnisSpalteWskAnzeigen)
            {
              VertikalerKnotenAbstand = (29.7 - SeitenRandOben - SeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * KnotenHoehe - 0.5 - (FormelHoeheErgebnis - KnotenHoehe)) / (AnzahlKnotenLetzteStufe - 1);
            }
            else
            {
              if(ErgebnisSpalteAnzeigen)
              {
                VertikalerKnotenAbstand = (29.7 - SeitenRandOben - SeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * KnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
              }
              else
              {
                VertikalerKnotenAbstand = (29.7 - SeitenRandOben - SeitenRandUnten - AnzahlKnotenLetzteStufe * KnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
              }
            }
          }
          if(ErgebnisSpalteAnzeigen)
          {
            if(ErgebnisSpalteWskAnzeigen)
            {
              HorizontalerKnotenAbstand= (21 - SeitenRandLinks - SeitenRandRechts - ErgebnisBreite - WskBreite - 1 - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
            }
            else
            {
              HorizontalerKnotenAbstand= (21 - SeitenRandLinks - SeitenRandRechts - ErgebnisBreite - 0.5 - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
            }
          }
          else
          {
            if(ErgebnisSpalteWskAnzeigen)
            {
              HorizontalerKnotenAbstand= (21 - SeitenRandLinks - SeitenRandRechts - WskBreite - 0.5 - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
            }
            else
            {
              HorizontalerKnotenAbstand= (21 - SeitenRandLinks - SeitenRandRechts - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
            }
          }
          break;
        case 2:
          SeitenHoehe = 21;
          SeitenBreite = 29.7;
          if(!BruchErgebnisFormel)
          {
            if(ErgebnisSpalteAnzeigen)
            {
              VertikalerKnotenAbstand= (21 - SeitenRandOben - SeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * KnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
            }
            else
            {
              VertikalerKnotenAbstand= (21 - SeitenRandOben - SeitenRandUnten - AnzahlKnotenLetzteStufe * KnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
            }
          }
          else
          {
            if(ErgebnisSpalteWskAnzeigen)
            {
              VertikalerKnotenAbstand= (21 - SeitenRandOben - SeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * KnotenHoehe - 0.5 - (FormelHoeheErgebnis - KnotenHoehe)) / (AnzahlKnotenLetzteStufe - 1);
            }
            else
            {
              if(ErgebnisSpalteAnzeigen)
              {
                VertikalerKnotenAbstand= (21 - SeitenRandOben - SeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * KnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
              }
              else
              {
                VertikalerKnotenAbstand= (21 - SeitenRandOben - SeitenRandUnten - AnzahlKnotenLetzteStufe * KnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
              }
            }
          }
          if(ErgebnisSpalteAnzeigen)
          {
            if(ErgebnisSpalteWskAnzeigen)
            {
              HorizontalerKnotenAbstand= (29.7 - SeitenRandLinks - SeitenRandRechts - ErgebnisBreite - WskBreite - 1 - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
            }
            else
            {
              HorizontalerKnotenAbstand= (29.7 - SeitenRandLinks - SeitenRandRechts - ErgebnisBreite - 0.5 - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
            }
          }
          else
          {
            if(ErgebnisSpalteWskAnzeigen)
            {
              HorizontalerKnotenAbstand= (29.7 - SeitenRandLinks - SeitenRandRechts - WskBreite - 0.5 - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
            }
            else
            {
              HorizontalerKnotenAbstand= (29.7 - SeitenRandLinks - SeitenRandRechts - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
            }
          }
          break;
        case 3:
          SeitenHoehe = 42;
          SeitenBreite = 29.7;
          if(!BruchErgebnisFormel)
          {
            if(ErgebnisSpalteAnzeigen)
            {
              VertikalerKnotenAbstand= (42 - SeitenRandOben - SeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * KnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
            }
            else
            {
              VertikalerKnotenAbstand= (42 - SeitenRandOben - SeitenRandUnten - AnzahlKnotenLetzteStufe * KnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
            }
          }
          else
          {
            if(ErgebnisSpalteWskAnzeigen)
            {
              VertikalerKnotenAbstand= (42 - SeitenRandOben - SeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * KnotenHoehe - 0.5 - (FormelHoeheErgebnis - KnotenHoehe)) / (AnzahlKnotenLetzteStufe - 1);
            }
            else
            {
              if(ErgebnisSpalteAnzeigen)
              {
                VertikalerKnotenAbstand= (42 - SeitenRandOben - SeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * KnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
              }
              else
              {
                VertikalerKnotenAbstand= (42 - SeitenRandOben - SeitenRandUnten - AnzahlKnotenLetzteStufe * KnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
              }
            }
          }
          if(ErgebnisSpalteAnzeigen)
          {
            if(ErgebnisSpalteWskAnzeigen)
            {
              HorizontalerKnotenAbstand= (29.7 - SeitenRandLinks - SeitenRandRechts - ErgebnisBreite - WskBreite - 1 - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
            }
            else
            {
              HorizontalerKnotenAbstand= (29.7 - SeitenRandLinks - SeitenRandRechts - ErgebnisBreite - 0.5 - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
            }
          }
          else
          {
            if(ErgebnisSpalteWskAnzeigen)
            {
              HorizontalerKnotenAbstand= (29.7 - SeitenRandLinks - SeitenRandRechts - WskBreite - 0.5 - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
            }
            else
            {
              HorizontalerKnotenAbstand= (29.7 - SeitenRandLinks - SeitenRandRechts - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
            }
          }
          break;
        case 4:
          SeitenHoehe = 29.7;
          SeitenBreite = 42;
          if(!BruchErgebnisFormel)
          {
            if(ErgebnisSpalteAnzeigen)
            {
              VertikalerKnotenAbstand = (29.7 - SeitenRandOben - SeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * KnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
            }
            else
            {
              VertikalerKnotenAbstand = (29.7 - SeitenRandOben - SeitenRandUnten - AnzahlKnotenLetzteStufe * KnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
            }
          }
          else
          {
            if(ErgebnisSpalteWskAnzeigen)
            {
              VertikalerKnotenAbstand = (29.7 - SeitenRandOben - SeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * KnotenHoehe - 0.5 - (FormelHoeheErgebnis - KnotenHoehe)) / (AnzahlKnotenLetzteStufe - 1);
            }
            else
            {
              if(ErgebnisSpalteAnzeigen)
              {
                VertikalerKnotenAbstand = (29.7 - SeitenRandOben - SeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * KnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
              }
              else
              {
                VertikalerKnotenAbstand = (29.7 - SeitenRandOben - SeitenRandUnten - AnzahlKnotenLetzteStufe * KnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
              }
            }
          }
          if(ErgebnisSpalteAnzeigen)
          {
            if(ErgebnisSpalteWskAnzeigen)
            {
              HorizontalerKnotenAbstand= (42 - SeitenRandLinks - SeitenRandRechts - ErgebnisBreite - WskBreite - 1 - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
            }
            else
            {
              HorizontalerKnotenAbstand= (42 - SeitenRandLinks - SeitenRandRechts - ErgebnisBreite - 0.5 - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
            }
          }
          else
          {
            if(ErgebnisSpalteWskAnzeigen)
            {
              HorizontalerKnotenAbstand= (42 - SeitenRandLinks - SeitenRandRechts - WskBreite - 0.5 - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
            }
            else
            {
              HorizontalerKnotenAbstand= (42 - SeitenRandLinks - SeitenRandRechts - (ArgumentAnzahl - 1) * KnotenBreite) / (ArgumentAnzahl - 1);
            }
          }
          break;
        default:
          break;
      }
    }
  }

  if(svg)
  {
    if(svgFormelHoeheErgebnis > svgKnotenHoehe + svgVertikalerKnotenAbstand)
    {
      svgFormelHoeheErgebnis = svgKnotenHoehe + svgVertikalerKnotenAbstand;
      svgSchriftgroesseWskErgebnis = 12 * svgFormelHoeheErgebnis;
      switch(svgOptimierung)
      {
        case 1:
          svgSeitenHoehe = 29.7;
          svgSeitenBreite = 21;
          if(!svgBruchDarstellungErgebnis)
          {
            if(svgErgebnisSpalteAnzeigen)
            {
              svgVertikalerKnotenAbstand = (29.7 - svgSeitenRandOben - svgSeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * svgKnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
            }
            else
            {
              svgVertikalerKnotenAbstand = (29.7 - svgSeitenRandOben - svgSeitenRandUnten - AnzahlKnotenLetzteStufe * svgKnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
            }
          }
          else
          {
            if(svgErgebnisSpalteWskAnzeigen)
            {
              svgVertikalerKnotenAbstand = (29.7 - svgSeitenRandOben - svgSeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * svgKnotenHoehe - 0.5 - (svgFormelHoeheErgebnis - svgKnotenHoehe)) / (AnzahlKnotenLetzteStufe - 1);
            }
            else
            {
              if(svgErgebnisSpalteAnzeigen)
              {
                svgVertikalerKnotenAbstand = (29.7 - svgSeitenRandOben - svgSeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * svgKnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
              }
              else
              {
                svgVertikalerKnotenAbstand = (29.7 - svgSeitenRandOben - svgSeitenRandUnten - AnzahlKnotenLetzteStufe * svgKnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
              }
            }
          }
          if(svgErgebnisSpalteAnzeigen)
          {
            if(svgErgebnisSpalteWskAnzeigen)
            {
              svgHorizontalerKnotenAbstand= (21 - svgSeitenRandLinks - svgSeitenRandRechts - svgErgebnisBreite - svgWskBreite - 1 - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
            }
            else
            {
              svgHorizontalerKnotenAbstand= (21 - svgSeitenRandLinks - svgSeitenRandRechts - svgErgebnisBreite - 0.5 - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
            }
          }
          else
          {
            if(svgErgebnisSpalteWskAnzeigen)
            {
              svgHorizontalerKnotenAbstand= (21 - svgSeitenRandLinks - svgSeitenRandRechts - svgWskBreite - 0.5 - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
            }
            else
            {
              svgHorizontalerKnotenAbstand= (21 - svgSeitenRandLinks - svgSeitenRandRechts - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
            }
          }
          break;
        case 2:
          svgSeitenHoehe = 21;
          svgSeitenBreite = 29.7;
          if(!svgBruchDarstellungErgebnis)
          {
            if(svgErgebnisSpalteAnzeigen)
            {
              svgVertikalerKnotenAbstand = (21 - svgSeitenRandOben - svgSeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * svgKnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
            }
            else
            {
              svgVertikalerKnotenAbstand = (21 - svgSeitenRandOben - svgSeitenRandUnten - AnzahlKnotenLetzteStufe * svgKnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
            }
          }
          else
          {
            if(svgErgebnisSpalteWskAnzeigen)
            {
              svgVertikalerKnotenAbstand = (21 - svgSeitenRandOben - svgSeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * svgKnotenHoehe - 0.5 - (svgFormelHoeheErgebnis - svgKnotenHoehe)) / (AnzahlKnotenLetzteStufe - 1);
            }
            else
            {
              if(svgErgebnisSpalteAnzeigen)
              {
                svgVertikalerKnotenAbstand = (21 - svgSeitenRandOben - svgSeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * svgKnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
              }
              else
              {
                svgVertikalerKnotenAbstand = (21 - svgSeitenRandOben - svgSeitenRandUnten - AnzahlKnotenLetzteStufe * svgKnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
              }
            }
          }
          if(svgErgebnisSpalteAnzeigen)
          {
            if(svgErgebnisSpalteWskAnzeigen)
            {
              svgHorizontalerKnotenAbstand= (29.7 - svgSeitenRandLinks - svgSeitenRandRechts - svgErgebnisBreite - svgWskBreite - 1 - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
            }
            else
            {
              svgHorizontalerKnotenAbstand= (29.7 - svgSeitenRandLinks - svgSeitenRandRechts - svgErgebnisBreite - 0.5 - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
            }
          }
          else
          {
            if(svgErgebnisSpalteWskAnzeigen)
            {
              svgHorizontalerKnotenAbstand= (29.7 - svgSeitenRandLinks - svgSeitenRandRechts - svgWskBreite - 0.5 - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
            }
            else
            {
              svgHorizontalerKnotenAbstand= (29.7 - svgSeitenRandLinks - svgSeitenRandRechts - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
            }
          }
          break;
        case 3:
          svgSeitenHoehe = 42;
          svgSeitenBreite = 29.7;
          if(!svgBruchDarstellungErgebnis)
          {
            if(svgErgebnisSpalteAnzeigen)
            {
              svgVertikalerKnotenAbstand = (42 - svgSeitenRandOben - svgSeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * svgKnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
            }
            else
            {
              svgVertikalerKnotenAbstand = (42 - svgSeitenRandOben - svgSeitenRandUnten - AnzahlKnotenLetzteStufe * svgKnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
            }
          }
          else
          {
            if(svgErgebnisSpalteWskAnzeigen)
            {
              svgVertikalerKnotenAbstand = (42 - svgSeitenRandOben - svgSeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * svgKnotenHoehe - 0.5 - (svgFormelHoeheErgebnis - svgKnotenHoehe)) / (AnzahlKnotenLetzteStufe - 1);
            }
            else
            {
              if(svgErgebnisSpalteAnzeigen)
              {
                svgVertikalerKnotenAbstand = (42 - svgSeitenRandOben - svgSeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * svgKnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
              }
              else
              {
                svgVertikalerKnotenAbstand = (42 - svgSeitenRandOben - svgSeitenRandUnten - AnzahlKnotenLetzteStufe * svgKnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
              }
            }
          }
          if(svgErgebnisSpalteAnzeigen)
          {
            if(svgErgebnisSpalteWskAnzeigen)
            {
              svgHorizontalerKnotenAbstand= (29.7 - svgSeitenRandLinks - svgSeitenRandRechts - svgErgebnisBreite - svgWskBreite - 1 - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
            }
            else
            {
              svgHorizontalerKnotenAbstand= (29.7 - svgSeitenRandLinks - svgSeitenRandRechts - svgErgebnisBreite - 0.5 - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
            }
          }
          else
          {
            if(svgErgebnisSpalteWskAnzeigen)
            {
              svgHorizontalerKnotenAbstand= (29.7 - svgSeitenRandLinks - svgSeitenRandRechts - svgWskBreite - 0.5 - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
            }
            else
            {
              svgHorizontalerKnotenAbstand= (29.7 - svgSeitenRandLinks - svgSeitenRandRechts - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
            }
          }
          break;
        case 4:
          svgSeitenHoehe = 29.7;
          svgSeitenBreite = 42;
          if(!svgBruchDarstellungErgebnis)
          {
            if(svgErgebnisSpalteAnzeigen)
            {
              svgVertikalerKnotenAbstand = (29.7 - svgSeitenRandOben - svgSeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * svgKnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
            }
            else
            {
              svgVertikalerKnotenAbstand = (29.7 - svgSeitenRandOben - svgSeitenRandUnten - AnzahlKnotenLetzteStufe * svgKnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
            }
          }
          else
          {
            if(svgErgebnisSpalteWskAnzeigen)
            {
              svgVertikalerKnotenAbstand = ( 25.7 - (AnzahlKnotenLetzteStufe + 1) * svgKnotenHoehe - 0.5 - (svgFormelHoeheErgebnis - svgKnotenHoehe)) / (AnzahlKnotenLetzteStufe - 1);
            }
            else
            {
              if(svgErgebnisSpalteAnzeigen)
              {
                svgVertikalerKnotenAbstand = (29.7 - svgSeitenRandOben - svgSeitenRandUnten - (AnzahlKnotenLetzteStufe + 1) * svgKnotenHoehe - 0.5 ) / (AnzahlKnotenLetzteStufe - 1);
              }
              else
              {
                svgVertikalerKnotenAbstand = (29.7 - svgSeitenRandOben - svgSeitenRandUnten - AnzahlKnotenLetzteStufe * svgKnotenHoehe) / (AnzahlKnotenLetzteStufe - 1);
              }
            }
          }
          if(svgErgebnisSpalteAnzeigen)
          {
            if(svgErgebnisSpalteWskAnzeigen)
            {
              svgHorizontalerKnotenAbstand= (42 - svgSeitenRandLinks - svgSeitenRandRechts - svgErgebnisBreite - svgWskBreite - 1 - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
            }
            else
            {
              svgHorizontalerKnotenAbstand= (42 - svgSeitenRandLinks - svgSeitenRandRechts - svgErgebnisBreite - 0.5 - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
            }
          }
          else
          {
            if(svgErgebnisSpalteWskAnzeigen)
            {
              svgHorizontalerKnotenAbstand= (42 - svgSeitenRandLinks - svgSeitenRandRechts - svgWskBreite - 0.5 - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
            }
            else
            {
              svgHorizontalerKnotenAbstand= (42 - svgSeitenRandLinks - svgSeitenRandRechts - (ArgumentAnzahl - 1) * svgKnotenBreite) / (ArgumentAnzahl - 1);
            }
          }
          break;
        default:
          break;
      }
    }
  }



  if(odt)
  {
    if(SeitenHoehe == 0)
    {
      if(!ErgebnisSpalteAnzeigen && !ErgebnisSpalteWskAnzeigen)
      {
        SeitenHoehe = (AnzahlKnotenLetzteStufe - 1) * (KnotenHoehe + VertikalerKnotenAbstand) + KnotenHoehe + SeitenRandOben + SeitenRandUnten;
      }
      else
      {
        if(BruchErgebnisFormel && ErgebnisSpalteWskAnzeigen)
        {
          SeitenHoehe = (AnzahlKnotenLetzteStufe - 1) * (KnotenHoehe + VertikalerKnotenAbstand) + 2 * KnotenHoehe + SeitenRandOben + SeitenRandUnten + 0.5 + FormelHoeheErgebnis / 2;
        }
        else
        {
          SeitenHoehe = (AnzahlKnotenLetzteStufe - 1) * (KnotenHoehe + VertikalerKnotenAbstand) + 2 * KnotenHoehe + SeitenRandOben + SeitenRandUnten + 0.5;
        }
      }
    }
    if(SeitenBreite == 0)
    {
      SeitenBreite = (ArgumentAnzahl - 1) * (KnotenBreite + HorizontalerKnotenAbstand) + SeitenRandLinks + SeitenRandRechts;
      if(ErgebnisSpalteAnzeigen != 0)
      {
        SeitenBreite += ErgebnisBreite + 0.5;
      }
      if(ErgebnisSpalteWskAnzeigen != 0)
      {
        SeitenBreite += WskBreite + 0.5;
      }
    }
  }

  if(svg)
  {
    if(svgSeitenHoehe == 0)
    {
      if(!svgErgebnisSpalteAnzeigen && !svgErgebnisSpalteWskAnzeigen)
      {
        svgSeitenHoehe = (AnzahlKnotenLetzteStufe - 1) * (svgKnotenHoehe + svgVertikalerKnotenAbstand) + svgKnotenHoehe + svgSeitenRandOben + svgSeitenRandUnten;
      }
      else
      {
        if(svgBruchDarstellungErgebnis && ErgebnisSpalteWskAnzeigen)
        {
          svgSeitenHoehe = (AnzahlKnotenLetzteStufe - 1) * (svgKnotenHoehe + svgVertikalerKnotenAbstand) + 2 * svgKnotenHoehe + svgSeitenRandOben + svgSeitenRandUnten + 0.5 + svgFormelHoeheErgebnis / 2;
        }
        else
        {
          svgSeitenHoehe = (AnzahlKnotenLetzteStufe - 1) * (svgKnotenHoehe + svgVertikalerKnotenAbstand) + 2 * svgKnotenHoehe + svgSeitenRandOben + svgSeitenRandUnten + 0.5;
        }
      }
    }
    if(svgSeitenBreite == 0)
    {
      svgSeitenBreite = (ArgumentAnzahl - 1) * (svgKnotenBreite + svgHorizontalerKnotenAbstand) + svgSeitenRandLinks + svgSeitenRandRechts;
      if(svgErgebnisSpalteAnzeigen != 0)
      {
        svgSeitenBreite += svgErgebnisBreite + 0.5;
      }
      if(svgErgebnisSpalteWskAnzeigen != 0)
      {
        svgSeitenBreite += svgWskBreite + 0.5;
      }
    }
  }


  /* Deklaration nicht einstellbarer Variablen */

  float WskBreiteFormel;

  float **yKoordinaten;
  if(odt)
  {
    yKoordinaten = (float **)calloc(ArgumentAnzahl,sizeof(float *));
    for(j=0; j < ArgumentAnzahl; j++)
    {
       yKoordinaten[j] = (float *)calloc(MaximaleAnzahlKnotenProStufe+1,sizeof(float));
    }
  }

  float **svgyKoordinaten;
  if(svg)
  {
    svgyKoordinaten = (float **)calloc(ArgumentAnzahl,sizeof(float *));
    for(j=0; j < ArgumentAnzahl; j++)
    {
       svgyKoordinaten[j] = (float *)calloc(MaximaleAnzahlKnotenProStufe+1,sizeof(float));
    }
  }

  int **A;
  A = (int **)calloc(ArgumentAnzahl,sizeof(int *));
  for(j=0; j < ArgumentAnzahl; j++)
  {
     A[j] = (int *)calloc(MaximaleAnzahlKnotenProStufe+1,sizeof(int));
  }
  for(j=0; j < ArgumentAnzahl; j++)
  {
     for(k=0;k<MaximaleAnzahlKnotenProStufe+1;k++)
     {
       //printf("CALLOC: A[%i][%i]=%i\n",j,k,A[j][k]);
     }
  }

  char ***KNOTEN;
  if(odt)
  {
    KNOTEN = (char ***)calloc(ArgumentAnzahl,sizeof(char**));
    for(j=0; j < ArgumentAnzahl; j++)
    {
      KNOTEN[j] = (char **)calloc(MaximaleAnzahlKnotenProStufe+1,sizeof(char*));
      for(k=0; k < MaximaleAnzahlKnotenProStufe+1; k++)
      {
        KNOTEN[j][k] = (char *)calloc(MaximaleLaengeKnoten*50,sizeof(char));
      }
    }
  }

  char ***svgKNOTEN;
  if(svg)
  {
    svgKNOTEN = (char ***)calloc(ArgumentAnzahl,sizeof(char**));
    for(j=0; j < ArgumentAnzahl; j++)
    {
      svgKNOTEN[j] = (char **)calloc(MaximaleAnzahlKnotenProStufe+1,sizeof(char*));
      for(k=0; k < MaximaleAnzahlKnotenProStufe+1; k++)
      {
        //printf("MaximaleLaengeKnoten: %d\n",MaximaleLaengeKnoten);
        svgKNOTEN[j][k] = (char *)calloc(MaximaleLaengeKnoten*50+1,sizeof(char));
      }
    }
  }

  float **Wahrscheinlichkeit;
  if(wsk)
  {
    Wahrscheinlichkeit = (float **)calloc(ArgumentAnzahl,sizeof(float *));
    for(j=0; j < ArgumentAnzahl; j++)
    {
      Wahrscheinlichkeit[j] = (float *)calloc(MaximaleAnzahlKnotenProStufe+1,sizeof(float));
    }
  }

  char ***Bruchwahrscheinlichkeit;
  if(wsk)
  {
    Bruchwahrscheinlichkeit = (char ***)calloc(ArgumentAnzahl,sizeof(char**));
    for(j=0; j < ArgumentAnzahl; j++)
    {
      Bruchwahrscheinlichkeit[j] = (char **)calloc(MaximaleAnzahlKnotenProStufe+2,sizeof(char*));
      for(k=0; k < MaximaleAnzahlKnotenProStufe+2; k++)
      {
        Bruchwahrscheinlichkeit[j][k] = (char *)calloc(laengsteWahrscheinlichkeit+1,sizeof(char));
      }
    }
  }

  char ***gekuerzteBruchwahrscheinlichkeit;
  if(wsk)
  {
    gekuerzteBruchwahrscheinlichkeit = (char ***)calloc(ArgumentAnzahl,sizeof(char**));
    for(j=0; j < ArgumentAnzahl; j++)
    {
      gekuerzteBruchwahrscheinlichkeit[j] = (char **)calloc(MaximaleAnzahlKnotenProStufe+1,sizeof(char*));
      for(k=0; k < MaximaleAnzahlKnotenProStufe+1; k++)
      {
        gekuerzteBruchwahrscheinlichkeit[j][k] = (char *)calloc(202,sizeof(char));
      }
    }
  }

  char **ZaehlerErgebnis;
  if(wsk)
  {
    ZaehlerErgebnis = (char **)calloc(MaximaleAnzahlKnotenProStufe+1,sizeof(char*));
    for(j=0; j < MaximaleAnzahlKnotenProStufe+1; j++)
    {
      ZaehlerErgebnis[j] = (char *)calloc(101,sizeof(char));
    }
  }

  char **NennerErgebnis;
  if(wsk)
  {
    NennerErgebnis = (char **)calloc(MaximaleAnzahlKnotenProStufe+1,sizeof(char*));
    for(j=0; j < MaximaleAnzahlKnotenProStufe+1; j++)
    {
      NennerErgebnis[j] = (char *)calloc(101,sizeof(char));
    }
  }

  char **ZaehlerZweig;
  if(wsk)
  {
    ZaehlerZweig = (char **)calloc(MaximaleAnzahlKnotenProStufe*ArgumentAnzahl+1,sizeof(char*));
    for(j=0; j < MaximaleAnzahlKnotenProStufe*ArgumentAnzahl+1; j++)
    {
      ZaehlerZweig[j] = (char *)calloc(101,sizeof(char));
    }
  }

  char **NennerZweig;
  if(wsk)
  {
    NennerZweig = (char **)calloc(MaximaleAnzahlKnotenProStufe*ArgumentAnzahl+1,sizeof(char*));
    for(j=0; j < MaximaleAnzahlKnotenProStufe*ArgumentAnzahl+1; j++)
    {
      NennerZweig[j] = (char *)calloc(101,sizeof(char));
    }
  }


  /* Erstellung eines temporären Verzeichnisses, welches später gezippt wird */

  char META[4097];
  if(odt)
  {
    mkdir(verzeichnisname MODUS;
    strcpy(META,verzeichnisname);
    strcat(META,"/META-INF");
    mkdir(META MODUS;
  }


  /* Hier wird die Datei content.xml erstellt! */

  if(odt)
  {
    fp=fopen(CONTENT,"a");
    fprintf(fp,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n<office:document-content xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\" xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\" xmlns:table=\"urn:oasis:names:tc:opendocument:xmlns:table:1.0\" xmlns:draw=\"urn:oasis:names:tc:opendocument:xmlns:drawing:1.0\" xmlns:fo=\"urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\" xmlns:number=\"urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0\" xmlns:svg=\"urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0\" xmlns:chart=\"urn:oasis:names:tc:opendocument:xmlns:chart:1.0\" xmlns:dr3d=\"urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0\" xmlns:math=\"http://www.w3.org/1998/Math/MathML\" xmlns:form=\"urn:oasis:names:tc:opendocument:xmlns:form:1.0\" xmlns:script=\"urn:oasis:names:tc:opendocument:xmlns:script:1.0\" xmlns:ooo=\"http://openoffice.org/2004/office\" xmlns:ooow=\"http://openoffice.org/2004/writer\" xmlns:oooc=\"http://openoffice.org/2004/calc\" xmlns:dom=\"http://www.w3.org/2001/xml-events\" xmlns:xforms=\"http://www.w3.org/2002/xforms\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:rpt=\"http://openoffice.org/2005/report\" xmlns:of=\"urn:oasis:names:tc:opendocument:xmlns:of:1.2\" xmlns:xhtml=\"http://www.w3.org/1999/xhtml\" xmlns:grddl=\"http://www.w3.org/2003/g/data-view#\" xmlns:tableooo=\"http://openoffice.org/2009/table\" xmlns:field=\"urn:openoffice:names:experimental:ooo-ms-interop:xmlns:field:1.0\" xmlns:formx=\"urn:openoffice:names:experimental:ooxml-odf-interop:xmlns:form:1.0\" xmlns:css3t=\"http://www.w3.org/TR/css3-text/\" office:version=\"1.2\">\n <office:scripts/>\n <office:font-face-decls>\n  <style:font-face style:name=\"%s\" svg:font-family=\"%s\"/>\n  <style:font-face style:name=\"%s\" svg:font-family=\"%s\"/>\n  <style:font-face style:name=\"%s\" svg:font-family=\"%s\"/>\n  <style:font-face style:name=\"%s\" svg:font-family=\"%s\"/>\n  <style:font-face style:name=\"%s\" svg:font-family=\"%s\"/>\n  <style:font-face style:name=\"%s\" svg:font-family=\"%s\"/>\n </office:font-face-decls>\n <office:automatic-styles>\n  <style:style style:name=\"P1\" style:family=\"paragraph\">\n   <style:paragraph-properties fo:text-align=\"center\"/>\n   <style:text-properties fo:color=\"#%s\" style:font-name=\"%s\" fo:font-size=\"%dpt\" fo:font-style=\"%s\" fo:font-weight=\"%s\"/>\n  </style:style>\n  <style:style style:name=\"P2\" style:family=\"paragraph\">\n   <style:paragraph-properties fo:text-align=\"center\"/>\n   <style:text-properties fo:color=\"#%s\" style:font-name=\"%s\" fo:font-size=\"%dpt\" fo:font-style=\"%s\" fo:font-weight=\"%s\"/>\n  </style:style>\n  <style:style style:name=\"P3\" style:family=\"paragraph\">\n   <style:paragraph-properties fo:text-align=\"center\"/>\n   <style:text-properties fo:color=\"#%s\" style:font-name=\"%s\" fo:font-size=\"%dpt\" fo:font-style=\"%s\" fo:font-weight=\"%s\"/>\n  </style:style>\n  <style:style style:name=\"P4\" style:family=\"paragraph\">\n   <style:paragraph-properties fo:text-align=\"center\"/>\n   <style:text-properties fo:color=\"#%s\" style:font-name=\"%s\" fo:font-size=\"%dpt\" fo:font-style=\"%s\" fo:font-weight=\"%s\"/>\n  </style:style>\n  <style:style style:name=\"P5\" style:family=\"paragraph\">\n   <style:paragraph-properties fo:text-align=\"center\"/>\n   <style:text-properties fo:color=\"#%s\" style:font-name=\"%s\" fo:font-size=\"%dpt\" fo:font-style=\"%s\" fo:font-weight=\"%s\"/>\n  </style:style>\n  <style:style style:name=\"P6\" style:family=\"paragraph\">\n   <style:paragraph-properties fo:text-align=\"center\"/>\n   <style:text-properties fo:color=\"#%s\" style:font-name=\"%s\" fo:font-size=\"%dpt\" fo:font-style=\"%s\" fo:font-weight=\"%s\"/>\n  </style:style>\n  <style:style style:name=\"T1\" style:family=\"text\">\n   <style:text-properties style:font-name=\"Times New Roman\" style:text-overline-style=\"solid\" style:text-overline-width=\"auto\" style:text-overline-color=\"font-color\"/>\n  </style:style>\n  <style:style style:name=\"fr1\" style:family=\"graphic\" style:parent-style-name=\"Formula\">\n   <style:graphic-properties fo:margin-left=\"0cm\" fo:margin-right=\"0cm\" style:wrap=\"run-through\" style:number-wrapped-paragraphs=\"no-limit\" style:wrap-contour=\"false\" style:vertical-pos=\"from-top\" style:vertical-rel=\"paragraph\" style:horizontal-pos=\"from-left\" style:horizontal-rel=\"paragraph\" fo:background-color=\"#%s\" style:background-transparency=\"%d%%\" draw:fill=\"%s\" fo:padding=\"0cm\" fo:border=\"%gcm solid #%s\" style:shadow=\"#%s\" draw:shadow-opacity=\"%d%%\" draw:ole-draw-aspect=\"1\">\n    <style:background-image/>\n   </style:graphic-properties>\n  </style:style>\n  <style:style style:name=\"gr1\" style:family=\"graphic\">\n   <style:graphic-properties style:run-through=\"background\" style:vertical-pos=\"from-top\" style:vertical-rel=\"paragraph\" style:horizontal-pos=\"from-left\" style:horizontal-rel=\"paragraph\" draw:wrap-influence-on-position=\"once-concurrent\" style:flow-with-text=\"false\"/>\n  </style:style>\n  <style:style style:name=\"gr2\" style:family=\"graphic\">\n   <style:graphic-properties draw:stroke=\"%s\" draw:stroke-dash=\"StrichelungZweig\" svg:stroke-width=\"%gcm\" svg:stroke-color=\"#%s\" draw:marker-start=\"%s\" draw:marker-start-width=\"%gcm\" draw:marker-end=\"%s\" draw:marker-end-width=\"%gcm\" svg:stroke-opacity=\"%d%%\" draw:stroke-linejoin=\"none\" svg:stroke-linecap=\"butt\" draw:textarea-horizontal-align=\"%s\" draw:textarea-vertical-align=\"middle\" fo:padding-top=\"%gcm\" fo:padding-bottom=\"%gcm\" fo:padding-left=\"%gcm\" fo:padding-right=\"%gcm\" style:run-through=\"background\"/>\n  </style:style>\n  <style:style style:name=\"gr3\" style:family=\"graphic\">\n   <style:graphic-properties draw:stroke=\"%s\" draw:stroke-dash=\"StrichelungKnoten\" svg:stroke-width=\"%gcm\" svg:stroke-color=\"#%s\" draw:marker-start-width=\"0cm\" draw:marker-start-center=\"false\" draw:marker-end-width=\"0cm\" draw:marker-end-center=\"false\" svg:stroke-opacity=\"%d%%\" draw:stroke-linejoin=\"none\" svg:stroke-linecap=\"butt\" draw:fill=\"%s\" draw:fill-color=\"#%s\"%s%s draw:shadow=\"%s\" draw:shadow-offset-x=\"%gcm\" draw:shadow-offset-y=\"%gcm\" draw:shadow-color=\"#%s\" draw:shadow-opacity=\"%d%%\" draw:textarea-horizontal-align=\"center\" draw:textarea-vertical-align=\"middle\" fo:padding-top=\"0.05cm\" fo:padding-bottom=\"0.05cm\" fo:padding-left=\"0.05cm\" fo:padding-right=\"0.05cm\" style:run-through=\"background\"/>\n  </style:style>\n  <style:style style:name=\"gr4\" style:family=\"graphic\">\n   <style:graphic-properties draw:stroke=\"%s\" draw:stroke-dash=\"StrichelungErgebnisTitel\" svg:stroke-width=\"%gcm\" svg:stroke-color=\"#%s\" draw:marker-start-width=\"0cm\" draw:marker-start-center=\"false\" draw:marker-end-width=\"0cm\" draw:marker-end-center=\"false\" svg:stroke-opacity=\"%d%%\" draw:stroke-linejoin=\"none\" svg:stroke-linecap=\"butt\" draw:fill=\"%s\" draw:fill-color=\"#%s\"%s%s draw:shadow=\"%s\" draw:shadow-offset-x=\"%gcm\" draw:shadow-offset-y=\"%gcm\" draw:shadow-color=\"#%s\" draw:shadow-opacity=\"%d%%\" draw:textarea-horizontal-align=\"center\" draw:textarea-vertical-align=\"middle\" fo:padding-top=\"0.05cm\" fo:padding-bottom=\"0.05cm\" fo:padding-left=\"0.05cm\" fo:padding-right=\"0.05cm\" style:run-through=\"background\"/>\n  </style:style>\n  <style:style style:name=\"gr5\" style:family=\"graphic\">\n   <style:graphic-properties draw:stroke=\"%s\" draw:stroke-dash=\"StrichelungErgebnis\" svg:stroke-width=\"%gcm\" svg:stroke-color=\"#%s\" draw:marker-start-width=\"0cm\" draw:marker-start-center=\"false\" draw:marker-end-width=\"0cm\" draw:marker-end-center=\"false\" svg:stroke-opacity=\"%d%%\" draw:stroke-linejoin=\"none\" svg:stroke-linecap=\"butt\" draw:fill=\"%s\" draw:fill-color=\"#%s\"%s%s draw:shadow=\"%s\" draw:shadow-offset-x=\"%gcm\" draw:shadow-offset-y=\"%gcm\" draw:shadow-color=\"#%s\" draw:shadow-opacity=\"%d%%\" draw:textarea-horizontal-align=\"center\" draw:textarea-vertical-align=\"middle\" fo:padding-top=\"0.05cm\" fo:padding-bottom=\"0.05cm\" fo:padding-left=\"0.05cm\" fo:padding-right=\"0.05cm\" style:run-through=\"background\"/>\n  </style:style>\n  <style:style style:name=\"gr6\" style:family=\"graphic\">\n   <style:graphic-properties draw:stroke=\"%s\" draw:stroke-dash=\"StrichelungWskErgebnisTitel\" svg:stroke-width=\"%gcm\" svg:stroke-color=\"#%s\" draw:marker-start-width=\"0cm\" draw:marker-start-center=\"false\" draw:marker-end-width=\"0cm\" draw:marker-end-center=\"false\" svg:stroke-opacity=\"%d%%\" draw:stroke-linejoin=\"none\" svg:stroke-linecap=\"butt\" draw:fill=\"%s\" draw:fill-color=\"#%s\"%s%s draw:shadow=\"%s\" draw:shadow-offset-x=\"%gcm\" draw:shadow-offset-y=\"%gcm\" draw:shadow-color=\"#%s\" draw:shadow-opacity=\"%d%%\" draw:textarea-horizontal-align=\"center\" draw:textarea-vertical-align=\"middle\" fo:padding-top=\"0.05cm\" fo:padding-bottom=\"0.05cm\" fo:padding-left=\"0.05cm\" fo:padding-right=\"0.05cm\" style:run-through=\"background\"/>\n  </style:style>\n  <style:style style:name=\"gr7\" style:family=\"graphic\">\n   <style:graphic-properties draw:stroke=\"%s\" draw:stroke-dash=\"StrichelungWskErgebnis\" svg:stroke-width=\"%gcm\" svg:stroke-color=\"#%s\" draw:marker-start-width=\"0cm\" draw:marker-start-center=\"false\" draw:marker-end-width=\"0cm\" draw:marker-end-center=\"false\" svg:stroke-opacity=\"%d%%\" draw:stroke-linejoin=\"none\" svg:stroke-linecap=\"butt\" draw:fill=\"%s\" draw:fill-color=\"#%s\"%s%s draw:shadow=\"%s\" draw:shadow-offset-x=\"%gcm\" draw:shadow-offset-y=\"%gcm\" draw:shadow-color=\"#%s\" draw:shadow-opacity=\"%d%%\" draw:textarea-horizontal-align=\"center\" draw:textarea-vertical-align=\"middle\" fo:padding-top=\"0.05cm\" fo:padding-bottom=\"0.05cm\" fo:padding-left=\"0.05cm\" fo:padding-right=\"0.05cm\" style:run-through=\"background\"/>\n  </style:style>\n </office:automatic-styles>\n <office:body>\n  <office:text>\n",SchriftartWsk, SchriftartWsk, SchriftartKnoten, SchriftartKnoten, SchriftartErgebnisTitel, SchriftartErgebnisTitel, SchriftartErgebnis, SchriftartErgebnis, SchriftartWskErgebnisTitel, SchriftartWskErgebnisTitel, SchriftartWskErgebnis, SchriftartWskErgebnis, SchriftfarbeWsk, SchriftartWsk, SchriftgroesseWsk, KursivWsk, FettWsk, SchriftfarbeKnoten, SchriftartKnoten, SchriftgroesseKnoten, KursivKnoten, FettKnoten, SchriftfarbeErgebnisTitel, SchriftartErgebnisTitel, SchriftgroesseErgebnisTitel, KursivErgebnisTitel, FettErgebnisTitel, SchriftfarbeErgebnis, SchriftartErgebnis, SchriftgroesseErgebnis, KursivErgebnis, FettErgebnis, SchriftfarbeWskErgebnisTitel, SchriftartWskErgebnisTitel, SchriftgroesseWskErgebnisTitel, KursivWskErgebnisTitel, FettWskErgebnisTitel, SchriftfarbeWskErgebnis, SchriftartWskErgebnis, SchriftgroesseWskErgebnis, KursivWskErgebnis, FettWskErgebnis, FuellfarbeWskErgebnis, 100-FuellundurchsichtigkeitWskErgebnis, RahmenWskErgebnis, RahmenWskErgebnisDicke, RahmenWskErgebnisFarbe, shadowstylewskergebnis, SchattenUndurchsichtigkeitWskErgebnis, Zweig, ZweigDicke ,ZweigFarbe, ZweigAnfangForm, ZweigAnfangDicke, ZweigEndeForm, ZweigEndeDicke, ZweigUndurchsichtigkeit, WahrscheinlichkeitsPositionAufZweig, VerschiebungWskNachUnten, VerschiebungWskNachOben, VerschiebungWskNachRechts, VerschiebungWskNachLinks, RahmenKnoten, RahmenKnotenDicke, RahmenKnotenFarbe, RahmenKnotenUndurchsichtigkeit, FuellungKnotenTyp, FuellfarbeKnoten, TransparenzVerlaufKnotenTyp, FuellungKnotenTypSpezifizierung, SchattenKnoten, SchattenVerschiebungXKnoten, SchattenVerschiebungYKnoten, SchattenFarbeKnoten, SchattenUndurchsichtigkeitKnoten, RahmenErgebnisTitel, RahmenErgebnisTitelDicke, RahmenErgebnisTitelFarbe, RahmenErgebnisTitelUndurchsichtigkeit, FuellungErgebnisTitelTyp, FuellfarbeErgebnisTitel, TransparenzVerlaufErgebnisTitelTyp, FuellungErgebnisTitelTypSpezifizierung, SchattenErgebnisTitel, SchattenVerschiebungXErgebnisTitel, SchattenVerschiebungYErgebnisTitel, SchattenFarbeErgebnisTitel, SchattenUndurchsichtigkeitErgebnisTitel, RahmenErgebnis, RahmenErgebnisDicke, RahmenErgebnisFarbe, RahmenErgebnisUndurchsichtigkeit, FuellungErgebnisTyp, FuellfarbeErgebnis, TransparenzVerlaufErgebnisTyp, FuellungErgebnisTypSpezifizierung, SchattenErgebnis, SchattenVerschiebungXErgebnis, SchattenVerschiebungYErgebnis, SchattenFarbeErgebnis, SchattenUndurchsichtigkeitErgebnis, RahmenWskErgebnisTitel, RahmenWskErgebnisTitelDicke, RahmenWskErgebnisTitelFarbe, RahmenWskErgebnisTitelUndurchsichtigkeit, FuellungWskErgebnisTitelTyp, FuellfarbeWskErgebnisTitel, TransparenzVerlaufWskErgebnisTitelTyp, FuellungWskErgebnisTitelTypSpezifizierung, SchattenWskErgebnisTitel, SchattenVerschiebungXWskErgebnisTitel, SchattenVerschiebungYWskErgebnisTitel, SchattenFarbeWskErgebnisTitel, SchattenUndurchsichtigkeitWskErgebnisTitel, RahmenWskErgebnis, RahmenWskErgebnisDicke, RahmenWskErgebnisFarbe, RahmenWskErgebnisUndurchsichtigkeit, FuellungWskErgebnisTyp, FuellfarbeWskErgebnis, TransparenzVerlaufWskErgebnisTyp, FuellungWskErgebnisTypSpezifizierung, SchattenWskErgebnis, SchattenVerschiebungXWskErgebnis, SchattenVerschiebungYWskErgebnis, SchattenFarbeWskErgebnis, SchattenUndurchsichtigkeitWskErgebnis);
    if(Gruppierung == 0 || Gruppierung == 2)
    {
      fprintf(fp,"    <draw:g text:anchor-type=\"paragraph\" draw:z-index=\"0\" draw:style-name=\"gr1\">\n");
    }
  }

  /* Der Header Für svg-Dateien */

  FILE *fps;

  //printf("Homeverzeichnis: %s\n",homeverzeichnis);
  char svgdatei[4097] = "";
  char hilfssvgdatei[4097] = "";
  if(svg)
  {
    if(svgDateipfad[0] == 48)
    {
      sprintf(svgdatei,"%s",homeverzeichnis);
    }
    else
    {
      if(chdir(svgDateipfad) == -1)
      {
        char *zeiger;
        char hilfsdateipfad[4097] = "";
        WURZELVERZEICHNIS;
        strcpy(hilfsdateipfad, svgDateipfad);
        zeiger = strtok(hilfsdateipfad,PFADTRENNER);
        while(zeiger)
        {
          //printf("%s\n",zeiger);
          strcat(hilfspfad,zeiger);
          strcat(hilfspfad,PFADTRENNER);
          //printf("%s\n",hilfsdateipfad);
          if(chdir(hilfspfad) == -1)
          {
            mkdir(hilfspfad MODUS;
          }
          zeiger = strtok(NULL,PFADTRENNER);
        }
        fprintf(stderr,"Warnung: Das Verzeichnis existiert nicht. Es wurde erstellt.");
        sprintf(svgdatei,"%s",svgDateipfad);
      }
      else
      {
        sprintf(svgdatei,"%s",svgDateipfad);
      }
    }
    strcat(svgdatei,PFADTRENNER);
    strcat(svgdatei,"baum.svg");
    strcpy(hilfssvgdatei, svgdatei);
    hilfssvgdatei[strlen(svgdatei)-4] = 0;
    i=1;
    //printf("svgdatei: %s\n",svgdatei);
    fps = fopen(svgdatei,"r");
    while(fps!=0)
    {
      fclose(fps);
      sprintf(svgdatei,"%s%i.svg",hilfssvgdatei,i);
      fps = fopen(svgdatei,"r");
      i++;
    }
    if(fps)
    {
      fclose(fps);
    }
  }


  if(svg)
  {
    //printf("svgdatei: %s\n",svgdatei);
    fps = fopen(svgdatei,"a");
    fprintf(fps,"<?xml version=\"1.0\"?>\n<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n<svg width=\"%g\" height=\"%g\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n <title>Baumdiagramm</title>\n",svgSeitenBreite*CMINPX,svgSeitenHoehe*CMINPX);
  }


  /* Jetzt werden die Knotenpunkte des Baumdiagrammes in die content.xml eingetragen! */

  i = ArgumentAnzahl - 1;
  while(i > 0)
  {
    n = 1;
    m = 1;
    int maximal = strlen(Argument[i]);
    char *argument;
    argument = calloc(maximal+2,sizeof(char));
    strcpy(argument,Argument[i]);
    strcat(argument,",");
    char *zeiger;
    int laengenpuffer = 0;
    char *knoten;
    zeiger = strtok(argument,",");
    while(zeiger != NULL)
    {
      int atmp = 0;
      laengenpuffer += (int)strlen(zeiger)+1;
      knoten = zeiger;
      knoten = strtok(zeiger,"+");
      while(knoten != NULL)
      {
        int knotenlaenge = strlen(knoten);
        char *ueberstreichung = strchr(knoten,126);
        if(ueberstreichung)
        {
          char hilfsknoten[300]="";
          char svghilfsknoten[300]="";
          for(j=0;j<knotenlaenge-1;j++)
          {
            if(knoten[j+1] == 126)
            {
              if(odt)
              {
                strcat(hilfsknoten,"<text:span text:style-name=\"T1\">");
                strncat(hilfsknoten,knoten+j,1);
                strcat(hilfsknoten,"</text:span>");
              }
              if(svg)
              {
                strcat(svghilfsknoten,"<tspan text-decoration=\"overline\">");
                strncat(svghilfsknoten,knoten+j,1);
                strcat(svghilfsknoten,"</tspan>");
              }
            }
            else
            {
              if(odt)
              {
                if(knoten[j] != 126)
                {
                  strncat(hilfsknoten,knoten+j,1);
                }
                if(j == knotenlaenge-2)
                {
                  strncat(hilfsknoten,knoten+j+1,1);
                }
              }
              if(svg)
              {
                if(knoten[j] != 126)
                {
                  strncat(svghilfsknoten,knoten+j,1);
                }
                if(j == knotenlaenge-2)
                {
                  strncat(svghilfsknoten,knoten+j+1,1);
                }
              }
            }
          }
          if(odt)
          {
            strcpy(KNOTEN[i][n],hilfsknoten);
          }
          if(svg)
          {
            strcpy(svgKNOTEN[i][n],svghilfsknoten);
          }
        }
        else
        {
          if(odt)
          {
            strcpy(KNOTEN[i][n],knoten);
          }
          if(svg)
          {
            strcpy(svgKNOTEN[i][n],knoten);
          }
        }
        if(odt)
        {
          if(*KNOTEN[i][n] == 45)
          {
            KNOTEN[i][n][0] = 0;
          }
        }
        if(svg)
        {
          if(*svgKNOTEN[i][n] == 45 && svg)
          {
            svgKNOTEN[i][n][0] = 0;
          }
        }
        atmp++;
        if(odt)
        {
          x = i * HorizontalerKnotenAbstand + ( i - 1 ) * KnotenBreite;
        }
        if(svg)
        {
          svgx = i * svgHorizontalerKnotenAbstand + ( i - 1 ) * svgKnotenBreite;
        }
        if(i == ArgumentAnzahl - 1)
          {
            if(odt)
            {
              if(!ErgebnisSpalteAnzeigen && !ErgebnisSpalteWskAnzeigen)
              {
                yKoordinaten[i][n] = (n - 1) * (KnotenHoehe + VertikalerKnotenAbstand);
              }
              else
              {
                if(BruchErgebnisFormel && KnotenHoehe < FormelHoeheErgebnis && ErgebnisSpalteWskAnzeigen)
                {
                  yKoordinaten[i][n] = (n - 1) * (KnotenHoehe + VertikalerKnotenAbstand) + 0.5 + KnotenHoehe + ( FormelHoeheErgebnis - KnotenHoehe) / 2;
                }
                else
                {
                  yKoordinaten[i][n] = (n - 1) * (KnotenHoehe + VertikalerKnotenAbstand) + 0.5 + KnotenHoehe;
                }
              }
            }
            if(svg)
            {
              if(!svgErgebnisSpalteAnzeigen && !svgErgebnisSpalteWskAnzeigen)
              {
                svgyKoordinaten[i][n] = (n - 1) * (svgKnotenHoehe + svgVertikalerKnotenAbstand);
              }
              else
              {
                if(svgBruchDarstellungErgebnis && svgKnotenHoehe < svgFormelHoeheErgebnis && svgErgebnisSpalteWskAnzeigen)
                {
                  svgyKoordinaten[i][n] = (n - 1) * (svgKnotenHoehe + svgVertikalerKnotenAbstand) + 0.5 + svgKnotenHoehe + ( svgFormelHoeheErgebnis - svgKnotenHoehe) / 2;
                }
                else
                {
                  svgyKoordinaten[i][n] = (n - 1) * (svgKnotenHoehe + svgVertikalerKnotenAbstand) + 0.5 + svgKnotenHoehe;
                }
              }
            }
          }
        else
        {
          int a = A[i+1][n];
          int e = 0;
          for(j=1;j<=n;j++)
          {
            e += A[i+1][j];
          }
          if(odt)
          {
            if(i+1 < ArgumentAnzahl && e-a+1 <= MaximaleAnzahlKnotenProStufe && e <= MaximaleAnzahlKnotenProStufe) // dient als Schutz vor Pufferüberlauf bei falscher Eingabe
            {
              yKoordinaten[i][n] = (yKoordinaten[i+1][e-a+1] + yKoordinaten[i+1][e])/2;
            }
          }
          if(svg)
          {
            if(i+1 < ArgumentAnzahl && e-a+1 <= MaximaleAnzahlKnotenProStufe && e <= MaximaleAnzahlKnotenProStufe) // dient als Schutz vor Pufferüberlauf bei falscher Eingabe
            {
              svgyKoordinaten[i][n] = (svgyKoordinaten[i+1][e-a+1] + svgyKoordinaten[i+1][e])/2;
            }
          }
        }
        if(odt)
        {
          fprintf(fp,"     <draw:%s draw:style-name=\"gr3\" draw:text-style-name=\"P2\" svg:width=\"%gcm\" svg:height=\"%gcm\" svg:x=\"%gcm\" svg:y=\"%gcm\">\n      <text:p text:style-name=\"P2\">%s</text:p>\n     </draw:%s>\n",rechteckoderellipse,KnotenBreite,KnotenHoehe,x,yKoordinaten[i][n],KNOTEN[i][n],rechteckoderellipse);
        }
        if(svg)
        {
          fprintf(fps," <svg width=\"%g\" height=\"%g\" x=\"%g\" y=\"%g\">\n  <rect x=\"0\" y=\"0\" width=\"%g\" height=\"%g\" fill=\"%s\" fill-opacity=\"%g\" stroke=\"%s\"%s stroke-opacity=\"%g\" stroke-width=\"%g\"/>\n  <text x=\"%g\" y=\"%g\" text-anchor=\"middle\" font-family=\"'%s'\" font-size=\"%dpt\">%s</text>\n </svg>\n",svgKnotenBreite*CMINPX, svgKnotenHoehe*CMINPX, (svgx+svgSeitenRandLinks)*CMINPX, (svgyKoordinaten[i][n]+svgSeitenRandOben)*CMINPX, svgKnotenBreite*CMINPX, svgKnotenHoehe*CMINPX, svgFuellfarbeKnoten, (float)svgFuellundurchsichtigkeitKnoten/100.0, svgRahmenKnotenFarbe, svgrahmenknotenstrichelung, (float)svgRahmenKnotenUndurchsichtigkeit/100.0, svgRahmenKnotenDicke, svgKnotenBreite/2*CMINPX, svgKnotenHoehe/2*CMINPX+svgSchriftgroesseKnoten*SCHRIFTFAKTOR*2/3, svgSchriftartKnoten, svgSchriftgroesseKnoten, svgKNOTEN[i][n]);
        }
        n++;
        knoten = strtok(NULL,"+");
      }
      A[i][m] = atmp;
      m++;
      zeiger = strtok(argument+laengenpuffer,",");
    }
    AnzahlKnotenStufe[i] = n - 1;
    AnzahlKnoten += n - 1;
    free(argument);
    i--;
  }

  if(svg)
  {
    free(svgrahmenknotenstrichelung);
  }

  /* Falls Zweigwahrscheinlichkeiten Dezimalzahlen sein sollen, werden die nun in die Variable Wahrscheinlichkeit[][] geschrieben! */

  if(wsk && ((!BruchZweige && odt) || (!svgBruchZweige && svg)))
  {
    i = 2*ArgumentAnzahl-1;
    while(i > ArgumentAnzahl)
    {
      n = 1;
      m = 1;
      int maximal = strlen(Argument[i]);
      char *argument;
      argument = calloc(maximal+2,sizeof(char));
      strcpy(argument,Argument[i]);
      strcat(argument,",");
      char *zeiger;
      int laengenpuffer = 0;
      char *zweigwahrscheinlichkeit;
      zeiger = strtok(argument,",");
      while(zeiger != NULL)
      {
        char *hilfszeiger = calloc(strlen(zeiger)+2,sizeof(char));
        strcpy(hilfszeiger,zeiger);
        strcat(hilfszeiger,"+");
        laengenpuffer += (int)strlen(zeiger)+1;
        int laengenpuffer2 = 0;
        zweigwahrscheinlichkeit = hilfszeiger;
        zweigwahrscheinlichkeit = strtok(hilfszeiger,"+");
        while(zweigwahrscheinlichkeit != NULL)
        {
          char *hilfszweigwahrscheinlichkeit = calloc(strlen(zweigwahrscheinlichkeit)+2,sizeof(char));
          strcpy(hilfszweigwahrscheinlichkeit,zweigwahrscheinlichkeit);
          laengenpuffer2 += (int)strlen(zweigwahrscheinlichkeit)+1;
          if(strchr(hilfszweigwahrscheinlichkeit,47) == NULL)
          {
            Wahrscheinlichkeit[i-ArgumentAnzahl][n] = atof(hilfszweigwahrscheinlichkeit);
          }
          else
          {
            Wahrscheinlichkeit[i-ArgumentAnzahl][n] = bruchzufloat(hilfszweigwahrscheinlichkeit);
          }
          n++;
          zweigwahrscheinlichkeit = strtok(hilfszeiger+laengenpuffer2,"+");
          free(hilfszweigwahrscheinlichkeit);
        }
        m++;
        zeiger = strtok(argument+laengenpuffer,",");
        free(hilfszeiger);
      }
      free(argument);
      i--;
    }
  }

  /* Falls Zweigwahrscheinlichkeiten Brüche sein sollen, werden die nun in die Variable Bruchwahrscheinlichkeit[][] geschrieben! */

  if(wsk && ((BruchZweige && odt) || (svgBruchZweige && svg)))
  {
    i = 2*ArgumentAnzahl-1;
    while(i > ArgumentAnzahl)
    {
      n = 1;
      m = 1;
      int maximal = strlen(Argument[i]);
      char *argument;
      argument = calloc(maximal+2,sizeof(char));
      strcpy(argument,Argument[i]);
      strcat(argument,",");
      char *zeiger;
      int laengenpuffer = 0;
      char *zweigwahrscheinlichkeit;
      zeiger = strtok(argument,",");
      while(zeiger != NULL)
      {
        laengenpuffer += (int)strlen(zeiger)+1;
        zweigwahrscheinlichkeit = zeiger;
        zweigwahrscheinlichkeit = strtok(zeiger,"+");
        while(zweigwahrscheinlichkeit != NULL)
        {
          if(strchr(zweigwahrscheinlichkeit,47) == NULL)
          {
            floatstringzubruch(zweigwahrscheinlichkeit,Bruchwahrscheinlichkeit[i-ArgumentAnzahl][n]);
            //printf("Bruchwahrscheinlichkeit[%i][%i] = %s\n",i-ArgumentAnzahl,n,Bruchwahrscheinlichkeit[i-ArgumentAnzahl][n]);
          }
          else
          {
            strcpy(Bruchwahrscheinlichkeit[i-ArgumentAnzahl][n], zweigwahrscheinlichkeit);
            //printf("Bruchwahrscheinlichkeit[%i][%i] = %s\n",i-ArgumentAnzahl,n,Bruchwahrscheinlichkeit[i-ArgumentAnzahl][n]);
            if(!strcmp(Bruchwahrscheinlichkeit[i-ArgumentAnzahl][n],"1/1"))
            {
              Bruchwahrscheinlichkeit[i-ArgumentAnzahl][n][0] = 49;
              Bruchwahrscheinlichkeit[i-ArgumentAnzahl][n][1] = 0;
            }
            //printf("%c\n",Bruchwahrscheinlichkeit[i-ArgumentAnzahl][n][0]);
            if(Bruchwahrscheinlichkeit[i-ArgumentAnzahl][n][0] == 48)
            {
              Bruchwahrscheinlichkeit[i-ArgumentAnzahl][n][1] = 0;
            }
            //printf("Bruchwahrscheinlichkeit[%i][%i] = %s\n",i-ArgumentAnzahl,n,Bruchwahrscheinlichkeit[i-ArgumentAnzahl][n]);
          }
          n++;
          zweigwahrscheinlichkeit = strtok(NULL,"+");
        }
        m++;
        zeiger = strtok(argument+laengenpuffer,",");
      }
      free(argument);
      i--;
    }
  }

  if((UrneKuerzen || svgUrneKuerzen) && wsk)
  {
    for(i=1;i<=ArgumentAnzahl-1;i++)
    {
      char eins[4] = "1/1";
      j=1;
      while(Bruchwahrscheinlichkeit[i][j][0] != 0)
      {
        char *hilfsbruchwsk = calloc(strlen(Bruchwahrscheinlichkeit[i][j])+1,sizeof(char));
        strcpy(hilfsbruchwsk,Bruchwahrscheinlichkeit[i][j]);
        bruchmultiplikation(eins,hilfsbruchwsk,gekuerzteBruchwahrscheinlichkeit[i][j]);
        //printf("gekuerzteBruchwahrscheinlichkeit[%i][%i] = %s\n",i,j,gekuerzteBruchwahrscheinlichkeit[i][j]);
        free(hilfsbruchwsk);
        j++;
      }
    }
  }

  /* Jetzt werden die Verbindungslinien in die content.xml eingetragen! */

  float xl, xr, yl, yr, svgxl, svgxr, svgyl, svgyr;
  k = 1;
  l = 1;
  for(i=1;i<ArgumentAnzahl;i++)
  {
    m = 1;
    n = 1;
    int summeAim = A[i][1];
    if(odt)
    {
      xl = (i - 1) * (HorizontalerKnotenAbstand + KnotenBreite);
      xr = xl + HorizontalerKnotenAbstand;
    }
    if(svg)
    {
      svgxl = (i - 1) * (svgHorizontalerKnotenAbstand + svgKnotenBreite) + svgSeitenRandLinks;
      svgxr = svgxl + svgHorizontalerKnotenAbstand;
    }
    for(j=1;j<=AnzahlKnotenStufe[i];j++)
    {
      if(n > summeAim)
      {
        summeAim += A[i][m+1];
        m++;
      }
      if(odt)
      {
        if(i == 1)
        {
          yl = (yKoordinaten[1][1] + yKoordinaten[1][AnzahlKnotenStufe[1]])/2 + KnotenHoehe / 2;
        }
        else
        {
          yl = yKoordinaten[i-1][m] + KnotenHoehe / 2;
        }
        yr = yKoordinaten[i][n] + KnotenHoehe / 2;
      }
      if(svg)
      {
        if(i == 1)
        {
          svgyl = (svgyKoordinaten[1][1] + svgyKoordinaten[1][AnzahlKnotenStufe[1]])/2 + svgKnotenHoehe / 2 + svgSeitenRandOben;
        }
        else
        {
          svgyl = svgyKoordinaten[i-1][m] + svgKnotenHoehe / 2 + svgSeitenRandOben;
        }
        svgyr = svgyKoordinaten[i][n] + svgKnotenHoehe / 2 + svgSeitenRandOben;
      }
      if(!wsk)
      {
        if(odt && !svg)
        {
          if(KNOTEN[i][n][0])
          {
            fprintf(fp,"     <draw:line draw:style-name=\"gr2\" draw:text-style-name=\"P2\" svg:x1=\"%gcm\" svg:y1=\"%gcm\" svg:x2=\"%gcm\" svg:y2=\"%gcm\">\n      <text:p text:style-name=\"P2\"></text:p>\n     </draw:line>\n",xl,yl,xr,yr);
          }
          k++;
        }
        else if(!odt && svg)
        {
          if(svgKNOTEN[i][n][0])
          {
            //printf("xl:%g\nxr:%g\nyl:%g\nyr:%g\n",svgxl,svgxr,svgyl,svgyr);
            fprintf(fps," <path id=\"zweig%d\" d=\"M %g %g L %g %g\" stroke=\"%s\" stroke-width=\"%g\"%s/>\n",k, svgxl*CMINPX, svgyl*CMINPX, svgxr*CMINPX, svgyr*CMINPX, svgZweigFarbe, svgZweigDicke, svgzweigstrichelung);
          }
          k++;
        }
        else
        {
          if(KNOTEN[i][n][0])
          {
            fprintf(fp,"     <draw:line draw:style-name=\"gr2\" draw:text-style-name=\"P2\" svg:x1=\"%gcm\" svg:y1=\"%gcm\" svg:x2=\"%gcm\" svg:y2=\"%gcm\">\n      <text:p text:style-name=\"P2\"></text:p>\n     </draw:line>\n",xl,yl,xr,yr);
            fprintf(fps," <path id=\"zweig%d\" d=\"M %g %g L %g %g\" stroke=\"%s\" stroke-width=\"%g\"%s/>\n",k, svgxl*CMINPX, svgyl*CMINPX, svgxr*CMINPX, svgyr*CMINPX, svgZweigFarbe, svgZweigDicke, svgzweigstrichelung);
          }
          k++;
        }
      }
      else
      {
        char WahrscheinlichkeitsString[9] = "00000000";
        if(Wahrscheinlichkeit[i][j]<0.0001)
        {
          if(Wahrscheinlichkeit[i][j] == 0)
          {
            WahrscheinlichkeitsString[1] = 0;
          }
          else
          {
            sprintf(WahrscheinlichkeitsString,"%g",Wahrscheinlichkeit[i][j]);
/*            while(WahrscheinlichkeitsString[strlen(WahrscheinlichkeitsString)-1] == 48)
            {
              WahrscheinlichkeitsString[strlen(WahrscheinlichkeitsString)-1] = 0;
            }
            WahrscheinlichkeitsString[1]=44;*/
          }
        }
        else
        {
          if(Wahrscheinlichkeit[i][j] == 1)
          {
            WahrscheinlichkeitsString[0] = 49;
            WahrscheinlichkeitsString[1] = 0;
          }
          else
          {
            sprintf(WahrscheinlichkeitsString,"%g",Wahrscheinlichkeit[i][j]);
            WahrscheinlichkeitsString[1]=44;
          }
        }
        if(odt && !svg)
        {
          if(KNOTEN[i][n][0])
          {
            if(BruchZweige)
            {
              if(BruchZweigeFormel)
              {
                char hilfspfadwahrscheinlichkeit[202];
                strcpy(hilfspfadwahrscheinlichkeit,((UrneKuerzen)?gekuerzteBruchwahrscheinlichkeit[i][j]:Bruchwahrscheinlichkeit[i][j]));
                //printf("HILFSPFADWSK: %s\n",hilfspfadwahrscheinlichkeit);
                char *bruchteil;
                bruchteil = strtok(hilfspfadwahrscheinlichkeit,"/");
                strcpy(ZaehlerZweig[l],bruchteil);
                bruchteil = strtok(NULL,"/");
                if(bruchteil)
                {
                  strcpy(NennerZweig[l],bruchteil);
                }
                else
                {
                  strcpy(NennerZweig[l], "1");
                }

                if(strlen(ZaehlerZweig[l]) <= strlen(NennerZweig[l]))
                {
                  WskBreiteFormel = 0.21 + (float)SchriftgroesseWsk / 150 + strlen(NennerZweig[l]) * ((float)SchriftgroesseWsk * 11 / 600);
                }
                else
                {
                  WskBreiteFormel = 0.21 + (float)SchriftgroesseWsk / 150 + strlen(ZaehlerZweig[l]) * ((float)SchriftgroesseWsk * 11 / 600);
                }
                float xformel = (xl+xr)/2 /*+ ( WskBreite - WskBreiteFormel ) / 2*/;
                y = (yl + yr) / 2 - FormelHoeheZweig - ((yl-yr>0)?(yl-yr)/(xr-xl)*WskBreiteFormel:0);
                fprintf(fp,"     <draw:line draw:style-name=\"gr2\" draw:text-style-name=\"P1\" svg:x1=\"%gcm\" svg:y1=\"%gcm\" svg:x2=\"%gcm\" svg:y2=\"%gcm\">\n     </draw:line>\n     <draw:frame draw:style-name=\"fr1\" draw:name=\"FormelZweig%i\" text:anchor-type=\"paragraph\" svg:x=\"%gcm\" svg:y=\"%gcm\" svg:width=\"%gcm\" svg:height=\"%gcm\" draw:z-index=\"1\">\n      <draw:object xlink:href=\"./FormelZweig%i\" xlink:type=\"simple\" xlink:show=\"embed\" xlink:actuate=\"onLoad\"/>\n     </draw:frame>\n",xl,yl,xr,yr,l,xformel,y,WskBreiteFormel,FormelHoeheErgebnis,l);
                l++;
              }
              else
              {
                fprintf(fp,"     <draw:line draw:style-name=\"gr2\" draw:text-style-name=\"P1\" svg:x1=\"%gcm\" svg:y1=\"%gcm\" svg:x2=\"%gcm\" svg:y2=\"%gcm\">\n      <text:p text:style-name=\"P1\"><text:s text:c=\"%d\"/>%s</text:p>\n      <text:p text:style-name=\"P1\"></text:p>\n     </draw:line>\n",xl,yl,xr,yr,LeerzeichenAnzahl,(UrneKuerzen)?gekuerzteBruchwahrscheinlichkeit[i][j]:Bruchwahrscheinlichkeit[i][j]);
              }
            }
            else
            {
              if(Genauigkeit >= 0)
              {
                if(strlen(WahrscheinlichkeitsString) == 1)
                {
                  WahrscheinlichkeitsString[1] = 44;
                  int zaehlvariable;
                  for(zaehlvariable = 2;zaehlvariable < ((Genauigkeit>6)?8:Genauigkeit + 2);zaehlvariable++)
                  {
                    WahrscheinlichkeitsString[zaehlvariable] = 48;
                  }
                  WahrscheinlichkeitsString[zaehlvariable] = 0;
                }
                else
                {
                  int zaehlvariable;
                  for(zaehlvariable = 2;zaehlvariable < ((Genauigkeit>6)?8:Genauigkeit + 2);zaehlvariable++)
                  {
                    if(!WahrscheinlichkeitsString[zaehlvariable])
                    {
                      WahrscheinlichkeitsString[zaehlvariable] = 48;
                    }
                  }
                  if(WahrscheinlichkeitsString[zaehlvariable]>52)
                  {
                    WahrscheinlichkeitsString[zaehlvariable-1] += 1;
                  }
                  WahrscheinlichkeitsString[zaehlvariable] = 0;
                }
              }
              fprintf(fp,"     <draw:line draw:style-name=\"gr2\" draw:text-style-name=\"P1\" svg:x1=\"%gcm\" svg:y1=\"%gcm\" svg:x2=\"%gcm\" svg:y2=\"%gcm\">\n      <text:p text:style-name=\"P1\"><text:s text:c=\"%d\"/>%s</text:p>\n      <text:p text:style-name=\"P1\"></text:p>\n     </draw:line>\n",xl,yl,xr,yr,LeerzeichenAnzahl,WahrscheinlichkeitsString);
            }
            k++;
          }
        }
        if(!odt && svg)
        {
          if(svgKNOTEN[i][n][0])
          {
            if(svgBruchZweige)
            {
              if(svgBruchDarstellungZweig)
              {
                int zv;
                char *pfadwahrscheinlichkeit = calloc(strlen(((svgUrneKuerzen)?gekuerzteBruchwahrscheinlichkeit[i][j]:Bruchwahrscheinlichkeit[i][j]))+1,sizeof(char));
                strcpy(pfadwahrscheinlichkeit,((svgUrneKuerzen)?gekuerzteBruchwahrscheinlichkeit[i][j]:Bruchwahrscheinlichkeit[i][j]));
                //printf("pfadwsk: %s\n",pfadwahrscheinlichkeit);
                short zaehlerlaenge = 0, nennerlaenge = 0;
                zv=0;
                while(pfadwahrscheinlichkeit[zv] && pfadwahrscheinlichkeit[zv] != 47)
                {
                  zaehlerlaenge++;
                  zv++;
                }
                if(pfadwahrscheinlichkeit[zv] == 47)
                {
                  zv++;
                  while(pfadwahrscheinlichkeit[zv])
                  {
                    nennerlaenge++;
                    zv++;
                  }
                }
                //printf("Zählerlänge: %d und Nennerlänge: %d\n",zaehlerlaenge,nennerlaenge);
                char zaehler[21]="", bruchstrich[21]="", nenner[21]="";
                for(zv=0;zv<zaehlerlaenge;zv++)
                {
                  zaehler[zv] = pfadwahrscheinlichkeit[zv];
                }
                if(nennerlaenge)
                {
                  for(zv=0;zv<((zaehlerlaenge<nennerlaenge)?nennerlaenge:zaehlerlaenge);zv++)
                  {
                    bruchstrich[zv] = 95;
                  }
                  for(zv=0;zv<nennerlaenge;zv++)
                  {
                    nenner[zv] = pfadwahrscheinlichkeit[zv+zaehlerlaenge+1];
                  }
                }
                free(pfadwahrscheinlichkeit);
                if(zaehlerlaenge <= nennerlaenge)
                {
                  WskBreiteFormel = 0.21 + (float)SchriftgroesseWsk / 150 + strlen(NennerZweig[l]) * ((float)SchriftgroesseWsk * 11 / 600);
                }
                else
                {
                  WskBreiteFormel = 0.21 + (float)SchriftgroesseWsk / 150 + strlen(ZaehlerZweig[l]) * ((float)SchriftgroesseWsk * 11 / 600);
                }
                if(svgBruchDrehung)
                {
                  if(nennerlaenge)
                  {
                    //printf("WskBreiteFormel = %g\n",WskBreiteFormel);
                    fprintf(fps," <path d=\"M %g %g L %g %g\" stroke=\"%s\" stroke-width=\"%g\"%s stroke-opacity=\"%g\"/>\n <g>\n  <text text-anchor=\"middle\" font-family=\"'%s'\" font-size=\"%dpt\" x=\"%g\" y=\"%g\">\n   <tspan dx=\"%g\" dy=\"%g\">%s</tspan>\n  </text>\n  <text text-anchor=\"middle\" font-family=\"'%s'\" font-size=\"%dpt\" x=\"%g\" y=\"%g\">\n   <tspan dx=\"%g\" dy=\"%g\">%s</tspan>\n  </text>\n  <text text-anchor=\"middle\" font-family=\"'%s'\" font-size=\"%dpt\" x=\"%g\" y=\"%g\">\n   <tspan dx=\"%g\" dy=\"%g\">%s</tspan>\n  </text>\n </g>\n",svgxl*CMINPX, svgyl*CMINPX, svgxr*CMINPX, svgyr*CMINPX, svgZweigFarbe, svgZweigDicke, svgzweigstrichelung, (float)svgZweigUndurchsichtigkeit/100.0, svgSchriftartWsk, svgSchriftgroesseWsk, (svgxl+svgxr)/2*CMINPX+svgWskVerschiebung*CMINPX, (svgyl+svgyr)/2*CMINPX-svgWskVerschiebung*SVGM*CMINPX, svgZweigWskAbstand-5, ((SVGM<0)?SVGM:-SVGM)*WskBreiteFormel*CMINPX-(float)SchriftgroesseWsk*4/3-svgZweigWskAbstand, zaehler, svgSchriftartWsk, svgSchriftgroesseWsk, (svgxl+svgxr)/2*CMINPX+svgWskVerschiebung*CMINPX, (svgyl+svgyr)/2*CMINPX-svgWskVerschiebung*SVGM*CMINPX, svgZweigWskAbstand-5, ((SVGM<0)?SVGM:-SVGM)*WskBreiteFormel*CMINPX-(float)SchriftgroesseWsk*4/3-svgZweigWskAbstand, bruchstrich, svgSchriftartWsk, svgSchriftgroesseWsk, (svgxl+svgxr)/2*CMINPX+svgWskVerschiebung*CMINPX, (svgyl+svgyr)/2*CMINPX-svgWskVerschiebung*SVGM*CMINPX, svgZweigWskAbstand-5, ((SVGM<0)?SVGM:-SVGM)*WskBreiteFormel*CMINPX-svgZweigWskAbstand, nenner);
                  }
                  else
                  {
                    fprintf(fps," <path id=\"zweig%d\" d=\"M %g %g L %g %g\" stroke=\"%s\" stroke-width=\"%g\"%s stroke-opacity=\"%g\"/>\n <text text-anchor=\"middle\" font-family=\"'%s'\" font-size=\"%dpt\">\n  <textPath xlink:href=\"#zweig%d\">\n   <tspan dx=\"%g\" dy=\"%g\">%s</tspan>\n  </textPath>\n </text>\n",k, svgxl*CMINPX, svgyl*CMINPX, svgxr*CMINPX, svgyr*CMINPX, svgZweigFarbe, svgZweigDicke, svgzweigstrichelung, (float)svgZweigUndurchsichtigkeit/100.0, svgSchriftartWsk, svgSchriftgroesseWsk, k, wurzel((svgxl-svgxr)*(svgxl-svgxr)+(svgyl-svgyr)*(svgyl-svgyr))/2*CMINPX+svgWskVerschiebung*CMINPX, -svgZweigWskAbstand-svgWskVerschiebung*SVGM*CMINPX, zaehler);
                  }
                }
              }
              else
              {
                fprintf(fps," <path id=\"zweig%d\" d=\"M %g %g L %g %g\" stroke=\"%s\" stroke-width=\"%g\"%s stroke-opacity=\"%g\"/>\n <text text-anchor=\"middle\" font-family=\"'%s'\" font-size=\"%dpt\">\n  <textPath xlink:href=\"#zweig%d\">\n   <tspan dx=\"%g\" dy=\"%g\">%s</tspan>\n  </textPath>\n </text>\n",k, svgxl*CMINPX, svgyl*CMINPX, svgxr*CMINPX, svgyr*CMINPX, svgZweigFarbe, svgZweigDicke, svgzweigstrichelung, (float)svgZweigUndurchsichtigkeit/100.0, svgSchriftartWsk, svgSchriftgroesseWsk, k, wurzel((svgxl-svgxr)*(svgxl-svgxr)+(svgyl-svgyr)*(svgyl-svgyr))/2*CMINPX+svgWskVerschiebung*CMINPX, -svgZweigWskAbstand-svgWskVerschiebung*SVGM*CMINPX, (svgUrneKuerzen)?gekuerzteBruchwahrscheinlichkeit[i][j]:Bruchwahrscheinlichkeit[i][j]);
              }
            }
            else
            {
              if(svgGenauigkeit >= 0)
              {
                if(strlen(WahrscheinlichkeitsString) == 1)
                {
                  WahrscheinlichkeitsString[1] = 44;
                  int zaehlvariable;
                  for(zaehlvariable = 2;zaehlvariable < ((svgGenauigkeit>6)?8:svgGenauigkeit + 2);zaehlvariable++)
                  {
                    WahrscheinlichkeitsString[zaehlvariable] = 48;
                  }
                  WahrscheinlichkeitsString[zaehlvariable] = 0;
                }
                else
                {
                  int zaehlvariable;
                  for(zaehlvariable = 2;zaehlvariable < ((svgGenauigkeit>6)?8:svgGenauigkeit + 2);zaehlvariable++)
                  {
                    if(!WahrscheinlichkeitsString[zaehlvariable])
                    {
                      WahrscheinlichkeitsString[zaehlvariable] = 48;
                    }
                  }
                  if(WahrscheinlichkeitsString[zaehlvariable]>52)
                  {
                    WahrscheinlichkeitsString[zaehlvariable-1] += 1;
                  }
                  WahrscheinlichkeitsString[zaehlvariable] = 0;
                }
              }
              fprintf(fps," <path id=\"zweig%d\" d=\"M %g %g L %g %g\" stroke=\"%s\" stroke-width=\"%g\"%s stroke-opacity=\"%g\"/>\n <text text-anchor=\"middle\" font-family=\"'%s'\" font-size=\"%dpt\">\n  <textPath xlink:href=\"#zweig%d\">\n   <tspan dx=\"%g\" dy=\"%g\">%s</tspan>\n  </textPath>\n </text>\n",k, svgxl*CMINPX, svgyl*CMINPX, svgxr*CMINPX, svgyr*CMINPX, svgZweigFarbe, svgZweigDicke, svgzweigstrichelung, (float)svgZweigUndurchsichtigkeit/100.0, svgSchriftartWsk, svgSchriftgroesseWsk, k, wurzel((svgxl-svgxr)*(svgxl-svgxr)+(svgyl-svgyr)*(svgyl-svgyr))/2*CMINPX+svgWskVerschiebung*CMINPX, -svgZweigWskAbstand, WahrscheinlichkeitsString);
            }
            k++;
          }
        }
        if(odt && svg)
        {
          if(KNOTEN[i][n][0])
          {
            if(BruchZweige)
            {
              if(BruchZweigeFormel)
              {
                char hilfspfadwahrscheinlichkeit[202];
                strcpy(hilfspfadwahrscheinlichkeit,((UrneKuerzen)?gekuerzteBruchwahrscheinlichkeit[i][j]:Bruchwahrscheinlichkeit[i][j]));
                //printf("HILFSPFADWSK: %s\n",hilfspfadwahrscheinlichkeit);
                char *bruchteil;
                bruchteil = strtok(hilfspfadwahrscheinlichkeit,"/");
                strcpy(ZaehlerZweig[l],bruchteil);
                bruchteil = strtok(NULL,"/");
                if(bruchteil)
                {
                  strcpy(NennerZweig[l],bruchteil);
                }
                else
                {
                  strcpy(NennerZweig[l], "1");
                }

                if(strlen(ZaehlerZweig[l]) <= strlen(NennerZweig[l]))
                {
                  WskBreiteFormel = 0.21 + (float)SchriftgroesseWsk / 150 + strlen(NennerZweig[l]) * ((float)SchriftgroesseWsk * 11 / 600);
                }
                else
                {
                  WskBreiteFormel = 0.21 + (float)SchriftgroesseWsk / 150 + strlen(ZaehlerZweig[l]) * ((float)SchriftgroesseWsk * 11 / 600);
                }
                float xformel = (xl+xr)/2 /*+ ( WskBreite - WskBreiteFormel ) / 2*/;
                y = (yl + yr) / 2 - FormelHoeheZweig - ((yl-yr>0)?(yl-yr)/(xr-xl)*WskBreiteFormel:0);
                fprintf(fp,"     <draw:line draw:style-name=\"gr2\" draw:text-style-name=\"P1\" svg:x1=\"%gcm\" svg:y1=\"%gcm\" svg:x2=\"%gcm\" svg:y2=\"%gcm\">\n     </draw:line>\n     <draw:frame draw:style-name=\"fr1\" draw:name=\"FormelZweig%i\" text:anchor-type=\"paragraph\" svg:x=\"%gcm\" svg:y=\"%gcm\" svg:width=\"%gcm\" svg:height=\"%gcm\" draw:z-index=\"1\">\n      <draw:object xlink:href=\"./FormelZweig%i\" xlink:type=\"simple\" xlink:show=\"embed\" xlink:actuate=\"onLoad\"/>\n     </draw:frame>\n",xl,yl,xr,yr,l,xformel,y,WskBreiteFormel,FormelHoeheErgebnis,l);
                l++;
              }
              else
              {
                fprintf(fp,"     <draw:line draw:style-name=\"gr2\" draw:text-style-name=\"P1\" svg:x1=\"%gcm\" svg:y1=\"%gcm\" svg:x2=\"%gcm\" svg:y2=\"%gcm\">\n      <text:p text:style-name=\"P1\"><text:s text:c=\"%d\"/>%s</text:p>\n      <text:p text:style-name=\"P1\"></text:p>\n     </draw:line>\n",xl,yl,xr,yr,LeerzeichenAnzahl,(UrneKuerzen)?gekuerzteBruchwahrscheinlichkeit[i][j]:Bruchwahrscheinlichkeit[i][j]);
              }
            }
            else
            {
              if(Genauigkeit >= 0)
              {
                if(strlen(WahrscheinlichkeitsString) == 1)
                {
                  WahrscheinlichkeitsString[1] = 44;
                  int zaehlvariable;
                  for(zaehlvariable = 2;zaehlvariable < ((Genauigkeit>6)?8:Genauigkeit + 2);zaehlvariable++)
                  {
                    WahrscheinlichkeitsString[zaehlvariable] = 48;
                  }
                  WahrscheinlichkeitsString[zaehlvariable] = 0;
                }
                else
                {
                  int zaehlvariable;
                  for(zaehlvariable = 2;zaehlvariable < ((Genauigkeit>6)?8:Genauigkeit + 2);zaehlvariable++)
                  {
                    if(!WahrscheinlichkeitsString[zaehlvariable])
                    {
                      WahrscheinlichkeitsString[zaehlvariable] = 48;
                    }
                  }
                  if(WahrscheinlichkeitsString[zaehlvariable]>52)
                  {
                    WahrscheinlichkeitsString[zaehlvariable-1] += 1;
                  }
                  WahrscheinlichkeitsString[zaehlvariable] = 0;
                }
              }
              fprintf(fp,"     <draw:line draw:style-name=\"gr2\" draw:text-style-name=\"P1\" svg:x1=\"%gcm\" svg:y1=\"%gcm\" svg:x2=\"%gcm\" svg:y2=\"%gcm\">\n      <text:p text:style-name=\"P1\"><text:s text:c=\"%d\"/>%s</text:p>\n      <text:p text:style-name=\"P1\"></text:p>\n     </draw:line>\n",xl,yl,xr,yr,LeerzeichenAnzahl,WahrscheinlichkeitsString);
            }
          }
          if(svgKNOTEN[i][n][0])
          {
            if(svgBruchZweige)
            {
              if(svgBruchDarstellungZweig)
              {
                int zv;
                char *pfadwahrscheinlichkeit = calloc(strlen(((svgUrneKuerzen)?gekuerzteBruchwahrscheinlichkeit[i][j]:Bruchwahrscheinlichkeit[i][j]))+1,sizeof(char));
                strcpy(pfadwahrscheinlichkeit,((svgUrneKuerzen)?gekuerzteBruchwahrscheinlichkeit[i][j]:Bruchwahrscheinlichkeit[i][j]));
                //printf("pfadwsk: %s\n",pfadwahrscheinlichkeit);
                short zaehlerlaenge = 0, nennerlaenge = 0;
                zv=0;
                while(pfadwahrscheinlichkeit[zv] && pfadwahrscheinlichkeit[zv] != 47)
                {
                  zaehlerlaenge++;
                  zv++;
                }
                if(pfadwahrscheinlichkeit[zv] == 47)
                {
                  zv++;
                  while(pfadwahrscheinlichkeit[zv])
                  {
                    nennerlaenge++;
                    zv++;
                  }
                }
                //printf("Zählerlänge: %d und Nennerlänge: %d\n",zaehlerlaenge,nennerlaenge);
                char zaehler[21]="", bruchstrich[21]="", nenner[21]="";
                for(zv=0;zv<zaehlerlaenge;zv++)
                {
                  zaehler[zv] = pfadwahrscheinlichkeit[zv];
                }
                if(nennerlaenge)
                {
                  for(zv=0;zv<((zaehlerlaenge<nennerlaenge)?nennerlaenge:zaehlerlaenge);zv++)
                  {
                    bruchstrich[zv] = 95;
                  }
                  for(zv=0;zv<nennerlaenge;zv++)
                  {
                    nenner[zv] = pfadwahrscheinlichkeit[zv+zaehlerlaenge+1];
                  }
                }
                free(pfadwahrscheinlichkeit);
                if(zaehlerlaenge <= nennerlaenge)
                {
                  WskBreiteFormel = 0.21 + (float)SchriftgroesseWsk / 150 + strlen(NennerZweig[l]) * ((float)SchriftgroesseWsk * 11 / 600);
                }
                else
                {
                  WskBreiteFormel = 0.21 + (float)SchriftgroesseWsk / 150 + strlen(ZaehlerZweig[l]) * ((float)SchriftgroesseWsk * 11 / 600);
                }
                if(svgBruchDrehung)
                {
                  if(nennerlaenge)
                  {
                    //printf("WskBreiteFormel = %g\n",WskBreiteFormel);
                    fprintf(fps," <path d=\"M %g %g L %g %g\" stroke=\"%s\" stroke-width=\"%g\"%s stroke-opacity=\"%g\"/>\n <g>\n  <text text-anchor=\"middle\" font-family=\"'%s'\" font-size=\"%dpt\" x=\"%g\" y=\"%g\">\n   <tspan dx=\"%g\" dy=\"%g\">%s</tspan>\n  </text>\n  <text text-anchor=\"middle\" font-family=\"'%s'\" font-size=\"%dpt\" x=\"%g\" y=\"%g\">\n   <tspan dx=\"%g\" dy=\"%g\">%s</tspan>\n  </text>\n  <text text-anchor=\"middle\" font-family=\"'%s'\" font-size=\"%dpt\" x=\"%g\" y=\"%g\">\n   <tspan dx=\"%g\" dy=\"%g\">%s</tspan>\n  </text>\n </g>\n",svgxl*CMINPX, svgyl*CMINPX, svgxr*CMINPX, svgyr*CMINPX, svgZweigFarbe, svgZweigDicke, svgzweigstrichelung, (float)svgZweigUndurchsichtigkeit/100.0, svgSchriftartWsk, svgSchriftgroesseWsk, (svgxl+svgxr)/2*CMINPX+svgWskVerschiebung*CMINPX, (svgyl+svgyr)/2*CMINPX-svgWskVerschiebung*SVGM*CMINPX, svgZweigWskAbstand-5, ((SVGM<0)?SVGM:-SVGM)*WskBreiteFormel*CMINPX-(float)SchriftgroesseWsk*4/3-svgZweigWskAbstand, zaehler, svgSchriftartWsk, svgSchriftgroesseWsk, (svgxl+svgxr)/2*CMINPX+svgWskVerschiebung*CMINPX, (svgyl+svgyr)/2*CMINPX-svgWskVerschiebung*SVGM*CMINPX, svgZweigWskAbstand-5, ((SVGM<0)?SVGM:-SVGM)*WskBreiteFormel*CMINPX-(float)SchriftgroesseWsk*4/3-svgZweigWskAbstand, bruchstrich, svgSchriftartWsk, svgSchriftgroesseWsk, (svgxl+svgxr)/2*CMINPX+svgWskVerschiebung*CMINPX, (svgyl+svgyr)/2*CMINPX-svgWskVerschiebung*SVGM*CMINPX, svgZweigWskAbstand-5, ((SVGM<0)?SVGM:-SVGM)*WskBreiteFormel*CMINPX-svgZweigWskAbstand, nenner);
                  }
                  else
                  {
                    fprintf(fps," <path id=\"zweig%d\" d=\"M %g %g L %g %g\" stroke=\"%s\" stroke-width=\"%g\"%s stroke-opacity=\"%g\"/>\n <text text-anchor=\"middle\" font-family=\"'%s'\" font-size=\"%dpt\">\n  <textPath xlink:href=\"#zweig%d\">\n   <tspan dx=\"%g\" dy=\"%g\">%s</tspan>\n  </textPath>\n </text>\n",k, svgxl*CMINPX, svgyl*CMINPX, svgxr*CMINPX, svgyr*CMINPX, svgZweigFarbe, svgZweigDicke, svgzweigstrichelung, (float)svgZweigUndurchsichtigkeit/100.0, svgSchriftartWsk, svgSchriftgroesseWsk, k, wurzel((svgxl-svgxr)*(svgxl-svgxr)+(svgyl-svgyr)*(svgyl-svgyr))/2*CMINPX+svgWskVerschiebung*CMINPX, -svgZweigWskAbstand-svgWskVerschiebung*SVGM*CMINPX, zaehler);
                  }
                }
              }
              else
              {
                fprintf(fps," <path id=\"zweig%d\" d=\"M %g %g L %g %g\" stroke=\"%s\" stroke-width=\"%g\"%s stroke-opacity=\"%g\"/>\n <text text-anchor=\"middle\" font-family=\"'%s'\" font-size=\"%dpt\">\n  <textPath xlink:href=\"#zweig%d\">\n   <tspan dx=\"%g\" dy=\"%g\">%s</tspan>\n  </textPath>\n </text>\n",k, svgxl*CMINPX, svgyl*CMINPX, svgxr*CMINPX, svgyr*CMINPX, svgZweigFarbe, svgZweigDicke, svgzweigstrichelung, (float)svgZweigUndurchsichtigkeit/100.0, svgSchriftartWsk, svgSchriftgroesseWsk, k, wurzel((svgxl-svgxr)*(svgxl-svgxr)+(svgyl-svgyr)*(svgyl-svgyr))/2*CMINPX+svgWskVerschiebung*CMINPX, -svgZweigWskAbstand-svgWskVerschiebung*SVGM*CMINPX, (svgUrneKuerzen)?gekuerzteBruchwahrscheinlichkeit[i][j]:Bruchwahrscheinlichkeit[i][j]);
              }
            }
            else
            {
              if(svgGenauigkeit >= 0)
              {
                if(strlen(WahrscheinlichkeitsString) == 1)
                {
                  WahrscheinlichkeitsString[1] = 44;
                  int zaehlvariable;
                  for(zaehlvariable = 2;zaehlvariable < ((svgGenauigkeit>6)?8:svgGenauigkeit + 2);zaehlvariable++)
                  {
                    WahrscheinlichkeitsString[zaehlvariable] = 48;
                  }
                  WahrscheinlichkeitsString[zaehlvariable] = 0;
                }
                else
                {
                  int zaehlvariable;
                  for(zaehlvariable = 2;zaehlvariable < ((svgGenauigkeit>6)?8:svgGenauigkeit + 2);zaehlvariable++)
                  {
                    if(!WahrscheinlichkeitsString[zaehlvariable])
                    {
                      WahrscheinlichkeitsString[zaehlvariable] = 48;
                    }
                  }
                  if(WahrscheinlichkeitsString[zaehlvariable]>52)
                  {
                    WahrscheinlichkeitsString[zaehlvariable-1] += 1;
                  }
                  WahrscheinlichkeitsString[zaehlvariable] = 0;
                }
              }
              fprintf(fps," <path id=\"zweig%d\" d=\"M %g %g L %g %g\" stroke=\"%s\" stroke-width=\"%g\"%s stroke-opacity=\"%g\"/>\n <text text-anchor=\"middle\" font-family=\"'%s'\" font-size=\"%dpt\">\n  <textPath xlink:href=\"#zweig%d\">\n   <tspan dx=\"%g\" dy=\"%g\">%s</tspan>\n  </textPath>\n </text>\n",k, svgxl*CMINPX, svgyl*CMINPX, svgxr*CMINPX, svgyr*CMINPX, svgZweigFarbe, svgZweigDicke, svgzweigstrichelung, (float)svgZweigUndurchsichtigkeit/100.0, svgSchriftartWsk, svgSchriftgroesseWsk, k, wurzel((svgxl-svgxr)*(svgxl-svgxr)+(svgyl-svgyr)*(svgyl-svgyr))/2*CMINPX+svgWskVerschiebung*CMINPX, -svgZweigWskAbstand, WahrscheinlichkeitsString);
            }
            k++;
          }
        }
      }
      n++;
    }
  }

  if(svg)
  {
    free(svgzweigstrichelung);
  }


  /* Die Ergebnisspalte wird erstellt! */

  if(ErgebnisSpalteAnzeigen && odt)
  {
    n=0;
    int mtmp = 1;
    char *ergebnis;
    ergebnis = calloc(ArgumentAnzahl*(MaximaleLaengeKnoten*50)+1,sizeof(char));
    x = (ArgumentAnzahl - 1) * (KnotenBreite + HorizontalerKnotenAbstand) + 0.5;
    fprintf(fp,"     <draw:rect draw:style-name=\"gr4\" draw:text-style-name=\"P3\" svg:width=\"%gcm\" svg:height=\"%gcm\" svg:x=\"%gcm\" svg:y=\"0cm\">\n      <text:p text:style-name=\"P3\">&#969;</text:p>\n     </draw:rect>\n",ErgebnisBreite,KnotenHoehe,x);
    for(i=1;i<=AnzahlKnotenStufe[ArgumentAnzahl-1];i++)
    {
      if(BruchErgebnisFormel && KnotenHoehe < FormelHoeheErgebnis && ErgebnisSpalteWskAnzeigen)
      {
        y = n * (KnotenHoehe + VertikalerKnotenAbstand) + 0.5 + KnotenHoehe + ( FormelHoeheErgebnis - KnotenHoehe) / 2;
      }
      else
      {
        y = n * (KnotenHoehe + VertikalerKnotenAbstand) + 0.5 + KnotenHoehe;
      }
      strcpy(ergebnis,"\0");
      for(j=ArgumentAnzahl-1;j>0;j--)
      {
        if(j == ArgumentAnzahl-1)
        {
          if(KNOTEN[j][i][0])
          {
            strcat(ergebnis,KNOTEN[j][i]);
          }
        }
        else
        {
          m = 1;
          int summeAim = A[j+1][1];
          if(j == ArgumentAnzahl-2)
          {
            while(i > summeAim)
              {
                m++;
                summeAim += A[j+1][m];
              }
            mtmp = m;
          }
          else
          {
            while(mtmp > summeAim && A[j+1][m+1])
            {
              m++;
              //printf("A[%i][%i]=%i\n",j+1,m,A[j+1][m]);
              summeAim += A[j+1][m];
            }
            mtmp = m;
          }
          if(KNOTEN[j][m][0])
          {
            char hilfsknoten[300];
            strcpy(hilfsknoten,KNOTEN[j][m]);
            strcat(hilfsknoten,ergebnis);
            strcpy(ergebnis,hilfsknoten);
          }
        }
      }
      fprintf(fp,"     <draw:rect draw:style-name=\"gr5\" draw:text-style-name=\"P4\" svg:width=\"%gcm\" svg:height=\"%gcm\" svg:x=\"%gcm\" svg:y=\"%gcm\">\n      <text:p text:style-name=\"P4\">%s</text:p>\n     </draw:rect>\n",ErgebnisBreite,KnotenHoehe,x,y,ergebnis);
      n++;
    }
  free(ergebnis);
  }

  if(svgErgebnisSpalteAnzeigen && svg)
  {
    n=0;
    int mtmp = 1;
    char *ergebnis;
    ergebnis = calloc(ArgumentAnzahl*(MaximaleLaengeKnoten*50)+1,sizeof(char));
    svgx = (ArgumentAnzahl - 1) * (svgKnotenBreite + svgHorizontalerKnotenAbstand) + 0.5;
    fprintf(fps," <svg width=\"%g\" height=\"%g\" x=\"%g\" y=\"%g\">\n  <rect x=\"0\" y=\"0\" width=\"%g\" height=\"%g\" fill=\"%s\" fill-opacity=\"%g\" stroke=\"%s\" stroke-width=\"%g\"%s stroke-opacity=\"%g\"/>\n  <text x=\"%g\" y=\"%g\" text-anchor=\"middle\" font-family=\"'%s'\" font-size=\"%dpt\">&#969;</text>\n </svg>\n",svgErgebnisBreite*CMINPX, svgKnotenHoehe*CMINPX, (svgx+svgSeitenRandLinks)*CMINPX, svgSeitenRandOben*CMINPX, svgErgebnisBreite*CMINPX, svgKnotenHoehe*CMINPX, svgFuellfarbeErgebnisTitel, (float)svgFuellundurchsichtigkeitErgebnisTitel/100.0, svgRahmenErgebnisTitelFarbe, svgRahmenErgebnisTitelDicke, svgrahmenergebnistitelstrichelung, (float)svgRahmenErgebnisTitelUndurchsichtigkeit/100.0, svgErgebnisBreite/2*CMINPX, svgKnotenHoehe/2*CMINPX+svgSchriftgroesseErgebnisTitel*SCHRIFTFAKTOR*2/3, svgSchriftartErgebnisTitel, svgSchriftgroesseErgebnisTitel);
    for(i=1;i<=AnzahlKnotenStufe[ArgumentAnzahl-1];i++)
    {
      if(svgBruchDarstellungErgebnis && svgKnotenHoehe < svgFormelHoeheErgebnis && svgErgebnisSpalteWskAnzeigen)
      {
        svgy = n * (svgKnotenHoehe + svgVertikalerKnotenAbstand) + 0.5 + svgKnotenHoehe + ( svgFormelHoeheErgebnis - svgKnotenHoehe) / 2;
      }
      else
      {
        svgy = n * (svgKnotenHoehe + svgVertikalerKnotenAbstand) + 0.5 + svgKnotenHoehe;
      }
      strcpy(ergebnis,"\0");
      for(j=ArgumentAnzahl-1;j>0;j--)
      {
        if(j == ArgumentAnzahl-1)
        {
          if(svgKNOTEN[j][i][0])
          {
            strcat(ergebnis,svgKNOTEN[j][i]);
          }
        }
        else
        {
          m = 1;
          int summeAim = A[j+1][1];
          if(j == ArgumentAnzahl-2)
          {
            while(i > summeAim)
              {
                m++;
                summeAim += A[j+1][m];
              }
            mtmp = m;
          }
          else
          {
            while(mtmp > summeAim && A[j+1][m+1])
            {
              m++;
              //printf("A[%i][%i]=%i\n",j+1,m,A[j+1][m]);
              summeAim += A[j+1][m];
            }
            mtmp = m;
          }
          if(svgKNOTEN[j][m][0])
          {
            char hilfsknoten[300];
            strcpy(hilfsknoten,svgKNOTEN[j][m]);
            strcat(hilfsknoten,ergebnis);
            strcpy(ergebnis,hilfsknoten);
          }
        }
      }
      fprintf(fps," <svg width=\"%g\" height=\"%g\" x=\"%g\" y=\"%g\">\n  <rect x=\"0\" y=\"0\" width=\"%g\" height=\"%g\" fill=\"%s\" fill-opacity=\"%g\" stroke=\"%s\" stroke-width=\"%g\"%s stroke-opacity=\"%g\"/>\n  <text x=\"%g\" y=\"%g\" text-anchor=\"middle\" font-family=\"'%s'\" font-size=\"%dpt\">%s</text>\n </svg>\n",svgErgebnisBreite*CMINPX, svgKnotenHoehe*CMINPX, (svgx+svgSeitenRandLinks)*CMINPX, (svgy+svgSeitenRandOben)*CMINPX, svgErgebnisBreite*CMINPX, svgKnotenHoehe*CMINPX, svgFuellfarbeErgebnis, (float)svgFuellundurchsichtigkeitErgebnis/100.0, svgRahmenErgebnisFarbe, svgRahmenErgebnisDicke, svgrahmenergebnisstrichelung, (float)svgRahmenErgebnisUndurchsichtigkeit/100.0, svgErgebnisBreite/2*CMINPX, svgKnotenHoehe/2*CMINPX+svgSchriftgroesseErgebnis*SCHRIFTFAKTOR*2/3, svgSchriftartErgebnis, svgSchriftgroesseErgebnis, ergebnis);
      n++;
    }
  free(ergebnis);
  }

  if(odt)
  {
    for(j=0; j < ArgumentAnzahl; j++)
    {
      free(yKoordinaten[j]);
    }
    free(yKoordinaten);
  }

  if(svg)
  {
    for(j=0; j < ArgumentAnzahl; j++)
    {
      free(svgyKoordinaten[j]);
    }
    free(svgyKoordinaten);
  }

  if(odt)
  {
    for(j=0; j < ArgumentAnzahl; j++)
    {
      for(k=0; k < MaximaleAnzahlKnotenProStufe+1; k++)
      {
        free(KNOTEN[j][k]);
      }
      free(KNOTEN[j]);
    }
    free(KNOTEN);
  }

  if(svg)
  {
    for(j=0; j < ArgumentAnzahl; j++)
    {
      for(k=0; k < MaximaleAnzahlKnotenProStufe+1; k++)
      {
        free(svgKNOTEN[j][k]);
      }
      free(svgKNOTEN[j]);
    }
    free(svgKNOTEN);
  }

  if(svg)
  {
    free(svgrahmenergebnistitelstrichelung);
    free(svgrahmenergebnisstrichelung);
  }


  /* Die Spalte für die Pfadwahrscheinlichkeiten wird erstellt! */

  if(ErgebnisSpalteWskAnzeigen && !BruchErgebnis && !BruchZweige && odt)
  {
    n=0;
    int mtmp = 1;
    if(ErgebnisSpalteAnzeigen)
    {
      x = (ArgumentAnzahl - 1) * (KnotenBreite + HorizontalerKnotenAbstand) + ErgebnisBreite + 1;
    }
    else
    {
      x = (ArgumentAnzahl - 1) * (KnotenBreite + HorizontalerKnotenAbstand) + 0.5;
    }
    fprintf(fp,"     <draw:rect draw:style-name=\"gr6\" draw:text-style-name=\"P5\" svg:width=\"%gcm\" svg:height=\"%gcm\" svg:x=\"%gcm\" svg:y=\"0cm\">\n      <text:p text:style-name=\"P5\">P(&#969;)</text:p>\n     </draw:rect>\n",WskBreite,KnotenHoehe,x);
    for(i=1;i<=AnzahlKnotenStufe[ArgumentAnzahl-1];i++)
    {
      float pfadwahrscheinlichkeit = 1;
      y = n * (KnotenHoehe + VertikalerKnotenAbstand) + 0.5 + KnotenHoehe;
      for(j=ArgumentAnzahl-1;j>0;j--)
      {
        if(j == ArgumentAnzahl-1)
        {
          pfadwahrscheinlichkeit *= Wahrscheinlichkeit[j][i];
        }
        else
        {
          m = 1;
          int summeAim = A[j+1][1];
          if(j == ArgumentAnzahl-2)
          {
            while(i > summeAim)
              {
                m++;
                summeAim += A[j+1][m];
              }
            mtmp = m;
          }
          else
          {
            while(mtmp > summeAim)
            {
              m++;
              summeAim += A[j+1][m];
            }
            mtmp = m;
          }
          pfadwahrscheinlichkeit *= Wahrscheinlichkeit[j][m];
        }
      }
      if(Genauigkeit == -1)
      {
        if(pfadwahrscheinlichkeit<0.0001)
        {
          if(pfadwahrscheinlichkeit == 0)
          {
            WahrscheinlichkeitsString[0] = 48;
            WahrscheinlichkeitsString[1] = 0;
          }
          else
          {
            sprintf(WahrscheinlichkeitsString,"%g",pfadwahrscheinlichkeit);
/*            while(WahrscheinlichkeitsString[strlen(WahrscheinlichkeitsString)-1] == 48)
            {
              WahrscheinlichkeitsString[strlen(WahrscheinlichkeitsString)-1] = 0;
            }
            WahrscheinlichkeitsString[1]=44;*/
          }
        }
        else
        {
          if(pfadwahrscheinlichkeit == 1)
          {
            WahrscheinlichkeitsString[0] = 49;
            WahrscheinlichkeitsString[1] = 0;
          }
          else
          {
            sprintf(WahrscheinlichkeitsString,"%g",pfadwahrscheinlichkeit);
            WahrscheinlichkeitsString[1]=44;
          }
        }
      }
      else
      {
        sprintf(WahrscheinlichkeitsString,"%f",pfadwahrscheinlichkeit);
        if(WahrscheinlichkeitsString[Genauigkeit+2]>52)
        {
          WahrscheinlichkeitsString[Genauigkeit+1] += 1;
        }
        WahrscheinlichkeitsString[Genauigkeit+2]=0;
        WahrscheinlichkeitsString[1]=44;
      }
      fprintf(fp,"     <draw:rect draw:style-name=\"gr7\" draw:text-style-name=\"P6\" svg:width=\"%gcm\" svg:height=\"%gcm\" svg:x=\"%gcm\" svg:y=\"%gcm\">\n      <text:p text:style-name=\"P6\">%s</text:p>\n     </draw:rect>\n",WskBreite,KnotenHoehe,x,y,WahrscheinlichkeitsString);
      n++;
    }
  }

  if(ErgebnisSpalteWskAnzeigen && BruchErgebnis && BruchZweige && odt)
  {
    n=0;
    int mtmp = 1;
    char Hilfsbruchwahrscheinlichkeit[44];
    if(ErgebnisSpalteAnzeigen)
    {
      x = (ArgumentAnzahl - 1) * (KnotenBreite + HorizontalerKnotenAbstand) + ErgebnisBreite + 1;
    }
    else
    {
      x = (ArgumentAnzahl - 1) * (KnotenBreite + HorizontalerKnotenAbstand) + 0.5;
    }
    fprintf(fp,"     <draw:rect draw:style-name=\"gr6\" draw:text-style-name=\"P5\" svg:width=\"%gcm\" svg:height=\"%gcm\" svg:x=\"%gcm\" svg:y=\"0cm\">\n      <text:p text:style-name=\"P5\">P(&#969;)</text:p>\n     </draw:rect>\n",WskBreite,KnotenHoehe,x);
    if(Gruppierung == 2 && BruchErgebnisFormel && odt)
    {
      fprintf(fp,"    </draw:g>\n");
    }
    for(i=1;i<=AnzahlKnotenStufe[ArgumentAnzahl-1];i++)
    {
      char pfadwahrscheinlichkeit[44] = "1/1";
      if(BruchErgebnisFormel && KnotenHoehe < FormelHoeheErgebnis)
      {
        y = n * (KnotenHoehe + VertikalerKnotenAbstand) + 0.5 + KnotenHoehe + ( FormelHoeheErgebnis - KnotenHoehe ) / 2;
      }
      else
      {
        y = n * (KnotenHoehe + VertikalerKnotenAbstand) + 0.5 + KnotenHoehe;
      }
      for(j=ArgumentAnzahl-1;j>0;j--)
      {
        if(j == ArgumentAnzahl-1)
        {
          strcpy(Hilfsbruchwahrscheinlichkeit,Bruchwahrscheinlichkeit[j][i]);
          bruchmultiplikation(pfadwahrscheinlichkeit, Hilfsbruchwahrscheinlichkeit, pfadwahrscheinlichkeit);
        }
        else
        {
          m = 1;
          int summeAim = A[j+1][1];
          if(j == ArgumentAnzahl-2)
          {
            while(i > summeAim)
              {
                m++;
                summeAim += A[j+1][m];
              }
            mtmp = m;
          }
          else
          {
            while(mtmp > summeAim)
            {
              m++;
              summeAim += A[j+1][m];
            }
            mtmp = m;
          }
          strcpy(Hilfsbruchwahrscheinlichkeit,Bruchwahrscheinlichkeit[j][m]);
          bruchmultiplikation(pfadwahrscheinlichkeit, Hilfsbruchwahrscheinlichkeit, pfadwahrscheinlichkeit);
        }
      }

      if(BruchErgebnisFormel)
      {
        char hilfspfadwahrscheinlichkeit[202];
        strcpy(hilfspfadwahrscheinlichkeit,pfadwahrscheinlichkeit);
//        //printf("Hilfspfadwsk: %s\n",hilfspfadwahrscheinlichkeit);
        char *bruchteil;
        bruchteil = strtok(hilfspfadwahrscheinlichkeit,"/");
        strcpy(ZaehlerErgebnis[n+1],bruchteil);
//        //printf("ZaehlerErgebnis: %s\n",ZaehlerErgebnis[n+1]);
        bruchteil = strtok(NULL,"/");
        if(bruchteil == NULL)
        {
//          //printf("NULL\n");
          strcpy(NennerErgebnis[n+1],"1");
        }
        else
        {
          strcpy(NennerErgebnis[n+1],bruchteil);
//          //printf("NennerErgebnis: %s\n",NennerErgebnis[n+1]);
        }

        if(strlen(ZaehlerErgebnis[n+1]) <= strlen(NennerErgebnis[n+1]))
        {
          WskBreiteFormel = 0.21 + (float)SchriftgroesseWskErgebnis / 150 + strlen(NennerErgebnis[n+1]) * ((float)SchriftgroesseWskErgebnis * 11 / 600);
        }
        else
        {
          WskBreiteFormel = 0.21 + (float)SchriftgroesseWskErgebnis / 150 + strlen(ZaehlerErgebnis[n+1]) * ((float)SchriftgroesseWskErgebnis * 11 / 600);
        }
        float xformel = x + ( WskBreite - WskBreiteFormel ) / 2;
        y -= ( FormelHoeheErgebnis - KnotenHoehe ) / 2;
        fprintf(fp,"     <draw:frame draw:style-name=\"fr1\" draw:name=\"Formel%i\" text:anchor-type=\"paragraph\" svg:x=\"%gcm\" svg:y=\"%gcm\" svg:width=\"%gcm\" svg:height=\"%gcm\" draw:z-index=\"1\">\n      <draw:object xlink:href=\"./Formel%i\" xlink:type=\"simple\" xlink:show=\"embed\" xlink:actuate=\"onLoad\"/>\n     </draw:frame>\n",n+1,xformel,y,WskBreiteFormel,FormelHoeheErgebnis,n+1);
      }
      else
      {
        fprintf(fp,"     <draw:rect draw:style-name=\"gr7\" draw:text-style-name=\"P6\" svg:width=\"%gcm\" svg:height=\"%gcm\" svg:x=\"%gcm\" svg:y=\"%gcm\">\n      <text:p text:style-name=\"P6\">%s</text:p>\n     </draw:rect>\n",WskBreite,KnotenHoehe,x,y,pfadwahrscheinlichkeit);
      }
      n++;
    }
  }

  if(ErgebnisSpalteWskAnzeigen && !BruchErgebnis && BruchZweige && odt)
  {
    n=0;
    int mtmp = 1;
    char Hilfsbruchwahrscheinlichkeit[44];
    if(ErgebnisSpalteAnzeigen)
    {
      x = (ArgumentAnzahl - 1) * (KnotenBreite + HorizontalerKnotenAbstand) + ErgebnisBreite + 1;
    }
    else
    {
      x = (ArgumentAnzahl - 1) * (KnotenBreite + HorizontalerKnotenAbstand) + 0.5;
    }
    fprintf(fp,"     <draw:rect draw:style-name=\"gr6\" draw:text-style-name=\"P5\" svg:width=\"%gcm\" svg:height=\"%gcm\" svg:x=\"%gcm\" svg:y=\"0cm\">\n      <text:p text:style-name=\"P5\">P(&#969;)</text:p>\n     </draw:rect>\n",WskBreite,KnotenHoehe,x);
    for(i=1;i<=AnzahlKnotenStufe[ArgumentAnzahl-1];i++)
    {
      float pfadwahrscheinlichkeit = 1;
      y = n * (KnotenHoehe + VertikalerKnotenAbstand) + 0.5 + KnotenHoehe;
      for(j=ArgumentAnzahl-1;j>0;j--)
      {
        if(j == ArgumentAnzahl-1)
        {
          strcpy(Hilfsbruchwahrscheinlichkeit,Bruchwahrscheinlichkeit[j][i]);
          pfadwahrscheinlichkeit *= bruchzufloat(Hilfsbruchwahrscheinlichkeit);
        }
        else
        {
          m = 1;
          int summeAim = A[j+1][1];
          if(j == ArgumentAnzahl-2)
          {
            while(i > summeAim)
              {
                m++;
                summeAim += A[j+1][m];
              }
            mtmp = m;
          }
          else
          {
            while(mtmp > summeAim)
            {
              m++;
              summeAim += A[j+1][m];
            }
            mtmp = m;
          }
          strcpy(Hilfsbruchwahrscheinlichkeit,Bruchwahrscheinlichkeit[j][m]);
          pfadwahrscheinlichkeit *= bruchzufloat(Hilfsbruchwahrscheinlichkeit);
        }
      }
      if(Genauigkeit == -1)
      {
        if(pfadwahrscheinlichkeit<0.0001)
        {
          if(pfadwahrscheinlichkeit == 0)
          {
            WahrscheinlichkeitsString[0] = 48;
            WahrscheinlichkeitsString[1] = 0;
          }
          else
          {
            sprintf(WahrscheinlichkeitsString,"%g",pfadwahrscheinlichkeit);
/*            while(WahrscheinlichkeitsString[strlen(WahrscheinlichkeitsString)-1] == 48)
            {
              WahrscheinlichkeitsString[strlen(WahrscheinlichkeitsString)-1] = 0;
            }
            WahrscheinlichkeitsString[1]=44;*/
          }
        }
        else
        {
          if(pfadwahrscheinlichkeit == 1)
          {
            WahrscheinlichkeitsString[0] = 49;
            WahrscheinlichkeitsString[1] = 0;
          }
          else
          {
            sprintf(WahrscheinlichkeitsString,"%g",pfadwahrscheinlichkeit);
            WahrscheinlichkeitsString[1]=44;
          }
        }
      }
      else
      {
        sprintf(WahrscheinlichkeitsString,"%f",pfadwahrscheinlichkeit);
        if(WahrscheinlichkeitsString[Genauigkeit+2]>52)
        {
          WahrscheinlichkeitsString[Genauigkeit+1] += 1;
        }
        WahrscheinlichkeitsString[Genauigkeit+2]=0;
        WahrscheinlichkeitsString[1]=44;
      }
      fprintf(fp,"     <draw:rect draw:style-name=\"gr7\" draw:text-style-name=\"P6\" svg:width=\"%gcm\" svg:height=\"%gcm\" svg:x=\"%gcm\" svg:y=\"%gcm\">\n      <text:p text:style-name=\"P6\">%s</text:p>\n     </draw:rect>\n",WskBreite,KnotenHoehe,x,y,WahrscheinlichkeitsString);
      n++;
    }
  }

  if(ErgebnisSpalteWskAnzeigen && BruchErgebnis && !BruchZweige && odt)
  {
    n=0;
    int mtmp = 1;
    char Hilfsbruchwahrscheinlichkeit[44];
    if(ErgebnisSpalteAnzeigen)
    {
      x = (ArgumentAnzahl - 1) * (KnotenBreite + HorizontalerKnotenAbstand) + ErgebnisBreite + 1;
    }
    else
    {
      x = (ArgumentAnzahl - 1) * (KnotenBreite + HorizontalerKnotenAbstand) + 0.5;
    }
    fprintf(fp,"     <draw:rect draw:style-name=\"gr6\" draw:text-style-name=\"P5\" svg:width=\"%gcm\" svg:height=\"%gcm\" svg:x=\"%gcm\" svg:y=\"0cm\">\n      <text:p text:style-name=\"P5\">P(&#969;)</text:p>\n     </draw:rect>\n",WskBreite,KnotenHoehe,x);
    if(Gruppierung == 2 && BruchErgebnisFormel && odt)
    {
      fprintf(fp,"    </draw:g>\n");
    }
    for(i=1;i<=AnzahlKnotenStufe[ArgumentAnzahl-1];i++)
    {
      char pfadwahrscheinlichkeit[44] = "1/1";
      if(BruchErgebnisFormel && KnotenHoehe < FormelHoeheErgebnis)
      {
        y = n * (KnotenHoehe + VertikalerKnotenAbstand) + 0.5 + KnotenHoehe + ( FormelHoeheErgebnis - KnotenHoehe ) / 2;
      }
      else
      {
        y = n * (KnotenHoehe + VertikalerKnotenAbstand) + 0.5 + KnotenHoehe;
      }
      for(j=ArgumentAnzahl-1;j>0;j--)
      {
        if(j == ArgumentAnzahl-1)
        {
          floatzubruch(Wahrscheinlichkeit[j][i],Hilfsbruchwahrscheinlichkeit);
          bruchmultiplikation(pfadwahrscheinlichkeit, Hilfsbruchwahrscheinlichkeit, pfadwahrscheinlichkeit);
        }
        else
        {
          m = 1;
          int summeAim = A[j+1][1];
          if(j == ArgumentAnzahl-2)
          {
            while(i > summeAim)
              {
                m++;
                summeAim += A[j+1][m];
              }
            mtmp = m;
          }
          else
          {
            while(mtmp > summeAim)
            {
              m++;
              summeAim += A[j+1][m];
            }
            mtmp = m;
          }
          floatzubruch(Wahrscheinlichkeit[j][m],Hilfsbruchwahrscheinlichkeit);
          bruchmultiplikation(pfadwahrscheinlichkeit, Hilfsbruchwahrscheinlichkeit, pfadwahrscheinlichkeit);
        }
      }
      if(BruchErgebnisFormel)
      {
        char hilfspfadwahrscheinlichkeit[202];
        strcpy(hilfspfadwahrscheinlichkeit,pfadwahrscheinlichkeit);
        char *bruchteil;
        bruchteil = strtok(hilfspfadwahrscheinlichkeit,"/");
        if(bruchteil)
        {
          strcpy(ZaehlerErgebnis[n+1],bruchteil);
        }
        bruchteil = strtok(NULL,"/");
        if(bruchteil)
        {
          strcpy(NennerErgebnis[n+1],bruchteil);
        }
        else
        {
          strcpy(NennerErgebnis[n+1],"1");
        }

        if(strlen(ZaehlerErgebnis[n+1]) <= strlen(NennerErgebnis[n+1]))
        {
          WskBreiteFormel = 0.21 + (float)SchriftgroesseWskErgebnis / 150 + strlen(NennerErgebnis[n+1]) * ((float)SchriftgroesseWskErgebnis * 11 / 600);
        }
        else
        {
          WskBreiteFormel = 0.21 + (float)SchriftgroesseWskErgebnis / 150 + strlen(ZaehlerErgebnis[n+1]) * ((float)SchriftgroesseWskErgebnis * 11 / 600);
        }
        float xformel = x + ( WskBreite - WskBreiteFormel ) / 2;
        y -= ( FormelHoeheErgebnis - KnotenHoehe ) / 2;
        fprintf(fp,"     <draw:frame draw:style-name=\"fr1\" draw:name=\"Formel%i\" text:anchor-type=\"paragraph\" svg:x=\"%gcm\" svg:y=\"%gcm\" svg:width=\"%gcm\" svg:height=\"%gcm\" draw:z-index=\"1\">\n      <draw:object xlink:href=\"./Formel%i\" xlink:type=\"simple\" xlink:show=\"embed\" xlink:actuate=\"onLoad\"/>\n     </draw:frame>\n",n+1,xformel,y,WskBreiteFormel,FormelHoeheErgebnis,n+1);
      }
      else
      {
        fprintf(fp,"     <draw:rect draw:style-name=\"gr7\" draw:text-style-name=\"P6\" svg:width=\"%gcm\" svg:height=\"%gcm\" svg:x=\"%gcm\" svg:y=\"%gcm\">\n      <text:p text:style-name=\"P6\">%s</text:p>\n     </draw:rect>\n",WskBreite,KnotenHoehe,x,y,pfadwahrscheinlichkeit);
      }
      n++;
    }
  }

  if(svgErgebnisSpalteWskAnzeigen && !svgBruchErgebnis && !svgBruchZweige && svg)
  {
    n=0;
    int mtmp = 1;
    if(svgErgebnisSpalteAnzeigen)
    {
      x = (ArgumentAnzahl - 1) * (svgKnotenBreite + svgHorizontalerKnotenAbstand) + svgErgebnisBreite + 1;
    }
    else
    {
      x = (ArgumentAnzahl - 1) * (svgKnotenBreite + svgHorizontalerKnotenAbstand) + 0.5;
    }
    fprintf(fps," <svg width=\"%g\" height=\"%g\" x=\"%g\" y=\"%g\">\n  <rect x=\"0\" y=\"0\" width=\"%g\" height=\"%g\" fill=\"%s\" fill-opacity=\"%g\" stroke=\"%s\" stroke-width=\"%g\"%s stroke-opacity=\"%g\"/>\n  <text x=\"%g\" y=\"%g\" text-anchor=\"middle\" font-family=\"'%s'\" font-size=\"%dpt\">P(&#969;)</text>\n </svg>\n",svgWskBreite*CMINPX, svgKnotenHoehe*CMINPX, (x+svgSeitenRandLinks)*CMINPX, svgSeitenRandOben*CMINPX, svgWskBreite*CMINPX, svgKnotenHoehe*CMINPX, svgFuellfarbeWskErgebnisTitel, (float)svgFuellundurchsichtigkeitWskErgebnisTitel/100.0, svgRahmenWskErgebnisTitelFarbe, svgRahmenWskErgebnisTitelDicke, svgrahmenwskergebnistitelstrichelung, (float)svgRahmenWskErgebnisTitelUndurchsichtigkeit/100.0, svgWskBreite/2*CMINPX, svgKnotenHoehe/2*CMINPX+svgSchriftgroesseWskErgebnisTitel*SCHRIFTFAKTOR*2/3, svgSchriftartWskErgebnisTitel, svgSchriftgroesseWskErgebnisTitel);
    fflush(fps);
    for(i=1;i<=AnzahlKnotenStufe[ArgumentAnzahl-1];i++)
    {
      float pfadwahrscheinlichkeit = 1;
      y = n * (svgKnotenHoehe + svgVertikalerKnotenAbstand) + 0.5 + svgKnotenHoehe;
      for(j=ArgumentAnzahl-1;j>0;j--)
      {
        if(j == ArgumentAnzahl-1)
        {
          pfadwahrscheinlichkeit *= Wahrscheinlichkeit[j][i];
        }
        else
        {
          m = 1;
          int summeAim = A[j+1][1];
          if(j == ArgumentAnzahl-2)
          {
            while(i > summeAim)
              {
                m++;
                summeAim += A[j+1][m];
              }
            mtmp = m;
          }
          else
          {
            while(mtmp > summeAim)
            {
              m++;
              summeAim += A[j+1][m];
            }
            mtmp = m;
          }
          pfadwahrscheinlichkeit *= Wahrscheinlichkeit[j][m];
        }
      }
      if(svgGenauigkeit == -1)
      {
        if(pfadwahrscheinlichkeit<0.0001)
        {
          if(pfadwahrscheinlichkeit == 0)
          {
            WahrscheinlichkeitsString[0] = 48;
            WahrscheinlichkeitsString[1] = 0;
          }
          else
          {
            sprintf(WahrscheinlichkeitsString,"%g",pfadwahrscheinlichkeit);
            while(WahrscheinlichkeitsString[strlen(WahrscheinlichkeitsString)-1] == 48)
            {
              WahrscheinlichkeitsString[strlen(WahrscheinlichkeitsString)-1] = 0;
            }
            WahrscheinlichkeitsString[1]=44;
          }
        }
        else
        {
          if(pfadwahrscheinlichkeit == 1)
          {
            WahrscheinlichkeitsString[0] = 49;
            WahrscheinlichkeitsString[1] = 0;
          }
          else
          {
            sprintf(WahrscheinlichkeitsString,"%g",pfadwahrscheinlichkeit);
            WahrscheinlichkeitsString[1]=44;
          }
        }
      }
      else
      {
        sprintf(WahrscheinlichkeitsString,"%g",pfadwahrscheinlichkeit);
        if(WahrscheinlichkeitsString[svgGenauigkeit+2] > 52)
        {
          WahrscheinlichkeitsString[svgGenauigkeit+1] += 1;
        }
        WahrscheinlichkeitsString[svgGenauigkeit+2] = 0;
        WahrscheinlichkeitsString[1] = 44;
      }
      fprintf(fps," <svg width=\"%g\" height=\"%g\" x=\"%g\" y=\"%g\">\n  <rect x=\"0\" y=\"0\" width=\"%g\" height=\"%g\" fill=\"%s\" fill-opacity=\"%g\" stroke=\"%s\" stroke-width=\"%g\"%s stroke-opacity=\"%g\"/>\n  <text x=\"%g\" y=\"%g\" text-anchor=\"middle\" font-family=\"'%s'\" font-size=\"%dpt\">%s</text>\n </svg>\n",svgWskBreite*CMINPX, svgKnotenHoehe*CMINPX, (x+svgSeitenRandLinks)*CMINPX, (y+svgSeitenRandOben)*CMINPX, svgWskBreite*CMINPX, svgKnotenHoehe*CMINPX, svgFuellfarbeWskErgebnis, (float)svgFuellundurchsichtigkeitWskErgebnis/100.0, svgRahmenWskErgebnisFarbe, svgRahmenWskErgebnisDicke, svgrahmenwskergebnisstrichelung, (float)svgRahmenWskErgebnisUndurchsichtigkeit/100.0, svgWskBreite/2*CMINPX, svgKnotenHoehe/2*CMINPX+svgSchriftgroesseWskErgebnis*SCHRIFTFAKTOR*2/3, svgSchriftartWskErgebnis, svgSchriftgroesseWskErgebnis, WahrscheinlichkeitsString);
      n++;
    }
  }

  if(svgErgebnisSpalteWskAnzeigen && svgBruchErgebnis && svgBruchZweige && svg)
  {
    n=0;
    int mtmp = 1;
    char Hilfsbruchwahrscheinlichkeit[44];
    if(svgErgebnisSpalteAnzeigen)
    {
      x = (ArgumentAnzahl - 1) * (svgKnotenBreite + svgHorizontalerKnotenAbstand) + svgErgebnisBreite + 1;
    }
    else
    {
      x = (ArgumentAnzahl - 1) * (svgKnotenBreite + svgHorizontalerKnotenAbstand) + 0.5;
    }
    fprintf(fps," <svg width=\"%g\" height=\"%g\" x=\"%g\" y=\"%g\">\n  <rect x=\"0\" y=\"0\" width=\"%g\" height=\"%g\" fill=\"%s\" fill-opacity=\"%g\" stroke=\"%s\" stroke-width=\"%g\"%s stroke-opacity=\"%g\"/>\n  <text x=\"%g\" y=\"%g\" text-anchor=\"middle\" font-family=\"'%s'\" font-size=\"%dpt\">P(&#969;)</text>\n </svg>\n",svgWskBreite*CMINPX, svgKnotenHoehe*CMINPX, (x+svgSeitenRandLinks)*CMINPX, svgSeitenRandOben*CMINPX, svgWskBreite*CMINPX, svgKnotenHoehe*CMINPX, svgFuellfarbeWskErgebnisTitel, (float)svgFuellundurchsichtigkeitWskErgebnisTitel/100.0, svgRahmenWskErgebnisTitelFarbe, svgRahmenWskErgebnisTitelDicke, svgrahmenwskergebnistitelstrichelung, (float)svgRahmenWskErgebnisTitelUndurchsichtigkeit/100.0, svgWskBreite/2*CMINPX, svgKnotenHoehe/2*CMINPX+svgSchriftgroesseWskErgebnisTitel*SCHRIFTFAKTOR*2/3, svgSchriftartWskErgebnisTitel, svgSchriftgroesseWskErgebnisTitel);
    for(i=1;i<=AnzahlKnotenStufe[ArgumentAnzahl-1];i++)
    {
      char pfadwahrscheinlichkeit[44] = "1/1";
      if(svgBruchDarstellungErgebnis && svgKnotenHoehe < svgFormelHoeheErgebnis)
      {
        y = n * (svgKnotenHoehe + svgVertikalerKnotenAbstand) + 0.5 + svgKnotenHoehe + ( svgFormelHoeheErgebnis - svgKnotenHoehe ) / 2;
      }
      else
      {
        y = n * (svgKnotenHoehe + svgVertikalerKnotenAbstand) + 0.5 + svgKnotenHoehe;
      }
      for(j=ArgumentAnzahl-1;j>0;j--)
      {
        if(j == ArgumentAnzahl-1)
        {
          strcpy(Hilfsbruchwahrscheinlichkeit,Bruchwahrscheinlichkeit[j][i]);
          bruchmultiplikation(pfadwahrscheinlichkeit, Hilfsbruchwahrscheinlichkeit, pfadwahrscheinlichkeit);
        }
        else
        {
          m = 1;
          int summeAim = A[j+1][1];
          if(j == ArgumentAnzahl-2)
          {
            while(i > summeAim)
              {
                m++;
                summeAim += A[j+1][m];
              }
            mtmp = m;
          }
          else
          {
            while(mtmp > summeAim)
            {
              m++;
              summeAim += A[j+1][m];
            }
            mtmp = m;
          }
          strcpy(Hilfsbruchwahrscheinlichkeit,Bruchwahrscheinlichkeit[j][m]);
          bruchmultiplikation(pfadwahrscheinlichkeit, Hilfsbruchwahrscheinlichkeit, pfadwahrscheinlichkeit);
        }
      }

      if(svgBruchDarstellungErgebnis)
      {
        char hilfspfadwahrscheinlichkeit[202];
        strcpy(hilfspfadwahrscheinlichkeit,pfadwahrscheinlichkeit);
//        //printf("Hilfspfadwsk: %s\n",hilfspfadwahrscheinlichkeit);
        char *bruchteil;
        bruchteil = strtok(hilfspfadwahrscheinlichkeit,"/");
        if(bruchteil)
        {
          strcpy(ZaehlerErgebnis[n+1],bruchteil);
        }
        else
        {
          strcpy(ZaehlerErgebnis[n+1],"0");
        }
//        //printf("ZaehlerErgebnis: %s\n",ZaehlerErgebnis[n+1]);
        bruchteil = strtok(NULL,"/");
        if(bruchteil == NULL)
        {
//          //printf("NULL\n");
          strcpy(NennerErgebnis[n+1],"1");
        }
        else
        {
          strcpy(NennerErgebnis[n+1],bruchteil);
//          //printf("NennerErgebnis: %s\n",NennerErgebnis[n+1]);
        }

        y -= ( svgFormelHoeheErgebnis - svgKnotenHoehe ) / 2;
        int zaehlerlaenge = 0, nennerlaenge = 0;
        for(k=0;k<strlen(pfadwahrscheinlichkeit);k++)
        {
          if(pfadwahrscheinlichkeit[k] == 47)
          {
            zaehlerlaenge = k;
          }
        }
        nennerlaenge = strlen(pfadwahrscheinlichkeit) - zaehlerlaenge - 1;
        char zaehler[21] = "", bruchstrich[21] = "", nenner[21] = "";
        for(k=0;k<zaehlerlaenge;k++)
        {
          zaehler[k] = pfadwahrscheinlichkeit[k];
        //printf("zaehlerlänge=%d\nzaehler[%d] = %c\n",zaehlerlaenge,k,zaehler[k]);
        }
        if(zaehlerlaenge == strlen(pfadwahrscheinlichkeit))
        {
          nenner[0] = 49;
          bruchstrich[0] = 0;
        }
        else
        {
          for(k=0;k<((zaehlerlaenge<nennerlaenge)?nennerlaenge:zaehlerlaenge);k++)
          {
            bruchstrich[k] = 95;
          }
          for(k=0;k<nennerlaenge;k++)
          {
            nenner[k] = pfadwahrscheinlichkeit[k+zaehlerlaenge+1];
          }
        }
        fprintf(fps," <svg width=\"%g\" height=\"%g\" x=\"%g\" y=\"%g\">\n  <rect x=\"0\" y=\"0\" width=\"%g\" height=\"%g\" fill=\"%s\" fill-opacity=\"%g\" stroke=\"%s\" stroke-width=\"%g\"%s stroke-opacity=\"%g\"/>\n  <text x=\"%g\" y=\"%g\" text-anchor=\"middle\" font-family=\"'%s'\" font-size=\"%dpt\" dy=\"-%g\">%s</text>\n  <text x=\"%g\" y=\"%g\" text-anchor=\"middle\" font-family=\"'%s'\" font-size=\"%dpt\" dy=\"-%g\">%s</text>\n  <text x=\"%g\" y=\"%g\" text-anchor=\"middle\" font-family=\"'%s'\" font-size=\"%dpt\" dy=\"%g\">%s</text>\n </svg>\n",svgWskBreite*CMINPX, svgFormelHoeheErgebnis*CMINPX, (x+svgSeitenRandLinks)*CMINPX, (y+svgSeitenRandOben)*CMINPX, svgWskBreite*CMINPX, svgFormelHoeheErgebnis*CMINPX, svgFuellfarbeWskErgebnis, (float)svgFuellundurchsichtigkeitWskErgebnis/100.0, svgRahmenWskErgebnisFarbe, svgRahmenWskErgebnisDicke, svgrahmenwskergebnisstrichelung, (float)svgRahmenWskErgebnisUndurchsichtigkeit/100.0, svgWskBreite/2*CMINPX, svgFormelHoeheErgebnis/2*CMINPX+svgSchriftgroesseWskErgebnis*SCHRIFTFAKTOR*2/3, svgSchriftartWskErgebnis, svgSchriftgroesseWskErgebnis, 0.75*svgSchriftgroesseWskErgebnis, zaehler, svgWskBreite/2*CMINPX, svgFormelHoeheErgebnis/2*CMINPX+svgSchriftgroesseWskErgebnis*SCHRIFTFAKTOR*2/3, svgSchriftartWskErgebnis, svgSchriftgroesseWskErgebnis, 0.75*svgSchriftgroesseWskErgebnis, bruchstrich, svgWskBreite/2*CMINPX, svgFormelHoeheErgebnis/2*CMINPX+svgSchriftgroesseWskErgebnis*SCHRIFTFAKTOR*2/3, svgSchriftartWskErgebnis, svgSchriftgroesseWskErgebnis, 0.75*svgSchriftgroesseWskErgebnis, nenner);
      }
      else
      {
        fprintf(fps," <svg width=\"%g\" height=\"%g\" x=\"%g\" y=\"%g\">\n  <rect x=\"0\" y=\"0\" width=\"%g\" height=\"%g\" fill=\"%s\" fill-opacity=\"%g\" stroke=\"%s\" stroke-width=\"%g\"%s stroke-opacity=\"%g\"/>\n  <text x=\"%g\" y=\"%g\" text-anchor=\"middle\" font-family=\"'%s'\" font-size=\"%dpt\">%s</text>\n </svg>\n",svgWskBreite*CMINPX, svgKnotenHoehe*CMINPX, (x+svgSeitenRandLinks)*CMINPX, (y+svgSeitenRandOben)*CMINPX, svgWskBreite*CMINPX, svgKnotenHoehe*CMINPX, svgFuellfarbeWskErgebnis, (float)svgFuellundurchsichtigkeitWskErgebnis/100.0, svgRahmenWskErgebnisFarbe, svgRahmenWskErgebnisDicke, svgrahmenwskergebnisstrichelung, (float)svgRahmenWskErgebnisUndurchsichtigkeit/100.0, svgWskBreite/2*CMINPX, svgKnotenHoehe/2*CMINPX+svgSchriftgroesseWskErgebnis*SCHRIFTFAKTOR*2/3, svgSchriftartWskErgebnis, svgSchriftgroesseWskErgebnis, pfadwahrscheinlichkeit);
      }
      n++;
    }
  }

  if(svgErgebnisSpalteWskAnzeigen && !svgBruchErgebnis && svgBruchZweige && svg)
  {
    n=0;
    int mtmp = 1;
    char Hilfsbruchwahrscheinlichkeit[44];
    if(svgErgebnisSpalteAnzeigen)
    {
      x = (ArgumentAnzahl - 1) * (svgKnotenBreite + svgHorizontalerKnotenAbstand) + svgErgebnisBreite + 1;
    }
    else
    {
      x = (ArgumentAnzahl - 1) * (svgKnotenBreite + svgHorizontalerKnotenAbstand) + 0.5;
    }
    fprintf(fps," <svg width=\"%g\" height=\"%g\" x=\"%g\" y=\"%g\">\n  <rect x=\"0\" y=\"0\" width=\"%g\" height=\"%g\" fill=\"%s\" fill-opacity=\"%g\" stroke=\"%s\" stroke-width=\"%g\"%s stroke-opacity=\"%g\"/>\n  <text x=\"%g\" y=\"%g\" text-anchor=\"middle\" font-family=\"'%s'\" font-size=\"%dpt\">P(&#969;)</text>\n </svg>\n",svgWskBreite*CMINPX, svgKnotenHoehe*CMINPX, (x+svgSeitenRandLinks)*CMINPX, svgSeitenRandOben*CMINPX, svgWskBreite*CMINPX, svgKnotenHoehe*CMINPX, svgFuellfarbeWskErgebnisTitel, (float)svgFuellundurchsichtigkeitWskErgebnisTitel/100.0, svgRahmenWskErgebnisTitelFarbe, svgRahmenWskErgebnisTitelDicke, svgrahmenwskergebnistitelstrichelung, (float)svgRahmenWskErgebnisTitelUndurchsichtigkeit/100.0, svgWskBreite/2*CMINPX, svgKnotenHoehe/2*CMINPX+svgSchriftgroesseWskErgebnisTitel*SCHRIFTFAKTOR*2/3, svgSchriftartWskErgebnisTitel, svgSchriftgroesseWskErgebnisTitel);
    for(i=1;i<=AnzahlKnotenStufe[ArgumentAnzahl-1];i++)
    {
      float pfadwahrscheinlichkeit = 1;
      y = n * (svgKnotenHoehe + svgVertikalerKnotenAbstand) + 0.5 + svgKnotenHoehe;
      for(j=ArgumentAnzahl-1;j>0;j--)
      {
        if(j == ArgumentAnzahl-1)
        {
          strcpy(Hilfsbruchwahrscheinlichkeit,Bruchwahrscheinlichkeit[j][i]);
          pfadwahrscheinlichkeit *= bruchzufloat(Hilfsbruchwahrscheinlichkeit);
        }
        else
        {
          m = 1;
          int summeAim = A[j+1][1];
          if(j == ArgumentAnzahl-2)
          {
            while(i > summeAim)
              {
                m++;
                summeAim += A[j+1][m];
              }
            mtmp = m;
          }
          else
          {
            while(mtmp > summeAim)
            {
              m++;
              summeAim += A[j+1][m];
            }
            mtmp = m;
          }
          strcpy(Hilfsbruchwahrscheinlichkeit,Bruchwahrscheinlichkeit[j][m]);
          pfadwahrscheinlichkeit *= bruchzufloat(Hilfsbruchwahrscheinlichkeit);
        }
      }
      if(svgGenauigkeit == -1)
      {
        if(pfadwahrscheinlichkeit<0.0001)
        {
          if(pfadwahrscheinlichkeit == 0)
          {
            WahrscheinlichkeitsString[0] = 48;
            WahrscheinlichkeitsString[1] = 0;
          }
          else
          {
            sprintf(WahrscheinlichkeitsString,"%g",pfadwahrscheinlichkeit);
            while(WahrscheinlichkeitsString[strlen(WahrscheinlichkeitsString)-1] == 48)
            {
              WahrscheinlichkeitsString[strlen(WahrscheinlichkeitsString)-1] = 0;
            }
            WahrscheinlichkeitsString[1]=44;
          }
        }
        else
        {
          if(pfadwahrscheinlichkeit == 1)
          {
            WahrscheinlichkeitsString[0] = 49;
            WahrscheinlichkeitsString[1] = 0;
          }
          else
          {
            sprintf(WahrscheinlichkeitsString,"%f",pfadwahrscheinlichkeit);
            WahrscheinlichkeitsString[1]=44;
          }
        }
      }
      else
      {
        sprintf(WahrscheinlichkeitsString,"%f",pfadwahrscheinlichkeit);
        if(WahrscheinlichkeitsString[svgGenauigkeit+2] > 52)
        {
          WahrscheinlichkeitsString[svgGenauigkeit+1] += 1;
        }
        WahrscheinlichkeitsString[svgGenauigkeit+2] = 0;
        WahrscheinlichkeitsString[1]=44;
      }
      fprintf(fps," <svg width=\"%g\" height=\"%g\" x=\"%g\" y=\"%g\">\n  <rect x=\"0\" y=\"0\" width=\"%g\" height=\"%g\" fill=\"%s\" fill-opacity=\"%g\" stroke=\"%s\" stroke-width=\"%g\"%s stroke-opacity=\"%g\"/>\n  <text x=\"%g\" y=\"%g\" text-anchor=\"middle\" font-family=\"'%s'\" font-size=\"%dpt\">%s</text>\n </svg>\n",svgWskBreite*CMINPX, svgKnotenHoehe*CMINPX, (x+svgSeitenRandLinks)*CMINPX, (y+svgSeitenRandOben)*CMINPX, svgWskBreite*CMINPX, svgKnotenHoehe*CMINPX, svgFuellfarbeWskErgebnis, (float)svgFuellundurchsichtigkeitWskErgebnis/100.0, svgRahmenWskErgebnisFarbe, svgRahmenWskErgebnisDicke, svgrahmenwskergebnisstrichelung, (float)svgRahmenWskErgebnisUndurchsichtigkeit/100.0, svgWskBreite/2*CMINPX, svgKnotenHoehe/2*CMINPX+svgSchriftgroesseWskErgebnis*SCHRIFTFAKTOR*2/3, svgSchriftartWskErgebnis, svgSchriftgroesseWskErgebnis, WahrscheinlichkeitsString);
      n++;
    }
  }

  if(svgErgebnisSpalteWskAnzeigen && svgBruchErgebnis && !svgBruchZweige && svg)
  {
    n=0;
    int mtmp = 1;
    char Hilfsbruchwahrscheinlichkeit[44];
    if(svgErgebnisSpalteAnzeigen)
    {
      x = (ArgumentAnzahl - 1) * (svgKnotenBreite + svgHorizontalerKnotenAbstand) + svgErgebnisBreite + 1;
    }
    else
    {
      x = (ArgumentAnzahl - 1) * (svgKnotenBreite + svgHorizontalerKnotenAbstand) + 0.5;
    }
    fprintf(fps," <svg width=\"%g\" height=\"%g\" x=\"%g\" y=\"%g\">\n  <rect x=\"0\" y=\"0\" width=\"%g\" height=\"%g\" fill=\"%s\" fill-opacity=\"%g\" stroke=\"%s\" stroke-width=\"%g\"%s stroke-opacity=\"%g\"/>\n  <text x=\"%g\" y=\"%g\" text-anchor=\"middle\" font-family=\"'%s'\" font-size=\"%dpt\">P(&#969;)</text>\n </svg>\n",svgWskBreite*CMINPX, svgKnotenHoehe*CMINPX, (x+svgSeitenRandLinks)*CMINPX, svgSeitenRandOben*CMINPX, svgWskBreite*CMINPX, svgKnotenHoehe*CMINPX, svgFuellfarbeWskErgebnisTitel, (float)svgFuellundurchsichtigkeitWskErgebnisTitel/100.0, svgRahmenWskErgebnisTitelFarbe, svgRahmenWskErgebnisTitelDicke, svgrahmenwskergebnistitelstrichelung, (float)svgRahmenWskErgebnisTitelUndurchsichtigkeit/100.0, svgWskBreite/2*CMINPX, svgKnotenHoehe/2*CMINPX+svgSchriftgroesseWskErgebnisTitel*SCHRIFTFAKTOR*2/3, svgSchriftartWskErgebnisTitel, svgSchriftgroesseWskErgebnisTitel);
    for(i=1;i<=AnzahlKnotenStufe[ArgumentAnzahl-1];i++)
    {
      char pfadwahrscheinlichkeit[44] = "1/1";
      if(svgBruchDarstellungErgebnis && svgKnotenHoehe < svgFormelHoeheErgebnis)
      {
        y = n * (svgKnotenHoehe + svgVertikalerKnotenAbstand) + 0.5 + svgKnotenHoehe + ( svgFormelHoeheErgebnis - svgKnotenHoehe ) / 2;
      }
      else
      {
        y = n * (svgKnotenHoehe + svgVertikalerKnotenAbstand) + 0.5 + svgKnotenHoehe;
      }
      for(j=ArgumentAnzahl-1;j>0;j--)
      {
        if(j == ArgumentAnzahl-1)
        {
          floatzubruch(Wahrscheinlichkeit[j][i],Hilfsbruchwahrscheinlichkeit);
          bruchmultiplikation(pfadwahrscheinlichkeit, Hilfsbruchwahrscheinlichkeit, pfadwahrscheinlichkeit);
        }
        else
        {
          m = 1;
          int summeAim = A[j+1][1];
          if(j == ArgumentAnzahl-2)
          {
            while(i > summeAim)
              {
                m++;
                summeAim += A[j+1][m];
              }
            mtmp = m;
          }
          else
          {
            while(mtmp > summeAim)
            {
              m++;
              summeAim += A[j+1][m];
            }
            mtmp = m;
          }
          floatzubruch(Wahrscheinlichkeit[j][m],Hilfsbruchwahrscheinlichkeit);
          bruchmultiplikation(pfadwahrscheinlichkeit, Hilfsbruchwahrscheinlichkeit, pfadwahrscheinlichkeit);
        }
      }
      if(svgBruchDarstellungErgebnis)
      {
        char hilfspfadwahrscheinlichkeit[202];
        strcpy(hilfspfadwahrscheinlichkeit,pfadwahrscheinlichkeit);
        char *bruchteil;
        bruchteil = strtok(hilfspfadwahrscheinlichkeit,"/");
        if(bruchteil)
        {
          strcpy(ZaehlerErgebnis[n+1],bruchteil);
        }
        else
        {
          strcpy(ZaehlerErgebnis[n+1],"0");
        }
        bruchteil = strtok(NULL,"/");
        if(bruchteil)
        {
          strcpy(NennerErgebnis[n+1],bruchteil);
        }
        else
        {
          strcpy(NennerErgebnis[n+1],"1");
        }

        y -= ( svgFormelHoeheErgebnis - svgKnotenHoehe ) / 2;
        int zaehlerlaenge = 0, nennerlaenge = 0;
        for(k=0;k<strlen(pfadwahrscheinlichkeit);k++)
        {
          if(pfadwahrscheinlichkeit[k] == 47)
          {
            zaehlerlaenge = k;
          }
        }
        nennerlaenge = strlen(pfadwahrscheinlichkeit) - zaehlerlaenge - 1;
        char zaehler[21] = "", bruchstrich[21] = "", nenner[21] = "";
        for(k=0;k<zaehlerlaenge;k++)
        {
          zaehler[k] = pfadwahrscheinlichkeit[k];
        //printf("zaehlerlänge=%d\nzaehler[%d] = %c\n",zaehlerlaenge,k,zaehler[k]);
        }
        if(zaehlerlaenge == strlen(pfadwahrscheinlichkeit))
        {
          nenner[0] = 49;
          bruchstrich[0] = 95;
        }
        else
        {
          for(k=0;k<((zaehlerlaenge<nennerlaenge)?nennerlaenge:zaehlerlaenge);k++)
          {
            bruchstrich[k] = 95;
          }
          for(k=0;k<nennerlaenge;k++)
          {
            nenner[k] = pfadwahrscheinlichkeit[k+zaehlerlaenge+1];
          }
        }
        fprintf(fps," <svg width=\"%g\" height=\"%g\" x=\"%g\" y=\"%g\">\n  <rect x=\"0\" y=\"0\" width=\"%g\" height=\"%g\" fill=\"%s\" fill-opacity=\"%g\" stroke=\"%s\" stroke-width=\"%g\"%s stroke-opacity=\"%g\"/>\n  <text x=\"%g\" y=\"%g\" text-anchor=\"middle\" font-family=\"'%s'\" font-size=\"%dpt\" dy=\"-%g\">%s</text>\n  <text x=\"%g\" y=\"%g\" text-anchor=\"middle\" font-family=\"'%s'\" font-size=\"%dpt\" dy=\"-%g\">%s</text>\n  <text x=\"%g\" y=\"%g\" text-anchor=\"middle\" font-family=\"'%s'\" font-size=\"%dpt\" dy=\"%g\">%s</text>\n </svg>\n",svgWskBreite*CMINPX, svgFormelHoeheErgebnis*CMINPX, (x+svgSeitenRandLinks)*CMINPX, (y+svgSeitenRandOben)*CMINPX, svgWskBreite*CMINPX, svgFormelHoeheErgebnis*CMINPX, svgFuellfarbeWskErgebnis, (float)svgFuellundurchsichtigkeitWskErgebnis/100.0, svgRahmenWskErgebnisFarbe, svgRahmenWskErgebnisDicke, svgrahmenwskergebnisstrichelung, (float)svgRahmenWskErgebnisUndurchsichtigkeit/100.0, svgWskBreite/2*CMINPX, svgFormelHoeheErgebnis/2*CMINPX+svgSchriftgroesseWskErgebnis*SCHRIFTFAKTOR*2/3, svgSchriftartWskErgebnis, svgSchriftgroesseWskErgebnis, 0.75*svgSchriftgroesseWskErgebnis, zaehler, svgWskBreite/2*CMINPX, svgFormelHoeheErgebnis/2*CMINPX+svgSchriftgroesseWskErgebnis*SCHRIFTFAKTOR*2/3, svgSchriftartWskErgebnis, svgSchriftgroesseWskErgebnis, 0.75*svgSchriftgroesseWskErgebnis, bruchstrich, svgWskBreite/2*CMINPX, svgFormelHoeheErgebnis/2*CMINPX+svgSchriftgroesseWskErgebnis*SCHRIFTFAKTOR*2/3, svgSchriftartWskErgebnis, svgSchriftgroesseWskErgebnis, 0.75*svgSchriftgroesseWskErgebnis, nenner);
      }
      else
      {
        fprintf(fps," <svg width=\"%g\" height=\"%g\" x=\"%g\" y=\"%g\">\n  <rect x=\"0\" y=\"0\" width=\"%g\" height=\"%g\" fill=\"%s\" fill-opacity=\"%g\" stroke=\"%s\" stroke-width=\"%g\"%s stroke-opacity=\"%g\"/>\n  <text x=\"%g\" y=\"%g\" text-anchor=\"middle\" font-family=\"'%s'\" font-size=\"%dpt\">%s</text>\n </svg>\n",svgWskBreite*CMINPX, svgKnotenHoehe*CMINPX, (x+svgSeitenRandLinks)*CMINPX, (y+svgSeitenRandOben)*CMINPX, svgWskBreite*CMINPX, svgKnotenHoehe*CMINPX, svgFuellfarbeWskErgebnis, (float)svgFuellundurchsichtigkeitWskErgebnis/100.0, svgRahmenWskErgebnisFarbe, svgRahmenWskErgebnisDicke, svgrahmenwskergebnisstrichelung, (float)svgRahmenWskErgebnisUndurchsichtigkeit/100.0, svgWskBreite/2*CMINPX, svgKnotenHoehe/2*CMINPX+svgSchriftgroesseWskErgebnis*SCHRIFTFAKTOR*2/3, svgSchriftartWskErgebnis, svgSchriftgroesseWskErgebnis, pfadwahrscheinlichkeit);
      }
      n++;
    }
  }

  if(svg)
  {
    free(svgrahmenwskergebnistitelstrichelung);
    free(svgrahmenwskergebnisstrichelung);
  }

  if(wsk)
  {
    for(j=0; j < ArgumentAnzahl; j++)
    {
      for(k=0; k < MaximaleAnzahlKnotenProStufe+2; k++)
      {
        free(Bruchwahrscheinlichkeit[j][k]);
      }
      free(Bruchwahrscheinlichkeit[j]);
    }
    free(Bruchwahrscheinlichkeit);
  }

  if(wsk && UrneKuerzen)
  {
    for(j=0; j < ArgumentAnzahl; j++)
    {
      for(k=0; k < MaximaleAnzahlKnotenProStufe+1; k++)
      {
        free(gekuerzteBruchwahrscheinlichkeit[j][k]);
      }
      free(gekuerzteBruchwahrscheinlichkeit[j]);
    }
    free(gekuerzteBruchwahrscheinlichkeit);
  }

  for(j=0; j < ArgumentAnzahl; j++)
  {
    free(A[j]);
  }
  free(A);

  if(wsk)
  {
    for(j=0; j < ArgumentAnzahl; j++)
    {
      free(Wahrscheinlichkeit[j]);
    }
    free(Wahrscheinlichkeit);
  }

  if(multinomial)
  {
    for(i=0;i<ArgumentAnzahl;i++)
    {
      free(Argument[i]);
    }
    if(wsk)
    {
      for(i=ArgumentAnzahl;i<2*ArgumentAnzahl;i++)
      {
        free(Argument[i]);
      }
    }
    free(Argument);
  }

  //printf("ARGUMENTANZAHLnachher: %i\n",ArgumentAnzahl);
  //printf("IF: %i\n",(wsk)?2*ArgumentAnzahl:ArgumentAnzahl);
  int hilfarg=(wsk)?2*ArgumentAnzahl:ArgumentAnzahl;
  if(unabhaengig)
  {
    for(i=0;i<hilfarg;i++)
    {
      free(Argument[i]);
    }
    free(Argument);
  }


  /* Die Datei content.xml wird abgeschlossen! */

  if(odt)
  {
    if(Gruppierung == 0)
    {
      fprintf(fp,"    </draw:g>\n");
    }
    fprintf(fp,"  </office:text>\n </office:body>\n</office:document-content>");
    fclose(fp);
  }
  if(svg)
  {
    fprintf(fps,"</svg>");
    fclose(fps);

    char aufruf2[4097] = "";
    if(svgProgramm[0] != 48)
    {
      /*if(strchr(svgProgramm, 38) || strchr(svgProgramm, 59) || strchr(svgProgramm, 124) || strchr(svgProgramm, 35) || strchr(svgProgramm, 40) || strchr(svgProgramm, 41) || strchr(svgProgramm, 123) || strchr(svgProgramm, 125))
      {
        fprintf(stderr,"\n\tWarnung: Eines dieser Zeichen erkannt: ; & | # ( ) { }\n\tDiese sind gefährlich!\n\tNicht im Programm-Namen bzw. -Pfad verwenden.\n\n");
        exit(0);
      }*/
      strcpy(aufruf2,svgProgramm);
      strcat(aufruf2," ");
      strcat(aufruf2,svgdatei);
      strcat(aufruf2," 2>/dev/null &");
      system(aufruf2);
    }
  }


  /* Hier wird die Datei manifest.xml erstellt! */

  if(odt)
  {
    fp=fopen(MANIFEST,"a");
    fprintf(fp,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<manifest:manifest xmlns:manifest=\"urn:oasis:names:tc:opendocument:xmlns:manifest:1.0\" manifest:version=\"1.2\">\n <manifest:file-entry manifest:full-path=\"/\" manifest:version=\"1.2\" manifest:media-type=\"application/vnd.oasis.opendocument.text\"/>\n <manifest:file-entry manifest:full-path=\"content.xml\" manifest:media-type=\"text/xml\"/>\n");
    if(BruchErgebnisFormel && wsk)
    {
      for(i=1;i<=AnzahlKnotenLetzteStufe;i++)
      {
        fprintf(fp," <manifest:file-entry manifest:full-path=\"Formel%i/content.xml\" manifest:media-type=\"text/xml\"/>\n <manifest:file-entry manifest:full-path=\"Formel%i/settings.xml\" manifest:media-type=\"text/xml\"/>\n <manifest:file-entry manifest:full-path=\"Formel%i/\" manifest:version=\"1.2\" manifest:media-type=\"application/vnd.oasis.opendocument.formula\"/>",i,i,i);
      }
    }
    if(BruchZweigeFormel && wsk)
    {
      n=1;
      for(i=1;i<ArgumentAnzahl;i++)
      {
        for(j=1;j<=AnzahlKnotenStufe[i];j++)
        {
          fprintf(fp," <manifest:file-entry manifest:full-path=\"FormelZweig%i/content.xml\" manifest:media-type=\"text/xml\"/>\n <manifest:file-entry manifest:full-path=\"FormelZweig%i/settings.xml\" manifest:media-type=\"text/xml\"/>\n <manifest:file-entry manifest:full-path=\"FormelZweig%i/\" manifest:version=\"1.2\" manifest:media-type=\"application/vnd.oasis.opendocument.formula\"/>",n,n,n);
          n++;
        }
      }
    }
    fprintf(fp," <manifest:file-entry manifest:full-path=\"styles.xml\" manifest:media-type=\"text/xml\"/>\n</manifest:manifest>\n");
    fclose(fp);
  }



  /* Hier wird die Datei styles.xml erstellt! */

  if(odt)
  {
    fp=fopen(STYLES,"a");
    fprintf(fp,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n<office:document-styles xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\" xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\" xmlns:table=\"urn:oasis:names:tc:opendocument:xmlns:table:1.0\" xmlns:draw=\"urn:oasis:names:tc:opendocument:xmlns:drawing:1.0\" xmlns:fo=\"urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\" xmlns:number=\"urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0\" xmlns:svg=\"urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0\" xmlns:chart=\"urn:oasis:names:tc:opendocument:xmlns:chart:1.0\" xmlns:dr3d=\"urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0\" xmlns:math=\"http://www.w3.org/1998/Math/MathML\" xmlns:form=\"urn:oasis:names:tc:opendocument:xmlns:form:1.0\" xmlns:script=\"urn:oasis:names:tc:opendocument:xmlns:script:1.0\" xmlns:ooo=\"http://openoffice.org/2004/office\" xmlns:ooow=\"http://openoffice.org/2004/writer\" xmlns:oooc=\"http://openoffice.org/2004/calc\" xmlns:dom=\"http://www.w3.org/2001/xml-events\" xmlns:rpt=\"http://openoffice.org/2005/report\" xmlns:of=\"urn:oasis:names:tc:opendocument:xmlns:of:1.2\" xmlns:xhtml=\"http://www.w3.org/1999/xhtml\" xmlns:grddl=\"http://www.w3.org/2003/g/data-view#\" xmlns:tableooo=\"http://openoffice.org/2009/table\" xmlns:css3t=\"http://www.w3.org/TR/css3-text/\" office:version=\"1.2\">\n <office:styles>\n  <draw:gradient draw:name=\"FarbverlaufKnoten\" draw:display-name=\"Farbverlauf Knoten\" draw:style=\"%s\" draw:cx=\"%d%%\" draw:cy=\"%d%%\" draw:start-color=\"#%s\" draw:end-color=\"#%s\" draw:start-intensity=\"%d%%\" draw:end-intensity=\"%d%%\" draw:angle=\"%d\" draw:border=\"%d%%\"/>\n  <draw:gradient draw:name=\"FarbverlaufErgebnisTitel\" draw:display-name=\"Farbverlauf Ergebnis (Überschrift)\" draw:style=\"%s\" draw:cx=\"%d%%\" draw:cy=\"%d%%\" draw:start-color=\"#%s\" draw:end-color=\"#%s\" draw:start-intensity=\"%d%%\" draw:end-intensity=\"%d%%\" draw:angle=\"%d\" draw:border=\"%d%%\"/>\n  <draw:gradient draw:name=\"FarbverlaufErgebnis\" draw:display-name=\"Farbverlauf Ergebnis\" draw:style=\"%s\" draw:cx=\"%d%%\" draw:cy=\"%d%%\" draw:start-color=\"#%s\" draw:end-color=\"#%s\" draw:start-intensity=\"%d%%\" draw:end-intensity=\"%d%%\" draw:angle=\"%d\" draw:border=\"%d%%\"/>\n  <draw:gradient draw:name=\"FarbverlaufWskErgebnisTitel\" draw:display-name=\"Farbverlauf Wahrscheinlichkeit (Überschrift)\" draw:style=\"%s\" draw:cx=\"%d%%\" draw:cy=\"%d%%\" draw:start-color=\"#%s\" draw:end-color=\"#%s\" draw:start-intensity=\"%d%%\" draw:end-intensity=\"%d%%\" draw:angle=\"%d\" draw:border=\"%d%%\"/>\n  <draw:gradient draw:name=\"FarbverlaufWskErgebnis\" draw:display-name=\"Farbverlauf Wahrscheinichkeit\" draw:style=\"%s\" draw:cx=\"%d%%\" draw:cy=\"%d%%\" draw:start-color=\"#%s\" draw:end-color=\"#%s\" draw:start-intensity=\"%d%%\" draw:end-intensity=\"%d%%\" draw:angle=\"%d\" draw:border=\"%d%%\"/>\n  <draw:stroke-dash draw:name=\"StrichelungZweig\" draw:display-name=\"Strichelung Zweige\" draw:dots1=\"%d\" draw:dots1-length=\"%gcm\" draw:dots2=\"%d\" draw:dots2-length=\"%gcm\" draw:distance=\"%gcm\"/>\n  <draw:stroke-dash draw:name=\"StrichelungKnoten\" draw:display-name=\"Strichelung Knotenrahmen\" draw:dots1=\"%d\" draw:dots1-length=\"%gcm\" draw:dots2=\"%d\" draw:dots2-length=\"%gcm\" draw:distance=\"%gcm\"/>\n  <draw:stroke-dash draw:name=\"StrichelungErgebnisTitel\" draw:display-name=\"Strichelung Ergebnisrahmen (Überschrift)\" draw:dots1=\"%d\" draw:dots1-length=\"%gcm\" draw:dots2=\"%d\" draw:dots2-length=\"%gcm\" draw:distance=\"%gcm\"/>\n  <draw:stroke-dash draw:name=\"StrichelungErgebnis\" draw:display-name=\"Strichelung Ergebnisrahmen\" draw:dots1=\"%d\" draw:dots1-length=\"%gcm\" draw:dots2=\"%d\" draw:dots2-length=\"%gcm\" draw:distance=\"%gcm\"/>\n  <draw:stroke-dash draw:name=\"StrichelungWskErgebnisTitel\" draw:display-name=\"Strichelung Wahrscheinlichkeiten (Überschrift)\" draw:dots1=\"%d\" draw:dots1-length=\"%gcm\" draw:dots2=\"%d\" draw:dots2-length=\"%gcm\" draw:distance=\"%gcm\"/>\n  <draw:stroke-dash draw:name=\"StrichelungWskErgebnis\" draw:display-name=\"Strichelung Wahrscheinlichkeiten\" draw:dots1=\"%d\" draw:dots1-length=\"%gcm\" draw:dots2=\"%d\" draw:dots2-length=\"%gcm\" draw:distance=\"%gcm\"/>\n  <draw:hatch draw:name=\"SchraffurKnoten\" draw:display-name=\"Schraffur Knoten\" draw:style=\"%s\" draw:color=\"#%s\" draw:distance=\"%gcm\" draw:rotation=\"%d\"/>\n  <draw:hatch draw:name=\"SchraffurErgebnisTitel\" draw:display-name=\"Schraffur ErgebnisTitel\" draw:style=\"%s\" draw:color=\"#%s\" draw:distance=\"%gcm\" draw:rotation=\"%d\"/>\n  <draw:hatch draw:name=\"SchraffurErgebnis\" draw:display-name=\"Schraffur Ergebnis\" draw:style=\"%s\" draw:color=\"#%s\" draw:distance=\"%gcm\" draw:rotation=\"%d\"/>\n  <draw:hatch draw:name=\"SchraffurWskErgebnisTitel\" draw:display-name=\"Schraffur WskErgebnisTitel\" draw:style=\"%s\" draw:color=\"#%s\" draw:distance=\"%gcm\" draw:rotation=\"%d\"/>\n  <draw:hatch draw:name=\"SchraffurWskErgebnis\" draw:display-name=\"Schraffur WskErgebnis\" draw:style=\"%s\" draw:color=\"#%s\" draw:distance=\"%gcm\" draw:rotation=\"%d\"/>\n  <draw:opacity draw:name=\"TransparenzverlaufKnoten\" draw:display-name=\"Transparency Knoten\" draw:style=\"%s\" draw:cx=\"%d%%\" draw:cy=\"%d%%\" draw:start=\"%d%%\" draw:end=\"%d%%\" draw:angle=\"%d\" draw:border=\"%d%%\"/>\n  <draw:opacity draw:name=\"TransparenzverlaufErgebnisTitel\" draw:display-name=\"Transparency ErgebnisTitel\" draw:style=\"%s\" draw:cx=\"%d%%\" draw:cy=\"%d%%\" draw:start=\"%d%%\" draw:end=\"%d%%\" draw:angle=\"%d\" draw:border=\"%d%%\"/>\n  <draw:opacity draw:name=\"TransparenzverlaufErgebnis\" draw:display-name=\"Transparency Ergebnis\" draw:style=\"%s\" draw:cx=\"%d%%\" draw:cy=\"%d%%\" draw:start=\"%d%%\" draw:end=\"%d%%\" draw:angle=\"%d\" draw:border=\"%d%%\"/>\n  <draw:opacity draw:name=\"TransparenzverlaufWskErgebnisTitel\" draw:display-name=\"Transparency WskErgebnisTitel\" draw:style=\"%s\" draw:cx=\"%d%%\" draw:cy=\"%d%%\" draw:start=\"%d%%\" draw:end=\"%d%%\" draw:angle=\"%d\" draw:border=\"%d%%\"/>\n  <draw:opacity draw:name=\"TransparenzverlaufWskErgebnis\" draw:display-name=\"Transparency WskErgebnis\" draw:style=\"%s\" draw:cx=\"%d%%\" draw:cy=\"%d%%\" draw:start=\"%d%%\" draw:end=\"%d%%\" draw:angle=\"%d\" draw:border=\"%d%%\"/>\n </office:styles>\n <office:automatic-styles>\n  <style:page-layout style:name=\"Mpm1\">\n   <style:page-layout-properties fo:page-width=\"%gcm\" fo:page-height=\"%gcm\" style:num-format=\"1\" style:print-orientation=\"portrait\" fo:margin-top=\"%gcm\" fo:margin-bottom=\"%gcm\" fo:margin-left=\"%gcm\" fo:margin-right=\"%gcm\" style:writing-mode=\"lr-tb\" style:footnote-max-height=\"0cm\">\n    <style:footnote-sep style:width=\"0.018cm\" style:distance-before-sep=\"0.101cm\" style:distance-after-sep=\"0.101cm\" style:line-style=\"solid\" style:adjustment=\"left\" style:rel-width=\"25%%\" style:color=\"#000000\"/>\n   </style:page-layout-properties>\n   <style:header-style/>\n   <style:footer-style/>\n  </style:page-layout>\n </office:automatic-styles>\n <office:master-styles>\n  <style:master-page style:name=\"Standard\" style:page-layout-name=\"Mpm1\"/>\n </office:master-styles>\n</office:document-styles>",FuellungFarbverlaufArtKnoten, FuellungFarbverlaufZentrumXKnoten, FuellungFarbverlaufZentrumYKnoten, FuellungFarbverlaufAnfangsfarbeKnoten, FuellungFarbverlaufEndfarbeKnoten, FuellungFarbverlaufAnfangsintensitaetKnoten, FuellungFarbverlaufEndintensitaetKnoten, FuellungFarbverlaufWinkelKnoten, FuellungFarbverlaufRandKnoten, FuellungFarbverlaufArtErgebnisTitel, FuellungFarbverlaufZentrumXErgebnisTitel, FuellungFarbverlaufZentrumYErgebnisTitel, FuellungFarbverlaufAnfangsfarbeErgebnisTitel, FuellungFarbverlaufEndfarbeErgebnisTitel, FuellungFarbverlaufAnfangsintensitaetErgebnisTitel, FuellungFarbverlaufEndintensitaetErgebnisTitel, FuellungFarbverlaufWinkelErgebnisTitel, FuellungFarbverlaufRandErgebnisTitel, FuellungFarbverlaufArtErgebnis, FuellungFarbverlaufZentrumXErgebnis, FuellungFarbverlaufZentrumYErgebnis, FuellungFarbverlaufAnfangsfarbeErgebnis, FuellungFarbverlaufEndfarbeErgebnis, FuellungFarbverlaufAnfangsintensitaetErgebnis, FuellungFarbverlaufEndintensitaetErgebnis, FuellungFarbverlaufWinkelErgebnis, FuellungFarbverlaufRandErgebnis, FuellungFarbverlaufArtWskErgebnisTitel, FuellungFarbverlaufZentrumXWskErgebnisTitel, FuellungFarbverlaufZentrumYWskErgebnisTitel, FuellungFarbverlaufAnfangsfarbeWskErgebnisTitel, FuellungFarbverlaufEndfarbeWskErgebnisTitel, FuellungFarbverlaufAnfangsintensitaetWskErgebnisTitel, FuellungFarbverlaufEndintensitaetWskErgebnisTitel, FuellungFarbverlaufWinkelWskErgebnisTitel, FuellungFarbverlaufRandWskErgebnisTitel, FuellungFarbverlaufArtWskErgebnis, FuellungFarbverlaufZentrumXWskErgebnis, FuellungFarbverlaufZentrumYWskErgebnis, FuellungFarbverlaufAnfangsfarbeWskErgebnis, FuellungFarbverlaufEndfarbeWskErgebnis, FuellungFarbverlaufAnfangsintensitaetWskErgebnis, FuellungFarbverlaufEndintensitaetWskErgebnis, FuellungFarbverlaufWinkelWskErgebnis, FuellungFarbverlaufRandWskErgebnis, ZweigStrichelungStrichAnzahl1, ZweigStrichelungStrichLaenge1, ZweigStrichelungStrichAnzahl2, ZweigStrichelungStrichLaenge2, ZweigStrichelungAbstand, RahmenKnotenStrichelungStrichAnzahl1, RahmenKnotenStrichelungStrichLaenge1, RahmenKnotenStrichelungStrichAnzahl2, RahmenKnotenStrichelungStrichLaenge2, RahmenKnotenStrichelungAbstand, RahmenErgebnisTitelStrichelungStrichAnzahl1, RahmenErgebnisTitelStrichelungStrichLaenge1, RahmenErgebnisTitelStrichelungStrichAnzahl2, RahmenErgebnisTitelStrichelungStrichLaenge2, RahmenErgebnisTitelStrichelungAbstand, RahmenErgebnisStrichelungStrichAnzahl1, RahmenErgebnisStrichelungStrichLaenge1, RahmenErgebnisStrichelungStrichAnzahl2, RahmenErgebnisStrichelungStrichLaenge2, RahmenErgebnisStrichelungAbstand, RahmenWskErgebnisTitelStrichelungStrichAnzahl1, RahmenWskErgebnisTitelStrichelungStrichLaenge1, RahmenWskErgebnisTitelStrichelungStrichAnzahl2, RahmenWskErgebnisTitelStrichelungStrichLaenge2, RahmenWskErgebnisTitelStrichelungAbstand, RahmenWskErgebnisStrichelungStrichAnzahl1, RahmenWskErgebnisStrichelungStrichLaenge1, RahmenWskErgebnisStrichelungStrichAnzahl2, RahmenWskErgebnisStrichelungStrichLaenge2, RahmenWskErgebnisStrichelungAbstand, FuellungSchraffurArtKnotenString, FuellungSchraffurFarbeKnoten, FuellungSchraffurAbstandKnoten, FuellungSchraffurRotationKnoten, FuellungSchraffurArtErgebnisTitelString, FuellungSchraffurFarbeErgebnisTitel, FuellungSchraffurAbstandErgebnisTitel, FuellungSchraffurRotationErgebnisTitel, FuellungSchraffurArtErgebnisString, FuellungSchraffurFarbeErgebnis, FuellungSchraffurAbstandErgebnis, FuellungSchraffurRotationErgebnis, FuellungSchraffurArtWskErgebnisTitelString, FuellungSchraffurFarbeWskErgebnisTitel, FuellungSchraffurAbstandWskErgebnisTitel, FuellungSchraffurRotationWskErgebnisTitel, FuellungSchraffurArtWskErgebnisString, FuellungSchraffurFarbeWskErgebnis, FuellungSchraffurAbstandWskErgebnis, FuellungSchraffurRotationWskErgebnis, FuellungTransparenzverlaufArtKnoten, FuellungTransparenzverlaufZentrumXKnoten, FuellungTransparenzverlaufZentrumYKnoten, FuellungTransparenzverlaufAnfangKnoten, FuellungTransparenzverlaufEndKnoten, FuellungTransparenzverlaufWinkelKnoten, FuellungTransparenzverlaufRandKnoten, FuellungTransparenzverlaufArtErgebnisTitel, FuellungTransparenzverlaufZentrumXErgebnisTitel, FuellungTransparenzverlaufZentrumYErgebnisTitel, FuellungTransparenzverlaufAnfangErgebnisTitel, FuellungTransparenzverlaufEndErgebnisTitel, FuellungTransparenzverlaufWinkelErgebnisTitel, FuellungTransparenzverlaufRandErgebnisTitel, FuellungTransparenzverlaufArtErgebnis, FuellungTransparenzverlaufZentrumXErgebnis, FuellungTransparenzverlaufZentrumYErgebnis, FuellungTransparenzverlaufAnfangErgebnis, FuellungTransparenzverlaufEndErgebnis, FuellungTransparenzverlaufWinkelErgebnis, FuellungTransparenzverlaufRandErgebnis, FuellungTransparenzverlaufArtWskErgebnisTitel, FuellungTransparenzverlaufZentrumXWskErgebnisTitel, FuellungTransparenzverlaufZentrumYWskErgebnisTitel, FuellungTransparenzverlaufAnfangWskErgebnisTitel, FuellungTransparenzverlaufEndWskErgebnisTitel, FuellungTransparenzverlaufWinkelWskErgebnisTitel, FuellungTransparenzverlaufRandWskErgebnisTitel, FuellungTransparenzverlaufArtWskErgebnis, FuellungTransparenzverlaufZentrumXWskErgebnis, FuellungTransparenzverlaufZentrumYWskErgebnis, FuellungTransparenzverlaufAnfangWskErgebnis, FuellungTransparenzverlaufEndWskErgebnis, FuellungTransparenzverlaufWinkelWskErgebnis, FuellungTransparenzverlaufRandWskErgebnis, SeitenBreite, SeitenHoehe, SeitenRandOben, SeitenRandUnten, SeitenRandLinks, SeitenRandRechts);
    fclose(fp);
  }


  /* Falls Formeln vorhanden sind, werden die Objektdateien erstellt! */

  if(BruchErgebnisFormel && wsk && odt)
  {
    for(i=1;i<=AnzahlKnotenLetzteStufe;i++)
    {
      char formelverzeichnis[4097];
      sprintf(formelverzeichnis,"%s",verzeichnisname);
      strcat(formelverzeichnis,FORMELVERZEICHNIS);
      char istring[21] = "";
      sprintf(istring,"%i",i);
      strcat(formelverzeichnis,istring);
      mkdir(formelverzeichnis MODUS;
/*      char kommando[110] = "mkdir -p ";
      strcat(kommando,formelverzeichnis);
      system(kommando);*/

      char formelcontent[113] = "";
      strcat(formelcontent,formelverzeichnis);
      strcat(formelcontent,"/content.xml");

      fp = fopen(formelcontent,"a");
      fprintf(fp,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n<math xmlns=\"http://www.w3.org/1998/Math/MathML\">\n\n <semantics>\n  <mstyle color=\"%s\">\n   <mstyle mathvariant=\"%s %s\">\n    <mfrac>\n     <mn>%s</mn>\n     <mn>%s</mn>\n    </mfrac>\n  </mstyle>\n  </mstyle>\n  <annotation encoding=\"StarMath 5.0\">color %s %s %s {%s over %s}</annotation>\n </semantics>\n</math>",SchriftfarbeWskErgebnis,FettWskErgebnis,KursivWskErgebnis,ZaehlerErgebnis[i],NennerErgebnis[i],SchriftfarbeWskErgebnis,FettWskErgebnis,KursivWskErgebnis,ZaehlerErgebnis[i],NennerErgebnis[i]);
      fclose(fp);

      char formelsettings[114] = "";
      strcat(formelsettings,formelverzeichnis);
      strcat(formelsettings,"/settings.xml");

      fp = fopen(formelsettings,"a");
      fprintf(fp,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n<office:document-settings xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns:config=\"urn:oasis:names:tc:opendocument:xmlns:config:1.0\" xmlns:ooo=\"http://openoffice.org/2004/office\" office:version=\"1.2\">\n <office:settings>\n  <config:config-item-set config:name=\"ooo:configuration-settings\">\n   <config:config-item config:name=\"BaseFontHeight\" config:type=\"short\">%i</config:config-item>\n   <config:config-item config:name=\"FontNameNumbers\" config:type=\"string\">%s</config:config-item>  </config:config-item-set>\n </office:settings>\n</office:document-settings>",SchriftgroesseWskErgebnis,SchriftartWskErgebnis);
      fclose(fp);
    }
  }

  if(BruchZweigeFormel && wsk && odt)
  {
    n=1;
    for(i=1;i<ArgumentAnzahl;i++)
    {
      for(j=1;j<=AnzahlKnotenStufe[i];j++)
      {
        char formelverzeichnis[4097];
        sprintf(formelverzeichnis,"%s",verzeichnisname);
        strcat(formelverzeichnis,FORMELVERZEICHNIS);
        strcat(formelverzeichnis,"Zweig");
        char istring[21] = "";
        sprintf(istring,"%i",n);
        strcat(formelverzeichnis,istring);
        mkdir(formelverzeichnis MODUS;
/*      char kommando[110] = "mkdir -p ";
        strcat(kommando,formelverzeichnis);
        system(kommando);*/

        char formelcontent[4097] = "";
        strcat(formelcontent,formelverzeichnis);
        strcat(formelcontent,"/content.xml");

        fp = fopen(formelcontent,"a");
        fprintf(fp,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n<math xmlns=\"http://www.w3.org/1998/Math/MathML\">\n\n <semantics>\n  <mstyle color=\"%s\">\n   <mstyle mathvariant=\"%s %s\">\n    <mfrac>\n     <mn>%s</mn>\n     <mn>%s</mn>\n    </mfrac>\n  </mstyle>\n  </mstyle>\n  <annotation encoding=\"StarMath 5.0\">color %s %s %s {%s over %s}</annotation>\n </semantics>\n</math>",SchriftfarbeWsk,FettWsk,KursivWsk,ZaehlerZweig[n],NennerZweig[n],SchriftfarbeWsk,FettWsk,KursivWsk,ZaehlerZweig[n],NennerZweig[n]);
        fclose(fp);

        char formelsettings[4097] = "";
        strcat(formelsettings,formelverzeichnis);
        strcat(formelsettings,"/settings.xml");

        fp = fopen(formelsettings,"a");
        fprintf(fp,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n<office:document-settings xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns:config=\"urn:oasis:names:tc:opendocument:xmlns:config:1.0\" xmlns:ooo=\"http://openoffice.org/2004/office\" office:version=\"1.2\">\n <office:settings>\n  <config:config-item-set config:name=\"ooo:configuration-settings\">\n   <config:config-item config:name=\"BaseFontHeight\" config:type=\"short\">%i</config:config-item>\n   <config:config-item config:name=\"FontNameNumbers\" config:type=\"string\">%s</config:config-item>  </config:config-item-set>\n </office:settings>\n</office:document-settings>",SchriftgroesseWsk,SchriftartWsk);
        fclose(fp);
        n++;
      }
    }
  }

  if(wsk)
  {
    for(j=0; j < MaximaleAnzahlKnotenProStufe*ArgumentAnzahl+1; j++)
    {
      free(ZaehlerZweig[j]);
    }
    free(ZaehlerZweig);
  }

  if(wsk)
  {
    for(j=0; j < MaximaleAnzahlKnotenProStufe*ArgumentAnzahl+1; j++)
    {
      free(NennerZweig[j]);
    }
    free(NennerZweig);
  }

  if(wsk)
  {
    for(j=0; j < MaximaleAnzahlKnotenProStufe+1; j++)
    {
      free(ZaehlerErgebnis[j]);
    }
    free(ZaehlerErgebnis);

    for(j=0; j < MaximaleAnzahlKnotenProStufe+1; j++)
    {
      free(NennerErgebnis[j]);
    }
    free(NennerErgebnis);
  }

  /* Jetzt werden alle erstellten Dateien gezippt, das temporäre Verzeichnis gelöscht und die ODT-Datei geöffnet! */

  if(odt)
  {
    char zipdateiname[4097] = "";
    if(strlen(Dateipfad) == 1)
    {
      sprintf(zipdateiname,"%s",homeverzeichnis);
    }
    else
    {
      sprintf(zipdateiname,"%s",Dateipfad);
    }
    strcat(zipdateiname,ZIPDATEINAME);
    fp = fopen(CONTENT,"r");
    fseek(fp,0L,SEEK_END);
    dateigroesse = ftell(fp);
    //printf("Dateigroesse = %ld\n",dateigroesse);
    rewind(fp);
    char *dateiinhalt=calloc(dateigroesse+2,sizeof(char));
    temp=0;
    j = 0;
    while((temp = fgetc(fp))!=EOF)
    {
      //printf("%c\n",temp);
      dateiinhalt[j]=temp;
      j++;
    }
    fclose(fp);
    dateiinhalt[j] = 10;
//  //printf("%s\n",dateiinhalt);
    mz_zip_add_mem_to_archive_file_in_place(zipdateiname, "content.xml", dateiinhalt, strlen(dateiinhalt), "Das ist ein Kommentar", (unsigned long)strlen("Das ist ein Kommentar"), MZ_BEST_COMPRESSION);
    free(dateiinhalt);

    fp = fopen(MANIFEST,"r");
    fseek(fp,0L,SEEK_END);
    dateigroesse = ftell(fp);
//  //printf("Dateigroesse = %ld\n",dateigroesse);
    rewind(fp);
    dateiinhalt=calloc(dateigroesse+2,sizeof(char));
    temp=0;
    j = 0;
    while((temp = fgetc(fp))!=EOF)
    {
      dateiinhalt[j]=temp;
      j++;
    }
    fclose(fp);
    dateiinhalt[j] = 10;
//  //printf("%s\n",dateiinhalt);
    mz_zip_add_mem_to_archive_file_in_place(zipdateiname, "META-INF/manifest.xml", dateiinhalt, strlen(dateiinhalt), "Das ist ein Kommentar", (unsigned long)strlen("Das ist ein Kommentar"), MZ_BEST_COMPRESSION);
    free(dateiinhalt);

    fp = fopen(STYLES,"r");
    fseek(fp,0L,SEEK_END);
    dateigroesse = ftell(fp);
    rewind(fp);
    dateiinhalt=calloc(dateigroesse+2,sizeof(char));
    temp=0;
    j = 0;
    while((temp = fgetc(fp))!=EOF)
    {
      dateiinhalt[j]=temp;
      j++;
    }
    fclose(fp);
    dateiinhalt[j] = 10;
//  //printf("%s\n",dateiinhalt);
    mz_zip_add_mem_to_archive_file_in_place(zipdateiname, "styles.xml", dateiinhalt, strlen(dateiinhalt), "Das ist ein Kommentar", (unsigned long)strlen("Das ist ein Kommentar"), MZ_BEST_COMPRESSION);
    free(dateiinhalt);

    if(BruchErgebnisFormel && wsk)
    {
      for(i=1;i<=AnzahlKnotenLetzteStufe;i++)
      {
        char FORMEL[50] = "Formel";
        char nummer[20] = "";
        sprintf(nummer,"%i",i);
        strcat(FORMEL, nummer);
        strcat(FORMEL,"/content.xml");
        char FORMELZIP[100];
        sprintf(FORMELZIP,"%s/Formel%i/content.xml",verzeichnisname,i);
        fp = fopen(FORMELZIP,"r");
        fseek(fp,0L,SEEK_END);
        dateigroesse = ftell(fp);
        rewind(fp);
        dateiinhalt=calloc(dateigroesse+2,sizeof(char));
        temp=0;
        j = 0;
        while((temp = fgetc(fp))!=EOF)
        {
          dateiinhalt[j]=temp;
          j++;
        }
        fclose(fp);
        dateiinhalt[j] = 0;
//      //printf("%s\n",dateiinhalt);
        mz_zip_add_mem_to_archive_file_in_place(zipdateiname, FORMEL, dateiinhalt, strlen(dateiinhalt), "Das ist ein Kommentar", (unsigned long)strlen("Das ist ein Kommentar"), MZ_BEST_COMPRESSION);
        free(dateiinhalt);
      }
    }

    if(BruchErgebnisFormel && wsk)
    {
      for(i=1;i<=AnzahlKnotenLetzteStufe;i++)
      {
        char FORMEL[50] = "Formel";
        char nummer[20] = "";
        sprintf(nummer,"%i",i);
        strcat(FORMEL, nummer);
        strcat(FORMEL,"/settings.xml");
        char FORMELZIP[100];
        sprintf(FORMELZIP,"%s/Formel%i/settings.xml",verzeichnisname,i);
        fp = fopen(FORMELZIP,"r");
        fseek(fp,0L,SEEK_END);
        dateigroesse = ftell(fp);
        rewind(fp);
        dateiinhalt=calloc(dateigroesse+2,sizeof(char));
        temp=0;
        j = 0;
        while((temp = fgetc(fp))!=EOF)
        {
          dateiinhalt[j]=temp;
          j++;
        }
        fclose(fp);
        dateiinhalt[j] = 0;
//      //printf("%s\n",dateiinhalt);
        mz_zip_add_mem_to_archive_file_in_place(zipdateiname, FORMEL, dateiinhalt, strlen(dateiinhalt), "Das ist ein Kommentar", (unsigned long)strlen("Das ist ein Kommentar"), MZ_BEST_COMPRESSION);
        free(dateiinhalt);
      }
    }

    if(BruchZweigeFormel && wsk)
    {
      n=1;
      for(i=1;i<ArgumentAnzahl;i++)
      {
        for(j=1;j<=AnzahlKnotenStufe[i];j++)
        {
          char FORMEL[55] = "FormelZweig";
          char nummer[20] = "";
          sprintf(nummer,"%i",n);
          strcat(FORMEL, nummer);
          strcat(FORMEL,"/content.xml");
          char FORMELZIP[100];
          sprintf(FORMELZIP,"%s/FormelZweig%i/content.xml",verzeichnisname,n);
          fp = fopen(FORMELZIP,"r");
          fseek(fp,0L,SEEK_END);
          dateigroesse = ftell(fp);
          rewind(fp);
          dateiinhalt=calloc(dateigroesse+2,sizeof(char));
          temp=0;
          k = 0;
          while((temp = fgetc(fp))!=EOF)
          {
            dateiinhalt[k]=temp;
            k++;
          }
          fclose(fp);
          dateiinhalt[k] = 0;
//        //printf("%s\n",dateiinhalt);
          mz_zip_add_mem_to_archive_file_in_place(zipdateiname, FORMEL, dateiinhalt, strlen(dateiinhalt), "Das ist ein Kommentar", (unsigned long)strlen("Das ist ein Kommentar"), MZ_BEST_COMPRESSION);
          free(dateiinhalt);
          n++;
        }
      }
    }

    if(BruchZweigeFormel && wsk)
    {
      n=1;
      for(i=1;i<ArgumentAnzahl;i++)
      {
        for(j=1;j<=AnzahlKnotenStufe[i];j++)
        {
          char FORMEL[55] = "FormelZweig";
          char nummer[20] = "";
          sprintf(nummer,"%i",n);
          strcat(FORMEL, nummer);
          strcat(FORMEL,"/settings.xml");
          char FORMELZIP[100];
          sprintf(FORMELZIP,"%s/FormelZweig%i/settings.xml",verzeichnisname,n);
          fp = fopen(FORMELZIP,"r");
          fseek(fp,0L,SEEK_END);
          dateigroesse = ftell(fp);
          rewind(fp);
          dateiinhalt=calloc(dateigroesse+2,sizeof(char));
          temp=0;
          k = 0;
          while((temp = fgetc(fp))!=EOF)
          {
            dateiinhalt[k]=temp;
            k++;
          }
          fclose(fp);
          dateiinhalt[k] = 0;
//        //printf("%s\n",dateiinhalt);
          mz_zip_add_mem_to_archive_file_in_place(zipdateiname, FORMEL, dateiinhalt, strlen(dateiinhalt), "Das ist ein Kommentar", (unsigned long)strlen("Das ist ein Kommentar"), MZ_BEST_COMPRESSION);
          free(dateiinhalt);
          n++;
        }
      }
    }



    char odtdateiname[4097] = "", hilfsname[4097] = "";
    sprintf(odtdateiname,"%s",zipdateiname);
    odtdateiname[strlen(odtdateiname)-4] = 0;
    strcpy(hilfsname, odtdateiname);
    //printf("%s\n",odtdateiname);
    sprintf(odtdateiname,"%s.odt",odtdateiname);
    i=1;
    fp = fopen(odtdateiname,"r");
    while(fp != NULL)
    {
      fclose(fp);
      sprintf(odtdateiname,"%s%i.odt",hilfsname,i);
      i++;
      fp = fopen(odtdateiname,"r");
    }
    rename(zipdateiname,odtdateiname);

    remove(CONTENT);
    remove(MANIFEST);
    remove(STYLES);
    if(BruchErgebnisFormel && wsk)
    {
      for(i=1;i<=AnzahlKnotenLetzteStufe;i++)
      {
        char formelverzeichnis[100];
        sprintf(formelverzeichnis,"%s/Formel%i",verzeichnisname,i);

        char formelcontent[113] = "";
        strcat(formelcontent,formelverzeichnis);
        strcat(formelcontent,"/content.xml");

        char formelsettings[114] = "";
        strcat(formelsettings,formelverzeichnis);
        strcat(formelsettings,"/settings.xml");

        remove(formelcontent);
        remove(formelsettings);
        rmdir(formelverzeichnis);
      }
    }

    if(BruchZweigeFormel && wsk)
    {
      n=1;
      for(i=1;i<ArgumentAnzahl;i++)
      {
        for(j=1;j<=AnzahlKnotenStufe[i];j++)
        {
          char formelverzeichnis[100];
          sprintf(formelverzeichnis,"%s/FormelZweig%i",verzeichnisname,n);

          char formelcontent[113] = "";
          strcat(formelcontent,formelverzeichnis);
          strcat(formelcontent,"/content.xml");

          char formelsettings[114] = "";
          strcat(formelsettings,formelverzeichnis);
          strcat(formelsettings,"/settings.xml");

          remove(formelcontent);
          remove(formelsettings);
          rmdir(formelverzeichnis);
          n++;
        }
      }
    }


    rmdir(META);
    rmdir(verzeichnisname);

    OEFFNEN

    system(aufruf2);
  }

  free(AnzahlKnotenStufe);

  return 0;
}




































/* Jetzt kommen die Funktionen zu den Prototypen! */

int zeichenzaehlen(char *string, int zeichen)
{
  int anzahl = 0;
  char *adresse = strchr(string,zeichen);
  while(adresse != NULL)
  {
    anzahl++;
    //printf("adresse: %s\n",adresse);
    if(adresse+2<string+strlen(string))
    {
      adresse = strchr(adresse+2,zeichen);
    }
    else
    {
      break;
    }
  }
  return anzahl;
}

char *knotenfinder(char *string, int nummer)
{
  char *zeiger = strtok(string,"+,");
  int i;
  for(i=0; i<nummer; i++)
  {
    zeiger = strtok(NULL,"+,");
  }
  return zeiger;
}

unsigned long int potenziere(unsigned long int basis, unsigned long int exponent)
{
  unsigned long int i, potenz = 1;
  for(i=0; i<exponent; i++)
  {
    potenz *= basis;
  }
  return potenz;
}

unsigned long int ggT(unsigned long int zahl1, unsigned long int zahl2)
{
  unsigned long int max = (zahl1 < zahl2)?zahl2:zahl1, min = (zahl1 < zahl2)?zahl1:zahl2, x, r;
  x = min;
  if(min == 0)
  {
    return max;
  }
  r = max % min;
  while(r != 0)
  {
    unsigned long int xh = r;
    r = x % r;
    x = xh;
  }
  return x;
}

char *bruchmultiplikation(char *bruch1, char *bruch2, char *ergebnis)
{
  char *zaehler1, *zaehler2, *nenner1, *nenner2;
  zaehler1 = strtok(bruch1,"/");
  nenner1 = strtok(NULL,"/");
  zaehler2 = strtok(bruch2,"/");
  nenner2 = strtok(NULL,"/");
  unsigned long int z1 = ((zaehler1)?atol(zaehler1):0), z2 = ((zaehler2)?atol(zaehler2):0),
                    n1 = (nenner1 == NULL) ? 1 : atol(nenner1), n2 = (nenner2 == NULL) ? 1 : atol(nenner2), g11, g22,
                    g12, g21;
  g11 = ggT(z1,n1);
  z1 /= g11;
  n1 /= g11;
  g22 = ggT(z2,n2);
  z2 /= g22;
  n2 /= g22;
  g12 = ggT(z1,n2);
  z1 /= g12;
  n2 /= g12;
  g21 = ggT(z2,n1);
  n1 /= g21;
  z2 /= g21;
  unsigned long int Z = z1 * z2, N = n1 * n2;
  char zs[21], ns[21];
  sprintf(zs,"%lu",Z);
  if(N == 1)
  {
    strcpy(ergebnis, zs);
    return ergebnis;
  }
  sprintf(ns,"%lu",N);
  strcpy(ergebnis, zs);
  strcat(ergebnis, "/");
  strcat(ergebnis, ns);
  return ergebnis;
}

double bruchzufloat(char *bruch)
{
  char *teiler;
  teiler = strtok(bruch,"/");
  double Dividend = (teiler)?atof(teiler):0;
  teiler = strtok(NULL,"/");
  if(teiler == NULL)
  {
    return Dividend;
  }
  double Divisor = atof(teiler);
//  double Quotient = Dividend / Divisor;
  return Dividend / Divisor;
}

char *floatstringzubruch(char *zahlstring, char *bruch)
{
  char *komma = strchr(zahlstring,46);
  if(komma == NULL)
  {
    bruch[0] = zahlstring[0];
    bruch[1] = 0;
    return bruch;
  }
  unsigned long int zaehler = atol(komma + 1);
  unsigned long int nenner = 1;
  int i;
  for(i=0; i<strlen(komma + 1); i++)
  {
    nenner *= 10;
  }
  unsigned long int g = ggT(zaehler,nenner);
  zaehler /= g;
  nenner /= g;
  sprintf(bruch,"%lu/%lu",zaehler,nenner);
  return bruch;
}

char *floatzubruch(float zahl, char *bruch)
{
  char zahlstring[11];
  sprintf(zahlstring,"%g",zahl);
  char *komma = strchr(zahlstring,46);
  if(komma == NULL)
  {
    bruch[0] = zahlstring[0];
    bruch[1] = 0;
    return bruch;
  }
  unsigned int zaehler = atol(komma + 1);
  unsigned int nenner = 1;
  int i;
  for(i=0; i<strlen(komma + 1); i++)
  {
    nenner *= 10;
  }
  unsigned long int g = ggT(zaehler,nenner);
  zaehler /= g;
  nenner /= g;
  sprintf(bruch,"%d/%d",zaehler,nenner);
  return bruch;
}

double wurzel(double zahl)
{
/*  int i;
  double links = (zahl>1)?0:zahl, rechts = (zahl>1)?zahl:1, qwurzel = (links+rechts)/2;
  for(i=1;i<100;i++)
  {
    if(qwurzel*qwurzel == zahl)
    {
      //printf("Iterationsschritt: %d\n",i);
      break;
    }
    if(qwurzel*qwurzel < zahl)
    {
      //printf("Iterationsschritt: %d\nlinks: %g\nrechts: %g\n",i,links,rechts);
      links = qwurzel;
      qwurzel = (qwurzel + rechts) / 2;
    }
    else
    {
      //printf("Iterationsschritt: %d\nlinks: %g\nrechts: %g\n",i,links,rechts);
      rechts = qwurzel;
      qwurzel = (links + qwurzel) / 2;
    }
  }*/
  return sqrt(zahl);
}

double arctan(double zahl)
{
  return atan(zahl);
}

double schriftbreite(char *Schriftart)
{
  if (!strcmp(Schriftart,"Zurklez Solid BRK")){return 0.710000000000000;}
  else if (!strcmp(Schriftart,"Zurklez Outline BRK")){return 0.711000000000000;}
  else if (!strcmp(Schriftart,"Zrnic")){return 0.766000000000000;}
  else if (!strcmp(Schriftart,"Zoidal BRK")){return 1.309000000000000;}
  else if (!strcmp(Schriftart,"Zoetrope BRK")){return 0.490000000000000;}
  else if (!strcmp(Schriftart,"Zodillinstrisstirust")){return 0.619000000000000;}
  else if (!strcmp(Schriftart,"Zirconia Cubic BRK")){return 0.500000000000000;}
  else if (!strcmp(Schriftart,"Zirconia BRK")){return 0.629000000000000;}
  else if (!strcmp(Schriftart,"Zero Velocity BRK")){return 1.265000000000000;}
  else if (!strcmp(Schriftart,"Zero Twos")){return 1.006000000000000;}
  else if (!strcmp(Schriftart,"Zero Threes")){return 1.006000000000000;}
  else if (!strcmp(Schriftart,"ZeroHour")){return 1.347000000000000;}
  else if (!strcmp(Schriftart,"Zephyrean Gust BRK")){return 0.606000000000000;}
  else if (!strcmp(Schriftart,"Zephyrean BRK")){return 0.606000000000000;}
  else if (!strcmp(Schriftart,"Zenith BRK")){return 0.814000000000000;}
  else if (!strcmp(Schriftart,"Zelda DX TT BRK")){return 0.870000000000000;}
  else if (!strcmp(Schriftart,"Zekton Dots")){return 1.123000000000000;}
  else if (!strcmp(Schriftart,"Zekton")){return 1.173000000000000;}
  else if (!strcmp(Schriftart,"Yytrium Dioxide")){return 1.361000000000000;}
  else if (!strcmp(Schriftart,"You're Gone")){return 0.784000000000000;}
  else if (!strcmp(Schriftart,"Your Complex O BRK")){return 0.590000000000000;}
  else if (!strcmp(Schriftart,"Your Complex I BRK")){return 0.598000000000000;}
  else if (!strcmp(Schriftart,"Your Complex BRK")){return 0.587000000000000;}
  else if (!strcmp(Schriftart,"Yoshi's Story game text BRK")){return 0.731000000000000;}
  else if (!strcmp(Schriftart,"YonderRecoil")){return 1.371000000000000;}
  else if (!strcmp(Schriftart,"Yonder BRK")){return 0.491000000000000;}
  else if (!strcmp(Schriftart,"YoFrankie.org")){return 1.064000000000000;}
  else if (!strcmp(Schriftart,"Yielding BRK")){return 0.676000000000000;}
  else if (!strcmp(Schriftart,"Yesterday BRK")){return 0.927000000000000;}
  else if (!strcmp(Schriftart,"Yellow Pills")){return 0.584000000000000;}
  else if (!strcmp(Schriftart,"Yearend BRK")){return 0.769000000000000;}
  else if (!strcmp(Schriftart,"YBandTuner")){return 1.128000000000000;}
  else if (!strcmp(Schriftart,"Yawnovision")){return 1.033000000000000;}
  else if (!strcmp(Schriftart,"Yanone Kaffeesatz Thin")){return 0.681000000000000;}
  else if (!strcmp(Schriftart,"Yanone Kaffeesatz Regular")){return 0.660000000000000;}
  else if (!strcmp(Schriftart,"Yanone Kaffeesatz Light")){return 0.649000000000000;}
  else if (!strcmp(Schriftart,"Yanone Kaffeesatz Bold")){return 0.672000000000000;}
  else if (!strcmp(Schriftart,"Yadou")){return 1.985000000000000;}
  else if (!strcmp(Schriftart,"Xtrusion BRK")){return 1.117000000000000;}
  else if (!strcmp(Schriftart,"Xtraflexidisc")){return 0.460000000000000;}
  else if (!strcmp(Schriftart,"Xolto")){return 0.724000000000000;}
  else if (!strcmp(Schriftart,"Xmas Lights BRK")){return 0.418000000000000;}
  else if (!strcmp(Schriftart,"Xipital BRK")){return 0.962000000000000;}
  else if (!strcmp(Schriftart,"Xhume BRK")){return 0.926000000000000;}
  else if (!strcmp(Schriftart,"Xerox Malfunction BRK")){return 0.807000000000000;}
  else if (!strcmp(Schriftart,"Xenowort")){return 0.989000000000000;}
  else if (!strcmp(Schriftart,"Wyvern Wings Wyde BRK")){return 0.810000000000000;}
  else if (!strcmp(Schriftart,"Wyvern Wings BRK")){return 0.405000000000000;}
  else if (!strcmp(Schriftart,"Worthless Bum")){return 0.507000000000000;}
  else if (!strcmp(Schriftart,"World of Water")){return 0.872000000000000;}
  else if (!strcmp(Schriftart,"Wobbly BRK")){return 0.851000000000000;}
  else if (!strcmp(Schriftart,"Withstand BRK")){return 0.552000000000000;}
  else if (!strcmp(Schriftart,"Wintermute")){return 0.853000000000000;}
  else if (!strcmp(Schriftart,"Winks")){return 0.741210937500000;}
  else if (!strcmp(Schriftart,"Wincing BRK")){return 0.828000000000000;}
  else if (!strcmp(Schriftart,"Wild Sewerage")){return 0.780000000000000;}
  else if (!strcmp(Schriftart,"Wiggly Squiggly BRK")){return 0.542000000000000;}
  else if (!strcmp(Schriftart,"WhiteLake")){return 0.822000000000000;}
  else if (!strcmp(Schriftart,"Whippersnapper BRK")){return 0.449000000000000;}
  else if (!strcmp(Schriftart,"Whatever BRK")){return 0.545000000000000;}
  else if (!strcmp(Schriftart,"Wet Pet")){return 0.944335937500000;}
  else if (!strcmp(Schriftart,"Welfare Brat")){return 0.458000000000000;}
  else if (!strcmp(Schriftart,"Webster World")){return 1.018000000000000;}
  else if (!strcmp(Schriftart,"Weaver BRK")){return 0.626000000000000;}
  else if (!strcmp(Schriftart,"Weathered Solid BRK")){return 0.991000000000000;}
  else if (!strcmp(Schriftart,"Weathered BRK")){return 1.006000000000000;}
  else if (!strcmp(Schriftart,"Wayward Shadow BRK")){return 0.351000000000000;}
  else if (!strcmp(Schriftart,"Wayward BRK")){return 0.346000000000000;}
  else if (!strcmp(Schriftart,"waver BRK")){return 1.003000000000000;}
  else if (!strcmp(Schriftart,"Wargames")){return 0.872070312500000;}
  else if (!strcmp(Schriftart,"Waree")){return 1.274414062500000;}
  else if (!strcmp(Schriftart,"Walshes Outline")){return 0.529000000000000;}
  else if (!strcmp(Schriftart,"Walshes")){return 0.511000000000000;}
  else if (!strcmp(Schriftart,"Wake &amp; Bake")){return 0.814000000000000;}
  else if (!strcmp(Schriftart,"Wager Won BRK")){return 0.575000000000000;}
  else if (!strcmp(Schriftart,"Wager Lost BRK")){return 0.591000000000000;}
  else if (!strcmp(Schriftart,"Wager BRK")){return 0.575000000000000;}
  else if (!strcmp(Schriftart,"Vrinda")){return 0.631347656250000;}
  else if (!strcmp(Schriftart,"Vollkorn Regular")){return 1.028000000000000;}
  else if (!strcmp(Schriftart,"Vollkorn Italic")){return 1.063000000000000;}
  else if (!strcmp(Schriftart,"Vollkorn Bold Italic")){return 1.068000000000000;}
  else if (!strcmp(Schriftart,"Vollkorn Bold")){return 1.059000000000000;}
  else if (!strcmp(Schriftart,"Volatile 2 BRK")){return 0.922000000000000;}
  else if (!strcmp(Schriftart,"Volatile 1 BRK")){return 0.950000000000000;}
  else if (!strcmp(Schriftart,"VL P&#12468;&#12471;&#12483;&#12463;")){return 0.931000000000000;}
  else if (!strcmp(Schriftart,"VL &#12468;&#12471;&#12483;&#12463;")){return 0.610000000000000;}
  else if (!strcmp(Schriftart,"Visitor TT2 BRK")){return 0.467000000000000;}
  else if (!strcmp(Schriftart,"Visitor TT1 BRK")){return 0.600000000000000;}
  else if (!strcmp(Schriftart,"Vipnagorgialla")){return 1.059000000000000;}
  else if (!strcmp(Schriftart,"Vindictive BRK")){return 0.720000000000000;}
  else if (!strcmp(Schriftart,"Vigilance BRK")){return 0.639000000000000;}
  else if (!strcmp(Schriftart,"Vibrocentric")){return 0.942000000000000;}
  else if (!strcmp(Schriftart,"Vertigo Upright BRK")){return 0.932000000000000;}
  else if (!strcmp(Schriftart,"Vertigo Upright 2 BRK")){return 0.932000000000000;}
  else if (!strcmp(Schriftart,"Vertigo BRK")){return 1.090000000000000;}
  else if (!strcmp(Schriftart,"Vertigo 2 BRK")){return 1.090000000000000;}
  else if (!strcmp(Schriftart,"Verdana")){return 1.265136718750000;}
  else if (!strcmp(Schriftart,"Venus Rising")){return 1.530000000000000;}
  else if (!strcmp(Schriftart,"Vemana2000")){return 0.758400000000000;}
  else if (!strcmp(Schriftart,"Velvenda Megablack")){return 0.682000000000000;}
  else if (!strcmp(Schriftart,"Vectroid")){return 0.720000000000000;}
  else if (!strcmp(Schriftart,"VDub")){return 1.527000000000000;}
  else if (!strcmp(Schriftart,"Variance BRK")){return 1.104000000000000;}
  else if (!strcmp(Schriftart,"Vantage BRK")){return 0.490000000000000;}
  else if (!strcmp(Schriftart,"Vanishing Girl")){return 0.338000000000000;}
  else if (!strcmp(Schriftart,"Vanished BRK")){return 0.848000000000000;}
  else if (!strcmp(Schriftart,"Vanilla Whale")){return 0.604000000000000;}
  else if (!strcmp(Schriftart,"Vahika")){return 0.875000000000000;}
  else if (!strcmp(Schriftart,"Vademecum")){return 0.795000000000000;}
  else if (!strcmp(Schriftart,"Vacant Capz BRK")){return 0.843000000000000;}
  else if (!strcmp(Schriftart,"Utah Condensed")){return 0.822753906250000;}
  else if (!strcmp(Schriftart,"Utah")){return 1.158203125000000;}
  else if (!strcmp(Schriftart,"UrUrMa")){return 0.460449218750000;}
  else if (!strcmp(Schriftart,"Urkelian Television Dynasty")){return 0.432000000000000;}
  else if (!strcmp(Schriftart,"Upraise BRK")){return 0.812000000000000;}
  else if (!strcmp(Schriftart,"Upheaval TT BRK")){return 0.700000000000000;}
  else if (!strcmp(Schriftart,"Unsteady Oversteer")){return 0.750000000000000;}
  else if (!strcmp(Schriftart,"Unresponsive BRK")){return 1.093000000000000;}
  else if (!strcmp(Schriftart,"Unlearned BRK")){return 0.768000000000000;}
  else if (!strcmp(Schriftart,"Unlearned 2 BRK")){return 0.774000000000000;}
  else if (!strcmp(Schriftart,"Univox")){return 3.380000000000000;}
  else if (!strcmp(Schriftart,"UNITED BRK")){return 0.967000000000000;}
  else if (!strcmp(Schriftart,"Unispace")){return 0.663000000000000;}
  else if (!strcmp(Schriftart,"Union City Blue")){return 0.770000000000000;}
  else if (!strcmp(Schriftart,"Unikurd Web")){return 0.956054687500000;}
  else if (!strcmp(Schriftart,"Unidings")){return 0.901367187500000;}
  else if (!strcmp(Schriftart,"Unexplored Galaxies WO BRK")){return 1.492000000000000;}
  else if (!strcmp(Schriftart,"Unexplored Galaxies W BRK")){return 1.492000000000000;}
  else if (!strcmp(Schriftart,"Unexplored Galaxies O BRK")){return 0.746000000000000;}
  else if (!strcmp(Schriftart,"Unexplored Galaxies BRK")){return 0.746000000000000;}
  else if (!strcmp(Schriftart,"Underwhelmed Outline BRK")){return 0.906000000000000;}
  else if (!strcmp(Schriftart,"Underwhelmed BRK")){return 0.891000000000000;}
  else if (!strcmp(Schriftart,"Underscore BRK")){return 0.635000000000000;}
  else if (!strcmp(Schriftart,"Underscore 2 BRK")){return 0.635000000000000;}
  else if (!strcmp(Schriftart,"Unanimous Inverted BRK")){return 0.378000000000000;}
  else if (!strcmp(Schriftart,"Unanimous BRK")){return 0.416000000000000;}
  else if (!strcmp(Schriftart,"Umpush")){return 1.081054687500000;}
  else if (!strcmp(Schriftart,"Ume P Gothic")){return 0.903320312500000;}
  else if (!strcmp(Schriftart,"Ume Gothic")){return 0.610107421875000;}
  else if (!strcmp(Schriftart,"UKIJ Zilwa")){return 0.923828125000000;}
  else if (!strcmp(Schriftart,"UKIJ Tuz Tor")){return 1.014160156250000;}
  else if (!strcmp(Schriftart,"UKIJ Tuz Tom")){return 0.955078125000000;}
  else if (!strcmp(Schriftart,"UKIJ Tuz Qara")){return 1.014160156250000;}
  else if (!strcmp(Schriftart,"UKIJ Tuz Neqish")){return 1.007812500000000;}
  else if (!strcmp(Schriftart,"UKIJ Tuz Kitab")){return 0.997558593750000;}
  else if (!strcmp(Schriftart,"UKIJ Tuz Gezit")){return 0.997558593750000;}
  else if (!strcmp(Schriftart,"UKIJ Tuz Basma")){return 0.997558593750000;}
  else if (!strcmp(Schriftart,"UKIJ Tuz")){return 1.014160156250000;}
  else if (!strcmp(Schriftart,"UKIJ Tughra")){return 0.398925781250000;}
  else if (!strcmp(Schriftart,"UKIJ Tor")){return 0.944824218750000;}
  else if (!strcmp(Schriftart,"UKIJ Title")){return 0.948242187500000;}
  else if (!strcmp(Schriftart,"UKIJ Tiken")){return 0.801757812500000;}
  else if (!strcmp(Schriftart,"UKIJ Teng")){return 0.718261718750000;}
  else if (!strcmp(Schriftart,"UKIJ Sulus Tom")){return 1.014160156250000;}
  else if (!strcmp(Schriftart,"UKIJ Sulus")){return 0.898925781250000;}
  else if (!strcmp(Schriftart,"UKIJ Saet")){return 1.108398437500000;}
  else if (!strcmp(Schriftart,"UKIJ Ruqi")){return 0.898925781250000;}
  else if (!strcmp(Schriftart,"UKIJ Qolyazma Yantu")){return 0.752441406250000;}
  else if (!strcmp(Schriftart,"UKIJ Qolyazma Tuz")){return 0.949707031250000;}
  else if (!strcmp(Schriftart,"UKIJ Qolyazma Tez")){return 0.716796875000000;}
  else if (!strcmp(Schriftart,"UKIJ Qolyazma")){return 1.020019531250000;}
  else if (!strcmp(Schriftart,"UKIJ Qara")){return 0.955078125000000;}
  else if (!strcmp(Schriftart,"UKIJ Orxun-Yensey")){return 1.173339843750000;}
  else if (!strcmp(Schriftart,"UKIJ Orqun Yazma")){return 0.944824218750000;}
  else if (!strcmp(Schriftart,"UKIJ Orqun Basma")){return 0.944824218750000;}
  else if (!strcmp(Schriftart,"UKIJ Nasq Zilwa")){return 0.923828125000000;}
  else if (!strcmp(Schriftart,"UKIJ Nasq")){return 0.997558593750000;}
  else if (!strcmp(Schriftart,"UKIJ Moy Qelem")){return 0.608886718750000;}
  else if (!strcmp(Schriftart,"UKIJ Merdane")){return 0.944824218750000;}
  else if (!strcmp(Schriftart,"UKIJ Mejnuntal")){return 0.577636718750000;}
  else if (!strcmp(Schriftart,"UKIJ Mejnun")){return 0.856933593750000;}
  else if (!strcmp(Schriftart,"UKIJ_Mac Basma")){return 0.997287522603978;}
  else if (!strcmp(Schriftart,"UKIJ Kufi Yolluq")){return 1.017089843750000;}
  else if (!strcmp(Schriftart,"UKIJ Kufi Yay")){return 0.955078125000000;}
  else if (!strcmp(Schriftart,"UKIJ Kufi Uz")){return 0.955078125000000;}
  else if (!strcmp(Schriftart,"UKIJ Kufi Tar")){return 1.108398437500000;}
  else if (!strcmp(Schriftart,"UKIJ Kufi Kawak")){return 0.740722656250000;}
  else if (!strcmp(Schriftart,"UKIJ Kufi Gul")){return 0.955078125000000;}
  else if (!strcmp(Schriftart,"UKIJ Kufi Chiwer")){return 1.078613281250000;}
  else if (!strcmp(Schriftart,"UKIJ Kufi 3D")){return 0.958496093750000;}
  else if (!strcmp(Schriftart,"UKIJ Kufi")){return 0.955078125000000;}
  else if (!strcmp(Schriftart,"UKIJ Kesme Tuz")){return 1.014160156250000;}
  else if (!strcmp(Schriftart,"UKIJ Kesme")){return 0.958007812500000;}
  else if (!strcmp(Schriftart,"UKIJ Kawak 3D")){return 0.801269531250000;}
  else if (!strcmp(Schriftart,"UKIJ Kawak")){return 0.973632812500000;}
  else if (!strcmp(Schriftart,"UKIJ Junun")){return 0.472167968750000;}
  else if (!strcmp(Schriftart,"UKIJ Jelliy")){return 1.270019531250000;}
  else if (!strcmp(Schriftart,"UKIJ Inchike")){return 1.042480468750000;}
  else if (!strcmp(Schriftart,"UKIJ Imaret")){return 0.513183593750000;}
  else if (!strcmp(Schriftart,"UKIJ Esliye Tom")){return 0.871093750000000;}
  else if (!strcmp(Schriftart,"UKIJ Esliye Qara")){return 0.944824218750000;}
  else if (!strcmp(Schriftart,"UKIJ Esliye Neqish")){return 0.736816406250000;}
  else if (!strcmp(Schriftart,"UKIJ Esliye Chiwer")){return 0.875976562500000;}
  else if (!strcmp(Schriftart,"UKIJ Esliye")){return 1.014160156250000;}
  else if (!strcmp(Schriftart,"UKIJ Elipbe_Chekitlik")){return 0.813476562500000;}
  else if (!strcmp(Schriftart,"UKIJ Elipbe")){return 0.799804687500000;}
  else if (!strcmp(Schriftart,"UKIJ Ekran")){return 0.873046875000000;}
  else if (!strcmp(Schriftart,"UKIJ Diwani Yantu")){return 0.750000000000000;}
  else if (!strcmp(Schriftart,"UKIJ Diwani Tom")){return 0.948242187500000;}
  else if (!strcmp(Schriftart,"UKIJ Diwani Kawak")){return 1.072753906250000;}
  else if (!strcmp(Schriftart,"UKIJ Diwani")){return 0.898925781250000;}
  else if (!strcmp(Schriftart,"UKIJ CJK")){return 0.980468750000000;}
  else if (!strcmp(Schriftart,"UKIJ Chiwer Kesme")){return 0.958007812500000;}
  else if (!strcmp(Schriftart,"UKIJ Chechek")){return 0.955078125000000;}
  else if (!strcmp(Schriftart,"UKIJ Bom")){return 0.955078125000000;}
  else if (!strcmp(Schriftart,"UKIJ Basma")){return 0.923828125000000;}
  else if (!strcmp(Schriftart,"UKIJ 3D")){return 1.527832031250000;}
  else if (!strcmp(Schriftart,"Ubuntu-Title")){return 0.675000000000000;}
  else if (!strcmp(Schriftart,"Ubuntu Mono")){return 0.679000000000000;}
  else if (!strcmp(Schriftart,"Ubuntu Medium")){return 1.096000000000000;}
  else if (!strcmp(Schriftart,"Ubuntu Light")){return 1.035000000000000;}
  else if (!strcmp(Schriftart,"Ubuntu Condensed")){return 0.796000000000000;}
  else if (!strcmp(Schriftart,"Ubuntu")){return 1.096000000000000;}
  else if (!strcmp(Schriftart,"Ubiquity BRK")){return 0.900000000000000;}
  else if (!strcmp(Schriftart,"Typodermic")){return 0.775000000000000;}
  else if (!strcmp(Schriftart,"Typesource Extol S BRK")){return 0.949000000000000;}
  else if (!strcmp(Schriftart,"Typesource Extol O BRK")){return 0.949000000000000;}
  else if (!strcmp(Schriftart,"TW-Sung")){return 0.660644531250000;}
  else if (!strcmp(Schriftart,"TW-MOE-Std-Kai")){return 0.673828125000000;}
  else if (!strcmp(Schriftart,"TW-Kai")){return 0.681640625000000;}
  else if (!strcmp(Schriftart,"Turmoil BRK")){return 0.962000000000000;}
  else if (!strcmp(Schriftart,"Turkish Participants")){return 1.100000000000000;}
  else if (!strcmp(Schriftart,"Tunga")){return 0.572265625000000;}
  else if (!strcmp(Schriftart,"Tuffy")){return 0.986328125000000;}
  else if (!strcmp(Schriftart,"TSCu_Times")){return 1.066666666666666;}
  else if (!strcmp(Schriftart,"TSCu_Paranar")){return 1.026000000000000;}
  else if (!strcmp(Schriftart,"TSCu_Comic")){return 0.972139303482587;}
  else if (!strcmp(Schriftart,"Troll Bait")){return 0.702000000000000;}
  else if (!strcmp(Schriftart,"TriacSeventyone")){return 0.410000000000000;}
  else if (!strcmp(Schriftart,"Trebuchet MS")){return 1.034179687500000;}
  else if (!strcmp(Schriftart,"TrapperJohn")){return 0.823000000000000;}
  else if (!strcmp(Schriftart,"TRAGIC BRK")){return 0.875000000000000;}
  else if (!strcmp(Schriftart,"Traditional Arabic")){return 1.005859375000000;}
  else if (!strcmp(Schriftart,"Tork")){return 1.063000000000000;}
  else if (!strcmp(Schriftart,"Top Bond")){return 1.231000000000000;}
  else if (!strcmp(Schriftart,"Tonik BRK")){return 1.325000000000000;}
  else if (!strcmp(Schriftart,"Tomson Talks")){return 0.759000000000000;}
  else if (!strcmp(Schriftart,"TommyGun")){return 0.729000000000000;}
  else if (!strcmp(Schriftart,"Tofu")){return 0.412000000000000;}
  else if (!strcmp(Schriftart,"Tobin Tax")){return 1.056000000000000;}
  else if (!strcmp(Schriftart,"Tlwg Typo")){return 0.815000000000000;}
  else if (!strcmp(Schriftart,"Tlwg Typist")){return 0.815000000000000;}
  else if (!strcmp(Schriftart,"TlwgTypewriter")){return 0.816000000000000;}
  else if (!strcmp(Schriftart,"TlwgMono")){return 0.815000000000000;}
  else if (!strcmp(Schriftart,"Titr")){return 0.657226562500000;}
  else if (!strcmp(Schriftart,"Tinsnips")){return 1.379000000000000;}
  else if (!strcmp(Schriftart,"Tinos")){return 1.014160156250000;}
  else if (!strcmp(Schriftart,"Times New Roman")){return 1.012207031250000;}
  else if (!strcmp(Schriftart,"Tibetan Machine Uni")){return 1.008789062500000;}
  else if (!strcmp(Schriftart,"Thwart BRK")){return 0.932000000000000;}
  else if (!strcmp(Schriftart,"Tholoth")){return 0.949444444444444;}
  else if (!strcmp(Schriftart,"Thiamine")){return 0.202000000000000;}
  else if (!strcmp(Schriftart,"Tetricide BRK")){return 0.710000000000000;}
  else if (!strcmp(Schriftart,"Terylene Top")){return 0.341000000000000;}
  else if (!strcmp(Schriftart,"Telephasic BRK")){return 1.176000000000000;}
  else if (!strcmp(Schriftart,"Teen Light")){return 0.964000000000000;}
  else if (!strcmp(Schriftart,"Teen")){return 1.017000000000000;}
  else if (!strcmp(Schriftart,"techno overload BRK")){return 0.463000000000000;}
  else if (!strcmp(Schriftart,"Technique OL BRK")){return 0.787000000000000;}
  else if (!strcmp(Schriftart,"Technique BRK")){return 0.859000000000000;}
  else if (!strcmp(Schriftart,"Tearful BRK")){return 0.522000000000000;}
  else if (!strcmp(Schriftart,"Tarablus")){return 0.966111111111111;}
  else if (!strcmp(Schriftart,"TAMu_Maduram")){return 1.468000000000000;}
  else if (!strcmp(Schriftart,"TAMu_Kalyani")){return 1.325195312500000;}
  else if (!strcmp(Schriftart,"TAMu_Kadambri")){return 1.467773437500000;}
  else if (!strcmp(Schriftart,"Tahoma")){return 1.042480468750000;}
  else if (!strcmp(Schriftart,"Syracuse BRK")){return 0.408000000000000;}
  else if (!strcmp(Schriftart,"Synthetic BRK")){return 0.448000000000000;}
  else if (!strcmp(Schriftart,"Syndrome BRK")){return 0.932000000000000;}
  else if (!strcmp(Schriftart,"Symmetry BRK")){return 0.704000000000000;}
  else if (!strcmp(Schriftart,"Symbol Neu")){return 0.587890625000000;}
  else if (!strcmp(Schriftart,"Symbola")){return 1.052246093750000;}
  else if (!strcmp(Schriftart,"Symbol")){return 0.617187500000000;}
  else if (!strcmp(Schriftart,"Sylfaen")){return 1.045898437500000;}
  else if (!strcmp(Schriftart,"Sybil Green")){return 0.945000000000000;}
  else if (!strcmp(Schriftart,"Switching and Effects")){return 0.478000000000000;}
  else if (!strcmp(Schriftart,"Swirled BRK")){return 1.181000000000000;}
  else if (!strcmp(Schriftart,"Swift")){return 0.791015625000000;}
  else if (!strcmp(Schriftart,"Suruma")){return 0.934000000000000;}
  else if (!strcmp(Schriftart,"Supra Genius Lines BRK")){return 0.380000000000000;}
  else if (!strcmp(Schriftart,"Supra Genius Curves BRK")){return 0.380000000000000;}
  else if (!strcmp(Schriftart,"SuperHeterodyne")){return 1.261000000000000;}
  else if (!strcmp(Schriftart,"Superglue")){return 1.701000000000000;}
  else if (!strcmp(Schriftart,"Sui Generis")){return 1.358000000000000;}
  else if (!strcmp(Schriftart,"Sudbury Basin 3D")){return 0.977000000000000;}
  else if (!strcmp(Schriftart,"Sudbury Basin")){return 0.992000000000000;}
  else if (!strcmp(Schriftart,"Subpear")){return 1.029473684210526;}
  else if (!strcmp(Schriftart,"Styrofoam Feelings")){return 1.260000000000000;}
  else if (!strcmp(Schriftart,"Stupefaction")){return 0.770000000000000;}
  else if (!strcmp(Schriftart,"Strenuous")){return 0.933000000000000;}
  else if (!strcmp(Schriftart,"Street Cred")){return 1.113000000000000;}
  else if (!strcmp(Schriftart,"Stranded BRK")){return 1.227000000000000;}
  else if (!strcmp(Schriftart,"STOMP_Zeroes")){return 1.016000000000000;}
  else if (!strcmp(Schriftart,"Stitch &amp; Bitch")){return 0.767578125000000;}
  else if (!strcmp(Schriftart,"Still Time")){return 0.787000000000000;}
  else if (!strcmp(Schriftart,"Steve")){return 1.050000000000000;}
  else if (!strcmp(Schriftart,"Stereofidelic")){return 0.420000000000000;}
  else if (!strcmp(Schriftart,"Steelfish")){return 0.517000000000000;}
  else if (!strcmp(Schriftart,"StayPuft")){return 1.043000000000000;}
  else if (!strcmp(Schriftart,"Stasmic")){return 2.233000000000000;}
  else if (!strcmp(Schriftart,"Stagnation BRK")){return 0.949000000000000;}
  else if (!strcmp(Schriftart,"Squealer Embossed")){return 0.966000000000000;}
  else if (!strcmp(Schriftart,"Squealer")){return 0.828000000000000;}
  else if (!strcmp(Schriftart,"Square Route BRK")){return 1.192000000000000;}
  else if (!strcmp(Schriftart,"Spongy")){return 0.906000000000000;}
  else if (!strcmp(Schriftart,"Splatz BRK")){return 1.221000000000000;}
  else if (!strcmp(Schriftart,"Spheroids X BRK")){return 0.580000000000000;}
  else if (!strcmp(Schriftart,"Spheroids BRK")){return 0.640000000000000;}
  else if (!strcmp(Schriftart,"Spastic BRK")){return 1.230000000000000;}
  else if (!strcmp(Schriftart,"Spacious Outline BRK")){return 1.368000000000000;}
  else if (!strcmp(Schriftart,"Spacious BRK")){return 1.398000000000000;}
  else if (!strcmp(Schriftart,"Soul Papa")){return 0.910000000000000;}
  else if (!strcmp(Schriftart,"Soul Mama")){return 0.794000000000000;}
  else if (!strcmp(Schriftart,"So Run Down")){return 0.605000000000000;}
  else if (!strcmp(Schriftart,"Sophia Nubian")){return 1.139000000000000;}
  else if (!strcmp(Schriftart,"SolaimanLipi")){return 0.965820312500000;}
  else if (!strcmp(Schriftart,"Sofachrome")){return 3.039000000000000;}
  else if (!strcmp(Schriftart,"Snidely")){return 0.913000000000000;}
  else if (!strcmp(Schriftart,"Snailets BRK")){return 0.627000000000000;}
  else if (!strcmp(Schriftart,"Sloe Gin Rickey")){return 0.898000000000000;}
  else if (!strcmp(Schriftart,"Slender Wide BRK")){return 0.423000000000000;}
  else if (!strcmp(Schriftart,"Slender Stubby BRK")){return 0.741000000000000;}
  else if (!strcmp(Schriftart,"Slender Mini BRK")){return 0.408000000000000;}
  else if (!strcmp(Schriftart,"Slender BRK")){return 0.219000000000000;}
  else if (!strcmp(Schriftart,"Skull Capz BRK")){return 0.628000000000000;}
  else if (!strcmp(Schriftart,"Skeletor Stance")){return 0.590000000000000;}
  else if (!strcmp(Schriftart,"Sindbad")){return 0.966111111111111;}
  else if (!strcmp(Schriftart,"Simplified Arabic Fixed")){return 0.753906250000000;}
  else if (!strcmp(Schriftart,"Simplified Arabic")){return 0.860351562500000;}
  else if (!strcmp(Schriftart,"Simpleton BRK")){return 0.804000000000000;}
  else if (!strcmp(Schriftart,"SIL Yi")){return 0.697265625000000;}
  else if (!strcmp(Schriftart,"Silicon Carne")){return 0.409179687500000;}
  else if (!strcmp(Schriftart,"SILDoulos IPA93")){return 0.904000000000000;}
  else if (!strcmp(Schriftart,"Sideways BRK")){return 0.664000000000000;}
  else if (!strcmp(Schriftart,"Shruti")){return 0.677734375000000;}
  else if (!strcmp(Schriftart,"Should've Known Shaded")){return 1.303000000000000;}
  else if (!strcmp(Schriftart,"Should've Known")){return 1.179000000000000;}
  else if (!strcmp(Schriftart,"Shlop")){return 0.599000000000000;}
  else if (!strcmp(Schriftart,"Shifty Chica")){return 0.804000000000000;}
  else if (!strcmp(Schriftart,"Sharjah")){return 0.966111111111111;}
  else if (!strcmp(Schriftart,"Shado")){return 0.966111111111111;}
  else if (!strcmp(Schriftart,"Sexsmith")){return 0.540000000000000;}
  else if (!strcmp(Schriftart,"Setback TT BRK")){return 0.675000000000000;}
  else if (!strcmp(Schriftart,"Sequence BRK")){return 0.737000000000000;}
  else if (!strcmp(Schriftart,"Senior Service")){return 1.686000000000000;}
  else if (!strcmp(Schriftart,"Send Cash")){return 1.225000000000000;}
  else if (!strcmp(Schriftart,"Scritzy")){return 1.224000000000000;}
  else if (!strcmp(Schriftart,"Screengem")){return 1.011000000000000;}
  else if (!strcmp(Schriftart,"Schoolbook Uralic")){return 0.990000000000000;}
  else if (!strcmp(Schriftart,"Scheherazade")){return 0.721679687500000;}
  else if (!strcmp(Schriftart,"Scalelines Maze BRK")){return 0.877000000000000;}
  else if (!strcmp(Schriftart,"Scalelines BRK")){return 0.923000000000000;}
  else if (!strcmp(Schriftart,"Sawasdee")){return 1.154785156250000;}
  else if (!strcmp(Schriftart,"Saved By Zero")){return 1.046000000000000;}
  else if (!strcmp(Schriftart,"Saunder BRK")){return 0.359000000000000;}
  else if (!strcmp(Schriftart,"Satanic Participants")){return 1.100000000000000;}
  else if (!strcmp(Schriftart,"Sarcastic BRK")){return 0.341000000000000;}
  else if (!strcmp(Schriftart,"Sarai")){return 0.752000000000000;}
  else if (!strcmp(Schriftart,"Sans Uralic")){return 1.121000000000000;}
  else if (!strcmp(Schriftart,"Sans Condensed Uralic")){return 0.787000000000000;}
  else if (!strcmp(Schriftart,"Sandoval")){return 1.309000000000000;}
  else if (!strcmp(Schriftart,"Samanata")){return 1.040039062500000;}
  else if (!strcmp(Schriftart,"Salem")){return 0.966111111111111;}
  else if (!strcmp(Schriftart,"Sahadeva")){return 0.948730468750000;}
  else if (!strcmp(Schriftart,"Sagar")){return 0.965820312500000;}
  else if (!strcmp(Schriftart,"Sad Films")){return 0.740000000000000;}
  else if (!strcmp(Schriftart,"Saab")){return 0.706054687500000;}
  else if (!strcmp(Schriftart,"Ryuker BRK")){return 0.818000000000000;}
  else if (!strcmp(Schriftart,"Rustproof Body")){return 1.613000000000000;}
  else if (!strcmp(Schriftart,"Rufscript")){return 0.872070312500000;}
  else if (!strcmp(Schriftart,"rsfs10")){return 1.150000000000000;}
  else if (!strcmp(Schriftart,"Rough Day BRK")){return 0.734000000000000;}
  else if (!strcmp(Schriftart,"Rotund Outline BRK")){return 1.058000000000000;}
  else if (!strcmp(Schriftart,"Rotund BRK")){return 1.089000000000000;}
  else if (!strcmp(Schriftart,"Rothwell")){return 0.700000000000000;}
  else if (!strcmp(Schriftart,"Roman Uralic")){return 1.019000000000000;}
  else if (!strcmp(Schriftart,"Rod Transparent")){return 0.773437500000000;}
  else if (!strcmp(Schriftart,"Rod")){return 0.808105468750000;}
  else if (!strcmp(Schriftart,"Roboto Thin")){return 1.002929687500000;}
  else if (!strcmp(Schriftart,"Roboto Medium")){return 0.953613281250000;}
  else if (!strcmp(Schriftart,"Roboto Light")){return 0.960449218750000;}
  else if (!strcmp(Schriftart,"Roboto Condensed Light")){return 0.824707031250000;}
  else if (!strcmp(Schriftart,"Roboto Condensed")){return 0.827148437500000;}
  else if (!strcmp(Schriftart,"Roboto Black")){return 0.924804687500000;}
  else if (!strcmp(Schriftart,"Roboto")){return 0.966308593750000;}
  else if (!strcmp(Schriftart,"RoboKoz")){return 1.342000000000000;}
  else if (!strcmp(Schriftart,"Riot Act")){return 1.129000000000000;}
  else if (!strcmp(Schriftart,"Rina")){return 1.149000000000000;}
  else if (!strcmp(Schriftart,"Revert Round BRK")){return 0.899000000000000;}
  else if (!strcmp(Schriftart,"Revert BRK")){return 0.899000000000000;}
  else if (!strcmp(Schriftart,"Relish Gargler")){return 0.865000000000000;}
  else if (!strcmp(Schriftart,"Relapse BRK")){return 0.823000000000000;}
  else if (!strcmp(Schriftart,"Rekha")){return 0.926757812500000;}
  else if (!strcmp(Schriftart,"Rehearsal Point BRK")){return 0.663000000000000;}
  else if (!strcmp(Schriftart,"Rehearsal Offset BRK")){return 0.612000000000000;}
  else if (!strcmp(Schriftart,"Rehearsal Curve BRK")){return 0.663000000000000;}
  else if (!strcmp(Schriftart,"Rehan")){return 0.966111111111111;}
  else if (!strcmp(Schriftart,"Registry BRK")){return 0.885000000000000;}
  else if (!strcmp(Schriftart,"Regenerate BRK")){return 0.935000000000000;}
  else if (!strcmp(Schriftart,"Redundant BRK")){return 0.562000000000000;}
  else if (!strcmp(Schriftart,"Reason Shadow BRK")){return 0.676000000000000;}
  else if (!strcmp(Schriftart,"Reason BRK")){return 0.564000000000000;}
  else if (!strcmp(Schriftart,"Razor Keen")){return 0.476000000000000;}
  else if (!strcmp(Schriftart,"Raydiate BRK")){return 1.172000000000000;}
  else if (!strcmp(Schriftart,"RaviPrakash")){return 0.634765625000000;}
  else if (!strcmp(Schriftart,"Ravenous Caterpillar BRK")){return 0.420000000000000;}
  else if (!strcmp(Schriftart,"Ravaged By Years BRK")){return 1.154000000000000;}
  else if (!strcmp(Schriftart,"Rasheeq-Bold")){return 0.957222222222222;}
  else if (!strcmp(Schriftart,"Rambling BRK")){return 0.790000000000000;}
  else if (!strcmp(Schriftart,"RaghuMalayalam")){return 0.570312500000000;}
  else if (!strcmp(Schriftart,"Radis Sans")){return 1.073000000000000;}
  else if (!strcmp(Schriftart,"Radio Stars")){return 0.939000000000000;}
  else if (!strcmp(Schriftart,"Radios in Motion Hard")){return 2.963000000000000;}
  else if (!strcmp(Schriftart,"Radios in Motion")){return 2.963000000000000;}
  else if (!strcmp(Schriftart,"Radiation Participants")){return 1.100000000000000;}
  else if (!strcmp(Schriftart,"Rachana")){return 0.948242187500000;}
  else if (!strcmp(Schriftart,"Raavi")){return 0.765625000000000;}
  else if (!strcmp(Schriftart,"Quixotic")){return 0.421386718750000;}
  else if (!strcmp(Schriftart,"Quinquefoliolate")){return 1.265625000000000;}
  else if (!strcmp(Schriftart,"Quinoline")){return 0.619000000000000;}
  else if (!strcmp(Schriftart,"Quinine")){return 0.819000000000000;}
  else if (!strcmp(Schriftart,"Quill Experimental S BRK")){return 0.519000000000000;}
  else if (!strcmp(Schriftart,"Quill Experimental O BRK")){return 0.519000000000000;}
  else if (!strcmp(Schriftart,"Quercus")){return 1.111818181818181;}
  else if (!strcmp(Schriftart,"Queasy Outline BRK")){return 0.342000000000000;}
  else if (!strcmp(Schriftart,"Queasy BRK")){return 0.325000000000000;}
  else if (!strcmp(Schriftart,"Quarterly Thin BRK")){return 0.608000000000000;}
  else if (!strcmp(Schriftart,"Quarterly Thick BRK")){return 0.625000000000000;}
  else if (!strcmp(Schriftart,"Quarantine BRK")){return 0.573000000000000;}
  else if (!strcmp(Schriftart,"Quantum Taper BRK")){return 0.760000000000000;}
  else if (!strcmp(Schriftart,"Quantum Round Hollow BRK")){return 0.819000000000000;}
  else if (!strcmp(Schriftart,"Quantum Round BRK")){return 0.821000000000000;}
  else if (!strcmp(Schriftart,"Quantum Flat Hollow BRK")){return 1.208000000000000;}
  else if (!strcmp(Schriftart,"Quantum Flat BRK")){return 1.210000000000000;}
  else if (!strcmp(Schriftart,"Quantity")){return 1.663574218750000;}
  else if (!strcmp(Schriftart,"Quandary BRK")){return 0.750000000000000;}
  else if (!strcmp(Schriftart,"Quadratic Cal BRK")){return 0.685000000000000;}
  else if (!strcmp(Schriftart,"Quadratic BRK")){return 0.686000000000000;}
  else if (!strcmp(Schriftart,"Quadrangle")){return 1.503000000000000;}
  else if (!strcmp(Schriftart,"Quadaptor")){return 0.930000000000000;}
  else if (!strcmp(Schriftart,"Quacksalver BRK")){return 0.639000000000000;}
  else if (!strcmp(Schriftart,"QSwitch Ax")){return 0.933593750000000;}
  else if (!strcmp(Schriftart,"Qlumpy Shadow BRK")){return 0.712000000000000;}
  else if (!strcmp(Schriftart,"Qlumpy BRK")){return 0.682000000000000;}
  else if (!strcmp(Schriftart,"Qbicle 4 BRK")){return 1.089000000000000;}
  else if (!strcmp(Schriftart,"Qbicle 3 BRK")){return 1.089000000000000;}
  else if (!strcmp(Schriftart,"Qbicle 2 BRK")){return 1.089000000000000;}
  else if (!strcmp(Schriftart,"Qbicle 1 BRK")){return 1.285000000000000;}
  else if (!strcmp(Schriftart,"Pyrite")){return 0.710000000000000;}
  else if (!strcmp(Schriftart,"Purisa")){return 1.178000000000000;}
  else if (!strcmp(Schriftart,"Pupcat")){return 0.590000000000000;}
  else if (!strcmp(Schriftart,"Pulse State")){return 0.679000000000000;}
  else if (!strcmp(Schriftart,"Pseudo BRK")){return 1.044000000000000;}
  else if (!strcmp(Schriftart,"progenisis")){return 0.499023437500000;}
  else if (!strcmp(Schriftart,"Prociono TT")){return 1.000000000000000;}
  else if (!strcmp(Schriftart,"Primer Print")){return 0.796666666666666;}
  else if (!strcmp(Schriftart,"Primer Apples")){return 0.945000000000000;}
  else if (!strcmp(Schriftart,"Prime Minister of Canada")){return 0.940000000000000;}
  else if (!strcmp(Schriftart,"Pricedown")){return 0.687000000000000;}
  else if (!strcmp(Schriftart,"PresidentGas")){return 0.538000000000000;}
  else if (!strcmp(Schriftart,"Powderworks BRK")){return 1.073000000000000;}
  else if (!strcmp(Schriftart,"Pothana2000")){return 0.643200000000000;}
  else if (!strcmp(Schriftart,"Port Credit")){return 0.987000000000000;}
  else if (!strcmp(Schriftart,"Pop Up Fontio")){return 0.611000000000000;}
  else if (!strcmp(Schriftart,"Ponnala")){return 0.633789062500000;}
  else if (!strcmp(Schriftart,"Poke")){return 0.360000000000000;}
  else if (!strcmp(Schriftart,"Po Beef")){return 0.553000000000000;}
  else if (!strcmp(Schriftart,"Pneumatics Wide BRK")){return 0.957000000000000;}
  else if (!strcmp(Schriftart,"Pneumatics Tall BRK")){return 0.619000000000000;}
  else if (!strcmp(Schriftart,"Pneumatics BRK")){return 0.795000000000000;}
  else if (!strcmp(Schriftart,"Play")){return 0.993000000000000;}
  else if (!strcmp(Schriftart,"Plastic Bag")){return 0.683000000000000;}
  else if (!strcmp(Schriftart,"Plasmatic")){return 0.566000000000000;}
  else if (!strcmp(Schriftart,"Plasma Drip Empty BRK")){return 0.937000000000000;}
  else if (!strcmp(Schriftart,"Plasma Drip BRK")){return 0.949000000000000;}
  else if (!strcmp(Schriftart,"Planet Benson 2")){return 0.863000000000000;}
  else if (!strcmp(Schriftart,"Plain Cred 1978")){return 0.802000000000000;}
  else if (!strcmp(Schriftart,"Plain Cred")){return 1.219000000000000;}
  else if (!strcmp(Schriftart,"Pixel Krud BRK")){return 0.970000000000000;}
  else if (!strcmp(Schriftart,"Pitabek")){return 1.073242187500000;}
  else if (!strcmp(Schriftart,"Pindown X Plain BRK")){return 0.445000000000000;}
  else if (!strcmp(Schriftart,"Pindown X BRK")){return 0.460000000000000;}
  else if (!strcmp(Schriftart,"Pindown Plain BRK")){return 0.592000000000000;}
  else if (!strcmp(Schriftart,"Pindown BRK")){return 0.584000000000000;}
  else if (!strcmp(Schriftart,"Pincers BRK")){return 0.715000000000000;}
  else if (!strcmp(Schriftart,"Phorfeit Slant BRK")){return 1.011000000000000;}
  else if (!strcmp(Schriftart,"Phorfeit Regular BRK")){return 0.845000000000000;}
  else if (!strcmp(Schriftart,"Phetsarath OT")){return 0.965820312500000;}
  else if (!strcmp(Schriftart,"Petra")){return 0.966111111111111;}
  else if (!strcmp(Schriftart,"Persuasion BRK")){return 0.990000000000000;}
  else if (!strcmp(Schriftart,"Perfect Dark BRK")){return 1.050000000000000;}
  else if (!strcmp(Schriftart,"Penguin Attack")){return 0.870117187500000;}
  else if (!strcmp(Schriftart,"Peatloaf")){return 1.136000000000000;}
  else if (!strcmp(Schriftart,"PC Tennessee")){return 1.007812500000000;}
  else if (!strcmp(Schriftart,"PC Brussels")){return 1.250000000000000;}
  else if (!strcmp(Schriftart,"Pastor of Muppets")){return 0.409000000000000;}
  else if (!strcmp(Schriftart,"ParaAminobenzoic")){return 0.813000000000000;}
  else if (!strcmp(Schriftart,"Pants Patrol")){return 1.166000000000000;}
  else if (!strcmp(Schriftart,"Palladio Uralic")){return 1.032000000000000;}
  else if (!strcmp(Schriftart,"Palatino Linotype")){return 1.069824218750000;}
  else if (!strcmp(Schriftart,"Pakenham")){return 0.535000000000000;}
  else if (!strcmp(Schriftart,"Paint Boy")){return 0.955000000000000;}
  else if (!strcmp(Schriftart,"Pagul")){return 0.908691406250000;}
  else if (!strcmp(Schriftart,"padmmaa")){return 0.995117187500000;}
  else if (!strcmp(Schriftart,"Overload")){return 0.673000000000000;}
  else if (!strcmp(Schriftart,"Overhead BRK")){return 0.372000000000000;}
  else if (!strcmp(Schriftart,"Outright Televism")){return 0.576000000000000;}
  else if (!strcmp(Schriftart,"Outer Sider BRK")){return 0.936000000000000;}
  else if (!strcmp(Schriftart,"Ouijadork")){return 1.088000000000000;}
  else if (!strcmp(Schriftart,"Ouhod-Bold")){return 0.957222222222222;}
  else if (!strcmp(Schriftart,"Ostorah")){return 0.966111111111111;}
  else if (!strcmp(Schriftart,"OskiWest")){return 1.203125000000000;}
  else if (!strcmp(Schriftart,"OskiEast")){return 1.203125000000000;}
  else if (!strcmp(Schriftart,"OskiDeneS")){return 1.203125000000000;}
  else if (!strcmp(Schriftart,"OskiDeneC")){return 1.203125000000000;}
  else if (!strcmp(Schriftart,"OskiDeneB")){return 1.203125000000000;}
  else if (!strcmp(Schriftart,"OskiDeneA")){return 1.203125000000000;}
  else if (!strcmp(Schriftart,"OskiDakelh")){return 1.203125000000000;}
  else if (!strcmp(Schriftart,"OskiBlackfoot")){return 1.203125000000000;}
  else if (!strcmp(Schriftart,"Orbicular BRK")){return 0.368000000000000;}
  else if (!strcmp(Schriftart,"Orange Kid")){return 0.680000000000000;}
  else if (!strcmp(Schriftart,"Opiated BRK")){return 0.924000000000000;}
  else if (!strcmp(Schriftart,"Operational Amplifier")){return 1.801000000000000;}
  else if (!strcmp(Schriftart,"OpenDinSchriftenEngshrift")){return 0.850000000000000;}
  else if (!strcmp(Schriftart,"Oliver's Barney")){return 0.980000000000000;}
  else if (!strcmp(Schriftart,"Old Standard TT")){return 1.218000000000000;}
  else if (!strcmp(Schriftart,"okolaks_g")){return 1.125000000000000;}
  else if (!strcmp(Schriftart,"Oklahoma")){return 1.053710937500000;}
  else if (!strcmp(Schriftart,"Off Kilter R BRK")){return 1.203000000000000;}
  else if (!strcmp(Schriftart,"Off Kilter L BRK")){return 1.203000000000000;}
  else if (!strcmp(Schriftart,"Octoville")){return 0.657000000000000;}
  else if (!strcmp(Schriftart,"OCRAItalic")){return 0.871000000000000;}
  else if (!strcmp(Schriftart,"OCRACondensed")){return 0.832000000000000;}
  else if (!strcmp(Schriftart,"OCRABold")){return 0.850000000000000;}
  else if (!strcmp(Schriftart,"OCRA")){return 0.870000000000000;}
  else if (!strcmp(Schriftart,"Obstacle Lines BRK")){return 0.558000000000000;}
  else if (!strcmp(Schriftart,"Obstacle BRK")){return 0.558000000000000;}
  else if (!strcmp(Schriftart,"Obloquy Solid BRK")){return 0.977000000000000;}
  else if (!strcmp(Schriftart,"Obloquy Outline BRK")){return 0.956000000000000;}
  else if (!strcmp(Schriftart,"Nymonak BRK")){return 0.618000000000000;}
  else if (!strcmp(Schriftart,"Nuosu SIL")){return 0.997070312500000;}
  else if (!strcmp(Schriftart,"Numskull BRK")){return 0.885000000000000;}
  else if (!strcmp(Schriftart,"Nucleus BRK")){return 0.829000000000000;}
  else if (!strcmp(Schriftart,"Not Quite Right BRK")){return 1.114000000000000;}
  else if (!strcmp(Schriftart,"Noto Serif Lao")){return 0.625000000000000;}
  else if (!strcmp(Schriftart,"Noto Serif")){return 1.152832031250000;}
  else if (!strcmp(Schriftart,"Noto Sans UI")){return 1.060058593750000;}
  else if (!strcmp(Schriftart,"Noto Sans Lao UI")){return 0.637695312500000;}
  else if (!strcmp(Schriftart,"Noto Sans Lao")){return 0.637695312500000;}
  else if (!strcmp(Schriftart,"Noto Sans Khmer UI")){return 0.637695312500000;}
  else if (!strcmp(Schriftart,"Noto Sans Khmer")){return 0.637695312500000;}
  else if (!strcmp(Schriftart,"Noto Sans")){return 1.060058593750000;}
  else if (!strcmp(Schriftart,"Nostalgia BRK")){return 0.995000000000000;}
  else if (!strcmp(Schriftart,"Norasi")){return 1.155000000000000;}
  else if (!strcmp(Schriftart,"Nominal BRK")){return 0.954000000000000;}
  else if (!strcmp(Schriftart,"Nightporter")){return 0.978000000000000;}
  else if (!strcmp(Schriftart,"Night Court")){return 0.740000000000000;}
  else if (!strcmp(Schriftart,"Nice")){return 0.966111111111111;}
  else if (!strcmp(Schriftart,"New Brilliant")){return 2.798000000000000;}
  else if (!strcmp(Schriftart,"Neuropolitical")){return 1.195800781250000;}
  else if (!strcmp(Schriftart,"Neuropol")){return 1.134277343750000;}
  else if (!strcmp(Schriftart,"Neurochrome")){return 1.115000000000000;}
  else if (!strcmp(Schriftart,"Neural Outline BRK")){return 0.633000000000000;}
  else if (!strcmp(Schriftart,"Neural BRK")){return 0.629000000000000;}
  else if (!strcmp(Schriftart,"Nazli")){return 0.866210937500000;}
  else if (!strcmp(Schriftart,"Navilu")){return 1.515136718750000;}
  else if (!strcmp(Schriftart,"Naughts BRK")){return 0.568000000000000;}
  else if (!strcmp(Schriftart,"Nasalization")){return 1.152000000000000;}
  else if (!strcmp(Schriftart,"Nasal")){return 0.939453125000000;}
  else if (!strcmp(Schriftart,"Narkisim")){return 0.857910156250000;}
  else if (!strcmp(Schriftart,"Nanosecond Wide BRK")){return 1.077000000000000;}
  else if (!strcmp(Schriftart,"Nanosecond Thin BRK")){return 0.654000000000000;}
  else if (!strcmp(Schriftart,"Nanosecond Thick BRK")){return 0.682000000000000;}
  else if (!strcmp(Schriftart,"Nakula")){return 0.948730468750000;}
  else if (!strcmp(Schriftart,"Nagham")){return 0.966111111111111;}
  else if (!strcmp(Schriftart,"NAFTAlene")){return 0.627000000000000;}
  else if (!strcmp(Schriftart,"Nafees Web Naskh")){return 0.796875000000000;}
  else if (!strcmp(Schriftart,"Nafees Nastaleeq")){return 2.944824218750000;}
  else if (!strcmp(Schriftart,"Nada")){return 0.966111111111111;}
  else if (!strcmp(Schriftart,"Mysterons BRK")){return 1.010000000000000;}
  else if (!strcmp(Schriftart,"MV Boli")){return 0.789062500000000;}
  else if (!strcmp(Schriftart,"Musica")){return 0.838867187500000;}
  else if (!strcmp(Schriftart,"MuktiNarrow")){return 0.965820312500000;}
  else if (!strcmp(Schriftart,"Mukti")){return 0.965820312500000;}
  else if (!strcmp(Schriftart,"Mufferaw")){return 0.690000000000000;}
  else if (!strcmp(Schriftart,"msbm10")){return 1.007812500000000;}
  else if (!strcmp(Schriftart,"mry_KacstQurn")){return 0.965332031250000;}
  else if (!strcmp(Schriftart,"Motorcade")){return 0.698000000000000;}
  else if (!strcmp(Schriftart,"Moronic Misfire BRK")){return 0.839000000000000;}
  else if (!strcmp(Schriftart,"Mono Uralic")){return 0.960000000000000;}
  else if (!strcmp(Schriftart,"Monofonto")){return 0.513000000000000;}
  else if (!strcmp(Schriftart,"Monkey Phonics BRK")){return 1.704000000000000;}
  else if (!strcmp(Schriftart,"Monapo")){return 0.830078125000000;}
  else if (!strcmp(Schriftart,"Mona")){return 0.829101562500000;}
  else if (!strcmp(Schriftart,"Mold Papa")){return 0.508000000000000;}
  else if (!strcmp(Schriftart,"MOESongUN")){return 0.661132812500000;}
  else if (!strcmp(Schriftart,"Model Worker")){return 0.629000000000000;}
  else if (!strcmp(Schriftart,"Mobilize BRK")){return 0.613000000000000;}
  else if (!strcmp(Schriftart,"Mob Concrete")){return 0.839000000000000;}
  else if (!strcmp(Schriftart,"MMCedar P")){return 0.900390625000000;}
  else if (!strcmp(Schriftart,"MMCedar")){return 0.602539062500000;}
  else if (!strcmp(Schriftart,"Mlurmlry")){return 0.604980468750000;}
  else if (!strcmp(Schriftart,"Mister Firley")){return 1.066000000000000;}
  else if (!strcmp(Schriftart,"Mississauga")){return 0.758000000000000;}
  else if (!strcmp(Schriftart,"Misirlou Day")){return 0.813000000000000;}
  else if (!strcmp(Schriftart,"Misirlou")){return 0.755000000000000;}
  else if (!strcmp(Schriftart,"Mishmash Fuse BRK")){return 0.823000000000000;}
  else if (!strcmp(Schriftart,"Mishmash BRK")){return 0.820000000000000;}
  else if (!strcmp(Schriftart,"Mishmash ALT2 BRK")){return 0.682000000000000;}
  else if (!strcmp(Schriftart,"Mishmash ALT1 BRK")){return 0.682000000000000;}
  else if (!strcmp(Schriftart,"Mishmash 4x4o BRK")){return 0.803000000000000;}
  else if (!strcmp(Schriftart,"Mishmash 4x4i BRK")){return 0.698000000000000;}
  else if (!strcmp(Schriftart,"Miriam Transparent")){return 0.860351562500000;}
  else if (!strcmp(Schriftart,"Miriam Fixed")){return 0.752929687500000;}
  else if (!strcmp(Schriftart,"Miriam")){return 0.860351562500000;}
  else if (!strcmp(Schriftart,"Minya Nouvelle")){return 1.042000000000000;}
  else if (!strcmp(Schriftart,"Minya")){return 1.105000000000000;}
  else if (!strcmp(Schriftart,"Minisystem")){return 1.092000000000000;}
  else if (!strcmp(Schriftart,"Mini Kaliber S TT BRK")){return 0.600000000000000;}
  else if (!strcmp(Schriftart,"Mini Kaliber O TT BRK")){return 0.600000000000000;}
  else if (!strcmp(Schriftart,"Mincer BRK")){return 0.629000000000000;}
  else if (!strcmp(Schriftart,"Migu 2M")){return 0.610000000000000;}
  else if (!strcmp(Schriftart,"Migu 1P")){return 1.010000000000000;}
  else if (!strcmp(Schriftart,"Migu 1M")){return 0.610000000000000;}
  else if (!strcmp(Schriftart,"Migu 1C")){return 0.943000000000000;}
  else if (!strcmp(Schriftart,"MigMix 2P")){return 1.010000000000000;}
  else if (!strcmp(Schriftart,"MigMix 2M")){return 0.610000000000000;}
  else if (!strcmp(Schriftart,"MigMix 1P")){return 1.010000000000000;}
  else if (!strcmp(Schriftart,"MigMix 1M")){return 0.610000000000000;}
  else if (!strcmp(Schriftart,"Microsoft Sans Serif")){return 0.956054687500000;}
  else if (!strcmp(Schriftart,"MgOpen Moderna")){return 1.163000000000000;}
  else if (!strcmp(Schriftart,"MgOpen Modata")){return 1.165000000000000;}
  else if (!strcmp(Schriftart,"MgOpen Cosmetica")){return 0.949000000000000;}
  else if (!strcmp(Schriftart,"MgOpen Canonica")){return 1.131000000000000;}
  else if (!strcmp(Schriftart,"Mexcellent 3D")){return 0.820000000000000;}
  else if (!strcmp(Schriftart,"Mexcellent")){return 0.690000000000000;}
  else if (!strcmp(Schriftart,"Metal Lord")){return 0.882000000000000;}
  else if (!strcmp(Schriftart,"Metal")){return 0.966111111111111;}
  else if (!strcmp(Schriftart,"Meera Tamil")){return 0.708007812500000;}
  else if (!strcmp(Schriftart,"Meera")){return 0.931152343750000;}
  else if (!strcmp(Schriftart,"Maya")){return 1.054687500000000;}
  else if (!strcmp(Schriftart,"Mathematica4Mono")){return 2.638183593750000;}
  else if (!strcmp(Schriftart,"Mathematica4")){return 1.750000000000000;}
  else if (!strcmp(Schriftart,"Mathematica3Mono")){return 1.228027343750000;}
  else if (!strcmp(Schriftart,"Mathematica3")){return 1.132812500000000;}
  else if (!strcmp(Schriftart,"Mathematica2Mono")){return 1.927246093750000;}
  else if (!strcmp(Schriftart,"Mathematica2")){return 1.758789062500000;}
  else if (!strcmp(Schriftart,"Mathematica1Mono")){return 0.718261718750000;}
  else if (!strcmp(Schriftart,"Mathematica1")){return 0.900878906250000;}
  else if (!strcmp(Schriftart,"Matara Supplement")){return 1.075683593750000;}
  else if (!strcmp(Schriftart,"Matara")){return 0.880859375000000;}
  else if (!strcmp(Schriftart,"MassiveRetaliation")){return 0.649000000000000;}
  else if (!strcmp(Schriftart,"Masinahikan Dene")){return 0.806152343750000;}
  else if (!strcmp(Schriftart,"Masinahikan")){return 0.806152343750000;}
  else if (!strcmp(Schriftart,"Mashq-Bold")){return 0.957222222222222;}
  else if (!strcmp(Schriftart,"Mashq")){return 0.966111111111111;}
  else if (!strcmp(Schriftart,"Maryland")){return 0.643066406250000;}
  else if (!strcmp(Schriftart,"Marquee Moon")){return 0.606933593750000;}
  else if (!strcmp(Schriftart,"Marked Fool")){return 0.757812500000000;}
  else if (!strcmp(Schriftart,"Map Of You")){return 0.846000000000000;}
  else if (!strcmp(Schriftart,"Mangal")){return 0.677734375000000;}
  else if (!strcmp(Schriftart,"Manchu")){return 0.759277343750000;}
  else if (!strcmp(Schriftart,"Malithi Web")){return 0.485839843750000;}
  else if (!strcmp(Schriftart,"Malache Crunch")){return 1.105000000000000;}
  else if (!strcmp(Schriftart,"Mai Tai")){return 0.468000000000000;}
  else if (!strcmp(Schriftart,"Mail Ray Stuff")){return 0.721000000000000;}
  else if (!strcmp(Schriftart,"Mad's Scrawl BRK")){return 0.753000000000000;}
  else if (!strcmp(Schriftart,"Macropsia BRK")){return 1.130000000000000;}
  else if (!strcmp(Schriftart,"M+ 2p thin")){return 0.983000000000000;}
  else if (!strcmp(Schriftart,"M+ 2p regular")){return 0.999000000000000;}
  else if (!strcmp(Schriftart,"M+ 2p medium")){return 1.006000000000000;}
  else if (!strcmp(Schriftart,"M+ 2p light")){return 0.990000000000000;}
  else if (!strcmp(Schriftart,"M+ 2p heavy")){return 1.014000000000000;}
  else if (!strcmp(Schriftart,"M+ 2p bold")){return 1.011000000000000;}
  else if (!strcmp(Schriftart,"M+ 2p black")){return 1.019000000000000;}
  else if (!strcmp(Schriftart,"M+ 2m thin")){return 0.630000000000000;}
  else if (!strcmp(Schriftart,"M+ 2m regular")){return 0.610000000000000;}
  else if (!strcmp(Schriftart,"M+ 2m medium")){return 0.598000000000000;}
  else if (!strcmp(Schriftart,"M+ 2m light")){return 0.620000000000000;}
  else if (!strcmp(Schriftart,"M+ 2m bold")){return 0.590000000000000;}
  else if (!strcmp(Schriftart,"M+ 2c thin")){return 0.923000000000000;}
  else if (!strcmp(Schriftart,"M+ 2c regular")){return 0.925000000000000;}
  else if (!strcmp(Schriftart,"M+ 2c medium")){return 0.926000000000000;}
  else if (!strcmp(Schriftart,"M+ 2c light")){return 0.924000000000000;}
  else if (!strcmp(Schriftart,"M+ 2c heavy")){return 0.948000000000000;}
  else if (!strcmp(Schriftart,"M+ 2c bold")){return 0.937000000000000;}
  else if (!strcmp(Schriftart,"M+ 2c black")){return 0.959000000000000;}
  else if (!strcmp(Schriftart,"M+ 1p thin")){return 0.983000000000000;}
  else if (!strcmp(Schriftart,"M+ 1p regular")){return 0.999000000000000;}
  else if (!strcmp(Schriftart,"M+ 1p medium")){return 1.006000000000000;}
  else if (!strcmp(Schriftart,"M+ 1p light")){return 0.990000000000000;}
  else if (!strcmp(Schriftart,"M+ 1p heavy")){return 1.014000000000000;}
  else if (!strcmp(Schriftart,"M+ 1p bold")){return 1.011000000000000;}
  else if (!strcmp(Schriftart,"M+ 1p black")){return 1.019000000000000;}
  else if (!strcmp(Schriftart,"M+ 1m thin")){return 0.630000000000000;}
  else if (!strcmp(Schriftart,"M+ 1m regular")){return 0.610000000000000;}
  else if (!strcmp(Schriftart,"M+ 1mn thin")){return 0.625000000000000;}
  else if (!strcmp(Schriftart,"M+ 1mn regular")){return 0.610000000000000;}
  else if (!strcmp(Schriftart,"M+ 1mn medium")){return 0.598000000000000;}
  else if (!strcmp(Schriftart,"M+ 1mn light")){return 0.618000000000000;}
  else if (!strcmp(Schriftart,"M+ 1mn bold")){return 0.590000000000000;}
  else if (!strcmp(Schriftart,"M+ 1m medium")){return 0.598000000000000;}
  else if (!strcmp(Schriftart,"M+ 1m light")){return 0.620000000000000;}
  else if (!strcmp(Schriftart,"M+ 1m bold")){return 0.590000000000000;}
  else if (!strcmp(Schriftart,"M+ 1c thin")){return 0.923000000000000;}
  else if (!strcmp(Schriftart,"M+ 1c regular")){return 0.925000000000000;}
  else if (!strcmp(Schriftart,"M+ 1c medium")){return 0.926000000000000;}
  else if (!strcmp(Schriftart,"M+ 1c light")){return 0.924000000000000;}
  else if (!strcmp(Schriftart,"M+ 1c heavy")){return 0.948000000000000;}
  else if (!strcmp(Schriftart,"M+ 1c bold")){return 0.937000000000000;}
  else if (!strcmp(Schriftart,"M+ 1c black")){return 0.959000000000000;}
  else if (!strcmp(Schriftart,"LYNX BRK")){return 1.090000000000000;}
  else if (!strcmp(Schriftart,"Lyneous Linear BRK")){return 0.540000000000000;}
  else if (!strcmp(Schriftart,"Lyneous BRK")){return 0.645000000000000;}
  else if (!strcmp(Schriftart,"Lunaurora")){return 1.335000000000000;}
  else if (!strcmp(Schriftart,"Lunasol")){return 1.375000000000000;}
  else if (!strcmp(Schriftart,"Lunasequent")){return 2.224000000000000;}
  else if (!strcmp(Schriftart,"LunaEclipsed ")){return 1.847167968750000;}
  else if (!strcmp(Schriftart,"LuckyApe")){return 0.428000000000000;}
  else if (!strcmp(Schriftart,"Lucid Type B Outline BRK")){return 0.562000000000000;}
  else if (!strcmp(Schriftart,"Lucid Type B BRK")){return 0.553000000000000;}
  else if (!strcmp(Schriftart,"Lucid Type A Outline BRK")){return 0.518000000000000;}
  else if (!strcmp(Schriftart,"Lucid Type A BRK")){return 0.505000000000000;}
  else if (!strcmp(Schriftart,"Lucida Sans Unicode")){return 1.029785156250000;}
  else if (!strcmp(Schriftart,"Lucida Console")){return 0.752929687500000;}
  else if (!strcmp(Schriftart,"Lowdown BRK")){return 1.311000000000000;}
  else if (!strcmp(Schriftart,"Loma")){return 1.106445312500000;}
  else if (!strcmp(Schriftart,"Lohit Tamil Classical")){return 0.701171875000000;}
  else if (!strcmp(Schriftart,"Lohit Tamil")){return 0.701171875000000;}
  else if (!strcmp(Schriftart,"Lohit Punjabi")){return 0.702210663198959;}
  else if (!strcmp(Schriftart,"Lohit Kannada")){return 0.806640625000000;}
  else if (!strcmp(Schriftart,"Lohit Gujarati")){return 0.745498199279711;}
  else if (!strcmp(Schriftart,"Lohit Devanagari")){return 0.695312500000000;}
  else if (!strcmp(Schriftart,"Lohit Bengali")){return 0.729518855656697;}
  else if (!strcmp(Schriftart,"Lohit Assamese")){return 0.729518855656697;}
  else if (!strcmp(Schriftart,"Lockergnome")){return 1.062000000000000;}
  else if (!strcmp(Schriftart,"Living by Numbers")){return 0.792000000000000;}
  else if (!strcmp(Schriftart,"Line Dings BRK")){return 0.911000000000000;}
  else if (!strcmp(Schriftart,"Limon S7")){return 0.699000000000000;}
  else if (!strcmp(Schriftart,"Limon S6")){return 0.694000000000000;}
  else if (!strcmp(Schriftart,"Limon S5")){return 0.943000000000000;}
  else if (!strcmp(Schriftart,"Limon S4")){return 0.591000000000000;}
  else if (!strcmp(Schriftart,"Limon S3")){return 0.821000000000000;}
  else if (!strcmp(Schriftart,"Limon S2")){return 0.758000000000000;}
  else if (!strcmp(Schriftart,"Limon S1")){return 0.674000000000000;}
  else if (!strcmp(Schriftart,"Limon R5")){return 0.796000000000000;}
  else if (!strcmp(Schriftart,"Limon R4")){return 0.566000000000000;}
  else if (!strcmp(Schriftart,"Limon R3")){return 0.811000000000000;}
  else if (!strcmp(Schriftart,"Limon R2")){return 0.731000000000000;}
  else if (!strcmp(Schriftart,"Limon R1")){return 0.796000000000000;}
  else if (!strcmp(Schriftart,"Limon F8")){return 0.707000000000000;}
  else if (!strcmp(Schriftart,"Limon F7")){return 0.903000000000000;}
  else if (!strcmp(Schriftart,"Limon F6")){return 0.850000000000000;}
  else if (!strcmp(Schriftart,"Limon F5")){return 0.680000000000000;}
  else if (!strcmp(Schriftart,"Limon F4")){return 0.802000000000000;}
  else if (!strcmp(Schriftart,"Limon F3")){return 0.666000000000000;}
  else if (!strcmp(Schriftart,"Limon F2")){return 0.670000000000000;}
  else if (!strcmp(Schriftart,"Limon F1")){return 0.781000000000000;}
  else if (!strcmp(Schriftart,"Lilliput Steps")){return 1.070000000000000;}
  else if (!strcmp(Schriftart,"Likhan")){return 0.766601562500000;}
  else if (!strcmp(Schriftart,"Lights Out BRK")){return 0.749000000000000;}
  else if (!strcmp(Schriftart,"Licorice Strings BRK")){return 0.630000000000000;}
  else if (!strcmp(Schriftart,"Liberation Serif")){return 1.014160156250000;}
  else if (!strcmp(Schriftart,"Liberation Sans Narrow")){return 0.844726562500000;}
  else if (!strcmp(Schriftart,"Liberation Sans")){return 1.030273437500000;}
  else if (!strcmp(Schriftart,"Liberation Mono")){return 0.740234375000000;}
  else if (!strcmp(Schriftart,"Libel Suit")){return 0.580000000000000;}
  else if (!strcmp(Schriftart,"Lewinsky")){return 1.961000000000000;}
  else if (!strcmp(Schriftart,"Levenim MT")){return 1.013671875000000;}
  else if (!strcmp(Schriftart,"Letter Set C")){return 0.848000000000000;}
  else if (!strcmp(Schriftart,"Letter Set B")){return 1.620000000000000;}
  else if (!strcmp(Schriftart,"Letter Set A")){return 1.437000000000000;}
  else if (!strcmp(Schriftart,"Letter Gothic")){return 0.707031250000000;}
  else if (!strcmp(Schriftart,"Let's Eat")){return 0.754000000000000;}
  else if (!strcmp(Schriftart,"Lethargic BRK")){return 1.410000000000000;}
  else if (!strcmp(Schriftart,"Lesser Concern")){return 0.599000000000000;}
  else if (!strcmp(Schriftart,"Lato Light")){return 1.014000000000000;}
  else if (!strcmp(Schriftart,"Lato Hairline")){return 1.019000000000000;}
  else if (!strcmp(Schriftart,"Lato Black")){return 1.076000000000000;}
  else if (!strcmp(Schriftart,"Lato")){return 1.052500000000000;}
  else if (!strcmp(Schriftart,"Latha")){return 0.861816406250000;}
  else if (!strcmp(Schriftart,"Larkspur BRK")){return 0.702000000000000;}
  else if (!strcmp(Schriftart,"Larabiefont")){return 0.680000000000000;}
  else if (!strcmp(Schriftart,"Lamebrain BRK")){return 0.950000000000000;}
  else if (!strcmp(Schriftart,"LakkiReddy")){return 0.662109375000000;}
  else if (!strcmp(Schriftart,"Lakeshore BRK")){return 0.351000000000000;}
  else if (!strcmp(Schriftart,"Lady Starlight")){return 0.476000000000000;}
  else if (!strcmp(Schriftart,"Kustom Kar")){return 1.351000000000000;}
  else if (!strcmp(Schriftart,"Kurvature BRK")){return 0.795000000000000;}
  else if (!strcmp(Schriftart,"Kredit")){return 0.574707031250000;}
  else if (!strcmp(Schriftart,"Konector O2 BRK")){return 0.443000000000000;}
  else if (!strcmp(Schriftart,"Konector O1 BRK")){return 0.443000000000000;}
  else if (!strcmp(Schriftart,"Konector Eerie BRK")){return 0.439000000000000;}
  else if (!strcmp(Schriftart,"Konector BRK")){return 0.459000000000000;}
  else if (!strcmp(Schriftart,"Komatuna P")){return 0.854492187500000;}
  else if (!strcmp(Schriftart,"Komatuna")){return 0.602539062500000;}
  else if (!strcmp(Schriftart,"Knuckle Down")){return 1.010000000000000;}
  else if (!strcmp(Schriftart,"Knot BRK")){return 0.889000000000000;}
  else if (!strcmp(Schriftart,"Kleptocracy")){return 0.512000000000000;}
  else if (!strcmp(Schriftart,"Klaudia")){return 1.440429687500000;}
  else if (!strcmp(Schriftart,"Kirsty Ink")){return 0.911000000000000;}
  else if (!strcmp(Schriftart,"Kirsty")){return 0.986000000000000;}
  else if (!strcmp(Schriftart,"Kirby No Kira Kizzu BRK")){return 0.750000000000000;}
  else if (!strcmp(Schriftart,"Kinnari")){return 1.088000000000000;}
  else if (!strcmp(Schriftart,"Kinkaid BRK")){return 0.814000000000000;}
  else if (!strcmp(Schriftart,"King Richard")){return 0.806000000000000;}
  else if (!strcmp(Schriftart,"Kimberley")){return 0.950000000000000;}
  else if (!strcmp(Schriftart,"Kicking Limos")){return 1.473000000000000;}
  else if (!strcmp(Schriftart,"Kickflip BRK")){return 0.889000000000000;}
  else if (!strcmp(Schriftart,"Khmer OS System")){return 0.965820312500000;}
  else if (!strcmp(Schriftart,"Khmer OS Siemreap")){return 0.980957031250000;}
  else if (!strcmp(Schriftart,"Khmer OS Muol Pali")){return 0.980957031250000;}
  else if (!strcmp(Schriftart,"Khmer OS Muol Light")){return 0.965820312500000;}
  else if (!strcmp(Schriftart,"Khmer OS Muol")){return 0.965820312500000;}
  else if (!strcmp(Schriftart,"Khmer OS Metal Chrieng")){return 0.965820312500000;}
  else if (!strcmp(Schriftart,"Khmer OS Freehand")){return 0.965820312500000;}
  else if (!strcmp(Schriftart,"Khmer OS Fasthand")){return 0.965820312500000;}
  else if (!strcmp(Schriftart,"Khmer OS Content")){return 0.965820312500000;}
  else if (!strcmp(Schriftart,"Khmer OS Bokor")){return 0.965820312500000;}
  else if (!strcmp(Schriftart,"Khmer OS Battambang")){return 0.965820312500000;}
  else if (!strcmp(Schriftart,"Khmer OS")){return 0.965820312500000;}
  else if (!strcmp(Schriftart,"Khalid")){return 0.966111111111111;}
  else if (!strcmp(Schriftart,"Key Ridge BRK")){return 0.690000000000000;}
  else if (!strcmp(Schriftart,"Key Ridge alt BRK")){return 0.690000000000000;}
  else if (!strcmp(Schriftart,"Kenyan Coffee")){return 0.561000000000000;}
  else if (!strcmp(Schriftart,"Kayrawan")){return 0.966111111111111;}
  else if (!strcmp(Schriftart,"Kayases")){return 0.912109375000000;}
  else if (!strcmp(Schriftart,"Katalyst inactive BRK")){return 0.950000000000000;}
  else if (!strcmp(Schriftart,"Katalyst active BRK")){return 0.946000000000000;}
  else if (!strcmp(Schriftart,"Kartika")){return 0.602050781250000;}
  else if (!strcmp(Schriftart,"Karma Suture")){return 0.611000000000000;}
  else if (!strcmp(Schriftart,"KaputaUnicode")){return 0.734375000000000;}
  else if (!strcmp(Schriftart,"kaputadotcom2004")){return 0.967285156250000;}
  else if (!strcmp(Schriftart,"KanjiStrokeOrders")){return 1.145507812500000;}
  else if (!strcmp(Schriftart,"KandyUnicode")){return 0.734375000000000;}
  else if (!strcmp(Schriftart,"Kalyani")){return 0.658000000000000;}
  else if (!strcmp(Schriftart,"Kaliber Xtreme BRK")){return 0.671000000000000;}
  else if (!strcmp(Schriftart,"Kaliber Solid BRK")){return 0.663000000000000;}
  else if (!strcmp(Schriftart,"Kaliber Round BRK")){return 0.667000000000000;}
  else if (!strcmp(Schriftart,"Kalapi")){return 0.716160220994475;}
  else if (!strcmp(Schriftart,"KacstOne")){return 0.610839843750000;}
  else if (!strcmp(Schriftart,"Jura")){return 0.958984375000000;}
  else if (!strcmp(Schriftart,"Jupiter Crash BRK")){return 0.550000000000000;}
  else if (!strcmp(Schriftart,"Junkyard")){return 0.711914062500000;}
  else if (!strcmp(Schriftart,"Junicode")){return 1.081054687500000;}
  else if (!strcmp(Schriftart,"jsMath-wasyb10")){return 1.853000000000000;}
  else if (!strcmp(Schriftart,"jsMath-wasy10")){return 1.611000000000000;}
  else if (!strcmp(Schriftart,"jsMath-stmary10")){return 1.167000000000000;}
  else if (!strcmp(Schriftart,"jsMath-rsfs10")){return 1.138000000000000;}
  else if (!strcmp(Schriftart,"jsMath-msbm10")){return 1.057000000000000;}
  else if (!strcmp(Schriftart,"jsMath-msam10")){return 1.389000000000000;}
  else if (!strcmp(Schriftart,"jsMath-lasyb10")){return 0.897000000000000;}
  else if (!strcmp(Schriftart,"jsMath-lasy10")){return 0.848000000000000;}
  else if (!strcmp(Schriftart,"jsMath-eusm10")){return 1.014000000000000;}
  else if (!strcmp(Schriftart,"jsMath-eusb10")){return 1.145000000000000;}
  else if (!strcmp(Schriftart,"jsMath-eurm10")){return 1.123000000000000;}
  else if (!strcmp(Schriftart,"jsMath-eurb10")){return 1.246000000000000;}
  else if (!strcmp(Schriftart,"jsMath-eufm10")){return 1.078000000000000;}
  else if (!strcmp(Schriftart,"jsMath-eufb10")){return 1.267000000000000;}
  else if (!strcmp(Schriftart,"jsMath-cmti10")){return 1.202000000000000;}
  else if (!strcmp(Schriftart,"jsMath-cmsy10")){return 1.229000000000000;}
  else if (!strcmp(Schriftart,"jsMath-cmss10")){return 0.974000000000000;}
  else if (!strcmp(Schriftart,"jsMath-cmr10")){return 1.046000000000000;}
  else if (!strcmp(Schriftart,"jsMath-cmmib10")){return 1.185000000000000;}
  else if (!strcmp(Schriftart,"jsMath-cmmi10")){return 1.003000000000000;}
  else if (!strcmp(Schriftart,"jsMath-cmex10")){return 1.758000000000000;}
  else if (!strcmp(Schriftart,"jsMath-cmbx10")){return 1.213000000000000;}
  else if (!strcmp(Schriftart,"jsMath-cmbsy10")){return 1.416000000000000;}
  else if (!strcmp(Schriftart,"jsMath-bbold10")){return 0.859000000000000;}
  else if (!strcmp(Schriftart,"Joystix")){return 0.873046875000000;}
  else if (!strcmp(Schriftart,"Joy Circuit")){return 2.573000000000000;}
  else if (!strcmp(Schriftart,"Jomolhari")){return 1.056640625000000;}
  else if (!strcmp(Schriftart,"Jolt Of Caffeine BRK")){return 0.975000000000000;}
  else if (!strcmp(Schriftart,"Johnny Mac Scrawl BRK")){return 0.758000000000000;}
  else if (!strcmp(Schriftart,"Johnny Fever")){return 0.810000000000000;}
  else if (!strcmp(Schriftart,"Jingopop")){return 0.522000000000000;}
  else if (!strcmp(Schriftart,"Jigsaw Trouserdrop")){return 0.950000000000000;}
  else if (!strcmp(Schriftart,"Jet")){return 0.966111111111111;}
  else if (!strcmp(Schriftart,"Jeopardize Thick BRK")){return 0.730000000000000;}
  else if (!strcmp(Schriftart,"Jeopardize BRK")){return 0.742000000000000;}
  else if (!strcmp(Schriftart,"Jekyll BRK")){return 1.043000000000000;}
  else if (!strcmp(Schriftart,"Jawbreaker OL2 BRK")){return 1.071000000000000;}
  else if (!strcmp(Schriftart,"Jawbreaker OL1 BRK")){return 1.047000000000000;}
  else if (!strcmp(Schriftart,"Jawbreaker Hard BRK")){return 1.034000000000000;}
  else if (!strcmp(Schriftart,"Jawbreaker BRK")){return 1.446000000000000;}
  else if (!strcmp(Schriftart,"Jasper Solid BRK")){return 0.376000000000000;}
  else if (!strcmp(Schriftart,"Jasper BRK")){return 0.396000000000000;}
  else if (!strcmp(Schriftart,"Jargon BRK")){return 0.655000000000000;}
  else if (!strcmp(Schriftart,"Jara")){return 1.051269531250000;}
  else if (!strcmp(Schriftart,"Japan")){return 0.966111111111111;}
  else if (!strcmp(Schriftart,"Janken BRK")){return 0.658000000000000;}
  else if (!strcmp(Schriftart,"Jamrul")){return 1.062500000000000;}
  else if (!strcmp(Schriftart,"Jagged BRK")){return 0.950000000000000;}
  else if (!strcmp(Schriftart,"It wasn't me")){return 0.485351562500000;}
  else if (!strcmp(Schriftart,"It Lives In The Swamp BRK")){return 0.561000000000000;}
  else if (!strcmp(Schriftart,"Isabella")){return 1.007812500000000;}
  else if (!strcmp(Schriftart,"Irritate BRK")){return 0.874000000000000;}
  else if (!strcmp(Schriftart,"Irish Participants")){return 1.100000000000000;}
  else if (!strcmp(Schriftart,"IPA X0208 UI&#12468;&#12471;&#12483;&#12463;")){return 0.997000000000000;}
  else if (!strcmp(Schriftart,"IPA X0208 P&#26126;&#26397;")){return 0.993000000000000;}
  else if (!strcmp(Schriftart,"IPA X0208 P&#12468;&#12471;&#12483;&#12463;")){return 0.997000000000000;}
  else if (!strcmp(Schriftart,"IPA X0208 &#26126;&#26397;")){return 0.660000000000000;}
  else if (!strcmp(Schriftart,"IPA X0208 &#12468;&#12471;&#12483;&#12463;")){return 0.711000000000000;}
  else if (!strcmp(Schriftart,"IPA UI&#12468;&#12471;&#12483;&#12463;")){return 0.711000000000000;}
  else if (!strcmp(Schriftart,"IPAsans")){return 0.701976284584980;}
  else if (!strcmp(Schriftart,"IPA PHONETICS")){return 1.244140625000000;}
  else if (!strcmp(Schriftart,"Iomanoid")){return 0.968000000000000;}
  else if (!strcmp(Schriftart,"Intersect O BRK")){return 0.928000000000000;}
  else if (!strcmp(Schriftart,"Intersect C BRK")){return 0.928000000000000;}
  else if (!strcmp(Schriftart,"Intersect BRK")){return 0.919000000000000;}
  else if (!strcmp(Schriftart,"Interplanetary Crap")){return 0.893000000000000;}
  else if (!strcmp(Schriftart,"InstantTunes")){return 1.878000000000000;}
  else if (!strcmp(Schriftart,"Ink Tank BRK")){return 0.604000000000000;}
  else if (!strcmp(Schriftart,"Ink Swipes BRK")){return 1.003000000000000;}
  else if (!strcmp(Schriftart,"Inflammable Age")){return 0.552000000000000;}
  else if (!strcmp(Schriftart,"Inevitable BRK")){return 0.332000000000000;}
  else if (!strcmp(Schriftart,"Inertia BRK")){return 1.052000000000000;}
  else if (!strcmp(Schriftart,"Induction")){return 1.455000000000000;}
  else if (!strcmp(Schriftart,"Impossibilium BRK")){return 0.893000000000000;}
  else if (!strcmp(Schriftart,"Impact")){return 0.814453125000000;}
  else if (!strcmp(Schriftart,"IcicleCountry")){return 1.358000000000000;}
  else if (!strcmp(Schriftart,"Hyperion Sunset BRK")){return 0.908000000000000;}
  else if (!strcmp(Schriftart,"HydrogenWhiskey")){return 0.890000000000000;}
  else if (!strcmp(Schriftart,"Hyde BRK")){return 1.016000000000000;}
  else if (!strcmp(Schriftart,"Husky Stash")){return 1.077000000000000;}
  else if (!strcmp(Schriftart,"Hurry Up")){return 1.118000000000000;}
  else if (!strcmp(Schriftart,"Hurontario")){return 0.278000000000000;}
  else if (!strcmp(Schriftart,"Hots")){return 0.776000000000000;}
  else if (!strcmp(Schriftart,"Horsepower")){return 0.448000000000000;}
  else if (!strcmp(Schriftart,"Hor")){return 0.966111111111111;}
  else if (!strcmp(Schriftart,"Hooked Up 101")){return 0.631000000000000;}
  else if (!strcmp(Schriftart,"Hooked on Booze")){return 0.399000000000000;}
  else if (!strcmp(Schriftart,"Home Sweet Home")){return 1.323000000000000;}
  else if (!strcmp(Schriftart,"Homespun TT BRK")){return 0.800000000000000;}
  else if (!strcmp(Schriftart,"Homa")){return 0.594238281250000;}
  else if (!strcmp(Schriftart,"Holy Smokes")){return 0.786000000000000;}
  else if (!strcmp(Schriftart,"Hippy Participants")){return 1.100000000000000;}
  else if (!strcmp(Schriftart,"Hillock BRK")){return 1.051000000000000;}
  else if (!strcmp(Schriftart,"Highway to Heck")){return 0.938000000000000;}
  else if (!strcmp(Schriftart,"Hemi Head 426")){return 1.054000000000000;}
  else if (!strcmp(Schriftart,"Hello Larry")){return 1.202000000000000;}
  else if (!strcmp(Schriftart,"Hebrew Participants")){return 1.100000000000000;}
  else if (!strcmp(Schriftart,"Heavy Heap")){return 0.955000000000000;}
  else if (!strcmp(Schriftart,"Heavy Bevel BRK")){return 0.716000000000000;}
  else if (!strcmp(Schriftart,"Hearts BRK")){return 0.686000000000000;}
  else if (!strcmp(Schriftart,"Head-Ding Maker BRK")){return 0.243000000000000;}
  else if (!strcmp(Schriftart,"Hawkeye")){return 1.257000000000000;}
  else if (!strcmp(Schriftart,"Hassle BRK")){return 0.954000000000000;}
  else if (!strcmp(Schriftart,"Haramain")){return 0.966111111111111;}
  else if (!strcmp(Schriftart,"Hani")){return 0.966111111111111;}
  else if (!strcmp(Schriftart,"Hand Me Down S BRK")){return 0.906000000000000;}
  else if (!strcmp(Schriftart,"Hand Me Down O BRK")){return 0.911000000000000;}
  else if (!strcmp(Schriftart,"Hamma Mamma Jamma")){return 0.523000000000000;}
  else if (!strcmp(Schriftart,"Hairball BRK")){return 0.562000000000000;}
  else if (!strcmp(Schriftart,"Hack &amp; Slash BRK")){return 0.442000000000000;}
  else if (!strcmp(Schriftart,"Gyrussian")){return 1.285000000000000;}
  else if (!strcmp(Schriftart,"Gyrose Squeeze BRK")){return 0.325000000000000;}
  else if (!strcmp(Schriftart,"Gyrose Shift BRK")){return 1.058000000000000;}
  else if (!strcmp(Schriftart,"Gyrose BRK")){return 1.150000000000000;}
  else if (!strcmp(Schriftart,"Gyparody")){return 0.659000000000000;}
  else if (!strcmp(Schriftart,"Gyneric BRK")){return 0.825000000000000;}
  else if (!strcmp(Schriftart,"Gyneric 3D BRK")){return 0.789000000000000;}
  else if (!strcmp(Schriftart,"Gunplay 3D")){return 0.752929687500000;}
  else if (!strcmp(Schriftart,"Gunplay")){return 0.839843750000000;}
  else if (!strcmp(Schriftart,"Gumtuckey")){return 0.608000000000000;}
  else if (!strcmp(Schriftart,"Guatemala")){return 1.208007812500000;}
  else if (!strcmp(Schriftart,"Guanine")){return 1.021000000000000;}
  else if (!strcmp(Schriftart,"Grudge BRK")){return 0.626000000000000;}
  else if (!strcmp(Schriftart,"Grudge 2 BRK")){return 0.516000000000000;}
  else if (!strcmp(Schriftart,"Grotesque BRK")){return 0.899000000000000;}
  else if (!strcmp(Schriftart,"GroovyGhosties")){return 0.868000000000000;}
  else if (!strcmp(Schriftart,"Green Fuz")){return 0.688000000000000;}
  else if (!strcmp(Schriftart,"Great Heights BRK")){return 0.452000000000000;}
  else if (!strcmp(Schriftart,"Graze BRK")){return 0.399000000000000;}
  else if (!strcmp(Schriftart,"Gravitate Segments BRK")){return 1.015000000000000;}
  else if (!strcmp(Schriftart,"Gravitate BRK")){return 1.015000000000000;}
  else if (!strcmp(Schriftart,"Graveyard BRK")){return 0.556000000000000;}
  else if (!strcmp(Schriftart,"Grapple BRK")){return 0.760000000000000;}
  else if (!strcmp(Schriftart,"Graph")){return 0.966111111111111;}
  else if (!strcmp(Schriftart,"Granular BRK")){return 0.587000000000000;}
  else if (!strcmp(Schriftart,"Granada")){return 0.966111111111111;}
  else if (!strcmp(Schriftart,"Graffiti Treat")){return 1.031000000000000;}
  else if (!strcmp(Schriftart,"Got No Heart")){return 1.228000000000000;}
  else if (!strcmp(Schriftart,"Gothic Uralic")){return 1.001000000000000;}
  else if (!strcmp(Schriftart,"Goose Bumps II BRK")){return 0.669000000000000;}
  else if (!strcmp(Schriftart,"Goose Bumps BRK")){return 0.655000000000000;}
  else if (!strcmp(Schriftart,"Good Times")){return 1.224000000000000;}
  else if (!strcmp(Schriftart,"Goodfish")){return 0.905000000000000;}
  else if (!strcmp(Schriftart,"Golden Girdle")){return 2.002000000000000;}
  else if (!strcmp(Schriftart,"GlazKrak")){return 0.932000000000000;}
  else if (!strcmp(Schriftart,"GiantTigers")){return 0.621000000000000;}
  else if (!strcmp(Schriftart,"Ghostmeat")){return 1.226000000000000;}
  else if (!strcmp(Schriftart,"Gesture Thin Slant BRK")){return 1.005000000000000;}
  else if (!strcmp(Schriftart,"Gesture Thin BRK")){return 0.948000000000000;}
  else if (!strcmp(Schriftart,"Gesture Slant BRK")){return 1.030000000000000;}
  else if (!strcmp(Schriftart,"Gesture BRK")){return 0.976000000000000;}
  else if (!strcmp(Schriftart,"German Participants")){return 1.100000000000000;}
  else if (!strcmp(Schriftart,"Georgia")){return 1.197753906250000;}
  else if (!strcmp(Schriftart,"Gentium Book Basic")){return 0.884277343750000;}
  else if (!strcmp(Schriftart,"Gentium Basic")){return 0.872558593750000;}
  else if (!strcmp(Schriftart,"GentiumAlt")){return 0.859863281250000;}
  else if (!strcmp(Schriftart,"Gentium")){return 0.859863281250000;}
  else if (!strcmp(Schriftart,"genotype S BRK")){return 0.737000000000000;}
  else if (!strcmp(Schriftart,"genotype RS BRK")){return 0.737000000000000;}
  else if (!strcmp(Schriftart,"genotype RH BRK")){return 0.737000000000000;}
  else if (!strcmp(Schriftart,"genotype H BRK")){return 0.737000000000000;}
  else if (!strcmp(Schriftart,"Gautami")){return 0.566406250000000;}
  else if (!strcmp(Schriftart,"Gather Gapped BRK")){return 0.636000000000000;}
  else if (!strcmp(Schriftart,"Gather BRK")){return 0.636000000000000;}
  else if (!strcmp(Schriftart,"Gasping BRK")){return 0.462000000000000;}
  else if (!strcmp(Schriftart,"Garuda")){return 1.108000000000000;}
  else if (!strcmp(Schriftart,"Gardiner")){return 0.838378906250000;}
  else if (!strcmp(Schriftart,"Gaposis Solid BRK")){return 0.891000000000000;}
  else if (!strcmp(Schriftart,"Gaposis Outline BRK")){return 0.891000000000000;}
  else if (!strcmp(Schriftart,"Galvanize BRK")){return 0.526000000000000;}
  else if (!strcmp(Schriftart,"Galatia SIL")){return 1.048828125000000;}
  else if (!strcmp(Schriftart,"Galapogos BRK")){return 0.690000000000000;}
  else if (!strcmp(Schriftart,"Furat")){return 0.949444444444444;}
  else if (!strcmp(Schriftart,"Fully Completely BRK")){return 0.936000000000000;}
  else if (!strcmp(Schriftart,"Frozen Dog Treats")){return 1.679000000000000;}
  else if (!strcmp(Schriftart,"Frizzed BRK")){return 1.007000000000000;}
  else if (!strcmp(Schriftart,"French Participants")){return 1.100000000000000;}
  else if (!strcmp(Schriftart,"FreeSerif")){return 1.056000000000000;}
  else if (!strcmp(Schriftart,"FreeSans")){return 1.112000000000000;}
  else if (!strcmp(Schriftart,"FreeMono")){return 0.815000000000000;}
  else if (!strcmp(Schriftart,"FreeFarsi Monospace")){return 0.808593750000000;}
  else if (!strcmp(Schriftart,"FreeFarsi")){return 1.079101562500000;}
  else if (!strcmp(Schriftart,"Freak Turbulence BRK")){return 0.645000000000000;}
  else if (!strcmp(Schriftart,"FrankRuehl")){return 0.957031250000000;}
  else if (!strcmp(Schriftart,"Franklin Gothic Medium")){return 0.942382812500000;}
  else if (!strcmp(Schriftart,"Frak")){return 0.957000000000000;}
  else if (!strcmp(Schriftart,"Fragile Bombers")){return 0.573000000000000;}
  else if (!strcmp(Schriftart,"Forgotten Futurist Shadow")){return 0.805000000000000;}
  else if (!strcmp(Schriftart,"Forgotten Futurist Rotten")){return 0.724000000000000;}
  else if (!strcmp(Schriftart,"Forgotten Futurist College")){return 0.739000000000000;}
  else if (!strcmp(Schriftart,"Forgotten Futurist")){return 0.790000000000000;}
  else if (!strcmp(Schriftart,"Foreshadow OL BRK")){return 0.702000000000000;}
  else if (!strcmp(Schriftart,"Foreshadow BRK")){return 0.717000000000000;}
  else if (!strcmp(Schriftart,"Forcible BRK")){return 0.695000000000000;}
  else if (!strcmp(Schriftart,"Fluoride Beings")){return 1.535000000000000;}
  else if (!strcmp(Schriftart,"Flipside BRK")){return 1.150000000000000;}
  else if (!strcmp(Schriftart,"flatline")){return 0.864257812500000;}
  else if (!strcmp(Schriftart,"Fixed Miriam Transparent")){return 0.752929687500000;}
  else if (!strcmp(Schriftart,"First Blind")){return 2.367000000000000;}
  else if (!strcmp(Schriftart,"Field Day Filter")){return 0.655000000000000;}
  else if (!strcmp(Schriftart,"Fidgety BRK")){return 0.508000000000000;}
  else if (!strcmp(Schriftart,"feta26")){return 0.569000000000000;}
  else if (!strcmp(Schriftart,"femkeklaver")){return 0.818359375000000;}
  else if (!strcmp(Schriftart,"Faux Snow BRK")){return 0.787000000000000;}
  else if (!strcmp(Schriftart,"Fatboy Slim BLTC BRK")){return 0.910000000000000;}
  else if (!strcmp(Schriftart,"Fatboy Slim BLTC 2 BRK")){return 0.910000000000000;}
  else if (!strcmp(Schriftart,"Fascii Twigs BRK")){return 0.622000000000000;}
  else if (!strcmp(Schriftart,"Fascii Smudge BRK")){return 0.615000000000000;}
  else if (!strcmp(Schriftart,"Fascii Scraggly BRK")){return 0.619000000000000;}
  else if (!strcmp(Schriftart,"Fascii Cross BRK")){return 0.634000000000000;}
  else if (!strcmp(Schriftart,"Fascii BRK")){return 0.710000000000000;}
  else if (!strcmp(Schriftart,"False Positive Round BRK")){return 0.617000000000000;}
  else if (!strcmp(Schriftart,"False Positive BRK")){return 0.617000000000000;}
  else if (!strcmp(Schriftart,"Fake Receipt")){return 0.714000000000000;}
  else if (!strcmp(Schriftart,"Failed Attempt")){return 0.555000000000000;}
  else if (!strcmp(Schriftart,"Fadgod")){return 1.840000000000000;}
  else if (!strcmp(Schriftart,"Fabian")){return 0.810000000000000;}
  else if (!strcmp(Schriftart,"Ezra SIL SR")){return 0.940917968750000;}
  else if (!strcmp(Schriftart,"Ezra SIL")){return 0.940917968750000;}
  else if (!strcmp(Schriftart,"Eye Rhyme")){return 0.446000000000000;}
  else if (!strcmp(Schriftart,"Extraction BRK")){return 0.947000000000000;}
  else if (!strcmp(Schriftart,"Exaggerate BRK")){return 0.702000000000000;}
  else if (!strcmp(Schriftart,"Euphorigenic")){return 0.671000000000000;}
  else if (!strcmp(Schriftart,"Euphoric BRK")){return 0.616000000000000;}
  else if (!strcmp(Schriftart,"Euphoric 3D BRK")){return 0.673000000000000;}
  else if (!strcmp(Schriftart,"eufm10")){return 1.078125000000000;}
  else if (!strcmp(Schriftart,"Ethnocentric")){return 1.389000000000000;}
  else if (!strcmp(Schriftart,"Ethiopic Zelan")){return 0.710000000000000;}
  else if (!strcmp(Schriftart,"Ethiopic Yigezu Bisrat Gothic")){return 0.710000000000000;}
  else if (!strcmp(Schriftart,"Ethiopic Yigezu Bisrat Goffer")){return 0.710000000000000;}
  else if (!strcmp(Schriftart,"Ethiopic Yebse")){return 0.697000000000000;}
  else if (!strcmp(Schriftart,"Ethiopic Wookianos")){return 0.710000000000000;}
  else if (!strcmp(Schriftart,"Ethiopic WashRa SemiBold")){return 0.579000000000000;}
  else if (!strcmp(Schriftart,"Ethiopic WashRa Bold")){return 0.697000000000000;}
  else if (!strcmp(Schriftart,"Ethiopic Tint")){return 0.738000000000000;}
  else if (!strcmp(Schriftart,"Ethiopic Hiwua")){return 0.710000000000000;}
  else if (!strcmp(Schriftart,"Ethiopic Fantuwua")){return 0.710000000000000;}
  else if (!strcmp(Schriftart,"Ethiopia Jiret")){return 0.710000000000000;}
  else if (!strcmp(Schriftart,"Estrangelo Edessa")){return 0.700195312500000;}
  else if (!strcmp(Schriftart,"Essays1743")){return 1.068359375000000;}
  else if (!strcmp(Schriftart,"Enthuse Solid BRK")){return 0.595000000000000;}
  else if (!strcmp(Schriftart,"Enthuse BRK")){return 0.595000000000000;}
  else if (!strcmp(Schriftart,"Entangled Plain BRK")){return 0.827000000000000;}
  else if (!strcmp(Schriftart,"Entangled Layer B BRK")){return 0.830000000000000;}
  else if (!strcmp(Schriftart,"Entangled Layer A BRK")){return 0.880000000000000;}
  else if (!strcmp(Schriftart,"Entangled BRK")){return 0.813000000000000;}
  else if (!strcmp(Schriftart,"Ennobled Pet")){return 0.860000000000000;}
  else if (!strcmp(Schriftart,"Engebrechtre Expanded")){return 1.263000000000000;}
  else if (!strcmp(Schriftart,"Engebrechtre")){return 0.954000000000000;}
  else if (!strcmp(Schriftart,"Engadget")){return 0.920000000000000;}
  else if (!strcmp(Schriftart,"Endless Showroom")){return 1.015000000000000;}
  else if (!strcmp(Schriftart,"Encapsulate Plain BRK")){return 0.589000000000000;}
  else if (!strcmp(Schriftart,"Encapsulate BRK")){return 0.558000000000000;}
  else if (!strcmp(Schriftart,"Emmentaler")){return 0.401000000000000;}
  else if (!strcmp(Schriftart,"EMERITA Latina")){return 1.062500000000000;}
  else if (!strcmp(Schriftart,"Embossing Tape 3 BRK")){return 0.986000000000000;}
  else if (!strcmp(Schriftart,"Embossing Tape 2 BRK")){return 0.883000000000000;}
  else if (!strcmp(Schriftart,"Embossing Tape 1 BRK")){return 0.968000000000000;}
  else if (!strcmp(Schriftart,"Embargo")){return 1.679199218750000;}
  else if (!strcmp(Schriftart,"Elsewhere BRK")){return 0.491000000000000;}
  else if (!strcmp(Schriftart,"Elsewhere 2 BRK")){return 0.491000000000000;}
  else if (!strcmp(Schriftart,"Ellhnikh")){return 0.987792968750000;}
  else if (!strcmp(Schriftart,"Elegante")){return 2.433000000000000;}
  else if (!strcmp(Schriftart,"Electron")){return 0.966111111111111;}
  else if (!strcmp(Schriftart,"Electroharmonix")){return 0.655000000000000;}
  else if (!strcmp(Schriftart,"Electorate Boogie")){return 0.561000000000000;}
  else if (!strcmp(Schriftart,"Electorate Boogaloo")){return 0.445000000000000;}
  else if (!strcmp(Schriftart,"Electorate Blue")){return 0.443000000000000;}
  else if (!strcmp(Schriftart,"El Abogado Loco")){return 0.650390625000000;}
  else if (!strcmp(Schriftart,"Ekushey Sumit")){return 0.904000000000000;}
  else if (!strcmp(Schriftart,"Ekushey Sharifa")){return 0.904000000000000;}
  else if (!strcmp(Schriftart,"Ekushey Saraswatii")){return 0.904000000000000;}
  else if (!strcmp(Schriftart,"Ekushey Punarbhaba")){return 0.904000000000000;}
  else if (!strcmp(Schriftart,"Ekushey Puja")){return 0.904000000000000;}
  else if (!strcmp(Schriftart,"Ekushey Mohua")){return 0.904000000000000;}
  else if (!strcmp(Schriftart,"Ekushey Lohit")){return 0.729518855656697;}
  else if (!strcmp(Schriftart,"Ekushey Godhuli")){return 0.904000000000000;}
  else if (!strcmp(Schriftart,"Ekushey Durga")){return 0.904000000000000;}
  else if (!strcmp(Schriftart,"Ekushey Azad")){return 0.904000000000000;}
  else if (!strcmp(Schriftart,"Effloresce Antique")){return 0.946000000000000;}
  else if (!strcmp(Schriftart,"Effloresce")){return 1.014000000000000;}
  else if (!strcmp(Schriftart,"Eeyek Unicode")){return 0.848632812500000;}
  else if (!strcmp(Schriftart,"Edmunds Distressed")){return 0.938000000000000;}
  else if (!strcmp(Schriftart,"Edmunds")){return 0.924000000000000;}
  else if (!strcmp(Schriftart,"Edit Undo Line BRK")){return 0.765000000000000;}
  else if (!strcmp(Schriftart,"Edit Undo Dot BRK")){return 0.765000000000000;}
  else if (!strcmp(Schriftart,"Edit Undo BRK")){return 0.648000000000000;}
  else if (!strcmp(Schriftart,"Edgewater")){return 1.312000000000000;}
  else if (!strcmp(Schriftart,"Eden Mills")){return 0.839000000000000;}
  else if (!strcmp(Schriftart,"Ecolier_lignes_court")){return 0.989000000000000;}
  else if (!strcmp(Schriftart,"Ecolier_court")){return 0.989000000000000;}
  else if (!strcmp(Schriftart,"Ecliptic BRK")){return 0.907000000000000;}
  else if (!strcmp(Schriftart,"Echelon Condensed")){return 0.407000000000000;}
  else if (!strcmp(Schriftart,"EB Garamond Initials Fill2")){return 1.175000000000000;}
  else if (!strcmp(Schriftart,"EB Garamond Initials Fill1")){return 1.005000000000000;}
  else if (!strcmp(Schriftart,"EB Garamond Initials")){return 1.005000000000000;}
  else if (!strcmp(Schriftart,"EB Garamond 12")){return 1.059000000000000;}
  else if (!strcmp(Schriftart,"EB Garamond 08 SC")){return 0.826000000000000;}
  else if (!strcmp(Schriftart,"EB Garamond 08")){return 1.061000000000000;}
  else if (!strcmp(Schriftart,"Earwig Factory")){return 0.872000000000000;}
  else if (!strcmp(Schriftart,"Dyuthi")){return 0.798828125000000;}
  else if (!strcmp(Schriftart,"Dystorque BRK")){return 0.608000000000000;}
  else if (!strcmp(Schriftart,"Dyspepsia")){return 0.445000000000000;}
  else if (!strcmp(Schriftart,"Dyphusion BRK")){return 0.759000000000000;}
  else if (!strcmp(Schriftart,"Dynamic BRK")){return 0.890000000000000;}
  else if (!strcmp(Schriftart,"Dustismo Roman")){return 0.929687500000000;}
  else if (!strcmp(Schriftart,"Dustismo")){return 0.795898437500000;}
  else if (!strcmp(Schriftart,"Duality")){return 0.831000000000000;}
  else if (!strcmp(Schriftart,"Droid Serif")){return 1.152832031250000;}
  else if (!strcmp(Schriftart,"Droid Sans Mono")){return 0.729003906250000;}
  else if (!strcmp(Schriftart,"Droid Sans Fallback")){return 0.980468750000000;}
  else if (!strcmp(Schriftart,"Droid Sans")){return 1.020019531250000;}
  else if (!strcmp(Schriftart,"Droid")){return 0.326000000000000;}
  else if (!strcmp(Schriftart,"Dream Orphans")){return 0.733000000000000;}
  else if (!strcmp(Schriftart,"Draggle over kerned BRK")){return 0.879000000000000;}
  else if (!strcmp(Schriftart,"Draggle BRK")){return 0.879000000000000;}
  else if (!strcmp(Schriftart,"Doulos SIL")){return 0.958496093750000;}
  else if (!strcmp(Schriftart,"Double Bogey BRK")){return 0.963000000000000;}
  else if (!strcmp(Schriftart,"Domestic Manners")){return 0.852050781250000;}
  else if (!strcmp(Schriftart,"DL-Madu. Cactus")){return 1.072265625000000;}
  else if (!strcmp(Schriftart,"DL-Lihini-22")){return 1.110000000000000;}
  else if (!strcmp(Schriftart,"DL-kotu")){return 0.995000000000000;}
  else if (!strcmp(Schriftart,"Dl-ice")){return 0.985000000000000;}
  else if (!strcmp(Schriftart,"Dl-Hansika.")){return 0.993000000000000;}
  else if (!strcmp(Schriftart,"Dl-Dulackshi.")){return 0.739000000000000;}
  else if (!strcmp(Schriftart,"DL-Biso.")){return 1.031250000000000;}
  else if (!strcmp(Schriftart,"DL-Araliya.")){return 0.985000000000000;}
  else if (!strcmp(Schriftart,"DkgHandwriting")){return 1.145507812500000;}
  else if (!strcmp(Schriftart,"Discordance BRK")){return 0.664000000000000;}
  else if (!strcmp(Schriftart,"DirtyBakersDozen")){return 1.128000000000000;}
  else if (!strcmp(Schriftart,"DinaminaUniWeb")){return 1.031738281250000;}
  else if (!strcmp(Schriftart,"Dimnah")){return 0.966111111111111;}
  else if (!strcmp(Schriftart,"Digohweli")){return 1.024902343750000;}
  else if (!strcmp(Schriftart,"Dignity Of Labour")){return 0.641000000000000;}
  else if (!strcmp(Schriftart,"Die Nasty")){return 1.050000000000000;}
  else if (!strcmp(Schriftart,"Detonate BRK")){return 0.652000000000000;}
  else if (!strcmp(Schriftart,"Deportees")){return 0.375000000000000;}
  else if (!strcmp(Schriftart,"Dephunked BRK")){return 1.209000000000000;}
  else if (!strcmp(Schriftart,"Dented BRK")){return 0.580000000000000;}
  else if (!strcmp(Schriftart,"Densmore")){return 0.913000000000000;}
  else if (!strcmp(Schriftart,"Denemo")){return 0.965820312500000;}
  else if (!strcmp(Schriftart,"Dendritic Voltage")){return 1.049000000000000;}
  else if (!strcmp(Schriftart,"Deluxe Ducks")){return 1.338000000000000;}
  else if (!strcmp(Schriftart,"Delta Hey Max Nine")){return 0.807000000000000;}
  else if (!strcmp(Schriftart,"Delphine")){return 0.707000000000000;}
  else if (!strcmp(Schriftart,"DejaVu Serif Condensed")){return 1.056152343750000;}
  else if (!strcmp(Schriftart,"DejaVu Serif")){return 1.173339843750000;}
  else if (!strcmp(Schriftart,"DejaVu Sans Mono")){return 0.778808593750000;}
  else if (!strcmp(Schriftart,"DejaVu Sans Light")){return 1.087402343750000;}
  else if (!strcmp(Schriftart,"DejaVu Sans Condensed")){return 1.088867187500000;}
  else if (!strcmp(Schriftart,"DejaVu Sans")){return 1.209960937500000;}
  else if (!strcmp(Schriftart,"Degrassi")){return 1.573000000000000;}
  else if (!strcmp(Schriftart,"Deftone Stylus")){return 0.816000000000000;}
  else if (!strcmp(Schriftart,"Decrepit BRK")){return 1.211000000000000;}
  else if (!strcmp(Schriftart,"DB Layer 4 BRK")){return 1.031000000000000;}
  else if (!strcmp(Schriftart,"DB Layer 3 BRK")){return 1.009000000000000;}
  else if (!strcmp(Schriftart,"DB Layer 2 BRK")){return 0.986000000000000;}
  else if (!strcmp(Schriftart,"DB Layer 1 BRK")){return 0.963000000000000;}
  else if (!strcmp(Schriftart,"Dazzle Ships")){return 0.940000000000000;}
  else if (!strcmp(Schriftart,"David Transparent")){return 0.957000000000000;}
  else if (!strcmp(Schriftart,"David")){return 1.008789062500000;}
  else if (!strcmp(Schriftart,"Dastardly BRK")){return 0.324000000000000;}
  else if (!strcmp(Schriftart,"Dash Dot BRK")){return 0.984000000000000;}
  else if (!strcmp(Schriftart,"Dark Side BRK")){return 0.557000000000000;}
  else if (!strcmp(Schriftart,"Dai Banna SIL Light")){return 1.013671875000000;}
  else if (!strcmp(Schriftart,"Dai Banna SIL Book")){return 1.013671875000000;}
  else if (!strcmp(Schriftart,"cwTeXYen")){return 0.974000000000000;}
  else if (!strcmp(Schriftart,"cwTeXMing")){return 1.045000000000000;}
  else if (!strcmp(Schriftart,"cwTeXKai")){return 1.045000000000000;}
  else if (!strcmp(Schriftart,"cwTeXHeiBold")){return 0.974000000000000;}
  else if (!strcmp(Schriftart,"cwTeXFangSong")){return 1.045000000000000;}
  else if (!strcmp(Schriftart,"Cuomotype")){return 0.653000000000000;}
  else if (!strcmp(Schriftart,"Crystal Radio Kit")){return 0.850000000000000;}
  else if (!strcmp(Schriftart,"Cretino")){return 1.168000000000000;}
  else if (!strcmp(Schriftart,"Credit Valley")){return 0.862000000000000;}
  else if (!strcmp(Schriftart,"Credit River")){return 0.750000000000000;}
  else if (!strcmp(Schriftart,"Crapola")){return 0.935000000000000;}
  else if (!strcmp(Schriftart,"Cranberry Gin")){return 0.775000000000000;}
  else if (!strcmp(Schriftart,"CrackMan")){return 1.011000000000000;}
  else if (!strcmp(Schriftart,"Crackdown R BRK")){return 1.310000000000000;}
  else if (!strcmp(Schriftart,"Crackdown R2 BRK")){return 1.223000000000000;}
  else if (!strcmp(Schriftart,"Crackdown O2 BRK")){return 1.138000000000000;}
  else if (!strcmp(Schriftart,"Crackdown O1 BRK")){return 1.042000000000000;}
  else if (!strcmp(Schriftart,"Cousine")){return 0.740234375000000;}
  else if (!strcmp(Schriftart,"Courier New")){return 0.815429687500000;}
  else if (!strcmp(Schriftart,"Counterscraps")){return 1.304000000000000;}
  else if (!strcmp(Schriftart,"Cortoba")){return 0.966111111111111;}
  else if (!strcmp(Schriftart,"Corpulent Caps Shadow BRK")){return 0.837000000000000;}
  else if (!strcmp(Schriftart,"Corpulent Caps BRK")){return 0.750000000000000;}
  else if (!strcmp(Schriftart,"Coolvetica")){return 0.930000000000000;}
  else if (!strcmp(Schriftart,"Contour Generator")){return 1.299000000000000;}
  else if (!strcmp(Schriftart,"Constantia")){return 1.016601562500000;}
  else if (!strcmp(Schriftart,"Consolas")){return 0.663574218750000;}
  else if (!strcmp(Schriftart,"Connecticut")){return 1.145019531250000;}
  else if (!strcmp(Schriftart,"Conduit BRK")){return 0.785000000000000;}
  else if (!strcmp(Schriftart,"Conduit 2 Italics BRK")){return 0.854000000000000;}
  else if (!strcmp(Schriftart,"Conduit 2 BRK")){return 0.726000000000000;}
  else if (!strcmp(Schriftart,"Conakry")){return 0.959472656250000;}
  else if (!strcmp(Schriftart,"Compliant Confuse 3s BRK")){return 0.539000000000000;}
  else if (!strcmp(Schriftart,"Compliant Confuse 3o BRK")){return 0.570000000000000;}
  else if (!strcmp(Schriftart,"Compliant Confuse 2s BRK")){return 0.539000000000000;}
  else if (!strcmp(Schriftart,"Compliant Confuse 2o BRK")){return 0.570000000000000;}
  else if (!strcmp(Schriftart,"Compliant Confuse 1s BRK")){return 0.536000000000000;}
  else if (!strcmp(Schriftart,"Compliant Confuse 1o BRK")){return 0.568000000000000;}
  else if (!strcmp(Schriftart,"Commerciality")){return 1.004000000000000;}
  else if (!strcmp(Schriftart,"Comic Sans MS")){return 1.107421875000000;}
  else if (!strcmp(Schriftart,"Comfortaa")){return 0.977539062500000;}
  else if (!strcmp(Schriftart,"Combustion Wide BRK")){return 0.582000000000000;}
  else if (!strcmp(Schriftart,"Combustion Tall BRK")){return 0.535000000000000;}
  else if (!strcmp(Schriftart,"Combustion Plain BRK")){return 0.554000000000000;}
  else if (!strcmp(Schriftart,"Combustion II BRK")){return 0.595000000000000;}
  else if (!strcmp(Schriftart,"Combustion I BRK")){return 0.649000000000000;}
  else if (!strcmp(Schriftart,"Colourbars")){return 0.840000000000000;}
  else if (!strcmp(Schriftart,"Collective S BRK")){return 0.774000000000000;}
  else if (!strcmp(Schriftart,"Collective RS BRK")){return 0.774000000000000;}
  else if (!strcmp(Schriftart,"Collective RO BRK")){return 0.774000000000000;}
  else if (!strcmp(Schriftart,"Collective O BRK")){return 0.774000000000000;}
  else if (!strcmp(Schriftart,"Cola Participants")){return 1.100000000000000;}
  else if (!strcmp(Schriftart,"Code Of Life BRK")){return 0.092000000000000;}
  else if (!strcmp(Schriftart,"CMU Typewriter Text Variable Width")){return 1.344000000000000;}
  else if (!strcmp(Schriftart,"CMU Typewriter Text")){return 0.726000000000000;}
  else if (!strcmp(Schriftart,"CMU Serif Upright Italic")){return 1.129000000000000;}
  else if (!strcmp(Schriftart,"CMU Serif Extra")){return 1.319000000000000;}
  else if (!strcmp(Schriftart,"CMU Serif")){return 1.377000000000000;}
  else if (!strcmp(Schriftart,"CMU Sans Serif Demi Condensed")){return 0.920000000000000;}
  else if (!strcmp(Schriftart,"CMU Sans Serif")){return 1.204000000000000;}
  else if (!strcmp(Schriftart,"CMU Concrete")){return 1.442000000000000;}
  else if (!strcmp(Schriftart,"CMU Classical Serif")){return 1.208000000000000;}
  else if (!strcmp(Schriftart,"CMU Bright")){return 1.201000000000000;}
  else if (!strcmp(Schriftart,"cmsy10")){return 1.227050781250000;}
  else if (!strcmp(Schriftart,"cmr10")){return 1.044921875000000;}
  else if (!strcmp(Schriftart,"cmmi10")){return 1.011230468750000;}
  else if (!strcmp(Schriftart,"Cleveland Condensed")){return 0.763183593750000;}
  else if (!strcmp(Schriftart,"Cleaved TTR BRK")){return 0.503000000000000;}
  else if (!strcmp(Schriftart,"CLAW 2 BRK")){return 0.586000000000000;}
  else if (!strcmp(Schriftart,"CLAW 1 BRK")){return 0.839000000000000;}
  else if (!strcmp(Schriftart,"Classic Trash 2 BRK")){return 1.016000000000000;}
  else if (!strcmp(Schriftart,"Classic Trash 1 BRK")){return 1.019000000000000;}
  else if (!strcmp(Schriftart,"Circulate BRK")){return 0.869000000000000;}
  else if (!strcmp(Schriftart,"Chumbly BRK")){return 0.801000000000000;}
  else if (!strcmp(Schriftart,"Christian Participants")){return 1.100000000000000;}
  else if (!strcmp(Schriftart,"CHR32")){return 0.925000000000000;}
  else if (!strcmp(Schriftart,"Chintzy CPU Shadow BRK")){return 0.782000000000000;}
  else if (!strcmp(Schriftart,"Chintzy CPU BRK")){return 0.709000000000000;}
  else if (!strcmp(Schriftart,"Chinese Rocks")){return 0.547000000000000;}
  else if (!strcmp(Schriftart,"Chicken Wire Lady")){return 0.455000000000000;}
  else if (!strcmp(Schriftart,"Chemical Reaction B BRK")){return 0.712000000000000;}
  else if (!strcmp(Schriftart,"Chemical Reaction A BRK")){return 0.712000000000000;}
  else if (!strcmp(Schriftart,"Charles in Charge")){return 1.225000000000000;}
  else if (!strcmp(Schriftart,"Charis SIL")){return 1.052246093750000;}
  else if (!strcmp(Schriftart,"Chandas")){return 1.121093750000000;}
  else if (!strcmp(Schriftart,"Chancery Uralic")){return 0.984000000000000;}
  else if (!strcmp(Schriftart,"Century Catalogue")){return 1.044000000000000;}
  else if (!strcmp(Schriftart,"Caslon")){return 1.063000000000000;}
  else if (!strcmp(Schriftart,"Carlito")){return 0.963378906250000;}
  else if (!strcmp(Schriftart,"Carbon Phyber")){return 0.714000000000000;}
  else if (!strcmp(Schriftart,"Carbon Block")){return 0.677000000000000;}
  else if (!strcmp(Schriftart,"Capacitor")){return 1.325000000000000;}
  else if (!strcmp(Schriftart,"Candy Stripe BRK")){return 0.739000000000000;}
  else if (!strcmp(Schriftart,"Candara")){return 0.923828125000000;}
  else if (!strcmp(Schriftart,"Canadian Participants")){return 1.100000000000000;}
  else if (!strcmp(Schriftart,"Calibri")){return 0.938964843750000;}
  else if (!strcmp(Schriftart,"Caliban")){return 1.099000000000000;}
  else if (!strcmp(Schriftart,"Caladea")){return 0.977000000000000;}
  else if (!strcmp(Schriftart,"Cabin Sketch")){return 0.940000000000000;}
  else if (!strcmp(Schriftart,"CabinSketch")){return 0.940000000000000;}
  else if (!strcmp(Schriftart,"Butterbelly")){return 0.804000000000000;}
  else if (!strcmp(Schriftart,"Burnstown Dam")){return 1.179000000000000;}
  else if (!strcmp(Schriftart,"Bumped BRK")){return 0.771000000000000;}
  else if (!strcmp(Schriftart,"Bullpen 3D")){return 1.238000000000000;}
  else if (!strcmp(Schriftart,"Bullpen")){return 1.123000000000000;}
  else if (!strcmp(Schriftart,"BugisA")){return 1.350000000000000;}
  else if (!strcmp(Schriftart,"Budmo Jiggler")){return 1.097000000000000;}
  else if (!strcmp(Schriftart,"British Participants")){return 1.100000000000000;}
  else if (!strcmp(Schriftart,"Brigadoom Wide BRK")){return 0.870000000000000;}
  else if (!strcmp(Schriftart,"Brigadoom BRK")){return 0.465000000000000;}
  else if (!strcmp(Schriftart,"Breip")){return 0.931640625000000;}
  else if (!strcmp(Schriftart,"Brass Knuckle Star BRK")){return 0.969000000000000;}
  else if (!strcmp(Schriftart,"Brass Knuckle SS BRK")){return 0.969000000000000;}
  else if (!strcmp(Schriftart,"Brass Knuckle BRK")){return 0.947000000000000;}
  else if (!strcmp(Schriftart,"Bramalea Beauty")){return 0.481000000000000;}
  else if (!strcmp(Schriftart,"Braeside Lumberboy")){return 0.847000000000000;}
  else if (!strcmp(Schriftart,"BPG Serif Modern GPL&amp;GNU")){return 1.073730468750000;}
  else if (!strcmp(Schriftart,"BPG Serif GPL&amp;GNU")){return 1.091796875000000;}
  else if (!strcmp(Schriftart,"BPG Sans Regular GPL&amp;GNU")){return 1.033691406250000;}
  else if (!strcmp(Schriftart,"BPG Sans Modern GPL&amp;GNU")){return 1.064941406250000;}
  else if (!strcmp(Schriftart,"BPG Sans Medium GPL&amp;GNU")){return 1.033691406250000;}
  else if (!strcmp(Schriftart,"BPG Sans GPL&amp;GNU")){return 1.147949218750000;}
  else if (!strcmp(Schriftart,"BPG Nino Medium GPL&amp;GNU")){return 0.713000000000000;}
  else if (!strcmp(Schriftart,"BPG Nino Medium Cond GPL&amp;GNU")){return 0.713000000000000;}
  else if (!strcmp(Schriftart,"BPG Nateli GPL&amp;GNU")){return 0.966000000000000;}
  else if (!strcmp(Schriftart,"BPG Nateli Condenced GPL&amp;GNU")){return 0.966000000000000;}
  else if (!strcmp(Schriftart,"BPG Nateli Caps GPL&amp;GNU")){return 0.966000000000000;}
  else if (!strcmp(Schriftart,"BPG Mrgvlovani GPL&amp;GNU")){return 1.064941406250000;}
  else if (!strcmp(Schriftart,"BPG Mrgvlovani Caps GNU&amp;GPL")){return 1.064941406250000;}
  else if (!strcmp(Schriftart,"BPG Ingiri GPL&amp;GNU")){return 0.956054687500000;}
  else if (!strcmp(Schriftart,"BPG Gorda GPL&amp;GNU")){return 0.916000000000000;}
  else if (!strcmp(Schriftart,"BPG Glaho GPL&amp;GNU")){return 0.956054687500000;}
  else if (!strcmp(Schriftart,"BPG Excelsior GPL&amp;GNU")){return 1.064941406250000;}
  else if (!strcmp(Schriftart,"BPG Excelsior Condencerd GPL&amp;GNU")){return 1.064941406250000;}
  else if (!strcmp(Schriftart,"BPG Excelsior Caps GPL&amp;GNU")){return 1.064941406250000;}
  else if (!strcmp(Schriftart,"BPG Elite GPL&amp;GNU")){return 1.073730468750000;}
  else if (!strcmp(Schriftart,"BPG DejaVu Sans 2011 GNU-GPL")){return 1.064941406250000;}
  else if (!strcmp(Schriftart,"BPG DedaEna Block GPL&amp;GNU")){return 0.896245059288537;}
  else if (!strcmp(Schriftart,"BPG Courier S GPL&amp;GNU")){return 0.756835937500000;}
  else if (!strcmp(Schriftart,"BPG Courier GPL&amp;GNU")){return 0.734375000000000;}
  else if (!strcmp(Schriftart,"BPG Chveulebrivi GPL&amp;GNU")){return 0.936000000000000;}
  else if (!strcmp(Schriftart,"BPG Algeti GPL&amp;GNU")){return 0.647949218750000;}
  else if (!strcmp(Schriftart,"Boron")){return 0.563000000000000;}
  else if (!strcmp(Schriftart,"Borg9")){return 1.104000000000000;}
  else if (!strcmp(Schriftart,"Bookman Uralic")){return 1.047000000000000;}
  else if (!strcmp(Schriftart,"Bocuma Dent BRK")){return 0.492000000000000;}
  else if (!strcmp(Schriftart,"Bocuma BRK")){return 0.466000000000000;}
  else if (!strcmp(Schriftart,"Bocuma Batty BRK")){return 0.422000000000000;}
  else if (!strcmp(Schriftart,"Bocuma Angle Dent BRK")){return 0.482000000000000;}
  else if (!strcmp(Schriftart,"Bocuma Angle BRK")){return 0.454000000000000;}
  else if (!strcmp(Schriftart,"Bobcaygeon Plain BRK")){return 0.527000000000000;}
  else if (!strcmp(Schriftart,"Bobcaygeon BRK")){return 1.134000000000000;}
  else if (!strcmp(Schriftart,"Blue Highway Linocut")){return 0.766000000000000;}
  else if (!strcmp(Schriftart,"Blue Highway D Type")){return 0.778000000000000;}
  else if (!strcmp(Schriftart,"Blue Highway Condensed")){return 0.464000000000000;}
  else if (!strcmp(Schriftart,"Blue Highway")){return 0.770000000000000;}
  else if (!strcmp(Schriftart,"Blox BRK")){return 0.509000000000000;}
  else if (!strcmp(Schriftart,"Block Tilt BRK")){return 0.584000000000000;}
  else if (!strcmp(Schriftart,"Bleak Segments BRK")){return 0.753000000000000;}
  else if (!strcmp(Schriftart,"Blackoninaut Redux BRK")){return 0.558000000000000;}
  else if (!strcmp(Schriftart,"Blackoninaut BRK")){return 0.714000000000000;}
  else if (!strcmp(Schriftart,"Blackoninaut Bold BRK")){return 0.737000000000000;}
  else if (!strcmp(Schriftart,"Bitstream Vera Serif")){return 1.148925781250000;}
  else if (!strcmp(Schriftart,"Bitstream Vera Sans Mono")){return 0.778808593750000;}
  else if (!strcmp(Schriftart,"Bitstream Vera Sans")){return 1.209960937500000;}
  else if (!strcmp(Schriftart,"Biting My Nails Outline")){return 0.937000000000000;}
  else if (!strcmp(Schriftart,"Biting My Nails")){return 0.952000000000000;}
  else if (!strcmp(Schriftart,"Bit Blocks TTF BRK")){return 1.225000000000000;}
  else if (!strcmp(Schriftart,"Birdland Aeroplane")){return 0.668000000000000;}
  else if (!strcmp(Schriftart,"Biometric Joe")){return 0.746000000000000;}
  else if (!strcmp(Schriftart,"Biohazard Participants")){return 1.100000000000000;}
  else if (!strcmp(Schriftart,"Binary X CHR BRK")){return 0.750000000000000;}
  else if (!strcmp(Schriftart,"Binary X BRK")){return 0.640000000000000;}
  else if (!strcmp(Schriftart,"Binary X 01s BRK")){return 0.800000000000000;}
  else if (!strcmp(Schriftart,"Binary CHR BRK")){return 0.750000000000000;}
  else if (!strcmp(Schriftart,"Binary BRK")){return 0.640000000000000;}
  else if (!strcmp(Schriftart,"Binary 01s BRK")){return 0.800000000000000;}
  else if (!strcmp(Schriftart,"Bewilder Thick BRK")){return 0.599000000000000;}
  else if (!strcmp(Schriftart,"Bewilder BRK")){return 0.519000000000000;}
  else if (!strcmp(Schriftart,"Betsy Flanagan")){return 0.691000000000000;}
  else if (!strcmp(Schriftart,"BetecknaLowerCase")){return 0.961000000000000;}
  else if (!strcmp(Schriftart,"Berylium")){return 1.018000000000000;}
  else if (!strcmp(Schriftart,"Berenika")){return 1.443847656250000;}
  else if (!strcmp(Schriftart,"Bendable BRK")){return 0.824000000000000;}
  else if (!strcmp(Schriftart,"Bend 2 Squares OL2 BRK")){return 0.533000000000000;}
  else if (!strcmp(Schriftart,"Bend 2 Squares OL1 BRK")){return 0.533000000000000;}
  else if (!strcmp(Schriftart,"Bend 2 Squares BRK")){return 0.505000000000000;}
  else if (!strcmp(Schriftart,"Beat My Guest")){return 0.744000000000000;}
  else if (!strcmp(Schriftart,"Baveuse 3D")){return 1.241000000000000;}
  else if (!strcmp(Schriftart,"Baveuse")){return 1.220000000000000;}
  else if (!strcmp(Schriftart,"Barbatrick")){return 0.651000000000000;}
  else if (!strcmp(Schriftart,"Bandwidth BRK")){return 0.278000000000000;}
  else if (!strcmp(Schriftart,"Bandwidth Bandmess BRK")){return 0.558000000000000;}
  else if (!strcmp(Schriftart,"Bandwidth Bandless BRK")){return 0.480000000000000;}
  else if (!strcmp(Schriftart,"Baltar")){return 0.769000000000000;}
  else if (!strcmp(Schriftart,"Balker")){return 0.726074218750000;}
  else if (!strcmp(Schriftart,"Balcony Angels")){return 0.773000000000000;}
  else if (!strcmp(Schriftart,"BaileysCar")){return 0.661000000000000;}
  else if (!strcmp(Schriftart,"Backlash BRK")){return 0.585000000000000;}
  else if (!strcmp(Schriftart,"Baby Jeepers")){return 0.850000000000000;}
  else if (!strcmp(Schriftart,"BABEL Unicode")){return 1.217285156250000;}
  else if (!strcmp(Schriftart,"Axaxax")){return 0.795898437500000;}
  else if (!strcmp(Schriftart,"Averia Serif GWF")){return 1.127441406250000;}
  else if (!strcmp(Schriftart,"Averia Sans GWF")){return 1.027832031250000;}
  else if (!strcmp(Schriftart,"Averia GWF")){return 1.083496093750000;}
  else if (!strcmp(Schriftart,"Avdira")){return 1.233398437500000;}
  else if (!strcmp(Schriftart,"Automatica BRK")){return 1.199000000000000;}
  else if (!strcmp(Schriftart,"Ataxia Outline BRK")){return 0.520000000000000;}
  else if (!strcmp(Schriftart,"Ataxia BRK")){return 0.513000000000000;}
  else if (!strcmp(Schriftart,"Atavyros")){return 1.012695312500000;}
  else if (!strcmp(Schriftart,"AtariSmallLight")){return 0.770000000000000;}
  else if (!strcmp(Schriftart,"AtariSmallItalic")){return 0.683000000000000;}
  else if (!strcmp(Schriftart,"AtariSmallCondensed")){return 0.600000000000000;}
  else if (!strcmp(Schriftart,"AtariSmallBold")){return 0.730000000000000;}
  else if (!strcmp(Schriftart,"Atari")){return 0.750000000000000;}
  else if (!strcmp(Schriftart,"Astron Boy Wonder")){return 1.060000000000000;}
  else if (!strcmp(Schriftart,"Astron Boy Video")){return 0.993000000000000;}
  else if (!strcmp(Schriftart,"Astron Boy")){return 0.993000000000000;}
  else if (!strcmp(Schriftart,"Aspartame BRK")){return 1.074000000000000;}
  else if (!strcmp(Schriftart,"Arundina Serif")){return 1.146484375000000;}
  else if (!strcmp(Schriftart,"Arundina Sans Mono")){return 0.779785156250000;}
  else if (!strcmp(Schriftart,"Arundina Sans")){return 1.178222656250000;}
  else if (!strcmp(Schriftart,"Arthritis BRK")){return 0.450000000000000;}
  else if (!strcmp(Schriftart,"AR PL SungtiL GB")){return 0.606445312500000;}
  else if (!strcmp(Schriftart,"AR PL Mingti2L Big5")){return 0.606445312500000;}
  else if (!strcmp(Schriftart,"AR PL KaitiM GB")){return 0.606445312500000;}
  else if (!strcmp(Schriftart,"AR PL KaitiM Big5")){return 0.606445312500000;}
  else if (!strcmp(Schriftart,"Aroania")){return 1.006347656250000;}
  else if (!strcmp(Schriftart,"Arnprior")){return 1.250000000000000;}
  else if (!strcmp(Schriftart,"Arimo")){return 1.030273437500000;}
  else if (!strcmp(Schriftart,"Arial Black")){return 1.061523437500000;}
  else if (!strcmp(Schriftart,"Arial")){return 1.068847656250000;}
  else if (!strcmp(Schriftart,"Arabic Transparent")){return 0.860351562500000;}
  else if (!strcmp(Schriftart,"Arab")){return 0.966111111111111;}
  else if (!strcmp(Schriftart,"AponaLohit")){return 0.729518855656697;}
  else if (!strcmp(Schriftart,"API PHON&#201;TIQUE")){return 1.244140625000000;}
  else if (!strcmp(Schriftart,"Antique Oakland")){return 1.122070312500000;}
  else if (!strcmp(Schriftart,"Anonymous Pro")){return 0.636718750000000;}
  else if (!strcmp(Schriftart,"Anklepants")){return 1.217000000000000;}
  else if (!strcmp(Schriftart,"AnjaliOldLipi")){return 0.970703125000000;}
  else if (!strcmp(Schriftart,"Angostura Black")){return 0.585000000000000;}
  else if (!strcmp(Schriftart,"Angostura")){return 0.760000000000000;}
  else if (!strcmp(Schriftart,"Anglepoise Lampshade")){return 0.621000000000000;}
  else if (!strcmp(Schriftart,"Andalus")){return 0.996093750000000;}
  else if (!strcmp(Schriftart,"Andale Mono")){return 0.759277343750000;}
  else if (!strcmp(Schriftart,"Anatolian")){return 1.054687500000000;}
  else if (!strcmp(Schriftart,"Analecta")){return 1.033691406250000;}
  else if (!strcmp(Schriftart,"Anaktoria")){return 0.962890625000000;}
  else if (!strcmp(Schriftart,"Amplitude BRK")){return 0.810000000000000;}
  else if (!strcmp(Schriftart,"American Participants")){return 1.100000000000000;}
  else if (!strcmp(Schriftart,"Amalgamate O BRK")){return 0.663000000000000;}
  else if (!strcmp(Schriftart,"Amalgamate BRK")){return 0.650000000000000;}
  else if (!strcmp(Schriftart,"AlYarmook")){return 0.966111111111111;}
  else if (!strcmp(Schriftart,"Alpha Beta BRK")){return 0.825000000000000;}
  else if (!strcmp(Schriftart,"Almonte Woodgrain")){return 0.630000000000000;}
  else if (!strcmp(Schriftart,"Almonte Snow")){return 0.563000000000000;}
  else if (!strcmp(Schriftart,"Almonte")){return 0.625000000000000;}
  else if (!strcmp(Schriftart,"AlManzomah")){return 0.966111111111111;}
  else if (!strcmp(Schriftart,"Alianna")){return 0.591000000000000;}
  else if (!strcmp(Schriftart,"AlHor")){return 0.966111111111111;}
  else if (!strcmp(Schriftart,"Alfios")){return 1.026000000000000;}
  else if (!strcmp(Schriftart,"Alfa-beta")){return 0.903076923076923;}
  else if (!strcmp(Schriftart,"Alexander")){return 1.030273437500000;}
  else if (!strcmp(Schriftart,"AlBattar")){return 0.966111111111111;}
  else if (!strcmp(Schriftart,"Alaska Extrabold")){return 0.998046875000000;}
  else if (!strcmp(Schriftart,"Alaska")){return 0.967773437500000;}
  else if (!strcmp(Schriftart,"AlArabiya")){return 0.966111111111111;}
  else if (!strcmp(Schriftart,"Akkadian")){return 1.054687500000000;}
  else if (!strcmp(Schriftart,"Airmole Stripe")){return 0.719000000000000;}
  else if (!strcmp(Schriftart,"Airmole Shaded")){return 0.840000000000000;}
  else if (!strcmp(Schriftart,"Airmole Antique")){return 0.720000000000000;}
  else if (!strcmp(Schriftart,"Airmole")){return 0.720000000000000;}
  else if (!strcmp(Schriftart,"AirCut")){return 0.595000000000000;}
  else if (!strcmp(Schriftart,"Aharoni")){return 1.013671875000000;}
  else if (!strcmp(Schriftart,"Aftermath BRK")){return 0.802000000000000;}
  else if (!strcmp(Schriftart,"Aegyptus")){return 0.838378906250000;}
  else if (!strcmp(Schriftart,"Aegean")){return 1.054687500000000;}
  else if (!strcmp(Schriftart,"Adriator")){return 0.696000000000000;}
  else if (!strcmp(Schriftart,"AdorshoLipi")){return 0.427734375000000;}
  else if (!strcmp(Schriftart,"Acknowledge TT BRK")){return 0.675000000000000;}
  else if (!strcmp(Schriftart,"Acid Reflux BRK")){return 0.445000000000000;}
  else if (!strcmp(Schriftart,"Abyssinica SIL")){return 0.940917968750000;}
  else if (!strcmp(Schriftart,"Aboriginal Serif")){return 0.980957031250000;}
  else if (!strcmp(Schriftart,"Aboriginal Sans")){return 1.098000000000000;}
  else if (!strcmp(Schriftart,"Abecedario_punt-pautada")){return 0.797000000000000;}
  else if (!strcmp(Schriftart,"Abecedario_punt-guiada")){return 0.797000000000000;}
  else if (!strcmp(Schriftart,"Abecedario_punteada")){return 0.808000000000000;}
  else if (!strcmp(Schriftart,"Abecedario_pautada")){return 0.797000000000000;}
  else if (!strcmp(Schriftart,"Abecedario_guiada")){return 0.797000000000000;}
  else if (!strcmp(Schriftart,"Abecedario")){return 0.849000000000000;}
  else if (!strcmp(Schriftart,"ABC-TEXT-05A")){return 0.883000000000000;}
  else if (!strcmp(Schriftart,"Abberancy")){return 0.988000000000000;}
  else if (!strcmp(Schriftart,"Abandoned Bitplane")){return 0.791000000000000;}
  else if (!strcmp(Schriftart,"aakar")){return 0.926757812500000;}
  else if (!strcmp(Schriftart,"90 Stars BRK")){return 1.112000000000000;}
  else if (!strcmp(Schriftart,"8-bit Limit RO BRK")){return 0.608000000000000;}
  else if (!strcmp(Schriftart,"8-bit Limit R BRK")){return 0.571000000000000;}
  else if (!strcmp(Schriftart,"8-bit Limit O BRK")){return 0.608000000000000;}
  else if (!strcmp(Schriftart,"8-bit Limit BRK")){return 0.568000000000000;}
  else if (!strcmp(Schriftart,"&#45208;&#45588;&#49552;&#44544;&#50472; &#54172;")){return 0.679000000000000;}
  else if (!strcmp(Schriftart,"&#45208;&#45588;&#49552;&#44544;&#50472; &#48531;")){return 0.706000000000000;}
  else if (!strcmp(Schriftart,"&#45208;&#45588;&#47749;&#51312; ExtraBold")){return 1.033203125000000;}
  else if (!strcmp(Schriftart,"&#45208;&#45588;&#47749;&#51312; &#50640;&#53076; ExtraBold")){return 1.033203125000000;}
  else if (!strcmp(Schriftart,"&#45208;&#45588;&#47749;&#51312; &#50640;&#53076;")){return 1.047851562500000;}
  else if (!strcmp(Schriftart,"&#45208;&#45588;&#47749;&#51312;")){return 1.047851562500000;}
  else if (!strcmp(Schriftart,"&#45208;&#45588;&#44256;&#46357; Light")){return 1.079000000000000;}
  else if (!strcmp(Schriftart,"&#45208;&#45588;&#44256;&#46357; ExtraBold")){return 1.051000000000000;}
  else if (!strcmp(Schriftart,"&#45208;&#45588;&#44256;&#46357;&#53076;&#46377;")){return 0.601000000000000;}
  else if (!strcmp(Schriftart,"&#45208;&#45588;&#44256;&#46357; &#50640;&#53076; ExtraBold")){return 1.051000000000000;}
  else if (!strcmp(Schriftart,"&#45208;&#45588;&#44256;&#46357; &#50640;&#53076;")){return 1.067000000000000;}
  else if (!strcmp(Schriftart,"&#45208;&#45588;&#44256;&#46357;")){return 1.067000000000000;}
  else if (!strcmp(Schriftart,"&#40657;&#20307;")){return 0.710937500000000;}
  else if (!strcmp(Schriftart,"3D LET BRK")){return 0.691000000000000;}
  else if (!strcmp(Schriftart,"&#38738;&#26611;&#34913;&#23665;&#12501;&#12457;&#12531;&#12488;T")){return 0.693359375000000;}
  else if (!strcmp(Schriftart,"&#38738;&#26611;&#30094;&#30707;&#12501;&#12457;&#12531;&#12488;2")){return 0.685546875000000;}
  else if (!strcmp(Schriftart,"36 days ago Thick BRK")){return 0.725000000000000;}
  else if (!strcmp(Schriftart,"36 days ago BRK")){return 0.651000000000000;}
  else if (!strcmp(Schriftart,"&#34913;&#23665;&#27611;&#31558;&#12501;&#12457;&#12531;&#12488;&#34892;&#26360;")){return 0.693359375000000;}
  else if (!strcmp(Schriftart,"&#34913;&#23665;&#27611;&#31558;&#12501;&#12457;&#12531;&#12488;&#33609;&#26360;")){return 0.693359375000000;}
  else if (!strcmp(Schriftart,"&#34913;&#23665;&#27611;&#31558;&#12501;&#12457;&#12531;&#12488;")){return 0.693359375000000;}
  else if (!strcmp(Schriftart,"&#33457;&#22290;&#26126;&#26397;B")){return 0.632812500000000;}
  else if (!strcmp(Schriftart,"&#33457;&#22290;&#26126;&#26397;A")){return 0.632812500000000;}
  else if (!strcmp(Schriftart,"&#32654;&#21682;&#26126;&#26397;")){return 0.500000000000000;}
  else if (!strcmp(Schriftart,"&#32654;&#21682;&#12468;&#12471;&#12483;&#12463;")){return 0.500000000000000;}
  else if (!strcmp(Schriftart,"&#26757;UI&#12468;&#12471;&#12483;&#12463;O5")){return 0.816406250000000;}
  else if (!strcmp(Schriftart,"&#26757;UI&#12468;&#12471;&#12483;&#12463;")){return 0.820312500000000;}
  else if (!strcmp(Schriftart,"&#26757;P&#26126;&#26397;S3")){return 0.871093750000000;}
  else if (!strcmp(Schriftart,"&#26757;P&#26126;&#26397;")){return 0.871093750000000;}
  else if (!strcmp(Schriftart,"&#26757;P&#12468;&#12471;&#12483;&#12463;S5")){return 0.816406250000000;}
  else if (!strcmp(Schriftart,"&#26757;P&#12468;&#12471;&#12483;&#12463;S4")){return 0.820312500000000;}
  else if (!strcmp(Schriftart,"&#26757;P&#12468;&#12471;&#12483;&#12463;O5")){return 0.816406250000000;}
  else if (!strcmp(Schriftart,"&#26757;P&#12468;&#12471;&#12483;&#12463;C5")){return 0.816406250000000;}
  else if (!strcmp(Schriftart,"&#26757;P&#12468;&#12471;&#12483;&#12463;C4")){return 0.820312500000000;}
  else if (!strcmp(Schriftart,"&#26757;P&#12468;&#12471;&#12483;&#12463;")){return 0.820312500000000;}
  else if (!strcmp(Schriftart,"&#26757;&#26126;&#26397;S3")){return 0.660156250000000;}
  else if (!strcmp(Schriftart,"&#26757;&#26126;&#26397;")){return 0.660156250000000;}
  else if (!strcmp(Schriftart,"&#26757;&#12468;&#12471;&#12483;&#12463;S5")){return 0.710937500000000;}
  else if (!strcmp(Schriftart,"&#26757;&#12468;&#12471;&#12483;&#12463;S4")){return 0.714843750000000;}
  else if (!strcmp(Schriftart,"&#26757;&#12468;&#12471;&#12483;&#12463;O5")){return 0.710937500000000;}
  else if (!strcmp(Schriftart,"&#26757;&#12468;&#12471;&#12483;&#12463;C5")){return 0.710937500000000;}
  else if (!strcmp(Schriftart,"&#26757;&#12468;&#12471;&#12483;&#12463;C4")){return 0.714843750000000;}
  else if (!strcmp(Schriftart,"&#26757;&#12468;&#12471;&#12483;&#12463;")){return 0.714843750000000;}
  else if (!strcmp(Schriftart,"&#2478;&#2497;&#2453;&#2509;&#2468;&#2495; &#2474;&#2494;&#2468;&#2472;&#2494;")){return 0.956542968750000;}
  else if (!strcmp(Schriftart,"&#23567;&#22799; &#31561;&#24133;")){return 0.715820312500000;}
  else if (!strcmp(Schriftart,"&#23567;&#22799;")){return 0.833007812500000;}
  else if (!strcmp(Schriftart,"&#2344;&#2366;&#2354;&#2367;&#2350;&#2366;&#2335;&#2368;")){return 0.935058593750000;}
  else if (!strcmp(Schriftart,"&#2327;&#2366;&#2352;&#2381;&#2327;&#2368;")){return 0.979492187500000;}
  else if (!strcmp(Schriftart,"&#225;&#128;&#149;&#225;&#128;&#173;&#225;&#128;&#144;&#225;&#128;&#177;&#225;&#128;&#172;&#225;&#128;&#128;&#225;&#128;&#186;&#225;&#128;&#133;&#225;&#128;&#172;&#225;&#128;&#161;&#225;&#128;&#175;&#225;&#128;&#149;&#225;&#128;&#186;")){return 0.878906250000000;}
  else if (!strcmp(Schriftart,"&#225;&#128;&#149;&#225;&#128;&#173;&#225;&#128;&#144;&#225;&#128;&#177;&#225;&#128;&#172;&#225;&#128;&#128;&#225;&#128;&#186;")){return 0.871093750000000;}
  else if (!strcmp(Schriftart,"&#224;&#166;&#8230;&#224;&#166;&#168;&#224;&#166;&#191; Dvf")){return 1.028808593750000;}
  else if (!strcmp(Schriftart,"&#224;&#166;&#174;&#224;&#167;&#129;&#224;&#166;&#8226;&#224;&#167;&#141;&#224;&#166;&#164;&#224;&#166;&#191;")){return 0.965820312500000;}
  else if (!strcmp(Schriftart,"&#198; Systematic TT BRK")){return 0.675000000000000;}
  else if (!strcmp(Schriftart,"&#198;nigma Scrawl 4 BRK")){return 0.797000000000000;}
  else if (!strcmp(Schriftart,"18 Holes BRK")){return 1.253000000000000;}
  else if (!strcmp(Schriftart,"&#12514;&#12488;&#12516;L&#12510;&#12523;&#12505;&#12522;3&#31561;&#24133;")){return 0.704101562500000;}
  else if (!strcmp(Schriftart,"&#12514;&#12488;&#12516;L&#12471;&#12540;&#12479;&#12443;3&#31561;&#24133;")){return 0.707031250000000;}
  else if (!strcmp(Schriftart,"&#12415;&#12363;&#12385;&#12419;&#12435;-PS")){return 0.612304687500000;}
  else if (!strcmp(Schriftart,"&#12415;&#12363;&#12385;&#12419;&#12435;-PB")){return 0.541992187500000;}
  else if (!strcmp(Schriftart,"&#12415;&#12363;&#12385;&#12419;&#12435;-P")){return 0.557617187500000;}
  else if (!strcmp(Schriftart,"&#12415;&#12363;&#12385;&#12419;&#12435;")){return 0.683593750000000;}
  else if (!strcmp(Schriftart,"&#12373;&#12431;&#12425;&#12403;&#26126;&#26397;")){return 1.210000000000000;}
  else if (!strcmp(Schriftart,"&#12373;&#12431;&#12425;&#12403;&#12468;&#12471;&#12483;&#12463;")){return 0.995000000000000;}
  else if (!strcmp(Schriftart,"&#12365;&#12429;&#23383; - P")){return 0.603515625000000;}
  else if (!strcmp(Schriftart,"&#12365;&#12429;&#23383; - D")){return 0.726562500000000;}
  else if (!strcmp(Schriftart,"&#12365;&#12429;&#23383; - B")){return 0.707031250000000;}
  else if (!strcmp(Schriftart,"&#12365;&#12429;&#23383;")){return 0.718750000000000;}
  else if (!strcmp(Schriftart,"10.15 Saturday Night R BRK")){return 0.353000000000000;}
  else if (!strcmp(Schriftart,"10.15 Saturday Night BRK")){return 0.361000000000000;}
  return 1.0;
}
