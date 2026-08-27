#ifndef KWNODE_H
#define KWNODE_H

#include "kwCommon.h"
#include "kwConst.h"
using namespace std;

template<typename NodeType>
class kwNode
{
public:
    kwNode<NodeType>    *next;
    kwNode<NodeType>    *prev;
    NodeType            nodeContent;

    /// @brief default constructor
    kwNode();

    /// @brief constructor that can assign nodeContent
    kwNode( NodeType &_val );

};

/////////////////////////////////////////////////////////////////////////////////
// below is implementation since we can't put these code into .cpp file

//-------------------------------------
// Sadly, we can't assign a value to nodeContent, we use the NodeType's default constructor
template <typename NodeType>
kwNode<NodeType>::kwNode(): prev(nullptr), next(nullptr) {}

//-------------------------------------
template <typename NodeType>
kwNode<NodeType>::kwNode( NodeType &_val ): prev(nullptr), next(nullptr), nodeContent(_val)  // use copy constructor
{ /*cout << "create a Node by value" << endl;*/ }

#endif // KWNODE_H
