#include "spdzext.h"
#include <iostream>
#include <vector>

#include "spdz2_ext_processor_base.h"
#include "spdz2_ext_processor_Z2n.h"
#include "spdz2_ext_processor_Z2.h"

#include <syslog.h>
#include <assert.h>

#define NUM_HANDLE 2

//SPDZEXT_VALTYPE *open_buffer;
//vector<SPDZEXT_VALTYPE> open_buffer;
//vector<SPDZEXT_VALTYPE> mult_buffer;
vector<SPDZEXT_VALTYPE> * open_buffer;
vector<SPDZEXT_VALTYPE> * mult_buffer;
//void * open_dest;
//void * mult_dest;
void ** open_dest;
void ** mult_dest;

//SPDZEXT_VALTYPE *factors=NULL;
//SPDZEXT_VALTYPE *factors;
SPDZEXT_VALTYPE factors[4*MAX_SHARES];

typedef struct
{
	u_int64_t token;
	int party_id, num_of_parties, proc_id;
}context_t;

int init(MPC_CTX *ctx, const int party_id, const int num_of_parties,
			 const char * field, const int open_count, const int mult_count,
			 const int bits_count, const int batch_size)
{
	context_t * pctx = new context_t;

	spdz2_ext_processor_base * proc = NULL;
	if(strncmp(field, "Z2n_Ring", 8) == 0)
	{
		proc = new spdz2_ext_processor_Z2n(batch_size);
		pctx->proc_id = 0;
	}
	else if(strncmp(field, "Z2_Bool", 7) == 0)
	{
		proc = new spdz2_ext_processor_Z2(batch_size);
		pctx->proc_id = 1;
	}
	else
	{
		syslog(LOG_ERR, "SPDZ-2 extension library init: invalid field type [%s]", field);
		return -1;
	}

	if(0 != proc->start(party_id, num_of_parties, field, mult_count))
	{
		delete proc;
		return -2;
	}

	pctx->token = (u_int64_t)proc;
	pctx->party_id = party_id;
	pctx->num_of_parties = num_of_parties;
	ctx->handle = (u_int64_t)(pctx);

	if (open_buffer == NULL) {
		open_buffer = new vector<SPDZEXT_VALTYPE>[NUM_HANDLE];
	}

	if (mult_buffer == NULL) {
		mult_buffer = new vector<SPDZEXT_VALTYPE>[NUM_HANDLE];
	}

	if (open_dest == NULL) {
		open_dest = new void*[NUM_HANDLE];
	}

	if (mult_dest == NULL) {
		mult_dest = new void*[NUM_HANDLE];
	}
//	if (factors == NULL) {
//		cout << "[spdzext.cpp::init] allocate factors: start" << endl;
//		factors = new SPDZEXT_VALTYPE[4*MAX_SHARES];
//		cout << "[spdzext.cpp::init] allocate factors: end" << endl;
//	}

	return 0;
}

int term(MPC_CTX *ctx){
//
//	for (int i=0; i<NUM_HANDLE; i++) {
//		mult_buffer[i].clear();
//		open_buffer[i].clear();
//	}
//
//	delete [] mult_dest;
//	delete [] open_dest;
//	delete [] mult_buffer;
//	delete [] open_buffer;
//
//	mult_dest = NULL;
//	open_dest = NULL;
//	mult_buffer = NULL;
//	open_buffer = NULL;
//
//	spdz2_ext_processor_base * proc = ((spdz2_ext_processor_base *)ctx->handle);
//	proc->stop(20);
//	delete proc;

	// statistics comment out (start)
//	cout <<  "[spdzext] extension library is terminated. " << endl;
	// statistics comment out (end)

	return 0;
}

