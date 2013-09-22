
#include "ObjectStub.h"
#include "FactoryImplementationBase.h"

FactoryImplementationBase::FactoryImplementationBase(FactoryImplementationBase *parent){
   parentFactory = parent;
   mySymbolTable = new FactorySymbolTable(true);
}

FactoryImplementationBase::~FactoryImplementationBase(){
   mySymbolTable->deleteAllSymbols();
   delete mySymbolTable;
}

bool
FactoryImplementationBase::add(ObjectStub *stub){
   return mySymbolTable->addSymbol((Factory *)stub);
}

bool
FactoryImplementationBase::add(FactoryImplementationBase *subFactory){
   return mySymbolTable->addSymbol((Factory *)subFactory);
}

ObjectStub *
FactoryImplementationBase::isObjectPresent( const string &objectName ){

   string *prefix = getPrefix(objectName);
   if(prefix->empty()){ 
      // empty string - return NULL
      delete prefix;
      return NULL;
   }

   Factory *searchResult = NULL;

   if((searchResult = mySymbolTable->searchSymbol(*prefix)) == NULL){
      delete prefix;
      return NULL;
   }
   
   if(searchResult->getKind() == STUB) {
      delete prefix;
      return (ObjectStub *)searchResult;
   }

   string *suffix = getSuffix(objectName);
   ObjectStub *stub =
      ((FactoryImplementationBase *)searchResult)->isObjectPresent(*suffix);
   delete suffix;
   return stub;
}

ObjectStub *
FactoryImplementationBase::isObjectPresent(const Factory *objectClass){
   return isObjectPresent(objectClass->getName());
}

FactoryImplementationBase *
FactoryImplementationBase::getParentFactory() const {
   return parentFactory;
}

FactoryImplementationBase *
FactoryImplementationBase::getMainFactory() const {
   FactoryImplementationBase *parent = parentFactory;

   while (parent->getParentFactory() != NULL) {
      parent = parent->getParentFactory();
   }

   return parent;
}

string
FactoryImplementationBase::listAll(const string &prefix){
  Factory *base;

  string newPrefix = prefix + getName();
  string list;

  base = mySymbolTable->iterativeSearch(ANY);
  while (base != NULL) {
    if (base->getKind() == STUB) {
      list += newPrefix + "." + base->getName();
      list += " [" + ((ObjectStub *)base)->getInformation() + "]\n";
    }
    else {
      list +=
	((FactoryImplementationBase *)base)->listAll(newPrefix);
    }
    
    base = mySymbolTable->successor();
  }

  return list;
}

string * 
FactoryImplementationBase::getPrefix(const string &objectName){
   string *returnString = NULL;
   string::size_type position = objectName.find(".");
   if(position != string::npos){
      // everything before the "." is the prefix
      returnString = new string(objectName.substr(0,position));
   }
   else {
      returnString = new string(objectName);
   }

   return returnString;
}

string *
FactoryImplementationBase::getSuffix(const string &objectName){
   string *returnString = NULL;
   string::size_type position = objectName.find(".");
   if(position != string::npos){
      // everything after the "." is the suffix
      returnString = new string(objectName.substr(position+1));
   }
   else {
      returnString = new string(objectName);
   }

   return returnString;
}

