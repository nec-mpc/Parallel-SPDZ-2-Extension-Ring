#ifndef SPDZEXT_H_
#define SPDZEXT_H_

/*
	API for extension
	Version: 0.1
	Status: Confidential 
	
*/

extern "C"
{
	/*
  	    Context type. This holds information on a context.
		We follow the OPENSSL convention
		We can use this to store self party id, number of parties and so forth.
		NOTE: this will be an opaque structure. we will only include a forward declaration
		
	*/
	typedef struct { 
	
		uint64_t 	   handle; 
					  // may hold other data		
		} MPC_CTX;
	
	/* type definition for data items.
	we use two data types: share_t for shares, and clear_t for clear values
	both types are vectorized types. 
 	*/
	
	typedef struct{
		u_int8_t * data			//array of data items. can be one
		size_t size;	  		//size of single data item in bytes . for 2^32 ring this is 8 bytes + 4 bytes = 12 bytes 
								// (as 2 values in share, and 4 bytes for flags and fixed point
		int count;		  		//number of items in the data array, and in the metadata array (same count)
		size_t md_ring_size;	//metadata property of the whole vector. 
								// TBD - if this fixed for the whole execution, we should move it into the MPC_CTX 
	}share_t;
	
	typedef struct{
		u_int8_t * data;  		//array of data items. can be one
		size_t size;	  		//size of single data item in bytes . for 2^32 ring this is 4 bytes + 4 bytes = 8 bytes 
								// (as 1 value in clear - TBD, and 4 bytes for flags and fixed point)
		int count;		  		//number of items in the data array, and in the metadata array (same count)
		size_t md_ring_size;	//metadata property of the whole vector 
								// TBD - if this fixed for the whole execution, we should move it into the MPC_CTX
		}clear_t;

	
	/*
		Initialization and termination
	*/
	
	// Function: init 
	// Initialization is called when the 
	// responsibilities of implementer:  
	// - initialize threads
	// - Initialize sockets and handshake between parties ("join") so they are all ready to receive data
	int init(	MPC_CTX *ctx, 	  			// this is an [out] parameter, returns a handle to a context
				const int party_id, 		// Self party id
				const int num_of_parties, 	// RING protocol - This is always 3
				const char * field,			
				const int open_count,  		// RING protocol passes 0, as this is not used
				const int mult_count,  		// RING protocol passes 0, as this is not used
				const int bits_count); 		// RING protocol passes 0, as this is not used
				
	// Function : term
	// called at tear-down of program
	// responsibilities of implementer: close sockets and open file descriptors, release memory
	int term(MPC_CTX * ctx);

	/*
		Bit Decomposition and Ring Composition
	*/

	//vectorized bit de-composition skew
	int start_skew_bit_decomp(	MPC_CTX * ctx, 
								const share_t * rings_in,
								item_t * bits_out);
								
	int stop_skew_bit_decomp(MPC_CTX * ctz);					

	//vectorized bit re-composition skew
	int start_skew_ring_comp(	MPC_CTX * ctx, 
								const share_t * bits_in,
								share_t * rings_out);
								
	int stop_skew_ring_comp(MPC_CTX * ctx);	
	
	/*
	    Input functions
		
		Note: Input functions are vectorized and synchronous 
		     (we do not insert local instructions between start and stop)		
	*/
	
	// The plugin will call the extension after reading the inputs
	// from the user in stdin, in third party (socket) etc.
	int input_party (MPC_CTX * ctx,
					int sharing_party_id, 
					clear_t * rings_in, //in all parties but party id, this will be NULL
					share_t *rings_out);
					
	// The plugin will call the extension after reading the inputs
	// from file
	int input_share (MPC_CTX * ctx,
					clear_t * rings_in,
					share_t *rings_out);

	//helper functions for creating ring values from operating system types
	int make_input_from_integer  ( MPC_CTX * ctx,
									uint64_t * integers,
									int integers_count,
									share_t *rings_out);
	
	//creates fixed point values
	int make_input_from_float 	  ( MPC_CTX * ctx,
									float * floats,
									int float_count,
									share_t *rings_out);
					
	//we do need the extension to implement input_clear. 
	
	/*
	    Output functions
		
		Note: We use standard open instruction for output.
			  this opens the shares and creates clear value for all players
			  functions for output to single player and third party will be added later on
			  same function is used for int and fixed,
			  precondition: in presentation should be rings. it is the responsibility
			                of the compiler to pass ring and not bits here e.g emit instruction
							for ring-compose if required
	*/
	int start_open(		MPC_CTX * ctx,
						const share_t * rings_in,
						clear_t * rings_out);

	int stop_open(MPC_CTX * ctx);
	
	int make_integer_output  (	MPC_CTX * ctx,
								const share_t *rings_in,
								uint64_t * integers,
								int * integers_count);
	
	int make_float_output  	( 	MPC_CTX * ctx,
								const share_t *rings_in,
								uint64_t * integers,
								int * integers_count);

	
	//the verify function should verify all pending outputs not verified yet
	//following the SPDZ convention in this version we call verify() once at end of program
	
	int verify (		MPC_CTX * ctx,
						int * error);  //error is out parameter. 0 if no error, 
									   //-1 if verify failed and should abort

	/*
	    Arithmetic Operations
	*/
	
	//asynchronous vectorized multiplication
	//Todo: Document the precondition for the factors (ring or bits)
	int start_mult	(	MPC_CTX * ctx, 
						const share_t * factor1,
						const share_t * factor2,
						share_t * product);
			
	int stop_mult	(MPC_CTX * ctx);
	
	//Addition: This is currently not included, as is a local operation
	//int int_add(void * handle, 
	//const item_t addend1, 
	//const item_t addend2,
	//item_t * sum);


}

#endif /* SPDZEXT_H_ */