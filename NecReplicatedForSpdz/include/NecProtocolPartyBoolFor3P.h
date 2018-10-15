#ifndef NEC_PROTOCOLPARTY_Bool_FOR_3P_H_
#define NEC_PROTOCOLPARTY_Bool_FOR_3P_H_

#include <stdlib.h>
#include "MPCParty.hpp"
#include "PRG.hpp"
#include "Z2nIntReplicated.h"
#include "Z2nShareReplicated.h"
#include <thread>
#include <iostream>
#include <bitset>
#include <vector>
#include <assert.h>
#include <math.h>

#define flag_print false
#define flag_print_timings true

#define N 3

using namespace std;

template <typename T>
class NecProtocolPartyBoolFor3P {
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

  NecProtocolPartyBoolFor3P(int partyID, int numOfOpens, int batch_size);
  ~NecProtocolPartyBoolFor3P();

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

  void shareOneInput(int numOfShares,
		  	  T *secrets,
			  T *share1,
			  T *share2,
			  T *share3);

  bool shareInput(int input_party_id,
		  	  	  	  int num_input,
		  	  	  	  T *input_values,
					  T *output_values);

  void getInputFromOneParty(int partyID, T *value);

  void openShares(int numOfShares,
	    Z2nShareReplicated<T> &shares,
	    Z2nIntReplicated<T> &secrets);

  bool openShares(int numOfShares,
  		 vector<T> &shares,
  		 vector<T> &secrets);

  void addS(int numOfShares,
		  Z2nShareReplicated<T> &inShare1,
		  Z2nShareReplicated<T> &inShare2,
		  Z2nShareReplicated<T> &outShare);

  void subS(int numOfShares,
		  Z2nShareReplicated<T> &inShare1,
		  Z2nShareReplicated<T> &inShare2,
		  Z2nShareReplicated<T> &outShare);

  bool SkewDec(int numOfBits,
		  Z2nShareReplicated<T> &inShare,
		  Z2nShareReplicated<T> &outShares);

  bool PostRec(int ring_size,
		  Z2nShareReplicated<T> &inShares,
		  Z2nShareReplicated<T> &outShare);

  bool SkewInj(
  		  Z2nShareReplicated<T> &inShare,
  		  Z2nShareReplicated<T> &outShares,
		  int ring_size_out);

  bool SkewInj(
    	  Z2nShareReplicated<T> &inShare,
    	  Z2nShareReplicated<T> &outShares1,
    	  Z2nShareReplicated<T> &outShares2,
    	  Z2nShareReplicated<T> &outShares3,
		  int ring_size_out);

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

  bool multShares(int numOfShares,
		  Z2nShareReplicated<T> &x_shares,
		  Z2nShareReplicated<T> &y_shares,
		  Z2nShareReplicated<T> &xy_shares);

  bool multShares(int numOfShares,
		  uint8_t * x_shares,
		  uint8_t * y_shares,
		  uint8_t * xy_shares);

  bool multShares(int numOfShares,
		  uint8_t * x1_shares,
		  uint8_t * x2_shares,
		  uint8_t * y1_shares,
		  uint8_t * y2_shares,
		  uint8_t * xy1_shares,
		  uint8_t * xy2_shares);

  bool load_share_immediate(Z2nIntReplicated<T> & values,
		  	  	  	  	  	Z2nShareReplicated<T> &shares);

  void end();

};


template <typename T>
NecProtocolPartyBoolFor3P<T>::NecProtocolPartyBoolFor3P(int partyID, int numOfOpens, int batch_size)
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
NecProtocolPartyBoolFor3P<T>::~NecProtocolPartyBoolFor3P() {
  delete prg_i;
  delete prg_iMinus1;
  delete prg_input;
//  delete prg_genshare1;
//  delete prg_genshare2;
  delete mpcParty;
}

template <typename T>
void NecProtocolPartyBoolFor3P<T>::init() {
	  mpcParty = new MPCParty(m_partyId, 1, "partiesMPCLocal.conf");
//	  mpcParty = new MPCParty(m_partyId, 1, "partiesMPC.conf");

  input.init(numOfMults);
  ownShare.init(numOfMults);
  output.init(numOfMults);

  triples_a.init(numOfMults);
  triples_b.init(numOfMults);
  triples_c.init(numOfMults);
}

template <typename T>
void NecProtocolPartyBoolFor3P<T>::offline() {
  generateRandomness(numOfMults, input);
}

