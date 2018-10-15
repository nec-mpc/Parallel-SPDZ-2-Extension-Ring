#include "spdz2_ext_processor_base.h"

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>

using namespace std;

//***********************************************************************************************//
void * spdz2_ext_processor_proc(void * arg)
{
	spdz2_ext_processor_base * processor = (spdz2_ext_processor_base *)arg;
	processor->run();
}
//***********************************************************************************************//
const int spdz2_ext_processor_base::op_code_open = 100;
const int spdz2_ext_processor_base::op_code_triple = 101;
const int spdz2_ext_processor_base::op_code_offline = 102;
const int spdz2_ext_processor_base::op_code_input = 103;
const int spdz2_ext_processor_base::op_code_verify = 104;
const int spdz2_ext_processor_base::op_code_input_asynch = 105;
const int spdz2_ext_processor_base::op_code_mult = 106;
const int spdz2_ext_processor_base::op_code_share_immediates = 107;
const int spdz2_ext_processor_base::op_code_share_immediate = 108;

//***********************************************************************************************//
spdz2_ext_processor_base::spdz2_ext_processor_base()
 : runner(0), run_flag(false), party_id(-1), offline_size(-1), num_of_parties(0)
 , offline_success(false)
 , start_open_on(false), do_verify_open(false), open_success(false)
 , pa(NULL), pb(NULL), pc(NULL), triple_success(false)
 , input_party_id(-1), input_success(false), p_input_value(NULL)
 , verification_error(NULL), verification_on(false), verify_success(false)
 , input_asynch_on(false), intput_asynch_party_id(-1), num_of_inputs(0), input_asynch_success(false)
 , mult_on(false), mult_success(false)
 , share_immediates_on(false), share_immediates_success(false)
 , p_immediate_share(NULL), share_immediate_success(false)
{
	pthread_mutex_init(&q_lock, NULL);
	sem_init(&task, 0, 0);
	sem_init(&open_done, 0, 0);
	sem_init(&triple_done, 0, 0);
	sem_init(&offline_done, 0, 0);
	sem_init(&input_done, 0, 0);
	sem_init(&verify_done, 0, 0);
	sem_init(&input_asynch_done, 0, 0);
	sem_init(&mult_done, 0, 0);
	sem_init(&share_immediates_done, 0, 0);
	sem_init(&share_immediate_done, 0, 0);

	openlog("spdz_ext_biu", LOG_NDELAY|LOG_PID, LOG_USER);
	setlogmask(LOG_UPTO(LOG_DEBUG));
}

//***********************************************************************************************//
spdz2_ext_processor_base::~spdz2_ext_processor_base()
{
	pthread_mutex_destroy(&q_lock);
	sem_destroy(&task);
	sem_destroy(&open_done);
	sem_destroy(&triple_done);
	sem_destroy(&offline_done);
	sem_destroy(&input_done);
	sem_destroy(&verify_done);
	sem_destroy(&input_asynch_done);
	sem_destroy(&mult_done);
	sem_destroy(&share_immediates_done);
	sem_destroy(&share_immediate_done);

	closelog();
}

//***********************************************************************************************//
int spdz2_ext_processor_base::start(const int pid, const int num_of_parties_, const char * field, const int offline)
{
	if(run_flag)
	{
		syslog(LOG_ERR, "spdz2_ext_processor_base::start: this processor is already started");
		return -1;
	}

	char sz[64];
	snprintf(sz, 64, "party_%d_input.txt", pid);
	input_file = sz;

	party_id = pid;
	offline_size = offline;
	num_of_parties = num_of_parties_;

	if(0 != init_protocol())
	{
		syslog(LOG_ERR, "spdz2_ext_processor_base::start: protocol initialization failure.");
		return -1;
	}

	run_flag = true;
	int result = pthread_create(&runner, NULL, spdz2_ext_processor_proc, this);
	if(0 != result)
	{
		char errmsg[512];
		syslog(LOG_ERR, "spdz2_ext_processor_base::start: pthread_create() failed with error %d : %s", result, strerror_r(result, errmsg, 512));
		run_flag = false;
		return -1;
	}

//	syslog(LOG_NOTICE, "spdz2_ext_processor_base::start: pid %d", party_id);
	return 0;
}

