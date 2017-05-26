# ifndef MATRIX_H_
# define MATRIX_H_

# include <SDL/SDL.h>
# include <SDL/SDL_image.h>
# include <stdlib.h>
# include <err.h>
# include "list.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

enum Couleur {NOIR, BLANC, BLEU1, ROUGE, VERT, VERT2, BLEU2, ROSE};

// Structure definition
struct s_matrix
{
    size_t lines;
    size_t cols;
    enum Couleur *data;
};

struct s_matrix *genMatFromImg(SDL_Surface *img);
void print_matrix(struct s_matrix *mat);

/*
** matrix_init(mat)
** struct s_matrix* -> |
** Initialize matrix with 0 in every cell
*/
void matrix_init(struct s_matrix *mat, size_t lines, size_t cols);
/*
** matrix_copy(mat)
** struct s_matrix* -> struct s_matrix*
** Returns a copy of the given matrix
*/
struct s_matrix* matrix_copy(struct s_matrix* mat);
void draw_rect(struct s_matrix *mat, SDL_Rect* box);
struct s_matrix* get_mat_rect(struct s_matrix* mat, SDL_Rect* box);
struct s_matrix* get_mat_rect_xN(struct s_matrix* mat, SDL_Rect* box, 
    int zoom);
void propagation(struct s_matrix* partition,size_t i , size_t j);
void propa_symbol(struct s_matrix* mat, size_t i ,
    size_t j, SDL_Rect* box, int* pixels,enum Couleur col_src, enum Couleur col_res);
void propa_note(struct s_matrix* mat, size_t i ,
    size_t j, SDL_Rect* limit, SDL_Rect* box, int* pixels,enum Couleur col_src, enum Couleur col_res);
size_t count_col_symbol(struct s_matrix* mat, SDL_Rect* box);
struct list * list_lines(struct s_matrix* mat);
void repare_lines(struct s_matrix *mat, struct list *list);
struct s_matrix * histo_hori(struct s_matrix* mat);
struct s_matrix * color_lines(struct s_matrix* mat, struct list* list);
struct s_matrix * delete_lines(struct s_matrix* mat, struct list* list);
struct s_matrix* color_graph(struct s_matrix* mat, struct list* list);
struct s_matrix * delete_vert_graph(struct s_matrix * mat, struct list * list);
void restore_rect(struct s_matrix *mat,SDL_Rect * box);
//struct list * create_list_note(struct s_matrix * mat, SDL_Rect * box);
// Memory Free
void matrix_free();
# endif
