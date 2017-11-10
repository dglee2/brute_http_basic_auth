#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>

#define BUFSIZE 50

void usage(){
	printf("read code\n");
}

FILE *get_file(const char *fn){
	FILE *ret = NULL;
	ret = fopen(fn,"r");
	if(ret == NULL)
		printf("%s fopen failed\n",fn);
	return ret;
}

char *get_str(char *buf){
	char *ret;
	int len;

	len = strlen(buf);
	ret = malloc(sizeof(char)*len);
	strncpy(ret,buf,len);
	if(ret[len-1]=='\n')
		ret[len-1]='\0';
	return ret;
}

size_t get_result(char *ptr, size_t size, size_t nmemb, void *data){
	char *line = "HTTP/1.1 200 OK";
	if(strncmp(line,ptr,strlen(line))==0)
		printf(" - OK");
	return size*nmemb;
}

int main(int argc, char *argv[]){
	int ch;
	char buff[BUFSIZE];
	char *str = NULL;
	char *url, *user, *pass;
	FILE *file_t, *file_u, *file_p;
	CURL *curl = NULL;
	CURLcode res;

	static struct option longopts[]={
		{"target",required_argument,NULL,'t'},
		{"user",required_argument,NULL,'u'},
		{"pass",required_argument,NULL,'p'}		
	};

	if(argc != 7){
		usage();
		return 0;
	}

	while((ch = getopt_long(argc,argv,"t:u:p:",longopts,NULL))!=-1){
		switch(ch){
		case 't':
			file_t = get_file(optarg);
			break;
		case 'u':
			file_u = get_file(optarg);
			break;
		case 'p':
			file_p = get_file(optarg);
			break;
		default:
			usage();
			return 0;
		}
	}
	argc -= optind;
	argv += optind;

	if(file_t == NULL || file_u == NULL || file_p == NULL)
		return 0;
	
	curl = curl_easy_init();
	if(curl == NULL)
		return 0;

	curl_easy_setopt(curl,CURLOPT_HEADER,1L);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,get_result);

	while((str = fgets(buff,BUFSIZE,file_t))!=NULL){
		url = get_str(str);
		curl_easy_setopt(curl,CURLOPT_URL,url);	

		while((str = fgets(buff,BUFSIZE,file_u))!=NULL){
			user = get_str(str);
			curl_easy_setopt(curl,CURLOPT_USERNAME,user);

			while((str = fgets(buff,BUFSIZE,file_p))!=NULL){
				pass = get_str(str);
				curl_easy_setopt(curl,CURLOPT_PASSWORD,pass);
				printf("%s, %s-%s",url,user,pass);
				res = curl_easy_perform(curl);
				if(res != CURLE_OK){
					printf("\n\ncurl_easy_perform() failed: %s\n",
						curl_easy_strerror(res));
					return 0;
				}
				printf("\n");
				free(pass);
			}
			rewind(file_p);
			free(user);
		}
		rewind(file_u);
		free(url);
	}

	curl_easy_cleanup(curl);
	fclose(file_t);
	fclose(file_u);
	fclose(file_p);
	return 0;
}