template <typename T>
void NecProtocolPartyBoolFor3P<T>::generateRandomness(int numOfOpens,
					  Z2nIntReplicated<T> &secrets) {
  // generate random values by same seed
  for (int i=0; i<numOfOpens; i++) {
    secrets.elem[i] = prg_input->getRandomLong();
  }
}

template <typename T>
void NecProtocolPartyBoolFor3P<T>::triples() {
	for (int i=0; i<numOfMults; i++) {
		triples_a[i] = 0;
		triples_b[i] = 0;
		triples_c[i] = 0;
	}
	//triples(numOfMults, triples_a, triples_b, triples_c);
}

template <typename T>
void NecProtocolPartyBoolFor3P<T>::triples(int numOfTriples,
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

		share1.elem1[i] ^= share1.elem2[i];
		share2.elem1[i] ^= share2.elem2[i];

		// partial computation for [c] = [a] * [b]
		share3.elem2[i] = (share1.elem1[i]&share2.elem1[i]) ^ (share1.elem2[i]&share2.elem2[i]);
	}

	//ring communication
	mpcParty->Write(share3.elem2, numOfTriples, m_nextParty);
	mpcParty->Read(share3.elem1, numOfTriples, m_prevParty);

	// remain of the computation for [c] = [a]*[b]
	for (int i=0; i<numOfTriples; i++) {
		share3.elem1[i] ^= share3.elem2[i];
	}

}

template <typename T>
void NecProtocolPartyBoolFor3P<T>::shareInput() {
  shareInput(numOfMults, input, ownShare);
}

template <typename T>
void NecProtocolPartyBoolFor3P<T>::shareOneInput(int numOfShares,
				  T *secret,
				  T *share1,
				  T *share2,
				  T *share3)
{
	int batch_size = (int)ceil((float)m_batchSize/(8*sizeof(T)));

  for (int i=0; i<numOfShares; i++) {
	  // original (start)
//	  share1[1] = prg_genshare1->getRandomLong();
//	  share2[1] = prg_genshare2->getRandomLong();
//	  share3[1] = secret ^ share1[1] ^ share2[1];
//	  share1[0] = share3[1] ^ share1[1];
//	  share2[0] = share1[1] ^ share2[1];
//	  share3[0] = share2[1] ^ share3[1];
	  // original (end)
	  share1[i+batch_size] = prg_genshare1->getRandomLong();
	  share2[i+batch_size] = prg_genshare2->getRandomLong();
	  share3[i+batch_size] = secret[i] ^ share1[i+batch_size] ^ share2[i+batch_size];
	  share1[i] = share3[i+batch_size] ^ share1[i+batch_size];
	  share2[i] = share1[i+batch_size] ^ share2[i+batch_size];
	  share3[i] = share2[i+batch_size] ^ share3[i+batch_size];

//	  cout << "[NecProtocolPartyBoolFor3P.h] secret[" << i << "] = " << secret[i] << endl;
  }

}

