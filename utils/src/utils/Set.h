#ifndef SET_H
#define SET_H

// See copyright notice in file Copyright in the root directory of this archive.

#include <iostream>
#include <cstring>

namespace utils {

  /** This class maintains a distinct set of pointers to the objects it's 
      templatized on.  Functions are provided to add and reome things from
      the set, to get the number of objects in the set, and walk the members
      of the set.  NOTE:  Since only pointers are kept within the set, care
      must be taken when "delete"ing members of the of set and so forth. 
      NOTE2: This class predates the common usage of the STL.
  */

template <class type>
class Set{

public:
  /** Constructor */
  Set( int starting_size = 4 );
  /** Constructor */
  Set( type *, int starting_size = 4 );
  /** Constructor */
  Set( const Set<type> & );

  void init( int starting_size );

  virtual ~Set();
  
  /** Add a member to the Set. */
  virtual void add( type * );
  /* Add all of that's members to this. */
  virtual void add( Set<type> *that );

  virtual void remove( type * );
  virtual int size() const;
  virtual type *getElement();
  virtual type *getNextElement();
  
  // See if this thingy is in the Set.
  virtual bool contains( type * );
  
  // Reset the list so it's prisitine once again...
  virtual void reset( int );

  virtual Set<type> &operator=( const Set<type> &that );

  virtual void intersect( Set<type> *that );

protected:
  mutable int num;
  type **data_array;

private:
  void init( int starting_size, bool need_to_clear_array );

  virtual int find( type * );
  
  void duplicate( const Set<type> & );

  int array_size;
  int get_pos;
};


template <class type>
inline
void 
Set<type>::init( int starting_size ){
  init( starting_size, true );
}

template <class type>
inline
void 
Set<type>::init(int starting_size, bool need_to_clear_array ){

  if( starting_size == 0 ){
    starting_size = 1;
  }

  get_pos = 0;

  array_size = starting_size;
  num = 0;

  data_array = new type*[array_size];

  if( need_to_clear_array == true ){
    memset( data_array, 0, array_size );
  }
}

template <class type>
inline
Set<type>::Set(int starting_size){
  init( starting_size );
}

template <class type>
inline
Set<type>::Set( type *first_element, int starting_size ){
  init( starting_size );

  add( first_element );
}

template <class type>
inline
void 
Set<type>::duplicate( const Set<type> &that ){
  delete [] data_array;

  int that_num = that.size();
  init( that_num, false );
  memcpy( data_array, that.data_array, that_num*sizeof( type * ) );
  num = that_num;
}

template <class type>
inline
Set<type>::Set( const Set<type> &that ){
  // Assure we're not deleting garbage in the call to duplicate Set
  data_array = 0;
  duplicate( that );
}

template <class type>
inline
Set<type> &
Set<type>::operator=( const Set<type> &that ){
  duplicate( that );

  return *this;
}

template <class type>
inline
void 
Set<type>::reset(int starting_size){
  delete [] data_array;

  init( starting_size );
}

template <class type>
inline
Set<type>::~Set(){
  delete [] data_array;
}

template <class type>
inline
void 
Set<type>::add( type *to_add ){
  if( contains( to_add ) == false ){
    if( num == array_size - 1 ){
      // Then we already have a full array - allocate more space.
      type **temp_array = new type*[ 2*array_size ];
      
      memcpy( temp_array, data_array, num*sizeof( type * ) );

      delete [] data_array;
      data_array = temp_array;
      array_size = 2*array_size;
    }

    data_array[ num ] = to_add;
    num++;
  }
  //  else {
  // cout << *to_add << " already in Set" << endl;
  //  }
}

template <class type>
inline
void 
Set<type>::add( Set<type> *that ){
  type *current = that->getElement();
  while( current != NULL ){
    add( current );
    current = that->getNextElement();
  }
}

template <class type>
inline
void 
Set<type>::remove( type *to_remove ){
  int pos = find( to_remove );
  if( pos != -1 ){
    // Then we actually had one of these in our Set.
    
    // This is so our next "getElement" does the right thing...
    if( get_pos >= pos ){
      get_pos--;
    }

    // Shift all of the pointers down a slot.    
    memmove( data_array + pos, data_array + pos + 1, (num - pos - 1) * sizeof( type * ) );
//     int i;
//     for( i = pos; i < num - 1; i++ ){
//       data_array[ i ] = data_array[ i + 1 ];
//     }

    num--;
  }
}

template <class type>
inline
int
Set<type>::find( type *to_find ){
  int i;
  bool found_it = false;

  for( i = 0; i < num; i++ ){
    if( data_array[i] == to_find ){
      found_it = true;
      break;
    }
  }

  if( found_it == true ){
    return i;
  }
  else{
    return -1;
  }
}

template <class type>
inline
bool 
Set<type>::contains( type *test_for ){
  int pos = find( test_for );

  if( pos == -1 ){
    return false;
  }
  else{
    return true;
  }
}

template <class type>
inline
type *
Set<type>::getElement(){
  if( num > 0 ){
    get_pos = 0;
    return data_array[ get_pos ];
  }
  else{
    return NULL;
  }
}

template <class type>
inline
type *
Set<type>::getNextElement(){
  get_pos++;
  if( get_pos >= num ){
    return NULL;
  }
  else{
    return data_array[ get_pos ];
  }
}


// This function takes two pointers to Sets, performs an intersection on the
// Sets, and creates a new Set (allocates memory) holding the intersection.
// The two Sets that are passed in are unaffected.  (Aside from their 
// internal position pointer being modified.)
template <class type>
inline
void
Set<type>::intersect( Set<type> *that ){
  if( that == 0 ){
    reset(0);
  }
  else{
    type *current = getElement();
    while( current != 0 ){
      if( !(that->contains( current )) ){
	remove( current );
      }
      current = getNextElement();
    }
  }
}

template <class type>
inline
int 
Set<type>::size() const {
  return num;
}
}
#endif
