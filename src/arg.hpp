#ifndef ARG_HPP_
#define ARG_HPP_

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>

class Arg
{
    //  option: string that is written in CUI by user
    //  key: string that identify options (unique for each process)
    //  [USAGE]
    //  - Use appendOption to register a pair of option and key
    //      - If you'd like to use key beginning with "-" or "--", include "-" or "--" to the argument "key" for appendOption
    //  - Use clearValues to initialize values for all keys
    //  - Use parse to analyze input
public:
    Arg(){}
    Arg(const Arg& obj)
    {
        obj.copyTo(*this);
    }
    const Arg& operator=(const Arg& obj)
    {
        obj.copyTo(*this);
        return obj;
    }
    ~Arg(){}
private:
    std::map<std::string,std::string> key2value;
    std::map<std::string,std::string> key2desc;
    std::map<std::string,std::string> option2key;
    void copyTo(Arg& trg) const
    {
        std::copy(key2value.begin(),key2value.end(),trg.key2value.begin());
        std::copy(key2desc.begin(),key2desc.end(),trg.key2desc.begin());
        std::copy(option2key.begin(),option2key.end(),trg.option2key.begin());
    }
    void init()
    {
        key2value.clear();
        key2desc.clear();
        option2key.clear();
    }
private:
    int appendOption(const std::string option, const std::string key, const std::string desc="")
    {
        int ret=0;
        if(option2key.find(option)==option2key.end())
        {
            option2key[option]=key;
            ret=option2key.size();
            if(key2value.find(key)==key2value.end())
            {
                key2value[key]="";
            }
            if(desc!="")
            {
                key2desc[key]=desc;
            }
        }
        else
        {
            std::cerr << "!Error: Option " << option << " has already been associated with key " << key << "." << std::endl;
            ret=0;
        }
        
        return ret;
    }
    int removeOptions()
    {
        int ret=0;
        while(option2key.size()>0)
        {
            option2key.erase(option2key.begin());
        }
        return ret;
    }
    int clearValues()
    {
        int ret=0;
        for(auto itr=key2value.begin();itr!=key2value.end();itr++)
        {
            itr->second="";
            ret++;
        }
        return ret;
    }
    int parse(int argc, const char** argv)
    {
        return 0;
    }
    std::string help() const
    {
        std::stringstream ss;
        
        ss << "[Usage]:" << std::endl;
        
        for(auto itrK=key2value.begin();itrK!=key2value.end();itrK++)
        {
            std::string key=itrK->first;
            std::map<std::string,int> options;
            for(auto itr=option2key.begin();itr!=option2key.end();itr++)
            {
                if(itr->second==key)
                {
                    options[itr->first]=0;
                }
            }
            //
            for(auto itr=options.begin();itr!=options.end();itr++)
            {
                ss << " " << (itr->first);
            }
            ss << std::endl;
            ss << "   " << key2desc.at(key) << std::endl;
        }
        
        
        return ss.str();
    }
    
};


#endif