//***********************************************************************************************//
int spdz2_ext_processor_base::stop(const time_t timeout_sec)
{
	if(!run_flag)
	{
		syslog(LOG_ERR, "spdz2_ext_processor_base::stop this processor is not running.");
		return -1;
	}

	run_flag = false;
	void * return_code = NULL;

	struct timespec timeout;
	clock_gettime(CLOCK_REALTIME, &timeout);
	timeout.tv_sec += timeout_sec;

	int result = pthread_timedjoin_np(runner, &return_code, &timeout);
	if(0 != result)
	{
		char errmsg[512];
		syslog(LOG_ERR, "spdz2_ext_processor_base::stop: pthread_timedjoin_np() failed with error %d : %s", result, strerror_r(result, errmsg, 512));
		return -1;
	}

	delete_protocol();

//	syslog(LOG_NOTICE, "spdz2_ext_processor_base::stop: pid %d", party_id);
	return 0;
}

//***********************************************************************************************//
void spdz2_ext_processor_base::run()
{
	long timeout_ns = 200 /*ms*/ * 1000 /*us*/ * 1000 /*ns*/;
	while(run_flag)
	{
		struct timespec timeout;
		clock_gettime(CLOCK_REALTIME, &timeout);
		timeout.tv_nsec += timeout_ns;
		timeout.tv_sec += timeout.tv_nsec / 1000000000;
		timeout.tv_nsec = timeout.tv_nsec % 1000000000;

		int result = sem_timedwait(&task, &timeout);
		if(0 == result)
		{
			int op_code = pop_task();
//			syslog(LOG_NOTICE, "spdz2_ext_processor_base::run: op_code %d", op_code);
			switch(op_code)
			{
			case op_code_open:
				exec_open();
				break;
			case op_code_triple:
				exec_triple();
				break;
			case op_code_offline:
				exec_offline();
				break;
			case op_code_input:
				exec_input();
				break;
			case op_code_verify:
				exec_verify();
				break;
			case op_code_input_asynch:
				exec_input_asynch();
				break;
			case op_code_mult:
				exec_mult();
				break;
			case op_code_share_immediates:
				exec_share_immediates();
				break;
			case op_code_share_immediate:
				exec_share_immediate();
				break;
			default:
				syslog(LOG_WARNING, "spdz2_ext_processor_base::run: unsupported op_code %d", op_code);
				break;
			}
		}
	}
}

//***********************************************************************************************//
int spdz2_ext_processor_base::push_task(const int op_code)
{
	int result = pthread_mutex_lock(&q_lock);

	if(0 != result)
	{
		char errmsg[512];
		syslog(LOG_ERR, "spdz2_ext_processor_base::push_task: pthread_mutex_lock() failed with error %d : %s", result, strerror_r(result, errmsg, 512));
		return -1;
	}

	task_q.push_back(op_code);

	sem_post(&task);
	result = pthread_mutex_unlock(&q_lock);


	if(0 != result)
	{
		char errmsg[512];
		syslog(LOG_ERR, "spdz2_ext_processor_base::push_task: pthread_mutex_unlock() failed with error %d : %s", result, strerror_r(result, errmsg, 512));
	}
//	syslog(LOG_NOTICE, "spdz2_ext_processor_base::push_task: op_code %d", op_code);
	return 0;
}

