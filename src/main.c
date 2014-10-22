/*
  Copyright (c) 2014 Karthik Nayak

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <unistd.h>
#include "cJSON.h"

#define WRITEBIN "wb"
#define READONLY "r"

char *get_body(char *url, char *bodyfilename)
{
	CURL *curl;
	CURLcode res;
	FILE *bodyfile = NULL;

	curl = curl_easy_init();
	if(curl){
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);

		bodyfile = fopen(bodyfilename, WRITEBIN);
		if (bodyfile == NULL)  
			goto error2;
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, bodyfile);
		
		res = curl_easy_perform(curl);
		
		if(res != CURLE_OK){
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
			goto error1;
		}
		
		curl_easy_cleanup(curl);
		fclose(bodyfile);
	}
	return bodyfilename;
error1:
	fclose(bodyfile);
error2:
        curl_easy_cleanup(curl);
	return NULL;
}

char *get_url(char *bodyfilename)
{
	char *buffer, *out;
	char *base;
	unsigned long int file_size;
	FILE *bodyfile;
	cJSON *json;
	
	bodyfile = fopen(bodyfilename, READONLY);
	if(!bodyfile){
		fprintf(stderr, "Failed to open File\n");
		return NULL;
	}
	
	fseek(bodyfile, 0L, SEEK_END);
	file_size = ftell(bodyfile);
	rewind(bodyfile);
	buffer = calloc(1, file_size + 1);

	if(!buffer){
		fprintf(stderr, "Could not allocate memory\n");
		goto error1;
	}
	
	if(file_size != fread(buffer, 1, file_size, bodyfile)){
		fprintf(stderr, "Could not copy to buffer\n");
		goto error;
	}
	fclose(bodyfile);
	
	json = cJSON_Parse(buffer);
	if(!json){
		fprintf(stderr, "cJSON error in %s", cJSON_GetErrorPtr());
		goto error1;
	}
	free(buffer);
	
	json = json->child->child;
	json = cJSON_GetObjectItem(json, "url");
	
	out = cJSON_Print(json);
	cJSON_Delete(json);
	
	base = calloc(1, strlen(out) + 100);
	if(!base)
		return NULL;
	strcat(base, "http://www.bing.com");
	strcat(base, out+1);
	base[strlen(base) - 1] = '\0';
	
	return base;

error:
	fclose(bodyfile);
error1:
	free(buffer);
	return NULL;
}

int main(int argc, char *argv[])
{
	char *url = NULL;
        char *bodyfilename = NULL;
 	char *url1 = "http://www.bing.com/HPImageArchive.aspx?format=js&idx=0&n=1&nc=1397809837851&pid=hp";
	char *file1 = "body.out";
	char cwd[1024], cmd[1024];

	bodyfilename = get_body(url1, file1);
	if(!bodyfilename){
		fprintf(stderr, "Could not obtain URL body\n");
		return -1;
	}

	url = get_url(bodyfilename);
	if(!url){
		fprintf(stderr, "Could not obtain URL\n");
		return -1;
	}

	bodyfilename = get_body(url, "image.jpg");
	if(!bodyfilename){
		fprintf(stderr, "Could not download image\n");
		return -1;
	}
	
	if(!getcwd(cwd, sizeof(cwd))){
		fprintf(stderr, "Could not get pwd\n");
		return -1;
	}

	remove("body.out");
	system("mkdir -p ~/.bingit");
	sprintf(cmd, "mv %s/image.jpg ~/.bingit/image.jpg", cwd);
	system(cmd);
	sprintf(cmd, "gsettings set org.gnome.desktop.background picture-uri ~/.bingit/image.jpg");
	system(cmd);
		
	return 0;
}
