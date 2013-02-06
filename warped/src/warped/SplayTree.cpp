// See copyright notice in file Copyright in the root directory of this archive.

#include <iostream>
#include "Event.h"
#include "SplayTree.h"

using namespace std;

SplayTree::SplayTree() {
   root = NULL;
   current = NULL;
   numberOfElements = 0;
}

SplayTree::~SplayTree() {
  cleanUp();
}

void 
SplayTree::insert( const Event *newElement ){
   TreeElement* node = NULL;
   TreeElement* left = NULL;
   TreeElement* right = NULL;
   ASSERT(newElement != NULL);
   TreeElement* newTreeElement = new TreeElement(newElement);
   numberOfElements++;

   if (root == NULL) {
      root = newTreeElement;
      current = newTreeElement;
      return;
   }

   if(current->getEvent() == NULL) {
      node = root;
      while(node->getLeftElement() != NULL) {
         node = node->getLeftElement();
      }
      current = node;
      ASSERT(current->getEvent() != NULL);
   }

   if(CompareEvent(newElement, current->getEvent()) <= 0) {
      current->setLeftElement(newTreeElement);
      newTreeElement->setParentElement(current);
      current = newTreeElement;
      splay(newTreeElement);
      return;
   }

   node = root;

   while (1) {
      if(CompareEvent(newElement, node->getEvent()) <= 0) {

         left = node->getLeftElement();

         if (left != NULL) {
            node = left;
         }
         else {
            node->setLeftElement(newTreeElement);
            break;
         }
      }
      else {
         right = node->getRightElement();

         if (right != NULL) {
            node = right;
         }
         else {
            node->setRightElement(newTreeElement);
            break;
         }
      }
   }
   newTreeElement->setParentElement(node);
   splay(newTreeElement);
}

const Event *
SplayTree::getEvent() {
   TreeElement* node = NULL;
   TreeElement* left = NULL;
   TreeElement* right = NULL;
   TreeElement* parent = NULL;

   if (root == NULL) {
      return NULL;
   }

   node = current;
   if (node == root) {
      root = root->getRightElement();
      current = root;

   }
   else {
      current = node->getParentElement();
      parent = node->getParentElement();
      right = node->getRightElement();
      parent->setLeftElement(right);
      if (right != NULL)  {
         right->setParentElement(parent);
      }
   }

   if(current != NULL) {
      while (1) {
         left = current->getLeftElement();
         if (left == NULL) {
            break;
         }
         current = left;
      }
   }

   processedElements.insert(node);
   numberOfElements--;

   return node->getEvent();
}

const Event*
SplayTree::peekEvent() {

  if( (root == NULL) || (numberOfElements == 0) ) {
    root = current = NULL;
    numberOfElements = 0;
    return NULL;

  } else {
    if(current == NULL) {
      TreeElement *node = root;
      while(node->getLeftElement() != NULL) {
        node = node->getLeftElement();
      }
      current = node;
    }
    return current->getEvent();
  }
}

void
SplayTree::cleanUp() {
   // delete all the elements in processedElement's stack
   // in sequential Simulation, we delete all the 
   // messages that have been processed
   TreeElement* treeElementPtr = processedElements.remove();

   while( treeElementPtr != NULL ) {

      //deleting the Event pointed by the tree element
      //delete treeElementPtr->getEvent();

      //deleting the tree element
      delete treeElementPtr;

      treeElementPtr = processedElements.remove();
   }
}

void
SplayTree::clear() {
   cleanUp();

   if(root == NULL) {
      return;
   }

   while(current != NULL) {
      erase(current->getEvent());
   }

   numberOfElements = 0;
}

const Event *
SplayTree::end() {
   return NULL;
}

