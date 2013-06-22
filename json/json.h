#ifndef R64FX_JSON_H
#define R64FX_JSON_H

#include <string>
#include <vector>
#include <map>

namespace r64fx{

typedef std::string JsonString;
class JsonObject;
class JsonPair;
class JsonArray;

/** @brief C++ representation for json values. 
 
    Use one of the constructors to create a new instace of JsonVariant.
    Note that JsonObject and JsonArray instaces must be allocated on the heap.
    
    If the type of the JsonVariant instace is JsonVariant::Type::Bad, then
    probably an error has occurred. In this case you may get the error message
    by calling toString() method.
    
    At the end, call the destroy() method on the root JsonVariant instace,
    to free allocated memory.
 */
class JsonVariant{
public:
    enum class Type{
        Null,
        Boolean,
        String,
        Integer,
        Double,
        Object,
        Array
    };
    
private:
    Type _type;
    
    union Data{
        bool            json_bool;
        JsonString*     json_string;
        long int        json_integer;
        long long       json_long_long;
        double          json_double;
        JsonObject*     json_object;
        JsonArray*      json_array;
        
        Data(){}
        ~Data(){}
    } _data;
    
public:
    JsonVariant() 
    { 
        _type = JsonVariant::Type::Null;
    }
    
    JsonVariant(bool value)
    {
        _type = JsonVariant::Type::Boolean;
        _data.json_bool = value;
    }
    
    JsonVariant(JsonString* value)
    {
        _type = JsonVariant::Type::String;
        _data.json_string = value;
    }
    
    JsonVariant(long int value)
    {
        _type = JsonVariant::Type::Integer;
        _data.json_integer = value;
    }
    
    JsonVariant(double value)
    {
        _type = JsonVariant::Type::Double;
        _data.json_double = value;
    }
    
    JsonVariant(JsonObject* ptr)
    {
        _type = JsonVariant::Type::Object;
        _data.json_object = ptr;
    }
    
    JsonVariant(JsonArray* ptr)
    {
        _type = JsonVariant::Type::Array;
        _data.json_array = ptr;
    }
    
    JsonVariant(const JsonVariant::Type type);
    
    JsonVariant(const JsonVariant &variant)
    {
        _type = variant._type;
        if(variant.type() == JsonVariant::Type::String)
        {
            _data.json_string = variant._data.json_string;
        }
        else
        {
            _data.json_long_long = variant._data.json_long_long;
        }
    }
    
    inline JsonVariant operator=(const JsonVariant &variant)
    {
        _type = variant._type;
        if(variant.type() == JsonVariant::Type::String)
        {
            _data.json_string = variant._data.json_string;
        }
        else
        {
            _data.json_long_long = variant._data.json_long_long;
        }
        
        return *this;
    }
    
    inline bool &toBoolean() { return _data.json_bool; }
    
    inline std::string &toString() { return *_data.json_string; }
    
    inline long int &toInteger() { return _data.json_integer; }
    
    inline double &toDouble() { return _data.json_double; }
    
    inline JsonObject &toObject() { return *_data.json_object; }
    
    inline JsonArray &toArray() { return *_data.json_array; }
    
    inline bool isNull() const { return _type == JsonVariant::Type::Null; }
    
    inline bool isBoolean() const { return _type == JsonVariant::Type::Boolean; }
    
    inline bool isString() const { return _type == JsonVariant::Type::String; }
    
    inline bool isInteger() const { return _type == JsonVariant::Type::Integer; }
    
    inline bool isDouble() const { return _type == JsonVariant::Type::Double; }
    
    inline bool isObject() const { return _type == JsonVariant::Type::Object; }
    
    inline bool isArray() const { return _type == JsonVariant::Type::Array; }
    
    inline JsonVariant::Type type() const { return _type; }
        
    /** @brief Recursively delete this object and all sub-objects. 
     
        Type of this instace is set to Null.
     */
    void destroy();

    inline static JsonVariant Null() { return JsonVariant::Type::Null; }
    
    inline static JsonVariant String() { return JsonVariant::Type::String; }
    
    inline static JsonVariant Object() { return JsonVariant::Type::Object; }
    
    inline static JsonVariant Array() { return JsonVariant::Type::Array; }
};


/** @brief  C++ representation for json objects. 
 
    This is a wrapper around std::map.
 */
class JsonObject : public std::map<JsonString, JsonVariant>{
public:
    JsonObject()
    {
    }
    
    JsonObject(const std::map<JsonString, JsonVariant> &other)
    : std::map<JsonString, JsonVariant>(other)
    {
    }
};


/** @brief C++ representation for json key:value pairs. 
 
    This is a wrapper around std::pair.
 */
class JsonPair : public std::pair<JsonString, JsonVariant>{
public:
    JsonPair() 
    {
    }
    
    JsonPair(JsonString key, JsonVariant value)
    {
        first = key;
        second = value;
    }
    
    JsonPair(std::pair<JsonString, JsonVariant> pair)
    {
        first = pair.first;
        second = pair.second;
    }
    
    inline void setKey(JsonString str)
    {
        first = str;
    }
    
    inline void setValue(JsonVariant value)
    {
        second = value;
    }
    
    inline JsonString key() const { return first; }
    
    inline JsonVariant value() const { return second; }
};


/** @brief C++ wrapper for json array. 
 
    This a wrapper around std::vector of JsonVariant instaces.
 */
class JsonArray : public std::vector<JsonVariant>{
public:
    JsonArray()
    {
    }
    
    JsonArray(const std::vector<JsonVariant> &other)
    : std::vector<JsonVariant>(other)
    {
    }
};


/** @brief Json parser. */
class JsonReader{        
    unsigned long int index = 0; //Current position in the text, when parsing.
    
    const JsonString &text;
    
    JsonString number_text;
    
    JsonReader(const JsonString &text) : text(text) {}
    
    JsonVariant parse();
    
    JsonVariant object();
    
    JsonVariant array();
    
    JsonPair key_value_pair();
    
    /* Should return String or Error. */
    JsonString key();
    
    JsonVariant value();
    
    JsonVariant number();
    
    /* Use min_code and max_code to switch between 0..9 and 1..9 digit ranges. */
    long int integer(char min_code, char max_code);
    
    JsonVariant quoted_string();

    JsonVariant boolean_or_null();
    
    bool open_object();
    
    bool close_object();
    
    bool open_array();
    
    bool close_array();
    
    bool colon();
    
    bool comma();
    
    bool period();

    bool end_of_text();
    
    void skip_whitespace();
    
    
    template<char ch> bool next_byte_is()
    {
        if(text[index] == ch){ index++; return true; }
        return false;
    }
    
public:
    static JsonVariant readText(const std::string &text);
    
    static JsonVariant readFile(std::string file_name);
    
    class Error{
        JsonString _message;
        
    public:
        Error(JsonString message) : _message(message) {}
        
        inline JsonString message() const { return _message; }
    };
};


class JsonWriter{
    JsonString text;
    JsonString indent_item;
    int indent_level = 0;
    
    inline std::string indent()
    {
        std::string str;
        for(int i=0; i<indent_level; i++)
        {
            str += indent_item;
        }
        
        return str;
    }
    
    void write(JsonVariant variant);
    
public:
    static JsonString writeJson(JsonVariant variant, JsonString indent_item = "    ");
};

}//namespace r64fx

#endif//R64FX_JSON_H