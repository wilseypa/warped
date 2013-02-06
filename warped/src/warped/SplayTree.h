#ifndef SPLAY_TREE_H
#define SPLAY_TREE_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "TreeElement.h"
#include "StackOfPtr.h"
#include "EventSet.h"
#include "Event.h"

/** The SplayTree class.

    The SplayTree class is an implementation of the abstract
    EventSet class. It implemented a splay tree data structure
    that is used as an event queue in a sequential simulation. 

*/
class SplayTree : public EventSet {

public:

  /**@name Public Class Methods of SplayTree. */
  //@{

  /// Default Constructor.
  SplayTree();

  /// Destructor.
  ~SplayTree();

  /** Insert an event into the event set.

      @param event A pointer to the event to insert.
  */
  void insert( const Event *event );
   
  /** Return a reference to the first event and remove from the event set.

      @return A reference to the first event in the event set.
  */
  const Event *getEvent();

  /** Return reference to first event without removing it from the event set.

      @return A reference to the first event in the event set.
  */
  const Event *peekEvent();

  /// Delete any old, unwanted events.
  void cleanUp();

  /// Delete the entire splay tree
  void clear();

  /** Return a reference to the end of the event set.

      @return A reference to the end of the event set.
  */
  const Event *end();

  /** Delete an event from the event set.

      @param event A pointer to the event to delete.
  */
  void erase(const Event *delEvent);

  /** Return the size of the splay tree.

      @return The count of elements in splay tree.
  */
  int size();

  virtual void configure( SimulationConfiguration & ){}

  static const string &getType();

  //@} // End of Public Class Methods of SplayTree

private:

  /**@name Private Class Methods of SplayTree. */
  //@{

  /** Internal data structure functions - splay the tree.

      @param event Splayed event.
  */
  void splay( TreeElement *event);

  /** Internal data structure functions - rotate to the left.

      @param treeItem Tree item on which to rotate.
      @return A pointer to the new root of the splay tree.
  */
  TreeElement *rotateLeft(TreeElement *treeItem);

  /** Internal data structure functions - rotate to the right.

      @param treeItem Tree item on which to rotate.
      @return A pointer to the new root of the splay tree.
  */
  TreeElement *rotateRight(TreeElement *treeItem);

  /// The compareEvent method for SplayTree...
  inline int CompareEvent(const Event *a, const Event *b){
    ASSERT(a != NULL);
    ASSERT(b != NULL);
    if (a->getReceiveTime() == b->getReceiveTime()) {
      //return ((a->getReceiver() > b->getReceiver() ? 1 : -1));
      if(a->getReceiver() > b->getReceiver()) {
        return 1;
      } else if(a->getReceiver() == b->getReceiver()) {
        return 0;
      } else {
        return -1;
      }
    }
    else {
      return ((a->getReceiveTime() > b->getReceiveTime()) ? 1 : -1);
    }
  }
   
  //@} End of Private Class Methods of SplayTree.

  /**@name Private Class Attributes of SplayTree. */
  //@{

  /// Tree Element Denoting the root of the SplayTree.
  TreeElement *root;
   
  /**
     Tree Element denoting the current Element that is with the
     lowest timestamp and is the current (next) element to be processed
  */
  TreeElement *current;

  /// List of processed elements that can be deleted.
  StackOfPtr<TreeElement> processedElements;

  /// Number of elements in the event set.
  int numberOfElements;

  //@} // End of  Private Class Attributes of SplayTree
};

#endif
