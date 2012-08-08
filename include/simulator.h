/*! \file simulator.h
*   \brief Headerdatei für den Cachesimulator.
*	
*	Deklaration der Funktionen und structs für den Cachesimulator
*/

#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

#include <stdio.h>
#include <stdlib.h>

#define WORDLENGTH 4

#define PATHLENGTH 100
int main();


typedef struct {
	int tag;
	int age;
	char line[WORDLENGTH];
} cacheline;

/**
 * Funktion zum Abfragen einer Zahl. Rückgabewert ist die Zahl 
 */
int read_number(char*);

/** 
 *Funktion zum Abfragen eines Datepfades. Rückgabewert ist der Pfad 
 */
char* read_path(char*);

/**
 * Prozedur zum setzen des Alters in einer Cache-line. Rückgabewert gibts nicht 
 */
void age_setter(cacheline**, int, int);

/**
 * Funktion zum erfragen der ältesten Position einer Cacheline. Rückgabewert ist der Arrayindex 
 */
int age_getter(cacheline**, int);

/** 
 * Prozedur zum ausgeben. Nur für Testzwecke 
 */
void printer(cacheline** cline, int asso);

#endif