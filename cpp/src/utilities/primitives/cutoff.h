//
// Created by jcfergus on 2/5/20.
//

#ifndef PUMA_CUTOFF_H
#define PUMA_CUTOFF_H

#include <iostream>

namespace puma {

    class Cutoff {
    public:
        float first;
        float second;
        Cutoff(float first, float second){

            if(first > second){
                first = -1.5;
                second = -1.5;
                std::cout << "Invalid cutoff. Low cutoff must be less than or equal to high cutoff" << std::endl;
            }

            if ((int)first == first) {
                first = first - 0.5;
            }
            if ((int)second == second) {
                second = second + 0.5;
            }

            this->first = first;
            this->second = second;
        }

        Cutoff(){
            first = -1;
            second = -1;
        }

    };
}

#endif //PUMA_CUTOFF_H