int skew_bit_decomp(MPC_CTX * ctx, const share_t * rings_in, share_t * bits_out) {
	int size = bits_out->count;
	int batch_size = rings_in->batch_size;
	int batch_size_bits = bits_out->batch_size;

	SPDZEXT_VALTYPE inshare[2*batch_size];
	SPDZEXT_VALTYPE outshare[2*size*batch_size_bits];
	int ring_size_in  = rings_in->md_ring_size;
	int ring_size_out = bits_out->md_ring_size;

	context_t *handle = (context_t *)ctx->handle;

	assert(bits_out->count % 3 == 0);

	int num_of_bits = bits_out->count/3;

	memcpy(&inshare[0],          rings_in->data,                                        batch_size * sizeof(SPDZEXT_VALTYPE));
	memcpy(&inshare[batch_size], rings_in->data + batch_size * sizeof(SPDZEXT_VALTYPE), batch_size * sizeof(SPDZEXT_VALTYPE));

	// debug (start)
//   for(int j = 0; j<batch_size; j++)
//	{
//		cout << "[spdzext.cpp] inshare[" <<  j << "] = " << inshare[j] << endl;
//		cout << "[spdzext.cpp] inshare[" << batch_size+j << "] = " << inshare[batch_size+j] << endl;
//	}
	// debug (end)

	int result = ((spdz2_ext_processor_base *)handle->token)->skew_dec(inshare, num_of_bits, outshare, ring_size_in, ring_size_out);

	// debug (start)
//	for(int i = 0; i<num_of_bits; i++)
//	{
//		for(int j = 0; j<batch_size_bits; j++)
//		{
//			cout << "[spdzext.cpp] "<< i << "-th x_1:" <<  endl;
//			cout << "[spdzext.cpp] outshare[" << 3*2*i*batch_size_bits +j << "] = " << outshare[3*2*i*batch_size_bits +j] << endl;
//			cout << "[spdzext.cpp] outshare[" << 3*2*i*batch_size_bits +batch_size_bits+j << "] = " << outshare[3*2*i*batch_size_bits +batch_size_bits+j] << endl;
//			cout << "[spdzext.cpp] "<< i << "-th x_2:" <<  endl;
//			cout << "[spdzext.cpp] outshare[" << 3*2*i*batch_size_bits+2*batch_size_bits+j << "] = " << outshare[3*2*i*batch_size_bits+2*batch_size_bits+j] << endl;
//			cout << "[spdzext.cpp] outshare[" << 3*2*i*batch_size_bits+3*batch_size_bits+j << "] = " << outshare[3*2*i*batch_size_bits +3*batch_size_bits+j] << endl;
//			cout << "[spdzext.cpp] "<< i << "-th x_3:" <<  endl;
//			cout << "[spdzext.cpp] outshare[" << 3*2*i*batch_size_bits+4*batch_size_bits+j << "] = " << outshare[3*2*i*batch_size_bits+4*batch_size_bits+j] << endl;
//			cout << "[spdzext.cpp] outshare[" << 3*2*i*batch_size_bits+5*batch_size_bits+j << "] = " << outshare[3*2*i*batch_size_bits +5*batch_size_bits+j] << endl;
//		}
//	}
	// debug (end)

//	cout << "[spdzext.cpp::skew_dec] start memcpy" << endl;

	memcpy(bits_out->data, outshare, sizeof(SPDZEXT_VALTYPE)*2*size*batch_size_bits);

//	cout << "[spdzext.cpp::skew_dec] end memcpy" << endl;


//	SPDZEXT_VALTYPE inshare[2];
//	int size = bits_out->count;
//	SPDZEXT_VALTYPE outshare[2*size];
//	int ring_size_in  = rings_in->md_ring_size;
//	int ring_size_out = bits_out->md_ring_size;
//
//	context_t *handle = (context_t *)ctx->handle;
//
//	assert(bits_out->count % 3 == 0);
//
//	int num_of_bits = bits_out->count/3;
//
//	memcpy(&inshare[0], rings_in->data,                           sizeof(SPDZEXT_VALTYPE));
//	memcpy(&inshare[1], rings_in->data + sizeof(SPDZEXT_VALTYPE), sizeof(SPDZEXT_VALTYPE));
//
//	int result = ((spdz2_ext_processor_base *)handle->token)->skew_dec(inshare, num_of_bits, outshare, ring_size_in, ring_size_out);
//
//	memcpy(bits_out->data, outshare, sizeof(SPDZEXT_VALTYPE)*2*size);
//
//	for (int i=0; i<2*size; i+=6) {
//		cout << "[spdzext.cpp::skew_bit_dec] outshare x1-1:" << i/6 << " = " << outshare[i] << endl;
//		cout << "[spdzext.cpp::skew_bit_dec] outshare x1-2:" << i/6 << " = " << outshare[i+1] << endl;
//		cout << "[spdzext.cpp::skew_bit_dec] outshare x2-1:" << i/6 << " = " << outshare[i+2] << endl;
//		cout << "[spdzext.cpp::skew_bit_dec] outshare x2-2:" << i/6 << " = " << outshare[i+3] << endl;
//		cout << "[spdzext.cpp::skew_bit_dec] outshare x3-1:" << i/6 << " = " << outshare[i+4] << endl;
//		cout << "[spdzext.cpp::skew_bit_dec] outshare x3-2:" << i/6 << " = " << outshare[i+5] << endl;
//	}

	return result;
}

