#ifndef NEC_PROTOCOLPARTY_RING_FOR_3P_H_
#define NEC_PROTOCOLPARTY_RING_FOR_3P_H_

#include <stdlib.h>
#include "MPCParty.hpp"
#include "PRG.hpp"
#include "Z2nIntReplicated.h"
#include "Z2nShareReplicated.h"
#include <thread>
#include <iostream>
#include <vector>
#include <assert.h>
#include <math.h>

#define flag_print false
#define flag_print_timings true

#define N 3

using namespace std;

template <typename T>
class NecProtocolPartyRingFor3P {
 private:
  /*
   * N - number of parties
   * T - number of malicious
   * M - number of gates
   */

  byte key1_prg [16] = {0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
			0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf};
  byte key2_prg [16] = {0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
			0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf};
  
  byte key1_cr [16] = {0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
		       0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf};
  byte key2_cr [16] = {0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
		       0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf};
  byte key3_cr [16] = {0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
		       0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef};
  byte *keys [3] = {key1_cr, key2_cr, key3_cr};

  byte key_input [16] = {0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
			 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xff};
  
  int m_partyId;
  MPCParty *mpcParty;
  int numOfMults = 1000; // should be given from spdz
  int m_nextParty, m_prevParty;
  int m_batchSize;

  Z2nIntReplicated<T> input;

  //Z2nIntReplicated<T> share1;
  //Z2nIntReplicated<T> share2;
  //Z2nIntReplicated<T> share3;

  Z2nShareReplicated<T> ownShare;

  Z2nIntReplicated<T> triples_a;
  Z2nIntReplicated<T> triples_b;
  Z2nIntReplicated<T> triples_c;

  Z2nIntReplicated<T> output;

  PRG *prg_input;

  PRG *prg_i;
  PRG *prg_iMinus1;
  
	PRG *prg_genshare1;
	PRG *prg_genshare2;

 public:
  
  NecProtocolPartyRingFor3P(int partyID, int numOfOpens, int batch_size);
  ~NecProtocolPartyRingFor3P();

  void init(); // establish communications
  
  void offline(); // generate input

  void triples(); // generate triples (tentatively do semi-honest multiplication)

  void shareInput(); // share input value (communicate with other parties)

  void open(); //open share (communicate with other parties)

  void test(); // only for debugging
  
  void generateRandomness(int numOfOpens,
			  Z2nIntReplicated<T> &secrets);

  void triples(int numOfTriples,
	       Z2nShareReplicated<T> &triples_x,
	       Z2nShareReplicated<T> &triples_y,
	       Z2nShareReplicated<T> &triples_z);
  
  void shareInput(int numOfShares,
		  Z2nIntReplicated<T> &secrets,
		  Z2nShareReplicated<T> &myShare);
  
  bool shareInput(int input_party_id,
		  	  	  	  int num_input,
		  	  	  	  T *input_values,
					  T *output_values);

  void shareOneInput(int numOfShares,
		  	  T *secrets,
			  T *share1,
			  T *share2,
			  T *share3);

  void getInputFromOneParty(int partyID, T *value);

  void openShares(int numOfShares,
	    Z2nShareReplicated<T> &shares,
	    Z2nIntReplicated<T> &secrets);

  bool openShares(int numOfShares,
		 vector<T> &shares,
		 vector<T> &secrets);

  void addS(int numOfShares,
		  Z2nIntReplicated<T> &inShare1,
		  Z2nIntReplicated<T> &inShare2,
		  Z2nIntReplicated<T> &outShare);

  void subS(int numOfShares,
		  Z2nIntReplicated<T> &inShare1,
		  Z2nIntReplicated<T> &inShare2,
		  Z2nIntReplicated<T> &outShare);

  bool SkewDec(int numOfBits,
		  Z2nShareReplicated<T> &inShare,
		  Z2nShareReplicated<T> &outShares);

  bool SkewDec(int numOfBits,
  		  Z2nShareReplicated<T> &inShare,
  		  Z2nShareReplicated<T> &outShares1,
  		  Z2nShareReplicated<T> &outShares2,
  		  Z2nShareReplicated<T> &outShares3);

  bool SkewDec(int numOfBits,
  		  vector<T> &inShare,
		  vector<T> &outShares);

  bool PostRec(int ring_size,
  		  Z2nShareReplicated<T> &inShares,
  		  Z2nShareReplicated<T> &outShare);

  bool SkewInj(
  		  Z2nShareReplicated<T> &inShare,
  		  Z2nShareReplicated<T> &outShares);

  bool SkewRec(
  		  Z2nShareReplicated<T> &inShare,
  		  Z2nShareReplicated<T> &outShares);

  bool mulM(int numOfShares,
		  Z2nShareReplicated<T> &inShare,
		  T coef,
		  Z2nShareReplicated<T> &outShare);

  bool mulSI(int numOfShares,
		  Z2nShareReplicated<T> &inShare,
		  T coef,
		  Z2nShareReplicated<T> &outShare);

  bool addM(int numOfShares,
		  Z2nShareReplicated<T> &inShare,
		  T coef,
		  Z2nShareReplicated<T> &outShare);

  bool addSI(int numOfShares,
		  Z2nShareReplicated<T> &inShare,
		  T coef,
		  Z2nShareReplicated<T> &outShare);

  bool subML(int numOfShares,
		  Z2nShareReplicated<T> &inShare,
		  T coef,
		  Z2nShareReplicated<T> &outShare);

  bool subMR(int numOfShares,
		  T coef,
		  Z2nShareReplicated<T> &inShare,
		  Z2nShareReplicated<T> &outShare);

//  bool multShares(int numOfShares,
//		  Z2nShareReplicated<T> &x_shares,
//		  Z2nShareReplicated<T> &y_shares,
//		  Z2nShareReplicated<T> &xy_shares);

    bool multShares(int numOfShares,
  		  vector<T> &x_shares,
  		  vector<T> &y_shares,
  		  vector<T> &xy_shares);

  bool load_share_immediate(Z2nIntReplicated<T> & values,
		  	  	  	  	  	Z2nShareReplicated<T> &shares);

  void end();
  
};


