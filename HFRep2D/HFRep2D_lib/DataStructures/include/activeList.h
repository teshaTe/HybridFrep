/*
 * This is an implementation of the narrowband data-structure called Active List;
 * This data structure is used in the algorithm called Fast Iterative Method (FIM)
 * for computing numerical solution of the Eikonal equation. Its main advantage is
 * that each node can be updated independantely, so it is suitable for parallel computations.
 *
 * Introduced in the core work on FIM: https://doi.org/10.1137/060670298
 * @copyright TeSha 2019
 */

#ifndef ACTIVE_LIST_CLASS
#define ACTIVE_LIST_CLASS

#include <vector>
#include <iterator>

namespace hfrep2D {

/*
 * ActiveList node that stores:
 * dx,dy      - coordinates of the grid point;
 * currentVal - current solution value;
 * prevVal    - solution value from the previous iteration;
 * isSource   - is the current node a source [true/false];
 * isNodeConverged( thres ) - function defines according to the threshold if node is active or not [true/false];
 */
struct aNode
{
    aNode(float x, float y):dx(x),dy(y){}
    bool isNodeConverged( float thres ) { return (currentVal - prevVal) < thres; }
    bool isSource = false;
    float dx, dy;
    float currentVal;
    float prevVal;
};

class activeList
{
public:
    activeList(){}
    ~activeList(){}

//basic operations over ActiveList based on std::vector<aNode> type;
    bool isNodeInList( aNode node, int *index )
    {
        for(int i = 0; i < aList.size(); i++)
        {
            if( aList[i].dx == node.dx && aList[i].dy == node.dy )
            {
                *index = i;
                return true;
            }
        }
        return false;
    }

    aNode popNode( int index )
    {
        aNode node = aList[index];
        aList.erase(aList.begin()+(index-1));
        return node;
    }

    inline aNode operator [] (int const i) { return aList[i]; }

    inline void pushNode( aNode node ) { aList.push_back( node ); }
    inline size_t Size()               { return aList.size(); }
    inline bool isListEmpty()          { return aList.size() == 0; }
    inline void Clear()                { return aList.clear(); }

private:
    std::vector<aNode> aList;
};


}//namespace hfrep2D
#endif // ifndef ACTIVE_LIST_CLASS
