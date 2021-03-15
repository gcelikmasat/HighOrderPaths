#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <dirent.h>
#include <math.h>

#define wordLength 100

//Define sturctures for the data structure
struct orderPath {
    struct word* Word;
    struct orderPath *nextNode;
    int isEmpty;
};

struct word {
    char text[wordLength];
    struct word *nextPtr;
    //Create a double char array for doc (m1,m2,e4,etc.) names with the amount of docs(assumed max would be 100)
    char docs[30][8];
    struct orderPath firstOrderPath ;
    struct orderPath secondOrderPath;
    struct orderPath thirdOrderPath;
    int isAddedToTopTen;
    int amount_eco;
    int amount_health;
    int amount_mag;
};



typedef struct word word;
typedef word *wordPtr;
typedef struct orderPath orderPath;
typedef orderPath *orderPathPtr;

void read_words(FILE *f,wordPtr *sP,char documentName[]);
void printMLL(wordPtr *sP);
int findAmountOfDocs (wordPtr *word);
void createFirstOrderPath(wordPtr *sPtr);
void printOrderPath(wordPtr *sPtr);
void createSecondOrderPath(wordPtr *sPtr);
void createThirdOrderPath(wordPtr *sPtr);
void printMostFreaquentWords(wordPtr *sPtr,int docsOfDirectories[]);


int main(void)
{
    setlocale(LC_ALL,"Turkish");
    wordPtr sPtr = NULL;

    int docs_of_eco=0,docs_of_health =0,docs_of_mag =0;

    struct dirent *de;  // Pointer for directory entry

    DIR *dr1 = opendir("magazin");
    DIR *dr2 = opendir("health");
    DIR *dr3 = opendir("econ");
    DIR *directories[] = {dr1,dr2,dr3};

    int i;
    for(i=0;i<3;i++){
        if (directories[i] == NULL)  // opendir returns NULL if couldn't open directory
    {
        printf("Could not open current directory" );
        return 0;
    }
    // Refer http://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html
    // update this part for other folders
    while ((de = readdir(directories[i])) != NULL){
        if (!strcmp (de->d_name, "."))
            continue;
        if (!strcmp (de->d_name, ".."))
            continue;

        char documentName[8];
        char path[100];

        if(strstr(directories[i]->dd_name,"magazin") != NULL){
            strcpy(path,"./magazin/");
            strcpy(documentName,"m");
            docs_of_mag++;
        }

        else if(strstr(directories[i]->dd_name,"health") != NULL){
            strcpy(path,"./health/");
            strcpy(documentName,"h");
            docs_of_health++;
        }

        else if(strstr(directories[i]->dd_name,"econ") != NULL){
            strcpy(path,"./econ/");
            strcpy(documentName,"e");
            docs_of_eco++;
        }

        else
            printf("Something went wrong.");


        strcat(path,(de->d_name));
        FILE *textPtr = fopen(path,"r");

        strcat(documentName,(de->d_name));

        read_words(textPtr,&sPtr,documentName);

    }

    closedir(directories[i]);
    }

    int docsOfDirectories[3] = {docs_of_eco,docs_of_health,docs_of_mag};

    //I created a function that prints our MLL to check my results
    //printMLL(&sPtr);
    //printf("\n");

    createFirstOrderPath(&sPtr);
    createSecondOrderPath(&sPtr);
    createThirdOrderPath(&sPtr);
    printOrderPath(&sPtr);

    printf("\n");
    printMostFreaquentWords(&sPtr,docsOfDirectories);
    printf("\n");

    return 0;
}



