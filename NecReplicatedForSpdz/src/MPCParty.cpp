#include "../include/MPCParty.hpp"
#include <netinet/tcp.h>
#include <string.h>
#include <signal.h>

using namespace std;

// set options for socket: set larger size and no delay
// this is a static function
void MPCParty::initSocket(int s)
{
 cout << "[initSocket] initialize socket" << endl;
 int opt = 1;
 if (setsockopt(s, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(int))) {
   cout << "ERROR SOCKET" << endl;
   throw (-1);
 }

}

MPCParty::MPCParty(int partyID, int grpID, const char *partiesConfFile){
  grpID_ = grpID;
  partyID_ = partyID;
  int SOCKET_TIMEOUT_SEC = 1;  //currently not adjustable
  int SOCKET_TIMEOUT_USEC = 0; //currently not adjutable
  FILE* fp;
  if ((fp = fopen(partiesConfFile,"r")) == NULL) {
    cout<<"File " << partiesConfFile <<" doesn't exist"<<endl;
  }
  cout<<"[main] file open"<<endl;
  
  char buf[100];
  int serverPorts[3];
  string serverAddr[3];
  for (int i=0; i<3; i++) {
    fgets(buf, sizeof(buf), fp);
    serverPorts[i] = atoi(buf);
    fgets(buf, sizeof(buf), fp);
    serverAddr[i]  = buf;
  }
  fclose(fp);
//  rightPort_ = serverPorts[(partyID_+2)%3];
//  int leftClientID = partyID_ % 3;

  int leftClientID = (partyID_ + 1) % 3;
  int rightClientID = (leftClientID + 1) % 3;
  leftPort_     = serverPorts[leftClientID];  
  leftPartyAddress_ = serverAddr[leftClientID]; 
  rightPort_ = serverPorts[partyID_];

  if (grpID_ > 0) {
	rightPort_ += grpID_;
        leftPort_ += grpID_;
  }

  cout<<"rightPort="    <<rightPort_<<endl;
  cout<<"leftPort="    <<leftPort_<<endl;
  cout<<"leftPartyAddress="<<leftPartyAddress_<<endl;

  //Create sockets
  socketForListen_ = socket(AF_INET, SOCK_STREAM, 0);
  if (socketForListen_<0) {
    cout<<"[CreateSocket] server socket failed"<<endl;
    perror("CreateSocket");
  }
  else {
    cout<<"[CreateSocket] Create Server Socket"<<endl;
  }

  // TODO adi add call to setsockopt
  initSocket(socketForListen_);


  leftSocket_ = socket(AF_INET, SOCK_STREAM, 0);
  if (leftSocket_<0) {
    cout<<"[CreateSocket] client socket failed"<<endl;
    perror("CreateSocket");
  }
  else {
    cout<<"[CreateSocket] Create Client Socket"<<endl;
  }

  // TODO adi add call to setsockopt
  initSocket(leftSocket_);


  
  //Get Server and Client Information
  sockaddr_in rightParty;
  rightParty.sin_family      = AF_INET;
  rightParty.sin_port        = htons(rightPort_);
  rightParty.sin_addr.s_addr = INADDR_ANY; //all IP address can be connected

  sockaddr_in leftParty;
  leftParty.sin_family      = AF_INET;
  leftParty.sin_port        = htons(leftPort_);
  leftParty.sin_addr.s_addr = inet_addr(leftPartyAddress_.c_str());
  
  struct timeval timeout;
  
  //Change socket options for the server
  int yes=1;
  setsockopt(socketForListen_,SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes));
  
  //bind address to server socket
  int statBind=bind(socketForListen_, (struct sockaddr *)&rightParty, sizeof(rightParty));
  if (statBind!=0) {
    cout<<"[Init] Bind Failed:"<<statBind<<endl;
    perror("Bind");
  }
  
  int statListen=listen(socketForListen_,5);  //second argument is the maximum number of client waiting connection
  if(statListen!=0) {
    cout<<"[Init] Listen Failed:"<<statListen<<endl;
    perror("Listen");
  }
  
  int ConResult=-1;
  int AccResult=-1;
  fd_set readfds; //file descriptor set
  
  //establish connections
  while ((ConResult<0) || (AccResult<=0)) {
    if (ConResult<0) {
      ConResult=connect(leftSocket_,(struct sockaddr *)&leftParty, sizeof(leftParty)); //try to connect to server which receives data from this server
    }
    if (AccResult<=0) {
      FD_ZERO(&readfds);            //reset file descriptor set
      FD_SET(socketForListen_,&readfds); //add server socket to file descriptor set
      timeout.tv_sec  = SOCKET_TIMEOUT_SEC;   //2
      timeout.tv_usec = SOCKET_TIMEOUT_USEC;  //0
      AccResult=select(socketForListen_+1, &readfds, NULL, NULL, &timeout); 
      //Note: when the socket in readfds receive the request of connection, select function return the number of request. 
      if (AccResult>0) {
	uint32_t len=sizeof(rightParty);
	rightSocket_ = accept(socketForListen_, (struct sockaddr *)&rightParty, &len); //wait connection from the server which sends data to this server
	AccResult = 1;
      }

      //use TCP_NODELAY on the socket
     
      //int flag = 1;
      //int result = setsockopt(rightSocket_,            /* socket affected */
      //			      IPPROTO_TCP,     /* set option at TCP level */
      //			      TCP_NODELAY,     /* name of option */
      //			      (char *) &flag,  /* the cast is historical */
      //			      sizeof(int));    /* length of option value */
      //if (result < 0) {
      //cout << "error setting NODELAY. exiting" << endl;
      //	exit (-1);
      //}
      
    }
  }
}

