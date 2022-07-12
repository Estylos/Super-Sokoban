/**
 * @file movements.h
 * @author Estylos, caneuze, reed
 * @brief En-tête du programme de gestion des mouvements du joueur.
 * @copyright Sous licence MIT
 * 
 */
#ifndef __MOVEMENTS_H__
#define __MOVEMENTS_H__

/*------------------------------------------------------------------------------
	MACROS
------------------------------------------------------------------------------*/

#define BOX        '$'
#define WALL       '#'
#define PLAYER     '@'
#define TARGET     '.'
#define NOTHING    ' '
#define FULLBOX    'X'
#define OVERTARGET 'O'


/*------------------------------------------------------------------------------
	DECLARATIONS
------------------------------------------------------------------------------*/

/**
 * @brief Fonction servant à modifier les coordonnées du joueur dans le tableau selon une direction.
 *
 * @param direction Direction dans laquelle on souhaite déplacer le joueur (UP, DOWN, RIGHT, LEFT)
 */
extern void move(char direction);


#endif