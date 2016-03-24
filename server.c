/* server2.c */
/* 1.Tạo các #include cần thiết */
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
using namespace std;

void *SendFile();
void *SendMessage(void *);
void *Messenger(void *);
void *RegAccount(void *);
void *SignIn(void *);
void *RunClient(void *);

struct Client
{
	int ID;
	int client_sockfd = 0;
	char Name[100];
	char Password[100];
};

struct Message
{
	char ClientName[100];
	char Content[100];
};

struct Client ListClient[50];

int main()
{
	int server_sockfd, client_sockfd;
	unsigned int server_len, client_len;
	int n = 0;
	char IP[25];
	int port;
	cout << "IP: ";
	gets(IP);
	cout << "Port: ";
	cin >> port;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	/* 2. Thực hiện khởi tạo socket mới cho trình chủ */
	server_sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	/* 3. Đặt tên và gán địa chỉ kết nối cho socket theo giao thức Internet */
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr( IP );
	server_address.sin_port = htons( port ) ;
	server_len = sizeof( server_address );
	/* 4. Ràng buộc tên với socket */
	bind( server_sockfd, (struct sockaddr *)&server_address, server_len );
	/* 5. Mở hàng đợi nhận kết nối - cho phép đặt hàng vào hàng đợi tối đa 5 kết nối */
	cout << "Nhap $file de gui file" << endl;
	pthread_t threadFile_id;
	if( pthread_create( &threadFile_id , NULL , SendFile , NULL) < 0) //tao tieu trinh cho tung client ket noi voi server
	{
		cout << "cannot create thread" << endl;
		return 1;
	}
	listen( server_sockfd, 50 );
	/* 6. Lặp vĩnh viễn để chờ và xử lý kết nối của trình khách */
	while ( 1 ) 
	{
		char ch;
		cout <<"server waiting..." << endl;
		/* Chờ và chấp nhận kết nối */
		ListClient[n].ID = n;
		ListClient[n].client_sockfd = accept( server_sockfd, (struct sockaddr*)&client_address, &client_len );
		cout << "Client " << n + 1 << " connected" << endl;
		pthread_t thread_id;
		if( pthread_create( &thread_id , NULL , RunClient , (void*) &ListClient[n]) < 0) //tao tieu trinh cho tung client ket noi voi server
		{
			cout << "cannot create thread" << endl;
			return 1;
		}
		n++;
		 
	}
	//pthread_exit (NULL);
	return 1;
}

void *SendFile()
{
	while(1)
	{
		char command[100];
		cout << "Command:" << endl;	
		gets(command);
		if(!strcmp(command, "$file"))
		{
			struct Message FileMessage;
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
			fclose (pFile);

			strcpy(FileMessage.ClientName, "Server");
			strcpy(FileMessage.Content, "gui dem ban 1 tap tin!");

			pthread_t thread_id;
			if( pthread_create( &thread_id , NULL , SendMessage , (void*) &FileMessage) < 0) //tao tieu trinh gui chat di
			{
				cout << "cannot create thread" << endl;
				return 0;
			}		

			for (i = 0; i < 10; i++)
			{
				write(ListClient[i].client_sockfd, &fileSize, sizeof(int));
				write(ListClient[i].client_sockfd, &buff, fileSize);
			}
		}
	}
}

void *Messenger(void *Client_desc)
{
	struct Client Client = *(struct Client*)Client_desc;
	int i;
	char signal = 0;
	struct Message ChatMessage;
	strcpy(ChatMessage.ClientName, Client.Name);
	read(Client.client_sockfd, &ChatMessage.Content, 100);
	if (!strcmp(ChatMessage.Content, "$exit"))
	{
		strcpy(ChatMessage.Content, "da roi khoi phong chat");
		cout << ChatMessage.ClientName << " da roi khong phong chat" << endl;
		signal = 'e';
		
	}
	cout << ChatMessage.ClientName << ": " << ChatMessage.Content << endl;
	
	pthread_t thread_id;
	if( pthread_create( &thread_id , NULL , SendMessage , (void*) &ChatMessage) < 0) //tao tieu trinh gui chat di
	{
		cout << "cannot create thread" << endl;
		return 0;
	}
	pthread_join( thread_id , NULL);
	return (void*)signal;
}

void *SendMessage(void *ChatMessage_desc)
{
	int i;
	char signal;
	struct Message ChatMessage = *(struct Message*)ChatMessage_desc;
	for (i = 0; i < 50; i ++)
	{
		if (ListClient[i].client_sockfd != 0)
		{
			char message[1000];
			strcpy(message, ChatMessage.ClientName);
			strcat(message, ": ");
			strcat(message, ChatMessage.Content);
			write(ListClient[i].client_sockfd, &message, strlen(message) + 1);
		}
	}
	return 0;
}