void read_words (FILE *f,wordPtr *sP,char documentName[]) {
    char x[wordLength];
    while (fscanf(f, " %s", x) == 1) {
    //If linked list is empty
    if(*sP == NULL){
        //Create a word and init it
        wordPtr new_wordPtr = malloc(sizeof(word));
        strncpy(new_wordPtr->text,x,wordLength);
        strcpy((new_wordPtr->docs[0]),documentName);
        new_wordPtr->isAddedToTopTen = 0;
        new_wordPtr->firstOrderPath.isEmpty=0;
        new_wordPtr->secondOrderPath.isEmpty=0;
        new_wordPtr->thirdOrderPath.isEmpty=0;

        if(documentName[0] == 'e') new_wordPtr->amount_eco =1;
        else if(documentName[0] == 'h') new_wordPtr->amount_health=1;
        else if(documentName[0] == 'm') new_wordPtr->amount_mag =1;

        //Since the sP is NULL we can assign it to our words next
        new_wordPtr->nextPtr = *sP;
        //sP now should be our new pointer
        *sP = new_wordPtr;
    }
    else{
        //Create a new word
        wordPtr new_wordPtr = malloc(sizeof(word));
        wordPtr currentPtr = *sP;
        new_wordPtr->isAddedToTopTen = 0;
        new_wordPtr->firstOrderPath.isEmpty=0;
        new_wordPtr->secondOrderPath.isEmpty=0;
        new_wordPtr->thirdOrderPath.isEmpty=0;

        //Find the last element in the MLL
        while(currentPtr->nextPtr != NULL){
             //If we find the same node twice increase the count
             if(strcmp(currentPtr->text,x)==0){
                if(documentName[0] == 'e') currentPtr->amount_eco++;
                else if(documentName[0] == 'h') currentPtr->amount_health++;
                else if(documentName[0] == 'm') currentPtr->amount_mag++;
                //When we find the same node we should check if it occured in the same document before or not
                int i ;
                for(i=0;i<sizeof(currentPtr->docs)/sizeof(currentPtr->docs[0]);i++){
                    //If it occured before in that document we shouldn't add the same document name to the array
                    if(strcmp(currentPtr->docs[i],documentName)== 0)
                        break;
                    //If it didn't occur then we should add the document name at the end of the array of our word
                    else if (currentPtr->docs[i][0] != 'm' && currentPtr->docs[i][0] != 'h' && currentPtr->docs[i][0] != 'e'){
                        strcpy(currentPtr->docs[i],documentName);
                        break;
                    }
                }
                break;
             }
             else
            currentPtr = currentPtr->nextPtr;
        }

        if(currentPtr->nextPtr == NULL){
            if(strcmp(currentPtr->text,x) == 0){
                if(documentName[0] == 'e') currentPtr->amount_eco++;
                else if(documentName[0] == 'h') currentPtr->amount_health++;
                else if(documentName[0] == 'm') currentPtr->amount_mag++;
            }

            else{
                strncpy(new_wordPtr->text,x,wordLength);
        strncpy(new_wordPtr->docs[0],documentName,8);
        if(documentName[0] == 'e') new_wordPtr->amount_eco=1;
        else if(documentName[0] == 'h') new_wordPtr->amount_health=1;
        else if(documentName[0] == 'm') new_wordPtr->amount_mag=1;

        currentPtr->nextPtr = new_wordPtr;
        new_wordPtr->nextPtr = NULL;
            }

        }
    }
    }
}

void printMLL(wordPtr *sP){
    wordPtr currentPtr = *sP;
    while(currentPtr != NULL){
        printf("%s\t",currentPtr->text);

        int j=0;
        while(currentPtr->docs[j][0] == 'm'||currentPtr->docs[j][0] == 'e'||currentPtr->docs[j][0] == 'h'){
            printf("%s  ",currentPtr->docs[j]);
            j++;
        }
        printf("\n");
        printf("Amount in eco: %d Amount in health: %d Amount in mag: %d\n",currentPtr->amount_eco,currentPtr->amount_health,currentPtr->amount_mag);
        currentPtr = currentPtr->nextPtr;
    }
}

int findAmountOfDocs(wordPtr *word){
    int result=0,i=0;
    wordPtr dummyPointer = *word;
    while(dummyPointer->docs[i][0] == 'm' || dummyPointer->docs[i][0] == 'h' || dummyPointer->docs[i][0] == 'e'){
        i++;
        result++;
    }
    return result;
}

