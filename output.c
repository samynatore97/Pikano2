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

void create_file_symbol(char* filename, struct partition* partition, 
     struct s_matrix* mat, SDL_Surface* window)
{
	FILE* file = fopen(filename, "w");
	if (file == NULL)
	{
		fprintf(stderr, "Impossible d'ouvrir le fichier %s\n", filename);
		return;
	}
	for(size_t i = 0; i< partition->taille; i++)
  {
    struct list* ptr = partition->portees[i]->symboles->next;
    while(ptr != NULL)
    {
      struct symbol* ptr_data = (struct symbol*)ptr->data;
      SDL_Surface* image =
        genImgFromMat(get_mat_rect_xN(mat, ptr_data->box, 4));
      display_image(window,image);
      printf("Type symbole : ");
      scanf("%d", (int*)&(ptr_data->typeNote));
      fprintf(file,"%f %f %f %zu %d\n", ptr_data->nbPasV, ptr_data->nbPasH,
      	  ptr_data->nbPixelNoir, ptr_data->nbCol, ptr_data->typeNote);
      SDL_FillRect(image, NULL, SDL_MapRGB(window->format, 0, 0, 0));
      display_image(window, image);
      ptr = ptr->next;
    }
  }
  fclose(file);
}

void append_file_note(char* filename, struct note* note, 
     struct s_matrix* mat, SDL_Surface* window)
{
	FILE* file = fopen(filename, "a");
	if (file == NULL)
	{
		fprintf(stderr, "Impossible d'ouvrir le fichier %s\n", filename);
		return;
	}
  SDL_Surface* image = genImgFromMat(get_mat_rect_xN(mat, 
    note->symb_note->box, 4));
  display_image(window,image);
  printf("Type note : ");
  scanf("%d", (int*)&(note->t_note));
  fprintf(file,"%f %f %f %d\n", note->symb_note->nbPasV,
    	note->symb_note->nbPasH, note->symb_note->nbPixelNoir, 
    	(int)note->t_note);
  SDL_FillRect(image, NULL, 
  		SDL_MapRGB(image->format, 0, 0, 0));
  display_image(window, image);
  fclose(file);
}

struct list * genListFromFile(char * path, char * type)
{
	FILE * fichier = NULL;
	fichier = fopen(path,"r");
	struct list * res = malloc(sizeof(struct list));
	list_init(res);
	char line[128];
	while (fgets(line, sizeof(line), fichier)!= NULL)
	{
		struct list *elm = malloc(sizeof(struct list));
		if(strcmp(type, "SYMB") == 0)
		{
			struct symbol * ptr = malloc(sizeof(struct symbol));
			int r;
			r = sscanf(line,"%f %f %f %zu %d", &(ptr->nbPasV),
        &(ptr->nbPasH), &(ptr->nbPixelNoir),
        &(ptr->nbCol),(int*)&(ptr->typeNote));
			if (r != 5)
				err(3,"error while writing");
      elm->data = ptr;
		}
		else 
		{
			struct note* ptr = malloc(sizeof(struct note));
      ptr->symb_note = malloc(sizeof(struct symbol));
			int r;
			r = sscanf(line,"%f %f %f %d", &(ptr->symb_note->nbPasV),
        &(ptr->symb_note->nbPasH), &(ptr->symb_note->nbPixelNoir),
        (int*)&(ptr->t_note));
			if (r != 4)
				err(3,"error while writing");
      elm->data = ptr;
		}
    list_push_front(res, elm);
	}
	fclose(fichier);
	return res ; 
}

void save_neurons(char* filename,struct neurone** n, size_t* topologie,
    size_t nb_couches)
{
  size_t i, j, k;
  FILE* file = NULL;
  file = fopen(filename, "w");
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

void load_neurons(char* filename, struct neurone** n, size_t* topologie, size_t nb_couches)
{
  size_t i, j, k;
  FILE* file = NULL;
  file =fopen(filename, "r");
  if (file == NULL)
    errx(3, "Couldn't open file %s\n",filename);
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

enum Type_N get_type_note(struct neurone** n, size_t* topologie,
    size_t nb_couches, struct note* note)
{
  double* valeurs = malloc(sizeof(double) * 3);
  valeurs[0] = note->symb_note->nbPasV;
  valeurs[1] = note->symb_note->nbPasH;
  valeurs[2] = note->symb_note->nbPixelNoir;
  active_entree(n, valeurs, 3);
  propage_vers_l_avant(n, topologie, nb_couches);
  return (enum Type_N) evalue(n, topologie, nb_couches);
}

void save_img(SDL_Surface * img,char * name)
{
  //FILE * fichier = fopen(name, "w+");
  SDL_SaveBMP(img, name);
}

const char* symb_to_string(enum Type type)
{
  switch(type)
  {
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
    case INCONNU:
      return "inconnu";
    default:
      return "inconnu";
  }
}

const char* value_to_string(enum Note value)
{
  switch(value)
  {
    case DO:
      return "do";
    case RE:
      return "ré";
    case MI:
      return "mi";
    case FA:
      return "fa";
    case SOL:
      return "sol";
    case LA:
      return "la";
    case SI:
      return "si";
    default:
      return "inconnu";
  }
}