int skew_ring_comp(MPC_CTX * ctx, const share_t * bits_in, share_t * rings_out) {
	assert(bits_in->count == rings_out->md_ring_size);
	context_t *handle = (context_t *)ctx->handle;

	int batch_size_bits = bits_in->batch_size;
	int batch_size = rings_out->batch_size;
	int num_of_bits = rings_out->md_ring_size;
	int size_in = bits_in->count * bits_in->size * batch_size_bits;
	int size_out = rings_out->count * rings_out->size * batch_size;

	SPDZEXT_VALTYPE inshare[size_in];
	SPDZEXT_VALTYPE outshare[size_out];

	memcpy(&inshare[0], bits_in->data, size_in);

	int result = ((spdz2_ext_processor_base *)handle->token)->post_rec(inshare, num_of_bits, outshare);

	memcpy(rings_out->data, &outshare[0], size_out);

	return result;

//	assert(bits_in->count == rings_out->md_ring_size);
//
//	context_t *handle = (context_t *)ctx->handle;
//
//	int num_of_bits = rings_out->md_ring_size;
//
////	cout << "[spdzext.cpp::skew_ring_comp] num_of_bits = " << num_of_bits << endl;
//
//	int size_in = bits_in->count * bits_in->size;
//	int size_out = rings_out->count * rings_out->size;
//
////	cout << "[spdzext.cpp::skew_ring_comp] inshare copy size  = " << size_in << endl;
////	cout << "[spdzext.cpp::skew_ring_comp] outshare copy size = " << size_out << endl;
//
//	SPDZEXT_VALTYPE inshare[2*num_of_bits];
//	SPDZEXT_VALTYPE outshare[2];
//
//	memcpy(&inshare[0], bits_in->data, bits_in->count * bits_in->size);
//
//	int result = ((spdz2_ext_processor_base *)handle->token)->post_rec(inshare, num_of_bits, outshare);
//
//	memcpy(rings_out->data, &outshare[0], rings_out->count * rings_out->size);
//
//	return result;

}

int input_party(MPC_CTX * ctx, int sharing_party_id, clear_t * rings_in, share_t * rings_out) {
	SPDZEXT_VALTYPE *input_values, *output_values;

	input_values  = new SPDZEXT_VALTYPE[rings_in->count * rings_in->batch_size];
	output_values = new SPDZEXT_VALTYPE[2*rings_out->count * rings_out->batch_size];

	for (int i = 0; i < rings_in->count * rings_in->batch_size; i++){
		memcpy(&input_values[i], rings_in->data + (i*rings_in->size), sizeof(SPDZEXT_VALTYPE));
	}

//	cout << "[spdzext.cpp] input_values[0] = " << input_values[0] << endl;

	int count = rings_in->count;

	context_t *handle = (context_t *)ctx->handle;
	int result = ((spdz2_ext_processor_base *)handle->token)->input(sharing_party_id, count, input_values, output_values);

	if (result != 0) {
		delete input_values;
		delete output_values;
		return -1;
	}

	// debug (start)
//	for(size_t i = 0; i < 2*rings_out->count * rings_out->batch_size; i++) {
//		cout << "[spdzext.cpp] output_values[" << i << "] = " << output_values[i] << endl;
//	}
	// debug (end)

//	for (int i=0; i<rings_out->count; i++) {
//		memcpy(rings_out->data + (i*rings_out->size),                           &output_values[2*i],   sizeof(SPDZEXT_VALTYPE));
//		memcpy(rings_out->data + (i*rings_out->size) + sizeof(SPDZEXT_VALTYPE), &output_values[2*i+1], sizeof(SPDZEXT_VALTYPE));
//	}

	SPDZEXT_VALTYPE *p_ro = (SPDZEXT_VALTYPE *)rings_out->data;
	for (int i=0; i<2*rings_out->count * rings_out->batch_size; i++) {
		p_ro[i] = output_values[i];
		// debug (start)
//		cout << "[spdzext.cpp] *p_ro[" << i << "] = " << p_ro[i] << endl;
		// debug (end)
	}

//	cout << endl;

	return 0;
}

