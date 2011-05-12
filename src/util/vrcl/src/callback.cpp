#include "callback.h"
#include "Logger.h"
using namespace std;

#if 0
namespace vrcl{

    const string Callback::CALLBACK_DEBUG_LOG = "CALLBACK_DEBUG_LOG";
/** assign callback object to the observable. e.g. tell it
  * who wants to observe. pass pointer so it can be grabbed.
  */
    void observable::registerCallback( cv::Ptr<Callback> cb){
    callbacks[cb->getName()] = cb;
}

void  observable::callback(const std::string& cbkey, data_map& data){
    if(callbacks.count(cbkey))
        callbacks[cbkey]->update(data);
}

void observable::send_message( const std::string& cbkey, const std::string &str_msg ) {
    if( callbacks.count(cbkey) ) {
        callbacks[cbkey]->receive_message(str_msg); // callback FROM ME receives
    }
}

void observable::receive_message( const std::string& cbkey, std::string &str_msg ) {
    if( callbacks.count(cbkey) ) {
        callbacks[cbkey]->send_message(str_msg); // callback TO ME sends
    }
}



const char * DebugCallback::getName(){return Callback::CALLBACK_DEBUG_LOG.c_str();}
/**echo's the message to the log
  */
void DebugCallback::receive_message( const std::string & str_log) {
    Log().out()<<str_log << std::endl;
}


}
#endif
