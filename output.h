# ifndef _OUTPUT_H
# define _OUTPUT_H
# include <stdlib.h>
# include <stdio.h>
# include "list.h"
# include "symbol.h"
# include "perceptron.h"
# define LINE 1000
void replace_extension(char* filename);
struct list* genListFromFile(char* path);
void save_neurons(struct neurone** n, 
    size_t* topologie, size_t nb_couches);
void load_neurons(struct neurone** n, size_t* topologie, 
    size_t nb_couches);
enum Type get_type_symb(struct neurone** n, size_t* topologie, 
    size_t nb_couches, struct symbol* symbol);
const char* to_string(enum Type type);
void save_img(SDL_Surface * img,char * name);
# endif
