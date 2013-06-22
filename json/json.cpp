#include "json.h"
#include <iostream>
#include <cmath>
#include <sstream>

using namespace std;

namespace r64fx{

template<typename T> string str(T x)
{
     stringstream ss;
     ss << x;
     return ss.str();
}


JsonVariant::JsonVariant(const JsonVariant::Type type)
{
    _type = type;
    switch(type)
    {
        case JsonVariant::Type::Null:
        {
            break;
        }
        
        case JsonVariant::Type::Boolean:
        {
            _data.json_bool = false;
            break;
        }
        
        case JsonVariant::Type::String:
        {
            _data.json_string = new JsonString;
            break;
        }
        
        case JsonVariant::Type::Integer:
        {
            _data.json_integer = 0;
            break;
        }
        
        case JsonVariant::Type::Double:
        {
            _data.json_double = 0.0;
            break;
        }
        
        case JsonVariant::Type::Object:
        {
            _data.json_object = new JsonObject();
            break;
        }
        
        case JsonVariant::Type::Array:
        {
            _data.json_array = new JsonArray();
            break;
        }
        
        default:
        {
            break;
        }
    }
}


void JsonVariant::destroy()
{
    if(_type == JsonVariant::Type::Array)
    {
        for(auto &variant : *_data.json_array)
        {
            variant.destroy();
        }
        
        delete _data.json_array;
    }
    else if(_type == JsonVariant::Type::Object)
    {
        for(auto &variant : *_data.json_object)
        {
            variant.second.destroy();
        }
        
        delete _data.json_object;
    }
    
    _type = JsonVariant::Type::Null;
}


JsonVariant JsonReader::parse()
{
    JsonVariant variant = object();
    if(variant.isNull()) variant = array();
    return variant;
}
   
   
JsonVariant JsonReader::object()
{
    if(!open_object())
        return JsonVariant::Null();
    
    JsonVariant result = JsonVariant::Object();
    
    bool got_comma = false;
    for(;;)
    {
        skip_whitespace();
        
        if(end_of_text())
            throw JsonReader::Error("Unexpected end of text!");
        
        if(close_object())
        {
            if(got_comma)
            {
                throw JsonReader::Error("Missing value after a comma!");
            }
            return result;
        }
        
        JsonPair pair = key_value_pair();
        result.toObject()[pair.key()] = pair.value();
        
        if(end_of_text())
            throw JsonReader::Error("Unexpected end of text!");
        else if(comma())
            got_comma = true;
        else
            got_comma = false;
    }
}


JsonVariant JsonReader::array()
{
    if(!open_array())
        return JsonVariant::Null();
    
    JsonVariant result = JsonVariant::Array();
    
    bool got_comma = false;
    for(;;)
    {
        skip_whitespace();
        
        if(end_of_text())
            throw JsonReader::Error("Unexpected end of text!");
        
        if(close_array())
        {
            if(got_comma)
            {
                throw JsonReader::Error("Missing value after a comma!");
            }
            return result;
        }
        
        JsonVariant variant = value();
        result.toArray().push_back(variant);
        
        if(end_of_text())
            throw JsonReader::Error("Unexpected end of text!");
        else if(comma())
            got_comma = true;
        else
            got_comma = false;
    }
}


JsonPair JsonReader::key_value_pair()
{
    JsonString k = key();
    if(!colon())
        throw JsonReader::Error("Missing colon!");
    return JsonPair(k, value());
}


JsonString JsonReader::key()
{
    JsonVariant str = quoted_string();
    if(!str.isString())
        throw JsonReader::Error("Key must be a string!");
    return str.toString();
}


JsonVariant JsonReader::value()
{
    JsonVariant variant = quoted_string();
    if(!variant.isNull())
        return variant;
    
    variant = number();
    if(!variant.isNull())
        return variant;
    
    variant = object();
    if(!variant.isNull())
        return variant;
    
    variant = array();
    if(!variant.isNull())
        return variant;
    
    return boolean_or_null();
}


JsonVariant JsonReader::number()
{
    skip_whitespace();

    double result;
    int mantissa_sign = 1;
    double exponent_sign;
    long int whole_part;
    long int fraction_part;
    long int exponent_part;
    
    if(next_byte_is<'-'>())
        mantissa_sign = -1;
    else
        next_byte_is<'+'>();
    
    whole_part = integer('0', '9');
    if(number_text.empty())
        return JsonVariant::Null(); //Nan!
    
    if(next_byte_is<'.'>())
        goto _fraction;
    if(next_byte_is<'e'>() || next_byte_is<'E'>())
        goto _scientific;
    else
    {
        return long(whole_part * mantissa_sign);
    }
    
 _fraction:
    fraction_part = integer('0', '9');
    if(number_text.empty())
        throw JsonReader::Error("Missing fraction_part after . token!");
    result = (double(whole_part) + double(fraction_part) / pow(10, number_text.size())) * mantissa_sign;
    return double(result);
 
 _scientific:
    if(next_byte_is<'+'>())
        exponent_sign = 1;
    else if(next_byte_is<'-'>())
        exponent_sign = -1;
    else
        throw JsonReader::Error("Missing exponent sign!");
    
    exponent_part = integer('0', '9');
    result = whole_part * pow(10, exponent_part * exponent_sign) * mantissa_sign;
    
    return double(result);
}

    
long int JsonReader::integer(char min_code, char max_code)
{
    number_text = "";
    long int num = 0;
    
    while(text[index] >= min_code && text[index] <= max_code)
    {
        num *= 10;
        num += text[index] - 48;
        number_text.push_back(text[index]);
        index++;
        
        if(end_of_text())
            throw JsonReader::Error("Unexpected end of text!");
    }
    
    return num;
}


JsonVariant JsonReader::quoted_string()
{
    skip_whitespace();
    
    if(!next_byte_is<'"'>())
        return JsonVariant::Null();

    JsonString* result = new JsonString;
    
    bool escaped = false;
    for(;;)
    {
        if(end_of_text())
            throw JsonReader::Error("Unexpected end of text!");
        
        char ch = text[index++];
        
        if(escaped)
        {
            if(ch == '"')
                result->push_back('"');
            else if(ch == '\\')
                result->push_back('\\');
            else if(ch == 'b')
                result->push_back('\b');
            else if(ch == 'f')
                result->push_back('\f');
            else if(ch == 'n')
                result->push_back('\n');
            else if(ch == 'r')
                result->push_back('\r');
            else if(ch == 't')
                result->push_back('\r');
            else
            {
                result->push_back('\\');
                result->push_back(ch);
            }
            
            escaped = false;
        }
        else
        {
            if(ch == '"')
                return result;
            else if(ch == '\\')
                escaped = true;
            else
                result->push_back(ch);
        }
    }
}


JsonVariant JsonReader::boolean_or_null()
{
    skip_whitespace();
    if(text[index] == 't' 
        && index + 4 < text.size()
        && text[index+1] == 'r' 
        && text[index+2] == 'u' 
        && text[index+3] == 'e'
    ){ index+=4; return true; }
    
    if(text[index] == 'f' 
       && index + 5 < text.size()
       && text[index+1] == 'a' 
       && text[index+2] == 'l' 
       && text[index+3] == 's'
       && text[index+4] == 'e'
    ){ index+=5; return false; }
    
    if(text[index] == 'n' 
        && index + 4 < text.size()
        && text[index+1] == 'u' 
        && text[index+2] == 'l' 
        && text[index+3] == 'l'
    ){ index+=4; return JsonVariant::Null(); }
    
    throw JsonReader::Error("Bad json value!");
}


bool JsonReader::open_object()
{
    skip_whitespace();
    return next_byte_is<'{'>();
}


bool JsonReader::close_object()
{
    skip_whitespace();
    return next_byte_is<'}'>();
}


bool JsonReader::open_array()
{
    skip_whitespace();
    return next_byte_is<'['>();
}


bool JsonReader::close_array()
{
    skip_whitespace();
    return next_byte_is<']'>();
}


bool JsonReader::colon()
{
    skip_whitespace();
    return next_byte_is<':'>();
}


bool JsonReader::comma()
{
    return next_byte_is<','>();
}


bool JsonReader::period()
{
    return next_byte_is<'.'>();
}


bool JsonReader::end_of_text()
{
    return index >= text.size();
}


void JsonReader::skip_whitespace()
{
    while(
        !end_of_text() && 
        (text[index] == ' ' || text[index] == '\t' || text[index] == '\n')
    ) index++;    
}


JsonVariant JsonReader::readText(const std::string &text)
{
    JsonReader reader(text);
    return reader.parse();
}

    
JsonVariant JsonReader::readFile(std::string file_name)
{
    FILE* file = fopen(file_name.c_str(), "r");
    if(!file)
    {
        throw JsonReader::Error("Failed to open file \"" + file_name + "\" !");
    }
    
    string text;
    char c;
    while(fread(&c, 1, 1, file))
    {
        text.push_back(c);
    }
    fclose(file);
    return readText(text);
}


void JsonWriter::write(JsonVariant variant)
{
    switch(variant.type())
    {
        case JsonVariant::Type::Null:
        {
            text += "null";
            break;
        }
        
        case JsonVariant::Type::Boolean:
        {
            if(variant.toBoolean())
                text += "true";
            else
                text += "false";
            break;
        }
        
        case JsonVariant::Type::String:
        {
            text += "\"" + variant.toString() + "\"";
            break;
        }
        
        case JsonVariant::Type::Integer:
        {
            text += str(variant.toInteger());
            break;
        }
        
        case JsonVariant::Type::Double:
        {
            text += str(variant.toDouble());
            break;
        }
        
        case JsonVariant::Type::Object:
        {
//             text += indent();
            text += "{\n";
            indent_level++;
            auto obj = variant.toObject();
            for(auto item : obj)
            {
                JsonPair pair(item);
                text += indent();
                text += "\"" + pair.key() + "\"";
                text += ":";
                write(pair.value());
                text += ",\n";
            }
            text.pop_back();
            text.pop_back();
            indent_level--;
            text += "\n";
            text += indent();
            text += "}";
            break;
        }
        
        case JsonVariant::Type::Array:
        {
//             text += indent();
            text += "{\n";
            indent_level++;
            auto obj = variant.toArray();
            for(auto item : obj)
            {
                text += indent();
                write(item);
                text += ",\n";
            }
            text.pop_back();
            text.pop_back();
            indent_level--;
            text += "\n";
            text += indent();
            text += "}";
            break;
        }
    }
}


JsonString JsonWriter::writeJson(JsonVariant variant, JsonString indent_item)
{
    JsonWriter writer;
    writer.indent_item = indent_item;
    writer.write(variant);
    return writer.text;
}

}//namespace r64fx