MPCParty::~MPCParty() {
  close(socketForListen_);
  close(rightSocket_);
  close(leftSocket_);
}

int MPCParty::Write(uint32_t* data, int buffer_size, int topartyID){
  int nWrite = 0;
  int IDdiff = (topartyID-partyID_+3) % 3;

  if (IDdiff == 1) {
    nWrite = write(leftSocket_,data,buffer_size*sizeof(uint32_t));
  }
  else if (IDdiff == 2) {
    nWrite = write(rightSocket_,data,buffer_size*sizeof(uint32_t));
  }
  else if(nWrite<=0){
    cout<<"[Write] Write Failed:"<<nWrite<<endl;
    perror("Write");
  }
  return nWrite;
}


int MPCParty::Write(uint64_t* data, int buffer_size, int topartyID){
	signal(SIGPIPE, SIG_IGN);

//  cout << "[MPCParty.cpp::Write] buffer_size = " << buffer_size << endl;
  int nWrite = 0;
  int IDdiff = (topartyID-partyID_+3) % 3;

  if (IDdiff == 1) {
//	cout << "[MPCParty.cpp::Write] branch 1 = " << nWrite << endl;
//	cout << "[MPCParty.cpp::Write] buffer_size*sizeof(uint64_t) = " << buffer_size*sizeof(uint64_t) << endl;
	nWrite = write(leftSocket_,data,buffer_size*sizeof(uint64_t));
//    if (nWrite < 0) {
//  	  switch(errno) {
//  	  case EPIPE:
//  		  cout << "[MPCParty::Write] broken pipe" << endl;
//  		  abort();
//  	  case EFBIG:
//  		  cout << "[MPCParty::Write] buffer size is too big" << endl;
//  		  abort();
//  	  }
//    }
//    cout << "[MPCParty.cpp::Write] nWrite = " << nWrite << endl;
  }
  else if (IDdiff == 2) {
//	cout << "[MPCParty.cpp::Write] branch 2 = " << nWrite << endl;
    nWrite = write(rightSocket_,data,buffer_size*sizeof(uint64_t));
//    if (nWrite == -1) {
//  	  switch(errno) {
//  	  case EPIPE:
//  		  cout << "[MPCParty::Write] broken pipe" << endl;
//  		  abort();
//  	  }
//    }
//    cout << "[MPCParty.cpp::Write] nWrite = " << nWrite << endl;
  }
  else if(nWrite<=0){
    cout<<"[Write] Write Failed:"<<nWrite<<endl;
    perror("Write");
  }
  return nWrite;
}




