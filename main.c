#include "main.h"
#include <unistd.h>

int main(int argc, char *argv[])
{
  if (SDL_Init(SDL_INIT_VIDEO) == -1)
    errx(1, "Could not initialize SDL : %s\n", SDL_GetError());

	if(argc < 2)
		 err(1,"must provide an img");
	else if(argc == 2)
	{
   	char* path = argv[1];
	  SDL_Surface *partition1 = load_image(path);
    SDL_Surface * window = SDL_SetVideoMode(partition1->w,partition1->h,
        32, SDL_SWSURFACE | SDL_RESIZABLE);
    if (window == NULL)
      errx(2, "Couldn't set video mode : %s\n", SDL_GetError());
	  SDL_Surface * partitionBW = 
      generateBlackAndWhiteFromGS(generateGrayScaleImage(partition1));
	  //display_image(partitionBW);
    //sleep(5);
	  struct s_matrix * mat = genMatFromImg(partitionBW);
    struct s_matrix * mat_histo = histo_hori(mat);
	  //display_image(genImgFromMat(mat_histo));
    //sleep(5);
    struct list *lst_lines = list_lines(mat_histo);
   //repare_lines(mat, lst_lines);
	  struct s_matrix* mat_color_lines = color_lines(mat, lst_lines);
    //display_image(genImgFromMat(mat_color_lines));
	  //sleep(5);
	  struct s_matrix *prey = color_graph(mat_color_lines, lst_lines);
	  SDL_Surface *graph = genImgFromMat(prey);

    struct partition * partition = analyse(prey, lst_lines);
    SDL_Surface *im = genImgFromMat(prey);
    save_img(im,"prey");
   /* FILE* file = NULL;
    replace_extension(path);
    file = fopen(path, "w");
    if (file == NULL)
      errx(3, "Couldn't open file %s\n", path);
    for(size_t i = 0; i< partition->taille; i++)
    {
      struct list* ptr = partition->portees[i]->symboles->next;
      while(ptr != NULL)
      {
        struct symbol* ptr_data = (struct symbol*)ptr->data;
        SDL_Surface* image =
          genImgFromMat(get_mat_rect_xN(prey, 
                ptr_data->box, 4));
        display_image(window,image);
        printf("Type symbole : ");
        scanf("%d", 
            &(ptr_data->typeNote));
        fprintf(file,"%f %f %f %zu %d\n", ptr_data->nbPasV, ptr_data->nbPasH,
            ptr_data->nbPixelNoir, ptr_data->nbCol, ptr_data->typeNote);
        SDL_FillRect(image, NULL, SDL_MapRGB(graph->format, 0, 0, 0));
        display_image(window, image);
        ptr = ptr->next;
      }
    }
    fclose(file);*/
    size_t nb_couches = 5;
    size_t topologie[5] = {4,120,90,60,11};
    //struct neurone** reseau = load_neurons(&nb_couches, topologie);
    for(size_t i = 0; i < partition->taille; i++)
    {
      struct list* ptr = partition->portees[i]->symboles->next;
      while(ptr != NULL)
      {
        struct symbol* ptr_data = (struct symbol*)ptr->data;
      // enum Type type_symb = get_type(reseau, topologie, nb_couches, 
        //   ptr_data);
        SDL_Surface* image =
          genImgFromMat(get_mat_rect_xN(prey, 
                ptr_data->box, 4));
        display_image(window,image);
        //printf("Type symbole : %s", to_string(type_symb));
        sleep(2);
        SDL_FillRect(image, NULL, SDL_MapRGB(graph->format, 0, 0, 0));
        display_image(window, image);
        ptr = ptr->next;
      }
    }
    free(mat);
    free(mat_histo);
  }
	else
	{
    char * path = argv[1];
    size_t nb_couches;
		if (sscanf(argv[2], "%zu", &nb_couches) != 1)
    {
      printf("Erreur d'entrée\n");
      exit(EXIT_FAILURE);
    }
    size_t *topologie = malloc(sizeof(size_t) * nb_couches);
    for (size_t i = 0; i < nb_couches; i++)
    {
      if (sscanf(argv[3+i], "%zu", topologie+i) != 1)
      {
        printf("Erreur d'entrée\n");
        exit(EXIT_FAILURE);
      }
    }
  
    struct neurone **reseau;

    if (alloc_reseau(&reseau, topologie, nb_couches) == -1)
    {
      printf("Allocation impossible\n");
      return EXIT_FAILURE;
    }
    init_poids(reseau, topologie, nb_couches);

		struct list * list_exemples = genListFromFile(path);
		size_t i = 0;
    size_t nb_exemples = list_len(list_exemples);
    double **tab_exemples = malloc(sizeof(double*) * nb_exemples); 
    for (i=0; i < nb_exemples; i++)
      tab_exemples[i] = malloc(sizeof(double) * 5);
		struct list * ptr = list_exemples->next;
    i=0;
		while (ptr != NULL)
		{
			struct symbol * ptr_data = (struct symbol *)ptr->data;
			if (ptr_data->typeNote != 12)
			{
        tab_exemples[i][0] = ptr_data->nbPasV;
        tab_exemples[i][1] = ptr_data->nbPasH;
			 	tab_exemples[i][2] = ptr_data->nbPixelNoir;
			  tab_exemples[i][3] = ptr_data->nbCol;
				tab_exemples[i][4] = ptr_data->typeNote;
        i++;
			}
      else
        nb_exemples--;

			ptr = ptr->next;
		}
    char rep = 'n';
    while(rep != 'o')
    {
      lance_apprentissage(reseau, tab_exemples, 
      nb_exemples, (size_t*) topologie, nb_couches);
      generalisation(reseau, tab_exemples, 
        nb_exemples, (size_t*)topologie, nb_couches);
      printf("\nSauvegarde du réseau ? (o/n) ");
      scanf("%c", &rep);
    }
    save_neurons(reseau, (size_t*)topologie, nb_couches);
  }
  SDL_Quit();
  return 0; 
}
