/**
 * @file steps.c
 * @author Estylos, caneuze, reed
 * @brief Programme de gestion de l'historique des déplacements.
 * @copyright Sous licence MIT
 * 
 */
#define _XOPEN_SOURCE   600	// Pour utiliser usleep()
#define _POSIX_C_SOURCE 200112L	// Pour utiliser usleep()

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "steps.h"
#include "levelLoader.h"
#include "levelSaver.h"
#include "keys.h"
#include "movements.h"
#include "graphics.h"

/*------------------------------------------------------------------------------
	PROTOTYPES
------------------------------------------------------------------------------*/

static inline void movePlayer(char *cell);


/*------------------------------------------------------------------------------
	FONCTIONS
------------------------------------------------------------------------------*/

void addStep(Step **steps, unsigned char direction, unsigned char cellReplaced, unsigned char cellReplacedPlus)
{
	Step *step = (Step *) malloc(sizeof(Step));
	if (step == NULL)
	{
		fprintf(stderr, "Mémoire insuffisante !\n");
		exit(1);
	}

	step->direction = direction;
	step->cellReplaced = cellReplaced;
	step->cellReplacedPlus = cellReplacedPlus;
	step->previousStep = *steps;

	*steps = step;
}

void backStep(Level *level)
{
	Step *lastStep = level->stepsNode;

	if (lastStep == NULL || (lastStep->cellReplaced == 0 && lastStep->cellReplacedPlus == 0))
		return;

	unsigned int x = level->playerX;
	unsigned int y = level->playerY;

	switch (level->stepsNode->direction)
	{
	case UP:
		movePlayer(&level->map[y + 1][x]);
		level->map[y][x] = lastStep->cellReplaced;
		if (lastStep->cellReplacedPlus)
			level->map[y - 1][x] = lastStep->cellReplacedPlus;
		level->playerY += 1;
		break;
	case DOWN:
		movePlayer(&level->map[y - 1][x]);
		level->map[y][x] = lastStep->cellReplaced;
		if (lastStep->cellReplacedPlus)
			level->map[y + 1][x] = lastStep->cellReplacedPlus;
		level->playerY -= 1;
		break;
	case RIGHT:
		movePlayer(&level->map[y][x - 1]);
		level->map[y][x] = lastStep->cellReplaced;
		if (lastStep->cellReplacedPlus)
			level->map[y][x + 1] = lastStep->cellReplacedPlus;
		level->playerX -= 1;
		break;
	case LEFT:
		movePlayer(&level->map[y][x + 1]);
		level->map[y][x] = lastStep->cellReplaced;
		if (lastStep->cellReplacedPlus)
			level->map[y][x - 1] = lastStep->cellReplacedPlus;
		level->playerX += 1;
		break;
	default:
		break;
	}

	level->numberMov -= 1;
	if (lastStep->cellReplaced == BOX)
		level->numberPush -= 1;

	level->stepsNode = lastStep->previousStep;
	free(lastStep);
}

void freeStepsNode(Level *level)
{
	Step *ptrFollow = level->stepsNode;

	while (level->stepsNode != NULL)
	{
		level->stepsNode = level->stepsNode->previousStep;
		free(ptrFollow);
		ptrFollow = level->stepsNode;
	}
	level->stepsNode = NULL;
}

void stepsParser(Step **steps, char *str)
{
	unsigned int stringLen = strlen(str);

	for (int i = 0; i < stringLen; i++)
	{
		switch (str[i] - '0')
		{
		case UP:
			addStep(steps, UP, 0, 0);
			break;

		case DOWN:
			addStep(steps, DOWN, 0, 0);
			break;

		case LEFT:
			addStep(steps, LEFT, 0, 0);
			break;

		case RIGHT:
			addStep(steps, RIGHT, 0, 0);
			break;

		default:
			fprintf(stderr, "Erreur dans la chaine de sauvegarde de trajets.\n");
			break;
		}
	}
}

char *stepsSerialiser(Step *steps)
{
	if (steps == NULL)
		return "";

	unsigned int strLen = 1;

	Step *ptrFollow = steps;
	while (ptrFollow->previousStep != NULL)
	{
		strLen++;
		ptrFollow = ptrFollow->previousStep;
	}

	char *serialisedStr = (char *) malloc((strLen + 1) * sizeof(char));
	if (serialisedStr == NULL)
	{
		fprintf(stderr, "Mémoire insuffisante !\n");
		exit(1);
	}

	serialisedStr[strLen] = '\0';
	for (int i = strLen - 1; i >= 0; i--)
	{
		serialisedStr[i] = steps->direction + '0';
		steps = steps->previousStep;
	}

	return serialisedStr;
}

void replaySteps(void)
{
	if (globalCurrentLevel->stepsNode == NULL)	// Si les étapes ont été effacé manuellement du fichier alors que le niveau était résolu
	{
		globalCurrentLevel->success = 0;
		return;
	}

	char *serialisedSteps = stepsSerialiser(globalCurrentLevel->stepsNode);
	unsigned int strLen = strlen(serialisedSteps);
	freeStepsNode(globalCurrentLevel);	// On supprime les déplacements déjà sauvegardés : on va les regénérer avec move

	freeLevel(globalCurrentLevel);
	initLevel(globalCurrentLevel, 0);

	for (unsigned int i = 0; i < strLen; i++)
	{
		printLevel(globalCurrentLevel);
		printf("\nRelecture du trajet en cours...\n");
		move(serialisedSteps[i] - '0');
		usleep(100 * 1000);	// Met le jeu en pause 100 ms
	}
	free(serialisedSteps);
}

/**
 * @brief Fonction remplaçant le contenu d'une cellule par le joueur.
 * Permet de gérer le cas où la cellule est une cible.
 *
 * @param cell Caractère représentant l'objet de la cellule
 */
static inline void movePlayer(char *cell)
{
	*cell = (*cell == TARGET) ? OVERTARGET : PLAYER;
}