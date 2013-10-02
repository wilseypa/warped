#ifndef TREE_ELEMENT_H
#define TREE_ELEMENT_H


#include <iostream>

class Event;
class TreeElement;

class TreeElement {
private:

    TreeElement* leftTree;
    TreeElement* rightTree;
    TreeElement* parentElement;
    const Event* infoNode;

public:

    TreeElement(const Event* bEvent):
        leftTree(NULL),
        rightTree(NULL),
        parentElement(NULL),
        infoNode(bEvent) {}

    ~TreeElement() {}

    inline TreeElement* getLeftElement() {
        return leftTree;
    }

    inline TreeElement* getRightElement() {
        return rightTree;
    }

    inline TreeElement* getParentElement() {
        return parentElement;
    }

    inline void setLeftElement(TreeElement* lElement) {
        leftTree = lElement;
    }

    inline void setRightElement(TreeElement* rElement) {
        rightTree = rElement;
    }

    inline void setParentElement(TreeElement* pElement) {
        parentElement = pElement;
    }

    inline const Event* getEvent() {
        return infoNode;
    }

};

#endif