void createFirstOrderPath(wordPtr *sPtr){
    //Create a ptr to loop in the MLL and a currentPtr to hold which node we are creating the order path to
    wordPtr loopPtr = NULL;
    wordPtr currentPtr = *sPtr;
    loopPtr = currentPtr->nextPtr;

    while(currentPtr != NULL){
        while(loopPtr != NULL){
         int i,j;
         int docsOfCPtr =findAmountOfDocs(&currentPtr);
         int docsOfLPtr = findAmountOfDocs(&loopPtr);
         for(i=0;i<docsOfCPtr;i++){
                for(j=0;j<docsOfLPtr;j++){
                if(strcmp(currentPtr->docs[i],loopPtr->docs[j])==0){
                    //If the first order path is empty then assign loop pointers address to orderpath's word pointer
                    if((currentPtr->firstOrderPath.isEmpty) == 0){
                        currentPtr->firstOrderPath.Word = loopPtr;
                        currentPtr->firstOrderPath.isEmpty =1;
                        currentPtr->firstOrderPath.nextNode = NULL;
                    }
                    else{
                        orderPathPtr new_orderPathPtr = malloc(sizeof(orderPath));
                        new_orderPathPtr->Word=loopPtr;
                        new_orderPathPtr->isEmpty=1;

                        orderPathPtr currentOrderPathPtr = &(currentPtr->firstOrderPath);
                        while(currentOrderPathPtr->nextNode != NULL)
                            currentOrderPathPtr= currentOrderPathPtr->nextNode;

                        currentOrderPathPtr->nextNode = new_orderPathPtr;
                        new_orderPathPtr->nextNode= NULL;
                    }
            }
         }
         }
         loopPtr = loopPtr->nextPtr;
        }
        currentPtr= currentPtr->nextPtr;
        if(currentPtr != NULL)
        loopPtr = currentPtr->nextPtr;
        else
            break;
    }
}

void printOrderPath(wordPtr *sPtr){
    wordPtr currentPtr = *sPtr;
    orderPathPtr currentPath;
    printf("first order term co-occurence: ");
    while(currentPtr != NULL){
        if(currentPtr->firstOrderPath.isEmpty != 0){
            currentPath = &(currentPtr->firstOrderPath);
            while(currentPath->nextNode != NULL){
                printf("{%s,%s} ",currentPtr->text,currentPath->Word->text);
                currentPath = currentPath->nextNode;
            }
        }
        currentPtr= currentPtr->nextPtr;
    }
    printf("\n");

    wordPtr currentPtr2 = *sPtr;
    orderPathPtr currentPath2;
    printf("second order term co-occurence: ");
    while(currentPtr2 != NULL){
        if(currentPtr2->secondOrderPath.isEmpty != 0){
            currentPath2 = &(currentPtr2->secondOrderPath);
            while(currentPath2->nextNode != NULL){
                printf("{%s,%s} ",currentPtr2->text,currentPath2->Word->text);
                currentPath2 = currentPath2->nextNode;
            }
        }
        currentPtr2= currentPtr2->nextPtr;
    }
    printf("\n");
    printf("\n");

   wordPtr currentPtr3 = *sPtr;
    orderPathPtr currentPath3;
    printf("third order term co-occurence: ");
    while(currentPtr3 != NULL){
        if(currentPtr3->thirdOrderPath.isEmpty != 0){
            currentPath3 = &(currentPtr3->thirdOrderPath);
            while(currentPath3->nextNode != NULL){
                printf("{%s,%s} ",currentPtr3->text,currentPath3->Word->text);
                currentPath3 = currentPath3->nextNode;
            }
        }
        currentPtr3= currentPtr3->nextPtr;
    }
    printf("\n");
}

