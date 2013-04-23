
#ifndef KS_DES_H
#define KS_DES_H

#include "CoreDes.h"
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <assert.h>
template<typename T>
T desCoreEncryption(const T& key8, const T& in8)
{
	assert(key8.size() == 8);
	BIT64 key;
	BIT64 in;
	BIT64 out;
	copy(key8.begin(), key8.end(), &key.b[0]);
	copy(in8.begin(), in8.end(), &in.b[0]);
	des_encryption(key, in, &out);
	return T(&out.b[0], &out.b[8]);
}

template<typename T>
T desEncryption(const T& _key8, const T& _in)
{
	assert(_key8.size() == 8);
	BIT64 key;
	copy(_key8.begin(), _key8.end(), &key.b[0]);
	int loop = _in.size() / 8;
	T result;
	int i;
	for(i=0; i<loop;i++) // 8개까지만 돔.
	{
		T t(&_in[i*8], &_in[i*8 + 8]);
		T desEnc = desCoreEncryption(_key8, t);
		result.insert(result.end(), desEnc.begin(), desEnc.end());
	}
	if(_in.size() % 8 != 0)
	{
		T t(&_in[i*8], &_in[i*8 + _in.size() % 8]);
		t.resize(8, '\0');
		T desEnc = desCoreEncryption(_key8, t);
		result.insert(result.end(), desEnc.begin(), desEnc.end());
	}
	return result;
}

template<typename T>
T desCoreDecryption(const T& key8, const T& in8)
{
	assert(key8.size() == 8);
	BIT64 key;
	BIT64 in;
	BIT64 out;
	copy(key8.begin(), key8.end(), &key.b[0]);
	copy(in8.begin(), in8.end(), &in.b[0]);
	des_decryption(key, in, &out);
	return T(&out.b[0], &out.b[8]);
}

template<typename T>
T desDecryption(const T& _key8, const T& _in)
{
	assert(_key8.size() == 8);
	BIT64 key;
	copy(_key8.begin(), _key8.end(), &key.b[0]);
	int loop = _in.size() / 8;
	T result;
	int i;
	for(i=0; i<loop;i++) // 8개까지만 돔.
	{
		T t(&_in[i*8], &_in[i*8 + 8]);
		T desDec = desCoreDecryption(_key8, t);
		result.insert(result.end(), desDec.begin(), desDec.end());
	}
	result.erase(std::remove(result.begin(), result.end(), '\0'), result.end());
	return result;
}

#endif
