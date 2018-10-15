#include "spdz2_ext_processor_Z2.h"

#include <syslog.h>
#include <math.h>

spdz2_ext_processor_Z2::spdz2_ext_processor_Z2(const int batch_size)
 : spdz2_ext_processor_base()
 , the_party(NULL), batch_size(batch_size)
{
}

spdz2_ext_processor_Z2::~spdz2_ext_processor_Z2()
{
}

int spdz2_ext_processor_Z2::mix_add(u_int64_t * share, u_int64_t scalar)
{

//	syslog(LOG_INFO, "spdz2_ext_processor_Z2::mix_add: (s)%lu + (c)%lu", *share, scalar);
	Z2nShareReplicated<SPDZEXT_VALTYPE> input(1), output(1);
	input.elem1[0] = share[0];
	input.elem2[0] = share[1];

	if(the_party->addM(1, input, scalar, output))
	{
		share[0] = output.elem1[0];
		share[1] = output.elem2[0];
//		syslog(LOG_INFO, "spdz2_ext_processor_Z2::mix_add: result = (s)%lu, %lu", share[0], share[1]);
		return 0;
	}
	syslog(LOG_ERR, "spdz2_ext_processor_Z2::mix_add: protocol addShareAndScalar failure.");

	return -1;
}

int spdz2_ext_processor_Z2::si_add(u_int64_t * share, u_int64_t scalar)
{

//	syslog(LOG_INFO, "spdz2_ext_processor_Z2::si_add: (s)%lu + (c)%lu", *share, scalar);
	Z2nShareReplicated<SPDZEXT_VALTYPE> input(1), output(1);
	input.elem1[0] = share[0];
	input.elem2[0] = share[1];

	if(the_party->addSI(1, input, scalar, output))
	{
		share[0] = output.elem1[0];
		share[1] = output.elem2[0];
//		syslog(LOG_INFO, "spdz2_ext_processor_Z2::mix_si: result = (s)%lu, %lu", share[0], share[1]);
		return 0;
	}
	syslog(LOG_ERR, "spdz2_ext_processor_Z2::mix_si: protocol addShareAndScalar failure.");

	return -1;
}

int spdz2_ext_processor_Z2::mix_sub_scalar(u_int64_t * share, u_int64_t scalar)
{

//	syslog(LOG_INFO, "spdz2_ext_processor_Z2::mix_sub_scalar: (s)%lu - (c)%lu", *share, scalar);
	Z2nShareReplicated<SPDZEXT_VALTYPE> input(1), output(1);
	input.elem1[0] = share[0];
	input.elem2[0] = share[1];

	if(the_party->subML(1, input, scalar, output))
	{
		share[0] = output.elem1[0];
		share[1] = output.elem2[0];
//		syslog(LOG_INFO, "spdz2_ext_processor_Z2::mix_sub_scalar: result = (s)%lu", *share);
		return 0;
	}
	syslog(LOG_ERR, "spdz2_ext_processor_Z2::mix_sub_scalar: protocol shareSubScalar failure.");

	return -1;
}

int spdz2_ext_processor_Z2::mix_sub_share(u_int64_t scalar, u_int64_t * share)
{

//	syslog(LOG_INFO, "spdz2_ext_processor_Z2::mix_sub_share: (c)%lu - (s)%lu", scalar, *share);
	Z2nShareReplicated<SPDZEXT_VALTYPE> input(1), output(1);
	input.elem1[0] = share[0];
	input.elem2[0] = share[1];

	if(the_party->subMR(1, scalar, input, output))
	{
		share[0] = output.elem1[0];
		share[1] = output.elem2[0];
//		syslog(LOG_INFO, "spdz2_ext_processor_Z2::mix_sub_share: result = (s)%lu", *share);
		return 0;
	}
	syslog(LOG_ERR, "spdz2_ext_processor_Z2::mix_sub_share: protocol shareSubScalar failure.");

	return -1;
}

