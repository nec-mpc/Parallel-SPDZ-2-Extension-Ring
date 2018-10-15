#include "spdz2_ext_processor_Z2n.h"

#include <syslog.h>
#include <bitset>

spdz2_ext_processor_Z2n::spdz2_ext_processor_Z2n(const int batch_size)
 : spdz2_ext_processor_base()
 , the_party(NULL), batch_size(batch_size)
{
}

spdz2_ext_processor_Z2n::~spdz2_ext_processor_Z2n()
{
}

int spdz2_ext_processor_Z2n::mix_add(SPDZEXT_VALTYPE * share, SPDZEXT_VALTYPE scalar)
{
//	syslog(LOG_INFO, "spdz2_ext_processor_Z2n::mix_add: (s)%lu + (c)%lu", *share, scalar);
	Z2nShareReplicated<SPDZEXT_VALTYPE> input(1), output(1);
	input.elem1[0] = share[0];
	input.elem2[0] = share[1];

	if(the_party->addM(1, input, scalar, output))
	{
		share[0] = output.elem1[0];
		share[1] = output.elem2[0];
//		syslog(LOG_INFO, "spdz2_ext_processor_Z2n::mix_add: result = (s)%lu, %lu", share[0], share[1]);
		return 0;
	}
	syslog(LOG_ERR, "spdz2_ext_processor_Z2n::mix_add: protocol addShareAndScalar failure.");

	return -1;
}

int spdz2_ext_processor_Z2n::si_add(SPDZEXT_VALTYPE * share, SPDZEXT_VALTYPE scalar)
{
//	syslog(LOG_INFO, "spdz2_ext_processor_Z2n::si_add: (s)%lu + (c)%lu", *share, scalar);
	Z2nShareReplicated<SPDZEXT_VALTYPE> input(1), output(1);
	input.elem1[0] = share[0];
	input.elem2[0] = share[1];

	if(the_party->addSI(1, input, scalar, output))
	{
		share[0] = output.elem1[0];
		share[1] = output.elem2[0];
//		syslog(LOG_INFO, "spdz2_ext_processor_Z2n::si_add: result = (s)%lu, %lu", share[0], share[1]);
		return 0;
	}
	syslog(LOG_ERR, "spdz2_ext_processor_Z2n::si_add: protocol addShareAndScalar failure.");

	return -1;
}

int spdz2_ext_processor_Z2n::mix_sub_scalar(SPDZEXT_VALTYPE * share, SPDZEXT_VALTYPE scalar)
{
//	syslog(LOG_INFO, "spdz2_ext_processor_Z2n::mix_sub_scalar: (s)%lu - (c)%lu", *share, scalar);
	Z2nShareReplicated<SPDZEXT_VALTYPE> input(1), output(1);
	input.elem1[0] = share[0];
	input.elem2[0] = share[1];

	if(the_party->subML(1, input, scalar, output))
	{
		share[0] = output.elem1[0];
		share[1] = output.elem2[0];
//		syslog(LOG_INFO, "spdz2_ext_processor_Z2n::mix_sub_scalar: result = (s)%lu", *share);
		return 0;
	}
	syslog(LOG_ERR, "spdz2_ext_processor_Z2n::mix_sub_scalar: protocol shareSubScalar failure.");

	return -1;
}

int spdz2_ext_processor_Z2n::mix_sub_share(SPDZEXT_VALTYPE scalar, SPDZEXT_VALTYPE * share)
{
//	syslog(LOG_INFO, "spdz2_ext_processor_Z2n::mix_sub_share: (c)%lu - (s)%lu", scalar, *share);
	Z2nShareReplicated<SPDZEXT_VALTYPE> input(1), output(1);
	input.elem1[0] = share[0];
	input.elem2[0] = share[1];

	if(the_party->subMR(1, scalar, input, output))
	{
		share[0] = output.elem1[0];
		share[1] = output.elem2[0];
//		syslog(LOG_INFO, "spdz2_ext_processor_Z2n::mix_sub_share: result = (s)%lu", *share);
		return 0;
	}
	syslog(LOG_ERR, "spdz2_ext_processor_Z2n::mix_sub_share: protocol shareSubScalar failure.");

	return -1;
}

