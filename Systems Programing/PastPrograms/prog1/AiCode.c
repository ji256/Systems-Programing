#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the Node structure
typedef struct Node {
    char *value;
    struct Node *left;
    struct Node *right;
} Node;

// Define the BST structure
typedef struct {
    Node *root;
} BST;

// Function prototypes
Node* createNode(const char *value);
void insert(Node **node, const char *value);
int search(Node *node, const char *value);
int removeNode(Node **node, const char *value);
void postOrder(Node *node, FILE *file);
void dealloc(Node **node);
void copy(Node *node, Node **dest);
Node* findMin(Node *node);

// Create a new node
Node* createNode(const char *value) {
    Node *newNode = (Node*)malloc(sizeof(Node));
    newNode->value = strdup(value);
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

// Insert a value into the BST
void insert(Node **node, const char *value) {
    if (*node == NULL) {
        *node = createNode(value);
        return;
    }
    if (strcmp(value, (*node)->value) < 0) {
        insert(&(*node)->left, value);
    } else if (strcmp(value, (*node)->value) > 0) {
        insert(&(*node)->right, value);
    }
}

// Perform post-order traversal of the BST
void postOrder(Node *node, FILE *file) {
    if (node == NULL) return;
    postOrder(node->left, file);
    postOrder(node->right, file);
    fprintf(file, "%s\n", node->value);
}

// Deallocate the BST
void dealloc(Node **node) {
    if (*node == NULL) return;
    dealloc(&(*node)->left);
    dealloc(&(*node)->right);
    free((*node)->value);
    free(*node);
    *node = NULL;
}

// Initialize a new BST
void bst_init(BST *bst) {
    bst->root = NULL;
}

// Free the resources of the BST
void bst_free(BST *bst) {
    dealloc(&bst->root);
    free(bst);
}

// Main function to demonstrate the BST operations
int main() {
    BST tree;
    bst_init(&tree);

    char input[256];

    printf("This program will read strings until 'STOP' is entered.\n");

    while (1) {
        printf("Enter a string: ");
        fgets(input, sizeof(input), stdin);
        // Remove newline character from fgets
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "STOP") == 0) {
            break;
        }

        insert(&tree.root, input);
    }

    printf("The data in the BST will now be displayed in post-order:\n");
    FILE *file = fopen("output.txt", "w");
    if (file) {
        postOrder(tree.root, file);
        fclose(file);
    }

    // Optional: Read from the file and print to console
    printf("Contents of the BST (post-order):\n");
    file = fopen("output.txt", "r");
    if (file) {
        while (fgets(input, sizeof(input), file)) {
            printf("%s", input);
        }
        fclose(file);
    }

    bst_free(&tree);

    return 0;
}