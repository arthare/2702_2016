#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#include <iostream>
#include <string.h>
#include <stdio.h>
#include <cstdlib>
#include <unistd.h>

using namespace std;

struct data
{
    int x;
    int y;
};


int main()
{
    data dataToSend;

    //Structure for address of server

   struct sockaddr_in myaddr;
   int sock;
   int result;

   //Construct the server sockaddr_ structure
   memset(&myaddr, 0, sizeof(myaddr));
   myaddr.sin_family=AF_INET;

   myaddr.sin_addr.s_addr=htonl(INADDR_ANY);


   //myaddr.sin_port=htons(9045);
   myaddr.sin_port=htons(0);

   //Create the socket
   if((sock=socket(AF_INET, SOCK_DGRAM, 0))<0) {
      cout <<"Failed to create socket"<<endl;
      exit(EXIT_FAILURE);
   }

   if(bind(sock,( struct sockaddr *) &myaddr, sizeof(myaddr))<0) {
      cout <<"bind failed"<<endl;
      exit(EXIT_FAILURE);
   }
    cout << "bind sucess"<<endl;


   inet_pton(AF_INET,"10.27.2.21",&myaddr.sin_addr.s_addr);

   myaddr.sin_port=htons(2702);


   //char data[10] = {1,2,3,4,5,6,7,8,9,0};

    while(true){
        cin >> dataToSend.x;
        cin >> dataToSend.y;
        result = sendto(sock, &dataToSend, sizeof(dataToSend), 0, (const sockaddr*) & myaddr, sizeof(myaddr));
        cout << "sent : " << result << " bytes of data" << endl;
    }


    return 0;
}
