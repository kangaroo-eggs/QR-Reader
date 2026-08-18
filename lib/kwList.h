#ifndef KWLIST_H
#define KWLIST_H

#include "kwNode.h"

/////////////////////////////////////////////////////////////////////////////////
/// @brief
///
template <typename NodeType>
class kwList
{
public:
    kwNode<NodeType>        *headPtr;   // always point to the headnode
    kwNode<NodeType>        *endPtr;
    int                     numOfListItems;
    int                     numOfMaxListItems = 10000;


    //-------------------------------------
    kwList();
    ~kwList();
    void                    Clear();
    inline void             SetMaxNumOfItem( int _MaxNumOfItems )  {  numOfMaxListItems = _MaxNumOfItems; }

    //-------------------------------------
    /// @brief default will insert after last
    void                    InsertNode( kwNode<NodeType> *newNode, kwNode<NodeType> *referNode = nullptr );

    /// @brief sorting by insertion sort
//    void                    InsertSORTED( kwNode<NodeType> *InsertNode) ;

    /// @brief default will remove the last node from list, and return that node's copy
    void                    RemoveNode( kwNode<NodeType> *nodeToRemove = nullptr );

    /// @brief remove all node after lastNodeToKeep(not include), and return how many node it deletes
    int                     RemoveNodesAfter( kwNode<NodeType> *lastNodeToKeep );


//    /// @brief remove first node, and return that node's copy
//    NodeType          RemoveFirst();

//    void                    SwitchNode( kwNode<NodeType> *node1, kwNode<NodeType> *node2 );
    void                    Combine( kwList &srcList );

    //-------------------------------------
    kwNode<NodeType>*       operator()( int IndexOfTheList );
    kwList<NodeType>&       operator=( const kwList<NodeType> &srcList );

};


/////////////////////////////////////////////////////////////////////////////////
// below is implementation, since we can't put these code into .cpp file

//-------------------------------------
template <typename NodeType>
kwList<NodeType>::kwList()
{
    this->headPtr = this->endPtr = new kwNode<NodeType>;   // use kwNode's constructor
    this->numOfListItems = 0;
}

//-------------------------------------
template <typename NodeType>
kwList<NodeType>::~kwList()
{
    this->RemoveNodesAfter( this->headPtr );
    delete this->headPtr;
}

//-------------------------------------
template <typename NodeType>
void kwList<NodeType>::InsertNode( kwNode<NodeType> *newNode, kwNode<NodeType> *referNode )
{
    if(this->numOfListItems == this->numOfMaxListItems){
        cout << "List is full, can't add Node" << endl;
        return;
    }

    kwNode<NodeType> *insertNode = new kwNode<NodeType>(*newNode);

    // insert at last
    if (referNode == nullptr || referNode == this->endPtr){
        referNode = this->endPtr;
        referNode->next = insertNode;
        insertNode->prev = referNode;
        insertNode->next = nullptr;
        this->endPtr = insertNode;
    }
    else{
        referNode->next->prev = insertNode;
        insertNode->next = referNode->next;
        insertNode->prev = referNode;
        referNode->next = insertNode;
    }
    this->numOfListItems++;
}

////-------------------------------------
//template <typename NodeType>
//void kwList<NodeType>::InsertSORTED(kwNode<NodeType> *InsertNode) {
//    NodeType *newNodePtr = new NodeType;
//    *newNodePtr = *InsertNode;  // use copy constructor
//    NodeType *ptr = this->headPtr, *MIN_ptr = this->endPtr;

//    for (int i = 0; i < this->numOfListItems; ++i) {
//        ptr = ptr->next;
//        if ((ptr->rank) < newNodePtr->rank) {
//            break;
//        }
//        if(ptr == this->endPtr){    // if the inserted node has min rank
//            MIN_ptr = newNodePtr;
//        }
//    }

//    if (MIN_ptr == newNodePtr){
//        this->endPtr->next = newNodePtr;
//        newNodePtr->prev = this->endPtr;
//        newNodePtr->next = nullptr;
//    }
//    else {
//        newNodePtr->next = ptr;
//        newNodePtr->prev = ptr->prev;
//        ptr->prev->next = newNodePtr;
//        ptr->prev = newNodePtr;
//    }
//    this->numOfListItems++;

//    if (this->numOfMaxListItems < this->numOfListItems) {
//      this->RemoveNode(MIN_ptr);
//    }
//    return;
//}

//  //-------------------------------------
//template <typename NodeType>
//NodeType kwList<NodeType>::RemoveFirst() {

