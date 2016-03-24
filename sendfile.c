#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
using namespace std;
void SendFile()
{
	
	char link[100];
	cout << "Nhap duong dan den file can gui: ";
	gets(link);
	char fileNameRv[20], fileName[20];
	int j = 0;
	int i = strlen(link)-1;
	while (link[i] != '/')
		fileNameRv[j++] = link[i--];
	fileNameRv[j] = '\0';
	for (i = 0; i < strlen(fileNameRv); i++)
		fileName[i] = fileNameRv[strlen(fileNameRv) - i - 1];
	fileName[j] = '\0';
	FILE * pFile;
  	pFile = fopen (link,"r");
	char mystring[100];
	int fileSize = 0;
	char *buff;
	if (pFile == NULL) perror ("Error opening file");
   	else 
	{
		fseek(pFile, 0, SEEK_END);
		fileSize = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);
		 
		buff = new char[fileSize];
		fread(buff, fileSize, 1, pFile);
	}
	cout << fileSize << endl;
	fclose (pFile);
	cout << "Nhap duong dan den noi luu file: ";
	gets(link);
	pFile = fopen (link,"w");
	fwrite(buff, fileSize, 1, pFile);
	fclose (pFile);
}
int main()
{
	SendFile();
	return 1;
}
