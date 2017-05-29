#include "perceptron.h"
#include <unistd.h>

/* Allocation du reseau */
/* -------------------- */
/* Les valeurs de topologie sont le nombre de neurones de chaque couche
 * Ex : topologie = {3, 4, 4, 5} couche d'entree : trois neurones
 *                               couche cachee 1 : 4 neurones
 *                               couche cachee 2 : 4 neurones
 *                               couche sortie : 5 neurones
 */

int alloc_reseau(struct neurone ***n, size_t *topologie, size_t nb_couches)
{
	struct neurone **n1;
 	size_t i, j;
	
	/* Allocation des couches */

	if ( (n1 = (struct neurone **) 
        malloc (sizeof(struct neurone*) *(nb_couches))) == NULL)
		return -1;
	
	/* Allocation des neurones */

	for(i=0;i<nb_couches;i++)
		if ( (n1[i] = (struct neurone *) 
          malloc (sizeof (struct neurone) * topologie[i]) ) == NULL )
 		{
			free(n1);
			return -1;
		}

	/* Allocation des poids */
	
	for(i=0;i<nb_couches-1;i++)
		for(j=0;j<topologie[i];j++)
			if ( (n1[i][j].w = (double *) 
            malloc(sizeof(double) * topologie[i+1]) ) == NULL )
			{
				free(n1);
				return -1;
			}

	*n = n1;
	return 0;
}

/* Fonctions d'activation */
/* ++++++++++++++++++++++ */

/* Fonction sigmoide */
/* ----------------- */

float sigmoide(float valeur)
{
	if (fabs(valeur)<1e-10)
		return 0.5;
	else
		return ( 1.0 / ( 1.0 + exp(-valeur) ) );
}

/* Fonction derivee de la sigmoide */
/* ------------------------------- */

float derivee(float valeur)
{
	return ( valeur * (1 - valeur) );
}

/* Fonctions generales */
/* ------------------- */

void init_poids(struct neurone **n, size_t *topologie, size_t nb_couches)
{
	size_t i, j, k;
	int t;
	
	/* Initialisation du generateur aleatoire */

	time((time_t *)&t);
	srand(t);

	/* Sur toutes les couches sauf la derniere */
		/* Sur tous les neurones de la couche */
			/*Pour chaque poids */

	for(i=0;i<nb_couches-1;i++)
		for(j=0;j<topologie[i];j++)
			for(k=0;k<topologie[i+1];k++)
				n[i][j].w[k] = 1.0 * rand() / RAND_MAX * 1.0 - 0.5;
}

/* Apprentissage */
/* +++++++++++++ */

/* Active entree */
/* ------------- */

void active_entree(struct neurone **n, double* exemple, size_t nb_entree)
{
	size_t i;

	/* Pour tous les neurones de la couche d'entree */

	for(i=0;i<nb_entree;i++)
		n[0][i].sortie = exemple[i];
}

/* Propagation vers l'avant */
/* ------------------------ */

void propage_vers_l_avant(struct neurone **n, 
    size_t *topologie, size_t nb_couches)
{
	size_t i, j, k;
	float valeur;

	for(i=1;i<nb_couches;i++)
	{
		for(j=0;j<topologie[i];j++)
		{
			valeur = 0.0;

			/* Somme sur toute les liaisons */

			for(k=0;k<topologie[i-1];k++)
				valeur+=n[i-1][k].w[j] * n[i-1][k].sortie;

			n[i][j].sortie = sigmoide(valeur);
		}
	}
}

/* Calcul de l'erreur finale */
/* ------------------------- */

float calcul_erreur_finale(struct neurone **n, double *exemple, 
    size_t *topologie, size_t nb_couches)
{
	size_t i;
	double erreur_sortie = 0.0;
	double erreur_brute;

	/* Sur tous les neurones de sortie */

	for(i=0;i<topologie[nb_couches-1];i++)
	{
		erreur_brute =  (exemple[topologie[0]] == i+1 ? 1 : 0) 
        - n[nb_couches-1][i].sortie;
		n[nb_couches-1][i].erreur = erreur_brute * derivee(n[nb_couches-1][i].sortie);
		erreur_sortie += 0.5 * erreur_brute * erreur_brute; 	
	}		
	
	return erreur_sortie;
}

/* Propagation de l'erreur vers l'arriere */
/* -------------------------------------- */

void propage_erreur_arriere(struct neurone **n, size_t *topologie, 
  size_t nb_couches)
{
	size_t i, j, k;
	double erreur;
	
	/* Pour tous les neurones de la couche intermediaire */

	for(i=nb_couches-2;i>1;i--)
	{
		for(j=0;j<topologie[i];j++)
		{
			erreur = 0;
			
			/* pour toutes les liaisons */

			for(k=0;k<topologie[i+1];k++)
				erreur+= n[i+1][k].erreur * n[i][j].w[k];
			n[i][j].erreur = erreur * derivee(n[i][j].sortie);
		}
	}
}

/* Ajustement des poids */
/* -------------------- */


void ajuste_poids(struct neurone **n, size_t *topologie, size_t nb_couches)
{
	int i; 
  size_t j, k;

	/* Pour toutes les couches sauf celle de sortie */

	for(i= nb_couches - 2;i>=0;i--)
		for(j=0;j<topologie[i];j++)
			for(k=0;k<topologie[i+1];k++)
				n[i][j].w[k]+= EPSILON * n[i+1][k].erreur * n[i][j].sortie;

				/* Poids += erreur dest * sortie src */
}
 
/* Lancement du reseau */
/* ------------------- */

