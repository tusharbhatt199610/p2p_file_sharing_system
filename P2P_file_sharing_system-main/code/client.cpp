// Client side C/C++ program to demonstrate Socket programming 


#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h>
#include<pthread.h>
#include<bits/stdc++.h>
#include<cstdlib>
#include <openssl/sha.h>
#include<fstream>
// #define PORT 0
# define SHA_DIGEST_LENGTH 20
using namespace std;

//server thread variables

int client_as_server_port=0;
int number_of_clients=0;
int number_of_peers=0;
int server_fd, new_socket, valread;

struct sockaddr_in address; 
int opt = 1; 
int addrlen = sizeof(address); 

char buffer[1024] = {0}; 
pthread_t client_as_server,client_as_client;
// char *hello = "Hello from server"; 

struct file_download_details{
	uint32_t peer_to_connect;
	string file_d_name;
	int number_of_pieces;
	int index;
	vector<string> sha_per_piece;
	int total_number_of_pieces;
	string destination_path;
	// ofstream new_file;
};

int do_sha1_file(char *name, unsigned char *out,size_t a);
void read_file_in_chunks(string file_name,vector<string> &sha_chunkwise);
void *communicate_as_server(void *a);
void *communicate(void *a);
void check_requests_as_server();
void make_server_code();
void* open_server(void *a);
void* connect_client(void *a);
void start_server_thread();
void start_client_thread(int *a);
vector<string> process_query(string s);

void read_file_in_chunks(string file_name,vector<string> &sha_chunkwise)
{	

	//her size value is 10 power 1 Bytes.     a<<b means (2 power b)*a bytes
	size_t buffer_size = 1<<19;
	// cout<<"buffer size "<<buffer_size;
	char *buffer = new char[buffer_size];

	std::ifstream fin(file_name);

	while (fin)
	{
    // Try to read next chunk of data
		fin.read(buffer, buffer_size);
    // Get the number of bytes actually read
		size_t count = fin.gcount();
    // If nothing has been read, break
		if (!count) 
			break;
		else
		{
			// cout<<"inside else\n";
			unsigned char out[SHA_DIGEST_LENGTH];
			do_sha1_file(buffer,out,buffer_size);

			string s((char *)out);
			sha_chunkwise.push_back(s);
			// cout<<buffer;
		}
			// cout<<"new chunk\n";

    // Do whatever you need with first count bytes in the buffer
    // ...
	}

	delete[] buffer;
}




int do_sha1_file(char *name, unsigned char *out,size_t len)
{
	FILE *f;

    //buf size of 512kb
	// unsigned char buf[524288];
	SHA_CTX sc;
	int err;

	// f = fopen(name, "rb");
	// if (f == NULL) {
 //         do something smart here: the file could not be opened 
	// 	return -1;
	// }
	SHA1_Init(&sc);

	// for (;;) {
	// 	size_t len;

	// 	len = fread(buf, 1, sizeof buf, f);
	// 	if (len == 0)
	// 		break;
	SHA1_Update(&sc, name, len);

        // string piece((char *)buf);

	// }	
	// err = ferror(f);
	// fclose(f);
	// if (err) {
 //         some I/O error was encountered; report the error 
	// 	return -1;
	// }
	SHA1_Final(out, &sc);
	return 0;
}


void* communicate(void *p_client)
{
	// cout<<"in func\n";
	int current_client_number;
	current_client_number=number_of_clients;
	int client=*((int *)p_client);
	// free(p_client);
	cout<<"client number "<<number_of_clients<<"\n";
	
	while(1)
	{

        // if(check_current_connection(client))
        // {
        //     break;
        // }
		// cout<<"in while\n";  
		int a=read( client , buffer, 1024);
		if(a==0)
		{
			
			break;
		} 
		
		printf("%s",buffer);
		memset(buffer, 0, sizeof(buffer));
		// cout<<"  this message if from "<<current_client_number<<"\n";
    // continue;

    // send(client , hello , strlen(hello) , 0 );
    // hello=&s[0];
    // printf("message recieved from client number ");
    // cout<<current_client_number<<"\n";
    // buffer[1024] = {0}; 
	}

	return NULL;
}


