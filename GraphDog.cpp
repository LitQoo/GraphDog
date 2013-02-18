//
//  GraphDog.cpp
//  archervszombie
//
//  Created by LitQoo on 13. 2. 14..
//
//

#include "GraphDog.h"

size_t GraphDog::WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp){
    size_t realsize = size * nmemb;
    struct GDStruct *mem = (struct GDStruct *)userp;
    
    mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
    
    if (mem->memory == NULL) {
        /* out of memory! */
        exit(EXIT_FAILURE);
    }
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

string GraphDog::connect(string url){
    
    GDStruct chunk;
    chunk.memory = (char*)malloc(1);
    chunk.size = 0;
    
    curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_perform(curl_handle);
    
    string r = chunk.memory;
    if(chunk.memory)free(chunk.memory);
    return r;
}


