#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_PATH_LENGTH 1024
#define LINE_SIZE 1024


FILE* open_file(const char* filename, const char* mode) {
    FILE* file = fopen(filename, mode);
    if (!file) {
        fprintf(stderr, "Error: Unable to open file '%s'\n", filename);
        perror("Reason");
        exit(EXIT_FAILURE);
    }
    return file;
}


double sumArray(double *arr, int n) {
    double sum = 0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    return sum;
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
void convert(double *data_e, double *data_h, int size) {
    double max_value = -INFINITY;

    for (int i = 0; i < size; i++) {
        data_e[i] = 20 * log10(data_e[i]);
        data_h[i] = 20 * log10(data_h[i]);
        if (data_h[i] > max_value) max_value = data_h[i];
        if (data_e[i] > max_value) max_value = data_e[i];
    }

    for (int i = 0; i < size; i++) data_e[i] = max_value - data_e[i];
    for (int i = 0; i < size; i++) data_h[i] = max_value - data_h[i];
}


int main(int argc, char* argv[]) {
    char file_path[MAX_PATH_LENGTH];
    char output_prefix[MAX_PATH_LENGTH] = "pars";  // Default output prefix
    char output_dir[MAX_PATH_LENGTH];
    if (argc == 0) {
        fprintf(stderr, "Error: Failed to provide Input file Name\n");
        perror("Reason");
        exit(EXIT_FAILURE);
    }
    //strcpy(file_path, ".\\example\\basic_radiator\\basic_radiator.out");
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

    double *theta_l = malloc(num_angles * sizeof(double));

    double *gain_e = malloc(num_angles * sizeof(double));
    double *phase_e = malloc(num_angles * sizeof(double));

    double *gain_h = malloc(num_angles * sizeof(double));
    double *phase_h = malloc(num_angles * sizeof(double));

    if (!theta_l || !gain_e || !phase_e|| !gain_h || !phase_h) {
        fprintf(stderr, "Memory allocation failed.\n");
        free(theta_l); free(gain_e); free(phase_e);
        free(gain_h); free(phase_h);
        fclose(input); fclose(output_e); fclose(output_h);
        return EXIT_FAILURE;
    }

    char line[LINE_SIZE];
    int in_radiation_section = 0, index = 0;

    while (fgets(line, sizeof(line), input)) {
        if (!in_radiation_section && strstr(line, "- - - RADIATION PATTERNS - - -")) {
            in_radiation_section = 1;
            continue;
        }

        if (in_radiation_section) {
            double theta, phi, vert_gain, hor_gain, e_theta_phase, e_phi_phase;
            if (sscanf(line, " %lf %lf%*55c %lf %lf %lf %lf", 
                       &theta, &phi, &vert_gain, &e_theta_phase, &hor_gain, &e_phi_phase) == 6) {
                if (((int)phi == 90 || (int)phi == 0) && index < num_angles) {
                    theta_l[index] = theta;
                    gain_e[index] = vert_gain;
                    phase_e[index] = e_theta_phase;
                    gain_h[index] = hor_gain;
                    phase_h[index] = e_phi_phase;
                    index++;
                } 
            }
        }
    }
    fclose(input);
    double sum_e = sumArray(gain_e, index / 2);
    double sum_t = sumArray(gain_e, index);
    convert(gain_e, gain_h, index);


    // Chose the slice with hihger values in the E field for the E field file
    if (sum_t - sum_e < sum_e) {
        printf("The E field does not aling with the Y axis, to correct for that the fields have been switched");
        for (int i = 0; i < index / 2; i++) {
            fprintf(output_h, "%lf %lf %lf\n", theta_l[i], gain_e[i], phase_e[i]);
        }
        for (int i = index / 2; i < index; i++) {
            fprintf(output_e, "%lf %lf %lf\n", theta_l[i], gain_h[i], phase_h[i]);
        }
    }
    else {
        for (int i = 0; i < index / 2; i++) {
            fprintf(output_h, "%lf %lf %lf\n", theta_l[i], gain_h[i], phase_h[i]);
        }
        for (int i = index / 2; i < index; i++) {
            fprintf(output_e, "%lf %lf %lf\n", theta_l[i], gain_e[i], phase_e[i]);
        }
    }



    fclose(output_e);
    fclose(output_h);

    free(theta_l); free(gain_e); free(phase_e);
    free(gain_h); free(phase_h);

    printf("Output files '%s' and '%s' created successfully.\n", output_e_file, output_h_file);

    return EXIT_SUCCESS;
}
