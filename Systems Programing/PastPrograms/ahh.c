#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdint.h>
#include <string.h>

// Define constants
#define PACKET_SIZE 20
#define OUTPUT_FILE "data.dat"
#define NUM_VALUES_PER_PACKET 4 // Assuming 4 double values per packet

// Error checking function
int errorCheck(int val, const char *msg) {
    if (val == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}

// Function to decompress data
void decompressData(int16_t *compressed, double *decompressed, int numValues) {
    // Simple decompression algorithm: For illustration only
    for (int i = 0; i < numValues; i++) {
        decompressed[i] = (double)compressed[i] / 100.0; // Example decompression
    }
}

int main() {
    int inputFd, outputFd;
    ssize_t bytesRead;
    uint8_t packet[PACKET_SIZE];
    double decompressedValues[NUM_VALUES_PER_PACKET];

    // Open the binary file for reading
    inputFd = errorCheck(open("raw.dat", O_RDONLY), "open for read");

    // Open the binary file for writing
    outputFd = errorCheck(open(OUTPUT_FILE, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR), "open for write");

    printf("Processing Data:\n");
    printf("_____________________________________\n");

    // Process each 20-byte packet
    while ((bytesRead = read(inputFd, packet, PACKET_SIZE)) > 0) {
        if (bytesRead != PACKET_SIZE) {
            fprintf(stderr, "Warning: Incomplete packet read\n");
            continue;
        }

        // Example decompression: Assuming each packet contains 4 pairs of 2-byte integers
        int16_t compressedData[NUM_VALUES_PER_PACKET];
        memcpy(compressedData, packet, sizeof(compressedData));

        // Decompress the data
        decompressData(compressedData, decompressedValues, NUM_VALUES_PER_PACKET);

        // Write decompressed data to binary output file
        errorCheck(write(outputFd, decompressedValues, sizeof(decompressedValues)), "write to output file");

        // Print decompressed data to console
        for (int i = 0; i < NUM_VALUES_PER_PACKET; i++) {
            printf("Value %d: %f\n", i, decompressedValues[i]);
        }
    }

    if (bytesRead == -1) {
        perror("read");
    }

    // Close file descriptors
    close(inputFd);
    close(outputFd);

    return EXIT_SUCCESS;
}