template <typename T>
void NecProtocolPartyBoolFor3P<T>::shareInput(int numOfShares,
				  Z2nIntReplicated<T> &secret,
				  Z2nShareReplicated<T> &myShare)
{
  if (secret.getNumOfData() == 0) {
    cout << "[NecProtocolPartyBoolFor3P::shareInput] source size is 0" << endl;
    exit(1);
  }

  Z2nShareReplicated<T> share1(secret.getNumOfData());
  Z2nShareReplicated<T> share2(secret.getNumOfData());
  Z2nShareReplicated<T> share3(secret.getNumOfData());

#pragma ivdep
  for (int i=0; i<numOfShares; i++) {
    share1.elem2[i] = prg_genshare1->getRandomLong();
    share2.elem2[i] = prg_genshare2->getRandomLong();
    share3.elem2[i] = secret.elem[i] ^ share1.elem2[i] ^ share2.elem2[i];

    share1.elem1[i] = share3.elem2[i] ^ share1.elem2[i];
    share2.elem1[i] = share1.elem2[i] ^ share2.elem2[i];
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
bool NecProtocolPartyBoolFor3P<T>::shareInput(int input_party_id,
					  int num_input,
		  	  	  	  T *input_values,
					  T *output_values)
{
	T* shares[3];
	// original (start)
//	shares[0] = new T[2*num_input];
//	shares[1] = new T[2*num_input];
//	shares[2] = new T[2*num_input];
	// original (end)

	int batch_size = (int)ceil((float)m_batchSize/(8*sizeof(T)));

	//added(start)
	shares[0] = new T[2*num_input*batch_size];
	shares[1] = new T[2*num_input*batch_size];
	shares[2] = new T[2*num_input*batch_size];
	//added(end)

	if (m_partyId == input_party_id) {

		for (int i=0; i<num_input; i++) {
			// original (start)
           //	shareOneInput(1, input_values[i], &shares[0][2*i], &shares[1][2*i], &shares[2][2*i]);
			// original (end)

			// added(start)
			shareOneInput(batch_size, &input_values[i*batch_size],
										&shares[0][2*i*batch_size], &shares[1][2*i*batch_size], &shares[2][2*i*batch_size]);
			//added(end)
		}
		// original (start)
//		mpcParty->Write(shares[m_nextParty], 2*num_input, m_nextParty);
//		mpcParty->Write(shares[m_prevParty], 2*num_input, m_prevParty);
//		memcpy(output_values, shares[m_partyId], 2*num_input*sizeof(T));
		// original (end)

		//added (start)
		mpcParty->Write(shares[m_nextParty], 2*num_input*batch_size, m_nextParty);
		mpcParty->Write(shares[m_prevParty], 2*num_input*batch_size, m_prevParty);
		memcpy(output_values, shares[m_partyId], 2*num_input*batch_size*sizeof(T));
		//added (end)
	}
	else {
		// original (start)
//		mpcParty->Read(output_values, 2*num_input, input_party_id);
		// original (end)

		// added (start)
		mpcParty->Read(output_values, 2*num_input*batch_size, input_party_id);
		// added (end)
	}

	return true;

}


template <typename T>
void NecProtocolPartyBoolFor3P<T>::open() {
  openShares(numOfMults, ownShare, output);
}

template <typename T>
void NecProtocolPartyBoolFor3P<T>::openShares(int numOfShares,
			    Z2nShareReplicated<T> &shares,
			    Z2nIntReplicated<T> &ret)
{
  int size = shares.getNumOfShares();

  T *recBuf;
  recBuf = new T[size];

  mpcParty->Write(shares.elem1, size, m_nextParty);
  mpcParty->Read(recBuf, size, m_prevParty);

  for (int i=0; i<size; i++) {
    ret.elem[i] = shares.elem2[i] ^ recBuf[i];
  }

}

template <typename T>
bool NecProtocolPartyBoolFor3P<T>::openShares(int numOfShares,
													vector<T> &shares,
													vector<T> &secrets) {

	int batch_size = (int)ceil((float)m_batchSize/(8*sizeof(T)));

//	cout << "======================" << endl;
//	cout << "[NecProtocolPartyBoolFor3P]" << endl;
//	cout << "batch_size    = "<< batch_size << endl;
//	cout << "numOfShares   = "<< numOfShares << endl;
//	cout << "shares.size() = "<< shares.size()<< endl;
//	cout << "======================" << endl;

	assert(2*numOfShares*batch_size == shares.size());

//    cout << "NecProtocolPartyBoolFor3P.h::test3" << endl;

	int size_opens = numOfShares*batch_size;

	T sendBuf[size_opens];
	T recBuf[size_opens];

	for (int i=0; i<numOfShares; i++) {
		for (int j=0; j<batch_size; j++) {
			sendBuf[i*batch_size+j] = shares[2*i*batch_size+j];
		}
	}

	mpcParty->Write(sendBuf, size_opens, m_nextParty);
	mpcParty->Read(recBuf, size_opens, m_prevParty);

	secrets.resize(size_opens);

	for (int i=0; i<numOfShares; i++) {
		for (int j=0; j<batch_size; j++) {
			secrets[i*batch_size+j] = shares[2*i*batch_size + j + batch_size] ^ recBuf[i*batch_size+j];
		}
	}
}


template <typename T>
void NecProtocolPartyBoolFor3P<T>::end()
{}

template <typename T>
void NecProtocolPartyBoolFor3P<T>::test()
{
  for (int i=0; i<numOfMults; i++) {
    if (input.elem[i] != output.elem[i])
      {
	cout << "[NecProtocolPartyBoolFor3P] test() failed: doesn't match" << endl;
	exit(1);
      }
  }
  if (flag_print) {
    cout << "output: " << endl;
    output.dump();
  }
  cout << "[NecProtocolPartyBoolFor3P] all shares are successfully reconstructed. " << endl;
}

template <typename T>
void NecProtocolPartyBoolFor3P<T>::addS(int numOfShares,
		Z2nShareReplicated<T> &inShare1,
		Z2nShareReplicated<T> &inShare2,
		Z2nShareReplicated<T> &outShare)
{
#pragma ivdep
	for (int i=0; i<numOfShares; i++) {
		outShare.elem1[i] = inShare1.elem1[i] ^ inShare2.elem1[i];
		outShare.elem2[i] = inShare2.elem2[i] ^ inShare2.elem2[i];
	}
}


template <typename T>
void NecProtocolPartyBoolFor3P<T>::subS(int numOfShares,
		Z2nShareReplicated<T> &inShare1,
		Z2nShareReplicated<T> &inShare2,
		Z2nShareReplicated<T> &outShare)
{
#pragma ivdep
	for (int i=0; i<numOfShares; i++) {
		outShare.elem1[i] = inShare1.elem1[i] ^ inShare2.elem1[i];
		outShare.elem2[i] = inShare2.elem2[i] ^ inShare2.elem2[i];
	}
}

template <typename T>
bool NecProtocolPartyBoolFor3P<T>::SkewDec(int numOfBits,
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
bool NecProtocolPartyBoolFor3P<T>::PostRec(int ring_size,
		  Z2nShareReplicated<T> &inShares,
		  Z2nShareReplicated<T> &outShare)
{
	size_t bs = static_cast<int>(ceil((float)m_batchSize / (8*sizeof(T))));

	for (int i = 0; i < ring_size; i++)
	{
		for(int j = 0; j<m_batchSize; j++)
		{
			int idx_b = j/64;
			T tmp1 = ((inShares.elem1[i*bs+idx_b] ^ inShares.elem2[i*bs+idx_b]) >> (j % (8*sizeof(T)))) & 0x1;
			T tmp2 = (inShares.elem2[i*bs+idx_b] >> (j % (8*sizeof(T)))) & 0x1;
			outShare.elem1[j] += (tmp1 << i);
			outShare.elem2[j] += (tmp2 << i);
		}
	}

	for(int j = 0; j<m_batchSize; j++)
	{
		outShare.elem1[j] += outShare.elem2[j];
	}

	return true;

//	for(int i = 0; i < ring_size; i++)
//	{
//		outShare.elem1[0] += ((inShares.elem1[i] ^ inShares.elem2[i]) << i);
//		outShare.elem2[0] += (inShares.elem2[i] << i);
//	}
//
//	outShare.elem1[0] = outShare.elem1[0] + outShare.elem2[0];
//
//	return true;
}

template <typename T>
bool NecProtocolPartyBoolFor3P<T>::SkewInj(
		  Z2nShareReplicated<T> &inShare,
		  Z2nShareReplicated<T> &outShares,
		  int ring_size_out)
{
	if (m_partyId == 0)
	{
		// [x_1,i]^R
		outShares.elem1[0] = 0;
		outShares.elem2[0] = 0;
		// [x_2,i]^R
		outShares.elem1[1] = (inShare.elem1[0] ^ inShare.elem2[0]) & 1;
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
		outShares.elem1[2] = (inShare.elem1[0] ^ inShare.elem2[0]) & 1;
		outShares.elem2[2] = 0;
	}
	else if (m_partyId == 2)
	{
		// [x_1,i]^R
		outShares.elem1[0] = (inShare.elem1[0] ^ inShare.elem2[0]) & 1;
		outShares.elem2[0] = 0;
		// [x_2,i]^R
		outShares.elem1[1] = 0;
		outShares.elem2[1] = inShare.elem2[0] & 1;
		// [x_3,i]^R
		outShares.elem1[2] = 0;
		outShares.elem2[2] = 0;
	}

	if (ring_size_out != 1) {
		outShares.elem1[0] = outShares.elem1[0] + outShares.elem2[0];
		outShares.elem1[1] = outShares.elem1[1] + outShares.elem2[1];
		outShares.elem1[2] = outShares.elem1[2] + outShares.elem2[2];
	}
	else {
		outShares.elem1[0] = outShares.elem1[0] ^ outShares.elem2[0];
		outShares.elem1[1] = outShares.elem1[1] ^ outShares.elem2[1];
		outShares.elem1[2] = outShares.elem1[2] ^ outShares.elem2[2];
	}

	return true;
}

template <typename T>
bool NecProtocolPartyBoolFor3P<T>::SkewInj(
		  Z2nShareReplicated<T> &inShare,
		  Z2nShareReplicated<T> &outShares1,
		  Z2nShareReplicated<T> &outShares2,
		  Z2nShareReplicated<T> &outShares3,
		  int ring_size_out)
{
	size_t bs = static_cast<int>(ceil((float)m_batchSize / (8*sizeof(T))));
	for(int i=0; i<m_batchSize; i++) {
		int idx_b = i/64;
		T tmp1 = ((inShare.elem1[idx_b] ^ inShare.elem2[idx_b]) >> (i % (8*sizeof(T)))) & 0x1;
		T tmp2 = (inShare.elem2[idx_b] >> (i % (8*sizeof(T)))) & 0x1;

		if (m_partyId == 0)
		{
			if (ring_size_out != 1)
			{
				outShares1.elem1[i] = 0;
				outShares1.elem2[i] = 0;
				outShares2.elem1[i] = tmp1;
				outShares2.elem2[i] = 0;
				outShares3.elem1[i] = 0;
				outShares3.elem2[i] = tmp2;
			}
			else
			{
				outShares1.elem1[idx_b] ^= 0;
				outShares1.elem2[idx_b] ^= 0;
				outShares2.elem1[idx_b] ^= (tmp1 << (i % (8*sizeof(T))));
				outShares2.elem2[idx_b] ^= 0;
				outShares3.elem1[idx_b] ^= 0;
				outShares3.elem2[idx_b] ^= (tmp2 << (i % (8*sizeof(T))));
			}
		}
		else if (m_partyId == 1)
		{
			if (ring_size_out != 1)
			{
				outShares1.elem1[i] = 0;
				outShares1.elem2[i] = tmp2;
				outShares2.elem1[i] = 0;
				outShares2.elem2[i] = 0;
				outShares3.elem1[i] = tmp1;
				outShares3.elem2[i] = 0;
			}
			else
			{
				outShares1.elem1[idx_b] ^= 0;
				outShares1.elem2[idx_b] ^= (tmp2 << (i % (8*sizeof(T))));
				outShares2.elem1[idx_b] ^= 0;
				outShares2.elem2[idx_b] ^= 0;
				outShares3.elem1[idx_b] ^= (tmp1 << (i % (8*sizeof(T))));
				outShares3.elem2[idx_b] ^= 0;
			}
		}
		else if (m_partyId == 2)
		{
			if (ring_size_out != 1)
			{
				outShares1.elem1[i] = tmp1;
				outShares1.elem2[i] = 0;
				outShares2.elem1[i] = 0;
				outShares2.elem2[i] = tmp2;
				outShares3.elem1[i] = 0;
				outShares3.elem2[i] = 0;
			}
			else
			{
				outShares1.elem1[idx_b] ^= (tmp1 << (i % (8*sizeof(T))));
				outShares1.elem2[idx_b] ^= 0;
				outShares2.elem1[idx_b] ^= 0;
				outShares2.elem2[idx_b] ^= (tmp2 << (i % (8*sizeof(T))));
				outShares3.elem1[idx_b] ^= 0;
				outShares3.elem2[idx_b] ^= 0;
			}
		}

		if (ring_size_out != 1)
		{
			outShares1.elem1[i] += outShares1.elem2[i];
			outShares2.elem1[i] += outShares2.elem2[i];
			outShares3.elem1[i] += outShares3.elem2[i];
		}
	}

	if (ring_size_out == 1)
	{
		for (size_t i=0; i<bs; i++)
		{
			outShares1.elem1[i] ^= outShares1.elem2[i];
			outShares2.elem1[i] ^= outShares2.elem2[i];
			outShares3.elem1[i] ^= outShares3.elem2[i];
		}
	}

	return true;
}

template <typename T>
bool NecProtocolPartyBoolFor3P<T>::SkewRec(
		  Z2nShareReplicated<T> &inShare,
		  Z2nShareReplicated<T> &outShares)
{
	if (m_partyId == 0)
	{
		// [x_1,i]^B
		outShares.elem1[0] = 0;
		outShares.elem2[0] = 0;
		// [x_2,i]^B
		outShares.elem1[1] = (inShare.elem1[0] ^ inShare.elem2[0]) & 1;
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
		outShares.elem1[2] = (inShare.elem1[0] ^ inShare.elem2[0]) & 1;
		outShares.elem2[2] = 0;
	}
	else if (m_partyId == 2)
	{
		// [x_1,i]^B
		outShares.elem1[0] = (inShare.elem1[0] ^ inShare.elem2[0]) & 1;
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
bool NecProtocolPartyBoolFor3P<T>::mulM(int numOfShares,
		Z2nShareReplicated<T> &inShare,
		T coef,
		Z2nShareReplicated<T> &outShare)
{

#pragma ivdep
	for (int i=0; i<numOfShares; i++) {
		outShare.elem1[i] = coef & inShare.elem1[i];
		outShare.elem2[i] = coef & inShare.elem2[i];
	}

	return true;
}

template <typename T>
bool NecProtocolPartyBoolFor3P<T>::mulSI(int numOfShares,
		Z2nShareReplicated<T> &inShare,
		T coef,
		Z2nShareReplicated<T> &outShare)
{

#pragma ivdep
	for (int i=0; i<numOfShares; i++) {
		outShare.elem1[i] = coef & inShare.elem1[i];
		outShare.elem2[i] = coef & inShare.elem2[i];
	}

	return true;
}

template <typename T>
bool NecProtocolPartyBoolFor3P<T>::addM(int numOfShares,
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
			outShare.elem1[i] = coef ^ inShare.elem1[i];
			outShare.elem2[i] = coef ^ inShare.elem2[i];
		}
	}
	else if (m_partyId == 2) {
		for (int i=0; i<numOfShares; i++) {
			outShare.elem1[i] = coef ^ inShare.elem1[i];
			outShare.elem2[i] = inShare.elem2[i];
		}
	}
	return true;
}

template <typename T>
bool NecProtocolPartyBoolFor3P<T>::addSI(int numOfShares,
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
			outShare.elem1[i] = coef ^ inShare.elem1[i];
			outShare.elem2[i] = coef ^ inShare.elem2[i];
		}
	}
	else if (m_partyId == 2) {
		for (int i=0; i<numOfShares; i++) {
			outShare.elem1[i] = coef ^ inShare.elem1[i];
			outShare.elem2[i] = inShare.elem2[i];
		}
	}
	return true;
}

template <typename T>
bool NecProtocolPartyBoolFor3P<T>::subML(int numOfShares,
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
			outShare.elem1[i] = coef ^ inShare.elem1[i];
			outShare.elem2[i] = coef ^ inShare.elem2[i];
		}
	}
	else if (m_partyId == 2) {
		for (int i=0; i<numOfShares; i++) {
			outShare.elem1[i] = coef ^ inShare.elem1[i];
			outShare.elem2[i] = inShare.elem2[i];
		}
	}

	return true;
}

