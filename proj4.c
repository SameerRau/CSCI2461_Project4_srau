#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Global variables.  
int buckets = 1;
int docmatch[3];
int keyphrases = 0;
int go = 0;

//Creating a struct which will point to the bucket containing all 
//the phrases that corresponded to that particular bucket. 
struct bucketPointer {
    int value;
    struct LinkList* link;
};

//A LinkNode Struct which holds a phrase and the document identification number.
struct LinkNode{
    char* phrase;
    int docID;
    struct LinkNode* next;
};

//LinkList struct containing 2 pointers, one that points to the bucketHead bucket 
//at that particular key and one that points at the bucketTail bucket of a particular 
//key.
//bucketHead = first position.
//bucketTail = last position.
struct LinkList{
    struct LinkNode *bucketHead;
    struct LinkNode *bucketTail;
    int size;
};

//The function takes a phrase/string as an input, and converts it into a number, using the ASCII 
//values that correspond to each letter, which is then added to the number of buckets.
//1. -> sums up the ASCII values in the string
//2. -> adds the result of (1) to a particular bucket, denoted by b = S%N.
//where N represents the number of buckets and S represents the particular number.
int hash_code(char* w) {
    int value;
    char* letter;
    
    letter = w;
    value = 0;
    
    while(*letter!='\0') {
        value = value + (int)*letter;
        letter = letter + 1;
    }
    
    value = value%buckets;
    
    return value;
}

//These two components will be inserted as a pair (i,k) from the function 
//CONDITION: ensures that a document is not added recurring times in any given
//bucket for the same string.
void hash_table_insert(char* phrase, int docID, struct bucketPointer table[]) {
    
    int recurring;
    int value;
    
    value = hash_code(phrase);
    recurring = search_list(phrase, docID, (table[value].link));
    
    if(recurring==1) {
        return;
    }
    
    else {
        add_to_list(phrase, docID, table[value].link);
    }
}

//inserts a string k and the document ID number into the hash table.
void add_to_list(char* phrase, int docID, struct LinkList* List) {
    struct LinkNode* node = malloc(sizeof(struct LinkNode));
    node->phrase = (char*)malloc(sizeof(char)*strlen(phrase));
    strcpy(node->phrase, phrase);
    node->docID = docID;
    insert_list(List, node);
}

//searches the list for a phrase that matches the query keyphrase.
int search_list(char* phrase, int docID, struct LinkList* List) { 
   
    int match;
    
    struct LinkNode* currentNode;
    
    match = 0; 
    currentNode = List->bucketHead;
    
    while(currentNode!=NULL) {
        match = strcmp(phrase, (currentNode->phrase));
        
        if(match==0 && docID == currentNode->docID) {
            return 1; 
        }
        
        else {
            currentNode = currentNode->next;
        }
    }
    
    return 0;
}

//inserts an element into the linked list.
void insert_list(struct LinkList* List, struct LinkNode* node) {
    
    if(List->bucketHead == NULL) { 
        List->bucketHead = node;
        List->bucketTail = node; 
        node->next = NULL;
    }
    
    else {                   
        
        List->bucketTail->next = node;
        List->bucketTail = node;
        node->next = NULL;
    }
}

//handles pre-processing - takes a set of documents as input, returning the full hash table.
struct bucketPointer* training() {
  
    FILE *D1;
    FILE *D2;
    FILE *D3;
    char B1[1000];
    char B2[1000];
    char B3[1000];
    char* strval; 
    int counter;
    
    //allocate space for buckets.
    struct bucketPointer* table = malloc(buckets * sizeof(struct bucketPointer));
    
    counter = 0;
     
    while(counter < buckets) {
        struct LinkList* bucket = malloc(sizeof(struct LinkList));
        table[counter].link = bucket;
        table[counter].value = counter;
        counter++;
    }
    
    D1 = fopen("D1.txt", "r"); 
    fgets(B1, 1000, D1); 
    strval = strtok(B1, " ");
    
