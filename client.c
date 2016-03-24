/* 1. Tạo các #include cần thiết để gọi hàm socket */
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
/* dành riêng cho AF_INET */
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
using namespace std;

void *LoadFile(void*);
void *LoadMessage(void*);
void RegAccount(int);
void SignIn(int);

int main()
{
	int sockfd; /* số mô tả socket – socket handle */
	int len;
	struct sockaddr_in address; /* structure sockaddr_in, chứa các thông tin về socket AF_INET */
	int result;
	char signal_send, signal_receive;
	char IP[25];
	int port;
	cout << "IP: ";
	gets(IP);
	cout << "Port: ";
	cin >> port;
	/* 2. Tạo socket cho trình khách. Lưu lại số mô tả socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	/* 3. Đặt tên và gán địa chỉ kết nối cho socket theo giao thức Internet */
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(IP);
	address.sin_port = htons(port);
	len = sizeof(address);
	
	/* 4. Thực hiện kết nối */
	result = connect(sockfd, (struct sockaddr*)&address, len);
	if (result == -1) 
	{ 
		perror("Oops: client1 problem");
		return 1;
	}
	
	/* 5. Sau khi socket kết nối, chúng ta có thể đọc ghi dữ liệu của socket tương tự đọc ghi trên file */
	char choice;
	cout << "Chao mung den voi chuong trinh Multichat!" << endl;	
	cout << "Ban co muon tao tai khoan khong? (1.Yes / 2.No): "; 
	cin >> choice;
	cin.ignore(); //xoa buffer
	if (choice == '1')
		RegAccount(sockfd);
	SignIn(sockfd);
	pthread_t thread_id;
	if( pthread_create( &thread_id , NULL , LoadMessage , (void*)&sockfd) < 0) //tao tieu trinh cho tung client ket noi voi server
	{
		cout << "cannot create thread" << endl;
		return 1;
	}
	//Now join the thread , so that we dont terminate before the thread
	//pthread_join( thread_id , NULL); 	
	system("clear");
	cout << "Nhap noi dung ban muon chat o day, nhap $exit de thoat khoi phong chat: " << endl;	
	while(1)
	{
		//system("clear");
		char Message[1000];
		gets(Message);
		signal_send = 'r';
		write(sockfd, &signal_send, 1);
		write(sockfd, &Message, strlen(Message) + 1);
		if (!strcmp(Message, "$exit"))
		{
			close(sockfd);
			return 0;
		}
	}
	//signal_send = 0;
	//write(sockfd, &signal_send,1);
}

void *LoadFile(void* sockfd_desc)
{
	int sockfd = *(int*)sockfd_desc;
	char link[100];
	cout << "Nhap duong dan den noi luu file: ";
	gets(link);
	FILE * pFile;
	int fileSize = 0;
	char *buff;
	read(sockfd, &fileSize, sizeof(int));
	read(sockfd, &buff, fileSize);
	pFile = fopen (link,"w");
	fwrite(buff, fileSize, 1, pFile);
	fclose (pFile);
	return 0;
}

void *LoadMessage(void* sockfd_desc)
{
	int sockfd = *(int*)sockfd_desc;
	char signal ='w';
	while(1)
	{
		char message[1000];
		read(sockfd, &message, 1000);
		cout << message << endl;
		if (message[0] == '!')
		{
			pthread_t thread_id;
			if( pthread_create( &thread_id , NULL , LoadFile , (void*)&sockfd) < 0)
			{
				cout << "cannot create thread" << endl;
				return 1;
			}	
			pthread_join(thread_id, NULL);
		}
	}
	return 0;
}

void SignIn(int sockfd)
{
	char ClientName[100];
	char ClientPassword[100];
	char signal_send;
	char signal_receive;
	
	signal_send = '2'; // Signal dang nhap
	write(sockfd, &signal_send ,1);
	system("clear");
	cout << "--------Dang nhap--------" << endl;
	while (1)
	{
		signal_receive = 0;
		read(sockfd, &signal_receive, 1);
		switch (signal_receive)
		{
			case '1': //server yeu cau client nhap username vaf password
			{
				cout << "Ten tai khoan: ";
				gets(ClientName);
				cout << "Mat khau: ";
				gets(ClientPassword);
				write(sockfd, &ClientName, strlen(ClientName)+1);
				write(sockfd, &ClientPassword, strlen(ClientPassword)+1);
				break;
			}
			case '2': //server yeu cau nhap lai username va password khac
			{
				cout << "Ten dang nhap hoac mat khau khong dung, vui long nhap lai" << endl;
				cout << "Ten tai khoan: ";
				gets(ClientName);
				cout << "Mat khau: ";
				gets(ClientPassword);
				write(sockfd, &ClientName, strlen(ClientName)+1);
				write(sockfd, &ClientPassword, strlen(ClientPassword)+1);
				break;
			}
			case '3': //server yeu cau ket thuc viec dang nhap
			{
				cout << "Chao mung " << ClientName << " da tham gia vao phong chat" << endl;
				cout << "connecting..." << endl;
				sleep(2);
				return;
			}
			//case 0: close(sockfd); return 0;
		}
	}
}

void RegAccount(int sockfd)
{
	char ClientName[100];
	char ClientPassword[100];
	char signal_send;
	char signal_receive;
	signal_send = '1'; // Signal bat dau nhap tai khoan moi
	write(sockfd, &signal_send ,1);
	system("clear");
	cout << "--------Dang ky--------" << endl;
	while (1)
	{
		signal_receive = 0;
		read(sockfd, &signal_receive, 1);
		
		switch (signal_receive)
		{
			case '1': //server yeu cau client nhap username vaf password
			{
				cout << "Ten tai khoan moi: ";
				gets(ClientName);
				cout << "Mat khau moi: ";
				gets(ClientPassword);
				write(sockfd, &ClientName, strlen(ClientName)+1);
				write(sockfd, &ClientPassword, strlen(ClientPassword)+1);
				break;
			}
			case '2': //server yeu cau nhap lai username va password khac
			{
				cout << "Tai khoan da bi trung. Xin tao lai tai khoan khac" << endl;
				cout << "Ten tai khoan moi: ";
				gets(ClientName);
				cout << "Mat khau moi: ";
				gets(ClientPassword);
				write(sockfd, &ClientName, strlen(ClientName)+1);
					write(sockfd, &ClientPassword, strlen(ClientPassword)+1);
				break;
			}
			case '3': //server yeu cau ket thuc viec dang nhap
			{
				cout << "Chuc mung " << ClientName << " da dang ky thanh cong" << endl;
				cout << "waiting..." << endl;
				sleep(2);
				return;
			}
			//case 0: close(sockfd); return 0;
		}
	}

}

