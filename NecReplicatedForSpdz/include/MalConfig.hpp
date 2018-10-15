//
// Created by adima on 26/09/16.
//

#ifndef INC_20160901_MALICIOUSMPC_DOUBLEBUFFER_MALCONFIG_H
#define INC_20160901_MALICIOUSMPC_DOUBLEBUFFER_MALCONFIG_H

#include <string>
#include <iostream>

using namespace std;

// types of security guarantees
enum SecurityType {SEMI_HONEST = 0, MALICIOUS = 1};

// modes of online-offline running:
enum OnlineType {ONLINE_OFFLINE = 0, ONLINE_ONLY = 1};

// ways of shuffling the helper array in online verification:
enum OnlineShuffleType {NO_SHUFFLE_ONLINE = 0, SHUFFLE_ONLINE = 1};

// ways of shuffling the helper arrays in beaverTriples generation:
enum BTShuffleType {NO_SHUFFLE_BT = 0, SHUFFLE_INDICES = 1, SHUFFLE_DATA = 2};

// types of hash to verify the "view" arrays:
enum HashType {AES_GCM = 0, SHA_256 = 1};

enum VerifyType {VERIFY_AFTER_ALL_ROUND = 0, VERIFY_EACH_ROUND = 1};


// singleton class, constructor is private, instead there is a public "instance" method
class MalConfig {

public:
    // get the singleton instance
    static MalConfig& instance();
    static MalConfig& instance(char const *confPath);

    ~MalConfig();

    // configurations getters:
    SecurityType getSecurityType() { return _securityType;}
    int getBucketSize() { return _bucketSize;}
    OnlineType getOnlineType() { return _onlineType;}
    OnlineShuffleType getOnlineShuffleType() { return _onlineShuffleType;}
    BTShuffleType getBTShuffleType() { return _BTShuffleType;}
    HashType getHashType() { return _hashType;}
    VerifyType getVerifyType() { return _verifyType; }
  

    //members for printing:
    static const string SECURITY_STR[];
    static const string ONLINE_STR[];
    static const string ONLINE_SHUFFLE_STR[];
    static const string BT_SHUFFLE_STR[];
    static const string HASH_STR[];
    static const string VERIFY_STR[];


private:
    // private constructor
    MalConfig(char const *confPath);

    // parsing methods:
    string _getNextLine(ifstream& infile);

    void _parseSecurityType(const string &line);
    void _parseBucketSize(const string &line);
    void _parseOnlineType(const string &line);
    void _parseBTShuffleType(const string &line);
    void _parseHashType(const string &line);
    void _parseVerifyType(const string &line);


    // the singleton instance
    static MalConfig *_configInstance;

    // configuration options
    SecurityType _securityType;             /* semi-honest or malicious */
    int _bucketSize;                        /* B >= 2, bucket size for beaver triples verification */
    OnlineType _onlineType;                 /* ONLINE_OFFLINE or ONLINE_ONLY */
    OnlineShuffleType _onlineShuffleType;   /* NO_SHUFFLE_ONLINE or SHUFFLE_ONLINE */
    BTShuffleType _BTShuffleType;           /* NO_SHUFFLE_BT or SHUFFLE_INDICES or SHUFFLE_DATA */
    HashType _hashType;                     /* AES_GCM or SHA_256 */
    VerifyType _verifyType;                 /* VERIFY_AFTER_ALL_ROUND or VERIFY_EACH_ROUND */

};


#endif //INC_20160901_MALICIOUSMPC_DOUBLEBUFFER_MALCONFIG_H
