//
//  list.c
//  c
// 
//  Created by  saber07 on 31/1/2021.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

List new_list (void){return NULL;}

//--------------------------------------------------------------------------------------------------------------------------

bool list_is_empty(List li)
{
    if(li==NULL)
        return  true;
    else return false;
} 
//--------------------------------------------------------------------------------------------------------------------------

int list_length(List li)
{
    int i=0;
    if (!list_is_empty(li))
    {
        while (li != NULL) {
            i++;
            li=li->next;
        }
    }
    return i;
}
//--------------------------------------------------------------------------------------------------------------------------

List push_back_list(List li, char x[NAMEZIZE], int st)
{
    ListElement *element;
    
    element=malloc(sizeof(*element));
    
    if (element==NULL) {
        fprintf(stderr, "probléme d'allocation dynamyque. \n");
        exit(EXIT_FAILURE);
    }
    
    strcpy(element->name, x);
    element->status=st;
    element->next=NULL;
    
    if(list_is_empty(li)) return element;
    else{
        
        ListElement *temp = li;
    
        while (temp->next != NULL)
            temp=temp->next;
        
        temp->next=element;
        
        return li;
    }
}
//--------------------------------------------------------------------------------------------------------------------------
List push_front_list(List li, char x[NAMEZIZE], int st)
{
    ListElement *element;
    
    element=malloc(sizeof(*element));
    
    if (element==NULL) {
        fprintf(stderr, "probléme d'allocation dynamyque. \n");
        exit(EXIT_FAILURE);
    }
    
    strcpy(element->name, x);
    element->status=st;
    
    if(list_is_empty(li))
        element->next=NULL;
    else
        element->next=li;
    
    return element;
}
//--------------------------------------------------------------------------------------------------------------------------

List pop_back_list(List li)
{
    if (list_is_empty(li)) return new_list();
    
    else if(li->next==NULL){
        free(li);
        li=NULL;
        
        return new_list();
    }
        
    else{
        ListElement *temp=li;
        ListElement *before=li;
        
        while (temp->next != NULL) {
            before=temp;
            temp=temp->next;
        }
        
        free(temp);
        temp=NULL;
        before->next=NULL;
        
        return li;
    }
} 
//--------------------------------------------------------------------------------------------------------------------------

List pop_front_list(List li)
{
    if (list_is_empty(li)) return new_list();
    
    else if(li->next==NULL){
        
        free(li);
        li=NULL;
        
        return new_list();
    }
    
    else{
        ListElement *elements = li->next;
        
        free(li);
        li=NULL;
        
        return elements;
    }
}
//--------------------------------------------------------------------------------------------------------------------------

void  print_list (List li)
{
    if(list_is_empty(li))
    {
        printf("rien a afficher liste vide \n");
        return;
    }
    while(li !=NULL)
     {  printf("[%s]\n",li->name);
        li=li->next;   }
     printf("\n");
}
//--------------------------------------------------------------------------------------------------------------------------

List clear_list(List li)
{
    while(!list_is_empty(li))
        li=pop_front_list(li);
    
    return new_list();
}