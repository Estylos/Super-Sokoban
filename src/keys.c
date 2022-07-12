/**
 * @file keys.c
 * @author Estylos, caneuze, reed
 * @brief Programme de détection des appuis de touches dans le terminal.
 * @copyright Sous licence MIT
 *
 */
#include <unistd.h>
#include <stdio.h>

#include "keys.h"
#include "graphics.h"
#include "levelLoader.h"
#include "levelSaver.h"
#include "movements.h"
#include "steps.h"

/*------------------------------------------------------------------------------
	PROTOTYPES
------------------------------------------------------------------------------*/

static int readKeyboard(void);


/*------------------------------------------------------------------------------
	FONCTIONS
------------------------------------------------------------------------------*/

void interactionLoop(char *saveFile)
{
	printLevel(globalCurrentLevel);

	for (;;)
	{
		char refreshTerminal = 0;	// On n'actualise l'affichage que lorsque cela est nécessaire

		switch (readKeyboard())
		{
		case UP:
			if (!globalCurrentLevel->success)
			{
				move(UP);
				refreshTerminal = 1;
			}
			break;
		case DOWN:
			if (!globalCurrentLevel->success)
			{
				move(DOWN);
				refreshTerminal = 1;
			}
			break;
		case RIGHT:
			if (!globalCurrentLevel->success)
			{
				move(RIGHT);
				refreshTerminal = 1;
			}
			break;
		case LEFT:
			if (!globalCurrentLevel->success)
			{
				move(LEFT);
				refreshTerminal = 1;
			}
			break;
		case 'z':	// Annuler un déplacement
			if (!globalCurrentLevel->success)
			{
				backStep(globalCurrentLevel);
				refreshTerminal = 1;
			}
			break;
		case 'r':	// Réinitialise le niveau
			if (!globalCurrentLevel->success)
			{
				freeLevel(globalCurrentLevel);
				initLevel(globalCurrentLevel, 1);
				refreshTerminal = 1;
			}
			break;
		case 't':	// Revoir les déplacements effectués
			if (globalCurrentLevel->success)
			{
				replaySteps();
				refreshTerminal = 1;
			}
			break;
		case 'p':	// Niveau précédent
			if (isPreviousReachable(globalCurrentLevel))
			{
				loadPreviousLevel();
				refreshTerminal = 1;
			}
			break;
		case 'n':	// Niveau suivant
			if (isNextReachable(globalCurrentLevel))
			{
				loadNextLevel();
				refreshTerminal = 1;
			}
			break;
		case 's':	// Sauvegarder
			saveLevels(saveFile);
			printf("\nPartie sauvegardée !\n");
			break;
		case 'q':	// Sauvegarder et quitter
			printf("\nA bientôt sur Super Sokoban =)\n");
			saveLevels(saveFile);
			return;
		}

		if (refreshTerminal)
			printLevel(globalCurrentLevel);
	}
}

/**
 * @brief Fonction servant à lire la touche pressée.
 * Cette fonction est bloquante.
 * 
 * @return Code de la touche lue.
 */
static int readKeyboard(void)
{
	char buff[MAXSIZE];
	int c;

	if ((c = read(0, buff, 3)) == -1)	// Si read échoue
		return 0;

	switch (buff[0])
	{
	case 27:
		if ((c == 3) && (buff[1] == 91))
			return (buff[2] - 64);
		else
			return 0;
		break;
	default:
		return buff[0];
		break;
	}
}