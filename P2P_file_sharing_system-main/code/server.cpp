// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include<pthread.h>
#include<bits/stdc++.h>
#include<limits.h>
#include<sys/stat.h>

#define PORT 8081
using namespace std;

int number_of_clients=0;

int chunk_size=512;


int server_fd, new_socket, valread;

struct sockaddr_in address; 
int opt = 1; 
int addrlen = sizeof(address); 

char buffer[1024] = {0}; 

char *hello = "Hello from server"; 
string s="message recieved";


struct clients_connected
{

	// int port_at_client_connected_to_tracker;
	int port_at_client_run_as_server;
	int pthread_id_of_client;
};


map<int,int> client_details;
vector<clients_connected> clients_connected_to_tracker;


//storig file details in map with key as file name and value as cleints_port_at_which_it_is_running_as_server
map<string,vector<int>> file_details_peers;

map<string,vector<string>> file_sha_details;

map<string,int> number_of_file_pieces;




//returns true if connection to client socket is not present else false
// bool is_connection_off(int client_socket)
// {



// } 
// void check_requests_as_server();

// void handle_query(string inp)
// {


// }


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


bool check_valid_file_name(string path)
{
	struct stat check_file;
	if (stat(&path[0], &check_file) != 0)
		return false;
	else
		return true;

}

