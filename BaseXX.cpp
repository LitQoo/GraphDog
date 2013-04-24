//
//  BaseXX.cpp
//  desTest
//
//  Created by ksoo k on 13. 4. 19..
//
//

#include "BaseXX.h"
#include <sstream>
#include <vector>

std::vector<char> base2To(const std::string& t)
{
	std::ostringstream oss(std::stringstream::out | std::stringstream::in | std::stringstream::binary);
    std::ostreambuf_iterator<char> out(oss);

	stlencoders::base2<char>::decode(t.begin(), t.end(), out);

	std::istreambuf_iterator<char> itt(oss.rdbuf()), eos;
    std::vector<char> serialVector;
    serialVector.reserve(60); // just a guess
    while(itt != eos) {
        char c = *itt++;
        serialVector.push_back(c);
    }
	return serialVector;
}

std::vector<char> base16To(const std::string& t)
{
	std::ostringstream oss(std::stringstream::out | std::stringstream::in | std::stringstream::binary);
    std::ostreambuf_iterator<char> out(oss);
	
	stlencoders::base16<char>::decode(t.begin(), t.end(), out);
	
	std::istreambuf_iterator<char> itt(oss.rdbuf()), eos;
    std::vector<char> serialVector;
    serialVector.reserve(60); // just a guess
    while(itt != eos) {
        char c = *itt++;
        serialVector.push_back(c);
    }
	return serialVector;
}

std::vector<char> base32To(const std::string& t)
{
	std::ostringstream oss(std::stringstream::out | std::stringstream::in | std::stringstream::binary);
    std::ostreambuf_iterator<char> out(oss);
	
	stlencoders::base32<char>::decode(t.begin(), t.end(), out);
	
	std::istreambuf_iterator<char> itt(oss.rdbuf()), eos;
    std::vector<char> serialVector;
    serialVector.reserve(60); // just a guess
    while(itt != eos) {
        char c = *itt++;
        serialVector.push_back(c);
    }
	return serialVector;
}

std::vector<char> base64To(const std::string& t)
{
	std::ostringstream oss(std::stringstream::out | std::stringstream::in | std::stringstream::binary);
    std::ostreambuf_iterator<char> out(oss);
	
	stlencoders::base64<char>::decode(t.begin(), t.end(), out);
	
	std::istreambuf_iterator<char> itt(oss.rdbuf()), eos;
    std::vector<char> serialVector;
    serialVector.reserve(60); // just a guess
    while(itt != eos) {
        char c = *itt++;
        serialVector.push_back(c);
    }
	return serialVector;
}
