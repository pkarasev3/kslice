#ifndef DATA_MAP_H_
#define DATA_MAP_H_
#include <iostream>
#include <map>
#include <string>
#include <exception>
#include <typeinfo>

//vegan-misc
#include "Logger.h"


namespace vrcl{
    class vdata{
    protected:

        virtual ~vdata(){}
        virtual const char* tname() = 0;
        virtual vdata* clone() = 0;
        virtual bool empty() = 0;
        virtual void* data() const = 0;
        friend class data_map;

    };

    template<class DataType>
    struct Cloner
    {

    public:

        DataType* clone(DataType* data,bool own){
            return own ? new DataType(*data):data;
        }
    };

    template<class BaseType, class DataType>
    struct VCloner
    {
        void compile_time_derived_test(DataType* d, BaseType* b){
            b = static_cast<BaseType*>(d);
        }
    public:

        BaseType* clone(BaseType* data,bool own){
            return own ? new DataType(*dynamic_cast<DataType*>(data)):data;
        }
    };

    template<class DataType,class _CLONE = Cloner<DataType> >
    class tdata:public vdata{
        typedef _CLONE cltype;
        cltype cloner;
        DataType * _data;
        bool _own;
        friend class data_map;
    protected:
        tdata(DataType * data, bool own = true):_data(data),_own(own){
        }

        virtual vdata* clone(){
            return new tdata(cloner.clone(_data,_own),_own);
        }

        virtual const char* tname(){
            return typeid(DataType).name();
        }

        virtual ~tdata(){
            if(_own){
                delete _data;
            }
        }

        virtual bool empty(){return _data == NULL;}

        virtual void* data() const {return _data;}




    };


    class data_map_exception: public std::exception
    {
    public:
        const char* _what;
        data_map_exception(const char * _what):_what(_what){

        }

        virtual const char* what() const throw()
        {
            return _what;
        }
    };



/** Heterogeneous data map
  *
  * usage:
  * data_map dm;
  * Alias<Foo> foo("my foo");
  * RegisterAliase
  */
    class data_map{

        typedef std::map<std::string,vdata*> VdataMap;
        VdataMap vdmap;
        void deleteData(const std::string &name){
            delete vdmap[name];
            vdmap[name] = NULL;
        }
        void clear_map(){

            VdataMap::iterator it = vdmap.begin();
            while(it != vdmap.end()){ // WARNING: it->second might be null!!!
                if( NULL != it->second )
                    delete it->second;
                ++it;
            }
            vdmap.clear();
        }
        template<typename DataType>
        DataType* vdata_cast(const vdata* vd) const{
            return dynamic_cast<DataType*>(vd->data());
        }

    public:

        data_map(){}
        data_map(const data_map& rhs){
            //Lout(Logger::FINEST) << "copy constructor of data_map" <<std::endl;
            VdataMap::const_iterator it = rhs.vdmap.begin();
            while(it != rhs.vdmap.end()){
                vdmap[it->first] = it->second->clone();
                ++it;
            }
        }

        virtual ~data_map(){
            clear_map();
        }

        virtual void operator=(const data_map& rhs){
            Lout(Logger::FINEST) << "operator= of data_map" <<std::endl;
            clear_map();
            VdataMap::const_iterator it = rhs.vdmap.begin();
            while(it != rhs.vdmap.end()){
                vdmap[it->first] = it->second->clone();
                ++it;
            }

        }




        template<typename DataType>
        void addData(const std::string &name, DataType * data,bool own = true)
                throw(){
            tdata<DataType>* td = new tdata<DataType>(data,own);
            delete vdmap[name];
            vdmap[name] = td;
        }

#if 0
        //now the data will be ingested and only avalaible through
        //base class interface...
        template<typename BaseType,typename DataType>
        void addPureVirtual(const std::string &name,
                            DataType * data,bool own = true){
            tdata<BaseType,VCloner<BaseType,BaseType> >* td =

                    new tdata<BaseType,VCloner<BaseType,BaseType> >
                    (static_cast<BaseType*>(data),own);
            delete vdmap[name];
            vdmap[name] = td;
        }


        template <typename BaseType>
        BaseType* getPureVirtual(const std::string &name){
            vdata* vd = vdmap.find(name)->second;
            tdata<DataType>* td = dynamic_cast<tdata<BaseType,VCloner<BaseType,BaseType>* >(vd);
            if(!td){
                std::string exceptstr;
                exceptstr = "bad type used for " + name + "\n"+
                            "used type: "+typeid(DataType).name() +
                            "\nshould have used type: " + vd->tname();
                throw data_map_exception(exceptstr.c_str());
            }
            data = td->_data;
        }
#endif