void *RunClient(void *Client_desc)
{
	struct Client Client = *(struct Client*)Client_desc;
	while (1)
	{		
		char signal_receive = '0';
		read(Client.client_sockfd, &signal_receive, 1);
		switch (signal_receive)
		{
			case '1': // dang ky tai khoan
			{
				pthread_t thread_id;
				if( pthread_create( &thread_id , NULL , RegAccount , (void*) &Client) < 0) //Tao tieu trinh dang ky tai khoan
				{
					cout << "cannot create thread" << endl;
					return 0;
				}
				pthread_join( thread_id , NULL); 
				//pthread_exit (NULL); 
				break;
			}
			case '2': //dang nhap
			{
				pthread_t thread_id;
				if( pthread_create( &thread_id , NULL , SignIn , (void*) &Client) < 0) //Tao tieu trinh dang ky tai khoan
				{
					cout << "cannot create thread" << endl;
					return 0;
				}
				pthread_join( thread_id , &Client_desc); 
				break;
			}
			case 'r':
			{
				void* signal_desc;
				pthread_t thread_id;
				if( pthread_create( &thread_id , NULL , Messenger, (void*)&Client) < 0) //tao tieu trinh cho tung client ket noi voi server
				{
					cout << "cannot create thread" << endl;
					return 0;
				}
				pthread_join(thread_id , &signal_desc);
				char signal = (char)signal_desc;
				if (signal == 'e')
				{
					close( Client.client_sockfd );
					pthread_exit(NULL);
					return 0;
				}
				break;
			}
		}	
		Client = *(struct Client*)Client_desc;
	}
	return 0;
}



void *SignIn(void *Client_desc)
{
	//Get the socket descriptor
	struct Client Client = *(struct Client*)Client_desc;
	int read_size, message_len;
	char ClientName[100], ClientPassword[100];
	char signal = '1';
	int pass = 0, i;
	cout << "Client " << Client.ID + 1 << " dang dang nhap" << endl;
	write(Client.client_sockfd , &signal , 1); //gui tin hieu yeu cau client nhap user name va password
	while (pass == 0)
	{
		for (i = 0; i < 100; i++)
		{
			ClientName[0] = 0;
			ClientPassword[i] = 0;
		}
		read (Client.client_sockfd, &ClientName, 100);	//nhan username va password tu client
		read (Client.client_sockfd, &ClientPassword, 100);
		int i;
		for (i = 0; i < 50; i++) //kiem tra xem ten dang ky da ton tai hay chua
		{
			if (strcmp(ClientName, ListClient[i].Name) == 0) 
			{
				if (strcmp(ClientPassword, ListClient[i].Password) != 0)
				{
					signal = '2';
					write(Client.client_sockfd , &signal , 1); //thong bao sai ten hoac pass
					break;
				}
				else 
				{
					pass++;
					break;
				}
			}
		}
		if (i == 50)
		{
			signal = '2';
			write(Client.client_sockfd , &signal , 1); //thong bao sai ten hoac pass
		}
	}	
	signal = '3';
	write(Client.client_sockfd , &signal , 1); //thong bao dang nhap thanh cong
	cout << "Client " << Client.ID + 1 << " da dang nhap thanh cong" << endl;
	cout << "Name: " << ClientName << " - Password: " << ClientPassword << endl;
	strcpy(Client.Name, ClientName);
	struct Message ChatMessage;
	strcpy(ChatMessage.ClientName, ClientName);
	strcpy(ChatMessage.Content, "da tham gia phong chat");
	pthread_t thread_id;
	if( pthread_create( &thread_id , NULL , SendMessage , (void*) &ChatMessage) < 0) //tao tieu trinh gui chat di
	{
		cout << "cannot create thread" << endl;
		return 0;
	}
	pthread_join( thread_id , NULL);
	return (void*)&Client;
} 

void *RegAccount(void *Client_desc)
{
	//Get the socket descriptor
	struct Client Client = *(struct Client*)Client_desc;
	int read_size, message_len;
	char ClientName[100], ClientPassword[100];
	char signal = '1';
	int pass = 0, i;
	cout << "Client " << Client.ID + 1<< " dang dang ky" << endl;
	write(Client.client_sockfd , &signal , 1); //gui tin hieu yeu cau client nhap user name va password
	signal = '2';
	while (pass == 0)
	{
		for (i = 0; i < 100; i++)
		{
			ClientName[0] = 0;
			ClientPassword[i] = 0;
		}
		read (Client.client_sockfd, &ClientName, 100);	//nhan username va password tu client
		read (Client.client_sockfd, &ClientPassword, 100);
		int i;
		for (i = 0; i < 50; i++) //kiem tra xem ten dang ky da ton tai hay chua
			if (strcmp(ClientName, ListClient[i].Name) == 0)
			{
				write(Client.client_sockfd , &signal , 1); //gui tin hieu thong bao ten dang ky da ton tai, yeu cau nhap ten moi
				break;
			}
		if (i == 50)
			pass++;
	}	
	strcpy(ListClient[Client.ID].Name, ClientName);
	strcpy(ListClient[Client.ID].Password, ClientPassword);
	signal = '3';
	write(Client.client_sockfd , &signal , 1); //gui tin hieu yeu cau client ket thuc viec dang ky
	cout << "Client " << Client.ID + 1<< " da dang ky thanh cong" << endl;
	cout << "Name: " << ClientName << " - Password: " << ClientPassword << endl;
	return 0;
} 

