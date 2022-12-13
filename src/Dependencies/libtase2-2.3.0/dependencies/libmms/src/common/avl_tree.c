/*
 *  avl_tree.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2020 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "libmms_platform_includes.h"


typedef struct sAvlTreeNode* AvlTreeNode;

struct sAvlTreeNode {
    void* key;
    void* value;

    /* collision keys will be stored at the right side */

    AvlTreeNode left;
    AvlTreeNode right;

    int height;
};

typedef struct sAvlTree* AvlTree;

struct sAvlTree {
    AvlTreeNode root;
};

AvlTreeNode
AvlTreeNode_create(void* key, void* value)
{
    AvlTreeNode self = (AvlTreeNode) GLOBAL_CALLOC(1, sizeof(struct sAvlTreeNode));

    if (self) {
        self->key  = key;
        self->height = 1;

        if (value)
            self->value = value;
        else
            self->value = key;

        self->left = NULL;
        self->right = NULL;
    }

    return self;
}

AvlTree
AvlTree_create()
{
    AvlTree self = (AvlTree) GLOBAL_CALLOC(1, sizeof(struct sAvlTree));

    return self;
}

static int
getMax(int a, int b)
{
    if (a > b)
        return a;
    else
        return b;
}

static int
nodeHeight(AvlTreeNode self)
{
    if (self)
        return self->height;
    else
        return 0;
}

static AvlTreeNode
rightRotate(AvlTreeNode y)
{
    AvlTreeNode x = y->left;
    AvlTreeNode t2 = x->right;

    x->right = y;
    y->left = t2;

    y->height = 1 + getMax(nodeHeight(y->left), nodeHeight(y->right));
    x->height = 1 + getMax(nodeHeight(x->left), nodeHeight(x->right));

    return x;
}

static AvlTreeNode
leftRotate(AvlTreeNode x)
{
    AvlTreeNode y = x->right;
    AvlTreeNode t2 = y->left;

    y->left = x;
    x->right = t2;

    x->height = 1 + getMax(nodeHeight(x->left), nodeHeight(x->right));
    y->height = 1 + getMax(nodeHeight(y->left), nodeHeight(y->right));

    return y;
}

AvlTreeNode
AvlTreeNode_add(AvlTreeNode self, AvlTreeNode node)
{
    if (self == NULL)
        return node;

    if (node->key < self->key) {
        self->left = AvlTreeNode_add(self->left, node);
    }
    else {
        self->right = AvlTreeNode_add(self->right, node);
    }

    /* update node height */
    self->height = 1 + getMax(nodeHeight(self->left), nodeHeight(self->right));

    int balance = nodeHeight(self->left) - nodeHeight(self->right);

    /* left left case */
    if (balance > 1 && (node->key < self->left->key)) {
        return rightRotate(self);
    }
    else if (balance < -1 && (node->key > self->right->key)) {
        return leftRotate(self);
    }
    else if (balance > 1 && (node->key > self->left->key)) {
        self->left = leftRotate(self->left);

        return rightRotate(self);
    }
    else if (balance < -1 && (node->key < self->right->key)) {
        self->right = rightRotate(self->right);
        return leftRotate(self);
    }

    return self;
}

bool
AvlTree_add(AvlTree self, void* key, void* value)
{
    AvlTreeNode node = AvlTreeNode_create(key, value);

    if (node) {
        self-> root = AvlTreeNode_add(self->root, node);
    }

    if (node)
        return true;
    else
        return false;
}

void*
AvlTreeNode_find(AvlTreeNode self, void* key)
{
    if (self == NULL)
        return NULL;

    if (self->key == key) {
        return self->value;
    }
    else if (self->key > key) {
        return AvlTreeNode_find(self->left, key);
    }
    else {
        return AvlTreeNode_find(self->right, key);
    }
}

void*
AvlTree_find(AvlTree self, void* key)
{
    if (self->root) {
        return AvlTreeNode_find(self->root, key);
    }
    else {
        return NULL;
    }
}

static void
AvlTreeNode_destroy(AvlTreeNode self)
{
    if (self->right) {
        AvlTreeNode_destroy(self->right);
    }

    if (self->left) {
        AvlTreeNode_destroy(self->left);
    }

    GLOBAL_FREEMEM(self->value);

    GLOBAL_FREEMEM(self);
}

static void
AvlTreeNode_destroyDeep(AvlTreeNode self, void(*valueDeleteFunction)(void*))
{
    if (self->right) {
        AvlTreeNode_destroyDeep(self->right, valueDeleteFunction);
    }

    if (self->left) {
        AvlTreeNode_destroyDeep(self->left, valueDeleteFunction);
    }

    if (self->value)
        valueDeleteFunction(self->value);

    GLOBAL_FREEMEM(self);
}

uint32_t
AvlTree_getMaxDepth(AvlTree self)
{
    int maxDepth = 0;
    int depth = 0;

    if (self->root) {
        maxDepth = self->root->height;
    }

    return maxDepth;
}

void
AvlTree_destroy(AvlTree self)
{
    if (self->root) {
        AvlTreeNode_destroy(self->root);
    }

    GLOBAL_FREEMEM(self);
}

void
AvlTree_destroyDeep(AvlTree self, void(*valueDeleteFunction)(void*))
{
    if (self->root) {
        AvlTreeNode_destroyDeep(self->root, valueDeleteFunction);
    }

    GLOBAL_FREEMEM(self);
}