template <typename T>
NecProtocolPartyRingFor3P<T>::NecProtocolPartyRingFor3P(int partyID, int numOfOpens, int batch_size)
 : numOfMults(numOfOpens), m_partyId(partyID), m_batchSize(batch_size)
{
  byte *key_i = keys[(partyID+1) % 3];
  byte *key_iMinus1 = keys[partyID % 3];

  prg_i = new PRG(key_i);
  prg_iMinus1 = new PRG(key_iMinus1);

  prg_input = new PRG(key_input);

  prg_genshare1 = new PRG(key1_prg);
  prg_genshare2 = new PRG(key2_prg);

  m_nextParty = (m_partyId +1) % 3;
  m_prevParty = (m_nextParty + 1) % 3;
  mpcParty = NULL;
}

template <typename T>
NecProtocolPartyRingFor3P<T>::~NecProtocolPartyRingFor3P() {
  delete prg_i;
  delete prg_iMinus1;
  delete prg_input;
//  delete prg_genshare1;
//  delete prg_genshare2;
  delete mpcParty;
}


template <typename T>
void NecProtocolPartyRingFor3P<T>::init() {
	  mpcParty = new MPCParty(m_partyId, 0, "partiesMPCLocal.conf");
//	  mpcParty = new MPCParty(m_partyId, 0, "partiesMPC.conf");

  input.init(numOfMults);

  ownShare.init(numOfMults);
  output.init(numOfMults);

  triples_a.init(numOfMults);
  triples_b.init(numOfMults);
  triples_c.init(numOfMults);
}

template <typename T>
void NecProtocolPartyRingFor3P<T>::offline() {
  generateRandomness(numOfMults, input);
}

template <typename T>
void NecProtocolPartyRingFor3P<T>::generateRandomness(int numOfOpens,
					  Z2nIntReplicated<T> &secrets) {
  // generate random values by same seed
  for (int i=0; i<numOfOpens; i++) {
    secrets.elem[i] = prg_input->getRandomLong();
  }
}

template <typename T>
void NecProtocolPartyRingFor3P<T>::triples() {
	for (int i=0; i<numOfMults; i++) {
		triples_a[i] = 0;
		triples_b[i] = 0;
		triples_c[i] = 0;
	}
	//triples(numOfMults, triples_a, triples_b, triples_c);
}

template <typename T>
void NecProtocolPartyRingFor3P<T>::triples(int numOfTriples,
			       Z2nShareReplicated<T> &share1,
			       Z2nShareReplicated<T> &share2,
			       Z2nShareReplicated<T> &share3)
{
  
#pragma ivdep
	for (int i=0; i<numOfTriples; i++) {
		// generate random shares [a], [b] and [c]
		share1.elem1[i] = prg_i->getRandomLong();
		share1.elem2[i] = prg_iMinus1->getRandomLong();
		share2.elem1[i] = prg_i->getRandomLong();
		share2.elem2[i] = prg_iMinus1->getRandomLong();

		share1.elem1[i] += share1.elem2[i];
		share2.elem1[i] += share2.elem2[i];

		// partial computation for [c] = [a] * [b]
		share3.elem2[i] = share1.elem1[i]*share2.elem1[i] - share1.elem2[i]*share2.elem2[i];
	}


	//ring communication
	mpcParty->Write(share3.elem2, numOfTriples, m_nextParty);
	mpcParty->Read(share3.elem1, numOfTriples, m_prevParty);

	// remain of the computation for [c] = [a]*[b]
	for (int i=0; i<numOfTriples; i++) {
		share3.elem1[i] += share3.elem2[i];
	}

}

template <typename T>
void NecProtocolPartyRingFor3P<T>::shareInput() {
  shareInput(numOfMults, input, ownShare);
}

template <typename T>
void NecProtocolPartyRingFor3P<T>::shareOneInput(int numOfShares,
				  T *secret,
				  T *share1,
				  T *share2,
				  T *share3)
{
  for (int i=0; i<numOfShares; i++) {
	  share1[i+m_batchSize] = prg_genshare1->getRandomLong();
	  share2[i+m_batchSize] = prg_genshare2->getRandomLong();
	  share3[i+m_batchSize] = secret[i] - share1[i+m_batchSize] - share2[i+m_batchSize];
	  share1[i] = share3[i+m_batchSize] + share1[i+m_batchSize];
	  share2[i] = share1[i+m_batchSize] + share2[i+m_batchSize];
	  share3[i] = share2[i+m_batchSize] + share3[i+m_batchSize];
  }
}

template <typename T>
void NecProtocolPartyRingFor3P<T>::shareInput(int numOfShares,
				  Z2nIntReplicated<T> &secret,
				  Z2nShareReplicated<T> &myShare)
{
  if (secret.getNumOfData() == 0) {
    cout << "[NecProtocolPartyRingFor3P::shareInput] source size is 0" << endl;
    exit(1);
  }

  Z2nShareReplicated<T> share1(secret.getNumOfData());
  Z2nShareReplicated<T> share2(secret.getNumOfData());
  Z2nShareReplicated<T> share3(secret.getNumOfData());

#pragma ivdep
  for (int i=0; i<numOfShares; i++) {
    share1.elem2[i] = prg_genshare1->getRandomLong();
    share2.elem2[i] = prg_genshare2->getRandomLong();
    share3.elem2[i] = secret.elem[i] - share1.elem2[i] - share2.elem2[i];

    share1.elem1[i] = share3.elem2[i] + share1.elem2[i];
    share2.elem1[i] = share1.elem2[i] + share2.elem2[i];
  }

  if      (m_partyId == 0) { myShare = share1; }
  else if (m_partyId == 1) { myShare = share2; }
  else if (m_partyId == 2) { myShare = share3; }

  if (flag_print) {
    cout << "ownShare : " << endl;
    ownShare.dump();
  }
  
}

