//
// Created by adima on 26/09/16.
//

#include <fstream>
#include "MalConfig.hpp"

// strings for informative printing on the current configuration
const string MalConfig::SECURITY_STR[] = {"SEMI_HONEST", "MALICIOUS"};
const string MalConfig::ONLINE_STR[] = {"ONLINE_OFFLINE", "ONLINE_ONLY"};
const string MalConfig::ONLINE_SHUFFLE_STR[] = {"NO_SHUFFLE_ONLINE", "SHUFFLE_ONLINE"};
const string MalConfig::BT_SHUFFLE_STR[] = {"NO_SHUFFLE_BT", "SHUFFLE_INDICES", "SHUFFLE_DATA"};
const string MalConfig::HASH_STR[] = {"AES_GCM", "SHA_256"};
const string MalConfig::VERIFY_STR[] = {"VERIFY_AFTER_ALL_ROUND", "VERIFY_EACH_ROUND"};


// initialize the singleton instance to null before the constructor is called.
MalConfig * MalConfig::_configInstance = nullptr;


// Instance function without config path.
// This version should be called only after a call to the "instance" with path parameter
MalConfig& MalConfig::instance()
{
    if(!_configInstance)
    {
        cout << "[MalConfig::instance] instance is null, call the instance method with config path before" << endl;
        throw(-1);
    }
    return *_configInstance;
}


// instance function, calls the constructor to create single instance
MalConfig& MalConfig::instance(char const *confPath)
{
    if (!_configInstance)
    {
        _configInstance = new MalConfig(confPath);
    }
    return (*_configInstance);
}


// Empty Dtor
MalConfig::~MalConfig() { }



// ---------------------------------------- PRIVATE METHODS --------------------------------
/**
 * Ctor- private for singleton implementation
 * @param confPath path of the configuration file
 */
MalConfig::MalConfig(char const *confPath)
{
    //open file
    ifstream infile(confPath);
    if(!infile) {
        cout << "[MalConfig] Problem with opening " << confPath << endl;
        throw(-1);
    }

    // parse security type:
    _parseSecurityType(_getNextLine(infile));

    // continue parsing malicious parameters:
    if (_securityType == MALICIOUS)
    {
        // parse bucket size:
        _parseBucketSize(_getNextLine(infile));

        // parse online type:
        _parseOnlineType(_getNextLine(infile));

        // parse shuffle type:
        _parseBTShuffleType(_getNextLine(infile));

        // parse hash type:
        _parseHashType(_getNextLine(infile));

	_parseVerifyType(_getNextLine(infile));
    }
}



// Get next configuration line, ignoring comment lines (lines that start with '#' char)
string MalConfig::_getNextLine(ifstream& infile)
{
    const string COMMENT_MARK = "#";
    string line;
    bool comment = true;

    while (comment == true)
    {
        getline(infile, line);
        // ignore comments or empty lines:
        if (!line.empty() and line.find(COMMENT_MARK) != 0)
        {
            comment = false;
        }
    }

    return line;
}



// parse security type
void MalConfig::_parseSecurityType(const string &line)
{
    const string SEMI = "SEMI_HONEST";
    const string MAL = "MALICIOUS";

    if (line == SEMI) {
        _securityType = SEMI_HONEST;
    }
    else if(line == MAL) {
        _securityType = MALICIOUS;
    }
    else {
        cout << "[MalConfig] **WARNING** first parameter in conf file should be security type "
                "(SEMI_HONEST or MALICIOUS)..." << endl;
        cout << "CHOOSING DEFAULT CONFIG: SEMI_HONEST" << endl;
        _securityType = SEMI_HONEST;
    }
}



