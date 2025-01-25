#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define LINE_SIZE 1024

// Function to normalize: shift by the negative max value
void normalize(double *data, int size) {
    double max_value = -INFINITY;
    for (int i = 0; i < size; i++) {
        if (data[i] > max_value) {
            max_value = data[i];
        }
    }
    for (int i = 0; i < size; i++) {
        data[i] = -(data[i] - max_value); // Shift values by max
    }
}

int main(int argc, char* argv[]) {
    const char *file_name; // Pointer to the file name string

    if (argc > 1) {
        file_name = argv[1]; // Point to the command-line argument
    } else {
        static char temp_file_name[255]; // Temporary buffer for user input
        printf("Enter file name: ");
        scanf("%254s", temp_file_name); // Read input safely (limit size)
        file_name = temp_file_name;     // Point to the user-provided file name
    }

    // Try to open the file for reading
    FILE *input = fopen(file_name, "r");
    
    if (input == NULL) {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }

    FILE *output_e = fopen("pars_e.dat", "w");
    if (output_e == NULL) {
        perror("Error creating E output file");
        fclose(input);
        return EXIT_FAILURE;
    }

    FILE *output_h = fopen("pars_h.dat", "w");
    if (output_h == NULL) {
        perror("Error creating H output file");
        fclose(input);
        fclose(output_e);
        return EXIT_FAILURE;
    }

    char line[LINE_SIZE];
    int in_radiation_section = 0;
    int lines_skiped = 0;
    int e_index = 0, h_index = 0;

    double theta_e[360], gain_e[360], phase_e[360];
    double theta_h[360], gain_h[360], phase_h[360];

    // Parse the file line by line
    while (fgets(line, sizeof(line), input)) {
        // Check for the start of the radiation patterns section
        if (strstr(line, "- - - RADIATION PATTERNS - - -")) {
            in_radiation_section = 1;
            continue;
        }

        if (in_radiation_section) {
            lines_skiped += 1;
        }

        if (in_radiation_section && lines_skiped > 4) {
            double theta, phi, vert_gain, hor_gain, e_theta_phase, e_phi_phase;

            // Parse relevant data from the line (adjust column positions)
            if (sscanf(line, " %lf %lf %lf %lf%*51c %lf %*11c %lf", &theta, &phi, &vert_gain, &hor_gain, &e_theta_phase, &e_phi_phase) == 6) {
                // Process for E file (phi == 90)
                if ((int)phi == 90) {
                    theta_e[e_index] = theta;
                    gain_e[e_index] = vert_gain; // Use magnitude
                    phase_e[e_index] = e_theta_phase;
                    e_index++;
                }

                // Process for H file (phi == 0)
                if ((int)phi == 0) {
                    theta_h[h_index] = theta;
                    gain_h[h_index] = hor_gain; // Use magnitude
                    phase_h[h_index] = e_phi_phase;
                    h_index++;
                }
            }
        }
    }

    fclose(input);

    // Normalize the gain values
    normalize(gain_e, e_index);
    normalize(gain_h, h_index);

    // Write data to output files
    for (int i = 0; i < e_index; i++) {
        fprintf(output_e, "%lf %lf %lf\n", theta_e[i], gain_e[i], phase_e[i]);
    }

    for (int i = 0; i < h_index; i++) {
        fprintf(output_h, "%lf %lf %lf\n", theta_h[i], gain_h[i], phase_h[i]);
    }

    fclose(output_e);
    fclose(output_h);

    printf("Output files 'pars_e.dat' and 'pars_h.dat' created successfully.\n");

    return EXIT_SUCCESS;
}