int spdz2_ext_processor_Z2n::mix_mul(SPDZEXT_VALTYPE * share, SPDZEXT_VALTYPE scalar)
{
//	syslog(LOG_INFO, "spdz2_ext_processor_Z2n::mix_add: (s)%lu + (c)%lu", *share, scalar);
	Z2nShareReplicated<SPDZEXT_VALTYPE> input(1), output(1);
	input.elem1[0] = share[0];
	input.elem2[0] = share[1];

	if(the_party->mulM(1, input, scalar, output))
	{
		share[0] = output.elem1[0];
		share[1] = output.elem2[0];
//		syslog(LOG_INFO, "spdz2_ext_processor_Z2n::mix_add: result = (s)%lu, %lu", share[0], share[1]);
		return 0;
	}
	syslog(LOG_ERR, "spdz2_ext_processor_Z2n::mix_add: protocol addShareAndScalar failure.");

	return -1;
}

int spdz2_ext_processor_Z2n::si_mul(SPDZEXT_VALTYPE * share, SPDZEXT_VALTYPE scalar)
{
//	syslog(LOG_INFO, "spdz2_ext_processor_Z2n::si_mul: (s)%lu + (c)%lu", *share, scalar);
	Z2nShareReplicated<SPDZEXT_VALTYPE> input(1), output(1);
	input.elem1[0] = share[0];
	input.elem2[0] = share[1];

	if(the_party->mulSI(1, input, scalar, output))
	{
		share[0] = output.elem1[0];
		share[1] = output.elem2[0];
//		syslog(LOG_INFO, "spdz2_ext_processor_Z2n::si_mul: result = (s)%lu, %lu", share[0], share[1]);
		return 0;
	}
	syslog(LOG_ERR, "spdz2_ext_processor_Z2n::si_mul: protocol mulShareAndImmediateScalar failure.");

	return -1;
}

//int spdz2_ext_processor_Z2n::skew_dec(SPDZEXT_VALTYPE * share, const int num_of_bit, SPDZEXT_VALTYPE output_shares[][2]) //output_shares[num_of_bit * 3][2]
//{
//	syslog(LOG_INFO, "spdz2_ext_processor_Z2n::skew_dec: (s)%lu + (c)%x", *share, num_of_bit);
//	Z2nShareReplicated<SPDZEXT_VALTYPE> input(1), output(3 * num_of_bit);
//	input.elem1[0] = share[0];
//	input.elem2[0] = share[1];
//
//	if(the_party->SkewDec(num_of_bit, input, output))
//	{
//		for(int i = 0; i < num_of_bit; i++)
//		{
//			output_shares[3 * i][0] = output.elem1[3 * i];
//			output_shares[3 * i][1] = output.elem2[3 * i];
//			output_shares[3 * i + 1][0] = output.elem1[3 * i + 1];
//			output_shares[3 * i + 1][1] = output.elem2[3 * i + 1];
//			output_shares[3 * i + 2][0] = output.elem1[3 * i + 2];
//			output_shares[3 * i + 2][1] = output.elem2[3 * i + 2];
//		}
//		syslog(LOG_INFO, "spdz2_ext_processor_Z2n::skew_dec: success");
//		return 0;
//	}
//	syslog(LOG_ERR, "spdz2_ext_processor_Z2n::skew_dec: protocol skew_decomposition is failure.");
//
//	return -1;
//}
//
//int spdz2_ext_processor_Z2n::skew_cmp(SPDZEXT_VALTYPE input_shares[][2], const int ring_size, SPDZEXT_VALTYPE * output_share) //input_shares[num_of_bit][2]
//{
//	syslog(LOG_INFO, "spdz2_ext_processor_Z2n::post_rec");
//	Z2nShareReplicated<SPDZEXT_VALTYPE> input(ring_size), output(1);
//
//	for(int i = 0; i < ring_size; i++)
//	{
//		input.elem1[i] = input_shares[i][0];
//		input.elem2[i] = input_shares[i][1];
//	}
//
//	if(the_party->PostRec(ring_size, input, output))
//	{
//		output_share[0] = output.elem1[0];
//		output_share[1] = output.elem2[0];
//
//		syslog(LOG_INFO, "spdz2_ext_processor_Z2n::post_rec: success");
//		return 0;
//	}
//	syslog(LOG_ERR, "spdz2_ext_processor_Z2n::post_rec: protocol post_re-composition is failure.");
//
//	return -1;
//}