void *communicate_as_server(void *peer)
{
	char *hello="hello";
	cout<<"peer send file to other peer \n";
	int peer_number=*((int *)peer);
	// free(peer);

	char buffer_for_peer[1024] = {0}; 
	int current_peer_number=number_of_peers+1;
	number_of_peers++;
	
	cout<<"peer number is  "<<  current_peer_number<<"\n";
	read(peer_number,buffer_for_peer,1024);
	string file_to_send=buffer_for_peer;
	// vector<string> query=process_query(p);
	send(peer_number , hello , strlen(hello) , 0);

		
	int number_of_pieces,n;
	int a=read(peer_number,&n,sizeof(n));
	if(a==0)
	{
		cout<<"Following is the error message \n";
		perror(strerror(errno));
		
	}
	send(peer_number , hello , strlen(hello) , 0);
	cout<<"pieces read"<<number_of_pieces<<" \n";
	number_of_pieces=ntohl(n);
	int index;
	read(peer_number,&n,sizeof(n));
	index=ntohl(n);
	send(peer_number , hello , strlen(hello) , 0);
	// string file_name=query[0];
	// int pieces=stoi(query[1]);
	// int index_to_start_from=stoi(query[2]);
	// string message=buffer_for_peer;


	cout<<"file to send to peer "<<file_to_send<<"\n";
	cout<<"index numebr  "<<index<<"\n";
	cout<<"number_of_pieces "<<number_of_pieces<<"\n";


	long long int bytes_before=index*number_of_pieces*524288;
	long long int end_add=bytes_before+ number_of_pieces*524288;


	ifstream fr;
	// f.open(dest_path);
	fr.open(file_to_send);
	fr.seekg (bytes_before,fr.beg);


	ifstream fend;
	fend.open(file_to_send);
	fend.seekg(0,fend.end);

	// size of buffer is 16kb
	number_of_pieces=number_of_pieces*32;
	int length=16384;
	int np=1;
	char * buffer_to_read = new char [length];
	int l;
	for(int i=bytes_before;i<=end_add;i=i+16384)
	{
		if(np==number_of_pieces)
		{
		l=fend.tellg()-fr.tellg();
		if(l<16384)
		{
			buffer_to_read = new char [l+1];
			length=l;
			end_add=fend.tellg();
			fr.read(buffer_to_read,length);
			buffer_to_read[l]='\0';
			length=length+1;
		}
			



		}
		// if(fr.peek()!=EOF)
		// {
			fr.read(buffer_to_read,length);
			// buffer_to_read[l]='\0';
			// length=length+1;
		send(peer_number,buffer_to_read,strlen(buffer_to_read),0);
		memset(buffer_to_read, 0, sizeof(buffer_to_read));
		read(peer_number,buffer_for_peer,1);
		np++;
		// }
		// else
		// 	break;
		  // for sync
	}

	//sample

	// int initial_l=fr.tellg();

	// ifstream fsample;
		
	// fsample.open(file_to_send);
	// fsample.seekg (bytes_before,fr.end);
	// int final_l=fsample.tellg();

	// // int end_add=final_l-initial_l;


	// ifstream fend;
	// fend.open(file_to_send);
	// fend.seekg(0,fend.end);

	// // if(fend.tellg()-)
	// int l;
	// for(int i=bytes_before;i<end_add;)
	// {
	// 	char * buffer_to_read;
	// 	if(fend.tellg()-fr.tellg()>16384)
	// 	{
	// 		buffer_to_read = new char [16384];
	// 		i=i+16384;
	// 	}
	// 	else
	// 	{
	// 		l=fend.tellg()-fr.tellg();
	// 		char * buffer_to_read = new char [l];
	// 		i=i+l;

	// 	}

	// 	fr.read(buffer_to_read,sizeof(buffer_to_read));
	// 	send(peer_number,buffer_to_read,strlen(buffer_to_read),0);
	// 	memset(buffer_to_read, 0, sizeof(buffer_to_read));
	// 	read(peer_number,buffer_for_peer,1);
	// 	initial_l=fr.tellg();
	// 	l=final_l-initial_l;
		


	// 	delete[] buffer_to_read;
	// }





	//start+sending file with offset calculated from index i and number of pieces and index are starting from 0






	return NULL;
}



