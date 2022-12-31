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
    const std::string OPERAND_FOUND="TRUE";
    bool isError;
    std::string errorMessage;
    std::map<std::string,std::string> key2value;
    std::map<std::string,std::string> key2desc;
    std::map<std::string,bool> key2hasOperand;
    std::map<std::string,std::string> option2key;
    void copyTo(Arg& trg) const
    {
        trg.isError=isError;
        trg.errorMessage=errorMessage;
        trg.key2value.clear();
        trg.key2desc.clear();
        trg.option2key.clear();
        for(auto itr=key2value.begin();itr!=key2value.end();itr++)
        {
            trg.key2value[itr->first]=itr->second;
        }
        for(auto itr=key2desc.begin();itr!=key2desc.end();itr++)
        {
            trg.key2desc[itr->first]=itr->second;
        }
        for(auto itr=option2key.begin();itr!=option2key.end();itr++)
        {
            trg.option2key[itr->first]=itr->second;
        }
        for(auto itr=key2hasOperand.begin();itr!=key2hasOperand.end();itr++)
        {
            trg.key2hasOperand[itr->first]=itr->second;
        }
    }
    void init()
    {
        isError=false;
        errorMessage="";
        key2value.clear();
        key2desc.clear();
        option2key.clear();
        key2hasOperand.clear();
    }
public:
    int registerKey(const std::string key, const std::string desc, bool hasOperand)
    {
        key2value[key]="";
        key2desc[key]=desc;
        key2hasOperand[key]=hasOperand;
    }
    int addOption(const std::string option, const std::string key)
    {
        int ret=0;
        if(key2value.find(key)!=key2value.end())
        {
            option2key[option]=key;
        }
        return ret;
    }
    int resetValues()
    {
        int ret=0;
        isError=false;
        errorMessage="";
        for(auto itr=key2value.begin();itr!=key2value.end();itr++)
        {
            itr->second="";
            ret++;
        }
        return ret;
    }
    int parse(int argc, char** argv)
    {
        resetValues();
        for(int i=1;i<argc;i++)
        {
            std::string arg=argv[i];
            if(option2key.find(arg)!=option2key.end())
            {
                std::string key=option2key[arg];
                if(key2value[key]!="")
                {
                    isError=true;
                    errorMessage="One option was assigned twice. This error was triggered by option " +arg+ " ("+std::to_string(i)+")";
                    break;
                }
                if(key2hasOperand[key])
                {
                    if(i+1<argc)
                    {
                        i++;
                        key2value[key]=argv[i];
                    }
                    else
                    {
                        isError=true;
                        errorMessage="No operand found for " + arg + "("+std::to_string(i)+")";
                        break;
                    }
                }
                else
                {
                    key2value[key]=OPERAND_FOUND;
                }
            }
            else
            {
                isError=true;
                errorMessage="Unknown option " + arg + "("+std::to_string(i)+")";
                break;
            }
        }
        return 0;
    }
    std::string get(std::string key) const
    {
        if(key2value.find(key)==key2value.end())
        {
            std::cerr << "!ERROR: No key named " << key << " found." << std::endl;
            return "";
        }
        return key2value.at(key);
    }
    std::string resultList() const
    {
        std::stringstream ss;
        if(isError)
        {
            ss << "ERROR:" << errorMessage << std::endl;
        }
        else
        {
            ss << "List of args:" << std::endl;
            for(auto itr=key2value.begin();itr!=key2value.end();itr++)
            {
                if(itr->second=="")
                {
                    continue;
                }
                ss << "  - " << (itr->first) << " = " << (itr->second) << std::endl;
            }
        }
        
        return ss.str();
    }
    
    std::string help() const
    {
        std::stringstream ss;
        
        ss << "[Usage]:" << std::endl;
        
        for(auto itrK=key2value.begin();itrK!=key2value.end();itrK++)
        {
            const std::string& key=itrK->first;
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
            ss << (key2hasOperand.at(key)?" [arg]":" ") << std::endl;
            ss << "   " << key2desc.at(key) << std::endl;
        }
        
        
        return ss.str();
    }
    
};


#endif
