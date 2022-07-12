/**
 * @file levelLoader.c
 * @author Estylos, caneuze, reed
 * @brief Programme chargeant les niveaux en mémoire.
 * @copyright Sous licence MIT
 * 
 */
#define _GNU_SOURCE		// Pour utiliser getline()

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "levelLoader.h"
#include "levelSaver.h"
#include "steps.h"
#include "movements.h"

/*------------------------------------------------------------------------------
	VARIABLES
------------------------------------------------------------------------------*/

Level *levelsNode = NULL;
Level *globalCurrentLevel = NULL;


/*------------------------------------------------------------------------------
	PROTOTYPES
------------------------------------------------------------------------------*/

static inline char startWith(char *keyword, char *command);
static inline char *removeKeyword(char *command);
static void parseLine(char *line);
static Level *insertLevel(unsigned int levelNumber);
static void insertInfo(char **member, char *args);
static void generateMap(char ***defaultMap, unsigned int *numberLines, char *line);


/*------------------------------------------------------------------------------
	FONCTIONS
------------------------------------------------------------------------------*/

void readLevelsFile(char *location)
{
	FILE *levelsFile = NULL;

	unsigned int lineLen = 0;
	long unsigned int lineSize = 0;
	char *lineBuffer = NULL;

	levelsFile = fopen(location, "r");
	if (levelsFile == NULL)
	{
		fprintf(stderr, "Erreur lors de l'ouverture du fichier...\n");
		perror("");
		exit(1);
	}

	while ((lineLen = getline(&lineBuffer, &lineSize, levelsFile)) != -1)	// getline alloue dynamiquement un buffer pour la ligne
		parseLine(lineBuffer);

	if (lineBuffer != NULL)
	{
		free(lineBuffer);
		lineBuffer = NULL;
	}

	fclose(levelsFile);
}

void initLevel(Level *level, char reset)
{
	char playerFound = 0;

	// Mise des scores à 0
	level->numberMov = 0;
	level->numberPush = 0;

	// Détermination de la position du joueur
	for (int y = 0; y < level->numberLines && !playerFound; y++)
	{
		char element = ' ';
		for (int x = 0; element != '\0' && !playerFound; x++)
		{
			element = level->defaultMap[y][x];
			if (element == PLAYER)
			{
				level->playerX = x;
				level->playerY = y;
				playerFound = 1;
			}
		}
	}

	if (!playerFound)
	{
		fprintf(stderr, "Joueur introuvable !\n");
		exit(1);
	}

	// Copie de defaultMap dans map pour la modification
	level->map = (char **) malloc((level->numberLines) * sizeof(char *));
	if (level->map == NULL)
	{
		fprintf(stderr, "Mémoire insuffisante !\n");
		exit(1);
	}

	for (int i = 0; i < level->numberLines; i++)
	{
		level->map[i] = (char *) malloc((strlen(level->defaultMap[i]) + 1) * sizeof(char));
		if (level->map[i] == NULL)
		{
			fprintf(stderr, "Mémoire insuffisante !\n");
			exit(1);
		}
		strcpy(level->map[i], level->defaultMap[i]);
	}

	if (reset)
	{
		freeStepsNode(level);	// On supprime les déplacements déjà sauvegardés
	}
	else if (level->stepsNode != NULL && !level->success)
	{
		// Quand on a quitté le jeu sans finir le niveau, on reprend là où on est
		char *serialisedSteps = stepsSerialiser(level->stepsNode);	// On récupère les mouvements pour les rejouer
		unsigned int strLen = strlen(serialisedSteps);
		freeStepsNode(level);	// On supprime les déplacements déjà sauvegardés : on va les regénérer avec move

		for (unsigned int i = 0; i < strLen; i++)
			move(serialisedSteps[i] - '0');

		free(serialisedSteps);
	}
}

char isPreviousReachable(Level *level)
{
	return (level->levelNumber != 1) ? 1 : 0;
}

char isNextReachable(Level *level)
{
	return ((level->nextLevel != NULL) && (level->success == 1)) ? 1 : 0;
}

void loadNextLevel(void)
{
	freeLevel(globalCurrentLevel);
	globalCurrentLevel = globalCurrentLevel->nextLevel;
	initLevel(globalCurrentLevel, 0);
}

void loadPreviousLevel(void)
{
	unsigned int levelToReach = globalCurrentLevel->levelNumber - 1;
	Level *ptrFollow = levelsNode;

	while (ptrFollow->levelNumber != levelToReach)
		ptrFollow = ptrFollow->nextLevel;

	freeLevel(globalCurrentLevel);
	globalCurrentLevel = ptrFollow;
	initLevel(globalCurrentLevel, 0);
}

/**
 * @brief Fonction servant à interpéter une ligne.
 * Chaque ligne du fichier de configuration est interprétée par cette fonction.
 * Pour les commandes (débutant par ;), elle sépare le mot clef des arguments et execute le code qui va avec le mot clef.
 * Pour les lignes des tableaux, elle construit la map 2D. 
 *   
 * @param line Chaîne de caractères a interpéter (peut être une commande ou un bout de tableau)
 */
