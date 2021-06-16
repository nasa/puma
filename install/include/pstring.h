#ifndef PSTRING_H
#define PSTRING_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>


namespace puma {

    class PString
    {
    public:

        ///
        /// \brief fromDouble: converts a double into a string
        /// \param d : double input
        /// \return : double represented as a float
        ///
        static std::string fromDouble(double d)
        {
            return std::to_string((long double)d);
        }

        ///
        /// \brief fromFloat: converts a float into a string
        /// \param f : float input
        /// \return : float represented as a float
        ///
        static std::string fromFloat(float f)
        {
            return std::to_string((long double)f);
        }

        ///
        /// \brief fromInt: converts a int into a string
        /// \param i : int input
        /// \return : int represented as a float
        ///
        static std::string fromInt(int i)
        {
            return std::to_string((long long int)i);
        }

        ///
        /// \brief fromLong: converts a long into a string
        /// \param l : long input
        /// \return : long represented as a float
        ///
        static std::string fromLong(long l)
        {
            return std::to_string((long long int)l);
        }

        ///
        /// \brief fromShort: converts a short into a string
        /// \param s : short input
        /// \return : short represented as a float
        ///
        static std::string fromShort(short s)
        {
            return std::to_string((long long int)s);
        }

        ///
        /// \brief toInt: takes a string and attempts to parse the string into an int
        /// \param s : input string
        /// \return : returns the string parsed as an int or returns 0 if the parsing failed
        ///
        static int toInt(const std::string& s)
        {
            try
            {
                return std::stoi(s);
            }
            catch(...)
            {
                return 0;
            }
        }

        ///
        /// \brief toInt: takes a string and attempts to parse the string into an int
        /// \param s : input string
        /// \param out : the int is initialized to the parsed value if successful
        /// \return : true or false if parsing the string was successful.
        ///
        static bool toInt(const std::string& s, int& out)
        {
            try
            {
                out = std::stoi(s);
                return true;
            }
            catch(...)
            {
                return false;
            }
        }

        ///
        /// \brief toDouble: takes a string and attempts to parse the string into a double
        /// \param s : input string
        /// \return : returns the string parsed as a double or returns 0 if the parsing failed
        ///
        static double toDouble(const std::string& s)
        {
            try
            {
                return std::stod(s);
            }
            catch(...)
            {
                return 0.0;
            }
        }

        ///
        /// \brief toDouble: takes a string and attempts to parse the string to a double
        /// \param s : input string
        /// \param out : the double is initialized to the parsed value if successful
        /// \return : true or false if parsing the string was successful.
        ///
        static bool toDouble(const std::string& s, double& out)
        {
            try
            {
                out =  std::stod(s);
                return true;
            }
            catch(...)
            {
                return false;
            }
        }

        ///
        /// \brief toShort: takes a string and attempts to parse the string into a short
        /// \param s : input string
        /// \return : returns the string parsed as a short or returns 0 if the parsing failed
        ///
        static short toShort(const std::string& s)
        {
            try
            {
                int temp = std::stoi(s);
                if(temp<=std::pow(2,15)-1 && temp>=-1*std::pow(2,15))
                {
                    return (short)temp;
                }
                return 0;
            }
            catch(...)
            {
                return 0;
            }
        }

        ///
        /// \brief toShort: takes a string and attempts to parse the string into a short
        /// \param s : input string
        /// \param out : the short is initialized to the parsed value if successful
        /// \return : true or false if parsing the string was successful.
        ///
        static bool toShort(const std::string& s, short& out)
        {
            try
            {
                int temp = std::stoi(s);
                if(temp<=std::pow(2,15)-1 && temp>=-1*std::pow(2,15))
                {
                    out = (short)temp;
                    return true;
                }
                return false;
            }
            catch(...)
            {
                return false;
            }
        }

        ///
        /// \brief toLong: takes a string and attempts to parse the string into a long
        /// \param s : input string
        /// \return : returns the string parsed as a long or returns 0 if the parsing failed
        ///
        static long toLong(const std::string& s)
        {
            try
            {
                return std::stol(s);
            }
            catch(...)
            {
                return 0;
            }
        }

        ///
        /// \brief toLong: takes a string and attempts to parse the string into a long
        /// \param s : input string
        /// \param out : the long is initialized to the parsed value if successful
        /// \return : true or false if parsing the string was successful.
        ///
        static bool toLong(const std::string& s, long& out)
        {
            try
            {
                out = std::stol(s);
                return true;
            }
            catch(...)
            {
                return false;
            }
        }

        ///
        /// \brief toFloat: takes a string and attempts to parse the string into a float
        /// \param s : input string
        /// \return : returns the string parsed as a float or returns 0 if the parsing failed
        ///
        static float toFloat(const std::string& s)
        {
            try
            {
                return std::stof(s);
            }
            catch(...)
            {
                return 0;
            }
        }

        ///
        /// \brief toFloat: takes a string and attempts to parse the string into a float
        /// \param s : input string
        /// \param out : the float is initialized to the parsed value if successful
        /// \return : true or false if parsing the string was successful.
        ///
        static bool toFloat(const std::string& s, float& out)
        {
            try
            {
                out = std::stof(s);
                return true;
            }
            catch(...)
            {
                return false;
            }
        }

