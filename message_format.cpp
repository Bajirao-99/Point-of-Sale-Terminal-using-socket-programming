#include "message_format.h"
#include <iostream>

using namespace std;

/**
 * @brief : Set the Attributes object with given value
 * @param str :  decodes str to sets attributes and its values
 * @return int : 0 if not success, 1 otherwise
 */
int MessageFormat::setAttributes(string str) {
    int state = 0;
    string temp = "";
    vector<string> attr;
    int success = 1;

    /*
     * searches for pattern below, ( ____ represents some string) 
     * { 
     *    ___ : ___,
     *    ___ : ___,
     *        .
     *        . 
     *    ___ : ___  
     * }
     */   
    for(int i = 0; i < str.length(); i++) {
        char c = str[i];
        if(c == ' ' || c == '\t' || c == '\n') continue;
        if(c == '{') {
            if(state != 0) success = 0; // error
            state = 1;
        }
        else if(c == ':') {
            if(state == 0 || state%2 == 0) {success = 0; break;} // error
            attr.push_back(temp);
            temp = "";
            state++;
        }
        else if(c == ',') {
            // if(state == 2 && temp == "1"){ 
            //     attributes[attr[0]] = temp; 
            //     break;
            // }
            if(state < 2 || state%2 == 1) {success = 0; break;} //error
            attr.push_back(temp);
            temp = "";
            state++;
        }
        else if(c == '}') {
            if(state != 6) {success = 0; break;}// error
            attr.push_back(temp);
            temp = "";
            break;
        }
        else {
            temp += c;
        }
        
    }

    // for(string s : attr) {
    //     cout << s << " , ";
    // }
    // cout << "." << endl;

    if(attr.size()%2 == 1) return 0; // error
    
    for(int i = 0; i < attr.size(); i+=2){
        attributes[attr[i]] = attr[i+1];
    }
    
    return success;
}

/**
 * @brief : Get the Attribute object
 * @param str :  attribute who's value is needed
 * @return string :  value of attribute
 */
void MessageFormat::addAttribute(string attribute, string value) {
    attributes[attribute] = value;
}

/**
 * @brief : adds attribute to message
 * @param attribute : attribute to set
 * @param value : value to set
 */
string MessageFormat::getAttribute(string attribute) {
    return attributes[attribute];
}

/**
 * @brief : Get the Request Format in String format = 
 *          {
 *              request_type : ____,
 *              upc_code : ____,
 *              no_of_items : ____
 *          }
 * @return string 
 */
string RequestMessageFormat::getRequestFormatString() {
    string req = "{\n\trequest_type : " + attributes["request_type"] + ", ";
    req += "\n\tupc_code : " + attributes["upc_code"] + ", ";
    req += "\n\tno_of_items : " + attributes["no_of_items"] + "\n}\n";
    return req;
}

/**
 * @brief : Get the Response Format in String format
 *          {
 *              response_type : ____,
 *              item_name : ____,
 *              items_price : ____
 *          }
 *               OR
 *          {
 *              response_type : ____,
 *              total_cost : ____
 *          }
 *               OR
 *          {
 *              response_type : ____,
 *              error_type : ____
 *          }
 * 
 * @return string 
 */
string ResponseMessageFormat::getResponseFormatString() {
    string req = "{\n\tresponse_type : " + attributes["response_type"] + ", ";
    if(attributes["response_type"] == "0") { // OK response
        // cout << "ok response message format, attr[item_name] = " << (this->attributes["item_name"].size() )<< ".."<< endl; 
        if(this->attributes["item_name"].size() == 0) {
            req += "\n\ttotal_cost : " + attributes["total_cost"] + "\n}\n";
        }
        else {
            req += "\n\titem_name : " + attributes["item_name"] + ",";
            req += "\n\titem_price : " + attributes["item_price"] + "\n}\n";
        }
    }
    else { // 1 - Error response
        // cout << "Error response message format :- attribute[\"response_type\"] =  " << attributes["response_type"] << ", size: " << attributes["response_type"].size() << endl;
        req += "\n\terror_type : " + attributes["error_type"] + "\n}\n";    
    }
    return req;
}