int spdz2_ext_processor_Z2::mix_mul(u_int64_t * share, u_int64_t scalar)
{

//	syslog(LOG_INFO, "spdz2_ext_processor_Z2::mix_add: (s)%lu + (c)%lu", *share, scalar);
	Z2nShareReplicated<SPDZEXT_VALTYPE> input(1), output(1);
	input.elem1[0] = share[0];
	input.elem2[0] = share[1];

	if(the_party->mulM(1, input, scalar, output))
	{
		share[0] = output.elem1[0];
		share[1] = output.elem2[0];
//		syslog(LOG_INFO, "spdz2_ext_processor_Z2::mix_add: result = (s)%lu, %lu", share[0], share[1]);
		return 0;
	}
	syslog(LOG_ERR, "spdz2_ext_processor_Z2::mix_add: protocol addShareAndScalar failure.");

	return -1;
}

int spdz2_ext_processor_Z2::si_mul(u_int64_t * share, u_int64_t scalar)
{

//	syslog(LOG_INFO, "spdz2_ext_processor_Z2::si_mul: (s)%lu + (c)%lu", *share, scalar);
	Z2nShareReplicated<SPDZEXT_VALTYPE> input(1), output(1);
	input.elem1[0] = share[0];
	input.elem2[0] = share[1];

	if(the_party->mulSI(1, input, scalar, output))
	{
		share[0] = output.elem1[0];
		share[1] = output.elem2[0];
//		syslog(LOG_INFO, "spdz2_ext_processor_Z2::si_mul: result = (s)%lu, %lu", share[0], share[1]);
		return 0;
	}
	syslog(LOG_ERR, "spdz2_ext_processor_Z2::si_mul: protocol mulShareAndImmediateScalar failure.");

	return -1;
}

int spdz2_ext_processor_Z2::skew_dec(u_int64_t * share, const int num_of_bit, u_int64_t * output_shares, int ring_size_in, int ring_size_out) //output_shares[num_of_bit * 3][2]
{
	syslog(LOG_INFO, "spdz2_ext_processor_Z2::skew_dec: (s)%lu + (c)%x", *share, num_of_bit);
	size_t batch_size_bits = static_cast<int>(ceil((float)batch_size / (8*sizeof(SPDZEXT_VALTYPE))));
	Z2nShareReplicated<SPDZEXT_VALTYPE> input(batch_size_bits);
	Z2nShareReplicated<SPDZEXT_VALTYPE> output1,output2,output3;
	if(ring_size_out != 1)
	{
		output1.init(batch_size);
		output2.init(batch_size);
		output3.init(batch_size);
	}
	else
	{
		output1.init(batch_size_bits);
		output2.init(batch_size_bits);
		output3.init(batch_size_bits);
	}

	for (size_t i=0; i<batch_size_bits; i++)
	{
		input.elem1[i] = share[i];
		input.elem2[i] = share[batch_size_bits+i];
	}

	if(ring_size_out != 1)
	{
		if(the_party->SkewInj(input, output1, output2, output3, ring_size_out))
		{
			for(size_t i = 0; i < batch_size; i++)
			{
				output_shares[               i] = output1.elem1[i];
				output_shares[1*batch_size + i] = output1.elem2[i];
				output_shares[2*batch_size + i] = output2.elem1[i];
				output_shares[3*batch_size + i] = output2.elem2[i];
				output_shares[4*batch_size + i] = output3.elem1[i];
				output_shares[5*batch_size + i] = output3.elem2[i];
			}
		//		syslog(LOG_INFO, "spdz2_ext_processor_Z2::skew_dec: success");
			return 0;
		}
	}
	else
	{
		if(the_party->SkewInj(input, output1, output2, output3, ring_size_out))
		{
			for(size_t i = 0; i < batch_size_bits; i++)
			{
				output_shares[                    i] = output1.elem1[i];
				output_shares[1*batch_size_bits + i] = output1.elem2[i];
				output_shares[2*batch_size_bits + i] = output2.elem1[i];
				output_shares[3*batch_size_bits + i] = output2.elem2[i];
				output_shares[4*batch_size_bits + i] = output3.elem1[i];
				output_shares[5*batch_size_bits + i] = output3.elem2[i];
			}
		//		syslog(LOG_INFO, "spdz2_ext_processor_Z2::skew_dec: success");
			return 0;
		}
	}
	syslog(LOG_ERR, "spdz2_ext_processor_Z2::skew_dec: protocol skew_decomposition is failure.");

	return -1;

////	syslog(LOG_INFO, "spdz2_ext_processor_Z2::skew_dec: (s)%lu + (c)%x", *share, num_of_bit);
//	Z2nShareReplicated<SPDZEXT_VALTYPE> input(1), output(3);
//	input.elem1[0] = share[0];
//	input.elem2[0] = share[1];
//
//	if(the_party->SkewInj(input, output, ring_size_out))
//	{
//		for(int i = 0; i < num_of_bit; i++)
//		{
//			output_shares[6 * i] = output.elem1[3 * i];
//			output_shares[6 * i + 1] = output.elem2[3 * i];
//			output_shares[6 * i + 2] = output.elem1[3 * i + 1];
//			output_shares[6 * i + 3] = output.elem2[3 * i + 1];
//			output_shares[6 * i + 4] = output.elem1[3 * i + 2];
//			output_shares[6 * i + 5] = output.elem2[3 * i + 2];
//		}
////		syslog(LOG_INFO, "spdz2_ext_processor_Z2::skew_dec: success");
//		return 0;
//	}
//	syslog(LOG_ERR, "spdz2_ext_processor_Z2::skew_dec: protocol skew_decomposition is failure.");
//
//	return -1;
}

