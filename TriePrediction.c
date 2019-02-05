//Scott Kramarz Trie Prediction

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "TriePrediction.h"


// Helper function called by printTrie(). (Credit: Dr. S.)
void printTrieHelper(TrieNode *root, char *buffer, int k)
{
	int i;

	if (root == NULL)
		return;

	if (root->count > 0)
		printf("%s (%d)\n", buffer, root->count);

	buffer[k + 1] = '\0';

	for (i = 0; i < 26; i++)
	{
		buffer[k] = 'a' + i;

		printTrieHelper(root->children[i], buffer, k + 1);
	}

	buffer[k] = '\0';
}

// If printing a subtrie, the second parameter should be 1; otherwise, if
// printing the main trie, the second parameter should be 0. (Credit: Dr. S.)
void printTrie(TrieNode *root, int useSubtrieFormatting)
{
	char buffer[1026];

	if (useSubtrieFormatting)
	{
		strcpy(buffer, "- ");
		printTrieHelper(root, buffer, 2);
	}
	else
	{
		strcpy(buffer, "");
		printTrieHelper(root, buffer, 0);
	}
}
//If the character is a letter returns 1, else returns 0. isLetter would probably be a better name but whatever.
int isChar(char c)
{
	if(c >= 'A' && c<= 'Z')
		return 1;
	if(c >= 'a' && c<= 'z')
		return 1;
	return 0;
}

int isNum(char c)
{
	if(c >= '0' && c<= '9')
		return 1;
	return 0;
}

//Adds a word to the subtrie - essentially a stripped down version of addWord
void addSubWord(TrieNode *root, char *str)
{
	if(root == NULL || str == NULL)
		return;
	
	TrieNode *temp = root;
	int i = 0, len = strlen(str);
	
	for(i = 0; i < len; i++)
	{
		if(temp->children[str[i]-'a'] == NULL)
			temp->children[str[i]-'a'] = calloc(1,sizeof(TrieNode));
		
		temp = temp->children[str[i]-'a'];
	}
	
	temp->count = temp->count + 1;

	return;
}

//Builds the subtrie of a word until it hits the end of a sentance or text
void buildSubtrie(TrieNode *root, char *text)
{
	char buffer[1026];
	int i = 0, j = 0;
	TrieNode *temp = root;

	if(root == NULL || text == NULL)
		return;
	
	//Parse the remainder of the sentence in the text. Each following word in the sentence is added to the subtrie
	for(i = 0; text[i] != '\0' && text[i] != '.' && text[i] != '!' && text[i] != '?'; i++)
	{
		strcpy(buffer, "");
		j = 0;
		while(isChar(text[i]))
		{
			buffer[j++] = text[i++];
		}
		buffer[j] = '\0';
		addSubWord(root, buffer);
	}
	root->count = 0;	
	
	return;
}

//			    	Word to be added, Rest of the text
void addWord(TrieNode *root, char *str, char *text)
{
	int i = 0, len;
	TrieNode *temp = root;
	
	if(root == NULL || str == NULL)
		return;
	
	len  = strlen(str);
	
	//Create the child if it does not exist, then set the pointer to the child
	for(i = 0; i < len; i++)
	{
		if(temp->children[str[i]-'a'] == NULL)
			temp->children[str[i]-'a'] = calloc(1,sizeof(TrieNode));
		
		temp = temp->children[str[i]-'a'];
	}
	//Increment count of final node of the word.
	temp->count = temp->count + 1;
	//Create subtrie if it doesn't exist then pass the remainder of the text to the buildSubtrie function.
	if(temp->subtrie == NULL)
		temp->subtrie = calloc(1, sizeof(TrieNode));
	
	buildSubtrie(temp->subtrie, text);

	return;
}

