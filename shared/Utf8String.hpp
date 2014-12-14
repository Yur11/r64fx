#ifndef UTF_8_STRING_H
#define UTF_8_STRING_H

#include <string>

namespace r64fx{

/** @brief Utf-8 wrapper for std::string. */
class Utf8String{
public:
    std::string stdstr;
    
    Utf8String(std::string str = "") : stdstr(str) {}
    
    Utf8String(const char* cstr) : Utf8String(std::string(cstr)) {}
    
    inline const char* c_str() { return stdstr.c_str(); }
    
    std::string operator[](unsigned int index);
    
    /** @brief First byte index of the next utf-8 character after the character starting at the given byte.
     
        In case of invalid utf-8 encoding function returns -1.
     */
    int next_utf8_char(int index);
    
    /** @brief Number of utf-8 characters in the string.
     
        In case of invalid utf-8 encoding function returns -1.
     */
    int size();
    
    /** @brief The number of bytes from the beginning of the string up to the given character index. 
      
        In case of invalid utf-8 encoding function returns -1.
     */
    inline int bytes_up_to(int index) { return bytes_from_to(0, index); }
   
    /** @brief The number of bytes from the first_byte index up to the given number of characters. 
     
        In case of invalid utf-8 encoding function returns -1.
     */
    int bytes_from_to(int first_byte, int ncharacters);
    
    /** @brief Insert substring at the given position. */
    void insert(int position, Utf8String other);
   
    /** @brief Insert n instances of ch at the given position. */
    void insert(int position, int n, std::string ch);
    
    /** @brief Erase n utf-8 characters starting at the utf-8 character at the given position. */
    void erase(int position, int n);
};

}//namespace r64fx

#endif//UTF_8_STRING_H