void check_requests_as_server()
{

	// cout<<"inside check_requests_as_server\n";
	while(true)
	{

		cout<<"Waiting for connections\n";
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
			(socklen_t*)&addrlen))<0) 
		{ 
			perror("accept"); 
			exit(EXIT_FAILURE); 
		} 
		else
		{
			number_of_clients++;
		}
		cout<<"Connection established at "<<ntohs(address.sin_port)<<"\n";
		int *p_client=(int *)malloc(sizeof(int));
		*p_client=new_socket;
		pthread_t t1;
		pthread_create(&t1,NULL,communicate_as_server,p_client);
		// pthread_join(t1,NULL);






	}
}




void make_server_code()
{

	// client_as_server_port=8081;
	if(client_as_server_port==0)
	{
		cout<<"invalid port\n";
		return;
	}
	cout<<"inside make_server_code\n";
    // Creating socket file descriptor 
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 

    // Forcefully attaching socket to the port 8080 
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
		&opt, sizeof(opt))) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( client_as_server_port ); 

    // Forcefully attaching socket to the port 8080 
	if (bind(server_fd, (struct sockaddr *)&address, 
		sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	if (listen(server_fd, 3) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 


	// check_requests_as_server();


}
void* open_server(void *args)
{

	// cout<<"inside open server\n";
	make_server_code();
	check_requests_as_server();

	return NULL;

}

vector<string> process_query(string s)
{
	int length=s.length();
	int i;
	vector<string> pro;
	string temp;
	for(i=0;i<=length;i++)
	{
		// cout<<s[i];

		if(s[i]==' ' || i==length)
		{
			// cout<<temp<<"\n";
			pro.push_back(temp);
			temp="";
		}

		else
			temp=temp+s[i];
	}
	return pro;
}

void *connect_to_peers(void *download_details)
{
	cout<<"peer want file\n";
	int sock = 0, valread; 
	struct sockaddr_in serv_addr; 
	char *hello = "connection for downloading file"; 
	file_download_details f1=*((struct file_download_details*)download_details);

	// free(download_details);
	int serv=f1.peer_to_connect;
	char buffer[1024] = {0}; 
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return NULL; 
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(serv); 
	
	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return NULL; 
	} 

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return NULL; 
	} 

	cout<<"file name"<<f1.file_d_name<<"\n";
	cout<<"number_of_pieces"<<f1.number_of_pieces<<"\n";
	cout<<"index to start  from"<<f1.index<<"\n";
	string f_name=f1.file_d_name;
	hello=&f_name[0];
	send(sock , hello , strlen(hello) , 0 );
	read(sock , buffer, 1024);   // for sync 
	int n;
	int f_pieces=f1.number_of_pieces;

	n=htonl(f_pieces);
	send(sock,&n,sizeof(n),0);
	read(sock , buffer, 1024);  //for sync

	int f_index=f1.index;
	n=htonl(f_index);
	send(sock,&n,sizeof(n),0);
	read(sock , buffer, 1024);  // for sync
	cout<<"download file from  peer number "<<serv<<"\n";

	string dest_path=f1.destination_path;

	cout<<"destination path "<<dest_path<<"\n";


	// ifstream fr;
	// fr.open(f_name);


	//proper running
	// long long int bytes_before=f_index*f_pieces*524288;
	// ofstream fw;
	// fw.open(dest_path);
	// fw.seekp(bytes_before,fw.beg);
	// long long int end_add=bytes_before+ f_pieces*524288;

	// int length=16384;
	// char * buffer_to_write = new char [length];

	// for(int i=bytes_before;i<=end_add;i=i+16384)
	// {
	// 	read(sock,buffer_to_write,16384); 
	// 	cout<<buffer_to_write<<"\n"; 
	// 	fw.write(buffer_to_write,length);
	// 	memset(buffer_to_write, 0, sizeof(buffer_to_write));
	// 	send(sock , hello , strlen(hello) , 0 );
		
		
	// }

	//  fw.close();
	//  close(sock);


	//sample

long long int bytes_before=f_index*f_pieces*524288;
	ofstream fw;
	fw.open(dest_path);
	fw.seekp(bytes_before,fw.beg);
	long long int end_add=bytes_before+ f_pieces*524288;

	int length=16384;
	char * buffer_to_write = new char [length];

	while(read(sock,buffer_to_write,16384))
	{
		// read(sock,buffer_to_write,16384); 
		cout<<buffer_to_write<<"\n"; 
		string s=buffer_to_write;
		fw.write(&s[0],strlen(&s[0]));
		memset(buffer_to_write, 0, sizeof(buffer_to_write));
		send(sock , hello , strlen(hello) , 0 );
		
		
	}

	 fw.close();
	 close(sock);


	//recieve


	







}