template <typename T>
bool NecProtocolPartyRingFor3P<T>::shareInput(int input_party_id,
					  int num_input,
		  	  	  	  T *input_values,
					  T *output_values)
{
	T* shares[3];
	shares[0] = new T[2*num_input*m_batchSize];
	shares[1] = new T[2*num_input*m_batchSize];
	shares[2] = new T[2*num_input*m_batchSize];

	if (m_partyId == input_party_id) {
		for (int i=0; i<num_input; i++) {
			shareOneInput(m_batchSize, &input_values[i*m_batchSize],
							&shares[0][2*i*m_batchSize], &shares[1][2*i*m_batchSize], &shares[2][2*i*m_batchSize]);
		}

		mpcParty->Write(shares[m_nextParty], 2*num_input*m_batchSize, m_nextParty);
		mpcParty->Write(shares[m_prevParty], 2*num_input*m_batchSize, m_prevParty);
		memcpy(output_values, shares[m_partyId], 2*num_input*m_batchSize*sizeof(T));
	}
	else {
		mpcParty->Read(output_values, 2*num_input*m_batchSize, input_party_id);
	}

	return true;

}

template <typename T>
void NecProtocolPartyRingFor3P<T>::open() {
  openShares(numOfMults, ownShare, output);
}

template <typename T>
void NecProtocolPartyRingFor3P<T>::openShares(int numOfShares,
			    Z2nShareReplicated<T> &shares,
			    Z2nIntReplicated<T> &ret)
{
  int size = shares.getNumOfShares();

  T *recBuf;
  recBuf = new T[size];

  mpcParty->Write(shares.elem1, size, m_nextParty);
  mpcParty->Read(recBuf, size, m_prevParty);

   for (int i=0; i<size; i++) {
    ret.elem[i] = shares.elem2[i] + recBuf[i];
   }
}

template <typename T>
bool NecProtocolPartyRingFor3P<T>::openShares(int numOfShares,
													vector<T> &shares,
													vector<T> &secrets) {

	assert(2*numOfShares*m_batchSize == shares.size());

	int size_opens = numOfShares*m_batchSize;

	T sendBuf[size_opens];
	T recBuf[size_opens];

	for (int i=0; i<numOfShares; i++) {
		for (int j=0; j<m_batchSize; j++) {
			sendBuf[i*m_batchSize+j] = shares[2*i*m_batchSize+j];
		}
	}

	mpcParty->Write(sendBuf, size_opens, m_nextParty);
	mpcParty->Read(recBuf, size_opens, m_prevParty);

	secrets.resize(size_opens);

	for (int i=0; i<numOfShares; i++) {
		for (int j=0; j<m_batchSize; j++) {
			secrets[i*m_batchSize+j] = shares[2*i*m_batchSize + j + m_batchSize] + recBuf[i*m_batchSize+j];
		}
	}
}

template <typename T>
void NecProtocolPartyRingFor3P<T>::end()
{}

template <typename T>
void NecProtocolPartyRingFor3P<T>::test()
{
  for (int i=0; i<numOfMults; i++) {
    if (input.elem[i] != output.elem[i])
      {
	cout << "[NecProtocolPartyRingFor3P] test() failed: doesn't match" << endl;
	exit(1);
      }
  }
  if (flag_print) {
    cout << "output: " << endl;
    output.dump();
  }
  cout << "[NecProtocolPartyRingFor3P] all shares are successfully reconstructed. " << endl;
}

template <typename T>
void NecProtocolPartyRingFor3P<T>::addS(int numOfShares,
		Z2nIntReplicated<T> &inShare1,
		Z2nIntReplicated<T> &inShare2,
		Z2nIntReplicated<T> &outShare)
{
	for (int i=0; i<numOfShares; i++) {
		outShare.elem[i] = inShare1.elem[i] + inShare2.elem[i];
	}
}


template <typename T>
void NecProtocolPartyRingFor3P<T>::subS(int numOfShares,
		Z2nIntReplicated<T> &inShare1,
		Z2nIntReplicated<T> &inShare2,
		Z2nIntReplicated<T> &outShare)
{
	for (int i=0; i<numOfShares; i++) {
		outShare.elem[i] = inShare1.elem[i] - inShare2.elem[i];
	}
}


template <typename T>
bool NecProtocolPartyRingFor3P<T>::SkewDec(int numOfBits,
		  Z2nShareReplicated<T> &inShare,
		  Z2nShareReplicated<T> &outShares)
{
	for(int i = 0; i < numOfBits; i++)
	{
		if (m_partyId == 0)
		{
			// [x_1,i]^B
			outShares.elem1[3 * i] = 0;
			outShares.elem2[3 * i] = 0;
			// [x_2,i]^B
			outShares.elem1[3 * i + 1] = ((inShare.elem1[0] - inShare.elem2[0]) >> i) & 1;
			outShares.elem2[3 * i + 1] = 0;
			// [x_3,i]^B
			outShares.elem1[3 * i + 2] = 0;
			outShares.elem2[3 * i + 2] = (inShare.elem2[0] >> i) & 1;
		}
		else if (m_partyId == 1)
		{
			// [x_1,i]^B
			outShares.elem1[3 * i] = 0;
			outShares.elem2[3 * i] = (inShare.elem2[0] >> i) & 1;
			// [x_2,i]^B
			outShares.elem1[3 * i + 1] = 0;
			outShares.elem2[3 * i + 1] = 0;
			// [x_3,i]^B
			outShares.elem1[3 * i + 2] = ((inShare.elem1[0] - inShare.elem2[0]) >> i) & 1;
			outShares.elem2[3 * i + 2] = 0;
		}
		else if (m_partyId == 2)
		{
			// [x_1,i]^B
			outShares.elem1[3 * i] = ((inShare.elem1[0] - inShare.elem2[0]) >> i) & 1;
			outShares.elem2[3 * i] = 0;
			// [x_2,i]^B
			outShares.elem1[3 * i + 1] = 0;
			outShares.elem2[3 * i + 1] = (inShare.elem2[0] >> i) & 1;
			// [x_3,i]^B
			outShares.elem1[3 * i + 2] = 0;
			outShares.elem2[3 * i + 2] = 0;
		}

		outShares.elem1[3 * i] = outShares.elem1[3 * i] ^ outShares.elem2[3 * i];
		outShares.elem1[3 * i + 1] = outShares.elem1[3 * i + 1] ^ outShares.elem2[3 * i + 1];
		outShares.elem1[3 * i + 2] = outShares.elem1[3 * i + 2] ^ outShares.elem2[3 * i + 2];
	}
	return true;
}