int spdz2_ext_processor_Z2::post_rec(u_int64_t * input_shares, const int ring_size, u_int64_t * output_share) //input_shares[ring_size][2]
{
	size_t batch_size_bits = static_cast<int>(ceil((float)batch_size / (8*sizeof(SPDZEXT_VALTYPE))));
	Z2nShareReplicated<SPDZEXT_VALTYPE> input(ring_size*batch_size_bits);
	Z2nShareReplicated<SPDZEXT_VALTYPE> output(batch_size);

	for(size_t i = 0; i < ring_size; i++)
	{
		for(size_t j = 0; j < batch_size_bits; j++)
		{
			input.elem1[i] = input_shares[2 * i * batch_size_bits + j];
			input.elem2[i] = input_shares[2 * i * batch_size_bits + batch_size_bits + j];
		}
	}

	if(the_party->PostRec(ring_size, input, output))
	{
		for(size_t i = 0; i<batch_size; i++)
		{
			output_share[i] = output.elem1[i];
			output_share[batch_size + i] = output.elem2[i];
		}
		return 0;
	}
	syslog(LOG_ERR, "spdz2_ext_processor_Z2::post_rec: protocol post_re-composition is failure.");

	return -1;

//	//	syslog(LOG_INFO, "spdz2_ext_processor_Z2::post_rec");
//	Z2nShareReplicated<SPDZEXT_VALTYPE> input(ring_size);
//	// Z2nShareReplicated<SPDZEXT_VALTYPE> output(ring_size);
//	Z2nShareReplicated<SPDZEXT_VALTYPE> output(1);
//
//	// cout << "before input.initialize:" << endl;
//	for(int i = 0; i < ring_size; i++)
//	{
//		input.elem1[i] = input_shares[2*i];
//		input.elem2[i] = input_shares[2*i+1];
//	}
//	// output.elem1[0] = 0;
//	// output.elem2[0] = 0;
//	// cout << "after input.initialize:" << endl;
//	if(the_party->PostRec(ring_size, input, output))
//	{
//		// cout << "after PostRec:" << endl;
//
//		// cout << "output.elem1[0]" << endl;
//		// cout << output.elem1[0] << endl;
//
//		// output_share[0] = output.elem1[ring_size - 1];
//		output_share[0] = output.elem1[0];
//
//		// cout << "output_share[0]" << endl;
//		// cout << output_share[0] << endl;
//
//		// output_share[1] = output.elem2[ring_size - 1];
//		output_share[1] = output.elem2[0];
//
//		// cout << "after output_share:" << endl;
////		syslog(LOG_INFO, "spdz2_ext_processor_Z2::post_rec: success");
//		// cout << "after test:" << endl;
//		return 0;
//	}
//	syslog(LOG_ERR, "spdz2_ext_processor_Z2::post_rec: protocol post_re-composition is failure.");
//
//	return -1;
}

