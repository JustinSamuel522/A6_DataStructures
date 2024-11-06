#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Node structure representing each rectangular block or cutline
typedef struct Node {
    int label;            // Label of the block if it's a leaf node, or 0 if it's a cutline
    int width, height;    // Dimensions of the block or smallest enclosing rectangle
    int x, y;             // Coordinates for the bottom-left corner of the block (only for leaf nodes)
    char type;            // 'H' for horizontal cut, 'V' for vertical cut, '\0' for leaf nodes
    struct Node *left, *right;
} Node;

// Function prototypes
Node *createNode(int label, int width, int height);
Node *buildTree(FILE *input);
void preorderTraversal(Node *root, FILE *outputFile);
void computeDimensions(Node *root);
void writeDimensions(Node *root, FILE *outputFile);
void computeCoordinates(Node *root, int x, int y);
void writeCoordinates(Node *root, FILE *outputFile);
void freeTree(Node *root);

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s in_file out_file1 out_file2 out_file3\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    if (!input) {
        return 1;
    }

    FILE *outputFile1 = fopen(argv[2], "w");
    FILE *outputFile2 = fopen(argv[3], "w");
    FILE *outputFile3 = fopen(argv[4], "w");
    if (!outputFile1 || !outputFile2 || !outputFile3) {
        fclose(input);
        return 1;
    }

    // Build the binary tree from the input file
    Node *root = buildTree(input);

    // Output pre-order traversal to first output file
    preorderTraversal(root, outputFile1);

    // Compute dimensions of enclosing rectangles
    computeDimensions(root);
    writeDimensions(root, outputFile2);

    // Calculate coordinates for each rectangle
    computeCoordinates(root, 0, 0);
    writeCoordinates(root, outputFile3);

    // Clean up
    freeTree(root);
    fclose(input);
    fclose(outputFile1);
    fclose(outputFile2);
    fclose(outputFile3);

    return 0;
}

// Creates a new node (either a leaf or internal)
Node *createNode(int label, int width, int height) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->label = label;
    node->width = width;
    node->height = height;
    node->x = node->y = 0; // Default coordinates
    node->type = '\0';     // Initialize as a leaf node
    node->left = node->right = NULL;
    return node;
}

// Builds the tree from input file (post-order traversal)
Node *buildTree(FILE *input) {
    Node *stack[1000];  // Assume a maximum of 1000 nodes for simplicity
    int stackTop = -1;
    char line[20];

    while (fgets(line, sizeof(line), input)) {
        if (line[0] == 'H' || line[0] == 'V') {
            Node *node = createNode(0, 0, 0);
            node->type = line[0];
            node->right = stack[stackTop--];
            node->left = stack[stackTop--];
            stack[++stackTop] = node;
        } else {
            int label, width, height;
            sscanf(line, "%d(%d,%d)", &label, &width, &height);
            Node *node = createNode(label, width, height);
            stack[++stackTop] = node;
        }
    }

    return stack[stackTop];
}

// Pre-order traversal of the tree to output the structure
void preorderTraversal(Node *root, FILE *outputFile) {
    if (!root) return;
    if (root->type == '\0') { // Leaf node
        fprintf(outputFile, "%d(%d,%d)\n", root->label, root->width, root->height);
    } else { // Internal node
        fprintf(outputFile, "%c\n", root->type);
    }
    preorderTraversal(root->left, outputFile);
    preorderTraversal(root->right, outputFile);
}

// Computes dimensions for each node (smallest enclosing rectangle for non-leaf nodes)
void computeDimensions(Node *root) {
    if (!root || root->type == '\0') return; // Base case for leaf node
    computeDimensions(root->left);
    computeDimensions(root->right);

    if (root->type == 'H') {
        root->width = (root->left->width > root->right->width) ? root->left->width : root->right->width;
        root->height = root->left->height + root->right->height;
    } else if (root->type == 'V') {
        root->width = root->left->width + root->right->width;
        root->height = (root->left->height > root->right->height) ? root->left->height : root->right->height;
    }
}

// Writes dimensions of each node in post-order to the output file
void writeDimensions(Node *root, FILE *outputFile) {
    if (!root) return;
    writeDimensions(root->left, outputFile);
    writeDimensions(root->right, outputFile);

    if (root->type == '\0') {
        fprintf(outputFile, "%d(%d,%d)\n", root->label, root->width, root->height);
    } else {
        fprintf(outputFile, "%c(%d,%d)\n", root->type, root->width, root->height);
    }
}

// Computes coordinates for each rectangular block
void computeCoordinates(Node *root, int x, int y) {
    if (!root) return;
    if (root->type == '\0') { // Leaf node
        root->x = x;
        root->y = y;
    } else if (root->type == 'H') { // Horizontal cut
        computeCoordinates(root->left, x, y + root->right->height);
        computeCoordinates(root->right, x, y);
    } else if (root->type == 'V') { // Vertical cut
        computeCoordinates(root->left, x, y);
        computeCoordinates(root->right, x + root->left->width, y);
    }
}

// Writes coordinates of each rectangular block to the output file
void writeCoordinates(Node *root, FILE *outputFile) {
    if (!root) return;
    if (root->type == '\0') { // Leaf node
        fprintf(outputFile, "%d((%d,%d)(%d,%d))\n", root->label, root->width, root->height, root->x, root->y);
    }
    writeCoordinates(root->left, outputFile);
    writeCoordinates(root->right, outputFile);
}

// Frees the memory allocated for the tree
void freeTree(Node *root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}
