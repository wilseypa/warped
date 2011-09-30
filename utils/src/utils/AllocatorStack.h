#ifndef ALLOCATOR_STACK_H
#define ALLOCATOR_STACK_H

// See copyright notice in file Copyright in the root directory of this archive.

/**
   This class is intended to help with overloading new and delete.  It is
   an implementation of a stack that is specialized in a couple of ways.
   Firstly, it's templatized on size_t.  The template parameter controls
   the size of the memory chunks that this stack is in control of.
   Secondly, calling "pop" on an empty stack will cause a chunk of the
   proper size to be allocated if it had not been previously.

   To use the Allocator stack, your class might overload "new" and "delete"
   in the following manner:
   class foo {
   public:
   void *operator new( size_t size ){
     return AllocatorStack<sizeof(foo)>::pop();
   }
   void operator delete( void *toDelete );
     return AllocatorStack<sizeof(foo)>::push( toDelete );
   }

   Note that you probably don't want to do this for very many classes in
   your programs!  Using this stack means that you will never truly free
   the memory associated with the objects being handled by the
   AllocatorStack.  Let me say this again - MEMORY ALLOCATED THROUGH THE
   USE OF THIS CLASS WILL NEVER BE FREED.  So use it only in places where
   you know that your objects will have an extremely high rate of reuse and
   memory allocation has been a demonstrated bottleneck.
*/

template<size_t size>
class AllocatorStack {
private:
  class Container {
  private:
    void *data;
    Container *next;
    

  public:
    inline Container( void *initData, Container *initNext ) : data( initData ), next( initNext ){}
    inline void *getData(){ return data; }
    inline Container *getNext(){ return next; }
    inline void setNext( Container *newNext ){ next = newNext; }
    /**
       If we don't manage the container list, too, we lose.  This is top of
       the Container free stack.
    */
    inline static Container *&getContainerTop(){
      static Container *containerTop = 0;
      return containerTop;
    }
  };

  inline static Container *&getTop(){
    static Container *top = 0;
    return top;
  }

  inline static Container *getContainer( void *data, Container *next ){
    Container *retval;
    if( Container::getContainerTop() == 0 ){
      retval = new Container( data, next );
    }
    else{
      Container *oldTop = Container::getContainerTop();
      Container::getContainerTop() = oldTop->getNext();
      retval = new (oldTop) Container( data, next );
    }

    return retval;
  }

  inline static void reclaimContainer( Container *newContainer ){
    newContainer->setNext( Container::getContainerTop() );
    Container::getContainerTop() = newContainer;
  }

public:
  inline static void push( void *toPush ){
    getTop() = getContainer( toPush, getTop() );
  }

  inline static void *pop(){
    void *retval;
    if( getTop() == 0 ){
      retval = new char[size];
    }
    else{
      Container *oldTop = getTop();
      retval = oldTop->getData();
      getTop() = oldTop->getNext();
      reclaimContainer( oldTop );
    }
    return retval;
  }
};

#endif