template <typename T>
bool NecProtocolPartyRingFor3P<T>::SkewDec(int numOfBits,
		  Z2nShareReplicated<T> &inShare,
		  Z2nShareReplicated<T> &outShares1,
		  Z2nShareReplicated<T> &outShares2,
		  Z2nShareReplicated<T> &outShares3) {

	size_t bs = static_cast<int>(ceil((float)m_batchSize / (8*sizeof(T))));

	for (int i=0; i<m_batchSize; i++) {
		for (int j=0; j<numOfBits; j++) {
//			size_t idx_b = static_cast<int>(ceil((float)i / (8*sizeof(T))));
			int idx_b = i/64;
//			cout << "[NecProtocolPartyRingFor3P.h] idx_b = " << idx_b << endl;
			T tmp1 = ((inShare.elem1[i] - inShare.elem2[i]) >> j) & 0x1;
			T tmp2 = (inShare.elem2[i] >> j) & 0x1;

//			cout << "[NecProtocolPartyRingFor3P.h] tmp1 = "<< tmp1 << endl;
//			cout << "[NecProtocolPartyRingFor3P.h] tmp2 = "<< tmp2 << endl;

			if (m_partyId == 0) {
				outShares1.elem1[bs*j + idx_b] ^= 0;
				outShares1.elem2[bs*j + idx_b] ^= 0;
				outShares2.elem1[bs*j + idx_b] ^= (tmp1 << (i % (8*sizeof(T))));
				outShares2.elem2[bs*j + idx_b] ^= 0;
				outShares3.elem1[bs*j + idx_b] ^= 0;
				outShares3.elem2[bs*j + idx_b] ^= (tmp2 << (i % (8*sizeof(T))));

//				cout << "[NecProtocolPartyRingFor3P.h] outShares1.elem1[" << bs*j + idx_b << "] = " << outShares1.elem1[bs*j + idx_b] << endl;
//				cout << "[NecProtocolPartyRingFor3P.h] outShares1.elem2[" << bs*j + idx_b << "] = " << outShares1.elem2[bs*j + idx_b] << endl;
//				cout << "[NecProtocolPartyRingFor3P.h] outShares2.elem1[" << bs*j + idx_b << "] = " << outShares2.elem1[bs*j + idx_b] << endl;
//				cout << "[NecProtocolPartyRingFor3P.h] outShares2.elem2[" << bs*j + idx_b << "] = " << outShares2.elem2[bs*j + idx_b] << endl;
//				cout << "[NecProtocolPartyRingFor3P.h] outShares3.elem1[" << bs*j + idx_b << "] = " << outShares3.elem1[bs*j + idx_b] << endl;
//				cout << "[NecProtocolPartyRingFor3P.h] outShares3.elem2[" << bs*j + idx_b << "] = " << outShares3.elem2[bs*j + idx_b] << endl;
			}
			else if (m_partyId == 1) {
				outShares1.elem1[bs*j + idx_b] ^= 0;
				outShares1.elem2[bs*j + idx_b] ^= (tmp2 << (i % (8*sizeof(T))));
				outShares2.elem1[bs*j + idx_b] ^= 0;
				outShares2.elem2[bs*j + idx_b] ^= 0;
				outShares3.elem1[bs*j + idx_b] ^= (tmp1 << (i % (8*sizeof(T))));
				outShares3.elem2[bs*j + idx_b] ^= 0;

//				cout << "[NecProtocolPartyRingFor3P.h] outShares1.elem1[" << bs*j + idx_b << "] = " << outShares1.elem1[bs*j + idx_b] << endl;
//				cout << "[NecProtocolPartyRingFor3P.h] outShares1.elem2[" << bs*j + idx_b << "] = " << outShares1.elem2[bs*j + idx_b] << endl;
//				cout << "[NecProtocolPartyRingFor3P.h] outShares2.elem1[" << bs*j + idx_b << "] = " << outShares2.elem1[bs*j + idx_b] << endl;
//				cout << "[NecProtocolPartyRingFor3P.h] outShares2.elem2[" << bs*j + idx_b << "] = " << outShares2.elem2[bs*j + idx_b] << endl;
//				cout << "[NecProtocolPartyRingFor3P.h] outShares3.elem1[" << bs*j + idx_b << "] = " << outShares3.elem1[bs*j + idx_b] << endl;
//				cout << "[NecProtocolPartyRingFor3P.h] outShares3.elem2[" << bs*j + idx_b << "] = " << outShares3.elem2[bs*j + idx_b] << endl;
			}
			else if (m_partyId == 2) {
				outShares1.elem1[bs*j + idx_b] ^= (tmp1 << (i % (8*sizeof(T))));
				outShares1.elem2[bs*j + idx_b] ^= 0;
				outShares2.elem1[bs*j + idx_b] ^= 0;
				outShares2.elem2[bs*j + idx_b] ^= (tmp2 << (i % (8*sizeof(T))));
				outShares3.elem1[bs*j + idx_b] ^= 0;
				outShares3.elem2[bs*j + idx_b] ^= 0;

//				cout << "[NecProtocolPartyRingFor3P.h] outShares1.elem1[" << bs*j + idx_b << "] = " << outShares1.elem1[bs*j + idx_b] << endl;
//				cout << "[NecProtocolPartyRingFor3P.h] outShares1.elem2[" << bs*j + idx_b << "] = " << outShares1.elem2[bs*j + idx_b] << endl;
//				cout << "[NecProtocolPartyRingFor3P.h] outShares2.elem1[" << bs*j + idx_b << "] = " << outShares2.elem1[bs*j + idx_b] << endl;
//				cout << "[NecProtocolPartyRingFor3P.h] outShares2.elem2[" << bs*j + idx_b << "] = " << outShares2.elem2[bs*j + idx_b] << endl;
//				cout << "[NecProtocolPartyRingFor3P.h] outShares3.elem1[" << bs*j + idx_b << "] = " << outShares3.elem1[bs*j + idx_b] << endl;
//				cout << "[NecProtocolPartyRingFor3P.h] outShares3.elem2[" << bs*j + idx_b << "] = " << outShares3.elem2[bs*j + idx_b] << endl;
			}
		}
	}

	for (int i=0; i<numOfBits*bs; i++) {
		outShares1.elem1[i] ^= outShares1.elem2[i];
		outShares2.elem1[i] ^= outShares2.elem2[i];
		outShares3.elem1[i] ^= outShares3.elem2[i];
	}

	return true;
}