int spdz2_ext_processor_Z2n::skew_dec(SPDZEXT_VALTYPE * share, const int num_of_bit, SPDZEXT_VALTYPE * output_shares, int ring_size_in, int ring_size_out) //output_shares[num_of_bit * 3][2]
{
//	syslog(LOG_INFO, "spdz2_ext_processor_Z2n::skew_dec: (s)%lu + (c)%x", *share, num_of_bit);
//	size_t batch_size_bits = static_cast<int>(ceil((float)batch_size / (8*sizeof(SPDZEXT_VALTYPE))));
//	vector<SPDZEXT_VALTYPE> input(1 * 2 * batch_size), output(2 * 3 * num_of_bit * batch_size_bits);
//
//	for(int i = 0; i<batch_size; i++)
//	{
//		input[i] = share[i];
//		input[batch_size+i] = share[batch_size+i];
//		cout << "[spdz2_ext_processor_Z2n.cpp] input[" << i << "] = " <<  input[i] << endl;
//		cout << "[spdz2_ext_processor_Z2n.cpp] input[" << batch_size+i << "] = " <<  input[batch_size+i] << endl;
//	}
//
//	if(the_party->SkewDec(num_of_bit, input, output))
//	{
//		for(int i = 0; i < num_of_bit; i++)
//		{
//			for(size_t j = 0; j < batch_size_bits; j++)
//			{
//				output_shares[6 * i * batch_size_bits + j]                       = output[6 * i * batch_size_bits + j];
//				output_shares[6 * i * batch_size_bits + batch_size_bits + j]     = output[6 * i * batch_size_bits + batch_size_bits + j];
//				output_shares[6 * i * batch_size_bits + 2 * batch_size_bits + j] = output[6 * i * batch_size_bits + 2 * batch_size_bits + j];
//				output_shares[6 * i * batch_size_bits + 3 * batch_size_bits + j] = output[6 * i * batch_size_bits + 3 * batch_size_bits + j];
//				output_shares[6 * i * batch_size_bits + 4 * batch_size_bits + j] = output[6 * i * batch_size_bits + 4 * batch_size_bits + j];
//				output_shares[6 * i * batch_size_bits + 5 * batch_size_bits + j] = output[6 * i * batch_size_bits + 5 * batch_size_bits + j];
//			}
//		}
//	//		syslog(LOG_INFO, "spdz2_ext_processor_Z2n::skew_dec: success");
//		return 0;
//	}
//	syslog(LOG_ERR, "spdz2_ext_processor_Z2n::skew_dec: protocol skew_decomposition is failure.");
//
//	return -1;

	syslog(LOG_INFO, "spdz2_ext_processor_Z2n::skew_dec: (s)%lu + (c)%x", *share, num_of_bit);
	size_t batch_size_bits = static_cast<int>(ceil((float)batch_size / (8*sizeof(SPDZEXT_VALTYPE))));
	Z2nShareReplicated<SPDZEXT_VALTYPE> input(batch_size);
	Z2nShareReplicated<SPDZEXT_VALTYPE> output1(num_of_bit*batch_size_bits),output2(num_of_bit*batch_size_bits),output3(num_of_bit*batch_size_bits);
	for (int i=0; i<batch_size; i++) {
		input.elem1[i] = share[i];
		input.elem2[i] = share[batch_size+i];
	}

//	if(the_party->SkewDec(num_of_bit, input, output))
	if(the_party->SkewDec(num_of_bit, input, output1, output2, output3))
	{
		for(int i = 0; i < num_of_bit; i++)
		{
			for (int j=0; j<batch_size_bits; j++) {
				//			output_shares[6 * i] = output.elem1[3 * i];
				//			output_shares[6 * i + 1] = output.elem2[3 * i];
				//			output_shares[6 * i + 2] = output.elem1[3 * i + 1];
				//			output_shares[6 * i + 3] = output.elem2[3 * i + 1];
				//			output_shares[6 * i + 4] = output.elem1[3 * i + 2];
				//			output_shares[6 * i + 5] = output.elem2[3 * i + 2];
//				cout << "[spdz2_ext_processor_Z2n.cpp] x1["<<i<<"] = " << output1.elem1[i*batch_size_bits + j] << ", " << output1.elem2[i*batch_size_bits + j] <<endl;
//				cout << "[spdz2_ext_processor_Z2n.cpp] x2["<<i<<"] = " << output2.elem1[i*batch_size_bits + j] << ", " << output2.elem2[i*batch_size_bits + j] <<endl;
//				cout << "[spdz2_ext_processor_Z2n.cpp] x3["<<i<<"] = " << output3.elem1[i*batch_size_bits + j] << ", " << output3.elem2[i*batch_size_bits + j] <<endl;
				output_shares[6*i*batch_size_bits + j] = output1.elem1[i*batch_size_bits + j];
				output_shares[6*i*batch_size_bits + 1*batch_size_bits + j] = output1.elem2[i*batch_size_bits + j];
				output_shares[6*i*batch_size_bits + 2*batch_size_bits + j] = output2.elem1[i*batch_size_bits + j];
				output_shares[6*i*batch_size_bits + 3*batch_size_bits + j] = output2.elem2[i*batch_size_bits + j];
				output_shares[6*i*batch_size_bits + 4*batch_size_bits + j] = output3.elem1[i*batch_size_bits + j];
				output_shares[6*i*batch_size_bits + 5*batch_size_bits + j] = output3.elem2[i*batch_size_bits + j];
			}
		}
//		syslog(LOG_INFO, "spdz2_ext_processor_Z2n::skew_dec: success");
		return 0;
	}
	syslog(LOG_ERR, "spdz2_ext_processor_Z2n::skew_dec: protocol skew_decomposition is failure.");

	return -1;
}