void createSecondOrderPath(wordPtr *sPtr){
    wordPtr currentPtr,tempPtr1,tempPtr2;
    currentPtr = *sPtr;
    while(currentPtr != NULL){
        int input = findAmountOfDocs(&currentPtr);
        if(input >= 2){
            tempPtr1 = *sPtr;
            tempPtr2=*sPtr;
            int k,j;
            for(k=0;k<input -1;k++){
                for(j=k+1;j<input;j++){
                    while(tempPtr1 != NULL){
                    tempPtr2 = *sPtr;
                    int l;
                    for(l=0;l<findAmountOfDocs(&tempPtr1);l++){
                        if(strcmp(tempPtr1->docs[l],currentPtr->docs[k]) == 0 && tempPtr1 != currentPtr){
                            while(tempPtr2 != NULL){
                                int m;
                                for(m = 0; m<findAmountOfDocs(&tempPtr2);m++){
                                    if(strcmp(tempPtr2->docs[m],currentPtr->docs[j]) == 0 && tempPtr2 != currentPtr){
                                        if((tempPtr1->secondOrderPath.isEmpty) == 0){
                                            tempPtr1->secondOrderPath.Word = tempPtr2;
                                            tempPtr1->secondOrderPath.nextNode = NULL;
                                            tempPtr1->secondOrderPath.isEmpty = 1;
                                        }
                                        else{
                                            orderPathPtr current_orderPath = &(tempPtr1->secondOrderPath);
                                            orderPathPtr newOrderPath = malloc(sizeof(orderPath));
                                            newOrderPath->Word = tempPtr2;
                                            newOrderPath->nextNode = NULL;
                                            newOrderPath->isEmpty = 1;

                                            while(current_orderPath->nextNode !=NULL)
                                            current_orderPath = current_orderPath->nextNode;

                                            current_orderPath->nextNode=newOrderPath;
                                        }

                                    }
                                }
                                tempPtr2 = tempPtr2->nextPtr;
                            }
                        }
                    }
                    tempPtr1 = tempPtr1->nextPtr;
                    }
                }
            }
        }
        currentPtr = currentPtr->nextPtr;
    }
}

void createThirdOrderPath(wordPtr *sPtr){
    wordPtr currentPtr,tempPtr1,tempPtr2;
    currentPtr= *sPtr;
    while(currentPtr != NULL){
     int input = findAmountOfDocs(&currentPtr);
     if(input >= 3){
        tempPtr1 = *sPtr;
        tempPtr2 = *sPtr;
        int k,l;
        for(k=0,l=k+2;k<input-2;k++,l++){
            while(tempPtr1 != NULL){
                tempPtr2 = *sPtr;
                int i;
                    for(i=0;i<findAmountOfDocs(&tempPtr1);i++){
                        if(strcmp(tempPtr1->docs[i],currentPtr->docs[k]) == 0 && tempPtr1 != currentPtr){
                            while(tempPtr2 != NULL){
                                    int m;
                                    for(m=0;m<findAmountOfDocs(&tempPtr2);m++){
                                        if(strcmp(tempPtr2->docs[m],currentPtr->docs[l])== 0 && tempPtr2 != currentPtr){
                                            if(tempPtr1->thirdOrderPath.isEmpty == 0){
                                                tempPtr1->thirdOrderPath.Word = tempPtr2;
                                                tempPtr1->thirdOrderPath.nextNode = NULL;
                                                tempPtr1->thirdOrderPath.isEmpty = 1;
                                            }
                                            else{
                                                orderPathPtr currentPathPtr = &(tempPtr1->thirdOrderPath);
                                                orderPathPtr newThird_o_Ptr = malloc(sizeof(orderPath));
                                                newThird_o_Ptr->Word=tempPtr2;
                                                newThird_o_Ptr->nextNode=NULL;
                                                newThird_o_Ptr->isEmpty = 1;

                                                while(currentPathPtr->nextNode != NULL)
                                                    currentPathPtr = currentPathPtr->nextNode;

                                                currentPathPtr->nextNode = newThird_o_Ptr;
                                            }
                                        }
                                    }
                                tempPtr2 = tempPtr2->nextPtr;
                            }
                        }
                    }
                tempPtr1 = tempPtr1->nextPtr;
            }
        }
     }
     currentPtr = currentPtr->nextPtr;
    }
}

