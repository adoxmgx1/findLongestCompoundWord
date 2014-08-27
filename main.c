/* 
 * 
 * Largest word: immunoelectrophoretically
 * Second largest word: phosphatidylethanolamines
 * Number of compound words found: 63780
 *
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct trieNode {
  char letter;
  struct trieNode *children[27];
} trieNode;

typedef struct trie {
  struct trieNode * root;
} trie;

//initializes trie
void init(struct trie *rootTrie) {
  rootTrie->root = calloc(1, sizeof(trieNode));
  rootTrie->root->letter = '\0';
  int i;
  for(i = 0; i < 27; i ++) {
    rootTrie->root->children[i] = NULL;
  }
}

//recursively free each node from bottom up
void cleanNode(struct trieNode * node) {
  //if no children, free and return. Otherwise, free children.
  int i;
  if(node) {
    if(node->children) {
      for(i = 0; i < 27; i ++) {
	if(node->children[i] != NULL) {
	  cleanNode(node->children[i]);
	}
      }
    }
    free(node);
  }
}

//free the trie
void clean(struct trie *rootTrie) {
  cleanNode(rootTrie->root);
  free(rootTrie);

}

//insert word into the trie
void insert(struct trie * rootTrie, char * word) {
  trieNode * node = rootTrie->root;
  //for each letter
  int wordLength = strlen(word);
  int i;
  for(i = 0; i < wordLength; i++) {
    int index = word[i] - 97;
    //if it exists go into said node
    if(node->children[index]) {
      node = node->children[index];
      
    } else { 
      //doesn't exist? create the node and descend into it
      node->children[index] = calloc(1, sizeof(trieNode));
      node = node->children[index];
      node->letter = word[i];
    }
  }
  //add null terminator node
  node->children[26] = calloc(1, sizeof(trieNode));
  node->children[26]->letter = '\0';
}

//walks the tree along the characters in searchtTerm and notes how many null characters are there
int searchWordCat(struct trie * rootTree, char * searchTerm) {
  int numWords = 0;
  int searchTermLength = strlen(searchTerm);
  trieNode * node = rootTree->root;

  int i;
  for(i = 0; i < searchTermLength; i ++) {
    if(!node) {
      return 0;
    }
    if(node->children[26]) {
      numWords++;
      node = rootTree->root;
      i--;
    } else {
      node = node->children[searchTerm[i]-97];
    }
    
  }

  if(numWords > 1) {
    return searchTermLength;
  }
  return 0;

}


int findLargestCompoundWords(const char * fileName) {
  struct trie *wordTree = malloc(sizeof(trie));
  init(wordTree);

  char buffer[1024];
  FILE * fp;
  
  if((fp = fopen(fileName, "r")) == NULL) {
    printf("%s", "Couldn't open file \n");
    return -1;
  }

  //reads file and inserts it into trie, then rewinds the file

  int smallestWordLength = 1024;

  while(fgets(buffer, 1024, fp) != NULL) {    
    buffer[strlen(buffer)-2] = '\0';
    int wordLength = strlen(buffer);
    if(wordLength < 2) break;
    if(wordLength < smallestWordLength) {
      smallestWordLength = wordLength;
    }
    insert(wordTree, buffer);
  }

  rewind(fp);


  //reads file again, keeps track of top two words as well as number of compound words

  char * maxWord = calloc(sizeof(char), 1024);
  char * secondMaxWord = calloc(sizeof(char), 1024);

  int maxWordLength = 0;
  int secondMaxWordLength = 0;

  int numCompoundWords = 0;

  while(fgets(buffer, 1024, fp) != NULL) {
    int searchTermLength = strlen(buffer)-2;
    buffer[searchTermLength] = '\0';
    int numFits = 0;
    if(searchTermLength >= smallestWordLength * 2) {
      numFits = searchWordCat(wordTree, buffer);
    }
    if(numFits > 0) numCompoundWords++;
    if(maxWordLength < numFits) {
      strcpy(secondMaxWord, maxWord);
      secondMaxWordLength = maxWordLength;
      strcpy(maxWord, buffer);
      maxWord[searchTermLength] = '\0';
      maxWordLength = numFits;
      
    } else if(secondMaxWordLength < numFits) {
       strcpy(secondMaxWord, buffer);
       secondMaxWord[searchTermLength] = '\0';
       secondMaxWordLength = numFits;
    }
  }

  printf("Largest word: %s, Second Largest word: %s Number of compound words: %d \n", maxWord, secondMaxWord, numCompoundWords);


  free(maxWord);
  free(secondMaxWord);
  clean(wordTree);
  fclose(fp);

  return 0;
}

int main(void) {

  const char * fileName = "wordsforproblem.txt";
  return findLargestCompoundWords(fileName);

}