float calcule(struct neurone **n, double *exemple, 
    size_t *topologie, size_t nb_couches)
{
	double erreur;

 	active_entree(n, exemple, topologie[0]);
	propage_vers_l_avant(n, topologie, nb_couches);
	erreur = calcul_erreur_finale(n, exemple, topologie, nb_couches);
	propage_erreur_arriere(n, topologie, nb_couches);
	ajuste_poids(n, topologie, nb_couches);
	
	return erreur;
}


/* Fonctions de verification */
/* +++++++++++++++++++++++++ */

/* Affichage des poids */
/* ------------------- */

void affiche_poids(struct neurone **n, size_t *topologie, size_t nb_couches)
{
	size_t i, j, k;
	
	for(i=0;i<nb_couches-1;i++)
	{
		printf("Couche No %zu \n", i);
		for(j=0;j<topologie[i];j++)
		{
			printf("  Neurone No %zu \n    ", j+1);
			for(k=0;k<topologie[i+1];k++)
				printf(" %2.4lf ",n[i][j].w[k]);
			printf("\n");
		}
	}
}

/* Affichage des activations */
/* ------------------------- */

void affiche_sortie(struct neurone **n, size_t *topologie, int nb_couches)
{
	//int i;
  size_t j;
	
  //printf("Sortie :\n");
	//for(i=0;i<nb_couches;i++)
	//{
		//printf("Couche No %d \n", i);
		for(j=0;j<topologie[nb_couches-1];j++)
		{
			printf("     Neurone No %zu   %lf\n",
          j+1, n[nb_couches-1][j].sortie);
		}
}

/* Lancement de l'apprentissage */
/* ---------------------------- */

void lance_apprentissage(struct neurone **n, double **exemples, 
    size_t nb_ex, size_t *topologie, size_t nb_couches)
{
	size_t i;
	double erreur;
 	long iter = 0;	

	do
	{
		iter++;
		erreur = 0.0;
	 
 		for(i=0;i<nb_ex;i++)
    {
			erreur += calcule(n, exemples[i], topologie, nb_couches);
      //printf("Entrée : %lf %lf %lf %lf %lf\n", exemples[i][0], exemples[i][1],
        //exemples[i][2], exemples[i][3], exemples[i][4]);
     //affiche_sortie(n, topologie, nb_couches);
     //sleep(5);
    }
    
	}
	while( (erreur>SEUIL_APP * nb_ex) && (iter<(long)MAX_ITER2) );
  	
	printf("\nPhase Apprentissage\n\n");
	printf(" Convergence en %ld iterations pour ecart moyen effectif %lf (%lf) \n"
      , iter, SEUIL_APP, erreur/nb_ex);
	printf("  Ecart moyen %lf pour %zu exemples\n", erreur, nb_ex);
}

/* Verification et calcul des sorties */
/* ++++++++++++++++++++++++++++++++++ */

/* Test sur les exemples */
/* --------------------- */

/* void general_sur_exemple(struct neurone **n, double **exemples,
    size_t *topologie, size_t nb_couches, int nb_app)
{
	size_t i;
	int faux = 0;

	printf("\nPhase de generalisation sur apprentissage\n\n");

	 Pour chaque exemple calcule la sortie 
	
	for(i=0;i<nb_app;i++)
	{
		int sortie;
		int err = 0;
		int j;

 		active_entree(n, exemples[i], topologie[0]);
		propage_vers_l_avant(n, topologie, nb_couches);
		
		 Verifie la sortie reelle et attendue 

		for(j=0;j<topologie[nb_couches-1];j++)
		{
			if (n[nb_couches-1][j].sortie >= SEUIL)
				sortie = 1;
			else
				sortie = 0;
			
			if (exemples[i][topologie[0]+j] != sortie)
				err = 1;
		}
		faux += err;
	}

	printf("  Erreur %d sur %d soit %3.2f \n", faux, nb_app,
      (faux * 100.0) / (nb_app * 1.0)); 
}*/

/* Generalisation */
/* -------------- */

void generalisation(struct neurone **n, double **exemples, size_t nb_ex,
    size_t *topologie, size_t nb_couches)
{
	size_t i;
  int faux = 0;

	printf("\nPhase de generalisation\n\n");

	for(i=0;i<nb_ex;i++)
	{
		int sortie;
		int err = 0;
		size_t k;

 		active_entree(n, exemples[i], topologie[0]);
		propage_vers_l_avant(n, topologie, nb_couches);
    //printf("Entrée : %lf %lf %lf %lf %lf\n", exemples[i][0], exemples[i][1],
      //    exemples[i][2], exemples[i][3], exemples[i][4]);
    //affiche_sortie(n, topologie, nb_couches);		
    //sleep(5);
		/* Verifie la sortie reelle et attendue */

		for(k=0;k<topologie[nb_couches-1];k++)
		{ 
			if (n[nb_couches-1][k].sortie >= SEUIL)
				sortie = 1;
			else
				sortie = 0;
			
			if ((exemples[i][topologie[0]] == k+1 ? 1 : 0) != sortie)
				err = 1;
		}
		faux += err;
	}
	printf("  Nbre d'erreurs :  %d sur %zu soit %3.2f %% \n", faux, nb_ex,
      (faux * 100.0) / (nb_ex * 1.0)); 
}

int evalue(struct neurone** n, size_t* topologie, size_t nb_couches)
{
  double max = 0.0;
  int index = 0;
  for(int i=0; (size_t)i < topologie[nb_couches-1]; i++)
  {
    if (n[nb_couches-1][i].sortie > max)
    {
      max = n[nb_couches-1][i].sortie;
      index = i;
    }
  }
  return index+1;
}
