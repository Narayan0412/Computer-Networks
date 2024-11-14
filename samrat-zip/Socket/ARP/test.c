#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void ipStringToArray(const char *ipString, int *ipArray) {
    // Initialize variables
    char octet[4] = {'\0'};
    int index = 0;

    // Loop through the IP address string
    for (int i = 0; i <= strlen(ipString); i++) {
        // Ignore dots in the string
        if (ipString[i] != '.' && ipString[i] != '\0') {
            // Copy characters into octet
            octet[index++] = ipString[i];
        } else {
            // If characters are read, convert to integer and store in the array
            if (index > 0) {
                ipArray[i / 4] = atoi(octet);
                // Reset the index for the next octet
                index = 0;
            }
        }
    }
}

int main() {
    const char *ipString = "10.42.0.162";
    int ipArray[4];

    // Convert IP address string to array
    ipStringToArray(ipString, ipArray);

    // Print the resulting array
    printf("IP Address Array: ");
    for (int i = 0; i < 4; i++) {
        printf("%d ", ipArray[i]);
    }
    printf("\n");

    return 0;
}