TrieNode *buildTrie(char *filename)
{
	char *fName, *text, *cleantext;
	char buffer[1026];
	FILE *file = fopen(filename, "r");
	int i = 0, j = 0;
	TrieNode *root = calloc(1,sizeof(TrieNode));
	
	if (file == NULL)  
	{
//		Try again with .txt appended just incase the user forgot or the OS needs it.
		fName = calloc((strlen(filename)+5),sizeof(char));
		strcpy(fName, filename);
		strcat(fName, ".txt");
		file = fopen(fName, "r");
		
		if(file == NULL)
		{
			free(fName);
			free(root);
			return NULL;
		}
	}

	fseek(file, 0, SEEK_END); 
	size_t length = ftell(file);
	fseek(file, 0, SEEK_SET);
	
	text = calloc((length+1),sizeof(char));
	cleantext = calloc((length+1),sizeof(char));
	
	for(i = 0;((text[i] = fgetc(file)) != EOF); i++)
		;
		
	text[length] = '\0';

//A loop to remove all unnecessary characters (any nonalphabetic character or any punctuation other than . ? !)	
	for(i = 0, j = 0; i < length; i++)
	{
		if(text[i] >= 'A' && text[i] <= 'Z')
		{
			cleantext[j++] = tolower(text[i]);
		}
		else if(text[i] >='a' && text[i] <= 'z')
		{
			cleantext[j++] = text[i];
		}
		else if(text[i] == '.' || text[i] == '!' || text[i] == '?' || text[i] == ' ')
		{
			cleantext[j++] = text[i];
		}
	}
	cleantext[j] = '\0';
	
	for(i = 0, j = 0; cleantext[i] != '\0'; i++)
	{
		strcpy(buffer, "");
		j = 0;
		while(isChar(cleantext[i]))
		{
			buffer[j++] = cleantext[i++];
		}
		buffer[j] = '\0';
		addWord(root, buffer, cleantext+i);
	}

	fclose(file);
	free(fName);
	free(text);
	free(cleantext);
	return root;
}

int processInputFile(TrieNode *root, char *filename)
{
	char *fName, *text;
	char buffer[1026], tempnum[1026];
	FILE *file = fopen(filename, "r");
	int i = 0, j = 0, k = 0, n = 0, switched = 0;
	TrieNode *temp = NULL;	
	
	if (file == NULL)  
	{
//		Try again with .txt appended just incase the user forgot or the OS needs it.
		fName = calloc((strlen(filename)+5),sizeof(char));
		strcpy(fName, filename);
		strcat(fName, ".txt");
		file = fopen(fName, "r");
		
		if(file == NULL)
		{
			free(fName);
			return 1;
		}
	}

	fseek(file, 0, SEEK_END); 
	size_t length = ftell(file);
	fseek(file, 0, SEEK_SET);
	
	text = calloc((length+1),sizeof(char));
	
	for(i = 0;((text[i] = fgetc(file)) != EOF); i++)
		;
		
	text[length] = '\0';
	
	for(i = 0; text[i] != '\0'; i++)
	{
		switched = 0;
		switch(text[i])
		{
			case '@' :
			{
				switched = 1;
				i++;
				i++;
				j = 0;
				while(isChar(text[i]))
				{
					buffer[j] = text[i];
					j++;
					i++;
				}
				i++;
				k = 0;
				while(isNum(text[i]))
				{
					tempnum[k] = text[i];
					k++;
					i++;
				}
				i--;
				tempnum[k] = '\0';
				buffer[j] = '\0';
				n = atoi(tempnum);
			
				printf("%s ", buffer);

				temp = getNode(root, buffer);
				
				if(temp == NULL)
					break;
				for(k = 0; k < n; k++)
				{
					getMostFrequentWord(temp->subtrie, buffer);
					printf("%s ", buffer);
					temp = getNode(root, buffer);
				}
				printf("\n");
				break;
			}
		
			case '!' :
			{
				printTrie(root, 0);
				switched = 1;
				break;
			}
										
			}
		if(isChar(text[i]) && switched == 0)
		{
			j = 0;
			while(isChar(text[i]))
				{
					buffer[j] = text[i];
					j++;
					i++;
				}
				i--;
				buffer[j] = '\0'; 
			printf("%s\n", buffer);
			temp = getNode(root, buffer);
			if(temp != NULL)
				printTrie(temp->subtrie, 1);
		}
	}

	fclose(file);
	free(fName);
	free(text);
	return 0;
}

