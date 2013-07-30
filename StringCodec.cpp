//
//  StringCodec.cpp
//  BasketWorldCup2
//
//  Created by ksoo k on 13. 1. 4..
//
//

#include "StringCodec.h"
#include <sstream>

#include <vector>
#include "BaseXX.h"
#include "KSDes.h"

std::string stringEnc(std::string plainText)
{
	std::string key = "JSHSKSYH";
	return toBase16(desEncryption(key, plainText));
}
std::string stringDecode(std::string encodedText)
{
	std::string key = "JSHSKSYH";
	std::vector<char> r = base16To(encodedText);
	std::string _in(r.begin(), r.end());
	return desDecryption(key, _in);
}