    while(strval!=NULL) {
        hash_table_insert(strval, 0, table);
        char* searchval = strval;
        int hash = hash_code(strval); 
        strval = strtok(NULL, " ");
    }
    
    fclose(D1);
    D2 = fopen("D2.txt", "r") ;
    fgets(B2, 1000, D2);
    strval = strtok(B2, " ");
    
    while(strval!=NULL) {
        hash_table_insert(strval, 1, table);
        strval = strtok(NULL, " ");
    }
    
    fclose(D2);
    D3 = fopen("D3.txt", "r");
    fgets(B3, 1000, D3);
    strval = strtok(B3, " ");
    
    while(strval!=NULL) {
        hash_table_insert(strval, 2, table);
        strval = strtok(NULL, " ");
    }
    
    fclose(D3);
    
    return table;
}

//reads the query set by asking for user interaction.
//The user chooses what phrases to query for.
char* read_query() {
    char* B1 = (char*)malloc(sizeof(char)*100);
    printf("enter the phrases you want to search for, seperated only by a space \n");
    fgets(B1, 100, stdin);
    
    return B1;
}

//keeps track of keyword matches found in the documents.
void score(char* qphrase, struct LinkList* List) {
    int match;
    struct LinkNode* currentNode;
    
    if(List!=NULL) {
        currentNode = List->bucketHead;
    }
        
    while(currentNode!=NULL) {
        match = strcmp(qphrase, currentNode->phrase);
                                            
        if(match == 0) {                 
            printf("the doc number of the phrase we found that matches is %d \n", currentNode->docID);
            incrementID(currentNode->docID);
        }
            
        currentNode = currentNode->next;
    }
}

// takes a set of keyphrases and returns the documents that consists of those keyphrases. 
// An indication is made if the match is a "perfect" one.
void find_match(struct bucketPointer table[]) {
    keyphrases = 0;
    docmatch[0]=0;
    docmatch[1]=0;
    docmatch[2]=0;

    int qvalue;
    struct LinkList* List;
    char*strval;
    char* avalue;
    
    avalue = read_query();
    printf("the contents of avalue are %s, the length is %lu", avalue, strlen(avalue));
    
    if(strcmp(avalue, "#\n")==0) {
        go = 1;
        return;
    }
    
    strval = strtok(avalue, " \n");
    
    while(strval!=NULL) {
        printf("the value of strval in the find match function is %s \n", strval);
        printf("the length of the string is %lu \n", strlen(strval));
        keyphrases++;
        qvalue = hash_code(strval);
        List = table[qvalue].link;
        score(strval, List);
        strval = strtok(NULL, " \n");
    }
    
    free(avalue);
}

//1a. -> calls training() to read documents and create hash table.
//1b. -> before moving to training() the number of buckets is specified.
//2. -> the search phase looks for all keyphrases and locate documents consisting of these phrases.
//3. -> the user will be asked to input a query set, which will then call read_query(), which will then read it.
//4. -> the matching score is computed, with the documents with higher scores (and relevance) appearing bucketHead.
//5. -> the user is asked to enter the next query set.
//6. -> #exit - the program exits when the user enters a #.
int main() {
    printf("enter the number of buckets \n");
    scanf("%d", &buckets);
    getchar();
    struct bucketPointer* table = training();
        
    while(go == 0) {
        find_match(table);
        
        if(go==0) {
            print();
        }
    }
    
    return 0;
}

//updates the score.
void incrementID(int docID) {
    docmatch[docID]++;
}
 
//prints the scores of each document.
//If the score is a perfect one, this is indicated in the message.
void print() {
   
    int counter = 0;
    
    while(counter < 3) {
        
        if(docmatch[counter] == keyphrases) {
            printf("the score of Document %d is %d.  This is a perfect score", counter, keyphrases);
        }
        
        else {
            printf("the score of Document %d is %d.", counter, docmatch[counter]);
        }
        
        counter++;
    }
}