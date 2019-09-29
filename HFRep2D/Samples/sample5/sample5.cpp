#include <iostream>
#include <chrono>
#include <random>

#include "activeList.h"

int main(int argc, char** argv)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr( 0, 20 );

    hfrep2D::activeList list1;

    for( size_t i = 0; i < 20; i++ )
    {
        hfrep2D::aNode node_i( distr(gen), distr(gen) );
        list1.pushNode(node_i);
        std::cout << list1[i].dx << ", " << list1[i].dy << std::endl;
    }

    std::cout << "list size: " << list1.Size() << std::endl;
    std::cout << "empty(?): "  << list1.isListEmpty() << std::endl;

    std::cout << "node 5 (dx,dy): " << list1[5].dx << ", " << list1[5].dy << std::endl;

    int x, y;
    std::cin >> x;
    std::cin >> y;

    hfrep2D::aNode node2( x, y ); int pos = 0;

    std::cout << "find node ( "<< x <<", " << y <<" )" << list1.isNodeInList( node2, &pos ) << " at " << pos << std::endl;

    if( list1.isNodeInList( node2, &pos ) )
    {
        hfrep2D::aNode removed = list1.popNode( pos );
        std::cout << "node removed (dx,dy): " << removed.dx << ", " << removed.dy << std::endl;
        std::cout << "new list size: " << list1.Size() << std::endl;
    }

    return 0;
}