int spdz2_ext_processor_Z2::skew_inj(u_int64_t * share, u_int64_t output_shares[][2]) //output_shares[3][2]
{
//	syslog(LOG_INFO, "spdz2_ext_processor_Z2::skew_inj: (s)%lu ", *share);
	Z2nShareReplicated<SPDZEXT_VALTYPE> input(1), output(3);
	input.elem1[0] = share[0];
	input.elem2[0] = share[1];

	if(the_party->SkewInj(input, output, sizeof(SPDZEXT_VALTYPE) * 8))
	{
		output_shares[0][0] = output.elem1[0];
		output_shares[0][1] = output.elem2[0];
		output_shares[1][0] = output.elem1[1];
		output_shares[1][1] = output.elem2[1];
		output_shares[2][0] = output.elem1[2];
		output_shares[2][1] = output.elem2[2];

//		syslog(LOG_INFO, "spdz2_ext_processor_Z2::skew_inj: success");
		return 0;
	}
	syslog(LOG_ERR, "spdz2_ext_processor_Z2::skew_inj: protocol skew_injection is failure.");

	return -1;
}

int spdz2_ext_processor_Z2::skew_rec(u_int64_t * share, u_int64_t output_shares[][2]) //output_shares[3][2]
{
//	syslog(LOG_INFO, "spdz2_ext_processor_Z2::skew_rec: (s)%lu ", *share);
	Z2nShareReplicated<SPDZEXT_VALTYPE> input(1), output(3);

	/*
	cout << "Z2: skew_rec (start)" << endl;
	cout << "share[0]:" << endl;
	cout << share[0] << endl;
	cout << "share[1]:" << endl;
	cout << share[1] << endl;
	*/

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

//		syslog(LOG_INFO, "spdz2_ext_processor_Z2::skew_rec: success");
		return 0;
	}
	syslog(LOG_ERR, "spdz2_ext_processor_Z2::skew_rec: protocol skew_re-composition is failure.");

	return -1;
}

int spdz2_ext_processor_Z2::init_protocol()
{
	the_party = new NecProtocolPartyBoolFor3P<uint64_t>(party_id, offline_size, batch_size);
	the_party->init();
	/*
	if(!the_party->offline())
	{
		syslog(LOG_ERR, "spdz2_ext_processor_Z2::init_protocol: protocol offline() failure.");
		return -1;
	}
	*/
	return 0;
}

void spdz2_ext_processor_Z2::delete_protocol()
{
	delete the_party;
	the_party = NULL;
}

bool spdz2_ext_processor_Z2::protocol_offline()
{
	//return the_party->offline();
	return 0;
}