template <typename T>
bool NecProtocolPartyRingFor3P<T>::SkewDec(int numOfBits,
		  vector<T> &inShare,
		  vector<T> &outShares)
{
	size_t batch_size_bits = static_cast<int>(ceil((float)m_batchSize / (8*sizeof(T))));
	for(size_t i =0; i<m_batchSize; i++)
	{
		size_t batch_size_bits_idx = static_cast<int>(ceil((float)i / (8*sizeof(T))));
		for(int j =0; j<numOfBits; j++)
		{
			if (m_partyId == 0)
			{
				// [x_1,i]^B
				outShares[(3*2*batch_size_bits*j)+batch_size_bits_idx] ^= 0 << (i%(8*sizeof(T)));
				outShares[(3*2*batch_size_bits*j)+batch_size_bits+batch_size_bits_idx] ^= 0 << (i%(8*sizeof(T)));
				cout << "[NecProtocolPartyRingFor3P.h] " << j << "-th x1 outShares[" << (3*2*batch_size_bits*j)+batch_size_bits_idx << "] = " << outShares[(3*2*batch_size_bits*j)+batch_size_bits_idx] << endl;
				cout << "[NecProtocolPartyRingFor3P.h] " << j << "-th x1 outShares[" << (3*2*batch_size_bits*j)+batch_size_bits+batch_size_bits_idx << "] = " << outShares[(3*2*batch_size_bits*j)+batch_size_bits+batch_size_bits_idx] << endl;
				// [x_2,i]^B
				outShares[(3*2*batch_size_bits*j)+2*batch_size_bits+batch_size_bits_idx] ^= (((inShare[i]-inShare[m_batchSize+i]) >> j) & 1) << (i%(8*sizeof(T)));
				outShares[(3*2*batch_size_bits*j)+2*batch_size_bits+batch_size_bits+batch_size_bits_idx] ^= 0 << (i%(8*sizeof(T)));
				cout << "[NecProtocolPartyRingFor3P.h] " << j << "-th x2 outShares[" << (3*2*batch_size_bits*j)+2*batch_size_bits+batch_size_bits_idx << "] = " << outShares[(3*2*batch_size_bits*j)+2*batch_size_bits+batch_size_bits_idx] << endl;
				cout << "[NecProtocolPartyRingFor3P.h] " << j << "-th x2 outShares[" << (3*2*batch_size_bits*j)+2*batch_size_bits+batch_size_bits+batch_size_bits_idx << "] = " << outShares[(3*2*batch_size_bits*j)+2*batch_size_bits+batch_size_bits+batch_size_bits_idx] << endl;
				// [x_3,i]^B
				outShares[(3*2*batch_size_bits*j)+4*batch_size_bits+batch_size_bits_idx] ^= 0 << (i%(8*sizeof(T)));
				outShares[(3*2*batch_size_bits*j)+4*batch_size_bits+batch_size_bits+batch_size_bits_idx] ^= ((inShare[m_batchSize+i] >> j) & 1) << (i%(8*sizeof(T)));
				cout << "[NecProtocolPartyRingFor3P.h] " << j << "-th x3 outShares[" << (3*2*batch_size_bits*j)+4*batch_size_bits+batch_size_bits_idx << "] = " << outShares[(3*2*batch_size_bits*j)+4*batch_size_bits+batch_size_bits_idx] << endl;
				cout << "[NecProtocolPartyRingFor3P.h] " << j << "-th x3 outShares[" << (3*2*batch_size_bits*j)+4*batch_size_bits+batch_size_bits+batch_size_bits_idx << "] = " << outShares[(3*2*batch_size_bits*j)+4*batch_size_bits+batch_size_bits+batch_size_bits_idx] << endl;
			}
			else if (m_partyId == 1)
			{
				// [x_1,i]^B
				outShares[(3*2*batch_size_bits*j)+batch_size_bits_idx] ^= 0 << (i%(8*sizeof(T)));
				outShares[(3*2*batch_size_bits*j)+batch_size_bits+batch_size_bits_idx] ^= ((inShare[m_batchSize+i] >> j) & 1) << (i%(8*sizeof(T)));
				cout << "[NecProtocolPartyRingFor3P.h] " << j << "-th x1 outShares[" << (3*2*batch_size_bits*j)+batch_size_bits_idx << "] = " << outShares[(3*2*batch_size_bits*j)+batch_size_bits_idx] << endl;
				cout << "[NecProtocolPartyRingFor3P.h] " << j << "-th x1 outShares[" << (3*2*batch_size_bits*j)+batch_size_bits+batch_size_bits_idx << "] = " << outShares[(3*2*batch_size_bits*j)+batch_size_bits+batch_size_bits_idx] << endl;
				// [x_2,i]^B
				outShares[(3*2*batch_size_bits*j)+2*batch_size_bits+batch_size_bits_idx] ^= 0 << (i%(8*sizeof(T)));
				outShares[(3*2*batch_size_bits*j)+2*batch_size_bits+batch_size_bits+batch_size_bits_idx] ^= 0 << (i%(8*sizeof(T)));
				cout << "[NecProtocolPartyRingFor3P.h] " << j << "-th x2 outShares[" << (3*2*batch_size_bits*j)+2*batch_size_bits+batch_size_bits_idx << "] = " << outShares[(3*2*batch_size_bits*j)+2*batch_size_bits+batch_size_bits_idx] << endl;
				cout << "[NecProtocolPartyRingFor3P.h] " << j << "-th x2 outShares[" << (3*2*batch_size_bits*j)+2*batch_size_bits+batch_size_bits+batch_size_bits_idx << "] = " << outShares[(3*2*batch_size_bits*j)+2*batch_size_bits+batch_size_bits+batch_size_bits_idx] << endl;
				// [x_3,i]^B
				outShares[(3*2*batch_size_bits*j)+4*batch_size_bits+batch_size_bits_idx] ^= (((inShare[i]-inShare[m_batchSize+i]) >> j) & 1) << (i%(8*sizeof(T)));
				outShares[(3*2*batch_size_bits*j)+4*batch_size_bits+batch_size_bits+batch_size_bits_idx] ^= 0 << (i%(8*sizeof(T)));
				cout << "[NecProtocolPartyRingFor3P.h] " << j << "-th x3 outShares[" << (3*2*batch_size_bits*j)+4*batch_size_bits+batch_size_bits_idx << "] = " << outShares[(3*2*batch_size_bits*j)+4*batch_size_bits+batch_size_bits_idx] << endl;
				cout << "[NecProtocolPartyRingFor3P.h] " << j << "-th x3 outShares[" << (3*2*batch_size_bits*j)+4*batch_size_bits+batch_size_bits+batch_size_bits_idx << "] = " << outShares[(3*2*batch_size_bits*j)+4*batch_size_bits+batch_size_bits+batch_size_bits_idx] << endl;
			}
			else if (m_partyId == 2)
			{
				// [x_1,i]^B
				outShares[(3*2*batch_size_bits*j)+batch_size_bits_idx] ^= (((inShare[i]-inShare[m_batchSize+i]) >> j) & 1) << (i%(8*sizeof(T)));
				outShares[(3*2*batch_size_bits*j)+batch_size_bits+batch_size_bits_idx] ^= 0 << (i%(8*sizeof(T)));
				cout << "[NecProtocolPartyRingFor3P.h] " << j << "-th x1 outShares[" << (3*2*batch_size_bits*j)+batch_size_bits_idx << "] = " << outShares[(3*2*batch_size_bits*j)+batch_size_bits_idx] << endl;
				cout << "[NecProtocolPartyRingFor3P.h] " << j << "-th x1 outShares[" << (3*2*batch_size_bits*j)+batch_size_bits+batch_size_bits_idx << "] = " << outShares[(3*2*batch_size_bits*j)+batch_size_bits+batch_size_bits_idx] << endl;
				// [x_2,i]^B
				outShares[(3*2*batch_size_bits*j)+2*batch_size_bits+batch_size_bits_idx] ^= 0 << (i%(8*sizeof(T)));
				outShares[(3*2*batch_size_bits*j)+2*batch_size_bits+batch_size_bits+batch_size_bits_idx] ^= ((inShare[m_batchSize+i] >> j) & 1) << (i%(8*sizeof(T)));
				cout << "[NecProtocolPartyRingFor3P.h] " << j << "-th x2 outShares[" << (3*2*batch_size_bits*j)+2*batch_size_bits+batch_size_bits_idx << "] = " << outShares[(3*2*batch_size_bits*j)+2*batch_size_bits+batch_size_bits_idx] << endl;
				cout << "[NecProtocolPartyRingFor3P.h] " << j << "-th x2 outShares[" << (3*2*batch_size_bits*j)+2*batch_size_bits+batch_size_bits+batch_size_bits_idx << "] = " << outShares[(3*2*batch_size_bits*j)+2*batch_size_bits+batch_size_bits+batch_size_bits_idx] << endl;
				// [x_3,i]^B
				outShares[(3*2*batch_size_bits*j)+4*batch_size_bits+batch_size_bits_idx] ^= 0 << (i%(8*sizeof(T)));
				outShares[(3*2*batch_size_bits*j)+4*batch_size_bits+batch_size_bits+batch_size_bits_idx] ^= 0 << (i%(8*sizeof(T)));
				cout << "[NecProtocolPartyRingFor3P.h] " << j << "-th x3 outShares[" << (3*2*batch_size_bits*j)+4*batch_size_bits+batch_size_bits_idx << "] = " << outShares[(3*2*batch_size_bits*j)+4*batch_size_bits+batch_size_bits_idx] << endl;
				cout << "[NecProtocolPartyRingFor3P.h] " << j << "-th x3 outShares[" << (3*2*batch_size_bits*j)+4*batch_size_bits+batch_size_bits+batch_size_bits_idx << "] = " << outShares[(3*2*batch_size_bits*j)+4*batch_size_bits+batch_size_bits+batch_size_bits_idx] << endl;
			}
			outShares[(3*2*batch_size_bits*j)+batch_size_bits_idx] = outShares[(3*2*batch_size_bits*j)+batch_size_bits_idx] ^ outShares[(3*2*batch_size_bits*j)+batch_size_bits+batch_size_bits_idx];
			outShares[(3*2*batch_size_bits*j)+2*batch_size_bits+batch_size_bits_idx] = outShares[(3*2*batch_size_bits*j)+2*batch_size_bits+batch_size_bits_idx] ^ outShares[(3*2*batch_size_bits*j)+2*batch_size_bits+batch_size_bits+batch_size_bits_idx];
			outShares[(3*2*batch_size_bits*j)+4*batch_size_bits+batch_size_bits_idx] = outShares[(3*2*batch_size_bits*j)+4*batch_size_bits+batch_size_bits_idx] ^ outShares[(3*2*batch_size_bits*j)+4*batch_size_bits+batch_size_bits+batch_size_bits_idx];
		}
	}
	return true;
}