//***********************************************************************************************//
int spdz2_ext_processor_base::pop_task()
{
	int op_code = -1;
	int result = pthread_mutex_lock(&q_lock);
	if(0 == result)
	{
		if(!task_q.empty())
		{
			op_code = *task_q.begin();
			task_q.pop_front();
		}

		result = pthread_mutex_unlock(&q_lock);
		if(0 != result)
		{
			char errmsg[512];
			syslog(LOG_ERR, "spdz2_ext_processor_base::pop_task: pthread_mutex_unlock() failed with error %d : %s", result, strerror_r(result, errmsg, 512));
		}
	}
	else
	{
		char errmsg[512];
		syslog(LOG_ERR, "spdz2_ext_processor_base::pop_task: pthread_mutex_lock() failed with error %d : %s", result, strerror_r(result, errmsg, 512));
	}

//	syslog(LOG_NOTICE, "spdz2_ext_processor_base::pop_task: op_code %d", op_code);
	return op_code;
}

//***********************************************************************************************//
int spdz2_ext_processor_base::offline(const int /*offline_size*/, const time_t timeout_sec)
{
	/*
	 * In BIU implementation the offline size is set at the protocol construction
	 * the call to offline will reallocate the same size of offline
	 */
	offline_success = false;
	if(0 != push_task(spdz2_ext_processor_base::op_code_offline))
	{
		syslog(LOG_ERR, "spdz2_ext_processor_base::offline: failed pushing an offline task to queue.");
		return -1;
	}

	struct timespec timeout;
	clock_gettime(CLOCK_REALTIME, &timeout);
	timeout.tv_sec += timeout_sec;

	int result = sem_timedwait(&offline_done, &timeout);
	if(0 != result)
	{
		result = errno;
		char errmsg[512];
		syslog(LOG_ERR, "spdz2_ext_processor_base::offline: sem_timedwait() failed with error %d : %s", result, strerror_r(result, errmsg, 512));
		return -1;
	}

	return (offline_success)? 0: -1;
}

//***********************************************************************************************//
void spdz2_ext_processor_base::exec_offline()
{
	offline_success = protocol_offline();
	sem_post(&offline_done);
}

//***********************************************************************************************//
int spdz2_ext_processor_base::start_open(int count, const SPDZEXT_VALTYPE * share_values, SPDZEXT_VALTYPE * open_values, int verify)
{
	if(start_open_on)
	{
		syslog(LOG_ERR, "spdz2_ext_processor_base::start_open: open is already started (a stop_open() call is required).");
		return -1;
	}
	start_open_on = true;
	open_success = false;

	dest = open_values;
	shares.assign(share_values, share_values + count);

// 	cout << "=================================" << endl;
//	cout << "[spdz2_ext_processor_base::start_open]" << endl;
//	cout << "shares.size = " << shares.size() << endl;
//	cout << "count       = " << count << endl;
//	cout << "=================================" << endl;

//	do_verify_open = (verify != 0)? true: false;
	if(0 != push_task(spdz2_ext_processor_base::op_code_open))
	{
		syslog(LOG_ERR, "spdz2_ext_processor_base::start_open: failed pushing an open task to queue.");
		start_open_on = false;
		return -1;
	}
	return 0;
}
//int spdz2_ext_processor_base::start_open(const size_t share_count, const u_int64_t * share_values, int verify)
//{
//	if(start_open_on)
//	{
//		syslog(LOG_ERR, "spdz2_ext_processor_base::start_open: open is already started (a stop_open() call is required).");
//		return -1;
//	}
//	start_open_on = true;
//	open_success = false;
//
//	shares.assign(share_values, share_values + share_count);
//	do_verify_open = (verify != 0)? true: false;
//	if(0 != push_task(spdz2_ext_processor_base::op_code_open))
//	{
//		syslog(LOG_ERR, "spdz2_ext_processor_base::start_open: failed pushing an open task to queue.");
//		start_open_on = false;
//		return -1;
//	}
//
//	return 0;
//}

