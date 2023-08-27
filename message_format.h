#include <string>
#include <vector>
#include <map>

using namespace std;

/**
 * @brief : Base class for request and response message format
 */
class MessageFormat {
    protected:
    
    map<string, string> attributes; // stores attributes and its values

    public:

    /**
     * @brief : Set the Attributes object with given value
     * @param str :  decodes str to sets attributes and its values
     * @return int : 0 if not success, 1 otherwise
     */
    int setAttributes(string str); 

    /**
     * @brief : Get the Attribute object
     * @param str :  attribute who's value is needed
     * @return string :  value of attribute
     */
    string getAttribute(string str);

    /**
     * @brief : adds attribute to message
     * @param attribute : attribute to set
     * @param value : value to set
     */
    void addAttribute(string attribute, string value);
};

/**
 * @brief request message format
 */
class RequestMessageFormat : public MessageFormat {
    public:

    /**
     * @brief : Get the Request Format in String format
     * @return string 
     */
    string getRequestFormatString();
};

class ResponseMessageFormat : public MessageFormat {
    public:
    
    /**
     * @brief : Get the Response Format in String format
     * @return string 
     */
    string getResponseFormatString();

};