int spdz2_ext_processor_Z2n::post_rec(u_int64_t * input_shares, const int ring_size, u_int64_t * output_share) //input_shares[num_of_bit][2]
{
//	syslog(LOG_INFO, "spdz2_ext_processor_Z2n::post_rec");
	Z2nShareReplicated<SPDZEXT_VALTYPE> input(ring_size), output(1);

	for(int i = 0; i < ring_size; i++)
	{
		input.elem1[i] = input_shares[2*i];
		input.elem2[i] = input_shares[2*i+1];
	}

	if(the_party->PostRec(ring_size, input, output))
	{
		output_share[0] = output.elem1[0];
		output_share[1] = output.elem2[0];

//		syslog(LOG_INFO, "spdz2_ext_processor_Z2n::post_rec: success");
		return 0;
	}
	syslog(LOG_ERR, "spdz2_ext_processor_Z2n::post_rec: protocol post_re-composition is failure.");

	return -1;
}

int spdz2_ext_processor_Z2n::skew_inj(u_int64_t * share, u_int64_t output_shares[][2]) //output_shares[3][2]
{
	syslog(LOG_INFO, "spdz2_ext_processor_Z2n::skew_inj: (s)%lu ", *share );
	Z2nShareReplicated<SPDZEXT_VALTYPE> input(1), output(3);
	input.elem1[0] = share[0];
	input.elem2[0] = share[1];

	if(the_party->SkewInj(input, output))
	{
		output_shares[0][0] = output.elem1[0];
		output_shares[0][1] = output.elem2[0];
		output_shares[1][0] = output.elem1[1];
		output_shares[1][1] = output.elem2[1];
		output_shares[2][0] = output.elem1[2];
		output_shares[2][1] = output.elem2[2];
		syslog(LOG_INFO, "spdz2_ext_processor_Z2n::skew_inj: success");
		return 0;
	}
	syslog(LOG_ERR, "spdz2_ext_processor_Z2n::skew_inj: protocol skew_injection is failure.");

	return -1;
}

int spdz2_ext_processor_Z2n::skew_rec(u_int64_t * share, u_int64_t output_shares[][2]) //output_shares[3][2]
{
//	syslog(LOG_INFO, "spdz2_ext_processor_Z2n::skew_rec: (s)%lu ", *share );
	Z2nShareReplicated<SPDZEXT_VALTYPE> input(1), output(3);
	input.elem1[0] = share[0];
	input.elem2[0] = share[1];

	if(the_party->SkewRec(input, output))
	{
		output_shares[0][0] = output.elem1[0];
		output_shares[0][1] = output.elem2[0];
		output_shares[1][0] = output.elem1[1];
		output_shares[1][1] = output.elem2[1];
		output_shares[2][0] = output.elem1[2];
		output_shares[2][1] = output.elem2[2];
//		syslog(LOG_INFO, "spdz2_ext_processor_Z2n::skew_rec: success");
		return 0;
	}
	syslog(LOG_ERR, "spdz2_ext_processor_Z2n::skew_rec: protocol skew_re-composition is failure.");

	return -1;
}