//***********************************************************************************************//
int spdz2_ext_processor_base::stop_open(const time_t timeout_sec)
{
	if(!start_open_on)
	{
		syslog(LOG_ERR, "spdz2_ext_processor_base::stop_open: open is not started.");
		return -1;
	}
	start_open_on = false;
	struct timespec timeout;
	clock_gettime(CLOCK_REALTIME, &timeout);
	timeout.tv_sec += timeout_sec;

	int result = sem_timedwait(&open_done, &timeout);

	if(0 != result)
	{
		result = errno;
		char errmsg[512];
		syslog(LOG_ERR, "spdz2_ext_processor_base::stop_open: sem_timedwait() failed with error %d : %s", result, strerror_r(result, errmsg, 512));
		return -1;
	}

	if(open_success)
	{
		if(!opens.empty())
		{
//			dest = new SPDZEXT_VALTYPE[opens.size()]; // dest is allocated in spdzext.cpp
			memcpy(dest, &opens[0], (opens.size())*sizeof(SPDZEXT_VALTYPE));

			opens.clear();
		}
		return 0;
	}
	else
	{
		syslog(LOG_ERR, "spdz2_ext_processor_base::stop_open: open failed.");
		return -1;
	}
	cout << "spdz2_ext_processor_base.cpp::stop_open:test5" << endl;
}
//
//int spdz2_ext_processor_base::stop_open(size_t * open_count, u_int64_t ** open_values, const time_t timeout_sec)
//{
//	if(!start_open_on)
//	{
//		syslog(LOG_ERR, "spdz2_ext_processor_base::stop_open: open is not started.");
//		return -1;
//	}
//	start_open_on = false;
//
//	*open_count = 0;
//	*open_values = NULL;
//
//	struct timespec timeout;
//	clock_gettime(CLOCK_REALTIME, &timeout);
//	timeout.tv_sec += timeout_sec;
//
//	int result = sem_timedwait(&open_done, &timeout);
//	if(0 != result)
//	{
//		result = errno;
//		char errmsg[512];
//		syslog(LOG_ERR, "spdz2_ext_processor_base::stop_open: sem_timedwait() failed with error %d : %s", result, strerror_r(result, errmsg, 512));
//		return -1;
//	}
//
//	if(open_success)
//	{
//		if(!opens.empty())
//		{
//			*open_values = new u_int64_t[*open_count = opens.size()];
//			memcpy(*open_values, &opens[0], (*open_count)*sizeof(u_int64_t));
//			opens.clear();
//		}
//		return 0;
//	}
//	else
//	{
//		syslog(LOG_ERR, "spdz2_ext_processor_base::stop_open: open failed.");
//		return -1;
//	}
//}


//***********************************************************************************************//
void spdz2_ext_processor_base::exec_open()
{
	open_success = protocol_open();
	sem_post(&open_done);
}

//***********************************************************************************************//
int spdz2_ext_processor_base::triple(u_int64_t * a, u_int64_t * b, u_int64_t * c, const time_t timeout_sec)
{
	pa = a;
	pb = b;
	pc = c;
	triple_success = false;

	if(0 != push_task(spdz2_ext_processor_base::op_code_triple))
	{
		syslog(LOG_ERR, "spdz2_ext_processor_base::triple: failed pushing a triple task to queue.");
		return -1;
	}

	struct timespec timeout;
	clock_gettime(CLOCK_REALTIME, &timeout);
	timeout.tv_sec += timeout_sec;

	int result = sem_timedwait(&triple_done, &timeout);
	if(0 != result)
	{
		result = errno;
		char errmsg[512];
		syslog(LOG_ERR, "spdz2_ext_processor_base::triple: sem_timedwait() failed with error %d : %s", result, strerror_r(result, errmsg, 512));
		return -1;
	}

	pa = pb = pc = NULL;
	return (triple_success)? 0: -1;
}

//***********************************************************************************************//
void spdz2_ext_processor_base::exec_triple()
{
	triple_success = protocol_triple();
	sem_post(&triple_done);
}