void* communicate(void *p_client)
{
    // cout<<"in func\n";
	int current_client_number;
	current_client_number=number_of_clients;
	int client=*((int *)p_client);
	free(p_client);
	cout<<"client number "<<number_of_clients<<"\n";
	read(client,buffer,1024);
	string client_port_details=buffer;
	cout<<"port "<<client_port_details<<"\n";
	memset(buffer, 0, sizeof(buffer));
	hello="from server--client details recieved";
	send(client , hello , strlen(hello) , 0 );

	clients_connected c1;
	c1.port_at_client_run_as_server=stoi(client_port_details);
	c1.pthread_id_of_client=client;
	clients_connected_to_tracker.push_back(c1);

	client_details[stoi(client_port_details)]=client;



	// client etails using vector
	// cout<<"all clients details ";
	// int i;
	// for(i=0;i<clients_connected_to_tracker.size();i++)
	// {
	// 	cout<<clients_connected_to_tracker[i].port_at_client_run_as_server<<" ";
	// 	cout<<clients_connected_to_tracker[i].pthread_id_of_client<<" ";
	// 	cout<<"\n";
	// }



	//cleint details using map
	cout<<"all client details\n";
	map<int,int>::iterator itr;
	for(itr=client_details.begin();itr!=client_details.end();itr++)
	{
		cout<<itr->first<<" "<<itr->second<<"\n";
	}

	while(1)
	{
		// cout<<"inside while loop\n";

		int a=read( client , buffer, 1024);
		// check if client is connected or not
		if(a==0)
		{
			cout<<"Client "<< current_client_number<<" disconnected\n";
			number_of_clients--;

			// delete client details from cleint_details map
			client_details.erase(stoi(client_port_details));
			break;
		} 
    // printf("%s",buffer);
		// cout<<"Above buffer\n";
		string inp=buffer;
		// printf("%s\n",inp);
		// cout<<inp<<"\n";
		vector<string> query=process_query(inp);
		
		

		if(query[0]=="download_file")
		{
			char path_to_file[PATH_MAX];
			realpath(&query[1][0],path_to_file);
			cout<<path_to_file<<"\n";
			string file_path=path_to_file;
    	// cout<<path_to_file<<"\n";
			if(check_valid_file_name(file_path))
			{



				int number_of_peers=(file_details_peers[query[1]].size());
				number_of_peers=htonl(number_of_peers);
				// hello=&number_of_peers[0];
				cout<<"peers number "<<number_of_peers<<"\n";
				send(client , &number_of_peers , sizeof(number_of_peers),0);

				//send number of pieces of files
				int p=number_of_file_pieces[query[1]];
				p=htonl(p);
				send(client,&p,sizeof(p),0);

				
				//send piecewise sha of file
				vector<string> sha_to_send=file_sha_details[query[1]];
				char *sha_send;
				for(int i=0;i<sha_to_send.size();i++)
				{
					sha_send=&(sha_to_send[i])[0];
					send(client,sha_send,strlen(sha_send),0);
					

					//for sync.
					read(client,buffer,1024);
				}




					//printing values of map which stores file details
				// cout<<"key "<<query[1]<<"\n";
				// cout<<"client ports\n";
				// for(int i=0;i<file_details_peers[query[1]].size();i++)
				// {
				// 	cout<<file_details_peers[query[1]][i]<<"\n";
				// }
				// cout<<"size of map "<<file_details_peers[query[1]].size()<<"\n";
				
				

				// send file to client
				// cout<<"value "<<file_details_peers[query[1]]<<"\n";
				for(int i=0;i<file_details_peers[query[1]].size();i++)
				{
					// hello=&(file_details_peers[query[1]][i])[0];
					// cout<<"sending "<<file_details_peers[query[1]][i]<<"\n";
					// send(client , hello , strlen(hello) , 0 );

					number_of_peers=file_details_peers[query[1]][i];
					number_of_peers=htonl(number_of_peers);

					send(client , &number_of_peers , sizeof(number_of_peers),0);

					

				}

				
				

				// cout<<"valid name \n";
			}
			else
			{
				// cout<<"invalid file name\n";


			}

    	// cout<<check_valid_file_name(file_path);

    	// transfer_file();




			// query.clear();
		}
		else if(query[0]=="upload_file")
		{
			if(file_details_peers.find(query[1])==file_details_peers.end())
			{
				vector<int> pd;
				pd.push_back(stoi(client_port_details));
				file_details_peers.insert({query[1],pd});

			}
			else
			{
				file_details_peers[query[1]].push_back(stoi(client_port_details));
			}


			vector<string> chunkwise_sha;
			cout<<"Filename is "<<query[1]<<"\n";
			// char buffer[]
			// chunkwise_sha[0]="overall sha of file";
			int number_of_pieces=0;
			int n;
			read(client, &n, sizeof(n));
			number_of_pieces = ntohl(n);
			number_of_file_pieces.insert({query[1],number_of_pieces});
			
			cout<<"number of pieces= "<<number_of_pieces<<"\n";
			cout<<"server recieving sha\n";
				// char sha_read[1024];



				//complete code for recieveing sha peicewise
			for(int i=0;i<number_of_pieces;i++)
			{
				cout<<"i value "<<i+1<<" ";
				int a=read( client , buffer, 32);
				if(a==0)
				{
					cout<<"below is error message for read\n";
					perror(strerror(errno));
				}

				string p=buffer;
				cout<<p<<"\n";
					// hello=&(to_string(i))[0];
				send(client , hello , strlen(hello) , 0);
				chunkwise_sha.push_back(p);
				memset(buffer, 0, sizeof(buffer));

			}
			file_sha_details.insert({query[1],chunkwise_sha});
			// 	// chunkwise_sha.clear();


			cout<<"sha of file recieved\n";
			cout<<"size of pieces"<<file_sha_details[query[1]].size()<<"\n";
			
			// file_details_peers.insert({[query[1]],push_back(client_port_details)});
		}
		else
		{
			cout<<inp;
			cout<<"  this message if from "<<current_client_number<<"\n";
		}
		memset(buffer, 0, sizeof(buffer));

    // continue;

    // send(client , hello , strlen(hello) , 0 );
    // hello=&s[0];
    // printf("message recieved from client number ");
    // cout<<current_client_number<<"\n";
    // buffer[1024] = {0}; 
	}

	return NULL;
}


void start_server_code()
{




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
	address.sin_port = htons( PORT ); 

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



void check_requests_as_server()
{
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
		cout<<"pthread_t value "<<t1<<"\n";
		pthread_create(&t1,NULL,communicate,p_client);






	}
}

int main(int argc, char const *argv[]) 
{ 




	start_server_code();
	cout<<"Server port "<<address.sin_port <<"\n";


	check_requests_as_server();


	return 0; 
} 