int spdz2_ext_processor_Z2n::init_protocol()
{
	the_party = new NecProtocolPartyRingFor3P<uint64_t>(party_id, offline_size, batch_size);
	the_party->init();

	/*
	if(!the_party->offline())
	{
		syslog(LOG_ERR, "spdz2_ext_processor_Z2n::init_protocol: protocol offline() failure.");
		return -1;
	}
	*/
	return 0;
}

void spdz2_ext_processor_Z2n::delete_protocol()
{
	delete the_party;
	the_party = NULL;
}

bool spdz2_ext_processor_Z2n::protocol_offline()
{
	//return the_party->offline();
	return 0;
}

bool spdz2_ext_processor_Z2n::protocol_open()
{
	bool op_open_success = false;

	int num_regs = shares.size()/(2*batch_size);

//	Z2nShareReplicated<SPDZEXT_VALTYPE> ext_shares_nec(num_regs);
//	Z2nIntReplicated<SPDZEXT_VALTYPE> ext_opens_nec(num_regs);
////	syslog(LOG_INFO, "spdz2_ext_processor_Z2n::protocol_open: calling open for %u shares", (u_int32_t)shares.size());
//
//	int cnt = 0;
//	for(std::vector<u_int64_t>::const_iterator i = shares.begin(); i != shares.end(); ++i)
//	{
//		if (cnt%2==0) {
//			ext_shares_nec.elem1[cnt/2] = (SPDZEXT_VALTYPE(*i));
//			cnt++;
//		}
//		else {
//			ext_shares_nec.elem2[cnt/2] = (SPDZEXT_VALTYPE(*i));
//			cnt++;
//		}
////		syslog(LOG_DEBUG, "spdz2_ext_processor_Z2n::protocol_open() share value %lu", *i);
//	}

	opens.clear();

//
//	the_party->openShares(ext_shares_nec.getNumOfShares(), ext_shares_nec, ext_opens_nec);
	if (the_party->openShares(num_regs, shares, opens) ) {
		shares.clear();
	}
	else {
		std::cerr << "spdz_ext_processor_cc_imp::exec_open: verify failed - no open values returned." << std::endl;
	}

	//if(!do_verify || the_party->verify())
//	if (true) // tentative: verify() is not implemeneted
//	{
//		for(int i=0; i<ext_opens_nec.getNumOfData(); i++)
//		{
//			opens.push_back(ext_opens_nec.elem[i]);
//		}
//	}
//	else
//	{
//		std::cerr << "spdz_ext_processor_cc_imp::exec_open: verify failed - no open values returned." << std::endl;
//	}

	op_open_success = true;

	return op_open_success;
}

bool spdz2_ext_processor_Z2n::protocol_triple()
{
	bool op_triple_success = false;

	*pa = 0;
	*pb = 0;
	*pc = 0;

	/* Not implemented yet
	 *
	std::vector<ZpMersenneLongElement> triple(3);
	if(op_triple_success = the_party->triples(1, triple))
	{
		*pa = triple[0].elem;
		*pb = triple[1].elem;
		*pc = triple[2].elem;
		syslog(LOG_DEBUG, "spdz2_ext_processor_Z2n::protocol_triple: share a = %lu; share b = %lu; share c = %lu;", *pa, *pb, *pc);
	}
	*/
	op_triple_success = true; // tentative

	return op_triple_success;
}

bool spdz2_ext_processor_Z2n::protocol_input()
{
	bool op_input_success = false;
//	the_party->getInputFromOneParty(input_party_id, p_input_value);
	op_input_success = the_party->shareInput(input_party_id, num_input, p_input_value, p_output_value);
	return op_input_success;
}

bool spdz2_ext_processor_Z2n::protocol_input_asynch()
{
	bool op_input_asynch_success = false;
	input_values.clear();
	input_values.resize(num_of_inputs, 0);

	op_input_asynch_success = true; // tentative

	return op_input_asynch_success;
}

