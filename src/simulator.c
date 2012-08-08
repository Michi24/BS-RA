/*! \file simulator.c
*   \brief Simulator zum simulieren von direct mapped, sowie 2 und 4 Wegge Asso Caches
*	
*	Diese Bibliothek stellt verschiedene Funktionen bereit, um geometrische Figuren auf eine BMP zu zeichnen.
*/

#include "simulator.h"


int main(){
	char* path;
	char* usage;
	int cachesize = 0;
	int cache_lines, addr, index;
	int asso = 0;
	int hit = 0;
	int miss = 0;
	int overwrites = 0;
	int empty_found = 0;
	FILE *fd;
	int nRet;
	size_t hm = 0;
	char **gptr = malloc(sizeof(char*));
	if (gptr == NULL){
		exit(EXIT_FAILURE);
	}
	*gptr = NULL;

	/* Pfad einlesen */
	path = read_path("Bitte Geben Sie einen Pfad ein\nEingabe: ");

	/* Cachegröße einlesen */
	do {
		cachesize = read_number("Bitte Geben Sie die Cachegroesse an (zw. 16-65536)\nEingabe: ");
	} while (cachesize <=15 && cachesize >= 65537);
		
	/* Assoziativitaet einlesen */
	do {
		asso = read_number("Bitte Geben Sie die Assoziativitaet an\n\t1 - direct mapped\n\t2 - 2-Wege-Assoziativ\n\t4 - 4-Wege-Assoziativ\nEingabe: ");
	} while (asso != 1 && asso != 2 && asso != 4);
	
	/* Nutzung einlesen */
	do {
		usage = read_path("Bitte Geben Sie die Nutzung an\n\tD - Daten-Cache\n\tI - Instruction-Cache\n\tU - Unified(beides)\nEingabe: ");
	printf("%c\n", usage[0]);
		
	} while (usage[0] != 'D' && usage[0] != 'U' && usage[0] != 'I');
	
	/* Arrayzeilen bestimmen und Array anlegen */
	//cache_lines = (cachesize / WORDLENGTH) / asso;
	cache_lines = (cachesize / asso / WORDLENGTH);
	cacheline* cache[cache_lines][asso];
	
	printf("Zeilen: %i\nSpalten: %i\n", cache_lines, asso);
	
	/* ########## Speicher klarmachen ########## */
	for (int hh = 0;hh<asso;hh++) {
		for (int gg = 0; gg <cache_lines; gg++ ) {
			cache[gg][hh] = NULL;
		}
	}
	/* ########## Speicher klarmachen ########## */
	
	if ( (fd = fopen(path,"r")) == NULL) {
		fprintf(stderr, "\nKonnte Datei %s nicht öffnen!", path);
		exit(EXIT_FAILURE);
	}

	while( (nRet=getline(gptr, &hm, fd)) > 0){
		/* prüfen ob die Zeile gelesen werden muss */
		if (usage[0] != 'U'){
			if (usage[0] == 'I' && (atoi(gptr[0]) == 0 || atoi(gptr[0]) == 1)){
				continue;
			}
			if (usage[0] == 'D' && (atoi(gptr[0]) == 2)){
				continue;
			}
		}
		/* Zahl in Dezimal umrechnen und Index bestimmen */
		addr = strtol(gptr[0]+2, NULL, 16);
		// Adresse durch 4 teilen, da offset uninteressant !!!!
		addr /= 4;
		index = addr % cache_lines;
		/* Cache prüfen und auslesen oder schreiben */
		int tmp = hit;
		int tmp2 = miss;
		for (int i = 0; i<asso; i++) { 
			/* Entweder wir finden unseren Eintrag oder wir finden nen leeren */
			if (cache[index][i] != NULL) {
				/* Was gefunden */ 
				if (cache[index][i]->tag == addr) {
					/* Hit! */
					hit++;
					age_setter(cache[index], i, asso);
					cache[index][i]->age = 0;
					break;
				}
			} else {
				/* Leerer Eintrag gefunden, ergo is alles danach egal ;) */
				empty_found++;
				miss++;
				cacheline* new = malloc(sizeof(cacheline));
				if (new == NULL){
					exit(EXIT_FAILURE);
				}
				new->tag = addr;
				new->age = 0;
				cache[index][i] = new;
				// irgendwas mim Alter machen
				age_setter(cache[index], i, asso);
				break;
			}
		}
		/* Prüfen ob was vorher gefunden wurde. Wenn nicht is das Array voll und unser Eintrag fehlt (noch) */
		if ((tmp == hit) && (tmp2 == miss)) {
			/* Größten Index suchen */
			int last = age_getter(cache[index], asso);
			//printer(cache[index], asso);
			//printf("Wert: %i - Index: %i\n",addr, last);
			//printf("chg: %i - cache[%i][0] = %i | cache[%i][1] = %i | cache[%i][1] = %i | cache[%i][1] = %i\n",last,  index, cache[index][0]->age, index,  cache[index][1]->age, index,  cache[index][2]->age, index,  cache[index][3]->age);
			miss++;
			overwrites++;
			/* Eintrag reinpacken an älteste Stelle */
			free(cache[index][last]);
			cacheline* new = malloc(sizeof(cacheline));
			if (new == NULL){
				exit(EXIT_FAILURE);
			}
			new->tag = addr;
			new->age = 0;
			cache[index][last] = new;
			// irgendwas mim Alter machen
			age_setter(cache[index], last, asso);
			
		}
	}
	printf("Fertig: \n\tHit: %i\n\tMiss: %i\n\tHitrate: %.3f\n\tMissrate: %.3f\n\tOverwrites: %i\n\tempy_found: %i\n\n", hit, miss, (float)hit/((float)miss+(float)hit), (float)miss/((float)miss+(float)hit), overwrites, empty_found);

	fclose(fd);
	/* ########## Speicher klarmachen ########## */
	for (int hh = 0;hh<asso;hh++) {
		for (int gg = 0; gg <cache_lines; gg++ ) {
			free(cache[gg][hh]);
		}
	}
	/* ########## Speicher klarmachen ########## */
	free(path);
	free(usage);
	free(*gptr);
	free(gptr);
	//free(t);
	
}