int input_share(MPC_CTX * ctx, clear_t * rings_in, share_t *rings_out) {
	return 0;
}
int make_input_from_integer(MPC_CTX * ctx, u_int64_t * integers, int integers_count, clear_t * rings_out) {

	if((int)rings_out->count < integers_count || rings_out->size < sizeof(u_int64_t))
		return -1;

	int count = integers_count * rings_out->batch_size;

	for (int i=0; i<count; i++) {
		memcpy(rings_out->data + i*rings_out->size, &integers[i], sizeof(SPDZEXT_VALTYPE));
	}

//	cout << "=================================" << endl;
//	cout << "[spdzext::make_input_from_integer]"<< endl;
//	cout << "count = "<< count <<  endl;
//	cout << "integers = " << integers[0] << endl;
//	SPDZEXT_VALTYPE *p = (SPDZEXT_VALTYPE *)rings_out->data;
//	cout << "rings_out = " << p[0] << endl;
//	cout << "=================================" << endl;

	return 0;
}

int make_input_from_fixed(MPC_CTX * ctx, const char * fix_strs[], int fix_count, clear_t * rings_out) {
	return 0;
}

int start_open(MPC_CTX * ctx, const share_t * rings_in, clear_t * rings_out) {
	context_t *handle = (context_t *)ctx->handle;

	int count = rings_in->count;
	int batch_size = rings_in->batch_size;
	SPDZEXT_VALTYPE *rings_in_v;
	rings_in_v  = new SPDZEXT_VALTYPE[2*count*batch_size];
//	rings_out->size = sizeof(SPDZEXT_VALTYPE); // 1 share value of SPDZVALTYPE
//	rings_out->count = count;
//	rings_out->md_ring_size = rings_in->md_ring_size;

//	open_buffer = new SPDZEXT_VALTYPE[count];
	open_buffer[handle->proc_id].resize(count*batch_size);
	open_dest[handle->proc_id] = rings_out;
//	for (int i=0; i<count; i++) {
//		memcpy(&rings_in_v[2*i], rings_in->data + (i * rings_in->size), sizeof(SPDZEXT_VALTYPE));
//		memcpy(&rings_in_v[2*i+1], rings_in->data + (i * rings_in->size) + sizeof(SPDZEXT_VALTYPE), sizeof(SPDZEXT_VALTYPE));
//	}

	//	cout << "============================" << endl;

	SPDZEXT_VALTYPE *p_ri = (SPDZEXT_VALTYPE *)rings_in->data;
	for (int i=0; i<2*count*batch_size; i++) {
		rings_in_v[i] = p_ri[i];
	}

//	cout << "============================" << endl;
//	cout << "[spdzext::start_open]" << endl;
//	cout << "count      = "<< count << endl;
//	cout << "batch_size = "<< batch_size << endl;
//	cout << "============================" << endl;


	return ((spdz2_ext_processor_base *)handle->token)->start_open((2*count*batch_size), rings_in_v, &open_buffer[handle->proc_id][0], 0);
}

