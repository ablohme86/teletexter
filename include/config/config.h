// Written by; S.L Blohmé, 2024
#include <stdint.h>

#ifndef CONFIG_H
#define CONFIG_H

#define MAX_FILENAME_LENGTH 100
#define MAX_PATH_LENGTH 100
#define MAX_IDENTIFIER_LENGTH 100

struct ServerConfig {
    int port;
    int maxClients;
    char banList[MAX_FILENAME_LENGTH];
    char whiteList[MAX_FILENAME_LENGTH];
    char dbFile[MAX_FILENAME_LENGTH];
    char serverIdentifier[MAX_IDENTIFIER_LENGTH];


};

struct MessageConfig {
    int maxMessageLength;
    char layoutFile[MAX_FILENAME_LENGTH];
    unsigned int scrollLongMessages;
    unsigned int scrollSpeed;
};

struct UserConfig {
    int maxNicknameLength;
    int enablePassword;
    char userFilePath[MAX_PATH_LENGTH];

};

struct LoggingConfig {
    char messageLogPath[MAX_PATH_LENGTH];
    char systemLogPath[MAX_PATH_LENGTH];
};

struct LCDConfig {
    uint8_t lcdAddress; // Assuming address can be a string
    char lcdDeviceFile[MAX_FILENAME_LENGTH];
    uint8_t lcdClrDisp;
    uint8_t lcdEnableBit;
    unsigned int lcdWidth;
    unsigned int lcdHeight;
    uint8_t lcdLine1Addr;
    uint8_t lcdLine2Addr;
    uint8_t lcdLine3Addr;
    uint8_t lcdLine4Addr;      
    uint8_t lcdBacklight;
    uint8_t lcdNoBacklight;
      
};

struct Config {
    struct ServerConfig serverConfig;
    struct MessageConfig messageConfig;
    struct UserConfig userConfig;
    struct LoggingConfig loggingConfig;
    struct LCDConfig lcdConfig;
};

// Declare config as extern to avoid multiple definitions
// it will be initialized in config.c laterz on!
extern struct Config config;

void loadConfig(const char *filename);

#endif /* CONFIG_H */
