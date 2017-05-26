# include "output.h"
void replace_extension(char* filename)
{
  size_t i = 0;
  while(filename[i] != '.')
    i++;
  filename[++i]='t';
  filename[++i]='x';
  filename[++i]='t';
  filename[++i]='\0';
}

struct list * genListFromFile(char * path)
{
	FILE * fichier = NULL;
	fichier = fopen(path,"r");
	struct list * res = malloc(sizeof(struct list));
	list_init(res);
	char line[128];
	while (fgets(line, sizeof(line), fichier)!= NULL)
	{
		struct list *elm = malloc(sizeof(struct list));
		struct symbol * ptr_symbol = malloc(sizeof(struct symbol));
		int r ;
		r = sscanf(line,"%f %f %f %zu %d", &(ptr_symbol->nbPasV),
        &(ptr_symbol->nbPasH), &(ptr_symbol->nbPixelNoir),
        &(ptr_symbol->nbCol),(int*)&(ptr_symbol->typeNote));
		if (r != 5)
			err(3,"error while writing");
    elm->data = ptr_symbol;
		list_push_front(res,elm);
	}
	fclose(fichier);
	return res ; 
}

void save_neurons(struct neurone** n, size_t* topologie, size_t nb_couches)
{
  size_t i, j, k;
  FILE* file = NULL;
  file = fopen("reseau.txt", "w");
  if (file == NULL)
    errx(3, "Couldn't open file reseau.txt");
   for(i=0; i <nb_couches-1; i++)
    for(j=0; j<topologie[i]; j++)
    {
      for(k=0; k < topologie[i+1];k++)
        fprintf(file,"%lf ", n[i][j].w[k]);
    }
  fclose(file);
}

void load_neurons(struct neurone** n, size_t* topologie, size_t nb_couches)
{
  size_t i, j, k;
  FILE* file = NULL;
  file =fopen("reseau.txt", "r");
  if (file == NULL)
    errx(3, "Couldn't open file reseau.txt");
  /*char line[LINE] = "";
  fgets(line, LINE, file);
  sscanf(line, "%zu", nb_couches);
  topologie = realloc(topologie,(*nb_couches) * sizeof(size_t));
  fgets(line, LINE, file);
  for (i = 0; i <*nb_couches; i++)
  {
    sscanf(line, "%zu", topologie+i);
    printf("%zu ", topologie[i]);
  }*/
   for(i=0; i <nb_couches-1; i++)
    for(j=0; j < topologie[i]; j++)
    {
      for(k=0; k< topologie[i+1]; k++)
      {
        fscanf(file, "%lf ", &(n[i][j].w[k]));
      }
    }
  fclose(file);
}

enum Type get_type_symb(struct neurone** n, size_t* topologie,
    size_t nb_couches, struct symbol* symbol)
{
  double* valeurs = malloc(sizeof(double) * 4);
  valeurs[0] = symbol->nbPasV;
  valeurs[1] = symbol->nbPasH;
  valeurs[2] = symbol->nbPixelNoir;
  valeurs[3] = symbol->nbCol;
  active_entree(n, valeurs, 4);
  propage_vers_l_avant(n, topologie, nb_couches);
  return (enum Type) evalue(n, topologie, nb_couches);
}

void save_img(SDL_Surface * img,char * name)
{
  //FILE * fichier = fopen(name, "w+");
  SDL_SaveBMP(img, name);
}

const char* to_string(enum Type type)
{
  switch(type)
  {
    case INCONNU:
      return "inconnu";
		case BARRE:
			return "barre de mesure";
    case CLESOL:
      return "clé de sol";
    case BEMOL:
      return "bémol";
    case QUATRE:
      return "quatre";
    case TROIS:
      return "trois";
    case DSOUPIR:
      return "demi-soupir";
    case NOTE:
      return "note ou ensembles de notes";
    case CLEFA:
      return "clé de fa";
    case POINT:
      return "point";
    case SOUPIR:
      return "soupir";
    case PAUSE:
      return "pause";
    default:
      return "inconnu";
  }
}