void
SplayTree::erase(const Event *delEvent) {

   TreeElement *delNode = NULL;
   TreeElement *node    = NULL;
   TreeElement *left    = NULL;
   TreeElement *right   = NULL;

   if(delEvent == NULL) {
      cout << "Erase received NULL event." << endl;
      return;
   }

   if(root == NULL) {
      if(current != NULL) {
         current = NULL;
         cout << "1.Something is wrong with erase." << endl;
      }
      return;
   }

   if(current == NULL) {
      cout << "2.Something is wrong with erase." << endl;
      node = root;
      while(node->getLeftElement() != NULL) {
         node = node->getLeftElement();
      }
      current = node;

      return;
   }

   /* Search for the event to be deleted */

   /* Also handles the condition when current = root */
   if( CompareEvent(delEvent, current->getEvent()) <= 0) {
      node = current;
   } else {
      node = root;
   }

   while(node != NULL) {

      if( CompareEvent(delEvent, node->getEvent()) < 0 ) {
         node = node->getLeftElement();
      } else if( CompareEvent(delEvent, node->getEvent()) == 0 ) {
         break;
      } else {
         node = node->getRightElement();
      }
   }

   if(node == NULL) {
      return;
   }

   /* Remove the node if found */
   if(node != root) {
      splay(node);
      if(node != root) {
         cout << "Splay not working properly." << endl;
         return;
      }
   }

   left  = root->getLeftElement();
   right = root->getRightElement();

   /* Deallocate the root */
   //delete root->getEvent();
   delete root;
   numberOfElements--;

   /* Re-construct the tree */
   if(left != NULL) {
      root = left;
      root->setParentElement(NULL);

      if(right != NULL) {
         while(left->getRightElement() != NULL) {
            left = left->getRightElement();
         }
         left->setRightElement(right);
         right->setParentElement(left);
      }
   } else {
      root = right;

      if(root != NULL) {
         root->setParentElement(NULL);
      }
   }

   /* Re-calculate the current */
   if(root != NULL) {
      current = root;
      while(current->getLeftElement() != NULL) {
         current = current->getLeftElement();
      }
   }
}

int
SplayTree::size() {
   return numberOfElements;
}

void
SplayTree::splay(TreeElement* node) {

   TreeElement* parent = NULL;
   TreeElement* grand_parent = NULL;
   int nsplay = 0;
   int parent_flag;
   int grand_flag;
   int maxSplay = numberOfElements / 2;

   while(node != root) {

      parent = node->getParentElement();

      if (parent == root) {
         if (parent->getLeftElement() == node) {
            rotateRight(parent);
         }else{
            rotateLeft(parent);
         }
         break;
      }else{
         if (parent->getLeftElement() == node) {
            parent_flag = 0;
         }else{
            parent_flag = 1;
         }
         grand_parent = parent->getParentElement();
         if (grand_parent->getLeftElement() == parent) {
            grand_flag = 0;
         }else{
            grand_flag = 1;
         }
         if ((parent_flag + grand_flag) == 1) {
            //...... Zig Zag
            if (parent_flag == 0) {
               rotateRight(parent);
               rotateLeft(grand_parent);
            }else{
               rotateLeft(parent);
               rotateRight(grand_parent);
            }
         }else{
            //...... Zig Zig
            if (parent_flag == 0) {
               rotateRight(grand_parent);
               rotateRight(parent);
            }else{
               rotateLeft(grand_parent);
               rotateLeft(parent);
            }
         }
      }

      if (nsplay >= maxSplay) {
         break;
      }
      nsplay++;
   }
}

TreeElement* 
SplayTree::rotateLeft(TreeElement* treeItem) {

   TreeElement* right = NULL;
   TreeElement* left = NULL;
   TreeElement* parent = NULL;

   if (treeItem == NULL) {
      return treeItem;
   }
   right = treeItem->getRightElement();
   if (right == NULL) {
      return treeItem;
   }
   left = right->getLeftElement();
   parent = treeItem->getParentElement();

   treeItem->setParentElement(right);
   right->setLeftElement(treeItem);

   treeItem->setRightElement(left);
   if (left != NULL) {
      left->setParentElement(treeItem);
   }

   if (treeItem == root) {
      root = right;
      root->setParentElement(NULL);
   }
   else{
      right->setParentElement(parent);
      if (parent->getLeftElement() == treeItem) {
         parent->setLeftElement(right);
      }
      else{
         parent->setRightElement(right);
      }
   }
   return right;
}

TreeElement* 
SplayTree::rotateRight(TreeElement* treeItem) {

   TreeElement* right = NULL;
   TreeElement* left = NULL;
   TreeElement* parent = NULL;

   if (treeItem == NULL) {
      return treeItem;
   }
   left = treeItem->getLeftElement();
   if (left == NULL) {
      return treeItem;
   }
   right = left->getRightElement();

   parent = treeItem->getParentElement();
   treeItem->setParentElement(left);
   left->setRightElement(treeItem);

   treeItem->setLeftElement(right);
   if (right != NULL) {
      right->setParentElement(treeItem);
   }

   if (treeItem == root) {
      root = left;
      root->setParentElement(NULL);
   }
   else{
      left->setParentElement(parent);
      if (parent->getLeftElement() == treeItem) {
         parent->setLeftElement(left);
      }
      else{
         parent->setRightElement(left);
      }
   }
   return left;
}

const string &
SplayTree::getType() {
  static const string splayTreeType = "SplayTree";
  return splayTreeType;
}
