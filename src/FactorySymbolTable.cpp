
#include "FactorySymbolTable.h"

FactorySymbolTable::FactorySymbolTable(bool uniqueFlag) {
    head              = NULL;
    tail              = NULL;
    current           = NULL;
    searchParameter   = UNKNOWN;
    uniqueEntriesOnly = uniqueFlag;
}

FactorySymbolTable::~FactorySymbolTable() {
    // delete contents of list
    ContainerClass* deleteNode = head;

    while (head != NULL) {
        head = head->next;
        delete deleteNode;
        deleteNode = head;
    }
}

Factory*
FactorySymbolTable::searchSymbol(const string& symbol) {
    ContainerClass* iterator = head;

    while (iterator != NULL) {
        if (iterator->data->getName() == symbol) {
            return iterator->data;
        }
        iterator = iterator->next;
    }
    return NULL;
}

Factory*
FactorySymbolTable::searchSymbol(const Factory* searchClass) {
    return searchSymbol(searchClass->getName());
}

Factory*
FactorySymbolTable::iterativeSearch(const Kind searchKind) {
    searchParameter = searchKind;
    current = searchFrom(head);

    if (current != NULL) {
        return current->data;
    }

    return NULL;
}

Factory*
FactorySymbolTable::successor() {
    if (current == NULL) {
        return NULL;
    }

    current = searchFrom(current->next);

    if (current != NULL) {
        return current->data;
    }
    return NULL;
}

bool
FactorySymbolTable::addSymbol(Factory* newClass) {
    ContainerClass* newNode;

    if (uniqueEntriesOnly == true) {
        if (searchSymbol(newClass) != NULL) {
            return false;
        }
    }

    if (head == NULL) {
        head       = new ContainerClass;
        head->next = NULL;
        head->data = newClass;
        tail       = head;
    } else {
        newNode       = new ContainerClass;
        newNode->next = NULL;
        newNode->data = newClass;
        tail->next    = newNode;
        tail          = newNode;
    }
    return true;
}

Factory*
FactorySymbolTable::removeSymbol(const string& symbol) {

    ContainerClass* prevNode = head, *iterator = head;
    Factory* returnValue = NULL;

    while (iterator != NULL) {
        if (symbol == iterator->data->getName()) {
            break;
        }

        prevNode = iterator;
        iterator = iterator->next;
    }

    if (iterator == NULL) {
        return NULL;
    }

    if (iterator == head) {
        head = head->next;
        if (head == NULL) {
            tail    = NULL;
        }
    } else {
        if (iterator == tail) {
            tail       = prevNode;
            tail->next = NULL;
        } else {
            prevNode->next = iterator->next;
        }
    }

    returnValue     = iterator->data;
    current         = NULL;
    searchParameter = UNKNOWN;

    delete iterator;

    return returnValue;
}

Factory*
FactorySymbolTable::removeSymbol(const Factory* oldClass) {
    return removeSymbol(oldClass->getName());
}

void
FactorySymbolTable::deleteAllSymbols() {
    ContainerClass* deleteNode = head;

    while (deleteNode != NULL) {
        head = head->next;
        delete deleteNode->data;
        delete deleteNode;
        deleteNode = head;
    }

    tail            = NULL;
    current         = NULL;
    searchParameter = UNKNOWN;
}

int
FactorySymbolTable::getNumberOfSymbols() const {
    ContainerClass* iterator = head;
    int count = 0;

    while (iterator != NULL) {
        count++;
        iterator = iterator->next;
    }

    return count;
}


ContainerClass*
FactorySymbolTable::searchFrom(ContainerClass* startPos) {
    if (searchParameter == UNKNOWN) {
        return NULL;
    }

    if (searchParameter == ANY) {
        return startPos;
    }

    while (startPos != NULL) {
        if (startPos->data->getKind() == searchParameter) {
            return startPos;
        }

        startPos = startPos->next;
    }

    return NULL;
}

