#ifndef VRCL_LOGGER_H
#define VRCL_LOGGER_H

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <ctime>



namespace vrcl {

//**********************************************************************
//Logging and debug utilities

/** \brief convience nullstream for Logger
  * this is used for redirecting output to null stream
 */
struct nullstream: std::ostream {
    nullstream() :std::ios(0),std::ostream(0) {}
};


/** \brief Simple time keeping class, useful for profiling
  * sections of code.
  */
class Clocker{
public:
    std::clock_t ticks;
    float total;

    /** \brief starts the clock, and starts keeping track of time,
      * undefined behavior in lap and totalT if this is not called
      */
    void startClock();

    /** \brief this returns, in seconds, the time since either the
      * last time lap() was called or the time since startClock() was
      * called
      * \return time in seconds
      */
    float lap();

    /** \brief returns the total time since the startClock() function
      * has been called
      * \return time in seconds
      */
    float totalT();
};

/** \brief Logger is a singleton class useful for global debugging.
  *  it should be aquired through the use of Log(). Logger may store global
  * flags by a string key or be used for a conditional cout.
  *
  *  sample use
  * <code>
  *  Log().setLevel(Logger::FINEST);
  *  Log().out() << "hello this is a Logger::FINE message" << endl;
  *  Log().out(Logger::WARNING) << "this is output with" <<
  *                       "hirer priority than Logger::FINE" << endl;
  *  </code>
  */


class Logger {
public:
    /** different levels
     *
     * the higher the number, the greater the need to
     * output, or conversely if the level of the logger
     * it will output only those things that are at
     * a greater level
     *
     */

     enum LogEnum {FINEST= 0,FINE,INFO,WARNING,ERR=100}  ;  //Anthony: changed ERROR to ERR (solve compiler error C2059)


//	typedef int LogEnum;
    /** \brief the current level of the Logger
     * \return the integer level of the logger,use like:
     * \code if(mylevel > Log().level())
     //execute verbose code
     */
    LogEnum level() const {
        return _level;
    }

    /** \brief set the Level of the logger
     * \param level the new level
     */
    void setLevel(LogEnum level) {
        _level = level;
    }

    /** Get an integer index to the flag name given
     \return a unique index for a give string
     */
    int flagIdx(const std::string& flag) {
        if (strmap.count(flag) == 0) {
            flags.push_back(false);
            strmap[flag] = _keyid++;
        }
        return strmap[flag];
    }

    /** Get a bool for a given index, faster for inner loop
     */
    bool operator[](int idx) const {
        return flags[idx];
    }

    /** Get a bool for a given flag
     * don't call in inner loop, use idx version
     */
    bool operator[](const std::string& flag) const {
        if (strmap.count(flag))
            return flags[strmap.find(flag)->second];
        return false;
    }
    /** Set a bool given index
     */
    void set(int idx, bool val = true) {
        flags[idx] = val;
    }

    /** set a bool givena  string
     * call idx version if speed is concern
     */
    void set(const std::string& flag, bool val = true) {
        flags[flagIdx(flag)] = val;
    }

    /** get an output stream. if level >= Log().level() this will
     return the given ostream, otherwise, a null output
     */
    std::ostream& out(LogEnum level = FINE, std::ostream& out = std::cout) {
        if (level >= _level)
            return out;
        //return a null stream if the logger level is greater than given
        //level
        return _nullout;
    }

private:
    friend Logger& Log();
    LogEnum _level;
    std::vector<bool> flags;
    std::map<std::string, int> strmap;
    int _keyid;
    nullstream _nullout;

    Logger() :
        _keyid(0) {
        setLevel(FINE);
    }
    static Logger log;

};

/** \brief get the Global Logger object - singleton pattern
 */
Logger& Log();

/** \brief get the Logger.out
 */
inline std::ostream& Lout(Logger::LogEnum level = Logger::FINE) {
    return Log().out(level);
}

/** some convenience log macros
  */

#ifndef ANDROID
    #ifndef  IFLOG
        #define  IFLOG(expr,statement)   if( Log()[ (expr) ]) { statement; }
    #endif
#endif
#ifdef ANDROID
    #ifndef  IFLOG
        #define  IFLOG(expr,statement) {}
    #endif
#endif
#ifndef  SETLOG
    #define  SETLOG(expr,val)         Log().set(expr,val);
#endif
#ifndef  SETL
    #define  SETL(expr)               Log().set(expr,true);
#endif
#ifndef  LENDL
    #define  LENDL                   ;Lout()<<std::endl;
#endif
#ifndef  LOUT
    #define  LOUT                    ;Lout()<<
#endif

}


#endif // LOGGER_H
