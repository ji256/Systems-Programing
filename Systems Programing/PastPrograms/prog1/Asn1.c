#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//Node Struct
typedef struct Node 
{
	char *s;
	struct Node *left;
	struct Node *right;
}Node;

//BST Struct
typedef struct
{
	Node *root;
}BST;



//New Node function
Node* newNode(const char *s) 
{
	Node *aNode = (Node*)malloc(sizeof(Node));
	aNode->s = strdup(s);
	aNode->left = NULL;
	aNode->right = NULL;
	return aNode;
}

//Insert into the BST
void insertNode(Node **node, const char *s)
{
	//if the BST is empty
	if (*node == NULL)
	{
		*node = newNode(s);
		return;
	}
	if (strcmp(s, (*node)->s) < 0)
	{
		insertNode(&(*node)->left, s);
	}
	else if (strcmp(s, (*node)->s) > 0)
	{
		insertNode(&(*node)->right, s);
	}
}

//PostOrder traversal
void postOrder(Node *node,FILE *file)
{
	if (node == NULL)
	{
		return;
	}
	postOrder(node->left, file);
	postOrder(node->right, file);
	
}

//Deallocate BST
void dealloc(Node **node)
{
	if (*node == NULL)
	{
		return;
	}
	dealloc(&(*node)->left);
	dealloc(&(*node)->right);
	free((*node)->s);
	free(*node);
	*node = NULL;
}

//Initialize 
void new_bst(BST *bst) 
{
	bst->root = NULL;
}

//Free up the BST
//**********************
// Had a problem with free(). It would just give me an error.
//**********************
void bst_free(BST *bst) 
{
	dealloc(&bst->root);
	free(bst);
}


int main()
{
	
	BST tree;
	new_bst(&tree);
	
	char input[256];
	
	printf("Strings will be read until 'STOP' is entered.\n");
	while(1)
	{
		printf("Please input a string: ");
		fgets(input, sizeof(input), stdin);
		input[strcspn(input, "\n")] = 0;
		
		if (strcmp(input, "STOP") == 0)
		{
			break;
		}
		insertNode(&tree.root, input);
	}
	printf("The data in the BST will be displayed in postOrder:\n");
	FILE *file = fopen("output.txt", "w");
	if (file)
	{
		postOrder(tree.root, file);
		fclose(file);
	}
	//prints out inputs in postOrder
	printf("The BST in postOrder based on your inputs:\n");
	file = fopen("output.txt", "r");
	if (file)
	{
		while (fgets(input, sizeof(input), file))
		{
			printf("%s", input);
		}
		fclose(file);
	}
	bst_free(&tree);
	
	return 0;
}