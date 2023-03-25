//
//  list.h
//  c 
// 
//  Created by  saber07 on 31/1/2021.
//

#ifndef list_h 
#define list_h

typedef enum {false, true} bool;

typedef struct ListElement
{ 
    int info;
    struct ListElement *next;
} ListElement, *List;

//prototype

List new_list(void);
bool list_is_empty(List li);
int list_length(List li);
void  print_list (List li);
List push_back_list(List li, int x);
List push_front_list(List li, int x);
List supp_list(List li);
List pop_back_list(List li);
List pop_front_list(List li);
List clear_list(List li);

//prototype++

List insert(List li, int x);
List fusion(List li1, List li2);
List supp_oc(List li, int x);
void eclate(List *li, List *lp, List *lip);

#endif /* list_h */
