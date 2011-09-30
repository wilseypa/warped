#ifndef FACTORY_SYMBOL_TABLE_H
#define FACTORY_SYMBOL_TABLE_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "Factory.h"

class FactorySymbolTable;

class ContainerClass {
  friend class FactorySymbolTable;
public:
  ContainerClass() {};
  ~ContainerClass(){};

private:
  ContainerClass *next;
  Factory *data;
};


/** The FactorySymbolTable class.
    
    This class implements a simple symbol table. The symbol table is a
    wrapper class around a single linked list. Every node in this symbol table 
    should be derived from the Factory abstract base class.

**/
class FactorySymbolTable {
public:
  /**@name Public Class Methods of FactorySymbolTable. */
  //@{

  /** Constructor.

      @param uniqueFlag Flags whether only unique entries will be allowed.
  */
  FactorySymbolTable(bool uniqueFlag);

  /// Destructor.
  ~FactorySymbolTable();

  /** Get a factory-handle that is mathing a symbol.

      @param symbol Symbol to be searched for.
      @return Handle to a factory that matches the symbol.
  */
  Factory* searchSymbol(const string &symbol);

  /** Get a factory-handle that is matching a class.

      @param searchClass The class to be searched for.
      @return Handle to a factory that matches the class.
  */
  Factory* searchSymbol(const Factory *searchClass);

  /** Search the symbol table until match found.

      The search is based on the searchKind kind of search.

      @param searchKind The kind of search to be done.
      @return Handle to a factory that matched the search.
  */
  Factory* iterativeSearch(const Kind searchKind);

  /** Get a handle to a successor factory.

      @return Handle to a successor factory.
  */
  Factory* successor();

  /** Add a new factory class.

      @param newClass The new factory class.
      @return True if added, False if class already exists.
  */
  bool addSymbol(Factory *newClass);

  /** Remove a symbol from the symbol table.

      @param symbol Find a factory matching symbol.
      @return A handle to the matching factory.
  */
  Factory *removeSymbol(const string &symbol);

  /** Remove a symbol from the symbol table.

      @param oldClass Find a factory matching the oldClass.
      @return A handle to the matching factory.
  */
  Factory *removeSymbol(const Factory *oldClass);

  /// Delete all symbols from the symbol table.
  void deleteAllSymbols();

  /** Get the number of symbols in the symbol table.

      @return The number of symbols in the symbol table.
  */
  int getNumberOfSymbols() const;
  //@} // End of Public Class Methods of FActorySymbolTable.

protected:
  /**@name Protected Class Methods of FactorySymbolTable. */
  //@{

  /** Search the symbol table from some start-position.

      The private searchParameter will be used to determine if a match
      has been found for the search.

      @param startPos Pointer to the position where to start search.
      @return A match for the search.
  */
  ContainerClass *searchFrom(ContainerClass *startPos);
  //@} // End of Protected Class Methods of FactorySymbolTable. */

private:
  //*@name Private Class Attributes of FactorySymbolTable. */
  //@{

  /// Pointer to the head of the symbol table.
  ContainerClass *head;

  /// Pointer to the tail of the symbol table.
  ContainerClass *tail;

  /// Pointer to the current position in the symbol table.
  ContainerClass *current;

  /// The kind of search that will be done in searches.
  Kind searchParameter;

  /// True/False - Is only unique entries going to be allowed?
  bool uniqueEntriesOnly;
  //@} // End of Private Class Attributes of FactorySymbolTable.
};

#endif

