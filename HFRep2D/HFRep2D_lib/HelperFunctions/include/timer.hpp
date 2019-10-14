#ifndef H_SIMPLE_TIMER
#define H_SIMPLE_TIMER

#include <chrono>
#include <ctime>
#include <string>
#include <iostream>

namespace hfrep2D {

class timer
{
public:
    timer(){iter = 0;}
    ~timer(){iter = 0;}

    void End( std::string var )
    {
        auto end = std::chrono::high_resolution_clock::now();
        auto time_span = std::chrono::duration_cast<std::chrono::microseconds>( end - start );
        std::cout << var << ", iteration ["<< iter <<"] : " << time_span.count() << " [micSec];" << std::endl;

        if(iter > 1)
            printf("\x1b[A");
    }

    inline void Start() { start = std::chrono::high_resolution_clock::now(); iter++; }
    inline void resetIter() { std::cout <<"\nlast iter: " << iter << std::endl; iter = 0; }
private:
    std::chrono::high_resolution_clock::time_point start;
    int iter;
};

}


#endif //#define H_SIMPLE_TIMER
