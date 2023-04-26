#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080 // Port number for the server
#define MAX 1024 // Maximum size of the data buffer
#define FLAG "01111110" // Flag pattern for bit stuffing

// Function to perform bit destuffing on a given data string
char* bitDestuffing(char* data) {
    // Initialize a new string to store the destuffed data
    char* destuffed = (char*)malloc(MAX * sizeof(char));
    int count = 0; // Count the number of consecutive 1 bits
    int j = 0; // Index for the destuffed string
    // Traverse the data string and add each bit to the destuffed string except the flag and the extra bits
    for (int i = strlen(FLAG); i < strlen(data) - strlen(FLAG); i++) {
        if (data[i] == '1') { // If the current bit is 1
            count++; // Increment the count
            destuffed[j] = data[i]; // Add the current bit to the destuffed string
            j++; // Increment the index for the destuffed string
        } else { // If the current bit is 0
            if (count == 5) { // If there are 5 consecutive 1 bits before this bit
                count = 0; // Reset the count
                continue; // Skip this bit as it is an extra bit added during stuffing
            } else { // If there are less than 5 consecutive 1 bits before this bit
                count = 0; // Reset the count
                destuffed[j] = data[i]; // Add the current bit to the destuffed string
                j++; // Increment the index for the destuffed string
            }
        }
    }
    destuffed[j] = '\0'; // Add a null character at the end of the destuffed string
    return destuffed; // Return the destuffed string
}

// Main function
int main() {
    int sock = 0; // File descriptor for client socket
    struct sockaddr_in serv_addr; // Address structure for server socket
    char buffer[MAX] = {0}; // Buffer to store the data from server

    // Create a socket file descriptor for client socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed"); // Print an error message if socket creation fails
        exit(EXIT_FAILURE); // Exit the program with failure status
    }

    serv_addr.sin_family = AF_INET; // Set the address family to IPv4
    serv_addr.sin_port = htons(PORT); // Set the port number to the predefined value

    char* ip_address = "127.0.0.1"; // IP address of the server (localhost)

    printf("Connecting to server at %s:%d\n", ip_address, PORT); // Print a message indicating that client is connecting to server

    if (inet_pton(AF_INET, ip_address, &serv_addr.sin_addr) <= 0) {
        perror("invalid address"); // Print an error message if IP address is invalid or not supported 
        exit(EXIT_FAILURE); // Exit the program with failure status
     }

    // Connect to the server socket using the specified address and port number
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connection failed"); // Print an error message if connection fails
        exit(EXIT_FAILURE); // Exit the program with failure status
    }

    printf("Connection established with server\n"); // Print a message indicating that connection is established

    char* data; // Variable to store the data to be sent to server

    while (1) { // Loop until user enters an empty message

        printf("Enter data to be sent to server: "); // Prompt the user to enter data
        data = (char*)malloc(MAX * sizeof(char)); // Allocate memory for the data
        scanf("%s", data); // Read the data from standard input

        if (strlen(data) == 0) { // If user enters an empty message
            printf("Connection closed by user\n"); // Print a message indicating that connection is closed
            break; // Break out of the loop
        }

        send(sock, data, strlen(data), 0); // Send the data to the server

        printf("Data sent to server: %s\n", data); // Print the data sent to server

        memset(buffer, 0, MAX); // Clear the buffer

        read(sock, buffer, MAX); // Read the stuffed data from server

        printf("Stuffed data received from server: %s\n", buffer); // Print the stuffed data received from server

        char* destuffed = bitDestuffing(buffer); // Perform bit destuffing on the stuffed data

        printf("Destuffed data: %s\n", destuffed); // Print the destuffed data

        free(data); // Free the memory allocated for the data
        free(destuffed); // Free the memory allocated for the destuffed string
    }

    close(sock); // Close the client socket

    return 0; // Return 0 to indicate successful termination
}