/** 
 * Funktion zum erfragen der ältesten Position einer Cacheline. Rückgabewert ist der Arrayindex 
 * @param cline Pointer auf Cacheline
 * @param asso Integer für die Assoziativität.
 * @post Keine Änerungen in der Zeile selbst
 * @return Integer mit Position der ältesten Cacheline
 */
int age_getter(cacheline** cline, int asso){
	int mx = cline[0]->age;
	int pos = 0;
	for (int j = 1; j<asso; j++) { 
		if (cline[j]->age > mx) {
			pos = j;
			mx = cline[j]->age;
		}
	}
	return pos;
}

/** 
 * Prozedur zum erfragen der ältesten Position einer Cacheline.
 * @param cline Pointer auf Cacheline
 * @param asso Integer für die Assoziativität.
 * @post Nur Bildschirmausgabe
 * @return -
 */
void printer(cacheline** cline, int asso){
	for (int j = 0; j<asso; j++) { 
		printf("cline[%i]: %i -age: %i - ", j, cline[j]->tag, cline[j]->age);
	}
}

/** 
 * Prozedur zum setzen des Alters in einer Cache-line.
 * @param cline Pointer auf Cacheline
 * @param i Integer mit der Position des nicht zu ändernden Alters
 * @param asso Integer für die Assoziativität.
 * @post Jedes Alter in der Cacheline ausser der Stelle i wird erhöht
 * @return -
 */
void age_setter(cacheline** cline, int i, int asso){
	/* Einträge zahlen */
	int max = 0;
	/* Länge der Cacheline feststellen */
	for (int j = 0; j<asso; j++) { 
		if (cline[j] != NULL) {
			max++;
		}
	}
	/* Alter setzen */
	for (int j = 0; j<max; j++) { 
		if (j != i) {
			cline[j]->age++;
		}
	}
}

/** 
 * Funktion zum Abfragen eines Strings. Wird nur Verlassen bei erfolgreicher 
 * Eingabe
 * @param question Frage die angezeigt wird
 * @post -
 * @return Pointer auf die eingegebene Antwort
 */
char* read_path(char* question){
	char* erg = malloc(PATHLENGTH);
	if (erg == NULL){
		exit(EXIT_FAILURE);
	}
	int number = 1;
	/* Die Schleife für die Eingabe. Sie wird erst verlassen bei sinnvoller Eingabe */
	while (number) {
		/* Frage stellen  */
		fputs(question, stdout);
		/* Buffer leeren */
		fflush(stdout);
		/* Einlesen */
		if ( fgets(erg, sizeof erg, stdin) ){
			return erg;
		} else {
			number = -1;
		}
	}
	return erg;
}

/** 
 * Methode zum Abfragen einer Zahl. Wird nur Verlassen bei erfolgreicher Eingabe
 * @param question Frage die angezeigt wird
 * @post -
 * @return Integer der eingegeben Zahl
 */
/** */
int read_number(char* question){
	/* char array für die Eingabe. Das Array ist größer 1 um etwas mehr komfort bei der Eingabe zu ermöglichen. */
	char text[20];
	int number = 1;
	/* Die Schleife für die Eingabe. Sie wird erst verlassen bei sinnvoller Eingabe */
	while (number) {
		/* Frage stellen  */
		fputs(question, stdout);
		/* Buffer leeren */
		fflush(stdout);
		/* Einlesen */
		if ( fgets(text, sizeof text, stdin) )
		{
			/* Einbabe in eine Zahl umwandeln */
			if ( sscanf(text, "%d", &number) == 1 )
			{
				return number;
			}else{
				number = -1;
			}
		}else{
			number = -1;
		}
	}
	return number;
}