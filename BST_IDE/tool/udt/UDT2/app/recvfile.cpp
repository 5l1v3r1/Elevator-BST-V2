#ifndef WIN32
#include <arpa/inet.h>
#endif
#include <iostream>
#include <cstdlib>
#include <udt.h>

using namespace std;

int main(int argc, char* argv[])
{
   if ((argc != 5) || (0 == atoi(argv[2])))
   {
      cout << "usage: recvfile server_ip server_port remote_filename local_filename" << endl;
      return 0;
   }

   UDTSOCKET fhandle = UDT::socket(AF_INET, SOCK_STREAM, 0);

   sockaddr_in serv_addr;
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_port = htons(short(atoi(argv[2])));
#ifndef WIN32
   if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0)
#else
   if (INADDR_NONE == (serv_addr.sin_addr.s_addr = inet_addr(argv[1])))
#endif
   {
      cout << "incorrect network address.\n";
      return 0;
   }
   memset(&(serv_addr.sin_zero), '\0', 8);

   if (UDT::ERROR == UDT::connect(fhandle, (sockaddr*)&serv_addr, sizeof(serv_addr)))
   {
      cout << "connect: " << UDT::getlasterror().getErrorMessage() << endl;
      return 0;
   }


   // send name information of the requested file
   int len = strlen(argv[3]);

   if (UDT::ERROR == UDT::send(fhandle, (char*)&len, sizeof(int), 0))
   {
      cout << "send: " << UDT::getlasterror().getErrorMessage() << endl;
      return 0;
   }

   if (UDT::ERROR == UDT::send(fhandle, argv[3], len, 0))
   {
      cout << "send: " << UDT::getlasterror().getErrorMessage() << endl;
      return 0;
   }


   // get size information
   __int64 size;

   if (UDT::ERROR == UDT::recv(fhandle, (char*)&size, sizeof(__int64), 0))
   {
      cout << "send: " << UDT::getlasterror().getErrorMessage() << endl;
      return 0;
   }


   // receive the file
   ofstream ofs(argv[4], ios::out | ios::binary | ios::trunc);
   __int64 recvsize; 

   if (UDT::ERROR == (recvsize = UDT::recvfile(fhandle, ofs, 0, size)))
   {
      cout << "recvfile: " << UDT::getlasterror().getErrorMessage() << endl;
      return 0;
   }


   UDT::close(fhandle);

   return 1;
}