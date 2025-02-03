#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_PATH_LENGTH 1024
#define LINE_SIZE 4096

FILE* open_file(const char* filename, const char* mode) {
    FILE* file = fopen(filename, mode);
    if (!file) {
        fprintf(stderr, "Error: Unable to open file '%s'\n", filename);
        perror("Reason");
        exit(EXIT_FAILURE);
    }
    return file;
}

// Function to extract directory path from a file path
void get_directory_path(const char* full_path, char* dir_path) {
    strcpy(dir_path, full_path);
    char* last_slash = strrchr(dir_path, '/');  // Unix/Linux/Mac
    #ifdef _WIN32
        last_slash = strrchr(dir_path, '\\');  // Windows
    #endif

    if (last_slash) {
        *(last_slash + 1) = '\0'; // Keep only the directory path
    } else {
        strcpy(dir_path, "./");  // Default to current directory
    }
}

// Function to count angles dynamically
int count_angles(FILE* input) {
    char line[LINE_SIZE];
    int count = 0, skip_count = 4, in_radiation_section = 0;

    while (fgets(line, sizeof(line), input)) {
        if (!in_radiation_section) {
            in_radiation_section = strstr(line, "- - - RADIATION PATTERNS - - -") != NULL;
            continue;
        }

        if (skip_count-- > 0 || strspn(line, " \t\n\r") == strlen(line)) {
            if (skip_count < 0) break;
            continue;
        }

        count++;
    }
    rewind(input);
    return count;
}

// Function to normalize gain values
void convert(double *data_e, double *data_h, int size_e, int size_h) {
    double max_value_e = -INFINITY, max_value_h = -INFINITY;

    for (int i = 0; i < size_e; i++) {
        data_e[i] = 20 * log10(data_e[i]);
        if (data_e[i] > max_value_e) max_value_e = data_e[i];
    }
    for (int i = 0; i < size_h; i++) {
        data_h[i] = 20 * log10(data_h[i]);
        if (data_h[i] > max_value_h) max_value_h = data_h[i];
    }

    double max_value = (max_value_h > max_value_e) ? max_value_h : max_value_e;

    for (int i = 0; i < size_e; i++) data_e[i] = max_value - data_e[i];
    for (int i = 0; i < size_h; i++) data_h[i] = max_value - data_h[i];
}

int main(int argc, char* argv[]) {
    char file_path[MAX_PATH_LENGTH];
    char output_prefix[MAX_PATH_LENGTH] = "pars";  // Default output prefix
    char output_dir[MAX_PATH_LENGTH];

    if (argc > 1) {
        strncpy(file_path, argv[1], sizeof(file_path) - 1);
        file_path[sizeof(file_path) - 1] = '\0';
    } 

    // Use second argument as prefix if provided
    if (argc > 2) {
        strncpy(output_prefix, argv[2], sizeof(output_prefix) - 1);
        output_prefix[sizeof(output_prefix) - 1] = '\0';
    }

    // Extract the directory path from the input file
    get_directory_path(file_path, output_dir);

    // Create output file paths in the same directory as the input file
    char output_e_file[MAX_PATH_LENGTH], output_h_file[MAX_PATH_LENGTH];
    snprintf(output_e_file, sizeof(output_e_file), "%s%s_e.dat", output_dir, output_prefix);
    snprintf(output_h_file, sizeof(output_h_file), "%s%s_h.dat", output_dir, output_prefix);

    FILE* input = open_file(file_path, "r");
    FILE* output_e = open_file(output_e_file, "w");
    FILE* output_h = open_file(output_h_file, "w");

    int num_angles = count_angles(input);
    if (num_angles == 0) {
        fprintf(stderr, "Error: No angles found in file.\n");
        fclose(input);
        fclose(output_e);
        fclose(output_h);
        return EXIT_FAILURE;
    }

    double *theta_e = malloc(num_angles * sizeof(double));
    double *gain_e = malloc(num_angles * sizeof(double));
    double *phase_e = malloc(num_angles * sizeof(double));

    double *theta_h = malloc(num_angles * sizeof(double));
    double *gain_h = malloc(num_angles * sizeof(double));
    double *phase_h = malloc(num_angles * sizeof(double));

    if (!theta_e || !gain_e || !phase_e || !theta_h || !gain_h || !phase_h) {
        fprintf(stderr, "Memory allocation failed.\n");
        free(theta_e); free(gain_e); free(phase_e);
        free(theta_h); free(gain_h); free(phase_h);
        fclose(input); fclose(output_e); fclose(output_h);
        return EXIT_FAILURE;
    }

    char line[LINE_SIZE];
    int in_radiation_section = 0, e_index = 0, h_index = 0;

    while (fgets(line, sizeof(line), input)) {
        if (!in_radiation_section && strstr(line, "- - - RADIATION PATTERNS - - -")) {
            in_radiation_section = 1;
            continue;
        }

        if (in_radiation_section) {
            double theta, phi, vert_gain, hor_gain, e_theta_phase, e_phi_phase;
            if (sscanf(line, " %lf %lf%*55c %lf %lf %lf %lf", 
                       &theta, &phi, &vert_gain, &e_theta_phase, &hor_gain, &e_phi_phase) == 6) {
                if ((int)phi == 90 && e_index < num_angles) {
                    theta_e[e_index] = theta;
                    gain_e[e_index] = vert_gain;
                    phase_e[e_index] = e_theta_phase;
                    e_index++;
                } else if ((int)phi == 0 && h_index < num_angles) {
                    theta_h[h_index] = theta;
                    gain_h[h_index] = hor_gain;
                    phase_h[h_index] = e_phi_phase;
                    h_index++;
                }
            }
        }
    }
    fclose(input);

    convert(gain_e, gain_h, e_index, h_index);

    for (int i = 0; i < e_index; i++) {
        fprintf(output_e, "%lf %lf %lf\n", theta_e[i], gain_e[i], phase_e[i]);
    }
    for (int i = 0; i < h_index; i++) {
        fprintf(output_h, "%lf %lf %lf\n", theta_h[i], gain_h[i], phase_h[i]);
    }

    fclose(output_e);
    fclose(output_h);

    free(theta_e); free(gain_e); free(phase_e);
    free(theta_h); free(gain_h); free(phase_h);

    printf("Output files '%s' and '%s' created successfully.\n", output_e_file, output_h_file);

    return EXIT_SUCCESS;
}
