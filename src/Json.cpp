#include "Json.hpp"
#include <iostream>
#include <cmath>
#include <sstream>


static_assert(
    sizeof(std::string) == sizeof(void*),
    "r64fx::JsonValue is assuming that std::string is as large as void*. But that is not true on your platform!"
);

static_assert(
    sizeof(long int) == sizeof(void*),
    "r64fx::JsonValue is assuming that long int is as large as void*. But that is not true on your platform!"
);

static_assert(
    sizeof(double) == sizeof(void*),
    "r64fx::JsonValue is assuming that double is as large as void*. But that is not true on your platform!"
);


using namespace std;

namespace r64fx{

template<typename T> string str(T x)
{
     stringstream ss;
     ss << x;
     return ss.str();
}


void JsonValue::destroy()
{
    if(this->isObject())
    {
        for(auto &item : *asObject())
        {
            item.second.destroy();
        }

        delete asObject();
    }
    else if(this->isArray())
    {
        for(auto &item : *asArray())
        {
            item.destroy();
        }

        delete asArray();
    }
}


JsonValue Json::parse()
{
    bool ok;
    JsonObject* obj = object(ok);
    if(ok) return JsonValue(obj);
    return JsonValue(array(ok));
}


JsonObject* Json::object(bool &ok)
{
    if(!open_object())
    {
        ok = false;
        return nullptr;
    }

    auto obj = new JsonObject;

    bool got_comma = false;
    for(;;)
    {
        skip_whitespace();

        if(end_of_text())
            throw Json::Error("Unexpected end of text!");

        if(close_object())
        {
            if(got_comma)
            {
                throw Json::Error("Missing value after a comma!");
            }
            ok = true;
            return obj;
        }

        auto pair = key_value_pair();
        (*obj)[pair.first] = pair.second;

        if(end_of_text())
            throw Json::Error("Unexpected end of text!");
        else if(comma())
            got_comma = true;
        else
            got_comma = false;
    }
}


JsonArray* Json::array(bool &ok)
{
    if(!open_array())
    {
        ok = false;
        return nullptr;
    }

    auto arr = new JsonArray;

    bool got_comma = false;
    for(;;)
    {
        skip_whitespace();

        if(end_of_text())
            throw Json::Error("Unexpected end of text!");

        if(close_array())
        {
            if(got_comma)
            {
                throw Json::Error("Missing value after a comma!");
            }
            ok = true;
            return arr;
        }

        JsonValue variant = value();
        arr->push_back(variant);

        if(end_of_text())
            throw Json::Error("Unexpected end of text!");
        else if(comma())
            got_comma = true;
        else
            got_comma = false;
    }
}


std::pair<std::string, JsonValue> Json::key_value_pair()
{
    std::pair<std::string, JsonValue> p;
    p.first = key();
    if(!colon())
        throw Json::Error("Missing colon!");
    p.second = value();
    return p;
}


std::string Json::key()
{
    bool ok;
    std::string str = quoted_string(ok);
    if(!ok)
        throw Json::Error("Key must be a string!");
    return str;
}


JsonValue Json::value()
{
    bool ok = true;

    auto str = quoted_string(ok);
    if(ok)
        return str;

    ok = true;
    auto num = number(ok);
    if(ok)
        return num;

    ok = true;
    auto obj = object(ok);
    if(ok)
        return obj;

    ok = true;
    auto arr = array(ok);
    if(ok)
        return arr;

    return boolean_or_null();
}


double Json::number(bool &ok)
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
    {
        ok = false;
        return 0.0;
    }

    if(next_byte_is<'.'>())
        goto _fraction;
    if(next_byte_is<'e'>() || next_byte_is<'E'>())
        goto _scientific;
    else
    {
        ok = true;
        return long(whole_part * mantissa_sign);
    }

 _fraction:
    fraction_part = integer('0', '9');
    if(number_text.empty())
        throw Json::Error("Missing fraction_part after . token!");
    result = (double(whole_part) + double(fraction_part) / pow(10, number_text.size())) * mantissa_sign;
    ok = true;
    return double(result);

 _scientific:
    if(next_byte_is<'+'>())
        exponent_sign = 1;
    else if(next_byte_is<'-'>())
        exponent_sign = -1;
    else
        throw Json::Error("Missing exponent sign!");

    exponent_part = integer('0', '9');
    result = whole_part * pow(10, exponent_part * exponent_sign) * mantissa_sign;

    ok = true;
    return double(result);
}


long int Json::integer(char min_code, char max_code)
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
            throw Json::Error("Unexpected end of text!");
    }

    return num;
}


