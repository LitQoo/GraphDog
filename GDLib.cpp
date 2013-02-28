#include "GDLib.h"

namespace GraphDogLib {
    
BYTE    mask[8] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x1};

void IP(BIT64 *text)
{
        BYTE    ip_table[64] = { 58, 50, 42, 34, 26, 18, 10, 2,
                                 60, 52, 44, 36, 28, 20, 12, 4,
                                 62, 54, 46, 38, 30, 22, 14, 6,
                                 64, 56, 48, 40, 32, 24, 16, 8,
                                 57, 49, 41, 33, 25, 17,  9, 1,
                                 59, 51, 43, 35, 27, 19, 11, 3,
                                 61, 53, 45, 37, 29, 21, 13, 5,
                                 63, 55, 47, 39, 31, 23, 15, 7 };
        BYTE    i;
        BIT64   t;

        t.dw[0] = t.dw[1] = 0;
        for(i=0;i<64;i++)
        {
                BYTE    temp = ip_table[i] -1;
                if((text->b[temp/8] & mask[temp%8]) != 0)
                        t.b[i/8] |= mask[i%8];
        }
        text->dw[0] = t.dw[0];
        text->dw[1] = t.dw[1];
}

void FP(BIT64 *text)
{
        BYTE    fp_table[64] = { 40,  8, 48, 16, 56, 24, 64, 32,
                                 39,  7, 47, 15, 55, 23, 63, 31,
                                 38,  6, 46, 14, 54, 22, 62, 30,
                                 37,  5, 45, 13, 53, 21, 61, 29,
                                 36,  4, 44, 12, 52, 20, 60, 28,
                                 35,  3, 43, 11, 51, 19, 59, 27,
                                 34,  2, 42, 10, 50, 18, 58, 26,
                                 33,  1, 41,  9, 49, 17, 57, 25 };
        BYTE    i;
        BIT64   t;

        t.dw[0] = t.dw[1] = 0;
        for(i=0;i<64;i++)
        {
                BYTE    temp = fp_table[i] -1;
                if(text->b[temp/8] & mask[temp%8])
                        t.b[i/8] |= mask[i%8];
        }
        text->dw[0] = t.dw[0];
        text->dw[1] = t.dw[1];
}

void MakeKey(BIT64 key, BIT64 keys[16])
{
        BYTE    i,j;
        BYTE    shift[16] = {1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};
        BYTE    init_table[56] = { 57, 49, 41, 33, 25, 17,  9,
                                    1, 58, 50, 42, 34, 26, 18,
                                   10,  2, 59, 51, 43, 35, 27,
                                   19, 11,  3, 60, 52, 44, 36,
                                   63, 55, 47, 39, 31, 23, 15,
                                    7, 62, 54, 46, 38, 30, 22,
                                   14,  6, 61, 53, 45, 37, 29,
                                   21, 13,  5, 28, 20, 12,  4 };

        BYTE    replace_table[48] = { 14, 17, 11, 24,  1,  5,
                                       3, 28, 15,  6, 21, 10,
                                      23, 19, 12,  4, 26,  8,
                                      16,  7, 27, 20, 13,  2,
                                      41, 52, 31, 37, 47, 55,
                                      30, 40, 51, 45, 33, 48,
                                      44, 49, 39, 56, 34, 53,
                                      46, 42, 50, 36, 29, 32 };
        BIT64   bit;

        /* PC-1 */
        bit.dw[0] = bit.dw[1] = 0;
        for(i=0;i<28;i++)
        {
                BYTE    temp = init_table[i] -1;

                if(key.b[temp/8] & mask[temp % 8])
				   bit.b[i/8] |= mask[i%8];
				else 
					;//printf("IP-1 1th\n");

				temp = init_table[i+28] -1;

                if(key.b[temp/8] & mask[temp % 8])
					bit.b[i/8 + 4] |= mask[i%8]; 					
                else 
					;//printf("IP-2 2th\n"); 
				
        }

        /* round 16 key */
        for(i=0;i<16;i++)
        {
                for(j=0;j<shift[i];j++)
                {
                        BYTE temp = (BYTE)(bit.b[0] & 0x80);

                        bit.b[0] <<= 1;
                        if(bit.b[1] & 0x80)
                                bit.b[0] += 1;
                        bit.b[1] <<= 1;
                        if(bit.b[2] & 0x80)
                                bit.b[1] += 1;
                        bit.b[2] <<= 1;
                        if(bit.b[3] & 0x80)
                                bit.b[2] += 1;
                        bit.b[3] <<= 1;
                        if(temp)
                                bit.b[3] += 0x10;

                        temp = (BYTE)(bit.b[4] & 0x80);

                        bit.b[4] <<= 1;
                        if(bit.b[5] & 0x80)
                                bit.b[4] += 1;
                        bit.b[5] <<= 1;
                        if(bit.b[6] & 0x80)
                                bit.b[5] += 1;
                        bit.b[6] <<= 1;
                        if(bit.b[7] & 0x80)
                                bit.b[6] += 1;
                        bit.b[7] <<= 1;
                        if(temp)
                                bit.b[7] += 0x10;

                }

                /* Compress and transpose  ==> 48 bit */
                keys[i].dw[0] = 0;
                keys[i].dw[1] = 0;
                for(j=0;j<48;j++)
                {
                        BYTE    temp = replace_table[j] -1;
                        if(temp < 28)
                        {
                                if(bit.b[temp / 8] & mask[temp%8])
                                        keys[i].b[j/6] |= mask[j%6];
                        } else {
                                temp = temp - 28;
                                if(bit.b[temp/8 + 4] & mask[temp % 8])
                                        keys[i].b[j/6] |= mask[j%6];
                        }
                }
        }
}