bool spdz2_ext_processor_Z2::protocol_open()
{
	bool op_open_success = false;

	//original (start)
//	Z2nShareReplicated<SPDZEXT_VALTYPE> ext_shares_nec(shares.size()/2);
//	Z2nIntReplicated<SPDZEXT_VALTYPE> ext_opens_nec(shares.size()/2);
	//original (end)

	// added(start)
	int batch_size_bits = (int)ceil((float)batch_size/(8*sizeof(SPDZEXT_VALTYPE)));
	int num_regs = shares.size()/(2*batch_size_bits);
////
//	cout << "==================================" << endl;
//	cout << "[spdz2_ext_processor_Z2::protocol_open]"<< endl;
//	cout << "shares.size()      = " << shares.size() << endl;
//	cout << "batch_size_bits    = " << batch_size_bits << endl;
//	cout << "num_regs           = " << num_regs << endl;
//	cout << "==================================" << endl;
////


//	Z2nShareReplicated<SPDZEXT_VALTYPE> ext_shares_nec(num_regs);
//	Z2nIntReplicated<SPDZEXT_VALTYPE> ext_opens_nec(num_regs);
	// added(end)

//	syslog(LOG_INFO, "spdz2_ext_processor_Z2::protocol_open: calling open for %u shares", (u_int32_t)shares.size());

//	int cnt = 0;
//	for(std::vector<u_int64_t>::const_iterator i = shares.begin(); i != shares.end(); ++i)
//	{
//		if (cnt%2==0) {ext_shares_nec.elem1[cnt/2] = (SPDZEXT_VALTYPE(*i)); cnt++; }
//		else { ext_shares_nec.elem2[cnt/2] = (SPDZEXT_VALTYPE(*i)); cnt++; }
////		syslog(LOG_DEBUG, "spdz2_ext_processor_Z2::protocol_open() share value %lu", *i);
//	}
//
//	shares.clear();

//	the_party->openShares(ext_shares_nec.getNumOfShares(), ext_shares_nec, ext_opens_nec);
//
//	opens.clear();
//
//	//if(!do_verify || the_party->verify())
//	if (true) // tentative: verify() is not implemeneted
//	{
//		// std::cout << "spdz_ext_processor_cc_imp::exec_gopen: verify open for " << ext_opens_nec.getNumOfData() << std::endl;
//		for(int i=0; i<ext_opens_nec.getNumOfData(); i++)
//		{
//			opens.push_back(ext_opens_nec.elem[i]);
//		}
//	}
//	else
//	{
//		std::cerr << "spdz_ext_processor_cc_imp::exec_gopen: verify failed - no open values returned." << std::endl;
//	}

	//added (start)
	opens.clear();
	if (the_party->openShares(num_regs, shares, opens) ) {
		shares.clear();
	}
	else {
		std::cerr << "spdz_ext_processor_cc_imp::exec_open: verify failed - no open values returned." << std::endl;
	}
	//added (end)

	// debug (start)
//	cout << "[spdz2_ext_processor_Z2.cpp] opens[0] = " << opens[0] << endl;
	// debug (end)

	op_open_success = true;

	return op_open_success;
}

bool spdz2_ext_processor_Z2::protocol_triple()
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
		syslog(LOG_DEBUG, "spdz2_ext_processor_Z2::protocol_triple: share a = %lu; share b = %lu; share c = %lu;", *pa, *pb, *pc);
	}
	*/
	op_triple_success = true; // tentative

	return op_triple_success;
}

bool spdz2_ext_processor_Z2::protocol_input()
{
	bool op_input_success = false;
//	the_party->getInputFromOneParty(input_party_id, p_input_value);
//	op_input_success = true; //tentative
	op_input_success = the_party->shareInput(input_party_id, num_input, p_input_value, p_output_value);
	return op_input_success;
}

bool spdz2_ext_processor_Z2::protocol_input_asynch()
{
	bool op_input_asynch_success = false;
	input_values.clear();
	input_values.resize(num_of_inputs, 0);

	op_input_asynch_success = true; // tentative

	return op_input_asynch_success;
}

