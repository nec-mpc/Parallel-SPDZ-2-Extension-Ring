#ifndef SPDZ_NEC_EXT_H_
#define SPDZ_NEC_EXT_H_

//#define MAX_SHARES 5000
#define MAX_SHARES 6400000

#include <stdlib.h>

extern "C"
{
	typedef struct
	{
		u_int64_t handle;
		// may hold other data
	} MPC_CTX;

	typedef struct{
		u_int8_t * data;
		size_t size, count, batch_size, md_ring_size;
	}share_t;

	typedef share_t clear_t;

	int init(MPC_CTX *ctx, const int party_id, const int num_of_parties,
			 const char * field, const int open_count, const int mult_count,
			 const int bits_count, const int batch_size);
    int term(MPC_CTX *ctx);

    int skew_bit_decomp(MPC_CTX * ctx, const share_t * rings_in, share_t * bits_out);
    int skew_ring_comp(MPC_CTX * ctx, const share_t * bits_in, share_t * rings_out);
    int input_party(MPC_CTX * ctx, int sharing_party_id, clear_t * rings_in, share_t * rings_out);
    int input_share(MPC_CTX * ctx, clear_t * rings_in, share_t *rings_out);
    int make_input_from_integer(MPC_CTX * ctx, u_int64_t * integers, int integers_count, clear_t * rings_out);
    int make_input_from_fixed(MPC_CTX * ctx, const char * fix_strs[], int fix_count, clear_t * rings_out);
    int start_open(MPC_CTX * ctx, const share_t * rings_in, clear_t * rings_out);
    int stop_open(MPC_CTX * ctx);
    int make_integer_output(MPC_CTX * ctx, const share_t * rings_in, u_int64_t * integers, int * integers_count);
    int make_fixed_output(MPC_CTX * ctx, const share_t * rings_in, char * fix_strs[], int * fixed_count);
    int verify_optional_suggest(MPC_CTX * ctx, int * error);
    int verify_final(MPC_CTX * ctx, int * error);
    int start_mult(MPC_CTX * ctx, const share_t * factor1, const share_t * factor2, share_t * product);
    int stop_mult(MPC_CTX * ctx);

}

#endif /* SPDZ_NEC_EXT_H_ */


