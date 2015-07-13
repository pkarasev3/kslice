/////////////////////////////////////
#define TOKEN_TO_STRING(TOK) # TOK
#define STRINGIZE_TOKEN(TOK) TOKEN_TO_STRING(TOK)
#define PRINT_AND_EVAL(X) {QString fnc=__FUNCTION__;qDebug()<<fnc<<": "<<STRINGIZE_TOKEN(X)<<"="<<X;}
/////////////////////////////////////    Print the statement and its executed value.

/////////////////////////////////////    Building Sub-Component   ///
#if defined(_WIN32)
    #define KVIEWER_LOCAL
    #if defined(KVIEWER_LIB)
        #define KVIEWER_EXPORT __declspec(dllexport)
    #else
        #define KVIEWER_EXPORT __declspec(dllimport)
    #endif
#else
    #define KVIEWER_EXPORT  __attribute__ ((visibility ("default")))
    #define KVIEWER_LOCAL   __attribute__ ((visibility ("hidden")))
#endif