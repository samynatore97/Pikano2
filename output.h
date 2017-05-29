# ifndef _OUTPUT_H
# define _OUTPUT_H
# include <stdlib.h>
# include "symbol.h"
# include "perceptron.h"
void replace_extension(char* filename);
void create_file_symbol(char* filename, struct partition* partition, 
    struct s_matrix* mat, SDL_Surface* window);
void create_file_note(char* filename, struct partition* partition,
		struct s_matrix* mat, SDL_Surface* window);
void append_file_note(char* filename, struct note* note, 
    struct s_matrix* mat, SDL_Surface* window);
struct list* genListFromFile(char* path, char* type);
void save_neurons(char* filename, struct neurone** n, 
		size_t* topologie, size_t nb_couches);
void load_neurons(char* filename, struct neurone** n, size_t* topologie, 
    size_t nb_couches);
enum Type get_type_symb(struct neurone** n, size_t* topologie, 
    size_t nb_couches, struct symbol* symbol);
enum Type_N get_type_note(struct neurone** n, size_t* topologie,
    size_t nb_couches, struct note* note);
const char* symb_to_string(enum Type type);
const char* value_to_string(enum Note value);
void generate_txt(char * path, struct note * note);
void generate_string_note(struct note * note,char * res);
void generate_space(char *path, struct symbol * symbol);
void test(char * path);
void save_img(SDL_Surface * img,char * name);
# endif