int stop_open(MPC_CTX * ctx) {
	context_t *handle = (context_t *)ctx->handle;
	int result = ((spdz2_ext_processor_base *)handle->token)->stop_open(20);
	clear_t *dp = (clear_t *)open_dest[handle->proc_id];
	int size = (dp->size)*(dp->count)*(dp->batch_size);
//	SPDZEXT_VALTYPE * destination = reinterpret_cast<SPDZEXT_VALTYPE *>(dp->data);
//	memcpy(destination, &open_buffer[handle->proc_id][0], size);

	memcpy(dp->data, &open_buffer[handle->proc_id][0], size);
//   cout << "[spdzext::stop_open] typeid(dp->data)  = " << typeid(dp->data).name() << endl;
//	cout << "[spdzext::stop_open] dp->size  = " << dp->size << endl;
//	cout << "[spdzext::stop_open] dp->count = " << dp->count << endl;
//	cout << "[spdzext::stop_open] size      = " << size << endl;
//	cout << "[spdzext::stop_open] VALTYPE   = " << sizeof(SPDZEXT_VALTYPE) << endl;
//	cout << "[spdzext::stop_open] buffer    = " << open_buffer[handle->proc_id][0] <<endl;
//	for(int i = 0; i < size; i++){
//		cout << "[spdzext::stop_open] dp->data[" << i << "]  = " << (SPDZEXT_VALTYPE)(*(dp->data+i)) <<endl;
//	}

	return result;
}

int make_integer_output(MPC_CTX * ctx, const share_t * rings_in, u_int64_t * integers, int * integers_count) {
	return 0;
}

int make_fixed_output(MPC_CTX * ctx, const share_t * rings_in, char * fix_strs[], int * fixed_count) {
	return 0;
}

int verify_optional_suggest(MPC_CTX * ctx, int * error) {
	return 0;
}

int verify_final(MPC_CTX * ctx, int * error) {
	return 0;
}

int start_mult(MPC_CTX * ctx, const share_t * factor1, const share_t * factor2, share_t * product) {
	context_t *handle = (context_t *)ctx->handle;

	int count = factor1->count;
	// added (start)
	int batch_size = factor1->batch_size;
	// added (end)
//	SPDZEXT_VALTYPE *factors;

//	assert(count <= MAX_SHARES); // Is it OK in the parallel processing?

	// added (start)
	assert(count * batch_size <= 4*MAX_SHARES);
	// added (end)

//	factors  = new SPDZEXT_VALTYPE[4*count]; // 4 = 2 (repricated) * 2 (factor1 + factor2)
//	if (factors == NULL) {
//		factors = new SPDZEXT_VALTYPE[4*MAX_SHARES];
//	}

//	product->size = 2 * sizeof(SPDZEXT_VALTYPE); // 2 share value of SPDZVALTYPE
//	product->count = count;
//	product->md_ring_size = factor1->md_ring_size;
//	// added (start)
//	product->batch_size = batch_size;
//	// added (end)

	// original (start)
	//	mult_buffer[handle->proc_id].resize(count * 2);
	// original (end)

	// added (start)
	mult_buffer[handle->proc_id].resize(count * batch_size * 2);
	// added (end)

	mult_dest[handle->proc_id] = product;

	SPDZEXT_VALTYPE *pf1 = (SPDZEXT_VALTYPE *)factor1->data;
	SPDZEXT_VALTYPE *pf2 = (SPDZEXT_VALTYPE *)factor2->data;

// original (start)
//	for (int i=0; i<count; i++) {
//		factors[4*i]   = pf1[2*i];
//		factors[4*i+1] = pf1[2*i+1];
//		factors[4*i+2] = pf2[2*i];
//		factors[4*i+3] = pf2[2*i+1];
////		cout << "##### spdzext::start_mult #####" << endl;
////		cout << "pf1 = " << pf1[2*i] << ", " << pf1[2*i+1] << endl;
////		cout << "pf2 = " << pf1[2*i] << ", " << pf1[2*i+1] << endl;
//
////		memcpy(&factors[4*i], factor1->data + (i * factor1->size), sizeof(SPDZEXT_VALTYPE));
////		memcpy(&factors[4*i+1], factor1->data + (i * factor1->size) + sizeof(SPDZEXT_VALTYPE), sizeof(SPDZEXT_VALTYPE));
////		memcpy(&factors[4*i+2], factor2->data + (i * factor2->size), sizeof(SPDZEXT_VALTYPE));
////		memcpy(&factors[4*i+3], factor2->data + (i * factor2->size) + sizeof(SPDZEXT_VALTYPE), sizeof(SPDZEXT_VALTYPE));
//	}
// original (end)

	// added (start)
	for (int i=0; i<count; i++) {
		for (int j=0; j<batch_size; j++) {
			factors[4*batch_size*i+j]              = pf1[2*batch_size*i+j];
			factors[4*batch_size*i+batch_size+j]   = pf1[2*batch_size*i+batch_size+j];
			factors[4*batch_size*i+2*batch_size+j] = pf2[2*batch_size*i+j];
			factors[4*batch_size*i+3*batch_size+j] = pf2[2*batch_size*i+batch_size+j];

			// debug(start)
//			cout << "[spdzext.cpp] batch_size = " << batch_size << endl;
//			cout << "[spdzext.cpp] count = " << count << endl;
//
//			cout << "[spdzext.cpp] i = " << i << endl;
//			cout << "[spdzext.cpp] j = " << j << endl;
//
//			cout << "[spdzext.cpp] factors[" << 4*batch_size*i+j << "] = " << factors[4*batch_size*i+j] << endl;
//			cout << "[spdzext.cpp] factors[" << 4*batch_size*i+batch_size+j << "] = " << factors[4*batch_size*i+batch_size+j] << endl;
//			cout << "[spdzext.cpp] factors[" << 4*batch_size*i+2*batch_size+j << "] = " << factors[4*batch_size*i+2*batch_size+j] << endl;
//			cout << "[spdzext.cpp] factors[" << 4*batch_size*i+3*batch_size+j << "] = " << factors[4*batch_size*i+3*batch_size+j] << endl;
//
//			cout << "[spdzext.cpp] pf1[" << 2*batch_size*i+j << "] = " << pf1[2*batch_size*i+j] << endl;
//			cout << "[spdzext.cpp] pf1[" << 2*batch_size*i+batch_size+j << "] = " << pf1[2*batch_size*i+batch_size+j] << endl;
//			cout << "[spdzext.cpp] pf2[" << 2*batch_size*i+j << "] = " << pf2[2*batch_size*i+j] << endl;
//			cout << "[spdzext.cpp] pf2[" << 2*batch_size*i+batch_size+j << "] = " << pf2[2*batch_size*i+batch_size+j] << endl;
			// debug(end)
		}
	}
	// added (end)

	// original (start)
//	return ((spdz2_ext_processor_base *)handle->token)->start_mult((4*count), factors, &mult_buffer[handle->proc_id][0], 0);
	// original (end)

	// added (start)
	return ((spdz2_ext_processor_base *)handle->token)->start_mult((4*count*batch_size), factors, &mult_buffer[handle->proc_id][0], 0);
	// added (end)
}

