# ifndef SYMBOL_H_
# define SYMBOL_H_
# include "matrix.h"
# include <stdio.h>
# include "SDL_operations.h"
# include "list.h"
enum Type { BARRE=1, CLESOL, BEMOL, QUATRE, TROIS, DSOUPIR,
  NOTE, CLEFA, POINT, SOUPIR, PAUSE, INCONNU};
struct symbol
{
	SDL_Rect* box; 
	float nbPasV; // Hauteur de la boîte en pas
  float nbPasH; // Largeur de la boîte en pas
	float nbPixelNoir; // Ratio de pixels noirs
	size_t nbCol; // Nombre de barres verticales 
	enum Type typeNote; // Type de l'élément
};
void print_symbol(struct symbol * ptr_symbol);
void print_list_symbol(struct list * list_symbol);
size_t* size_list_to_tab(struct list* list, size_t* taille);

enum Note {DO, RE, MI, FA, SOL, LA, SI};
enum Type_N {NOIRE=1, BLANCHE, CROCH, DCROCH, POINTEE,INCONNUE};

struct note
{
 enum Note value;
 int octave;
 enum Type_N t_note;
 struct symbol * symb_note;
 int duree;
};

void list_insert_note(struct list* list, struct list* elm);
struct list* analyse_note(struct s_matrix* mat, 
	struct symbol* symbol, size_t pas);
	
struct portee
{
  size_t tab_lignes[5]; // Tableau des lignes hautes de la portée
  enum Note cle; // Clé de la portée
  int diese[7]; // Tableau des dièses à la clé
  int bemol[7]; // TaNresbleau des bémols à la clé
  struct list* symboles; // Liste des symboles contenus dans la portée
  struct list* notes;
};

struct partition {
  size_t h_ligne; // Hauteur en pixels d'une ligne de portée
  size_t i_ligne; // Hauteur en pixels de l'interligne d'une portée
  size_t taille; // Taille du tableau de portées
  struct portee** portees;// Tableau des portées
  char* encodage;
};

struct partition * analyse(struct s_matrix* mat, struct list* list);
enum Note find_height_box(struct partition * partition, 
    struct portee * portee, SDL_Rect * box, struct note* note);
#endif