void RoundFunction(BIT64 key, BIT64 *text)
{
        BIT64   a;
        BIT64   t;
        BYTE    i,j;
        BYTE    expand_table[48] = { 32,  1,  2,  3,  4,  5,
                                      4,  5,  6,  7,  8,  9,
                                      8,  9, 10, 11, 12, 13,
                                     12, 13, 14, 15, 16, 17,
                                     16, 17, 18, 19, 20, 21,
                                     20, 21, 22, 23, 24, 25,
                                     24, 25, 26, 27, 28, 29,
                                     28, 29, 30, 31, 32,  1 };
        BYTE    tp_table[32] = { 16,  7, 20, 21,
                                 29, 12, 28, 17,
                                  1, 15, 23, 26,
                                  5, 18, 31, 10,
                                  2,  8, 24, 14,
                                 32, 27,  3,  9,
                                 19, 13, 30,  6,
                                 22, 11,  4, 25 };
        BYTE    s_table[8][64] = {
	        {
                14,  0,  4, 15, 13,  7,  1,  4,  2, 14, 15,  2, 11, 13,  8,  1,
                 3, 10, 10,  6,  6, 12, 12, 11,  5,  9,  9,  5,  0,  3,  7,  8,
                 4, 15,  1, 12, 14,  8,  8,  2, 13,  4,  6,  9,  2,  1, 11,  7,
                15,  5, 12, 11,  9,  3,  7, 14,  3, 10, 10,  0,  5,  6,  0, 13
                }, {
                15,  3,  1, 13,  8,  4, 14,  7,  6, 15, 11,  2,  3,  8,  4, 14,
                 9, 12,  7,  0,  2,  1, 13, 10, 12,  6,  0,  9,  5, 11, 10,  5,
                 0, 13, 14,  8,  7, 10, 11,  1, 10,  3,  4, 15, 13,  4,  1,  2,
                 5, 11,  8,  6, 12,  7,  6, 12,  9,  0,  3,  5,  2, 14, 15,  9
                }, {
                10, 13,  0,  7,  9,  0, 14,  9,  6,  3,  3,  4, 15,  6,  5, 10,
                 1,  2, 13,  8, 12,  5,  7, 14, 11, 12,  4, 11,  2, 15,  8,  1,
                13,  1,  6, 10,  4, 13,  9,  0,  8,  6, 15,  9,  3,  8,  0,  7,
                11,  4,  1, 15,  2, 14, 12,  3,  5, 11, 10,  5, 14,  2,  7, 12
                }, {
                 7, 13, 13,  8, 14, 11,  3,  5,  0,  6,  6, 15,  9,  0, 10,  3,
                 1,  4,  2,  7,  8,  2,  5, 12, 11,  1, 12, 10,  4, 14, 15,  9,
                10,  3,  6, 15,  9,  0,  0,  6, 12, 10, 11,  1,  7, 13, 13,  8,
                15,  9,  1,  4,  3,  5, 14, 11,  5, 12,  2,  7,  8,  2,  4, 14
                }, {
                 2, 14, 12, 11,  4,  2,  1, 12,  7,  4, 10,  7, 11, 13,  6,  1,
                 8,  5,  5,  0,  3, 15, 15, 10, 13,  3,  0,  9, 14,  8,  9,  6,
                 4, 11,  2,  8,  1, 12, 11,  7, 10,  1, 13, 14,  7,  2,  8, 13,
                15,  6,  9, 15, 12,  0,  5,  9,  6, 10,  3,  4,  0,  5, 14,  3
                }, {
                12, 10,  1, 15, 10,  4, 15,  2,  9,  7,  2, 12,  6,  9,  8,  5,
                 0,  6, 13,  1,  3, 13,  4, 14, 14,  0,  7, 11,  5,  3, 11,  8,
                 9,  4, 14,  3, 15,  2,  5, 12,  2,  9,  8,  5, 12, 15,  3, 10,
                 7, 11,  0, 14,  4,  1, 10,  7,  1,  6, 13,  0, 11,  8,  6, 13
                }, {
                 4, 13, 11,  0,  2, 11, 14,  7, 15,  4,  0,  9,  8,  1, 13, 10,
                 3, 14, 12,  3,  9,  5,  7, 12,  5,  2, 10, 15,  6,  8,  1,  6,
                 1,  6,  4, 11, 11, 13, 13,  8, 12,  1,  3,  4,  7, 10, 14,  7,
                10,  9, 15,  5,  6,  0,  8, 15,  0, 14,  5,  2,  9,  3,  2, 12,
                }, {
                13,  1,  2, 15,  8, 13,  4,  8,  6, 10, 15,  3, 11,  7,  1,  4,
                10, 12,  9,  5,  3,  6, 14, 11,  5,  0,  0, 14, 12,  9,  7,  2,
                 7,  2, 11,  1,  4, 14,  1,  7,  9,  4, 12, 10, 14,  8,  2, 13,
                 0, 15,  6, 12, 10,  9, 13,  0, 15,  3,  3,  5,  5,  6,  8, 11,
	        }
        };

        /* Expand */
        t.dw[0] = t.dw[1] = 0;
        for(i=0;i<48;i++)
        {
                j = expand_table[i] + 31;
                if(text->b[j/8] & mask[j%8])
                {
                        t.b[i/6] |= mask[i%6];
                }
        }

	/* XOR */
        for(i=0;i<8;i++)
        {
                t.b[i] ^= key.b[i];
        }

        /* S-BOX */
        a.dw[0] = a.dw[1] = 0;
        for(i=0;i<8;i++)
        {
                if(i%2 == 0)
                        a.b[i/2] = s_table[i][t.b[i]>>2] << 4;
                else
                        a.b[i/2] += s_table[i][t.b[i]>>2];
        }

        /* Transpose */
        t.dw[0] = t.dw[1] = 0;
        for(i=0;i<32;i++)
        {
                BYTE    temp = tp_table[i] -1;
                if(a.b[temp / 8] & mask[temp%8])
                        t.b[i/8] |= mask[i%8];
        }

        text->dw[0] ^= t.dw[0];
}

