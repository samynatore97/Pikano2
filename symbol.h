# ifndef SYMBOL_H_
# define SYMBOL_H_
# include "matrix.h"
# include <stdio.h>
# include "SDL_operations.h"
# include "list.h"
enum Type { INCONNU, BARRE, CLESOL, BEMOL, QUATRE, TROIS, DSOUPIR, NOTE, CLEFA,
  POINT, SOUPIR, PAUSE };
struct symbol
{
	SDL_Rect* box; 
	float nbPasV; // Hauteur de la boîte en pas
  float nbPasH; // Largeur de la boîte en pas
	float nbPixelNoir; // Ratio de pixels noirs
	size_t nbCol; // Nombre de barres verticales 
	int typeNote; // Type de l'élément
};
void print_symbol(struct symbol * ptr_symbol);
void print_list_symbol(struct list * list_symbol);
size_t* size_list_to_tab(struct list* list, size_t* taille);

enum Note {DO, RE, MI, FA, SOL, LA, SI};

struct portee
{
  size_t tab_lignes[5]; // Tableau des lignes hautes de la portée
  enum Note cle; // Clé de la portée
  enum Note diese[7]; // Tableau des dièses à la clé
  enum Note bemol[7]; // Tableau des bémols à la clé
  struct list* symboles; // Liste des symboles contenus dans la portée
};

struct partition {
  size_t h_ligne; // Hauteur en pixels d'une ligne de portée
  size_t i_ligne; // Hauteur en pixels de l'interligne d'une portée
  size_t taille; // Taille du tableau de portées
  struct portee** portees; // Tableau des portées
};

struct partition * analyse(struct s_matrix* mat, struct list* list);
# endif