TrieNode *destroyTrie(TrieNode *root)
{
	int i;
	if(root == NULL)
		return NULL;
	
	destroyTrie(root->subtrie);
	for(i = 0; i <26; i++)
		destroyTrie(root->children[i]);
	
	free(root);
	return NULL;
}

TrieNode *getNode(TrieNode *root, char *str)
{
	int i, len;
	if(root == NULL || str == NULL)
		return NULL;
	
	len  = strlen(str);
	
	for(i = 0; i < len; i++)
	{
		if(root->children[(tolower(str[i]))-'a'] == NULL)
			return NULL;
		else
			root = root->children[tolower(str[i])-'a'];
	}
	if(root->count > 0)
		return root;
	return NULL;
}
//Letter, Position in Str,  Highest Count seen
int getMostFrequentWordHelper(TrieNode *root, char *str, int n, int pos, int *count)
{
	int i, ret = 0;
	
	if(root == NULL)
		return 0;
	for(i = 0; i < 26; i++)
		ret = getMostFrequentWordHelper(root->children[i], str, i, pos + 1, count);
	//As the function works down if it's the highest count seen it'll update the string to be null terminated after
	//The last character, then set the characters for the string as it calls back up the stack
	if(root->count > *count)
	{
		*count = root->count;
		str[pos] = 'a' + n;
//		printf("putting %c at position %d ", ('a'+ n), pos);
		str[pos+1] = '\0';
//		printf("returning 1\n");
		return 1;
	}
	if(ret == 1)
	{
//		printf("putting %c at position %d\n", ('a' + n), pos);
		str[pos] = 'a' + n;
	}
	return 0;
}


void getMostFrequentWord(TrieNode *root, char *str)
{
	int i, j = 0;
	
	if(str == NULL)
		return;
	if(root == NULL)
	{
		strcpy(str, "");
		return;
	}
	
	for(i = 0; i < 26; i++)
		getMostFrequentWordHelper(root->children[i], str, i, 0, &j);
	
	return;
}

//Find if a string exists in the trie
int containsWord(TrieNode *root, char *str)
{
	int i = 0, len;

	if(root == NULL || str == NULL)
		return 0;
	
	len  = strlen(str);
	
	for(i = 0; i < len; i++)
	{
		if(root->children[tolower(str[i])-'a'] == NULL)
			return 0;
		else
			root = root->children[tolower(str[i])-'a'];
	}
	if(root->count > 0)
		return 1;
	return 0;
}

int prefixCountHelper(TrieNode *root)
{
	int i, count;
	
	if(root == NULL)
		return 0;
	//Add this nodes count
	count = root->count;
	//And all of it's children
	for(i = 0; i < 26; i++)
		count += prefixCountHelper(root->children[i]);
	
	return count;
} 
//Finds all words that start with string str
int prefixCount(TrieNode *root, char *str)
{
	int i, len = strlen(str), count = 0;

	if(root == NULL)
		return 0;
	//Find the node at the end of the string
	for(i = 0; i < len; i++)
	{
		if(root->children[tolower(str[i])-'a'] == NULL)
			return 0;
		else
			root = root->children[tolower(str[i])-'a'];
	}
	//The string itself matters
	count = root->count;
	//Add the count for all it's children
	for(i = 0; i <26; i++)
		count += prefixCountHelper(root->children[i]);
	
	return 0;
}

int main(int argc, char **argv)
{
	TrieNode *root = buildTrie(argv[1]);
	processInputFile(root, argv[2]);
	root = destroyTrie(root);
	return 0;
}