static void parseLine(char *line)
{
	static Level *currentLevel = NULL;

	// On supprime les fins de ligne (CR et/ou LF) (évite de nombreux problèmes)
	if (strchr(line, '\r') != NULL)
		line[strcspn(line, "\r")] = '\0';
	if (strchr(line, '\n') != NULL)
		line[strcspn(line, "\n")] = '\0';

	if (startWith(";LEVEL", line))	// Mot clef LEVEL
		currentLevel = insertLevel(atoi(removeKeyword(line)));

	else if (startWith(";COMMENT", line))	// Mot clef COMMENT
		insertInfo(&(currentLevel->comment), removeKeyword(line));

	else if (startWith(";AUTHOR", line))	// Mot clef AUTHOR
		insertInfo(&(currentLevel->author), removeKeyword(line));

	else if (startWith(";SUCCESS", line))	// Mot clef SUCCESS
		currentLevel->success = atoi(removeKeyword(line));

	else if (startWith(";STEPS", line))	// Mot clef STEPS
		stepsParser(&currentLevel->stepsNode, removeKeyword(line));

	else	// Pas de mot clef : c'est une ligne d'un tableau
	{
		if (levelsNode == NULL)	// On n'interprète pas les lignes avant le premier ;LEVEL X
			return;

		generateMap(&(currentLevel->defaultMap), &(currentLevel->numberLines), line);
	}
}

/**
 * @brief Fonction servant à insérer un niveau dans la file.
 * 
 * @param levelNumber Numéro du niveau à insérer
 * @return lv : Pointeur vers le dernier élement de file
 */
static Level *insertLevel(unsigned int levelNumber)
{
	Level *lv = (Level *) malloc(sizeof(Level));
	if (lv == NULL)
	{
		fprintf(stderr, "Mémoire insuffisante !\n");
		exit(1);
	}

	lv->levelNumber = levelNumber;
	lv->comment = NULL;
	lv->author = NULL;
	lv->success = 0;
	lv->numberMov = 0;
	lv->numberPush = 0;
	lv->defaultMap = NULL;
	lv->map = NULL;
	lv->numberLines = 0;
	lv->stepsNode = NULL;
	lv->nextLevel = NULL;

	if (levelsNode == NULL)	// Dans le cas où la liste est vide
	{
		levelsNode = lv;
		return lv;
	}

	Level *ptrFollow = levelsNode;

	while (ptrFollow->nextLevel != NULL)	// On cherche la liste qui a le lien vide
		ptrFollow = ptrFollow->nextLevel;

	if (levelNumber != ptrFollow->levelNumber + 1)
	{
		fprintf(stderr, "Les numéros de niveaux ne se jouxtent pas =(\n");
		exit(1);
	}

	ptrFollow->nextLevel = lv;	// On fait pointer le dernier élément de la liste à notre nouvel élément

	return lv;
}

/**
 * @brief Fonction servant à copier une chaîne de caractères vers un emplacement mémoire.
 * Utilisée ici afin d'insérer une information dans un membre de la stucture d'un niveau (auteur, commentaire...).
 * 
 * @param member Pointeur vers un emplacement de la mémoire (peut être NULL)
 * @param args Information à placer dans la mémoire
 */
static void insertInfo(char **member, char *args)
{
	*member = (char *) malloc((strlen(args) + 1) * sizeof(char));
	if (*member == NULL)
	{
		fprintf(stderr, "Mémoire insuffisante !\n");
		exit(1);
	}
	strcpy(*member, args);
}

/**
 * @brief Fonction permetant de générer un tableau 2D (map) à partir de lignes d'un tableau. 
 * La fonction va rajouter la ligne du tableau passée en paramètre à la fin de la map.
 * Chaque ligne du tableau se termine par \0 (chaîne de caractères). 
 * 
 * @param defaultMap Pointeur vers la map a qui on veut rajouter une ligne, **defaultMap peut être NULL
 * @param numberLines Pointeur vers le nombre de lignes de la map
 * @param line Chaîne de caracères à ajouter à la map
 */
static void generateMap(char ***defaultMap, unsigned int *numberLines, char *line)
{
	/*
	 * Cette fonction va créer une map temporaire de la taille de la map originale +1 afin
	 * de stocker une nouvelle ligne du tableau. Il faut bien penser à alouer dynamiquement
	 * de la mémoire pour notre ligne afin de la mettre dans la map... Puis on fait pointer
	 * notre ancienne map sur notre nouvelle 
	 */

	char **newMap = (char **) realloc(*defaultMap, (*numberLines + 1) * sizeof(char *));
	if (newMap == NULL)
	{
		fprintf(stderr, "Mémoire insuffisante !\n");
		exit(1);
	}

	newMap[*numberLines] = (char *) malloc((strlen(line) + 1) * sizeof(char));
	if (newMap[*numberLines] == NULL)
	{
		fprintf(stderr, "Mémoire insuffisante !\n");
		exit(1);
	}

	strcpy(newMap[*numberLines], line);

	*defaultMap = newMap;
	*numberLines += 1;
}

/**
 * @brief Fonction déterminant si une chaîne débute par un mot clef donné.
 * 
 * @param keyword Mot clef
 * @param command Commande
 * @return Booléen : 0 si le mot clef est trouvé, 1 sinon
 */
static inline char startWith(char *keyword, char *command)
{
	return strncmp(keyword, command, strlen(keyword)) == 0;
}

/**
 * @brief Fonction qui supprime un mot clef d'une chaîne.
 * Chaque commande commence par un mot clef, suivie d'un espace.
 * La fonction renvoie tout ce qui suit l'espace.
 * 
 * @param command Commande
 * @return args : Pointeur vers le caractère suivant l'espace
 */
static inline char *removeKeyword(char *command)
{
	char *args = command;
	while (*args && *args != ' ')
		args++;

	return ++args;
}