template <typename T>
bool NecProtocolPartyRingFor3P<T>::PostRec(int ring_size,
		  Z2nShareReplicated<T> &inShares,
		  Z2nShareReplicated<T> &outShare)
{
	for(int i = 0; i < ring_size; i++)
	{
		outShare.elem1[0] += ((inShares.elem1[i] ^ inShares.elem2[i]) << i);
		outShare.elem2[0] += (inShares.elem2[i] << i);
	}
	outShare.elem1[0] = outShare.elem1[0] + outShare.elem2[0];
	return true;
}

template <typename T>
bool NecProtocolPartyRingFor3P<T>::SkewInj(
		  Z2nShareReplicated<T> &inShare,
		  Z2nShareReplicated<T> &outShares)
{
	if (m_partyId == 0)
	{
		// [x_1,i]^R
		outShares.elem1[0] = 0;
		outShares.elem2[0] = 0;
		// [x_2,i]^R
		outShares.elem1[1] = (inShare.elem1[0] - inShare.elem2[0]) & 1;
		outShares.elem2[1] = 0;
		// [x_3,i]^R
		outShares.elem1[2] = 0;
		outShares.elem2[2] = inShare.elem2[0] & 1;
	}
	else if (m_partyId == 1)
	{
		// [x_1,i]^R
		outShares.elem1[0] = 0;
		outShares.elem2[0] = inShare.elem2[0] & 1;
		// [x_2,i]^R
		outShares.elem1[1] = 0;
		outShares.elem2[1] = 0;
		// [x_3,i]^R
		outShares.elem1[2] = (inShare.elem1[0] - inShare.elem2[0]) & 1;
		outShares.elem2[2] = 0;
	}
	else if (m_partyId == 2)
	{
		// [x_1,i]^R
		outShares.elem1[0] = (inShare.elem1[0] - inShare.elem2[0]) & 1;
		outShares.elem2[0] = 0;
		// [x_2,i]^R
		outShares.elem1[1] = 0;
		outShares.elem2[1] = inShare.elem2[0] & 1;
		// [x_3,i]^R
		outShares.elem1[2] = 0;
		outShares.elem2[2] = 0;
	}

	outShares.elem1[0] = outShares.elem1[0] + outShares.elem2[0];
	outShares.elem1[1] = outShares.elem1[1] + outShares.elem2[1];
	outShares.elem1[2] = outShares.elem1[2] + outShares.elem2[2];

	return true;
}

