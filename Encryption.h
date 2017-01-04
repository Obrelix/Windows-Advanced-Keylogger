#ifndef ENCRYPTION_H
#define ENCRYPTION_H
#include <string>
std::string encrypt(unsigned char const* , unsigned int len);

static const std::string base64_chars =
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";


static inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}


const std::string &SALT1 = "HH::MM::SS";
const std::string &SALT2 = "87_{:/}-";
const std::string &SALT3 = "KazablancaRC-98";

std::string EncryptMix(std::string s)
{
//    s = SALT1 + s + SALT2 + SALT3;
    s = encrypt(reinterpret_cast<const unsigned char*>(s.c_str()), s.length());
    s.insert(7, SALT3);
    s += SALT1;
    s = encrypt(reinterpret_cast<const unsigned char*>(s.c_str()), s.length());
    s = SALT2 + SALT3 + SALT1 + s;
    s = encrypt(reinterpret_cast<const unsigned char*>(s.c_str()), s.length());
    s.insert(1, "0v");
    s = encrypt(reinterpret_cast<const unsigned char*>(s.c_str()), s.length());
    s.insert(7, "Na");
    s = encrypt(reinterpret_cast<const unsigned char*>(s.c_str()), s.length());
    return s;
}

std::string encrypt(unsigned char const* bytes_to_encode, unsigned int in_len) {
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret += '=';

  }

  return ret;

}


#endif