void* connect_client(void* server_to_connect)
{
	int serv=*((int*)server_to_connect);
	// free(server_to_connect);
	int sock = 0, valread; 
	struct sockaddr_in serv_addr; 
	char *hello = "Hello from client1"; 
	char buffer[1024] = {0}; 
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return NULL; 
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(serv); 
	
	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return NULL; 
	} 

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return NULL; 
	} 
	// cout<<"client con succ\n";
	// cout<<"port number "<<serv_addr.sin_port<<"\n";
	struct sockaddr_in local_address;
	// socklen_t a=sizeof(sockaddr_in);
	socklen_t *addr_size=(socklen_t *)malloc(sizeof(socklen_t));
	*addr_size = sizeof(local_address);
	int gs=getsockname(sock,(struct sockaddr *)&local_address, addr_size);
	// cout<<"return value of getsockname "<<gs<<"\n";
	string s;
	client_as_server_port=local_address.sin_port;
	start_server_thread();
	cout<<"port number of client as server "<<local_address.sin_port<<"\n";
	// cout<<"port number of server "<<serv_addr.sin_port<<"\n";
	// string s;
	hello=&(to_string(client_as_server_port))[0];
	// cout<<"my port is"<<hello;
	send(sock , hello , strlen(hello) , 0 );
	read( sock , buffer, 1024);

	while(true)
	{


		// cout<<"inside client while loop \n";
		// cin>>s;
		getline(cin,s);

		vector<string> query=process_query(s);
		hello=&s[0];
		// cout<<"send message is "<<hello<<"\n";
		send(sock , hello , strlen(hello) , 0 );
		if(query[0]=="download_file")
		{
			// recieve_file();
			// cout<<"client inside download\n";
			vector<uint32_t> file_peer_details;
			

			int number_of_peers,n;
			read(sock, &n, sizeof(n));
			number_of_peers = ntohl(n);
			
			cout<<number_of_peers<<"\n";
			// int number_of_peers=stoi(np);


			int file_pieces;
			read(sock, &file_pieces, sizeof(file_pieces));
			file_pieces = ntohl(file_pieces);


			vector<string> file_piece_sha;
			string d="demo for sync";
			for(int i=0;i<file_pieces;i++)
			{
				read(sock,buffer,1024);
				string p=buffer;
				file_piece_sha.push_back(p);
				//for sync.
				send(sock,&d,strlen(&d[0]),0);
			}



			cout<<"All sha recieved succesfully with n= "<<file_piece_sha.size()<<"\n";


			int peer_port;
			for(int i=0;i<number_of_peers;i++)
			{
				read(sock, &n, sizeof(n));
				peer_port = ntohl(n);
				file_peer_details.push_back(peer_port);

			}
			

			//read number of file pieces
			

			int pieces_per_peer;
			pieces_per_peer=file_pieces/number_of_peers;
			// if(file_pieces%number_of_peers==0)
			// {
			// 	pieces_per_peer=file_pieces/number_of_peers;
			// }



			//read sha of file to download from tracker
			
			//connect to all peers


			ofstream fd1;
			fd1.open(query[2]);
			for(int i=0;i<file_peer_details.size();i++)
			{
				// int *f1=(int *)malloc(sizeof(file_dd));
				// f1.fd=query[1];
				// f1->number_of_pieces=;
				// f1->index=i;

				file_download_details *f1=(file_download_details*)malloc(sizeof(file_download_details));
				f1->file_d_name=query[1];
				f1->total_number_of_pieces=file_pieces;
				f1->index=i;
				// f1->new_file=fd1;
				f1->destination_path=query[2];
				f1->sha_per_piece=file_piece_sha;
				if(file_pieces%number_of_peers!=0 && i==file_peer_details.size()-1 )
				{
					f1->number_of_pieces=pieces_per_peer+1;
				}
				else
				{
					f1->number_of_pieces=pieces_per_peer;
				}

				f1->peer_to_connect=file_peer_details[i];


				// int *connect_peer=(int *)malloc(sizeof(uint32_t));
				// *connect_peer=file_peer_details[i];
				cout<<"Connecting to peer "<<file_peer_details[i]<<"\n";
				pthread_t t1;
				pthread_create(&t1,NULL,connect_to_peers,f1);
				// pthread_join(t1,NULL);
				// close()
				
			}





			// while(read( sock , buffer, 1024)!=0)
			// {
			// 	cout<<"client isnide while also\n";
			// 	string p=buffer;
			// 	cout<<p<<" ";
			// 	file_peer_details.push_back(p);
			// }


			for(int i=0;i<file_peer_details.size();i++)
			{
				cout<<file_peer_details[i]<<" ";
			}
			cout<<"\n";
			
		}
		else if(query[0]=="upload_file")
		{
			// cout<<"waiting for server message\n";
			
			
			char *c;
			cout<<"uploadding start of sha\n";
			vector<string> sha_of_chunks;
				// sha_of_chunks[0]="overall file sha";


			read_file_in_chunks(query[1],sha_of_chunks);
  				// cout<<sha_of_chunks.size();
			int n=sha_of_chunks.size();

			cout<<"numbr of piece "<<n<<"\n";
			n=htonl(n);
			send(sock,&n,sizeof(n),0);



  				// complete code for sending sha piecewise
  				for(int i=0;i<sha_of_chunks.size();i++)
  				{


  					hello=&sha_of_chunks[i][0];
  					// cout<<"sending from client"<<i+1<<"\n ";
// <<" "<<hello
  					send(sock,hello,strlen(hello),0);
  					read(sock , buffer, 1024);
  					// string p=buffer;
  					// cout<<"reciveing from server "<<p<<"\n";
  				}
  				// sha_of_chunks.clear();
			cout<<"Sha sent succesffuly for "<<query[1]<<"\n";

		}
		else
		{
			cout<<s<<"\n";
		}
		// buffer[1024]={0};
		// hello=NULL; 
		// printf("Hello message sent\n"); 
		
		// valread = read( sock , buffer, 1024); 
		// printf("%s\n",buffer ); 
		
		memset(buffer, 0, sizeof(buffer));
	}
	

	// return NULL;



	return NULL;
}

void start_server_thread()
{

	// cout<<"inside server thread\n";
	
	pthread_create(&client_as_server,NULL,open_server,NULL);
	// pthread_join(client_as_server,NULL);

}



void start_client_thread(int to_connect_server)
{	
	int *server_to_connect=(int *)malloc(sizeof(int));
	*server_to_connect=to_connect_server;
	// free(to_connect_server);
	
	pthread_create(&client_as_client,NULL,connect_client,server_to_connect);
	// pthread_join(client_as_client,NULL);

}
int main(int argc, char const *argv[]) 
{ 
	cout<<"connect "<<client_as_server_port<<"\n";


	// if(argc==1)
	// {
	// 	start_server_thread();
	// }


	// if(argc>1)
	// {
	int server_port;
		// =(int*)malloc(sizeof(int));

	server_port=atoi(argv[1]);
	start_client_thread(server_port);

	pthread_join(client_as_client,NULL);
	pthread_join(client_as_server,NULL);
	// }
	
	return 0; 
} 
