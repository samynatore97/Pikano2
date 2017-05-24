# include "list.h"
void list_init(struct list *list)
{
   list->next = NULL;
}

int list_is_empty(struct list *list)
{
  return list->next == NULL;
}

size_t list_len(struct list *list)
{
  size_t len = 0;
  while(list->next) {
    list = list->next;
    len+=1;

    }
  return len;
}

void list_push_front(struct list *list, struct list *elm){
elm->next = list->next;
list->next = elm;
}

struct list* list_pop_front(struct list  *list){
  if (list_is_empty(list))
     return NULL;
  struct list *res = list->next;
  list->next = list->next->next;
  free(list);
  return res;
}

struct list* list_find(struct list* list,void * value){
  while(list){
  if (list->data == value){
     return list;
    }
  list = list->next;
  }
  return list;
}

int list_is_sorted(struct list *list)
{
  while(list->next && list->next->data < list->next->next->data)
    list= list->next;
  if (list->next == NULL)
    return 1;
  else
    return 0;
}

void list_insert(struct list *list, struct list *elm)
{
  while(list->next && list->next->data < elm->data)
    list = list->next;
  list_push_front(list, elm);
}
void print_list_int(struct list * list)
{
	struct list * ptr = list->next;
	while (ptr!= NULL)
	{
		size_t tmp = *(size_t *)(ptr->data);
		printf("%zu\n",tmp);
		ptr= ptr->next;
	}
}
