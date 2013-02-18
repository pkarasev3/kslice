#ifndef VRCL_CALLBACK_H
#define VRCL_CALLBACK_H

#if 0

#include <iostream>
//#include <opencv2/core/core.hpp>
#include <map>
#include <string>
#include <exception>
#include <typeinfo>


namespace vrcl {

class data_map;

/**  base class for creating entities whose sole purpose is a handling a 'jump from'
  * inside a complicated algorithm / routine. do not have an intrinsic state other than
  * the datamap being operated on. importantly, it can itself be put in a data_map. furthermore,
  * it can defy physics and exist inside its own data_map.
  */
class Callback {
public:

    static const std::string CALLBACK_DEBUG_LOG;
    /**For DataMap - naming
      */
    virtual const char * getName(){return "callback";}
    virtual ~Callback() {}
    virtual void update(data_map& data){}
    virtual void receive_message( const std::string & str_log) { } // observable objects sends us a message
    virtual void send_message( std::string & str_log) { } // return string "command" or "message" to observable object
};

/** a Simple debugging callback that echo's messages sent to
  * Callback::CALLBACK_DEBUG_LOG
  * to use an observable object just sends messages like:
  * send_message(Callback::CALLBACK_DEBUG_LOG, "my debug message")
  * and this callback will echo them to the Log().out()
  *
  */
class DebugCallback:public Callback{

    virtual const char * getName();
    /**echo's the message to the log
      */
    virtual void receive_message( const std::string & str_log);
};


/** interface class, heavier-weight classes can inherit from it and just implement Update( ).
  * things inheriting from this cannot be put inside a data_map due to pure virtualness.
  *
  * 1. getStatus   =>    force the object to do something to data_map
  *
  * 2. registerCallback =>  the object decides when to update the Callback object
  */
class observable {
private:
    std::map<std::string, cv::Ptr<Callback> > callbacks;
public:
     /** must have virtual destructor if delete is to work through a pointer
     */
    virtual ~observable(){}

    /** assign callback object to the observable. e.g. tell it
      * who wants to observe. pass pointer so it can be grabbed.
      */
    void registerCallback(cv::Ptr<Callback> cb);

    /** tell the Callback at key cbkey to trigger, passing it data_map IO container
      */
    void callback(const std::string& cbkey, data_map& data);

    /**
      * The *OBSERVABLE* sends a message *TO THE CALLBACK*
      */
    void send_message( const std::string& cbkey, const std::string& str_log);

    /** The *CALLBACK* sends a message *TO THE OBSERVABLE*
      */
   void receive_message( const std::string& cbkey, std::string& str_msg );


};

}

#endif

#endif
