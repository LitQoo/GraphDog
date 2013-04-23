//
//  BaseXX.h
//  desTest
//
//  Created by ksoo k on 13. 4. 19..
//
//

#ifndef __desTest__BaseXX__
#define __desTest__BaseXX__

#include "stlencoders/base2.hpp"
#include "stlencoders/base16.hpp"
#include "stlencoders/base32.hpp"
#include "stlencoders/base64.hpp"

#include <string>
#include <sstream>
#include <vector>

template <typename T>
std::string toBase2(T t)
{
	std::ostringstream oss;
    std::ostreambuf_iterator<char> out(oss);
    stlencoders::base2<char>::encode(t.begin(), t.end(), out);
	return oss.str();
}

template <typename T>
std::string toBase16(T t)
{
	std::ostringstream oss;
    std::ostreambuf_iterator<char> out(oss);
    stlencoders::base16<char>::encode(t.begin(), t.end(), out);
	return oss.str();

}
template <typename T>
std::string toBase32(T t)
{
	std::ostringstream oss;
    std::ostreambuf_iterator<char> out(oss);
    stlencoders::base32<char>::encode(t.begin(), t.end(), out);
	return oss.str();

}
template <typename T>
std::string toBase64(T t)
{
	std::ostringstream oss;
    std::ostreambuf_iterator<char> out(oss);
    stlencoders::base64<char>::encode(t.begin(), t.end(), out);
	return oss.str();
}

std::vector<char> base2To(std::string t); // std::string str(v.begin(), v.end())
std::vector<char> base16To(std::string t);
std::vector<char> base32To(std::string t);
std::vector<char> base64To(std::string t);



#endif /* defined(__desTest__BaseXX__) */