int MPCParty::Read(uint32_t* data, int buffer_size, int frompartyID){
  int IDdiff = (frompartyID - partyID_+3) % 3;
  int loaded = 0;
  int nRead  = 0;

  if (IDdiff == 1) {
    int frame = buffer_size*sizeof(uint32_t);
    while (frame>0) {
      nRead   = read(leftSocket_,(char*)data+loaded,frame);
      frame  -= nRead;
      loaded += nRead;
    }
  }
  else if (IDdiff == 2) {
    int frame = buffer_size*sizeof(uint32_t);
    while (frame>0) {
      nRead   = read(rightSocket_,(char*)data+loaded,frame);
      frame  -= nRead;
      loaded += nRead;
    }
  }
  else if (IDdiff == 0) {
    cout<<"sending to the party itself"<<endl;
  }

  if(loaded<=0){
    cout<<"[Read] cannot receive packet"<<endl;
    perror("Read");
    return 0;
  }
  return loaded; 
}

int MPCParty::Read(uint64_t* data, int buffer_size, int frompartyID){
  int IDdiff = (frompartyID - partyID_+3) % 3;
  int loaded = 0;
  int nRead  = 0;

//  cout << "[MPCParty.cpp::Read] buffer_size = " << buffer_size << endl;
  if (IDdiff == 1) {
    int frame = buffer_size*sizeof(uint64_t);
//    cout << "[MPCParty.cpp::Read] frame = " << frame << endl;
    while (frame>0) {
//    	cout << "[MPCParty::Read] test 1:"<<frame << endl;
      nRead   = read(leftSocket_,(char*)data+loaded,frame);
      frame  -= nRead;
      loaded += nRead;
//      cout << "[MPCParty::Read] test 1:"<<frame << endl;
    }
  }
  else if (IDdiff == 2) {
    int frame = buffer_size*sizeof(uint64_t);
//    cout << "[MPCParty.cpp::Read] frame = " << frame << endl;
    while (frame>0) {
//    	cout << "[MPCParty::Read] test 2:"<<frame << endl;
      nRead   = read(rightSocket_,(char*)data+loaded,frame);
      frame  -= nRead;
      loaded += nRead;
//      cout << "[MPCParty::Read] test 2:"<<frame << endl;
    }
  }
  else if (IDdiff == 0) {
    cout<<"sending to the party itself"<<endl;
  }

  if(loaded<=0){
    cout<<"[Read] cannot receive packet"<<endl;
    perror("Read");
    return 0;
  }

  return loaded;
}



int MPCParty::Write(int buffer_size, uint32_t *data, int topartyID){
  int nWrite = 0;
  int IDdiff = (topartyID-partyID_+3) % 3;

  if (IDdiff == 1) {
    nWrite = write(leftSocket_,data,buffer_size*sizeof(uint32_t));
  }
  else if (IDdiff == 2) {
    nWrite = write(rightSocket_,data,buffer_size*sizeof(uint32_t));
  }
  else if(nWrite<=0){
    cout<<"[Write] Write Failed:"<<nWrite<<endl;
    perror("Write");
  }
  return nWrite;
}




