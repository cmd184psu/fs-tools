/***************************************************************
 * (C) Copyright 1998-2010 Chris Delezenski
 * This software is released under the LGPL v2, see COPYING and LGPL
 ***************************************************************/
#ifndef __CURL_PP_H
#define __CURL_PP_H

#include <OpenStringLib.h>
#include <curl/curl.h>

struct MemoryStruct {
	char *memory;
	size_t size;
};


class cCurlList:public cStringList {
	private:
		static size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data) {
			size_t realsize = size * nmemb;
			struct MemoryStruct *mem = (struct MemoryStruct *)data;
			
			mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
			if (mem->memory == NULL) {
				//out of memory!
				printf("not enough memory (realloc returned NULL)\n");
				exit(EXIT_FAILURE);
			}
			memcpy(&(mem->memory[mem->size]), ptr, realsize);
			mem->size += realsize;
			mem->memory[mem->size] = 0;
			return realsize;
		}
	public:	
		bool FromUrl(cString url) {
			CURL *curl_handle=NULL;
			struct MemoryStruct chunk;
			chunk.memory = (char *)malloc(1);  // will be grown as needed by the realloc above
			chunk.size = 0;    // no data at this point 

			curl_global_init(CURL_GLOBAL_ALL);

			//init the curl session
			curl_handle = curl_easy_init();
	
			//cerr<<"url.c_str()="<<url.c_str()<<endl;
			
			// specify URL to get 
			curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
			
			//send all data to this function
			curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, cCurlList::WriteMemoryCallback);
			
			//we pass our 'chunk' struct to the callback function
			curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
			
			//some servers don't like requests that are made without a user-agent field, so we provide one
			curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
			
			//get it! 
			curl_easy_perform(curl_handle);
			//cleanup curl stuff
			curl_easy_cleanup(curl_handle);
			
			//Now, our chunk.memory points to a memory block that is chunk.size
			//bytes big and contains the remote file.
			//Do something nice with it!
			if(chunk.memory && chunk.memory[chunk.size-1]!='\0') {
				chunk.memory[chunk.size-1]='\0';
			} else {
				curl_global_cleanup();
				return false;
			}
			cString stuff=chunk.memory;
			
			FromString(stuff,'\n');
			
			//You should be aware of the fact that at this point we might have an
			//allocated data block, and nothing has yet deallocated that data. So when
			//you're done with it, you should free() it as a nice application.
			if(chunk.memory) free(chunk.memory);
			//we're done with libcurl, so clean it up
			curl_global_cleanup();
			return true;
		}
};

#endif
