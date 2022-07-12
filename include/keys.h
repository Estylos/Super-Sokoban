/**
 * @file keys.h
 * @author Estylos, caneuze, reed
 * @brief En-tête du programme de détection des appuis de touches sur le terminal.
 * @copyright Sous licence MIT
 * 
 */
#ifndef __KEYS_H__
#define __KEYS_H__

/*------------------------------------------------------------------------------
	MACROS
------------------------------------------------------------------------------*/

#define MAXSIZE    10

#define UP         1
#define DOWN       2
#define RIGHT      3
#define LEFT       4


/*------------------------------------------------------------------------------
	DECLARATIONS
------------------------------------------------------------------------------*/

/**
 * @brief Fonction servant de boucle principale du programme.
 * Attend qu'une touche soit pressée et exécute le sous-programme correspondant suivant la touche.
 * 
 * @param saveFile Chemin d'accès du fichier de sauvegarde
 */
extern void interactionLoop(char *saveFile);


#endif