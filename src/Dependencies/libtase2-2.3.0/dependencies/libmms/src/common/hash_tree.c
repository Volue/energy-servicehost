/*
 *  hash_tree.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2020 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "libmms_platform_includes.h"
#include "string_utilities.h"
#include "hash_tree.h"

static uint32_t
hash_string(const char* str)
{
    unsigned long hash = 0;

    while (*str) {
    	int c = *str;
        hash = c + (hash << 6) + (hash << 16) - hash;

        str++;
    }

    if (hash % 2)
        hash = ~hash;

    if (hash % 3)
        hash = ~(hash >> 1);

    if (hash % 4)
        hash = ~(hash << 1);

    return hash;
}

typedef struct sHashTreeNode* HashTreeNode;

struct sHashTreeNode {
    uint32_t hashKey;  /* require hash function to be generic? */
    char* originalKey; /* require compare function to be generic */
    void* value;

    /* collision keys will be stored at the right side */

    HashTreeNode left;
    HashTreeNode right;

    int height;
};

typedef struct sHashTree* HashTree;

struct sHashTree {
    HashTreeNode root;
};

static HashTreeNode
HashTreeNode_create(uint32_t hashCode, const char* originalKey, void* value)
{
    HashTreeNode self = (HashTreeNode) GLOBAL_CALLOC(1, sizeof(struct sHashTreeNode));

    if (self) {
        self->hashKey = hashCode;
        self->originalKey = StringUtils_copyString(originalKey);
        self->height = 1;

        if (value)
            self->value = value;
        else
            self->value = (void*) originalKey;

        self->left = NULL;
        self->right = NULL;
    }

    return self;
}

HashTree
HashTree_create()
{
    HashTree self = (HashTree) GLOBAL_CALLOC(1, sizeof(struct sHashTree));

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
nodeHeight(HashTreeNode self)
{
    if (self)
        return self->height;
    else
        return 0;
}

static HashTreeNode
rightRotate(HashTreeNode y)
{
    HashTreeNode x = y->left;
    HashTreeNode t2 = x->right;

    x->right = y;
    y->left = t2;

    y->height = 1 + getMax(nodeHeight(y->left), nodeHeight(y->right));
    x->height = 1 + getMax(nodeHeight(x->left), nodeHeight(x->right));

    return x;
}

static HashTreeNode
leftRotate(HashTreeNode x)
{
    HashTreeNode y = x->right;
    HashTreeNode t2 = y->left;

    y->left = x;
    x->right = t2;

    x->height = 1 + getMax(nodeHeight(x->left), nodeHeight(x->right));
    y->height = 1 + getMax(nodeHeight(y->left), nodeHeight(y->right));

    return y;
}

static HashTreeNode
HashTreeNode_add(HashTreeNode self, HashTreeNode node)
{
    if (self == NULL)
        return node;

    if (node->hashKey < self->hashKey) {
        self->left = HashTreeNode_add(self->left, node);
    }
    else {
        self->right = HashTreeNode_add(self->right, node);
    }

    /* update node height */
    self->height = 1 + getMax(nodeHeight(self->left), nodeHeight(self->right));

    int balance = nodeHeight(self->left) - nodeHeight(self->right);

    /* left left case */
    if (balance > 1 && (node->hashKey < self->left->hashKey)) {
        return rightRotate(self);
    }
    else if (balance < -1 && (node->hashKey > self->right->hashKey)) {
        return leftRotate(self);
    }
    else if (balance > 1 && (node->hashKey > self->left->hashKey)) {
        self->left = leftRotate(self->left);

        return rightRotate(self);
    }
    else if (balance < -1 && (node->hashKey < self->right->hashKey)) {
        self->right = rightRotate(self->right);
        return leftRotate(self);
    }

    return self;
}

bool
HashTree_add(HashTree self, const char* key, void* value)
{
    HashTreeNode node = HashTreeNode_create(hash_string(key), key, value);

    if (node) {
        self-> root = HashTreeNode_add(self->root, node);
    }

    if (node)
        return true;
    else
        return false;
}

static void*
HashTreeNode_find(HashTreeNode self, uint32_t hashCode, const char* key)
{
    if (self == NULL)
        return NULL;

    if (self->hashKey == hashCode) {
        if (self->right) {
            if (self->hashKey == self->right->hashKey) {
                if (!(strcmp(self->originalKey, key))) {
                    return self->value;
                }
                else {
                    return HashTreeNode_find(self->right, hashCode, key);
                }
            }
            else {
                return self->value;
            }
        }
        else {
            return self->value;
        }
    }
    else if (self->hashKey > hashCode) {
        return HashTreeNode_find(self->left, hashCode, key);
    }
    else {
        return HashTreeNode_find(self->right, hashCode, key);
    }
}

void*
HashTree_find(HashTree self, const char* key)
{
    if (self->root) {
        uint32_t hashCode = hash_string(key);

        return HashTreeNode_find(self->root, hashCode, key);
    }
    else {
        return NULL;
    }
}

static void
HashTreeNode_destroy(HashTreeNode self)
{
    if (self->right) {
        HashTreeNode_destroy(self->right);
    }

    if (self->left) {
        HashTreeNode_destroy(self->left);
    }

    GLOBAL_FREEMEM(self->originalKey);
    GLOBAL_FREEMEM(self);
}

static void
HashTreeNode_destroyDeep(HashTreeNode self, void(*valueDeleteFunction)(void*))
{
    if (self->right) {
        HashTreeNode_destroyDeep(self->right, valueDeleteFunction);
    }

    if (self->left) {
        HashTreeNode_destroyDeep(self->left, valueDeleteFunction);
    }

    if (self->value)
        valueDeleteFunction(self->value);

    GLOBAL_FREEMEM(self->originalKey);
    GLOBAL_FREEMEM(self);
}


uint32_t
HashTree_getMaxDepth(HashTree self)
{
    int maxDepth = 0;

    if (self->root) {
        maxDepth = self->root->height;
    }

    return maxDepth;
}

void
HashTree_destroy(HashTree self)
{
    if (self->root) {
        HashTreeNode_destroy(self->root);
    }

    GLOBAL_FREEMEM(self);
}

void
HashTree_destroyDeep(HashTree self, void(*valueDeleteFunction)(void*))
{
    if (self->root) {
        HashTreeNode_destroyDeep(self->root, valueDeleteFunction);
    }

    GLOBAL_FREEMEM(self);
}

