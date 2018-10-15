#include <NecProtocolPartyRingFor3P.h>
#include <stdlib.h>
#include "../include/PRG.hpp"
#include <chrono>

using namespace std::chrono;

int main(int argc, char* argv[]) {

  /*
   * Usage: 
   *  ./main [partyID] [number of opens]
   */
  
  int partyID = atoi(argv[1]);
  int numOfOpens = atoi(argv[2]);

  cout << "[test.main] invoke ctr of NecProtocolPartyFor3P" << endl;

  int batch_size=8;

  NecProtocolPartyRingFor3P<uint64_t> protocol(partyID, numOfOpens, batch_size);

  cout << "[test.main] invoke init() " << endl;
  
  protocol.init();

  cout << "[test.main] invoke offline()" << endl;
  
  protocol.offline();

  //triples();

  cout << "[test.main] invoke shareInput()" << endl;

  protocol.shareInput();

  cout << "[test.main] invoke open()" << endl;
  
  protocol.open();

  cout << "[test.main] invoke test()" << endl;

  protocol.test();
  
}
