# include "symbol.h"
# include <unistd.h>

void print_symbol(struct symbol *symbol)
{
  printf("\nbox(%d,%d) w: %d, h:%d \n",symbol->box->x, symbol->box->y,
      symbol->box->w, symbol->box->h);
	printf("Nombre vertical de pas : %f \n", symbol->nbPasV);
  printf("Nombre horizontal de pas : %f \n", symbol->nbPasH);
	printf("Ratio PixelNoir/Pixelblanc: %f \n",symbol->nbPixelNoir);
	//printf("Nombre de barres verticales : %zu \n", symbol->nbCol);
	//printf("Type de note : %d \n \n", (int)symbol->typeNote); 
}

void print_list_symbol(struct list * list_symbol)
{
	while(list_symbol->next!=NULL)
	{
		struct symbol * symbol = (struct symbol *)list_symbol->data;
		print_symbol(symbol);
		printf("\n");
		list_symbol = list_symbol->next;
	}
}

struct symbol * symbol_init(size_t x, size_t y)
{
	struct symbol * ptr_symbol = malloc(sizeof(struct symbol));
  ptr_symbol->box = malloc(sizeof(SDL_Rect));
	ptr_symbol->box->x = x;
  ptr_symbol->box->y = y;
  ptr_symbol->box->w = 1;
  ptr_symbol->box->h = 1;
	return ptr_symbol;
}

size_t* size_list_to_tab(struct list* list, size_t* taille)
{
  *taille = list_len(list);
  size_t* tab = malloc((*taille) * sizeof(size_t));
  for (size_t i = 0; i <*taille ; i++)
  {
    tab[*taille - i - 1] = *(size_t*)list->next->data;
    list = list->next;
  }
  return tab;
}

void list_insert_symbol(struct list *list, struct list *elm)
{
  while(list->next != NULL
      && ((struct symbol*)list->next->data)->box->y < 
        ((struct symbol*)elm->data)->box->y)
    list = list->next;
  list_push_front(list, elm);
}

void list_insert_note(struct list *list, struct list *elm)
{
  while(list->next != NULL
      && ((struct note*)list->next->data)->symb_note->box->y < 
        ((struct note*)elm->data)->symb_note->box->y)
    list = list->next;
  list_push_front(list, elm);
}

void clean_list_symbol(struct s_matrix * mat, struct list* list)
{
  struct list * ptr = list->next;
  while(ptr->next->next != NULL)
  {
    if (is_rect_in(((struct symbol*)ptr->next->data)->box,
          ((struct symbol*)ptr->data)->box) == 1)
    {
			restore_rect(mat,((struct symbol *)ptr->next->data)->box);
      struct list* tmp = ptr->next;
      ptr->next = ptr->next->next;
      free(tmp);
      continue;
    }
    ptr = ptr->next;
  }
}

struct symbol * create_symbol( struct s_matrix* mat, size_t i, size_t j,
    size_t pas, enum Couleur col_source,enum Couleur col_res)
{
  int nb_pixels = 0;
  struct symbol * ptr_symbol = symbol_init(i, j);
  propa_symbol(mat, i, j, ptr_symbol->box, &nb_pixels,col_source, col_res);
  ptr_symbol->nbPasV = (1.0 * ptr_symbol->box->h) 
    / (1.0 * pas);
  ptr_symbol->nbPasH = (1.0 * ptr_symbol->box->w)
    / (1.0 * pas);
  ptr_symbol->nbPixelNoir = (float)nb_pixels
    / (float)(ptr_symbol->box->h * ptr_symbol->box->w);
  ptr_symbol->nbCol = count_col_symbol(mat, ptr_symbol->box);
  return ptr_symbol;
}

struct partition * analyse(struct s_matrix* mat, struct list* histo)
{
  size_t i, cpt;
  /*
   * Détermination de la hauteur en pixels d'une ligne de portée
   */
  size_t list_lg;
  size_t* tab_lignes = size_list_to_tab(histo, &list_lg);
  size_t max_h = 0;
  for (i = 0; i < list_lg - 1; i++)
  {
    cpt = 1;
    while (tab_lignes[i] == tab_lignes[i+1] - 1)
    {
      i++;
      cpt++;
    }
    if (cpt > max_h)
      max_h = cpt;
  }

  struct partition* partition = malloc(sizeof(partition));
  partition->h_ligne = max_h;
  
  /* 
   * Détermination de la hauteur en pixels de interligne des portées
   */
  partition->i_ligne = tab_lignes[max_h] - tab_lignes[0];