void des_encrytion(BIT64 key, BIT64 plain, BIT64 *out)
{
	BIT64	keys[16];
        BYTE    i;
        DWORD   t;

	/* Make Key */
	MakeKey(key,keys);

        /* Plain -> Out */
        out->dw[0] = plain.dw[0];
        out->dw[1] = plain.dw[1];

	/* IP */
	IP(out);

	/* Round 16 */
	for(i=0;i<16;i++)
	{
		/* Function */
		RoundFunction(keys[i],out);
		/* Swap */
                t = out->dw[0];
                out->dw[0] = out->dw[1];
                out->dw[1] = t;
	}

	/* Reset Swap */
        t = out->dw[0];
        out->dw[0] = out->dw[1];
        out->dw[1] = t;

	/* IP(-1) */
	FP(out);
}

void des_decrytion(BIT64 key, BIT64 plain, BIT64 *out)
{
	BIT64	keys[16];
        BYTE    i;
        DWORD   t;

	/* Make Key */
	MakeKey(key,keys);

        /* Plain -> Out */
        out->dw[0] = plain.dw[0];
        out->dw[1] = plain.dw[1];

	/* IP */
	IP(out);

	/* Round 16 */
	for(i=0;i<16;i++)
	{
		/* Function */
		RoundFunction(keys[15-i],out);
		/* Swap */
                t = out->dw[0];
                out->dw[0] = out->dw[1];
                out->dw[1] = t;
	}

	/* Reset Swap */
        t = out->dw[0];
        out->dw[0] = out->dw[1];
        out->dw[1] = t;

	/* IP(-1) */
	FP(out);
}
#include <vector>
vector<char> des_enc(string secretKey, string msg)
{
	vector<char> subResult;
	string a1,start;
	BIT64   key,in,out,iv;
	DWORD   i,j;
	int imsi,imsi1;
	//BYTE    KEY[8] = {0x02,0x06,0x05,0x01,0x03,0x02,0x02,0x04};




	/*for(i=0;i<8;i++)
	{
	key.b[i] = KEY[i];
	}*/

	string m_iv = "11111111";
	imsi = m_iv.length();
	imsi1 = (imsi)%8;
	imsi = (imsi-1)/8;

	if (imsi1 > 0) {
		for (j=0 ; j < (8-imsi1) ; j++){
			m_iv += " ";
		}
	}

	start = "11111111";//m_iv.Mid(0,8);
	for(i=0;i<8;i++)
	{					
		iv.b[i] = start[i];
	}



	imsi = secretKey.length();
	imsi1 = (imsi)%8;
	imsi = (imsi-1)/8;

	if (imsi1 > 0) {
		for (j=0 ; j < (8-imsi1) ; j++){
			secretKey += " ";
		}
	}
	start = secretKey;
	for(i=0;i<8;i++)
	{
		key.b[i] = start[i];
	}


	imsi = msg.length();
	imsi1 = (imsi)%8;
	imsi = (imsi-1)/8;

	if (imsi1 > 0) {
		for (j=0 ; j < (8-imsi1) ; j++){
			msg += " ";
		}
	}
	for (j=0 ; j <= imsi ; j++){
		start.assign(msg.begin() + 8*j, msg.begin() + 8*j + 8);
		for(i=0;i<8;i++)
		{				
			in.b[i] = start[i];
		}
		des_encrytion(key,in,&out);
		for(int i=0; i<8; i++)
			subResult.push_back(out.b[i]);		
	}
	return subResult;
}