// parse B (bucket size), and set the shuffle type
void MalConfig::_parseBucketSize(const string &line)
{
    // try to parse the line value as an int, line should be: "B=x" where x is an int.
    int b = stoi(line.substr(2), nullptr);

    if (b >= 2) {
        _bucketSize = b;
    }
    else {
        cout << "[MalConfig] **WARNING** in MALICIOUS configuration the second parameter in versions.conf file "
                "should be bucket size (format 'B=x', x >= 2)..." << endl;
        cout << "CHOOSING DEFAULT CONFIG: B=2" << endl;
        _bucketSize = 2;
    }

    // set online-shuffle-type:
    if (_bucketSize == 2)
    {
        // bucket size of 2 requires shuffling in the online protocol
        _onlineShuffleType = SHUFFLE_ONLINE;
    }
    else
    {
        // bucket size larger than 2 allows no-shuffling in the online protocol
        _onlineShuffleType = NO_SHUFFLE_ONLINE;
    }
}



// parse online type
// In malicious mode the online can run either in parallel with "offline" BeaverTriples generator,
// or "online only" mode that generates BeaverTriples only once, and consume them over and over.
void MalConfig::_parseOnlineType(const string &line)
{
    const string OFFLINE = "ONLINE_OFFLINE";
    const string ONLY = "ONLINE_ONLY";

    if (line == OFFLINE) {
        _onlineType = ONLINE_OFFLINE;
    }
    else if (line == ONLY)
    {
        _onlineType = ONLINE_ONLY;
    }
    else {
        cout << "[MalConfig] **WARNING** in MALICIOUS configuration the 3rd parameter in versions.conf file "
                "should be online type (ONLINE_OFFLINE or ONLINE_ONLY)..." << endl;
        cout << "CHOOSING DEFAULT CONFIG: ONLINE_OFFLINE" << endl;
        _onlineType = ONLINE_OFFLINE;
    }
}



// parse shuffle type
void MalConfig::_parseBTShuffleType(const string &line)
{
    const string INDICES = "SHUFFLE_INDICES";
    const string DATA = "SHUFFLE_DATA";

    if (line == INDICES) {
        _BTShuffleType = SHUFFLE_INDICES;
    }
    else if (line == DATA)
    {
        _BTShuffleType = SHUFFLE_DATA;
    }
    else {
        cout << "[MalConfig] **WARNING** in MALICIOUS configuration the 4th parameter in versions.conf file "
                "should be shuffle type (SHUFFLE_INDICES or SHUFFLE_DATA)..." << endl;
        cout << "CHOOSING DEFAULT CONFIG: SHUFFLE_INDICES" << endl;
        _BTShuffleType = SHUFFLE_INDICES;
    }
}



// parse hashType
void MalConfig::_parseHashType(const string &line)
{
    const string GCM = "AES_GCM";
    const string SHA = "SHA_256";

    if (line == GCM) {
        _hashType = AES_GCM;
    }
    else if(line == SHA) {
        _hashType = SHA_256;
    }
    else {
        cout << "[MalConfig] **WARNING** in MALICIOUS configuration the 5th parameter in versions.conf file "
                "should be hash type (AES_GCM or SHA_256)..." << endl;
        cout << "CHOOSING DEFAULT CONFIG: AES_GCM" << endl;
        _hashType = AES_GCM;
    }
}


// parse verifyType
void MalConfig::_parseVerifyType(const string &line)
{
    const string ALL = "VERIFY_AFTER_ALL_ROUND";
    const string EACH = "VERIFY_EACH_ROUND";

    if (line == ALL) {
        _verifyType = VERIFY_AFTER_ALL_ROUND;
    }
    else if(line == EACH) {
        _verifyType = VERIFY_EACH_ROUND;
    }
    else {
        cout << "[MalConfig] **WARNING** in MALICIOUS configuration the 6th parameter in versions.conf file "
                "should be verification type (VERIFY_AFTER_ALL_ROUND or VERIFY_EACH_ROUND)..." << endl;
        cout << "CHOOSING DEFAULT CONFIG: VERIFY_AFTER_ALL_ROUND" << endl;
        _verifyType = VERIFY_AFTER_ALL_ROUND;
    }
}
