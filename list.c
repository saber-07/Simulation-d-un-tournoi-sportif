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

List push_back_list(List li, char x[NAMEZIZE])
{
    ListElement *element;
    
    element=malloc(sizeof(*element));
    
    if (element==NULL) {
        fprintf(stderr, "probléme d'allocation dynamyque. \n");
        exit(EXIT_FAILURE);
    }
    
    strcpy(element->name, x);
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
List push_front_list(List li, char x[NAMEZIZE])
{
    ListElement *element;
    
    element=malloc(sizeof(*element));
    
    if (element==NULL) {
        fprintf(stderr, "probléme d'allocation dynamyque. \n");
        exit(EXIT_FAILURE);
    }
    
    strcpy(element->name, x);
    
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
     {  printf("[%d]",li->name);
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

//--------------------------------------------------------------------------------------------------------------------------

List fusion(List li1, List li2){
    List li=new_list(), temp;
    
    if (li1->name < li2->name){
        li = li1;
        li1 = li1->next;
    }
    else {
        li = li2;
        li2 = li2->next;
    }
    temp = li;
    
    while (li1 != NULL && li2 != NULL) {
        if (li1->name < li2->name){
            temp->next = li1;
            li1 = li1->next;
        }
        else{
            temp->next = li2;
            li2 = li2->next;
        }
        temp = temp->next;
    }
    
    if (li2 == NULL)
        temp->next = li1;
    
    if (li1 == NULL)
        temp->next = li2;
    
    return li;
}
//--------------------------------------------------------------------------------------------------------------------------

List supp_oc(List li, int x)
{
    ListElement *temp=new_list(), *before=li, *current=li->next;
    
    while (current != NULL) {
        if (current->name == x) {
            temp = current;
            before->next = current->next;
            current = before->next;
            free(temp);
        }
        else{
            before = current;
            current = current->next;
        }
    }
    
    return li;
}
//--------------------------------------------------------------------------------------------------------------------------

// void eclate(List *li, List *lp, List *lip)
// {
//     ListElement *temp=*li, *current1=NULL, *current2=NULL;
    
//     if (temp==NULL)
//         printf("cette list est vide");
//     else{
//         while (temp != NULL) {
//             if (temp->name %2 == 0) {
//                 if (current1 == NULL) {
//                     *lp = temp;
//                     current1 = temp;
//                 }
//                 else{
//                     current1->next = temp;
//                     current1 = current1->next;
//                 }
//             }
//             else{
//                 if (current2 == NULL) {
//                     *lip = temp;
//                     current2 = temp;
//                 }
//                 else{
//                     current2->next = temp;
//                     current2 = current2->next;
//                 }
//             }
//             temp = temp->next;
//         }
//         if (current1 != NULL)
//             current1->next = NULL;
//         if (current2 != NULL)
//             current2->next = NULL;
//         *li = NULL;
//     }
// }
