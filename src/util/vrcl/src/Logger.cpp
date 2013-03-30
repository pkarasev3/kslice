#include "Logger.h"

namespace vrcl {

Logger Logger::log;

void Clocker::startClock(){
    ticks = clock();
    total = 0;
}
float Clocker::lap()
{
    float laptime = total;

    return totalT() - laptime ;

}
float Clocker::totalT(){
    total = (clock() - ticks)/(float)CLOCKS_PER_SEC;
    return total;
}

Logger& Log() {

    return Logger::log;
}


} // end namespace