// base64 func


static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";


static inline bool is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(char const* bytes_to_encode, unsigned int in_len) {
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

std::string base64_decode(std::string const& encoded_string) {
	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;
    
	while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i ==4) {
			for (i = 0; i <4; i++)
				char_array_4[i] = base64_chars.find(char_array_4[i]);
            
			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
            
			for (i = 0; (i < 3); i++)
				ret += char_array_3[i];
			i = 0;
		}
	}
    
	if (i) {
		for (j = i; j <4; j++)
			char_array_4[j] = 0;
        
		for (j = 0; j <4; j++)
			char_array_4[j] = base64_chars.find(char_array_4[j]);
        
		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
        
		for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
	}
    
	return ret;
}

std::string gdkeyEnc(const std::string& msg,string key)
{
	vector<char> desed = des_enc(key, msg);
	return base64_encode(&desed[0], desed.size());
}

std::string GDCreateToken(string auID,string udid,string flag,string nick,string email,string platform,string cTime,string secretKey){
    string msg = auID;
    msg.append("||");
    msg.append(udid);
    msg.append("||");
    msg.append(flag);
    msg.append("||");
    msg.append(nick);
    msg.append("||");
    msg.append(email);
    msg.append("||");
    msg.append(platform);
    msg.append("||");
    msg.append(cTime);
    msg.append("||");
    
    vector<char> desed = des_enc(secretKey, msg);
	return base64_encode(&desed[0], desed.size());
}

std::string getDeviceID() {
    string _id;
    
    #if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    char* macAddress=(char*)malloc(18);
        std::string ifName="en0";
        int  success;
        struct ifaddrs * addrs;
        struct ifaddrs * cursor;
        const struct sockaddr_dl * dlAddr;
        const unsigned char* base;
        int i;
    
        success = getifaddrs(&addrs) == 0;
        if (success) {
            cursor = addrs;
            while (cursor != 0) {
                if ( (cursor->ifa_addr->sa_family == AF_LINK)
                    && (((const struct sockaddr_dl *) cursor->ifa_addr)->sdl_type == 0x06) && strcmp(ifName.c_str(),  cursor->ifa_name)==0 ) {
                    dlAddr = (const struct sockaddr_dl *) cursor->ifa_addr;
                    base = (const unsigned char*) &dlAddr->sdl_data[dlAddr->sdl_nlen];
                    strcpy(macAddress, "");
                    for (i = 0; i < dlAddr->sdl_alen; i++) {
                        if (i != 0) {
                            strcat(macAddress, ":");
                        }
                        char partialAddr[3];
                        sprintf(partialAddr, "%02X", base[i]);
                        strcat(macAddress, partialAddr);
    
                    }
                }
                cursor = cursor->ifa_next;
            }
    
            freeifaddrs(addrs);
        }
    _id = macAddress;
#else
    
    //_id=NULL; //##
#endif
    //string _id = base64_decode(macAddress);
    
    return base64_encode(_id.c_str(), _id.length());// _id;
}
    
}
