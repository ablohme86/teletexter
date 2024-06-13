// Written by S.L Blohme
#include "../include/config.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>

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
    "Port",
    "MaxClients",
    "MaxMessageLength",
    "DisplayTextLayout", 
    "MaxNicknameLength",
    "MustIdentify", "EnablePassword",
    "UserFilePath",
    "MessageLogPath",
    "SystemLogPath",
    "LCDColumns",
    "LCDRows",
    "LCDAddress",
    "LCDDeviceFile"
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
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Error opening configuration file");
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
            fprintf(stderr, "Configuration syntax: line %d: %s\n", line_number,line);
            exit(EXIT_FAILURE);
        }

        if (!is_valid_keyword(key))
        {
            fprintf(stderr, "Invalid keyword '%s' in configuration file at line %d\n", key, line_number);
            exit(EXIT_FAILURE);
        }
        
        // Parse each configuration parameter based on da keyz, more to come..
        if (strcmp(key, "Port") == 0) {
            config.serverConfig.port = atoi(value);
        }
        else if (strcmp(key, "MaxClients") == 0)
        {
            config.serverConfig.maxClients = atoi(value);
        }
        else if (strcmp(key, "MaxMessageLength") == 0)
        {
            config.messageConfig.maxMessageLength = atoi(value);
        }
        else if (strcmp(key, "DisplayTextLayout") == 0)
        {
            strncpy(config.messageConfig.displayTextLayout, value, MAX_FILENAME_LENGTH);
        }
        else if (strcmp(key, "MaxNicknameLength") == 0)
        {
            config.userConfig.maxNicknameLength = atoi(value);
        }
        else if (strcmp(key, "MustIdentify") == 0)
        {
            config.userConfig.mustIdentify = atoi(value);
        }
        else if (strcmp(key, "EnablePassword") == 0)
        {
            config.userConfig.enablePassword = atoi(value);
        }
        else if (strcmp(key, "UserFilePath") == 0)
        {
            strncpy(config.userConfig.userFilePath, value, MAX_PATH_LENGTH);
        }
        else if (strcmp(key, "MessageLogPath") == 0)
        {
            strncpy(config.loggingConfig.messageLogPath, value, MAX_PATH_LENGTH);
        }
        else if (strcmp(key, "SystemLogPath") == 0)
        {
            strncpy(config.loggingConfig.systemLogPath, value, MAX_PATH_LENGTH);
        }
        else if (strcmp(key, "LCDColumns") == 0)
        {
            config.lcdConfig.lcdColumns = atoi(value);
        }
        else if (strcmp(key, "LCDRows") == 0)
        {
            config.lcdConfig.lcdRows = atoi(value);
        } 
        else if (strcmp(key, "LCDAddress") == 0)
        {
            config.lcdConfig.lcdAddress = (uint8_t)strtol(value, NULL, 16);
            
        }
        else if (strcmp(key, "LCDDeviceFile") == 0)
        {
            strncpy(config.lcdConfig.lcdDeviceFile, value, MAX_PATH_LENGTH);
        }        
    }

    fclose(file);
}