void printMostFreaquentWords(wordPtr *sPtr,int docsOfDirectories[]){
        wordPtr currentPtr = (*sPtr)->nextPtr;
        wordPtr highest = *sPtr;

        word* topFiveEco[5];
        word* topFiveHealth[5];
        word* topFiveMag[5];

        int k;
        for(k =0;k<3;k++){
        //for econ
        if(k == 0){
            int j;
        for(j=0;j<5;j++){
        while(currentPtr != NULL){
            if(currentPtr->amount_eco > highest->amount_eco && currentPtr->isAddedToTopTen == 0)
         highest = currentPtr;
         else if(currentPtr->amount_eco == highest->amount_eco && highest->isAddedToTopTen != 0)
         highest = currentPtr;
         currentPtr = currentPtr->nextPtr;
        }
         topFiveEco[j] = highest;
         highest->isAddedToTopTen = 1;
         currentPtr = (*sPtr)->nextPtr;
         highest = *sPtr;
        }
        }
        else if(k == 1){
           int j;
        for(j=0;j<5;j++){
        while(currentPtr != NULL){
            if(currentPtr->amount_health > highest->amount_health && currentPtr->isAddedToTopTen == 0)
         highest = currentPtr;
         else if(currentPtr->amount_health == highest->amount_health && highest->isAddedToTopTen != 0)
         highest = currentPtr;
         currentPtr = currentPtr->nextPtr;
        }
         topFiveHealth[j] = highest;
         highest->isAddedToTopTen = 1;
         currentPtr = (*sPtr)->nextPtr;
         highest = *sPtr;
        }
        }
        else if(k == 2){
            int j;
        for(j=0;j<5;j++){
        while(currentPtr != NULL){
            if(currentPtr->amount_mag > highest->amount_mag && currentPtr->isAddedToTopTen == 0)
         highest = currentPtr;
         else if(currentPtr->amount_mag == highest->amount_mag && highest->isAddedToTopTen != 0)
         highest = currentPtr;
         currentPtr = currentPtr->nextPtr;
        }
         topFiveMag[j] = highest;
         highest->isAddedToTopTen = 1;
         currentPtr = (*sPtr)->nextPtr;
         highest = *sPtr;
        }
        }
        }

        printf("------------------------------------\n");
        printf(" Econ    | Health    | Magazine    |\n");
        int i;
        for(i=0;i<5;i++){
        printf("%s;%d   | %s;%d    | %s;%d    |\n",topFiveEco[i]->text,topFiveEco[i]->amount_eco,topFiveHealth[i]->text,topFiveHealth[i]->amount_health,
               topFiveMag[i]->text,topFiveMag[i]->amount_mag);
        }

        printf("\n");
        /*printf(" Econ    | Health    | Magazine    |\n");
        int k;
        for(k=0;k<5;k++{

        }*/
        int m;
        for(m = 0; m<3;m++){
        if(m == 0){
        printf("Econ\n");
        int g;
        for(g =0;g<5;g++){
            double tf_idf,freq,docs_of_word=0;
            int h;
            for(h=0;h<findAmountOfDocs(&topFiveEco[g]);h++){
                if(topFiveEco[g]->docs[h][0] == 'e')
                    docs_of_word++;
            }
            freq =(double)docsOfDirectories[m]/docs_of_word;
            tf_idf = log(freq);
            printf("%s;%f\n",topFiveEco[g]->text,tf_idf);
        }
       }
       else if(m == 1){
        printf("Health\n");
        int g;
        for(g =0;g<5;g++){
            double tf_idf,freq,docs_of_word=0;
            int h;
            for(h=0;h<findAmountOfDocs(&topFiveHealth[g]);h++){
                if(topFiveHealth[g]->docs[h][0] == 'h')
                    docs_of_word++;
            }
            freq =(double)docsOfDirectories[m]/docs_of_word;
            tf_idf = log(freq);
            printf("%s;%f\n",topFiveHealth[g]->text,tf_idf);
        }
       }
       else if(m==2){
        printf("Magazine\n");
        int g;
        for(g =0;g<5;g++){
            double tf_idf,freq,docs_of_word=0;
            int h;
            for(h=0;h<findAmountOfDocs(&topFiveMag[g]);h++){
                if(topFiveMag[g]->docs[h][0] == 'e')
                    docs_of_word++;
            }
            freq =(double)docsOfDirectories[m]/docs_of_word;
            if(freq == 1)
                tf_idf = 0;
            else
            tf_idf = log(freq);

            printf("%s;%f\n",topFiveMag[g]->text,tf_idf);
        }
       }

        }
}