//
//#ifndef SPDZEXT_H_
//#define SPDZEXT_H_
//
//#include <stdlib.h>
//
////#define SPDZEXT_VALTYPE	u_int32_t
//#define SPDZEXT_VALTYPE u_int64_t
//
//extern "C"
//{
//	/**
//	 * Initialize a session with the extension library for the client.
//	 * @param[out] handle A session handle to be filled by the function
//	 * @param[in] pid The party identifier
//	 * @param[in] num_of_parties The total number of parties
//	 * @param[in] field The prime field used in text format
//	 * @param[in] offline_size The number of offline elements to create
//	 * @return 0 on success, -1 otherwise
//	 */
//	int init(void ** handle, const int pid, const int num_of_parties, const char * field, const int offline_size = 0);
//
//	/**
//	 * Terminates an initialized session with the extension library and frees all resources
//	 * @param[in] handle An initialized session handle
//	 * @return 0 on success, -1 otherwise
//	 */
//	int term(void * handle);
//
//	/**
//	 * Create additional offline elements
//	 * @param[in] handle An initialized session handle
//	 * @param[in] offline_size The number of offline elements to create
//	 * @return 0 on success, -1 otherwise
//	 */
//	int offline(void * handle, const int offline_size);
//
//	/**
//	 * Start an asynchronous opening of shared values
//	 * Must be followed by a call to stop open
//	 * @param[in] handle An initialized session handle
//	 * @param[in] share_count The number of shared values to be opened
//	 * @param[in] shares An array of share values to be opened
//	 * @param[in] verify Whether the opened values are to be verified, 0 for false
//	 * @return 0 on success, -1 otherwise
//	 */
//	int start_open(void * handle, const size_t share_count, const SPDZEXT_VALTYPE * shares, int verify);
//
//	/**
//	 * Complete an asynchronous opening of shared values
//	 * Will fail if no open start was called before
//	 * @param[in] handle An initialized session handle
//	 * @param[out] open_count The number of the opened values
//	 * @param[out] opens The opened values - the memory will be allocated by the function
//	 * @return 0 on success, -1 otherwise
//	 */
//	int stop_open(void * handle, size_t * open_count, SPDZEXT_VALTYPE ** opens);
//
//	/**
//	 * Retrieve a triple of values
//	 * @param[in] handle An initialized session handle
//	 * @param[out] a The first value
//	 * @param[out] b The second value
//	 * @param[out] c The third value
//	 * @return 0 on success, -1 otherwise
//	 */
//	int triple(void * handle, SPDZEXT_VALTYPE * a, SPDZEXT_VALTYPE * b, SPDZEXT_VALTYPE * c);
//
//	/**
//	 * Retrieve an input value from a party
//	 * @param[in] handle An initialized session handle
//	 * @param[in] input_of_pid The party identifier from which input is required
//	 * @param[out] input_value The input value
//	 * @return 0 on success, -1 otherwise
//	 */
//	int input(void * handle, const int input_of_pid, SPDZEXT_VALTYPE * input_value);
//
//	/**
//	 * Start an asynchronous verification operation
//	 * Must be followed by a call to stop verify
//	 * @param[in] handle An initialized session handle
//	 * @param[out] error Verification error code
//	 * @return 0 on success, -1 otherwise
//	 */
//	int start_verify(void * handle, int * error);
//
//	/**
//	 * Complete an asynchronous verification operation
//	 * Will fail if no verification start was called before
//	 * @param[in] handle An initialized session handle
//	 * @return 0 on success, -1 otherwise
//	 */
//	int stop_verify(void * handle);
//
//	/**
//	 * Start an asynchronous multiple input values operation
//	 * Must be followed by a call to stop input
//	 * @param[in] handle An initialized session handle
//	 * @param[in] input_of_pid The party identifier from which input is required
//	 * @param[in] num_of_inputs The number of required input values
//	 * @return 0 on success, -1 otherwise
//	 */
//    int start_input(void * handle, const int input_of_pid, const size_t num_of_inputs);
//
//	/**
//	 * Complete an asynchronous multiple input values operation
//	 * Will fail if no input start was called before
//	 * @param[in] handle An initialized session handle
//	 * @param[out] input_count Number of the input values
//	 * @param[out] inputs The input values - the memory will be allocated by the function
//	 * @return 0 on success, -1 otherwise
//	 */
//    int stop_input(void * handle, size_t * input_count, SPDZEXT_VALTYPE ** inputs);
//
//    /**
//     * Start an asynchronous multiply operation
//	 * Must be followed by a call to stop mult
//	 * @param[in] handle An initialized session handle
//	 * @param[in] share_count Number of the shared values
//	 * @param[in] shares The shared values
//	 * @param[in] verify Verification flag
//	 * @return 0 on success, -1 otherwise
//     */
//    int start_mult(void * handle, const size_t share_count, const SPDZEXT_VALTYPE * shares, int verify);
//
//	/**
//	 * Complete an asynchronous multiply operation
//	 * Will fail if no start mult was called before
//	 * @param[in] handle An initialized session handle
//	 * @param[out] product_count Number of the product values
//	 * @param[out] products The product results
//	 * @return 0 on success, -1 otherwise
//	 */
//    int stop_mult(void * handle, size_t * product_count, SPDZEXT_VALTYPE ** products);
//
//    /**
//     * Addition of a share value with a scalar value
//     * @param[in] handle An initialized session handle
//     * @param[in/out] share The share value to which the scalar value is added
//     * @param[in] scalar A scalar value to add to the share
//	 * @return 0 on success, -1 otherwise
//     */
//    int mix_add(void * handle, SPDZEXT_VALTYPE * share, SPDZEXT_VALTYPE scalar);
//
//    /**
//     * Subtraction of a scalar value from a share
//     * @param[in] handle An initialized session handle
//     * @param[in/out] share The share value from which the scalar value is subtracted
//     * @param[in] scalar A scalar value to subtract from the share
//	 * @return 0 on success, -1 otherwise
//     */
//
//    int si_add(void * handle, SPDZEXT_VALTYPE * share, SPDZEXT_VALTYPE scalar);
//
//     /**
//      * Subtraction of a scalar value from a share
//      * @param[in] handle An initialized session handle
//      * @param[in/out] share The share value from which the scalar value is subtracted
//      * @param[in] scalar A scalar value to subtract from the share
//      * @return 0 on success, -1 otherwise
//       */
//
//    int mix_sub_scalar(void * handle, SPDZEXT_VALTYPE * share, SPDZEXT_VALTYPE scalar);
//
//    /**
//     * Subtraction of a share value from a scalar
//     * @param[in] handle An initialized session handle
//     * @param[in/out] share The share value to subtract from the scalar
//     * @param[in] scalar A scalar from which the share value is subtracted
//	 * @return 0 on success, -1 otherwise
//     */
//    int mix_sub_share(void * handle, SPDZEXT_VALTYPE scalar, SPDZEXT_VALTYPE * share);
//
//    /**
//     * Multiplication of a share value with a scalar value
//     * @param[in] handle An initialized session handle
//     * @param[in/out] share The share value to which the scalar value is added
//     * @param[in] scalar A scalar value to multiply to the share
//	 * @return 0 on success, -1 otherwise
//     */
//    int mix_mul(void * handle, SPDZEXT_VALTYPE * share, SPDZEXT_VALTYPE scalar);
//
//    /**
//     * Start an asynchronous operation of sharing given values
//	 * Must be followed by a stop call to share_immediates
//	 * @param[in] handle An initialized session handle
//	 * @param[in] input_of_pid The party identifier that will initiate the share (by simulating input)
//	 * @param[in] value_count The number of the values to share
//	 * @param[in] values An array of values to share
//	 * @return 0 on success, -1 otherwise
//     */
//
//    int si_mul(void * handle, SPDZEXT_VALTYPE * share, SPDZEXT_VALTYPE scalar);
//
//    /**
//     * Start an asynchronous operation of sharing given values
//	 * Must be followed by a stop call to share_immediates
//	 * @param[in] handle An initialized session handle
//	 * @param[in] input_of_pid The party identifier that will initiate the share (by simulating input)
//	 * @param[in] value_count The number of the values to share
//	 * @param[in] values An array of values to share
//	 * @return 0 on success, -1 otherwise
//     */
//
//    int start_share_immediates(void * handle, const int input_of_pid, const size_t value_count, const SPDZEXT_VALTYPE * values);
//
//	/**
//	 * Complete an asynchronous value sharing operation
//	 * Will fail if no start share_immediates was previously called
//	 * @param[in] handle An initialized session handle
//	 * @param[out] share_count Number of the shared values
//	 * @param[out] shares The shared values
//	 * @return 0 on success, -1 otherwise
//	 */
//    int stop_share_immediates(void * handle, size_t * share_count, SPDZEXT_VALTYPE ** shares);
//
//    /**
//     * Synchrnous load share from immediate
//	 * @param[in] handle An initialized session handle
//	 * @param[in] value A value to share
//	 * @param[out] share A share to fill
//	 * @return 0 on success, -1 otherwise
//     */
//
//    int skew_dec(void * handle, u_int64_t * share, const int number_of_bit, SPDZEXT_VALTYPE output_shares[][2]);
//
//    /**
//      * It is pre-computation for bit-decomposition.
//     * @param[in] handle An initialized session handle
//     * @param[in] a share for bit-decomposition
//     * @param[in] number of bit for bit-decomposition
//     * @param[out] results of skew-decomposition
//     */
//
//    int skew_inj(void * handle, u_int64_t * share, SPDZEXT_VALTYPE output_shares[][2]);
//
//     /**
//      * It is pre-computation for bit-injection.
//      * @param[in] handle An initialized session handle
//      * @param[in] a share for bit-injection
//      * @param[out] results of skew-injection
//      */
//
//    int skew_rec(void * handle, u_int64_t * share, SPDZEXT_VALTYPE output_shares[][2]);
//
//     /**
//      * It is pre-computation for bit re-composition (ring-composition).
//      * @param[in] handle An initialized session handle
//      * @param[in] a share for skew-re-composition
//      * @param[out] results of skew-re-composition
//      */
//
//    int post_rec(void * handle, SPDZEXT_VALTYPE input_shares[][2], const int ring_size, u_int64_t * share);
//
//    /**
//      * It is post-computation for bit re-composition (ring-composition).
//     * @param[in] handle An initialized session handle
//     * @param[in] shares for bit re-composition (ring-composition)
//     * @param[in] the size of ring
//     * @param[out] results of bit re-composition (ring-composition)
//     */
//
//    int share_immediate(void * handle, const SPDZEXT_VALTYPE value, SPDZEXT_VALTYPE * share);
//
//	SPDZEXT_VALTYPE gfp_add(SPDZEXT_VALTYPE, SPDZEXT_VALTYPE);
//	SPDZEXT_VALTYPE gfp_sub(SPDZEXT_VALTYPE, SPDZEXT_VALTYPE);
//	SPDZEXT_VALTYPE gfp_mult(SPDZEXT_VALTYPE, SPDZEXT_VALTYPE);
//
//	SPDZEXT_VALTYPE gf2n_add(SPDZEXT_VALTYPE, SPDZEXT_VALTYPE);
//	SPDZEXT_VALTYPE gf2n_sub(SPDZEXT_VALTYPE, SPDZEXT_VALTYPE);
//	SPDZEXT_VALTYPE gf2n_mult(SPDZEXT_VALTYPE, SPDZEXT_VALTYPE);
//
//    /**
//     * Test the library value conversion from integer (32/64) to internal representation
//     * The returned value should be equal to the provided value
//     * @param value The test value to convert
//     * @return The value restored from the converted value
//     */
//	SPDZEXT_VALTYPE test_conversion(const SPDZEXT_VALTYPE value);
//
//}
//
//#endif /* SPDZEXT_H_ */