  /* 
   * Création de la liste des portées
   */
  partition->taille = list_lg / (5*max_h);
  partition->portees = malloc(sizeof(struct portee*) 
      * partition->taille);
  i = 0;
  cpt = 0;
  while ( i < list_lg)
  {
    struct portee* ptr_portee = malloc(sizeof(struct portee));
    ptr_portee->symboles = malloc(sizeof(struct list));
    list_init(ptr_portee->symboles);	
    for (size_t j = 0; j < 5; j++)
    {
      ptr_portee->tab_lignes[j] = tab_lignes[i];
      for(size_t k = 0; k < mat->cols; k++)
      {
        if (mat->data[tab_lignes[i]*mat->cols + k] == ROUGE)
        {
          struct list * ptr_elm = malloc(sizeof(struct list));
          ptr_elm->data = create_symbol(mat, tab_lignes[i], k, 
              partition->i_ligne, ROUGE, VERT);
          list_insert_symbol(ptr_portee->symboles, ptr_elm);
        } 
      }
      if (j < 4)
      {
        for(size_t l = tab_lignes[i] ; l < tab_lignes[i+max_h]; l++)
          for(size_t k = 0; k < mat->cols; k++)
          {
            if (mat->data[l*mat->cols + k] == NOIR)
            {
              struct list * ptr_elm = malloc(sizeof(struct list));
              ptr_elm->data = create_symbol(mat, l, k, partition->i_ligne,
                  NOIR, VERT);
              list_insert_symbol(ptr_portee->symboles, ptr_elm);
            }
          }
      }
      i += max_h;
    }
    clean_list_symbol(mat, ptr_portee->symboles);
    partition->portees[cpt++] = ptr_portee;
  }
  return partition;
}

enum Note find_height_box(struct partition * partition,
    struct portee * portee, SDL_Rect * box)
{
	enum Note res;
  if (box->x + box->h + partition->h_ligne < portee->tab_lignes[0])
  {
    int up = 1;
    while (box->x + box->h + up * partition->i_ligne/4 < 
        portee->tab_lignes[0])
      up++;
    if (portee->cle == SOL)
    {
      switch(up)
      {
        case(1):
          res = SOL;
          break;
        case(2):
          res = LA;
          break;
        case(3):
          res = SI;
          break;
        case(4):
          res = DO;
          break;
      }
    }
    else
    {
      switch(up)
      {
        case(1):
          res = SI;
          break;
        case(2):
          res = DO;
          break;
        case(3):
          res = RE;
          break;
        case(4):
          res = MI;
          break;
      }
    }
  }
  else
  {
	  for(size_t i = 0 ; i < 5; i++)
	  {
	  	if(box->x + box->h + partition->h_ligne - portee->tab_lignes[i]
          < partition->i_ligne/4)
	  	{
		  	if(portee->cle == SOL)
		  	{
			  	switch(i)
				  {
					  case(1):
						  res = SI;
						  break;
					  case(2):
						  res = DO;
						  break;
					  case(3):
						  res = LA;
						  break;
            case(4):
						  res = FA;
						  break;
				  }
			  }
			  else
			  {
				  switch(i)
				  {
					  case(1):
					  	res = SOL;
						  break;
					  case(2):
						  res = MI;
						  break;
					  case(3):
						  res = DO;
						  break;
					  case(4):
						  res = LA;
						  break;
				  }
			  }
		  }
		  else
		  {
			  if(portee->cle == SOL)
			  {
				  switch(i)
					{
						case(0):
							res = FA;
							break;
						case(1):
							res = RE;
							break;
						case(2):
							res = SI;
							break;
					  case(3):
							res = SOL;
							break;
						case(4):
							res = MI;
							 break;
					}
				}
			  else
			  {
				  switch(i)
			  	{
				  	case(0):
					  	res = LA;
						  break;
					  case(1):
						  res = FA;
					  	break;
				  	case(2):
					  	res = RE;
						  break;
					  case(3):
						  res = SI;
						  break;
					  case(4):
						  res = SOL;
						  break;
				  }
			  }
		  }
    }
  }
	return res;
}

struct note * init_note(size_t x, size_t y)
{
	struct note * note = malloc(sizeof(struct note));
  note->symb_note = malloc(sizeof(struct symbol));
  note->symb_note->box = malloc(sizeof(SDL_Rect));
	note->symb_note->box->x = x;
  note->symb_note->box->y = y;
  note->symb_note->box->w = 1;
  note->symb_note->box->h = 1;
	return note;
}

struct note * create_note(struct s_matrix * mat, size_t i, size_t j,
    struct symbol* symbol, size_t pas,
    enum Couleur clr_src, enum Couleur clr_res)
{
  int nb_pixels = 0;
  struct note * note = init_note(i,j);
  propa_note(mat, i, j, symbol->box, note->symb_note->box, &nb_pixels,
      clr_src, clr_res);
  note->symb_note->nbPasV = (1.0 * note->symb_note->box->h) 
    / (1.0 * pas);
  note->symb_note->nbPasH = (1.0 * note->symb_note->box->w)
    / (1.0 * pas);
  note->symb_note->nbPixelNoir = (float)nb_pixels
    / (float)(note->symb_note->box->h * 
        note->symb_note->box->w);
  return note;
}

struct list * analyse_note(struct s_matrix* mat, 
    struct symbol* symbol, size_t pas)
{
	struct list * list_notes = malloc(sizeof(struct list));
  list_init(list_notes);
  for (size_t i = symbol->box->x; i < symbol->box->x + symbol->box->h;
      i++)
    for (size_t j = symbol->box->y; j < symbol->box->y + symbol->box->w;
        j++)
    {
	    if (mat->data[i*mat->cols + j] == VERT)
      {
        struct list* elm = malloc(sizeof(struct list));            
        elm->data = create_note(mat, i, j, symbol, pas, VERT, ROUGE);
        list_insert_note(list_notes, elm);
      }
    }
  return list_notes;
}