bool spdz2_ext_processor_Z2::protocol_mult()
{
	bool op_mult_success = false;
	// original (start)
//	size_t xy_pair_count = mult_values.size()/4;
    // original (end)


	//added (start)
//	cout << "[spdz_ext_processor_Z2.cpp] batch_size    = " << batch_size << endl;

	int batch_size_bits = (int)ceil((float)batch_size/(8*sizeof(SPDZEXT_VALTYPE)));

	//	cout << "[spdz_ext_processor_Z2.cpp] batch_size_bits    = " << batch_size_bits << endl;
//	cout << "[spdz_ext_processor_Z2.cpp] mult_values.size() = " << mult_values.size() << endl;

	size_t xy_pair_count = mult_values.size()/(4*batch_size_bits);

//	cout << "[spdz_ext_processor_Z2.cpp] xy_pair_count = " << xy_pair_count << endl;

	// added (end)

//	Z2nShareReplicated<SPDZEXT_VALTYPE> x_shares(xy_pair_count), y_shares(xy_pair_count), xy_shares(xy_pair_count);

	//	######## without packing, 64-bit communication per gate ##########

//	for(size_t i = 0; i < xy_pair_count; ++i)
//	{
//		x_shares.elem1[i] = mult_values[4*i];
//		x_shares.elem2[i] = mult_values[4*i+1];
//		y_shares.elem1[i] = mult_values[4*i+2];
//		y_shares.elem2[i] = mult_values[4*i+3];
////		syslog(LOG_DEBUG, "spdz_ext_processor_cc_imp::exec_mult: X-Y pair %lu: X=(%lu,%lu) Y=(%lu,%lu)", i, x_shares.elem1[i], x_shares.elem2[i], y_shares.elem1[i], y_shares.elem2[i]);
//	}
//
//	if((op_mult_success = the_party->multShares(xy_pair_count, x_shares, y_shares, xy_shares)))
//	{
//		for(size_t i = 0; i < xy_pair_count; ++i)
//		{
//			products.push_back(xy_shares.elem1[i]);
//			products.push_back(xy_shares.elem2[i]);
////			syslog(LOG_DEBUG, "spdz_ext_processor_cc_imp::exec_mult: X-Y product %lu: X*Y=(%lu,%lu)", i, products[2*i], products[2*i+1]);
//		}
//	}

	//	######## without packing, 8-bit communication per gate ##########

//	uint8_t x_shares_byte[2*xy_pair_count];
//	uint8_t y_shares_byte[2*xy_pair_count];
//	uint8_t xy_shares_byte[2*xy_pair_count];
//
//	for (size_t i=0; i<xy_pair_count; i++) {
//		x_shares_byte[2*i]   = (uint8_t)(mult_values[4*i]   & 0xFF);
//		x_shares_byte[2*i+1] = (uint8_t)(mult_values[4*i+1] & 0xFF);
//		y_shares_byte[2*i]   = (uint8_t)(mult_values[4*i+2] & 0xFF);
//		y_shares_byte[2*i+1] = (uint8_t)(mult_values[4*i+3] & 0xFF);
//	}
//
//	if((op_mult_success = the_party->multShares(xy_pair_count, x_shares_byte, y_shares_byte, xy_shares_byte)))
//	{
//		for(size_t i = 0; i < xy_pair_count; ++i)
//		{
//			products.push_back((SPDZEXT_VALTYPE)(xy_shares_byte[i]));
//			products.push_back((SPDZEXT_VALTYPE)(xy_shares_byte[xy_pair_count+i]));
////	 		cout << "xy_shares_byte = " << (uint32_t)xy_shares_byte[i] << ", " << (uint32_t)xy_shares_byte[xy_pair_count+i] << endl;
////			syslog(LOG_DEBUG, "spdz_ext_processor_cc_imp::exec_mult: X-Y product %lu: X*Y=(%lu,%lu)", i, products[2*i], products[2*i+1]);
//		}
//	}

//	######## with packing ##########

	int bufsize = xy_pair_count * batch_size_bits * 8;

	memset(&x1_buf[0], 0, bufsize);
	memset(&x2_buf[0], 0, bufsize);
	memset(&y1_buf[0], 0, bufsize);
	memset(&y2_buf[0], 0, bufsize);

	for (size_t i=0; i<xy_pair_count; ++i) {
		for (int j=0; j<batch_size_bits; j++) {
			memcpy(&x1_buf[0] + (i * batch_size_bits * 8) + j * 8, &mult_values[4 * i * batch_size_bits + j], sizeof(SPDZEXT_VALTYPE) );
			memcpy(&x2_buf[0] + (i * batch_size_bits * 8) + j * 8, &mult_values[4 * i * batch_size_bits + batch_size_bits + j], sizeof(SPDZEXT_VALTYPE) );
			memcpy(&y1_buf[0] + (i * batch_size_bits * 8) + j * 8, &mult_values[4 * i * batch_size_bits + 2 * batch_size_bits + j], sizeof(SPDZEXT_VALTYPE) );
			memcpy(&y2_buf[0] + (i * batch_size_bits * 8) + j * 8, &mult_values[4 * i * batch_size_bits + 3 * batch_size_bits + j], sizeof(SPDZEXT_VALTYPE) );
		}
	}

	if((op_mult_success = the_party->multShares(bufsize, x1_buf, x2_buf, y1_buf, y2_buf, xy1_buf, xy2_buf)))
	{
		for(size_t i = 0; i < xy_pair_count; i++)
		{
			for (int j=0; j<batch_size_bits; j++)
			{
				SPDZEXT_VALTYPE tmp_xy1 = 0;
				for(int k=0; k<8; k++)
				{
					tmp_xy1 ^= (SPDZEXT_VALTYPE)xy1_buf[(i * batch_size_bits * 8) + (j * 8) + k] << k*8;

				}
				products.push_back(tmp_xy1);
			}
			for (int j=0; j<batch_size_bits; j++)
			{
				SPDZEXT_VALTYPE tmp_xy2 = 0;
				for(int k=0; k<8; k++)
				{
					tmp_xy2 ^= (SPDZEXT_VALTYPE)xy2_buf[(i * batch_size_bits * 8) + (j * 8) + k] << k*8;

				}
				products.push_back(tmp_xy2);
			}
		}
	}
	else
	{
		syslog(LOG_ERR, "spdz_ext_processor_cc_imp::exec_mult: protocol mult failure.");
	}

	return op_mult_success;

	// original (start)
//	int bufsize = ceil((float)xy_pair_count/8);
//
//	memset(&x1_buf[0], 0, bufsize);
//	memset(&x2_buf[0], 0, bufsize);
//	memset(&y1_buf[0], 0, bufsize);
//	memset(&y2_buf[0], 0, bufsize);
//
//	for (size_t i=0; i<xy_pair_count; i++) {
//		x1_buf[i/8] ^= (mult_values[4*i]   & 0x1) << (i%8);
//		x2_buf[i/8] ^= (mult_values[4*i+1] & 0x1) << (i%8);
//		y1_buf[i/8] ^= (mult_values[4*i+2] & 0x1) << (i%8);
//		y2_buf[i/8] ^= (mult_values[4*i+3] & 0x1) << (i%8);
//	}
//
//	if((op_mult_success = the_party->multShares(bufsize, x1_buf, x2_buf, y1_buf, y2_buf, xy1_buf, xy2_buf)))
//	{
//		for(size_t i = 0; i < xy_pair_count; ++i)
//		{
//			uint8_t tmpshare1 = xy1_buf[i/8] >> (i%8);
//			uint8_t tmpshare2 = xy2_buf[i/8] >> (i%8);
//			SPDZEXT_VALTYPE share1 = (SPDZEXT_VALTYPE)tmpshare1 & 0x1;
//			SPDZEXT_VALTYPE share2 = (SPDZEXT_VALTYPE)tmpshare2 & 0x1;
//			products.push_back(share1);
//			products.push_back(share2);
//		}
//	}
//
//	else
//	{
//		syslog(LOG_ERR, "spdz_ext_processor_cc_imp::exec_mult: protocol mult failure.");
//	}
//
//	return op_mult_success;
	// original (end)
}

bool spdz2_ext_processor_Z2::protocol_share_immediates()
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
			syslog(LOG_DEBUG, "spdz2_ext_processor_Z2::protocol_share_immediates: share[%lu] = %lu", i, immediates_shares[i]);
		}
	}
	else
	{
		syslog(LOG_ERR, "spdz2_ext_processor_Z2::protocol_share_immediates: protocol share_immediates failure.");
	}
	*/

	op_share_immediates_success = true; //tentative
	return op_share_immediates_success;
}

bool spdz2_ext_processor_Z2::protocol_share_immediate()
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