        template<typename DataType>
        void getData(const std::string &name,DataType*& data) const
                throw(data_map_exception){
            throwHas(name);
            vdata* vd = vdmap.find(name)->second;
            tdata<DataType>* td = dynamic_cast<tdata<DataType>* >(vd);
            if(!td){
                std::string exceptstr;
                exceptstr = "bad type used for " + name + "\n"+
                            "used type: "+typeid(DataType).name() +
                            "\nshould have used type: " + vd->tname();
                throw data_map_exception(exceptstr.c_str());
            }
            data = td->_data;

        }



        bool has(const std::string& name) const throw(){
            VdataMap::const_iterator it = vdmap.find(name);
            return (it != vdmap.end());
        }

        void throwHas(const std::string&name)const throw(data_map_exception){
            if(!has(name))
            {
                std::string exceptstr;
                exceptstr = "bad name used : " + name;
                throw data_map_exception(exceptstr.c_str());
            }

        }
#ifdef DATA_MAP_AT_PTR_DEPRECIATED
        template<typename DataType>
        DataType* ptr(const std::string& name,bool own = true)const
                throw(data_map_exception){
            DataType* data;
            getData(name,data);
            return data;

        }

        template<typename DataType>
        DataType& at(const std::string& name) const throw(data_map_exception){
            return *ptr<DataType>(name,true);

        }
#endif
#if 0
        virtual void serialize(std::ostream& out, int flags) const {
            VdataMap::iterator it = vdmap.begin();
            out << "[";
            while(it != vdmap.end()){
                it->serialize(out,flags);
                ++it;
            }
            out << "]";
        }
        virtual void deserialize(std::istream& in, int flags){
            deserialize(in,*_data,flags);
            eatchar(in, '[');

            while (in.peek() != ']') {

                in >> val;
                vect.push_back(val);
            }
            eatchar(in, ']');
        }
#endif

    };

    template<typename DataType>
    class Alias{
    public:
        Alias(const std::string& name):
                name(name),_data(0),_own(false){
        }
        Alias(const std::string& name,data_map& dm) throw(data_map_exception):
                name(name),_data(0),_own(false){
            getFromMap(dm);
        }
        Alias(const std::string& name,data_map& dm,DataType* data,bool own=true)
                throw(data_map_exception)
                    :name(name),_data(data),_own(own){
            addToMap(dm);
        }
        ~Alias(){
            if(_own)delete _data;
        }

        void setData(DataType* data,bool own=true){
            _data = data;
            _own = own;
        }
        void getFromMap(const data_map& dm)throw(data_map_exception){

            DataType* td;
            dm.getData(name,td);
            if(_own)
                delete _data;
            _data = td;
            _own = false;

        }
        void addToMap(data_map& dm)throw(data_map_exception){
            throwEmpty();
            dm.addData(name,_data,_own);
            _own = false;
        }

        DataType& operator()() throw(data_map_exception){
            throwEmpty();
            return *_data;
        }
        void throwEmpty() const throw(data_map_exception){
            if(empty())
                throw data_map_exception("alias is empty!");
        }

        bool empty() const throw(){
            return _data == NULL;
        }

        const std::string name;
    private:
        DataType* _data;
        bool _own;

    };
    template<typename DataType>
    Alias<DataType> & operator<<(Alias<DataType>& alias, const data_map& dm)
            throw(){
        try{
            alias.getFromMap(dm);
        }catch(data_map_exception e){
            Lout(Logger::FINEST) << "Error getting data from map!" << std::endl
                    << "Are you sure it exits?"
                    << std::endl;
            Lout(Logger::FINEST) << e.what() << std::endl;
        }
        return alias;
    }
    template<typename DataType>
    Alias<DataType> & operator>>(Alias<DataType>& alias, data_map& dm)
            throw(){
        try{
            alias.addToMap(dm);
        }catch(data_map_exception e){
            Lout(Logger::ERR) << "Error adding data to map!" << std::endl;
            Lout(Logger::ERR) << e.what() << std::endl;
        }
        return alias;
    }

    template<typename DataType>
    Alias<DataType> & RegisterAlias(Alias<DataType>& alias, data_map& dm){
        try{
            alias.getFromMap(dm);
        }catch(data_map_exception e){
            alias.setData(new DataType());
            alias >> dm;
        }
        return alias;
    }

    template<typename DataType>
    Alias<DataType> MakeAlias(const std::string& name,data_map& dm,
                              DataType* val,bool own = true){
        Alias<DataType> alias(name,dm,val,true);
        return alias;
    }
    template<typename DataType>
    Alias<DataType> MakeAlias(const std::string& name,data_map& dm,
                              const DataType& val){
        Alias<DataType> alias(name,dm,new DataType(val),true);
        return alias;
    }


}
#endif