int MPCParty::Read(int buffer_size, uint32_t *data, int frompartyID){
  int IDdiff = (frompartyID - partyID_+3) % 3;
  int loaded = 0;
  int nRead  = 0;

  if (IDdiff == 1) {
    int frame = buffer_size*sizeof(uint32_t);
    while (frame>0) {
      nRead   = read(leftSocket_,(char*)data+loaded,frame);
      frame  -= nRead;
      loaded += nRead;
    }
  }
  else if (IDdiff == 2) {
    int frame = buffer_size*sizeof(uint32_t);
    while (frame>0) {
      nRead   = read(rightSocket_,(char*)data+loaded,frame);
      frame  -= nRead;
      loaded += nRead;
    }
  }
  else if (IDdiff == 0) {
    cout<<"sending to the party itself"<<endl;
  }

  if(loaded<=0){
    cout<<"[Read] cannot receive packet"<<endl;
    perror("Read");
    return 0;
  }
  return loaded; 
}


int MPCParty::Write(uint16_t* data, int buffer_size, int topartyID){
  int nWrite = 0;
  int IDdiff = (topartyID-partyID_+3) % 3;

  if (IDdiff == 1) {
    nWrite = write(leftSocket_,data,buffer_size*sizeof(uint16_t));
  }
  else if (IDdiff == 2) {
    nWrite = write(rightSocket_,data,buffer_size*sizeof(uint16_t));
  }
  else if(nWrite<=0){
    cout<<"[Write] Write Failed:"<<nWrite<<endl;
    perror("Write");
  }
  return nWrite;
}

int MPCParty::Read(uint16_t* data, int buffer_size, int frompartyID){
  int IDdiff = (frompartyID - partyID_+3) % 3;
  int loaded = 0;
  int nRead  = 0;

  if (IDdiff == 1) {
    int frame = buffer_size*sizeof(uint16_t);
    while (frame>0) {
      nRead   = read(leftSocket_,(char*)data+loaded,frame);
      frame  -= nRead;
      loaded += nRead;
    }
  }
  else if (IDdiff == 2) {
    int frame = buffer_size*sizeof(uint16_t);
    while (frame>0) {
      nRead   = read(rightSocket_,(char*)data+loaded,frame);
      frame  -= nRead;
      loaded += nRead;
    }
  }
  else if (IDdiff == 0) {
    cout<<"sending to the party itself"<<endl;
  }

  if(loaded<=0){
    cout<<"[Read] cannot receive packet"<<endl;
    perror("Read");
    return 0;
  }
  return loaded;
}


int MPCParty::Write(uint8_t* data, int buffer_size, int topartyID){
  int nWrite = 0;
  int IDdiff = (topartyID-partyID_+3) % 3;

  if (IDdiff == 1) {
    nWrite = write(leftSocket_,data,buffer_size*sizeof(uint8_t));
  }
  else if (IDdiff == 2) {
    nWrite = write(rightSocket_,data,buffer_size*sizeof(uint8_t));
  }
  else if(nWrite<=0){
    cout<<"[Write] Write Failed:"<<nWrite<<endl;
    perror("Write");
  }
  return nWrite;
}


int MPCParty::Read(uint8_t* data, int buffer_size, int frompartyID){
  int IDdiff = (frompartyID - partyID_+3) % 3;
  int loaded = 0;
  int nRead  = 0;

  if (IDdiff == 1) {
    int frame = buffer_size*sizeof(uint8_t);
    while (frame>0) {
      nRead   = read(leftSocket_,(char*)data+loaded,frame);
      frame  -= nRead;
      loaded += nRead;
    }
  }
  else if (IDdiff == 2) {
    int frame = buffer_size*sizeof(uint8_t);
    while (frame>0) {
      nRead   = read(rightSocket_,(char*)data+loaded,frame);
      frame  -= nRead;
      loaded += nRead;
    }
  }
  else if (IDdiff == 0) {
    cout<<"sending to the party itself"<<endl;
  }

  if(loaded<=0){
    cout<<"[Read] cannot receive packet"<<endl;
    perror("Read");
    return 0;
  }

  return loaded;
}

int MPCParty::getServerID(void) {
  return partyID_;
}
