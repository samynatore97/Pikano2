# include "reseau.h"

void usage()
{
	printf("Erreur dans l'entrée...\n");
	printf("Usage : reseau SYMB || NOTE filename nb_couches topologie\n");
}

int main(int argc, char* argv[])
{
  if (argc < 5)
  {
    usage();
    return EXIT_FAILURE;
  }
	char * type = argv[1];
	char * filename = argv[2];
  FILE* file = NULL;
	if (strcmp(type, "SYMB") == 0 || strcmp(type, "NOTE") == 0)
	{
    file = fopen(filename, "r");
		if (file == NULL)
		{
			printf("Impossible de lire le fichier %s\n", filename);
			usage();
			return EXIT_FAILURE;
		}
	}
	else
		usage();
		
  size_t nb_couches;
	if (sscanf(argv[3], "%zu", &nb_couches) != 1)
  {
    usage();
    fclose(file);
    return EXIT_FAILURE;
  }
  
  size_t *topologie = malloc(sizeof(size_t) * nb_couches);
  for (size_t i = 0; i < nb_couches; i++)
  {
    if (sscanf(argv[4+i], "%zu", topologie+i) != 1)
    {
      usage();
      fclose(file);
      return EXIT_FAILURE;
    }
  }
  
  struct neurone **reseau;
  if (alloc_reseau(&reseau, topologie, nb_couches) == -1)
  {
    printf("Allocation impossible\n");
    fclose(file);
    return EXIT_FAILURE;
  }
  init_poids(reseau, topologie, nb_couches);
	struct list * list_exemples = genListFromFile(filename, type);
	size_t i = 0;
  size_t nb_exemples = list_len(list_exemples);
  double **tab_exemples = malloc(sizeof(double*) * nb_exemples); 
  for (i=0; i < nb_exemples; i++)
    tab_exemples[i] = malloc(sizeof(double) * 5);
	struct list * ptr = list_exemples->next;
  i=0;
	while (ptr != NULL)
	{
		if (strcmp(type, "SYMB") == 0)
		{
			struct symbol* ptr_data = (struct symbol *)ptr->data;
      tab_exemples[i][0] = ptr_data->nbPasV;
      tab_exemples[i][1] = ptr_data->nbPasH;
			tab_exemples[i][2] = ptr_data->nbPixelNoir;
			tab_exemples[i][3] = ptr_data->nbCol;
			tab_exemples[i][4] = ptr_data->typeNote;
		}
    else
    {
			struct note* ptr_data = (struct note*)ptr->data;
      tab_exemples[i][0] = ptr_data->symb_note->nbPasV;
      tab_exemples[i][1] = ptr_data->symb_note->nbPasH;
			tab_exemples[i][2] = ptr_data->symb_note->nbPixelNoir;
			tab_exemples[i][4] = ptr_data->t_note;    	    
		}
		i++;
		ptr = ptr->next;
	}
  char rep = 'n';
  do
  {
    lance_apprentissage(reseau, tab_exemples, 
    nb_exemples, (size_t*) topologie, nb_couches);
    generalisation(reseau, tab_exemples, 
    nb_exemples, (size_t*)topologie, nb_couches);
    printf("\nSauvegarde du réseau ? (o/n) ");
  } while((rep = getchar()) != 'o');
  if (strcmp(type, "SYMB") == 0)
    save_neurons("reseau_symb.txt",reseau, 
        (size_t*)topologie, nb_couches);
  else
    save_neurons("reseau_note.txt", reseau, 
        (size_t*)topologie, nb_couches);
  fclose(file);
  return EXIT_SUCCESS;
}