template <typename T>
bool NecProtocolPartyRingFor3P<T>::SkewRec(
		  Z2nShareReplicated<T> &inShare,
		  Z2nShareReplicated<T> &outShares)
{
	if (m_partyId == 0)
	{
		// [x_1,i]^B
		outShares.elem1[0] = 0;
		outShares.elem2[0] = 0;
		// [x_2,i]^B
		outShares.elem1[1] = (inShare.elem1[0] - inShare.elem2[0]) & 1;
		outShares.elem2[1] = 0;
		// [x_3,i]^B
		outShares.elem1[2] = 0;
		outShares.elem2[2] = inShare.elem2[0] & 1;
	}
	else if (m_partyId == 1)
	{
		// [x_1,i]^B
		outShares.elem1[0] = 0;
		outShares.elem2[0] = inShare.elem2[0] & 1;
		// [x_2,i]^B
		outShares.elem1[1] = 0;
		outShares.elem2[1] = 0;
		// [x_3,i]^B
		outShares.elem1[2] = (inShare.elem1[0] - inShare.elem2[0]) & 1;
		outShares.elem2[2] = 0;
	}
	else if (m_partyId == 2)
	{
		// [x_1,i]^B
		outShares.elem1[0] = (inShare.elem1[0] - inShare.elem2[0]) & 1;
		outShares.elem2[0] = 0;
		// [x_2,i]^B
		outShares.elem1[1] = 0;
		outShares.elem2[1] = inShare.elem2[0] & 1;
		// [x_3,i]^B
		outShares.elem1[2] = 0;
		outShares.elem2[2] = 0;
	}

	outShares.elem1[0] = outShares.elem1[0] ^ outShares.elem2[0];
	outShares.elem1[1] = outShares.elem1[1] ^ outShares.elem2[1];
	outShares.elem1[2] = outShares.elem1[2] ^ outShares.elem2[2];

	return true;
}

template <typename T>
bool NecProtocolPartyRingFor3P<T>::mulM(int numOfShares,
		Z2nShareReplicated<T> &inShare,
		T coef,
		Z2nShareReplicated<T> &outShare)
{

#pragma ivdep
	for (int i=0; i<numOfShares; i++) {
		outShare.elem1[i] = coef * inShare.elem1[i];
		outShare.elem2[i] = coef * inShare.elem2[i];
	}

	return true;
}

template <typename T>
bool NecProtocolPartyRingFor3P<T>::mulSI(int numOfShares,
		Z2nShareReplicated<T> &inShare,
		T coef,
		Z2nShareReplicated<T> &outShare)
{

#pragma ivdep
	for (int i=0; i<numOfShares; i++) {
		outShare.elem1[i] = coef * inShare.elem1[i];
		outShare.elem2[i] = coef * inShare.elem2[i];
	}

	return true;
}

template <typename T>
bool NecProtocolPartyRingFor3P<T>::addM(int numOfShares,
		Z2nShareReplicated<T> &inShare,
		T coef,
		Z2nShareReplicated<T> &outShare)
{

#pragma ivdep
	if (m_partyId == 0) {
		outShare = inShare;
	}
	else if (m_partyId == 1) {
		for (int i=0; i<numOfShares; i++) {
			outShare.elem1[i] = coef + inShare.elem1[i];
			outShare.elem2[i] = coef + inShare.elem2[i];
		}
	}
	else if (m_partyId == 2) {
		for (int i=0; i<numOfShares; i++) {
			outShare.elem1[i] = coef + inShare.elem1[i];
			outShare.elem2[i] = inShare.elem2[i];
		}
	}
	return true;
}

template <typename T>
bool NecProtocolPartyRingFor3P<T>::addSI(int numOfShares,
		Z2nShareReplicated<T> &inShare,
		T coef,
		Z2nShareReplicated<T> &outShare)
{

#pragma ivdep
	if (m_partyId == 0) {
		outShare = inShare;
	}
	else if (m_partyId == 1) {
		for (int i=0; i<numOfShares; i++) {
			outShare.elem1[i] = coef + inShare.elem1[i];
			outShare.elem2[i] = coef + inShare.elem2[i];
		}
	}
	else if (m_partyId == 2) {
		for (int i=0; i<numOfShares; i++) {
			outShare.elem1[i] = coef + inShare.elem1[i];
			outShare.elem2[i] = inShare.elem2[i];
		}
	}
	return true;
}

template <typename T>
bool NecProtocolPartyRingFor3P<T>::subML(int numOfShares,
		Z2nShareReplicated<T> &inShare,
		T coef,
		Z2nShareReplicated<T> &outShare)
{

#pragma ivdep
	if (m_partyId == 0) {
		outShare = inShare;
	}
	else if (m_partyId == 1) {
		for (int i=0; i<numOfShares; i++) {
			outShare.elem1[i] = inShare.elem1[i] - coef;
			outShare.elem2[i] = inShare.elem2[i] - coef;
		}
	}
	else if (m_partyId == 2) {
		for (int i=0; i<numOfShares; i++) {
			outShare.elem1[i] = inShare.elem1[i] - coef;
			outShare.elem2[i] = inShare.elem2[i];
		}
	}

	return true;
}

