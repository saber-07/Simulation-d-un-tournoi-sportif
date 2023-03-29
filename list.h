//
//  list.h
//  c 
// 
//  Created by  saber07 on 31/1/2021.
//

#ifndef list_h 
#define list_h
#define NAMEZIZE 30
#define BUFFER_SIZE 1024
#define TEAMS 48

typedef enum {false, true} bool;

typedef struct ListElement
{ 
    char name[NAMEZIZE+1];
    int status;
    struct ListElement *next;
} ListElement, *List;

//prototype

List new_list(void);
bool list_is_empty(List li);
int list_length(List li);
void  print_list (List li);
List push_back_list(List li, char x[NAMEZIZE]);
List push_front_list(List li, char x[NAMEZIZE]);
List pop_back_list(List li);
List pop_front_list(List li);
List clear_list(List li);
void eclate(List li, List *lp, List *lip);

#endif /* list_h */