//***********************************************************************************************//
int spdz2_ext_processor_base::input(const int input_of_pid, int count, SPDZEXT_VALTYPE * input_values, SPDZEXT_VALTYPE * output_values)
{
	num_input = count;
	p_input_value  = input_values;
	p_output_value = output_values;
	input_party_id = input_of_pid;
	input_success = false;

	if(0 != push_task(spdz2_ext_processor_base::op_code_input))
	{
		syslog(LOG_ERR, "spdz2_ext_processor_base::input: failed pushing an input task to queue.");
		return -1;
	}

	//No timeout waiting for input - the user might take long to enter data

	int result = sem_wait(&input_done);
	if(0 != result)
	{
		result = errno;
		char errmsg[512];
		syslog(LOG_ERR, "spdz2_ext_processor_base::input: sem_wait() failed with error %d : %s", result, strerror_r(result, errmsg, 512));
		return -1;
	}

	p_input_value = NULL;
	input_party_id = -1;

	return (input_success)? 0: -1;
}


//***********************************************************************************************//
void spdz2_ext_processor_base::exec_input()
{
	input_success = protocol_input();
	sem_post(&input_done);
}

//***********************************************************************************************//
int spdz2_ext_processor_base::start_verify(int * error)
{
	if(verification_on)
	{
		syslog(LOG_ERR, "spdz2_ext_processor_base::start_verify: verify is already started (a stop_verify() call is required).");
		return -1;
	}
	verification_on = true;
	verify_success = false;
	verification_error = error;
	if(0 != push_task(spdz2_ext_processor_base::op_code_verify))
	{
		syslog(LOG_ERR, "spdz2_ext_processor_base::start_verify: failed pushing a verify task to queue.");
		return -1;
	}
	return 0;
}

//***********************************************************************************************//
int spdz2_ext_processor_base::stop_verify(const time_t timeout_sec)
{
	if(!verification_on)
	{
		syslog(LOG_ERR, "spdz2_ext_processor_base::stop_verify: verify is not started.");
		return -1;
	}
	verification_on = false;

	struct timespec timeout;
	clock_gettime(CLOCK_REALTIME, &timeout);
	timeout.tv_sec += timeout_sec;

	int result = sem_timedwait(&verify_done, &timeout);
	if(0 != result)
	{
		result = errno;
		char errmsg[512];
		syslog(LOG_ERR, "spdz2_ext_processor_base::stop_verify: sem_timedwait() failed with error %d : %s", result, strerror_r(result, errmsg, 512));
		return -1;
	}

	verification_error = NULL;
	return (verify_success)? 0: -1;
}

//***********************************************************************************************//
void spdz2_ext_processor_base::exec_verify()
{
	*verification_error = 0;
	verify_success = true;
	sem_post(&verify_done);
}

//***********************************************************************************************//
int spdz2_ext_processor_base::start_input(const int input_of_pid, const size_t num_of_inputs_)
{
	if(input_asynch_on)
	{
		syslog(LOG_ERR, "spdz2_ext_processor_base::start_input: asynch input is already started (a stop_input() call is required).");
		return -1;
	}
	input_asynch_on = true;
	input_asynch_success = false;
	intput_asynch_party_id = input_of_pid;
	num_of_inputs = num_of_inputs_;

	if(0 != push_task(spdz2_ext_processor_base::op_code_input_asynch))
	{
		syslog(LOG_ERR, "spdz2_ext_processor_base::start_input: failed pushing an input task to queue.");
		return -1;
	}
	return 0;
}

//***********************************************************************************************//
int spdz2_ext_processor_base::stop_input(size_t * input_count, u_int64_t ** inputs)
{
	if(!input_asynch_on)
	{
		syslog(LOG_ERR, "spdz2_ext_processor_base::stop_input: asynch input is not started.");
		return -1;
	}
	input_asynch_on = false;

	int result = sem_wait(&input_asynch_done);
	if(0 != result)
	{
		result = errno;
		char errmsg[512];
		syslog(LOG_ERR, "spdz2_ext_processor_base::stop_input: sem_wait() failed with error %d : %s", result, strerror_r(result, errmsg, 512));
		return -1;
	}

	if(input_asynch_success)
	{
		if(!input_values.empty())
		{
			*inputs = new u_int64_t[*input_count = input_values.size()];
			memcpy(*inputs, &input_values[0], *input_count * sizeof(u_int64_t));
		}
		return 0;
	}
	else
	{
		return -1;
	}
}

