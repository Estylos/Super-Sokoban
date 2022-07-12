/**
 * @file graphics.c
 * @author Estylos, caneuze, reed
 * @brief Programme de gestion de la configuration et de l'affichage sur le terminal.
 * @copyright Sous licence MIT
 * 
 */
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graphics.h"
#include "movements.h"
#include "levelLoader.h"

/*------------------------------------------------------------------------------
	VARIABLES
------------------------------------------------------------------------------*/

static struct termios terminalSetup;


/*------------------------------------------------------------------------------
	PROTOTYPES
------------------------------------------------------------------------------*/

static inline void refreshTerminal(void);
static void printMap(char **map, int maxHeight);
static void printHeader(unsigned int levelNumber, char *author, char *comment);
static void printFooter(char success, char reachablePrevious, char reachableNext);
static void printScore(unsigned int numberMov, unsigned int numberPush);


/*------------------------------------------------------------------------------
	FONCTIONS
------------------------------------------------------------------------------*/

void printLevel(Level *level)
{
	refreshTerminal();
	printHeader(level->levelNumber, level->author, level->comment);
	printMap(level->map, level->numberLines);
	printScore(level->numberMov, level->numberPush);
	printFooter(level->success, isPreviousReachable(level), isNextReachable(level));
}

void configureTerminal(void)
{
	if (tcgetattr(0, &terminalSetup) == -1)
	{
		perror("tcgetattr");
		exit(1);
	}
	terminalSetup.c_lflag &= ~(ICANON);	// Met le terminal en mode non canonique (interprète à chaque touche)
	terminalSetup.c_lflag &= ~(ECHO);	// Les touches tapées ne s'inscriront plus dans le terminal
	terminalSetup.c_cc[VMIN] = 1;		// Nombre minimum de caractères lors d'une lecture en mode non canonique
	terminalSetup.c_cc[VTIME] = 0;		// Délai en dixièmes de seconde pour une lecture en mode non canonique
	if (tcsetattr(0, TCSANOW, &terminalSetup) == -1)	// tcsetattr() indique une réussite si une des modifications peut être réalisée
	{
		perror("tcsetattr");
		exit(1);
	}
}

void resetTerminal(void)
{
	terminalSetup.c_lflag |= (ICANON);	// Met le terminal en mode canonique (attend un appui sur entrée pour interpréter)
	terminalSetup.c_lflag |= (ECHO);	// Les touches tapées s'inscriront dans le terminal
	if (tcsetattr(0, TCSANOW, &terminalSetup) == -1)
	{
		perror("tcsetattr");
		exit(1);
	}
}

/**
 * @brief Fonction servant à afficher la map avec des couleurs.
 *
 * @param map Tableau 2D
 * @param maxHeight Nombre de lignes du tableau
 */
static void printMap(char **map, int maxHeight)
{
	for (int i = 0; i < maxHeight; i++)
	{
		for (int j = 0; j < strlen(map[i]); j++)
		{
			switch (map[i][j])
			{
			case PLAYER:
				printf(ANSI_CODE_RED "%c" ANSI_CODE_RESET, map[i][j]);
				break;

			case BOX:
				printf(ANSI_CODE_YELLOW "%c" ANSI_CODE_RESET, map[i][j]);
				break;

			case TARGET:
				printf(ANSI_CODE_GREEN "%c" ANSI_CODE_RESET, map[i][j]);
				break;

			case FULLBOX:
				printf(ANSI_CODE_BLUE "%c" ANSI_CODE_RESET, map[i][j]);
				break;

			case OVERTARGET:
				printf(ANSI_CODE_CYAN "%c" ANSI_CODE_RESET, map[i][j]);
				break;

			default:
				printf("%c", map[i][j]);
				break;
			}
		}
		printf("\n");
	}

	printf("\n");
}

/**
 * @brief Fonction qui affiche l'en-tête du jeu (titre, noms, niveau, auteur, commentaire).
 * 
 * @param levelNumber Numéro du niveau
 * @param author Auteur du niveau
 * @param comment Commentaire sur le niveau
 */
static void printHeader(unsigned int levelNumber, char *author, char *comment)
{
	printf(ANSI_CODE_BG_BLUE ANSI_CODE_BOLD ANSI_CODE_UNDERLINE
	       "Super Sokoban par Estylos, caneuze, reed"
	       ANSI_CODE_RESET);
	printf("\n\n");
	printf(ANSI_CODE_BOLD ANSI_CODE_GREEN 
	       "Niveau numéro %d\n"
	       ANSI_CODE_RESET, levelNumber);
	if (author != NULL)
		printf(ANSI_CODE_CYAN 
		       "Auteur : %s"
			   ANSI_CODE_RESET, author);
	else
		printf(ANSI_CODE_CYAN
		       "Auteur : inconnu"
			   ANSI_CODE_RESET);

	if (comment != NULL)
	{
		printf(ANSI_CODE_CYAN
		       " | Commentaire : %s"
		       ANSI_CODE_RESET, comment);
	}

	printf("\n\n");
}

/**
 * @brief Fonction qui affiche les commandes disponibles en bas de terminal selon si le niveau est réussi et si le niveau suivant / précédent sont atteignables.
 * 
 * @param success Booléen : 1 si le niveau est réussi ; 0 sinon
 * @param reachablePrevious Booléen : 1 si le niveau précédent est atteingnable (ie si on n'est pas au niveau 1) ; 0 sinon
 * @param reachableNext Booléen : 1 si le niveau suivant est atteignable (s'il y a un niveau après ET si le niveau actuel est résolu) ; 0 sinon
 */
static void printFooter(char success, char reachablePrevious,
			char reachableNext)
{
	if (success)
	{
		printf(ANSI_CODE_YELLOW
		       "Bravo, vous avez réussi ce niveau !\n");
		printf(ANSI_CODE_MAGENTA "t : revoir votre trajet\n");

		if (reachablePrevious && reachableNext)
			printf
			    ("p : niveau précédent    n : niveau suivant\n");
		else if (reachablePrevious && !reachableNext)
			printf("p : niveau précédent\n");
		else if (!reachablePrevious && reachableNext)
			printf("n : niveau suivant\n");

		printf("s : sauvegarder         q : sauvergarder et quitter\n"
		       ANSI_CODE_RESET);
	}
	else
	{
		printf(ANSI_CODE_MAGENTA
		       "Deplacer le joueur avec les fleches du clavier\n");
		printf("z : annuler\t\tr : recommencer\n");
		if (reachablePrevious)
			printf("p : niveau précédent\n");
		printf("s : sauvegarder\t\tq : sauvergarder et quitter\n"
		       ANSI_CODE_RESET);
	}

}

/**
 * @brief Fonction qui affiche les scores en dessous de la carte.
 * 
 * @param numberMov Nombre de déplacements du joueur
 * @param numberPush Nombre de caisses poussées
 */
static void printScore(unsigned int numberMov, unsigned int numberPush)
{
	printf("Mouvements : %d\tCaisses poussées : %d\n", numberMov, numberPush);
}

/**
 * @brief Fonction servant à effacer le terminal.
 * 
 */
static inline void refreshTerminal(void)
{
	printf("\e[1;1H\e[2J");	// Séquence d'échappement permettant d'effacer le terminal
}