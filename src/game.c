/**
 * @file game.c
 * @author Estylos, caneuze, reed
 * @brief Programme principal du jeu.
 * @copyright Sous licence MIT
 * 
 */
#include <stdlib.h>
#include <stdio.h>

#include "levelLoader.h"
#include "levelSaver.h"
#include "graphics.h"
#include "keys.h"
#include "movements.h"
#include "steps.h"

/**
 * @brief Fonction d'entrée du programme.
 * 
 * @param argc Nombre d'arguments d'éxecution
 * @param argv Tableau des arguments d'éxecution
 * @return Etat de sortie du programme
 */
int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		fprintf(stderr, "Mauvaise utilisation de la commande... Utilisez : %s [fichier de niveaux] [fichier de sauvegarde]\n", argv[0]);
		return 1;
	}
	printf("Chargement...\n");

	readLevelsFile(argv[1]);
	if (levelsNode == NULL)
	{
		fprintf(stderr, "Fichier d'entrée incorrect\n");
		return 1;
	}

	configureTerminal();

	globalCurrentLevel = levelsNode;
	while (isNextReachable(globalCurrentLevel))
		globalCurrentLevel = globalCurrentLevel->nextLevel;

	initLevel(globalCurrentLevel, 0);

	interactionLoop(argv[2]);

	freeLevel(globalCurrentLevel);
	freeNode();

	resetTerminal();

	return 0;
}