//***********************************************************************************************//
void spdz2_ext_processor_base::exec_input_asynch()
{
	input_asynch_success = protocol_input_asynch();
	sem_post(&input_asynch_done);
}

//***********************************************************************************************//

int spdz2_ext_processor_base::start_mult(int count, const SPDZEXT_VALTYPE * factors_values, SPDZEXT_VALTYPE * prod_values, int verify)
{

	if(mult_on)
	{
		syslog(LOG_ERR, "spdz2_ext_processor_base::start_mult: mult is already started (a stop_mult() call is required).");
		return -1;
	}
	mult_on = true;
	mult_success = false;
	mult_values.assign(factors_values, factors_values + count);

	mult_dest = prod_values;
	products.clear();

	if(0 != push_task(spdz2_ext_processor_base::op_code_mult))
	{
		syslog(LOG_ERR, "spdz2_ext_processor_base::start_mult: failed pushing a mult task to queue.");
		return -1;
	}
	return 0;
}


//int spdz2_ext_processor_base::start_mult(const size_t share_count, const u_int64_t * shares, int verify)
//{
//	if(mult_on)
//	{
//		syslog(LOG_ERR, "spdz2_ext_processor_base::start_mult: mult is already started (a stop_mult() call is required).");
//		return -1;
//	}
//	mult_on = true;
//	mult_success = false;
//	mult_values.assign(shares, shares + share_count);
//	products.clear();
//
//	if(0 != push_task(spdz2_ext_processor_base::op_code_mult))
//	{
//		syslog(LOG_ERR, "spdz2_ext_processor_base::start_mult: failed pushing a mult task to queue.");
//		return -1;
//	}
//	return 0;
//}

//***********************************************************************************************//

int spdz2_ext_processor_base::stop_mult(const time_t timeout_sec)
{
	if(!mult_on)
	{
		syslog(LOG_ERR, "spdz2_ext_processor_base::stop_mult: mult is not started.");
		return -1;
	}
	mult_on = false;

	struct timespec timeout;
	clock_gettime(CLOCK_REALTIME, &timeout);
	timeout.tv_sec += timeout_sec;

	int result = sem_timedwait(&mult_done, &timeout);

	if(0 != result)
	{
		result = errno;
		char errmsg[512];
		syslog(LOG_ERR, "spdz2_ext_processor_base::stop_open: sem_timedwait() failed with error %d : %s", result, strerror_r(result, errmsg, 512));
		return result;
	}

	if(mult_success)
	{

		if(!products.empty())
		{
			memcpy(mult_dest, &products[0], (products.size())*sizeof(SPDZEXT_VALTYPE));
			products.clear();
		}
		return 0;
	}
	else
	{
		syslog(LOG_ERR, "spdz2_ext_processor_base::stop_mult: mult failed.");
		return -1;
	}
}


//int spdz2_ext_processor_base::stop_mult(size_t * product_count, u_int64_t ** product_values)
//{
//	if(!mult_on)
//	{
//		syslog(LOG_ERR, "spdz2_ext_processor_base::stop_mult: mult is not started.");
//		return -1;
//	}
//	mult_on = false;
//
//	int result = sem_wait(&mult_done);
//	if(0 != result)
//	{
//		result = errno;
//		char errmsg[512];
//		syslog(LOG_ERR, "spdz2_ext_processor_base::stop_mult: sem_wait() failed with error %d : %s", result, strerror_r(result, errmsg, 512));
//		return -1;
//	}
//
//	if(mult_success)
//	{
//		if(!products.empty())
//		{
//			*product_values = new u_int64_t[*product_count = products.size()];
//			memcpy(*product_values, &products[0], *product_count * sizeof(u_int64_t));
//		}
//		return 0;
//	}
//	else
//	{
//		return -1;
//	}
//}

