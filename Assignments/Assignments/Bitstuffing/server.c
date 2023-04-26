#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080 // Port number for the server
#define MAX 1024 // Maximum size of the data buffer
#define FLAG "01111110" // Flag pattern for bit stuffing

// Function to perform bit stuffing on a given data string
char* bitStuffing(char* data) {
    // Initialize a new string to store the stuffed data
    char* stuffed = (char*)malloc(MAX * sizeof(char));
    strcpy(stuffed, FLAG); // Add the flag at the beginning
    int count = 0; // Count the number of consecutive 1 bits
    // Traverse the data string and add each bit to the stuffed string
    for (int i = 0; i < strlen(data); i++) {
        stuffed[strlen(stuffed)] = data[i]; // Add the current bit
        if (data[i] == '1') { // If the current bit is 1
            count++; // Increment the count
            if (count == 5) { // If there are 5 consecutive 1 bits
                stuffed[strlen(stuffed)] = '0'; // Add a 0 bit after them
                count = 0; // Reset the count
            }
        } else { // If the current bit is 0
            count = 0; // Reset the count
        }
    }
    strcat(stuffed, FLAG); // Add the flag at the end
    return stuffed; // Return the stuffed string
}

// Main function
int main() {
    int server_fd, new_socket; // File descriptors for server and client sockets
    struct sockaddr_in address; // Address structure for server socket
    int opt = 1; // Option for setting socket options
    int addrlen = sizeof(address); // Size of address structure
    char buffer[MAX] = {0}; // Buffer to store the data from client

    // Create a socket file descriptor for server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed"); // Print an error message if socket creation fails
        exit(EXIT_FAILURE); // Exit the program with failure status
    }

    // Set socket options to allow multiple connections and reuse port number
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt"); // Print an error message if setting socket options fails
        exit(EXIT_FAILURE); // Exit the program with failure status
    }

    address.sin_family = AF_INET; // Set the address family to IPv4
    address.sin_addr.s_addr = INADDR_ANY; // Set the IP address to any interface
    address.sin_port = htons(PORT); // Set the port number to the predefined value

    // Bind the server socket to the specified address and port number
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed"); // Print an error message if binding fails
        exit(EXIT_FAILURE); // Exit the program with failure status
    }

    // Listen for incoming connections on the server socket with a maximum queue of 3
    if (listen(server_fd, 3) < 0) {
        perror("listen"); // Print an error message if listening fails
        exit(EXIT_FAILURE); // Exit the program with failure status
    }

    printf("Server is listening on port %d\n", PORT); // Print a message indicating that server is listening

    while (1) { // Loop forever until terminated by user

        // Accept a connection from a client socket and get its file descriptor
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept"); // Print an error message if accepting fails
            exit(EXIT_FAILURE); // Exit the program with failure status
        }

        printf("Connection accepted from client\n"); // Print a message indicating that connection is accepted

        int valread; // Variable to store the number of bytes read from client

        while (1) { // Loop until client closes connection or sends an empty message

            memset(buffer, 0, MAX); // Clear the buffer
                        // Read data from client socket and store it in buffer
            valread = read(new_socket, buffer, MAX);

            if (valread == 0) { // If client closes connection or sends an empty message
                printf("Connection closed by client\n"); // Print a message indicating that connection is closed
                break; // Break out of the inner loop
            }

            printf("Data received from client: %s\n", buffer); // Print the data received from client

            char* stuffed = bitStuffing(buffer); // Perform bit stuffing on the data

            printf("Stuffed data: %s\n", stuffed); // Print the stuffed data

            send(new_socket, stuffed, strlen(stuffed), 0); // Send the stuffed data back to the client

            printf("Stuffed data sent to client\n"); // Print a message indicating that stuffed data is sent

            free(stuffed); // Free the memory allocated for the stuffed string
        }

        close(new_socket); // Close the client socket
    }

    return 0; // Return 0 to indicate successful termination
}