template <typename T>
bool NecProtocolPartyRingFor3P<T>::subMR(int numOfShares,
		T coef,
		Z2nShareReplicated<T> &inShare,
		Z2nShareReplicated<T> &outShare)
{

#pragma ivdep
	if (m_partyId == 0) {
		outShare = inShare;
	}
	else if (m_partyId == 1) {
		for (int i=0; i<numOfShares; i++) {
			outShare.elem1[i] = coef - inShare.elem1[i];
			outShare.elem2[i] = coef - inShare.elem2[i];
		}
	}
	else if (m_partyId == 2) {
		for (int i=0; i<numOfShares; i++) {
			outShare.elem1[i] = coef - inShare.elem1[i];
			outShare.elem2[i] = inShare.elem2[i];
		}
	}

	return true;
}


template <typename T>
void NecProtocolPartyRingFor3P<T>::getInputFromOneParty(int partyID, T *value)
{
	T* shares[3];
	shares[0] = new T[2];
	shares[1] = new T[2];
	shares[2] = new T[2];
	if (m_partyId == partyID) {
		//read from terminal
		T input;
		cout << "input --> " ; cin >> input;
		shareOneInput(1, input, shares[0], shares[1], shares[2]);
		value[0] = shares[partyID][0];
		value[1] = shares[partyID][1];
		mpcParty->Write(shares[m_nextParty], 2, m_nextParty);
		mpcParty->Write(shares[m_prevParty], 2, m_prevParty);
	}
	else {
		mpcParty->Read(value, 2, partyID);
	}
}

//template <typename T>
//bool NecProtocolPartyRingFor3P<T>::multShares(int numOfShares,
//		  Z2nShareReplicated<T> &x_shares,
//		  Z2nShareReplicated<T> &y_shares,
//		  Z2nShareReplicated<T> &xy_shares)
//{
//	if (x_shares.getNumOfShares() != numOfShares || y_shares.getNumOfShares() != numOfShares || xy_shares.getNumOfShares() != numOfShares)
//	{
//		return false;
//	}
//
//#pragma ivdep
//	for (int i=0; i<numOfShares; i++)
//	{
//		xy_shares.elem2[i] = (x_shares.elem1[i] * y_shares.elem1[i]) - (x_shares.elem2[i] * y_shares.elem2[i])
//				+ (prg_i->getRandomLong() - prg_iMinus1->getRandomLong());
//	}
//	mpcParty->Write(xy_shares.elem2, numOfShares, m_nextParty);
//	mpcParty->Read(xy_shares.elem1, numOfShares, m_prevParty);
//	for (int i=0; i<numOfShares; i++)
//	{
//		xy_shares.elem1[i] += xy_shares.elem2[i];
//	}
//
//	return true;
//}

template <typename T>
bool NecProtocolPartyRingFor3P<T>::multShares(int numOfShares,
		  vector<T> &x_shares,
		  vector<T> &y_shares,
		  vector<T> &xy_shares)
{
	if (x_shares.size() != 2 * numOfShares * m_batchSize || y_shares.size() != 2 * numOfShares * m_batchSize  || xy_shares.size() != 2 * numOfShares * m_batchSize )
	{
		return false;
	}

#pragma ivdep

	T* shares[2];
	shares[0] = new T[numOfShares*m_batchSize];
	shares[1] = new T[numOfShares*m_batchSize];

	for (int i=0; i<numOfShares; i++)
	{
		for (int j=0; j<m_batchSize; j++)
		{
			xy_shares[2*m_batchSize*i+m_batchSize+j] =  shares[1][m_batchSize*i+j] = (x_shares[2*m_batchSize*i+j] * y_shares[2*m_batchSize*i+j])
																							- (x_shares[2*m_batchSize*i+m_batchSize+j] * y_shares[2*m_batchSize*i+m_batchSize+j])
																							+ (prg_i->getRandomLong() - prg_iMinus1->getRandomLong());
		}
	}

//	cout << "[NecProtocolPartyRingFor3P::multShare] numOfShares = " << numOfShares << endl;
//	cout << "[NecProtocolPartyRingFor3P::multShare] batch size  = " << m_batchSize << endl;

//	cout << "[NecProtocolPartyRingFor3P::multShare] Write (start)." << endl;
	mpcParty->Write(shares[1], numOfShares*m_batchSize, m_nextParty);
//	cout << "[NecProtocolPartyRingFor3P::multShare] Write (end)." << endl;
	mpcParty->Read(shares[0], numOfShares*m_batchSize, m_prevParty);
	for (int i=0; i<numOfShares; i++)
	{
		for (int j=0; j<m_batchSize; j++)
		{
			xy_shares[2*m_batchSize*i+j] =  shares[0][m_batchSize*i+j];
			xy_shares[2*m_batchSize*i+j] += xy_shares[2*m_batchSize*i+m_batchSize+j];
		}
	}

	return true;
}

template <typename T>
bool NecProtocolPartyRingFor3P<T>::load_share_immediate(Z2nIntReplicated<T> & values,
														Z2nShareReplicated<T> &shares)
{
	int size = values.getNumOfData();
	if (m_partyId == 0) {
		for (int i=0; i<size; i++) {
			shares.elem1[i] = 0;
			shares.elem2[i] = 0;
		}
	}
	else if (m_partyId == 1) {
		for (int i=0; i<size; i++) {
			shares.elem1[i] = values.elem[i];
			shares.elem2[i] = values.elem[i];
		}
	}
	else if (m_partyId == 2) {
		for (int i=0; i<size; i++) {
			shares.elem1[i] = values.elem[i];
			shares.elem2[i] = 0;
		}
	}
	else {
		cout << "ProtocolPartyRing - load_share_immediate failed: the party_id = " << m_partyId << "doesn't exists" << endl;
		return false;
	}

	return true;
}

#endif // NEC_PROTOCOLPARTY_H_
