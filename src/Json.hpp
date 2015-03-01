#ifndef R64FX_JSON_H
#define R64FX_JSON_H

#include <string>
#include <vector>
#include <unordered_map>

namespace r64fx{

class JsonValue;

typedef std::unordered_map<
    std::string,
    JsonValue
> JsonObject;

typedef std::vector<JsonValue> JsonArray;


class JsonValue{

public:
    enum class Type{
        String,
        Number,
        Object,
        Array,
        Boolean,
        Null
    };

private:
    Type _type;

    union Data{
        std::string as_string;
        double as_number;
        JsonObject* as_object;
        JsonArray* as_array;
        bool as_boolean;
        void* as_ptr;

        Data() {}
       ~Data() {}
    } _data;

public:
    JsonValue()
    {
        _type = JsonValue::Type::Null;
    }

    JsonValue(const JsonValue &other)
    {
        operator=(other);
    }

    JsonValue(JsonValue &&other)
    {
        operator=(other);
    }

    JsonValue(std::string str)
    {
        _type = JsonValue::Type::String;
        _data.as_string = str;
    }

    JsonValue(double num)
    {
        _type = JsonValue::Type::Number;
        _data.as_number = num;
    }

    JsonValue(JsonObject* obj)
    {
        _type = JsonValue::Type::Object;
        _data.as_object = obj;
    }

    JsonValue(JsonArray* arr)
    {
        _type = JsonValue::Type::Array;
        _data.as_array = arr;
    }

    JsonValue(bool tf)
    {
        _type = JsonValue::Type::Boolean;
        _data.as_boolean = tf;
    }

    JsonValue &operator=(const JsonValue &other)
    {
        /* Properly copy std::string. Everything else is POD or pointer. */
        if(other.type() == JsonValue::Type::String)
        {
            _type = JsonValue::Type::String;
            _data.as_string = other._data.as_string;
        }
        else
        {
            _type = other._type;
            _data.as_ptr = other._data.as_ptr;
        }
        return *this;
    }

    inline JsonValue::Type type() const { return _type; }

    inline std::string &asString() { return _data.as_string; }

    inline double &asNumber() { return _data.as_number; }

    inline JsonObject* &asObject() { return _data.as_object; }

    inline JsonArray* &asArray() { return _data.as_array; }

    inline bool asBoolean() { return _data.as_boolean; }

    inline bool isString() { return _type == JsonValue::Type::String; }

    inline bool isNumber() { return _type == JsonValue::Type::Number; }

    inline bool isObject() { return _type == JsonValue::Type::Object; }

    inline bool isArray() { return _type == JsonValue::Type::Array; }

    inline bool isBoolean() { return _type == JsonValue::Type::Boolean; }

    inline bool isNull() { return _type == JsonValue::Type::Null; }

    void destroy();
};


/** @brief Json reader/writer. */
class Json{
    unsigned long int index = 0; //Current position in the text, when parsing.
    
    std::string &text;
    
    std::string number_text;
    
    Json(std::string &text) : text(text) {}
    
    JsonValue parse();
    
    JsonObject* object(bool &ok);
    
    JsonArray* array(bool &ok);
    
    std::pair<std::string, JsonValue> key_value_pair();
    
    /* Should return String or Error. */
    std::string key();
    
    JsonValue value();
    
    double number(bool &ok);
    
    /* Use min_code and max_code to switch between 0..9 and 1..9 digit ranges. */
    long int integer(char min_code, char max_code);
    
    std::string quoted_string(bool &ok);

    JsonValue boolean_or_null();
    
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

    void _write(JsonValue value, int &indent_level, std::string &indent_item);
    
public:
    static JsonValue readText(std::string &text);
    
    static JsonValue readFile(std::string file_name);

    static std::string write(JsonValue value, std::string indent_item = "    ");
    
    static std::string readString(JsonValue value, std::string alternative);

    static double readNumber(JsonValue value, double alternative);

    static bool readBoolean(JsonValue value, bool alternative);

    static JsonObject* readObject(JsonValue value, JsonObject* alternative = nullptr);

    static JsonArray* readArray(JsonValue value, JsonArray* alternative = nullptr);

    class Error{
        std::string _message;
        
    public:
        Error(std::string message) : _message(message) {}
        
        inline std::string message() const { return _message; }
    };
};


}//namespace r64fx

#endif//R64FX_JSON_H