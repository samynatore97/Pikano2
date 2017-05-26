# include <err.h>
# include <stdio.h>

# include "matrix.h"

void print_matrix(struct s_matrix *mat)
{
	for(size_t i = 0 ; i<mat->lines;i++)
	{
		size_t line_offset = i *mat->cols;
		for (size_t j = 0 ; j<mat->cols;j++)
			printf("| %d ",(int) mat->data[line_offset+j]);			
		printf("|\n");
	}
}

// Init Matrix
void matrix_init(struct s_matrix *mat, size_t lines, size_t columns)
{
	mat->lines = lines;
	mat->cols = columns;
	mat->data = malloc(lines * columns * sizeof(int));
	for (size_t i = 0 ; i < lines ; i++)
		for(size_t j = 0 ; j <columns ; j++)
			mat->data[i * columns + j]= BLANC;
}

// Copy Matrix
struct s_matrix *matrix_copy(struct s_matrix *src)
{
	struct s_matrix *cpy;
	if(src != NULL )
	{
		cpy = malloc(sizeof(struct s_matrix));
		matrix_init(cpy,src->lines, src->cols);
		for(size_t i = 0 ; i< cpy->lines; i++)
		{
			for(size_t j = 0 ; j<cpy->cols;j++)
			{
				cpy->data[i * cpy->cols +j] =src->data[i * src->cols + j];
			}
		}
	}
	else
		errx(1,"Could not copy matrix (NULL)");
	return cpy;
}

void  draw_rect(struct s_matrix *mat, SDL_Rect* box )
{
	for (int i = box->x; i <= box->x + box->h ;i++)
  {
		mat->data[i*mat->cols+ box->y] = VERT ;
		mat->data[i*mat->cols + box->y + box->w] = VERT ;
  }
	for (int j = box->y ;j<= box->y + box->w ;j++)
	{
		mat->data[box->x*mat->cols + j] = VERT;
		mat->data[(box->x+box->h)*mat->cols + j] = VERT ;
	}
}

struct s_matrix* get_mat_rect(struct s_matrix* mat, SDL_Rect* box)
{
  struct s_matrix* res = malloc(sizeof(struct s_matrix));
  matrix_init(res, box->h, box->w);
  for(size_t i = 0; i < res->lines; i++)
    for(size_t j = 0; i <res->cols; j++)
      res->data[i*res->cols +j] = 
        mat->data[(i+box->x)*mat->cols + j + box->y];
  return res;
}

struct s_matrix* get_mat_rect_xN(struct s_matrix* mat, SDL_Rect* box, 
    int zoom)
{
	struct s_matrix* res = malloc(sizeof(struct s_matrix));
	matrix_init(res, box->h*zoom, box->w*zoom);
	for(size_t i = 0 ; i < res->lines;i+=zoom)
		for (size_t j = 0 ; j < res->cols; j+=zoom)
		{
			res->data[i*res->cols + j] =
         mat->data[(i/zoom + box->x)*mat->cols + (j/zoom + box->y)];
      for(int k = 1; k < zoom; k++)
      {
        res->data[(i+k)*res->cols + j] =
        res->data[i*res->cols + j+k] =
        res->data[(i+k)*res->cols + j +k] =
        mat->data[(i/zoom + box->x)*mat->cols + (j/zoom + box->y)];
      }
		}
	return res ; 
}

struct s_matrix* color_lines(struct s_matrix* mat, struct list* list)
{
  struct list* ptr  = list->next;
	while(ptr != NULL)
	{
		for(size_t j = 0 ; j <mat->cols;j++)
			{
				if(mat->data[*((size_t *)(ptr->data))*mat->cols+j] 
            == NOIR)
				   mat->data[*((size_t *)(ptr->data))*mat->cols+j] 
             = BLEU1;
			}
		ptr = ptr->next;
	}
  return mat;
}

void repare_lines(struct s_matrix* mat, struct list *lst_lines)
{
  struct list *ptr = lst_lines->next;
  while (ptr != NULL)
  {
    for (size_t j = 0; j < mat->cols; j++)
      mat->data[*(size_t*)ptr->data*mat->cols + j] = NOIR;
    ptr = ptr->next;
  }
}

struct list * list_lines(struct s_matrix* mat)
{
	struct list  *lst_lines = malloc(sizeof(struct list));
	list_init(lst_lines);
	for (size_t i = 0 ; i<mat->lines; i++)
	{
		 size_t cpt = 0;
		 
		 for(size_t j = 0 ; j<mat->cols;j++)
		   if(mat->data[i*mat->cols + j] == NOIR)
			   cpt++;

		 if (cpt>= mat->cols/2)
		 {
			 struct list * elm = malloc(sizeof(struct list));
			 size_t * tmp = malloc(sizeof(size_t));
			 *tmp = i;
			 elm->data = tmp;
			 list_push_front(lst_lines,elm);
     } 
  }
  return lst_lines;
}

struct s_matrix* histo_hori(struct s_matrix* mat)
{
	struct s_matrix* res;
	res = malloc(sizeof(struct s_matrix));
	matrix_init(res,mat->lines,mat->cols);
	for(size_t i = 0 ; i<mat->lines; i++)
	{
		size_t k = 0;
		for (size_t j = 0 ; j<mat->cols; j++)
		{
			if (mat->data[i*mat->cols + j] == NOIR)
			{
				res->data[i*res->cols + k] = NOIR ;
				k++;
			}

		}	
	}
	return res;
}

void propagation(struct s_matrix *mat,size_t i , size_t j)
{
	if (mat->data[i*mat->cols+j] == NOIR )
	{
		mat->data[i*mat->cols+j] = ROUGE ;
		if(i < mat->lines - 1)
		propagation(mat,i+1,j);
		if(i > 0)
		propagation(mat,i-1,j);
		if(j < mat->cols - 1)
		propagation(mat,i,j+1);
		if(j > 0)
		propagation(mat,i,j-1);
	}

}