//***********************************************************************************************//
void spdz2_ext_processor_base::exec_mult()
{
	mult_success = protocol_mult();
	sem_post(&mult_done);
}

//***********************************************************************************************//
int spdz2_ext_processor_base::start_share_immediates(const int input_of_pid, const size_t value_count, const u_int64_t * values)
{
	if(share_immediates_on)
	{
		syslog(LOG_ERR, "spdz2_ext_processor_base::start_share_immediates: share_immediates is already started (a stop_share_immediates() call is required).");
		return -1;
	}
	share_immediates_on = true;
	share_immediates_success = false;
	immediates_values.assign(values, values + value_count);
	immediates_shares.clear();

	if(0 != push_task(spdz2_ext_processor_base::op_code_share_immediates))
	{
		syslog(LOG_ERR, "spdz2_ext_processor_base::start_share_immediates: failed pushing a share_immediates task to queue.");
		return -1;
	}
	return 0;
}

//***********************************************************************************************//
int spdz2_ext_processor_base::stop_share_immediates(size_t * share_count, u_int64_t ** shares, const time_t timeout_sec)
{
	if(!share_immediates_on)
	{
		syslog(LOG_ERR, "spdz2_ext_processor_base::stop_share_immediates: share_immediates is not started.");
		return -1;
	}
	share_immediates_on = false;

	struct timespec timeout;
	clock_gettime(CLOCK_REALTIME, &timeout);
	timeout.tv_sec += timeout_sec;

	int result = sem_timedwait(&share_immediates_done, &timeout);
	if(0 != result)
	{
		result = errno;
		char errmsg[512];
		syslog(LOG_ERR, "spdz2_ext_processor_base::stop_share_immediates: sem_wait() failed with error %d : %s", result, strerror_r(result, errmsg, 512));
		return -1;
	}

	if(share_immediates_success)
	{
		if(!immediates_shares.empty())
		{
			*shares = new u_int64_t[*share_count = immediates_shares.size()];
			memcpy(*shares, &immediates_shares[0], *share_count * sizeof(u_int64_t));
		}
		return 0;
	}
	else
	{
		return -1;
	}
}

//***********************************************************************************************//
void spdz2_ext_processor_base::exec_share_immediates()
{
	share_immediates_success = protocol_share_immediates();
	sem_post(&share_immediates_done);
}

//***********************************************************************************************//
int spdz2_ext_processor_base::share_immediate(const u_int64_t value, u_int64_t * share, const time_t timeout_sec)
{
	p_immediate_share = share;
	immediate_value.clear();
	immediate_value.push_back(value);
	share_immediate_success = false;

	if(0 != push_task(spdz2_ext_processor_base::op_code_share_immediate))
	{
		syslog(LOG_ERR, "spdz2_ext_processor_base::share_immediate: failed pushing a share_immediate task to queue.");
		return -1;
	}

	struct timespec timeout;
	clock_gettime(CLOCK_REALTIME, &timeout);
	timeout.tv_sec += timeout_sec;

	int result = sem_timedwait(&share_immediate_done, &timeout);
	if(0 != result)
	{
		result = errno;
		char errmsg[512];
		syslog(LOG_ERR, "spdz2_ext_processor_base::share_immediate: sem_wait() failed with error %d : %s", result, strerror_r(result, errmsg, 512));
		return -1;
	}

	p_immediate_share = NULL;
	immediate_value.clear();

	return (share_immediate_success)? 0: -1;

}

//***********************************************************************************************//
void spdz2_ext_processor_base::exec_share_immediate()
{
	share_immediate_success = protocol_share_immediate();
	sem_post(&share_immediate_done);
}

//***********************************************************************************************//