bool spdz2_ext_processor_Z2n::protocol_mult()
{
	bool op_mult_success = false;

	// original (start)
	//	size_t xy_pair_count = mult_values.size()/4;
	// original (end)

	// added (start)
	size_t xy_pair_count = mult_values.size()/(4*batch_size);
	// added (end)

	// original (start)
//	Z2nShareReplicated<SPDZEXT_VALTYPE> x_shares(xy_pair_count), y_shares(xy_pair_count), xy_shares(xy_pair_count);
	// original (end)

//	for(size_t i = 0; i < xy_pair_count; ++i)
//	{
//		x_shares.elem1[i] = mult_values[4*i];
//		x_shares.elem2[i] = mult_values[4*i+1];
//		y_shares.elem1[i] = mult_values[4*i+2];
//		y_shares.elem2[i] = mult_values[4*i+3];
//
////		syslog(LOG_DEBUG, "spdz_ext_processor_cc_imp::exec_mult: X-Y pair %lu: X=(%lu,%lu) Y=(%lu,%lu)", i, x_shares.elem1[i], x_shares.elem2[i], y_shares.elem1[i], y_shares.elem2[i]);
//	}

//	if((op_mult_success = the_party->multShares(xy_pair_count, x_shares, y_shares, xy_shares)))
//	{
//		for(size_t i = 0; i < xy_pair_count; ++i)
//		{
//			products.push_back(xy_shares.elem1[i]);
//			products.push_back(xy_shares.elem2[i]);
////			syslog(LOG_DEBUG, "spdz_ext_processor_cc_imp::exec_mult: X-Y product %lu: X*Y=(%lu,%lu)", i, products[2*i], products[2*i+1]);
//		}
//	}
//	else
//	{
//		syslog(LOG_ERR, "spdz_ext_processor_cc_imp::exec_mult: protocol mult failure.");
//	}

	std::vector<SPDZEXT_VALTYPE> x_shares(2 * xy_pair_count * batch_size), y_shares(2 * xy_pair_count * batch_size), xy_shares(2 * xy_pair_count * batch_size);
	for(size_t i = 0; i < xy_pair_count; ++i) {
		for(size_t j = 0; j < batch_size; ++j) {
			x_shares[2*batch_size*i+j]            = mult_values[4*batch_size*i+j];
			x_shares[2*batch_size*i+batch_size+j] = mult_values[4*batch_size*i+batch_size+j];
			y_shares[2*batch_size*i+j]            = mult_values[4*batch_size*i+2*batch_size+j];
			y_shares[2*batch_size*i+batch_size+j] = mult_values[4*batch_size*i+3*batch_size+j];
		}
	}


	if((op_mult_success = the_party->multShares(xy_pair_count, x_shares, y_shares, xy_shares)))
	{
		products = xy_shares;
		//	syslog(LOG_DEBUG, "spdz_ext_processor_cc_imp::exec_mult: X-Y product %lu: X*Y=(%lu,%lu)", i, products[2*i], products[2*i+1]);
	}
	else
	{
		syslog(LOG_ERR, "spdz_ext_processor_cc_imp::exec_mult: protocol mult failure.");
	}

	return op_mult_success;
}

bool spdz2_ext_processor_Z2n::protocol_share_immediates()
{
	bool op_share_immediates_success = false;
	/* Not implemented yet
	 *
	size_t value_count =  immediates_values.size();
	std::vector<ZpMersenneLongElement> shares(value_count);

	if(op_share_immediates_success = the_party->load_share_immediates(0, shares, immediates_values))
	{
		for(size_t i = 0; i < value_count; ++i)
		{
			immediates_shares.push_back(shares[i].elem);
			syslog(LOG_DEBUG, "spdz2_ext_processor_Z2n::protocol_share_immediates: share[%lu] = %lu", i, immediates_shares[i]);
		}
	}
	else
	{
		syslog(LOG_ERR, "spdz2_ext_processor_Z2n::protocol_share_immediates: protocol share_immediates failure.");
	}
	*/

	op_share_immediates_success = true; //tentative
	return op_share_immediates_success;
}

bool spdz2_ext_processor_Z2n::protocol_share_immediate()
{
	bool op_share_immediate_success = false;

	Z2nIntReplicated<SPDZEXT_VALTYPE> values(1);
	Z2nShareReplicated<SPDZEXT_VALTYPE> shares(1);

	values.elem[0] = immediate_value[0];

	if ((op_share_immediate_success = the_party->load_share_immediate(values, shares))) {
		p_immediate_share[0] = shares.elem1[0];
		p_immediate_share[1] = shares.elem2[0];
//		syslog(LOG_INFO, "spdz2_ext_processor_Z2n::protocol_share_immediate: share value %lu", *p_immediate_share);
	}
	else {
		syslog(LOG_ERR, "spdz2_ext_processor_Z2n::protocol_share_immediate: protocol load_share_immediates failure.");
	}

	return op_share_immediate_success;
}