void propa_symbol(struct s_matrix* mat,
    size_t i, size_t j, SDL_Rect* box, int* pixels,enum Couleur col_source, enum Couleur col_res)
{
  if (mat->data[i*mat->cols+j] == col_source)
  {
    mat->data[i*mat->cols+j] = col_res;
    (*pixels)++;
    if (box->x > (int) i 
          || box->x < (int) i - box->h + 1)
      box->h++;
    box->x = MIN((int)i, box->x);
    if (box->y > (int) j
        || box->y < (int) j - box->w + 1)
      box->w++;
    box->y = MIN((int)j, box->y);
    if (i < mat->lines - 1)
      propa_symbol(mat, i+1, j, box, pixels,col_source, col_res);
    if (i > 0)
      propa_symbol(mat, i-1, j, box, pixels, col_source, col_res);
    if (j < mat->cols - 1)
      propa_symbol(mat, i, j+1, box, pixels, col_source, col_res);
    if (j > 0)
      propa_symbol(mat, i, j-1, box, pixels, col_source, col_res);
  }
}

void propa_note(struct s_matrix* mat, size_t i, size_t j, SDL_Rect* limit, SDL_Rect* box, 
	int* pixels,enum Couleur col_source, enum Couleur col_res)
{
  if (mat->data[i*mat->cols+j] == col_source)
  {
    mat->data[i*mat->cols+j] = col_res;
    (*pixels)++;
    if (box->x > (int) i 
          || box->x < (int) i - box->h + 1)
      box->h++;
    box->x = MIN((int)i, box->x);
    if (box->y > (int) j
        || box->y < (int) j - box->w + 1)
      box->w++;
    box->y = MIN((int)j, box->y);
    if (i < limit->x + limit->h - 1)
      propa_note(mat, i+1, j, limit, box, pixels,col_source, col_res);
    if (i > limit->x)
      propa_note(mat, i-1, j, limit, box, pixels, col_source, col_res);
    if (j < limit->y + limit->w - 1)
      propa_note(mat, i, j+1, limit, box, pixels, col_source, col_res);
    if (j > limit->y)
      propa_note(mat, i, j-1, limit,box, pixels, col_source, col_res);
  }
}

size_t count_col_symbol(struct s_matrix* mat, SDL_Rect* box)
{
  size_t nbcols = 0;
  int cpt;
  for(int j = box->y; j < box->y + box->w; j++)
  {
    for ( int i = box->x; i <= box->x + (box->h + 1)/ 2; i++)
    {
      cpt = 0;
      int u = i;
      while(mat->data[i*(int)mat->cols + j] == VERT)
      {
        i++;
        cpt++;
      }
      if (cpt > box->h / 2)
      {
        for(int v = u; v < u+cpt; v++)
          mat->data[v*mat->cols + j] = BLEU1;
        nbcols++;
      }
    }
  }
  return nbcols;
}

struct s_matrix * delete_lines(struct s_matrix * mat, struct list * list)
{
	struct list * ptr = list->next; 
	while (ptr != NULL)
	{
		for (size_t j = 0 ; j < mat->cols; j++)
		{
			if (mat->data[((*(size_t *)(ptr->data)) + 1) *mat->cols+j] == ROUGE
          || mat->data[((*(size_t *)(ptr->data)) -1) * mat->cols + j ] == ROUGE)
					  mat->data[(*(size_t * )ptr->data)*mat->cols +j ] = ROUGE ;
			else
			{
			  mat->data[(*(size_t *)ptr->data) * mat->cols + j ] = BLANC ;
			}
		}
		ptr = ptr->next ;
	}
	return mat ;
}

struct s_matrix* color_graph(struct s_matrix* mat, struct list* lst)
{
  
	struct list* ptr = lst->next;
  while(ptr != NULL)
	{
		for (size_t j = 0 ; j<mat->cols;j++)
		{
	  			propagation(mat,*(size_t*)ptr->data+1,j);
					propagation(mat,*(size_t*)ptr->data-1,j);
		}
    ptr = ptr->next;
	}
	struct s_matrix * res ;
	res = delete_lines(mat, lst);

	return res ;
}
//Remplace les pixels bleus contenues dans les boxs par des pixels verts  
void restore_rect(struct s_matrix *mat,SDL_Rect* box)
{
	for (int i = box->x ; i<box->x +box->h;i++)
		for(int j = box->y ; j < box->y +box->w;j++)
		{
			if (mat->data[i*mat->cols+j]== BLEU1)
			{
				mat->data[i*mat->cols+j] = VERT;
			}
		}
}

// Free Matrix 
void matrix_free(struct s_matrix *mat)
{
	if(mat != NULL)
	{
	  if(mat->data != NULL)
		{
			free(mat->data);
			free(mat);
		}
  }
	else
		errx(1,"Could not free matrix (NULL)");
}

/*struct list * create_list_note(struct s_matrix * mat, SDL_Rect * box box)
{
	struct list * res = malloc(sizeof(struct list));
	list_init(res);
	for(int i = box->x ; i< box->x + box->h; i++)
	{
		for(int j = box->y; j < box->y + box->w; j++)
		{
			if (mat->data[i* mat->cols + j] == VERT)
			{
				SDL_rect * rect = malloc(sizeof(SDL_Rect));
				propa_symbol(mat, i, j, rect , &0, VERT, ROSE);
				struct list * elm = malloc(sizeof(struct list));
				elm->data = rect;
				list_push_front(res,elm);
			}
		}
	}
	return res;
}*/
