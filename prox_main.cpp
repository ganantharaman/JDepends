#include "realsock.h"
#include <stdlib.h>

int accept_callback(int peer_fd);
int process_request(SOCKET peer_fd, char *buf, unsigned long len);

int main(int argc , char ** argv)
{
  // Initialize sockets(Winsock)
  CRealSocket::init();
  // Create a HTTP listener on port
  // 8080
  CRealSocket * srv_socket =
     CRealSocket::create_srv_socket(NULL, 8080, accept_callback);

  // accept client connections
  if(srv_socket) {
    for(; ;)
      srv_socket->accept();
  }

  return 1;
}
// data buffer
char buffer[8192];

// return 1 if you wanna close connection
int accept_callback(int peer_fd)
{
  unsigned long total_read = 
    CRealSocket::read(peer_fd, buffer, 0UL, sizeof(buffer), 1000);

  if( total_read < (sizeof(buffer) - 1))
    buffer[total_read + 1] = 0;
  else
	buffer[sizeof(buffer) -1 ] = 0;

  int val = 1;
  setsockopt(peer_fd, 
	  SOL_SOCKET, SO_KEEPALIVE, (char *)&val, sizeof(int));

  return process_request(peer_fd, buffer, total_read);
}

int process_request(SOCKET peer_fd, char *buf, unsigned long tot_read_len)
{
  cout << buf << endl;
  unsigned port = 80;
  char * host_name = NULL;

  if (buf[0] == 'G' &&
	  buf[1] == 'E' &&
	  buf[2] == 'T') {
    char * p = strchr(buf, ' ') + 1;
	char * q = strchr(p, ' ');
	int len = q - p + 1;
	if(len > 2) {
	char * full_name = new char[len + 1];
	memcpy(full_name, p, len);
	full_name[len-1] = 0;

	// skip http://
	p = strchr(full_name, '/');
	// search for embedded ports
	q = strchr(p + 2, ':');
	if(q && isdigit(*(q+1))){
	  // assuming port name cannot be more than
	  // 64 bytes!
      char tmp[64];
	  len = strchr(q + 1, '/') - q;
	  memcpy(tmp, q + 1, len);
  	  tmp[len] = 0;
	  port = atoi(tmp);
	} 
    // copy host name
	q = strchr(p+2, '/');
	len = q - p -1;
    host_name = new char[len + 1];
	memcpy(host_name, p+2, len);
	host_name[len-1] = 0;
    // clean_up if port info is
	// also embedded
	// host_name must be 
	// www.cs.columbia.edu
	// not www.cs.columbia.edu:6162
	// this will cause sock error 10049
	q = strchr(host_name, ':');
	if(q) 
	  *q = 0;
    delete [] full_name;
    cout << "attempting to connect to ..." << endl;
	cout <<(host_name ? host_name : "local") << ":" << port << endl;
    // try connecting to host , port
	CRealSocket * socket = 
		CRealSocket::create_client_socket(host_name, port);
   
	if(socket){ // successful ?
		// send out original
    	CRealSocket::write(socket->fd(), 
			buffer, 0UL, tot_read_len, 0);

      for (; ; ) {
        unsigned read_len = CRealSocket::read(socket->fd(), 
		     buffer, 0UL, sizeof(buffer), 2000);
        if( read_len == 0)
          break;
        if( read_len < (sizeof(buffer) - 1))
           buffer[read_len + 1] = 0;
         else
	       buffer[sizeof(buffer) -1 ] = 0;
         cout << buffer << endl;
    	   CRealSocket::write(peer_fd, 
		   	   buffer, 0UL, read_len, 0);
      }
	}

	}
	if(host_name) delete [] host_name;
  }
  return 1;
}
