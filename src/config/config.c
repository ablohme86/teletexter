// Written by S.L Blohme
#include "../include/config/config.h"
#include "../include/log.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

struct Config config = {0}; // Define config here

// Utility function to trim newline characters
void trim_newline(char *str)
{
    str[strcspn(str, "\n")] = 0;
}

// Check if the line is a comment or empty line
int is_comment_or_empty(const char *line)
{
    if (line[0] == '\n' || line[0] == '#')
    {
        return 1; // Line is a comment or empty
    }
    return 0; // Line is not a comment or empty
}

// Add the config variables
const char *valid_keywords[] = {
    "ServerPort",
    "MaxClients",
    "MaxMessageLength",
    "LayoutFile", 
    "MaxNicknameLength",
    "ScrollLongMessages",
    "ScrollSpeed",
    "DatabaseFile",
    "SystemLogPath",
    "WhiteList",
    "BlackList",
    "LCDCols",
    "LCDRows",
    "LCDAddress",
    "LCDDeviceFile",
    "LCDNoBacklight",
    "ServerBindAddress",
    "LCDBacklight",
    "LCDLine1",
    "LCDLine2",
    "LCDLine3",
    "LCDLine4",
    "LCDClrDisp",
    "LCDEnableBit",
    "ServerIdentifier"
    
    
};



int is_valid_keyword(const char *key)
{
    for (size_t i = 0; i < sizeof(valid_keywords) / sizeof(valid_keywords[0]); i++)
    {
        if (strcmp(valid_keywords[i], key) == 0)
        {
            return 1; // Valid keyword found
        }
    }
    return 0; // Not a valid keyword
}

void loadConfig(const char *filename) 
{
    fprintf(stdout,"Loading configuration file %s\n",filename);
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr,"Cannot open configuration file %s!\n", filename);
        exit(EXIT_FAILURE);
    }

    char line[256];
    int line_number = 0;
    while (fgets(line, sizeof(line), file))
    {
        line_number++;

        // Skip empty lines and comments
        if (is_comment_or_empty(line))
        {
            continue;
        }

        // Remove trailing newline character
        trim_newline(line);

        // Split line into key and value
        char *key = strtok(line, " ");
        char *value = strtok(NULL, "\n");
        
        // Do some silly-simple syntax-error check!
        if (key == NULL || value == NULL)
        {
            fprintf(stderr,"Configuration syntax error: line %d: %s\n", line_number,line);
            exit(EXIT_FAILURE);
        }

        if (!is_valid_keyword(key))
        {
            fprintf(stderr, "Invalid keyword '%s' in configuration file at line %d\n", key, line_number);
            exit(EXIT_FAILURE);
        }
        
        // Parse each configuration parameter based on da keyz, more to come..
        if (strcmp(key, "ServerPort") == 0) {
            config.serverConfig.port = atoi(value);
        }
        else if (strcmp(key, "MaxClients") == 0)
        {
            config.serverConfig.maxClients = atoi(value);
        }
        else if (strcmp(key,"DatabaseFile") == 0)
        {
            strncpy(config.serverConfig.dbFile,value,MAX_FILENAME_LENGTH);
        }
        else if (strcmp(key,"ScrollLongMessages") == 0)
        {
            config.messageConfig.scrollLongMessages = atoi(value);
        }
        else if (strcmp(key,"ServerBindAddress") == 0)
        {
            strncpy(config.serverConfig.serverHost, value, MAX_IDENTIFIER_LENGTH);
        }

        else if (strcmp(key,"ServerIdentifier") == 0)
        {
            strncpy(config.serverConfig.serverIdentifier, value, MAX_IDENTIFIER_LENGTH);
        }
        else if (strcmp(key,"ScrollSpeed") == 0)
        {
            config.messageConfig.scrollSpeed = atoi(value);
        }
        else if (strcmp(key, "MaxMessageLength") == 0)
        {
            config.messageConfig.maxMessageLength = atoi(value);
        }
        else if (strcmp(key, "LayoutFile") == 0)
        {
            strncpy(config.messageConfig.layoutFile, value, MAX_FILENAME_LENGTH);
        }
        else if (strcmp(key, "MaxNicknameLength") == 0)
        {
            config.userConfig.maxNicknameLength = atoi(value);
        }
        else if (strcmp(key, "SystemLogPath") == 0)
        {
            strncpy(config.loggingConfig.systemLogPath, value, MAX_PATH_LENGTH);
        }
        // Ikke rart vi ikke fikk noe???
        else if (strcmp(key, "LCDCols") == 0)   // Antall linjer i høyden
        {
            config.lcdConfig.lcdHeight = atoi(value);
        }
        else if (strcmp(key, "LCDRows") == 0)   // Antall linjer bortover på linja
        {
            config.lcdConfig.lcdWidth = atoi(value);
        } 
        else if (strcmp(key, "LCDAddress") == 0)
        {
            config.lcdConfig.lcdAddress = (uint8_t)strtol(value, NULL, 16);  
        }
        else if (strcmp(key, "LCDDeviceFile") == 0)
        {
            strncpy(config.lcdConfig.lcdDeviceFile, value, MAX_PATH_LENGTH);
        }  
        else if (strcmp(key, "LCDLine1") == 0)
        {
            config.lcdConfig.lcdLine1Addr = (uint8_t)strtol(value, NULL, 16);  
        }        
        else if (strcmp(key, "LCDLine2") == 0)
        {
            config.lcdConfig.lcdLine2Addr = (uint8_t)strtol(value, NULL, 16);  
        }  
        
        else if (strcmp(key, "BlackList") == 0)
        {
            strncpy(config.serverConfig.blackListFile, value, MAX_PATH_LENGTH);
        }  
        else if (strcmp(key, "WhiteList") == 0)
        {
            strncpy(config.serverConfig.whiteListFile, value, MAX_PATH_LENGTH);
        }  
        else if (strcmp(key, "LCDLine3") == 0)
        {
            config.lcdConfig.lcdLine3Addr = (uint8_t)strtol(value, NULL, 16);  
        }  
        else if (strcmp(key, "LCDLine4") == 0)
        {
            config.lcdConfig.lcdLine4Addr = (uint8_t)strtol(value, NULL, 16);  
        }
        
        else if (strcmp(key, "LCDNoBacklight") == 0)
        {
            config.lcdConfig.lcdNoBacklight = (uint8_t)strtol(value, NULL, 16);  
        }  
        else if (strcmp(key, "LCDBacklight") == 0)
        {
            config.lcdConfig.lcdBacklight = (uint8_t)strtol(value, NULL, 16);  
        }                       
        else if (strcmp(key, "LCDEnableBit") == 0)
        {
            config.lcdConfig.lcdEnableBit = (uint8_t)strtol(value, NULL, 16);  
        }                     
    }

    fclose(file);
}
