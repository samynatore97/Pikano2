#ifndef PERCEPTRON_H_
#define PERCEPTRON_H_
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

struct neurone
{
  double *w; // poids
  double sortie;
  double erreur;
};

#define SEUIL_APP 0.0001 /* Erreur moyenne tolérée en sortie */
#define SEUIL     0.5 /* Seuil pour vrai/faux */

#define  MAX_ITER2  100000
#define EPSILON 0.1 /* Pas d'apprentissage */

int alloc_reseau(struct neurone*** n, size_t* topologie, 
    size_t nb_couches);
void init_poids(struct neurone** n, size_t* topologie, 
    size_t nb_couches);
void active_entree(struct neurone** n, double* exemple,
    size_t nb_entree);
void propage_vers_l_avant(struct neurone** n, size_t* topologie,
    size_t nb_couches);
int evalue(struct neurone** n, size_t* topologie, 
    size_t nb_couches);
void lance_apprentissage(struct neurone **n, double **exemples, 
    size_t nb_ex, size_t *topologie, size_t nb_couches);
void generalisation(struct neurone **n, double **exemples, 
    size_t nb_ex, size_t *topologie, size_t nb_couches);
#endif
