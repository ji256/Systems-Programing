#include <stdio.h>


int main() 
{

int Num;

printf("type a number: ");

scanf("%d", &Num);

if (Num > 100)
{
	 main();
}
else
{

printf("Your number is: %d\n", Num);
}
	return 0;
}                   
/*
Struct BST
{
	struct Node *root;
};
int insertNode(struct Node ** node, const char * s)
{
	if (strcmp((*node)->value, s) < 0)
	{
		return insertNode(&(*node)->left),s);
	}
}
int insert(struct BST *bst, const char * s);
{
	return insertNode(&(bst->root), s)
}
*/