/**
 * @file graphics.h
 * @author Estylos, caneuze, reed
 * @brief En-tête du programme de gestion de la configuration et l'affichage sur le terminal.
 * @copyright Sous licence MIT
 * 
 */
#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "levelLoader.h"

/*------------------------------------------------------------------------------
	MACROS
------------------------------------------------------------------------------*/

#define ANSI_CODE_RESET      "\033[00m"
#define ANSI_CODE_BOLD       "\033[1m"
#define ANSI_CODE_DARK       "\033[2m"
#define ANSI_CODE_UNDERLINE  "\033[4m"
#define ANSI_CODE_BLINK      "\033[5m"
#define ANSI_CODE_REVERSE    "\033[7m"
#define ANSI_CODE_CONCEALED  "\033[8m"
#define ANSI_CODE_GREY       "\033[30m"
#define ANSI_CODE_RED        "\033[31m"
#define ANSI_CODE_GREEN      "\033[32m"
#define ANSI_CODE_YELLOW     "\033[33m"
#define ANSI_CODE_BLUE       "\033[34m"
#define ANSI_CODE_MAGENTA    "\033[35m"
#define ANSI_CODE_CYAN       "\033[36m"
#define ANSI_CODE_WHITE      "\033[37m"
#define ANSI_CODE_BG_GREY    "\033[40m"
#define ANSI_CODE_BG_RED     "\033[41m"
#define ANSI_CODE_BG_GREEN   "\033[42m"
#define ANSI_CODE_BG_YELLOW  "\033[43m"
#define ANSI_CODE_BG_BLUE    "\033[44m"
#define ANSI_CODE_BG_MAGENTA "\033[45m"
#define ANSI_CODE_BG_CYAN    "\033[46m"
#define ANSI_CODE_BG_WHITE   "\033[47m"


/*------------------------------------------------------------------------------
	DECLARATIONS
------------------------------------------------------------------------------*/

/**
 * @brief Fonction servant à configurer le terminal en mode canonique sans affichage des touches.
 * 
 */
extern void configureTerminal(void);

/**
 * @brief Fonction servant à remettre le terminal avec affichage des touches.
 * 
 */
extern void resetTerminal(void);

/**
 * @brief Fonction servant à afficher sur l'écran toutes les informations d'un niveau.
 * En particulier, le numéro, l'auteur, le commentaire, la map et les contrôles. 
 * 
 * @param level Niveau à afficher
 */
extern void printLevel(Level *level);


#endif