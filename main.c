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
    SDL_Surface *img1 = genImgFromMat(mat_histo);
    save_img(img1,"demonstration/histogramme1");
	  //display_image(genImgFromMat(mat_histo));
    //sleep(5);
    struct list *lst_lines = list_lines(mat_histo);
   //repare_lines(mat, lst_lines);
	  struct s_matrix* mat_color_lines = color_lines(mat, lst_lines);
	  SDL_Surface * img2 = genImgFromMat(mat_color_lines);
	  save_img(img2,"demonstration/coloration2");
    //display_image(genImgFromMat(mat_color_lines));
	  //sleep(5);
	  struct s_matrix *prey = color_graph(mat_color_lines, lst_lines);
    struct partition * partition = analyse(prey, lst_lines);
    SDL_Surface *im = genImgFromMat(prey);
    save_img(im,"demonstration/grapheme_identification3");

    size_t nb_couches_s = 5;
    size_t topologie_s[5] = {4,120,90,60,11};
    struct neurone** res_s;
    alloc_reseau(&res_s, topologie_s, nb_couches_s);
    load_neurons("./reseau/reseau_symb.txt", res_s, 
        topologie_s, nb_couches_s);
    size_t nb_couches_n = 4;
    size_t topologie_n[4] = {3, 100, 100, 6};
    struct neurone** res_n;
    alloc_reseau(&res_n, topologie_n, nb_couches_n);
    load_neurons("./reseau/reseau_note.txt", res_n,
        topologie_n, nb_couches_n);
    for(size_t i = 0; i < partition->taille; i++)
    {
      struct portee * portee = partition->portees[i];
      portee->notes = malloc(sizeof(struct list));
      list_init(portee->notes);    
      struct list* ptr = portee->symboles->next;
      while(ptr != NULL)
      {
        struct symbol* symbol = (struct symbol*)ptr->data;
        enum Type type_symb = get_type_symb(res_s, topologie_s,
          nb_couches_s, symbol);
          //printf("Type symbole : %s\n", to_string(type_symb));
          /*SDL_Surface* image =
            genImgFromMat(get_mat_rect_xN(prey, 
                  symbol->box, 4));
          display_image(window,image);
          printf("Type symbole : %s", to_string(type_symb));
          sleep(2);
          SDL_FillRect(image, NULL, SDL_MapRGB(graph->format, 0, 0, 0));
          display_image(window, image);*/

          switch (type_symb)
          {
            case(BARRE):
              break;
            case(CLESOL):
              portee->cle = SOL;
              break;
            case(BEMOL):
              ;
              size_t i = (size_t)find_height_box(partition, portee,
                  symbol->box, NULL);
              portee->bemol[i]= 1;
              break;
            case(QUATRE):
            case(TROIS):
              break;
            case(DSOUPIR):
              break;
            case(NOTE):
              ;
              SDL_Surface* image;
              struct list * list_notes = analyse_note(prey, symbol,
                  partition->i_ligne);
              //printf("Longueur de la liste : %zu\n", list_len(list_notes));
              list_notes = list_notes->next;
              while (list_notes != NULL)
              {
                struct note * note = (struct note*)list_notes->data;
                note->t_note = get_type_note(res_n, 
                 topologie_n, nb_couches_n, note);
                switch(note->t_note)
                {
                  case(NOIRE):
                    note->duree = 1;
                    
                    note->value = find_height_box(partition,portee,
                      note->symb_note->box, note);
                    printf("Valeur de la note : %s%d\n", 
                        value_to_string(note->value), note->octave);
										 generate_txt("partition.txt",note);
                    draw_rect(prey, note->symb_note->box);
                		image = genImgFromMat(prey);
                		display_image(window, image);
                    break;
                  case(BLANCHE):
                    note->duree = 2;
                    note->value = find_height_box(partition,portee,
                      note->symb_note->box, note);
                    printf("Valeur de la note : %s%d\n", 
                        value_to_string(note->value), note->octave);
										 generate_txt("partition.txt",note);                   
                    draw_rect(prey, note->symb_note->box);
                		image = genImgFromMat(prey);
                		display_image(window, image);    
                    break;
                  case(CROCH):
                    break;
                  case(DCROCH):
                    break;
                  case(POINTEE):
                    break;
                  case(INCONNUE):
                    break;
                }
                //draw_rect(prey, note->symb_note->box);
                //SDL_Surface* image = genImgFromMat(prey);
                //display_image(window, image);
                
                
                //struct list * elm = list_notes;
                //list_insert_note(portee->notes, elm);
                //append_file_note("exemples_notes.txt",note, prey, window);
               
                list_notes = list_notes->next;
              }
			        break;
		        case(CLEFA):
			        portee->cle = FA;
              break;
		        case(POINT):
			        break;
		        case(SOUPIR):
		        case(PAUSE):
			        break;
            case(INCONNU):
              break;
	        }
          ptr = ptr->next;
        }
      }
      SDL_Surface *img3 = genImgFromMat(prey);
      save_img(img3, "demonstration/final4");
      //fclose(file);
      free(mat);
      free(mat_histo);
    }
	/*else
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
  }*/
  SDL_Quit();
  return 0; 
}
