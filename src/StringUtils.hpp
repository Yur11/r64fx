#ifndef R64FX_STRING_UTILS_HPP
#define R64FX_STRING_UTILS_HPP

#include <string>

namespace r64fx{
    
/** @brief Fetch next utf-8 sequence from a byte string, 
 *         starting at a specific position.
 * 
 *  @param str Byte string to scan.
 * 
 *  @param pos Position to start scanning at. Expressed in bytes.
 * 
 *  @return Returns the number of bytes in a code point or a negative error code.
*/
int next_utf8(const std::string &str, int pos);


/** @brief Convert utf-8 byte sequence to utf-32. 
 *
 *  Assuming valid utf-8 input.
 */
int to_utf32(const std::string &str, int pos, int size);


std::string hexstr(unsigned char byte);

std::string hexstr(unsigned short word);

std::string hexstr(unsigned int dword);

std::string hexstr(int dword);

}//namespace r64fx

#endif//R64FX_STRING_UTILS_HPP