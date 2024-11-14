#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_ENTRIES 100

// Structure to store IP and MAC addresses
struct Entry 
{
    char ip_address[16];
    char mac_address[18];
};

int main() 
{

    system("arp -a > arptable.txt");

    FILE *file = fopen("arptable.txt", "r");
    if (file == NULL) 
    {
        perror("Error opening file");
        return 1;
    }

    // Array to store entries
    struct Entry entries[MAX_ENTRIES];
    int entry_count = 0;

    // Read lines from the file
    char line[100];
    while (fgets(line, sizeof(line), file) != NULL && entry_count < MAX_ENTRIES) 
    {
        // Parse the line to extract IP and MAC addresses
        char *ip_start = strchr(line, '(');
        char *ip_end = strchr(line, ')');
        char *mac_start = ip_end + 4;
        char *mac_end = strchr(mac_start, '[') - 1;

        if (ip_start && ip_end && mac_start && mac_end) 
        {
            ip_start++;
            strncpy(entries[entry_count].ip_address, ip_start, ip_end - ip_start);
            entries[entry_count].ip_address[ip_end - ip_start] = '\0';

            mac_start++;
            strncpy(entries[entry_count].mac_address, mac_start, mac_end - mac_start);
            entries[entry_count].mac_address[mac_end - mac_start] = '\0';

            entry_count++;
        }
    }

    // Close the file
    fclose(file);

    // Print the extracted data
    for (int i = 0; i < entry_count; i++) 
    {
        //system("sudo ./myrpc %s %s", entries[i].ip_address,  entries[(i+1)%entry_count].mac_address);
        //printf("IP Address: %s\n", entries[i].ip_address);
        //printf("MAC Address: %s\n", entries[i].mac_address);
        //printf("\n");
        int c=fork();
        if(c==0)
        {
            char *args[] = {"sudo", "./myarp", entries[i].ip_address, entries[(i+1)%entry_count].mac_address, NULL};
            //doesnt work due to sudo perms issue
            //execvp(args[0], args);
        }
    }

    return 0;
}