template <typename T>
bool NecProtocolPartyBoolFor3P<T>::subMR(int numOfShares,
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
			outShare.elem1[i] = inShare.elem1[i] ^ coef;
			outShare.elem2[i] = inShare.elem2[i] ^ coef;
		}
	}
	else if (m_partyId == 2) {
		for (int i=0; i<numOfShares; i++) {
			outShare.elem1[i] = inShare.elem1[i] ^ coef;
			outShare.elem2[i] = inShare.elem2[i];
		}
	}

	return true;
}

template <typename T>
void NecProtocolPartyBoolFor3P<T>::getInputFromOneParty(int partyID, T *value)
{
	T* shares[3];
	shares[0] = new T[2];
	shares[1] = new T[2];
	shares[2] = new T[2];
	if (m_partyId == partyID) {
		//read from terminal
		T input;
		memset(&input, 0, 1);
		cout << "ginput --> " ; cin >> input;
		if (input != 0 && input != 1) {
			cout << "[NecProtocolPartyBoolFor3P] input() failed: the input is not bool" << endl;
			exit(1);
		}
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

template <typename T>
bool NecProtocolPartyBoolFor3P<T>::multShares(int numOfShares,
		  Z2nShareReplicated<T> &x_shares,
		  Z2nShareReplicated<T> &y_shares,
		  Z2nShareReplicated<T> &xy_shares)
{
	if (x_shares.getNumOfShares() != numOfShares || y_shares.getNumOfShares() != numOfShares || xy_shares.getNumOfShares() != numOfShares)
	{
		return false;
	}
#pragma ivdep
	for (int i=0; i<numOfShares; i++)
	{
		xy_shares.elem2[i] = (x_shares.elem1[i] & y_shares.elem1[i]) ^ (x_shares.elem2[i] & y_shares.elem2[i])
				^ ((prg_i->getRandom() & 1) ^ (prg_iMinus1->getRandom() & 1));
	}

	mpcParty->Write(xy_shares.elem2, numOfShares, m_nextParty);
	mpcParty->Read(xy_shares.elem1, numOfShares, m_prevParty);
	for (int i=0; i<numOfShares; i++)
	{
		xy_shares.elem1[i] ^= xy_shares.elem2[i];
	}

	return true;
}

template <typename T>
bool NecProtocolPartyBoolFor3P<T>::multShares(int numOfShares,
		  uint8_t * x_shares,
		  uint8_t * y_shares,
		  uint8_t * xy_shares) {

#pragma ivdep
	for (int i=0; i<numOfShares; i++)
	{
		xy_shares[numOfShares+i] = (x_shares[2*i] & y_shares[2*i]) ^ (x_shares[2*i+1] & y_shares[2*i+1])
										^ (prg_i->getRandom()&1) ^ (prg_iMinus1->getRandom()&1);
	}

	mpcParty->Write(&xy_shares[numOfShares], numOfShares, m_nextParty);
	mpcParty->Read(&xy_shares[0], numOfShares, m_prevParty);
#pragma ivdep
	for (int i=0; i<numOfShares; i++)
	{
		xy_shares[i] ^= (xy_shares[numOfShares+i]);
	}

	return true;
}

template <typename T>
bool NecProtocolPartyBoolFor3P<T>::multShares(int numOfShares,
		  uint8_t * x1_shares,
		  uint8_t * x2_shares,
		  uint8_t * y1_shares,
		  uint8_t * y2_shares,
		  uint8_t * xy1_shares,
		  uint8_t * xy2_shares) {

	int batch_size = (int)ceil((float)m_batchSize/(sizeof(T)));
#pragma ivdep
//	for (size_t i=0; i<numOfShares; i++) {
//		for (int j=0; j<batch_size; j++) {
//			xy2_shares[i*batch_size + j] = (x1_shares[i*batch_size + j] & y1_shares[i*batch_size + j]) ^ (x2_shares[i*batch_size+ j] & y2_shares[i*batch_size + j])
//							^ prg_i->getRandomByte() ^ prg_iMinus1->getRandomByte();
//		}
//	}

	for (int i=0; i<numOfShares; i++) {
		xy2_shares[i] = (x1_shares[i] & y1_shares[i]) ^ (x2_shares[i] & y2_shares[i])
									^ prg_i->getRandomByte() ^ prg_iMinus1->getRandomByte();
	}

	mpcParty->Write(&xy2_shares[0], numOfShares, m_nextParty);
	mpcParty->Read(&xy1_shares[0], numOfShares, m_prevParty);
//	cout << "[NecProtocolPartyBoolFor3P.h] numOfShares = " << numOfShares << endl;
//	cout << "[NecProtocolPartyBoolFor3P.h] batch_size = " << batch_size << endl;

#pragma ivdep
//	for (int i=0; i<numOfShares; i++)
//	{
//		for (int j=0; j<batch_size; j++)
//		{
//			xy1_shares[i*batch_size + j] ^= (xy2_shares[i*batch_size + j]);
//		}
//	}
	for (int i=0; i<numOfShares; i++) {
		xy1_shares[i] ^= xy2_shares[i];
	}
	return true;

//#pragma ivdep
//	for (size_t i=0; i<numOfShares; i++) {
//		xy2_shares[i] = (x1_shares[i] & y1_shares[i]) ^ (x2_shares[i] & y2_shares[i])
//						^ prg_i->getRandomByte() ^ prg_iMinus1->getRandomByte();
//	}
//
//	mpcParty->Write(&xy2_shares[0], numOfShares, m_nextParty);
//	mpcParty->Read(&xy1_shares[0], numOfShares, m_prevParty);
//
//#pragma ivdep
//	for (int i=0; i<numOfShares; i++)
//	{
//		xy1_shares[i] ^= (xy2_shares[i]);
//	}
//
//	return true;

}

template <typename T>
bool NecProtocolPartyBoolFor3P<T>::load_share_immediate(Z2nIntReplicated<T> & values,
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
		cout << "ProtocolPartyBool - load_share_immediate failed: the party_id = " << m_partyId << "doesn't exists" << endl;
		return false;
	}

	return true;
}

#endif // NEC_PROTOCOLPARTY_H_
