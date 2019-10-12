/**************************************************************************

This is a code sample from an assignment I did implementing an AVL balanced
tree using a map implementation. Below are the functions used to rotate the
tree when it was unbalanced, as well as how I checked for and implemented
the rebalancing of the tree. It was written in C++ and is documented using
Doxygen file and function headers. This project taught me about the
usefulness of AVL data structure as well as how to utilize elements of the
STL to build more efficient data structures to meet more specific needs.

**************************************************************************/


/**************************************************************************/
/*!
\file   avl-map.cpp
\author Jack Klein
\par    email: jack.klein\@digipen.edu
\par    DigiPen login: jack.klein
\par    Course: CS280
\par    Assignment 4
\date   3/13/2019
\brief
    This file contains the implementation of the functions for
    the AVL map assignment

*/
/**************************************************************************/

/************************************************************************/
/*!
  \brief
    Rotates the tree to the left

  \param oldRoot
    The node node that used to be at the top of where you're rotating about
*/
/************************************************************************/
template< typename KEY_TYPE, typename VALUE_TYPE >
void CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::rotateLeft(Node *oldRoot)
{
  /* Save the old and new roots so we won't lose them */
  Node *newChild = oldRoot;
  Node *newRoot = oldRoot->right;

  /* The new root will lose its child, so we'll yoink it first */
  newChild->right = newRoot->left;
  /* Set the new root's child to be the old root */
  newRoot->left = newChild;

  /* Make the new root the child of the old root's parent if applicable */
  if (oldRoot->parent)
  {
    if (oldRoot->parent->left == oldRoot)
    {
      oldRoot->parent->left = newRoot;
    }
    else
    {
      oldRoot->parent->right = newRoot;
    }
  }

  /* Change some parents */
  newRoot->parent = oldRoot->parent;
  newChild->parent = newRoot;
  if (newChild->right)
  {
    newChild->right->parent = newChild;
  }

  /* Change some depths */
  /* Change these non-recursively so they don't double count anything */
  newRoot->depth--;
  oldRoot->depth ++;
  /* Recursively change the rest */
  incrementDepthRecursive(oldRoot->left);
  decrementDepthRecursive(newRoot->right);

  /* Set roots if necessary */
  if (pRoot == oldRoot)
  {
    pRoot = newRoot;
  }
}

/************************************************************************/
/*!
  \brief
    Rotates the tree to the right

  \param oldRoot
    The node node that used to be at the top of where you're rotating about
*/
/************************************************************************/
template< typename KEY_TYPE, typename VALUE_TYPE >
void CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::rotateRight(Node *oldRoot)
{
  /* Save the old and new roots so we won't lose them */
  Node *newChild = oldRoot;
  Node *newRoot = oldRoot->left; 

  /* The new root will lose its child, so we'll yoink it first */
  newChild->left = newRoot->right;
  /* Set the new root's child to be the old root */
  newRoot->right = newChild;

  /* Make the new root the child of the old root's parent if applicable */
  if (oldRoot->parent)
  {
    if (oldRoot->parent->left == oldRoot)
    {
      oldRoot->parent->left = newRoot;
    }
    else
    {
      oldRoot->parent->right = newRoot;
    }
  }

  /* Change some parents */
  newRoot->parent = oldRoot->parent;
  newChild->parent = newRoot;
  if (newChild->left)
  {
    newChild->left->parent = newChild;
  }

  /* Change some depths */
  /* Change these non-recursively so they don't double count anything */
  newRoot->depth--;
  oldRoot->depth++;
  /* Recursively change the rest */
  incrementDepthRecursive(oldRoot->right);
  decrementDepthRecursive(newRoot->left);

  /* Set roots if necessary */
  if (pRoot == oldRoot)
  {
    pRoot = newRoot;
  }
}

/************************************************************************/
/*!
  \brief
    Checks the balance of the tree at all nodes in the given stack

  \param nodeStack
    A stack of nodes to check the balance of

  \return
    Whether or not the tree must be rebalanced
*/
/************************************************************************/
template< typename KEY_TYPE, typename VALUE_TYPE >
bool CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::needsRebalancing(std::stack<Node *> &nodeStack) const
{
  /* Check all the nodes in the stack */
  while (!nodeStack.empty())
  {
    /* Get the node to check */
    Node *nodeToCheck = nodeStack.top();

    /* Check the height of the left and right subtree */
    int leftHeight = getHeightRecursive(nodeToCheck->left);
    int rightHeight = getHeightRecursive(nodeToCheck->right);

    /* Check if the heights differ by two, if so then it is unbalanced */
    if (abs(leftHeight - rightHeight) >= 2)
    {
      return true;
    }

    /* Otherwise move on to the next node in the stack */
    nodeStack.pop();
  }

  /* Stack is empty, return that it is balanced */
  return false;
}

/************************************************************************/
/*!
  \brief
    Rebalances the tree at a given node

  \param problemChild
    The point where the tree becomes unbalanced, point of rebalancing
*/
/************************************************************************/
template< typename KEY_TYPE, typename VALUE_TYPE >
void CS280::AVLmap<KEY_TYPE, VALUE_TYPE>::rebalance(Node *problemChild)
{
  Node *heavyChild = problemChild->right; /* The heavy side of the unbalanced tree   */
  Node *lightChild = problemChild->left;  /* The lighter side of the unbalanced tree */

  /* If that random assignment of the heavy side was wrong, switch all the sides */
  if (getHeightRecursive(lightChild) > getHeightRecursive(heavyChild))
  {
    Node *temp = heavyChild;
    heavyChild = lightChild;
    lightChild = temp;

    /* Check whether the height is greater on the left or right of heavy side */
    if (getHeightRecursive(heavyChild->right) > getHeightRecursive(heavyChild->left))
    {
      /* LR unbalanced tree, rotate left then right */
      rotateLeft(heavyChild);
      rotateRight(problemChild);
    }
    else
    {
      /* LL unbalanced tree, rotate right */
      rotateRight(problemChild);
    }
  }
  else
  {
    /* Check whether the height is greater on the left or right of heavy side */
    if (getHeightRecursive(heavyChild->left) > getHeightRecursive(heavyChild->right))
    {
      /* RL unbalanced tree, rotate right then left */
      rotateRight(heavyChild);
      rotateLeft(problemChild);
    }
    else
    {
      /* RR unbalanced tree, rotate left */
      rotateLeft(problemChild);
    }
  }
}