        ///
        /// \brief splitString : takes an input string as splits the string into different pieces based on a delimiter
        /// \param str : input string to split
        /// \param del : delimiter used to identify where the string should be split
        /// \return a vector is returned with the differnt parts of the split string
        ///
        static std::vector<std::string> splitString(std::string str, const std::string& del)
        {

            std::vector<std::string> parsedValues;


            if(del.empty())
            {
                parsedValues.push_back(str);
                return parsedValues;
            }

            size_t deliminterIndex = str.find(del);
            while(deliminterIndex != std::string::npos)
            {
                parsedValues.push_back(str.substr(0,deliminterIndex));
                if(deliminterIndex == str.size()-1)
                {
                    str = "";
                    deliminterIndex = std::string::npos;
                }
                else
                {
                    str = str.substr(deliminterIndex+1);
                    deliminterIndex = str.find(del);
                }
            }
            parsedValues.push_back(str);
            return parsedValues;
        }

        ///
        /// \brief splitStringUnique : takes an input string and splits the string into based on a deliminter and removes empty strings
        /// \param str : input string to split
        /// \param del : delimiter used to identify where to split the string
        /// \return a vector is returned with the split parts of the string. There will not be any empty strings
        ///
        static std::vector<std::string> splitStringUnique(std::string str, const std::string& del)
        {

            std::vector<std::string> parsedValues;

            if(str.empty())
                return parsedValues;

            if(del.empty())
            {
                parsedValues.push_back(str);
                return parsedValues;
            }

            size_t deliminterIndex = str.find(del);
            while(deliminterIndex != std::string::npos)
            {
                std::string token = str.substr(0,deliminterIndex);
                if(!token.empty())
                    parsedValues.push_back(token);
                if(deliminterIndex == str.size()-1)
                {
                    str = "";
                    deliminterIndex = std::string::npos;
                }
                else
                {
                    str = str.substr(deliminterIndex+1);
                    deliminterIndex = str.find(del);
                }
            }
            if(!str.empty())
                parsedValues.push_back(str);
            return parsedValues;
        }

        ///
        /// \brief trim remove \r \n \t and " " from both ends of the string
        /// \param str input string to trim
        /// \return a new string is returned with no whitespace on the ends of the string
        ///
        static std::string trim(std::string str)
        {
            while(starts_with(str,"\n") || starts_with(str,"\r") || starts_with(str,"\t") || starts_with(str," "))
                str = str.substr(1);

            while(ends_with(str,"\n") || ends_with(str,"\r") || ends_with(str,"\t") || ends_with(str," "))
                str = str.substr(0,str.size()-1);

            return str;
        }

        ///
        /// \brief replaceFirst replace the first instance of string with a new string
        /// \param str : input string to adjust
        /// \param oldString : old string pattern to remove from input string
        /// \param newString : new pattern to replace the old pattern
        /// \return returns a new string with the first instance of oldString swapped. return the same string if the oldString does not exist
        ///
        static std::string replaceFirst(std::string str, const std::string& oldString, std::string newString)
        {
            if(oldString.empty() || oldString.size()>str.size())
            {
                return str;
            }
            size_t loc = str.find(oldString);
            if(loc == 0)
                str = newString.append(str.substr(oldString.size()));
            else if(loc == str.size() - oldString.size())
            {
                str = str.substr(0,loc).append(newString);
            }
            else
            {
                str = str.substr(0,loc).append(newString).append(str.substr(loc+oldString.size()));
            }

            return str;
        }

        ///
        /// \brief replaceAll replaces all instances of oldString and replaces them with newString
        /// \param str : input string to adjust
        /// \param oldString : old string pattern to remove
        /// \param newString : new string pattern to replace the old string pattern
        /// \return returns a new string with all instances of oldString replaced. returns the same string if the oldString does not exist
        ///
        static std::string replaceAll(std::string str, const std::string& oldString, const std::string& newString)
        {
            if(oldString.empty() || oldString.empty() || oldString.size()>str.size())
            {
                return str;
            }

            size_t start_pos = 0;
            while((start_pos = str.find(oldString, start_pos)) != std::string::npos) {
                str.replace(start_pos, oldString.length(), newString);
                start_pos += newString.length();
            }

            return str;
        }

        ///
        /// \brief contains determines if the token exists in the input string
        /// \param str : input string to check
        /// \param token : token to look for in input string
        /// \return returns a boolean if the token exists in the string
        ///
        static bool contains(const std::string& str, const std::string& token)
        {
            if(!token.empty() && token.size()<= str.size())
                return str.find(token)!= std::string::npos;
            else
                return false;
        }

        ///
        /// \brief starts_with determines if the string starts with the specified token
        /// \param str : input string to check
        /// \param token : token to look at the beginning of the string
        /// \return returns a boolean if the token exists at the beginning of the string
        ///
        static bool starts_with(const std::string& str, const std::string& token)
        {
            if(token.empty() || token.size()> str.size())
                return false;
            return str.find(token) == 0;
        }

        static bool begins_with(const std::string& str, const std::string& token)
        {
            return starts_with(str, token);
        }

        ///
        /// \brief ends_with determines if the string ends with the specified token
        /// \param str : input string to check
        /// \param token : token to look for at the end of the string
        /// \return  returns a boolean if the token exists at the end of the string
        ///
        static bool ends_with(const std::string& str, const std::string& token)
        {
            if(token.empty() || token.size()> str.size())
                return false;

            return str.find(token,str.size()-token.size()) == str.size()-token.size();
        }


        static bool writeString(const std::string& output, const std::string& location){
            std::ofstream myfile;
            myfile.open (location);
            myfile << output;
            myfile.close();

            return true;
        }

        static bool writeStringAppend(const std::string& output, const std::string& location){
            std::ofstream myfile;
            myfile.open (location, std::ios::app);
            myfile << output;
            myfile.close();

            return true;
        }



    };

}

#endif // PSTRING_H