std::string Json::quoted_string(bool &ok)
{
    skip_whitespace();

    if(!next_byte_is<'"'>())
    {
        ok = false;
        return "";
    }

    std::string result;

    bool escaped = false;
    for(;;)
    {
        if(end_of_text())
            throw Json::Error("Unexpected end of text!");

        char ch = text[index++];

        if(escaped)
        {
            if(ch == '"')
                result.push_back('"');
            else if(ch == '\\')
                result.push_back('\\');
            else if(ch == 'b')
                result.push_back('\b');
            else if(ch == 'f')
                result.push_back('\f');
            else if(ch == 'n')
                result.push_back('\n');
            else if(ch == 'r')
                result.push_back('\r');
            else if(ch == 't')
                result.push_back('\r');
            else
            {
                result.push_back('\\');
                result.push_back(ch);
            }

            escaped = false;
        }
        else
        {
            if(ch == '"')
            {
                ok = true;
                return result;
            }
            else if(ch == '\\')
                escaped = true;
            else
                result.push_back(ch);
        }
    }
}


JsonValue Json::boolean_or_null()
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
    ){ index+=4; return JsonValue(); }

    throw Json::Error("Bad json value!");
}


bool Json::open_object()
{
    skip_whitespace();
    return next_byte_is<'{'>();
}


bool Json::close_object()
{
    skip_whitespace();
    return next_byte_is<'}'>();
}


bool Json::open_array()
{
    skip_whitespace();
    return next_byte_is<'['>();
}


bool Json::close_array()
{
    skip_whitespace();
    return next_byte_is<']'>();
}


bool Json::colon()
{
    skip_whitespace();
    return next_byte_is<':'>();
}


bool Json::comma()
{
    return next_byte_is<','>();
}


bool Json::period()
{
    return next_byte_is<'.'>();
}


bool Json::end_of_text()
{
    return index >= text.size();
}


void Json::skip_whitespace()
{
    while(
        !end_of_text() &&
        (text[index] == ' ' || text[index] == '\t' || text[index] == '\n')
    ) index++;
}


JsonValue Json::readText(std::string &text)
{
    Json reader(text);
    return reader.parse();
}


JsonValue Json::readFile(std::string file_name)
{
    FILE* file = fopen(file_name.c_str(), "r");
    if(!file)
    {
        throw Json::Error("Failed to open file \"" + file_name + "\" !");
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


inline std::string indent(int indent_level, std::string indent_item)
{
    std::string str;
    for(int i=0; i<indent_level; i++)
    {
        str += indent_item;
    }

    return str;
}


void Json::_write(JsonValue variant, int &indent_level, string &indent_item)
{
    switch(variant.type())
    {
        case JsonValue::Type::Null:
        {
            text += "null";
            break;
        }

        case JsonValue::Type::Boolean:
        {
            if(variant.asBoolean())
                text += "true";
            else
                text += "false";
            break;
        }

        case JsonValue::Type::String:
        {
            text += "\"" + variant.asString() + "\"";
            break;
        }

        case JsonValue::Type::Number:
        {
            text += str(variant.asNumber());
            break;
        }

        case JsonValue::Type::Object:
        {
            text += "{\n";
            indent_level++;
            auto obj = variant.asObject();
            for(auto &item : *obj)
            {
                text += indent(indent_level, indent_item);
                text += "\"" + item.first + "\"";
                text += ":";
                _write(item.second, indent_level, indent_item);
                text += ",\n";
            }
            text.pop_back();
            text.pop_back();
            indent_level--;
            text += "\n";
            text += indent(indent_level, indent_item);
            text += "}";
            break;
        }

        case JsonValue::Type::Array:
        {
            text += "{\n";
            indent_level++;
            auto obj = variant.asArray();
            for(auto &item : *obj)
            {
                text += indent(indent_level, indent_item);
                _write(item, indent_level, indent_item);
                text += ",\n";
            }
            text.pop_back();
            text.pop_back();
            indent_level--;
            text += "\n";
            text += indent(indent_level, indent_item);
            text += "}";
            break;
        }
    }
}


std::string Json::write(JsonValue variant, std::string indent_item)
{
    int indent_level = 0;
    string text;
    Json json(text);
    json._write(variant, indent_level, indent_item);
    return text;
}


std::string Json::readString(JsonValue value, std::string alternative)
{
    if(value.isString())
        return value.asString();
    else
        return alternative;
}


double Json::readNumber(JsonValue value, double alternative)
{
    if(value.isNumber())
        return value.asNumber();
    else
        return alternative;
}


JsonObject* Json::readObject(JsonValue value, JsonObject* alternative)
{
    if(value.isObject())
        return value.asObject();
    else
        return alternative;
}


JsonArray* Json::readArray(JsonValue value, JsonArray* alternative)
{
    if(value.isArray())
        return value.asArray();
    else
        return alternative;
}


bool Json::readBoolean(JsonValue value, bool alternative)
{
    if(value.isBoolean())
        return value.asBoolean();
    else
        return alternative;
}

}//namespace r64fx