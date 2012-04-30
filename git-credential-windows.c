#include <windows.h>
#include <wincred.h>
#include <stdio.h>
#include <fcntl.h>
#pragma comment(lib, "advapi32")

#define MAX_LINE_LEN 1024
#define MAX_VALUE_LEN 128

int get(char *repository, char *username, size_t maxusername, char *password, size_t maxpassword);
void store(char *repository, char *username, char *password);
void erase(char *repository);

int main(int argc, char *argv[]) {
    char line[MAX_LINE_LEN];
    char url[MAX_LINE_LEN];
    enum { protocol, host, path, username, password, numkeys };
    char *keys[] = {"protocol", "host", "path", "username", "password"};
    char values[numkeys][MAX_VALUE_LEN] = {"", "", "", "", ""};
    char *cmd;
    _setmode(_fileno(stdin), _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
    if (argc == 2)
        cmd = argv[1];
    else {
        fprintf(stderr, "usage: %s {get|store|erase}\n", argv[0]);
        return 1;
    }
    while(fgets(line, sizeof(line), stdin)) {
        int i, state = 1, key = -1;
        char *t;
        for (t = strtok(line, "="); t; t = strtok(NULL, "\n"))
            switch (state) {
                case 1: 
                    for(i = 0; i < numkeys; ++i)
                        if (!strcmp(t, keys[i])) {
                            key = i;
                            break;
                        }
                    state = key < 0 ? 0 : 2;
                    break;
                case 2: 
                    strncpy(values[key], t, MAX_VALUE_LEN);
                    state = 3;
                    break;
                case 3: 
                    state = 4;
            }
        if (state != 0 && state != 3) {
            fprintf(stderr, "Unexpected input: %s\n", line);
            return 1;
        }
    }
    _snprintf(url, sizeof(url), "%s://%s/%s", values[protocol], values[host], values[path]);
    if (!strcmp(cmd, "get")) {
        if (get(url, values[username], sizeof(values[username]), values[password], sizeof(values[password])))
            printf("%s=%s\n%s=%s\n", keys[username], values[username], keys[password], values[password]);
    } else if (!strcmp(cmd, "store"))
        store(url, values[username], values[password]);
    else if (!strcmp(cmd, "erase"))
        erase(url);
}

int get(char *repository, char *username, size_t maxusername, char *password, size_t maxpassword) {
    PCREDENTIAL pcred;
    if (CredRead(repository, CRED_TYPE_GENERIC, 0, &pcred)) {
        strncpy(username, pcred->UserName, maxusername);
        strncpy(password, (char*)pcred->CredentialBlob, maxpassword);
        CredFree(pcred);
        return 1;
    } else
        return 0;
}
    
void store(char *repository, char *username, char *password) {
    CREDENTIAL cred;
    cred.Flags = 0; 
    cred.Type = CRED_TYPE_GENERIC; 
    cred.TargetName = repository; 
    cred.Comment = NULL; 
    cred.LastWritten.dwLowDateTime = 0; 
    cred.LastWritten.dwHighDateTime = 0; 
    cred.CredentialBlobSize = strlen(password) + 1; 
    cred.CredentialBlob = (LPBYTE)password;
    cred.Persist = CRED_PERSIST_LOCAL_MACHINE; 
    cred.AttributeCount = 0; 
    cred.Attributes = NULL; 
    cred.TargetAlias = NULL; 
    cred.UserName = username;
    if (!CredWrite(&cred, 0)) {
        fprintf(stderr, "Unexpected error storing credentials: %08x\n", GetLastError());
        exit(1);
    }
}

void erase(char *repository) {
    CredDelete(repository, CRED_TYPE_GENERIC, 0);
}
