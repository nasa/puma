#ifndef toString_H
#define toString_H

#include <string>
#include <vector>
#include <sstream>
#include <iostream>


class toString {

public:

    static std::string convert(int v) {
        std::stringstream ss;
        ss << v;
        return ss.str();
    }

    static std::string convert(long v) {
        std::stringstream ss;
        ss << v;
        return ss.str();
    }

    static std::string convert(float v) {
        std::stringstream ss;
        ss << v;
        return ss.str();
    }

    static std::string convert(double v) {
        std::stringstream ss;
        ss << v;
        return ss.str();
    }

    static std::string convert(char v) {
        std::stringstream ss;
        ss << v;
        return ss.str();
    }

    static std::string convert(bool v) {
        if(v) {
            return "true";
        }
        return "false";
    }


    static std::string splice(std::string s) {
        if(s=="") return s;

        int target=60;
        int current=0;
        for(unsigned int i=0;i<s.length();i++){
            if(current>=target&&s.at(i)==' '){
                s.insert(i+1,"\n            ");
                current=0;
                i+=14;
            }
            current++;
        }
        return s;
    }

    static std::vector<std::string> splitString(std::string str, std::string del) {

        std::vector<std::string> splitStrings;

        size_t pos = 0;
        std::string token;
        while ((pos = str.find(del)) != std::string::npos) {
            token = str.substr(0, pos);
            splitStrings.push_back(token);
            str.erase(0, pos + del.length());
        }
        splitStrings.push_back(str);

        return splitStrings;
    }

    static std::string replaceAll(std::string baseString, std::string searchFor, std::string replaceWith) {
        while( baseString.find(searchFor) != std::string::npos) {
            baseString.replace(baseString.find(searchFor),searchFor.length(),replaceWith);
        }
        return baseString;
    }


};



#endif // toString_H