//    if (numOfListItems == 0) {
//      return ;
//    }
//    NodeType* remove = this->headPtr->next;
//    if (remove == this->endPtr) {
//      this->RemoveNode();
//    }
//    else {
//      this->headPtr->next = remove->next;
//      remove->next->prev = this->headPtr;
//      delete remove;
//      numOfListItems--;
//    }
//    return;
//}

//-------------------------------------
template <typename NodeType>
void kwList<NodeType>::RemoveNode( kwNode<NodeType> *ToBeRemove )
{
    if( numOfListItems == 0 ){
        cout << "there is no Node to delete" << endl;
        return;
    }

    if (ToBeRemove == this->endPtr || ToBeRemove == nullptr){
        ToBeRemove->prev->next = nullptr;
        this->endPtr = ToBeRemove->prev;
    }
    else{
        ToBeRemove->prev->next = ToBeRemove->next;
        ToBeRemove->next->prev = ToBeRemove->prev;
    }
    delete ToBeRemove;
    this->numOfListItems--;
    return;
}

//-------------------------------------
template <typename NodeType>
int kwList<NodeType>::RemoveNodesAfter( kwNode<NodeType> *lastNodeToKeep )
{
    if( numOfListItems == 0 ){
        cout << "there is no Node to delete" << endl;
        return 0;
    }

    int nNumNodesFreed = 0;
    kwNode<NodeType> *tmp, *tmpdel;
    tmp = tmpdel = lastNodeToKeep->next;
    while ( tmp != nullptr )
    {
        tmpdel = tmp;
        tmp = tmp->next;
        delete tmpdel;
        ++nNumNodesFreed;
    }
    lastNodeToKeep->next = nullptr;
    return nNumNodesFreed;
}

////-------------------------------------
//template <typename NodeType>
//void kwList<NodeType>::SwitchNode( kwNode<NodeType> *node1, kwNode<NodeType> *node2 )
//{
//    // check two node isn't headNode
//    if(node1 == this->headPtr || node2 == this->headPtr){
//        cout << "can't switch neadNode" << endl;
//        return;
//    }

//    NodeType *l1 = node1->prev, *l2 = node2->prev, *r1 = node1->next, *r2 = node2->next;
//    l1->next = node2;
//    node2->prev = l1;

//    l2->next = node1;
//    node1->prev = l2;

//    if (r1 != nullptr){
//        r1->prev = node2;
//        node2->next = r1;
//    }
//    else {
//        node2->next = nullptr;
//    }

//    if (r2 != nullptr){
//        r2->prev = node1;
//        node1->next = r2;
//    }
//    else {
//        node1->next = nullptr;
//    }
//}

//-------------------------------------
template <typename NodeType>
void kwList<NodeType>::Clear() {
    kwNode<NodeType> *ptr = this->endPtr;
    for (int i = 0; i < this->numOfListItems; ++i) {
        ptr = ptr->prev;
        delete ptr->next;
    }
    this->headPtr->next = nullptr;
    this->endPtr = this->headPtr;
    this->numOfListItems = 0;
}

//  //-------------------------------------
//template <typename NodeType>
//  ZxList(int nMaxNumOfList = Zx_INT32_MAX) {
//    pHead = new NodeType[1];
//    pTail = pHead;
//    pHead->prev = NULL;
//    pTail->next = NULL;
//    m_nNumOfListItems = 0;
//    m_nnumOfMaxListItems = nMaxNumOfList;
//  }

//-------------------------------------
template <typename NodeType>
kwNode<NodeType>* kwList<NodeType>::operator()( int IndexOfTheList )    // 0 代表 headnode
{
    if ( IndexOfTheList > this->numOfListItems){
        cout << "Out of List's index" << endl;
        return nullptr;
    }

    kwNode<NodeType> *ptr = this->headPtr;
    for (int i = 1; i <= IndexOfTheList; ++i) {
        ptr = ptr->next;
    }
    return ptr;
}

//-------------------------------------
template <typename NodeType>
kwList<NodeType>&  kwList<NodeType>::operator=( const kwList<NodeType> &srcList )
{
    this->Clear();
    kwNode<NodeType> *tmp = srcList.headPtr->next;
    while (tmp != nullptr) {
        this->InsertNode(tmp);
        tmp = tmp->next;
    }
    return *this;
}

//-------------------------------------
template <typename NodeType>
void kwList<NodeType>::Combine( kwList<NodeType> &srcList )
{
    kwNode<NodeType> *tmp = srcList.headPtr->next;
    while (tmp != nullptr) {
        this->InsertNode(tmp);
        tmp = tmp->next;
    }
}

#endif // KWLIST_H