//int start_mult(MPC_CTX * ctx, const share_t * factor1, const share_t * factor2, share_t * product) {
//	return 0;
//}

int stop_mult(MPC_CTX * ctx) {
	context_t *handle = (context_t *)ctx->handle;

	int result = ((spdz2_ext_processor_base *)handle->token)->stop_mult(20);

	share_t *dp = (share_t *)mult_dest[handle->proc_id];
	memcpy(dp->data, &mult_buffer[handle->proc_id][0], dp->size*dp->count*dp->batch_size);

//	cout << "[spdzext.cpp] dp->size = " << dp->size << endl;
//	cout << "[spdzext.cpp] dp->count = " << dp->count << endl;
//	cout << "[spdzext.cpp] dp->batch_size = " << dp->batch_size << endl;

	return result;
}

//int stop_mult(MPC_CTX * ctx) {
//	return 0;
//}

////-------------------------------------------------------------------------------------------//
//int init(void ** handle, const int pid, const int num_of_parties, const char * field, const int offline_size)
//{
//	spdz2_ext_processor_base * proc = NULL;
//	if(strncmp(field, "Z2n_Ring", 8) == 0)
//	{
//		proc = new spdz2_ext_processor_Z2n;
//	}
//	else if(strncmp(field, "Z2_Bool", 7) == 0)
//	{
//		proc = new spdz2_ext_processor_Z2;
//	}
//	else
//	{
//		syslog(LOG_ERR, "SPDZ-2 extension library init: invalid field type [%s]", field);
//		return -1;
//	}
//
//	if(0 != proc->start(pid, num_of_parties, field, offline_size))
//	{
//		delete proc;
//		return -1;
//	}
//	*handle = proc;
//return 0;
//}
////-------------------------------------------------------------------------------------------//
//int term(void * handle)
//{
//	spdz2_ext_processor_base * proc = ((spdz2_ext_processor_base *)handle);
//	proc->stop(20);
//	delete proc;
//	return 0;
//}
////-------------------------------------------------------------------------------------------//
//int offline(void * handle, const int offline_size)
//{
//	return ((spdz2_ext_processor_base *)handle)->offline(offline_size);
//}
////-------------------------------------------------------------------------------------------//
//int start_open(void * handle, const size_t share_count, const u_int64_t * shares, int verify)
//{
//	return ((spdz2_ext_processor_base *)handle)->start_open(share_count, shares, verify);
//}
////-------------------------------------------------------------------------------------------//
//int stop_open(void * handle, size_t * open_count, u_int64_t ** opens)
//{
//	return ((spdz2_ext_processor_base *)handle)->stop_open(open_count, opens, 20);
//}
////-------------------------------------------------------------------------------------------//
//int triple(void * handle, u_int64_t * a, u_int64_t * b, u_int64_t * c)
//{
//	return ((spdz2_ext_processor_base *)handle)->triple(a, b, c, 20);
//}
////-------------------------------------------------------------------------------------------//
//int input(void * handle, const int input_of_pid, u_int64_t * input_value)
//{
//	return ((spdz2_ext_processor_base *)handle)->input(input_of_pid, input_value);
//}
////-------------------------------------------------------------------------------------------//
//int start_verify(void * handle, int * error)
//{
//	return ((spdz2_ext_processor_base *)handle)->start_verify(error);
//}
////-------------------------------------------------------------------------------------------//
//int stop_verify(void * handle)
//{
//	return ((spdz2_ext_processor_base *)handle)->stop_verify();
//}
////-------------------------------------------------------------------------------------------//
//int start_input(void * handle, const int input_of_pid, const size_t num_of_inputs)
//{
//	return ((spdz2_ext_processor_base *)handle)->start_input(input_of_pid, num_of_inputs);
//}
////-------------------------------------------------------------------------------------------//
//int stop_input(void * handle, size_t * input_count, u_int64_t ** inputs)
//{
//	return ((spdz2_ext_processor_base *)handle)->stop_input(input_count, inputs);
//}
////-------------------------------------------------------------------------------------------//
//u_int64_t test_conversion(const SPDZEXT_VALTYPE value)
//{
//	//ZpMersenneLongElement element(value);
//	return value;
//}
////-------------------------------------------------------------------------------------------//
//int start_mult(void * handle, const size_t share_count, const u_int64_t * shares, int verify)
//{
//	return ((spdz2_ext_processor_base *)handle)->start_mult(share_count, shares, verify);
//}
////-------------------------------------------------------------------------------------------//
//int stop_mult(void * handle, size_t * product_count, u_int64_t ** products)
//{
//	return ((spdz2_ext_processor_base *)handle)->stop_mult(product_count, products);
//}
////-------------------------------------------------------------------------------------------//
//int mix_add(void * handle, u_int64_t * share, u_int64_t scalar)
//{
//	return ((spdz2_ext_processor_base *)handle)->mix_add(share, scalar);
//}
////-------------------------------------------------------------------------------------------//
//int si_add(void * handle, u_int64_t * share, u_int64_t scalar)
//{
//	return ((spdz2_ext_processor_base *)handle)->si_add(share, scalar);
//}
////-------------------------------------------------------------------------------------------//
//int mix_sub_scalar(void * handle, u_int64_t * share, u_int64_t scalar)
//{
//	return ((spdz2_ext_processor_base *)handle)->mix_sub_scalar(share, scalar);
//}
////-------------------------------------------------------------------------------------------//
//int mix_sub_share(void * handle, u_int64_t scalar, u_int64_t * share)
//{
//	return ((spdz2_ext_processor_base *)handle)->mix_sub_share(scalar, share);
//}
////-------------------------------------------------------------------------------------------//
//int mix_mul(void * handle, u_int64_t * share, u_int64_t scalar)
//{
//	return ((spdz2_ext_processor_base *)handle)->mix_mul(share, scalar);
//}
////-------------------------------------------------------------------------------------------//
//int si_mul(void * handle, u_int64_t * share, u_int64_t scalar)
//{
//	return ((spdz2_ext_processor_base *)handle)->mix_mul(share, scalar);
//}
////-------------------------------------------------------------------------------------------//
//int skew_dec(void * handle, u_int64_t * share, const int number_of_bit, u_int64_t output_of_shares[][2])
//{
//	return ((spdz2_ext_processor_base *)handle)->skew_dec(share, number_of_bit, output_of_shares);
//}
////-------------------------------------------------------------------------------------------//
//int post_rec(void * handle, u_int64_t input_of_shares[][2], const int ring_size, u_int64_t * share)
//{
//	return ((spdz2_ext_processor_base *)handle)->post_rec(input_of_shares, ring_size, share);
//}
////-------------------------------------------------------------------------------------------//
//int skew_inj(void * handle, u_int64_t * share, u_int64_t output_of_shares[][2])
//{
//	return ((spdz2_ext_processor_base *)handle)->skew_inj(share, output_of_shares);
//}
////-------------------------------------------------------------------------------------------//
//int skew_rec(void * handle, u_int64_t * share, u_int64_t output_of_shares[][2])
//{
//	return ((spdz2_ext_processor_base *)handle)->skew_rec(share, output_of_shares);
//}
////-------------------------------------------------------------------------------------------//
//int start_share_immediates(void * handle, const int input_of_pid, const size_t value_count, const u_int64_t * values)
//{
//	return ((spdz2_ext_processor_base *)handle)->start_share_immediates(input_of_pid, value_count, values);
//}
////-------------------------------------------------------------------------------------------//
//int stop_share_immediates(void * handle, size_t * share_count, u_int64_t ** shares)
//{
//	return ((spdz2_ext_processor_base *)handle)->stop_share_immediates(share_count, shares);
//}
////-------------------------------------------------------------------------------------------//
//int share_immediate(void * handle, const u_int64_t value, u_int64_t * share)
//{
//	return ((spdz2_ext_processor_base *)handle)->share_immediate(value, share);
//}
////-------------------------------------------------------------------------------------------//
//SPDZEXT_VALTYPE gfp_add(SPDZEXT_VALTYPE v1, SPDZEXT_VALTYPE v2)
//{
////	ZpMersenneLongElement e1(v1), e2(v2);
//	return (v1 + v2);
//}
////-------------------------------------------------------------------------------------------//
//SPDZEXT_VALTYPE gfp_sub(SPDZEXT_VALTYPE v1, SPDZEXT_VALTYPE v2)
//{
////	ZpMersenneLongElement e1(v1), e2(v2);
//	return (v1 - v2);
//}
////-------------------------------------------------------------------------------------------//
//SPDZEXT_VALTYPE gfp_mult(SPDZEXT_VALTYPE v1, SPDZEXT_VALTYPE v2)
//{
////	ZpMersenneLongElement e1(v1), e2(v2);
//	return (v1 * v2);
//}
////-------------------------------------------------------------------------------------------//
//SPDZEXT_VALTYPE gf2n_add(SPDZEXT_VALTYPE v1, SPDZEXT_VALTYPE v2)
//{
////	ZpMersenneLongElement e1(v1), e2(v2);
//	return (v1 ^ v2);
//}
////-------------------------------------------------------------------------------------------//
//SPDZEXT_VALTYPE gf2n_sub(SPDZEXT_VALTYPE v1, SPDZEXT_VALTYPE v2)
//{
////	ZpMersenneLongElement e1(v1), e2(v2);
//	return (v1 ^ v2);
//}
////-------------------------------------------------------------------------------------------//
//SPDZEXT_VALTYPE gf2n_mult(SPDZEXT_VALTYPE v1, SPDZEXT_VALTYPE v2)
//{
////	ZpMersenneLongElement e1(v1), e2(v2);
//	return (v1 & v2);
//}
////-------------------------------------------------------------------------------------------//
