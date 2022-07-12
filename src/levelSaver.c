/**
 * @file levelSaver.c
 * @author Estylos, caneuze, reed
 * @brief Programme sauvegardant les niveaux en mémoire.
 * @copyright Sous licence MIT
 * 
 */
#include <stdio.h>
#include <stdlib.h>

#include "levelSaver.h"
#include "levelLoader.h"
#include "steps.h"

/*------------------------------------------------------------------------------
	FONCTIONS
------------------------------------------------------------------------------*/

void saveLevels(char *destination)
{
	FILE *saveFile = NULL;

	saveFile = fopen(destination, "w");
	if (saveFile == NULL)
	{
		fprintf(stderr, "Erreur lors de l'ouverture du fichier de sauvegarde...\n");
		perror("");
		exit(1);
	}

	Level *ptr = levelsNode;

	while (ptr != NULL)
	{
		fprintf(saveFile, ";LEVEL %d\n", ptr->levelNumber);

		if (ptr->comment != NULL)
			fprintf(saveFile, ";COMMENT %s\n", ptr->comment);

		if (ptr->author != NULL)
			fprintf(saveFile, ";AUTHOR %s\n", ptr->author);

		fprintf(saveFile, ";SUCCESS %d\n", ptr->success);

		if (ptr->stepsNode != NULL)
		{
			char *strStep = stepsSerialiser(ptr->stepsNode);
			fprintf(saveFile, ";STEPS %s\n", strStep);
			free(strStep);
		}

		for (int i = 0; i < (ptr->numberLines); i++)
			fprintf(saveFile, "%s\n", ptr->defaultMap[i]);

		ptr = ptr->nextLevel;
	}

	fclose(saveFile);
}

void freeNode(void)
{
	Level *ptrFollow = levelsNode;
	Level *precPtdr = NULL;

	while (ptrFollow != NULL)
	{
		for (int i = 0; i != ptrFollow->numberLines; i++)
			free(ptrFollow->defaultMap[i]);
		free(ptrFollow->defaultMap);

		freeStepsNode(ptrFollow);
		free(ptrFollow->author);
		free(ptrFollow->comment);
		precPtdr = ptrFollow;
		ptrFollow = ptrFollow->nextLevel;
		free(precPtdr);
	}
}

void freeLevel(Level *level)
{
	for (int i = 0; i < level->numberLines; i++)
		free(level->map[i]);
	free(level->map);
}