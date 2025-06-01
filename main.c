#include <stdio.h>
#include <string.h>

// Prints help message for the user
void print_help()
{
    printf("Is used: main.exe -i <input_filename> -o <output_filename>\n");
    printf("Parameters:\n");
    printf("  -c network          Configuration section (should be 'network')\n");
    printf("  -i <input_filename> Input file with configuration\n");
    printf("  -o <output_filename> Output file with results\n");
}

// Validates command-line arguments and extracts input/output file names
int check_args(int args, char *argv[], char **input_file, char **output_file)
{
    if (args != 7)
    {
        return 0;
    }

    // Check the expected flags and section name
    if (strcmp(argv[1], "-c") != 0 || strcmp(argv[2], "network") != 0)
    {
        return 0;
    }

    if (strcmp(argv[3], "-i") != 0)
    {
        return 0;
    }

    *input_file = argv[4];

    if (args <= 5 || strcmp(argv[5], "-o") != 0)
    {
        return 0;
    }

    *output_file = argv[6];

    return 1;
}

int main(int argc, char *argv[])
{
    char *input_file = NULL;
    char *output_file = NULL;

    // Check and parse arguments
    if (!check_args(argc, argv, &input_file, &output_file))
    {
        print_help();
        return 1;
    }

    printf("Input File: %s\n", input_file);
    printf("Output File: %s\n", output_file);

    FILE *fin = fopen(input_file, "r");
    if (fin == NULL)
    {
        printf("Error: failed to open file '%s'\n", input_file);
        return 1;
    }

    // printf("DEBUG: file found '%s'\n", input_file);
    char line[256];
    char net_ip[64] = "";
    char net_mask[64] = "";
    char net_gw[64] = "";
    char net_dns[64] = "";

    // Read configuration lines from input file
    while (fgets(line, sizeof(line), fin))
    {
        line[strcspn(line, "\r\n")] = 0; // Strip newline characters
        if (line[0] == '\0' || line[0] == '#')
            continue; // Skip empty lines and comments
        // printf("Read: %s\n", line);
        char *eq = strchr(line, '=');
        if (!eq)
        {
            printf("Error: the line does not contain '=' %s\n", line);
            continue;
        }

        *eq = '\0'; // Null-terminate key
        char *key = line;
        char *value = eq + 1;

        // printf("Key: '%s', Value: '%s'\n", key, value);

        if (value[0] == '"' && value[strlen(value) - 1] == '"')
        {
            value[strlen(value) - 1] = '\0';
            value++;
        }

        // Match known keys and copy values
        if (strcmp(key, "net_ip") == 0)
            strncpy(net_ip, value, sizeof(net_ip) - 1);
        else if (strcmp(key, "net_mask") == 0)
            strncpy(net_mask, value, sizeof(net_mask) - 1);
        else if (strcmp(key, "net_gw") == 0)
            strncpy(net_gw, value, sizeof(net_gw) - 1);
        else if (strcmp(key, "net_dns") == 0)
            strncpy(net_dns, value, sizeof(net_dns) - 1);
        else
            printf("Ignoring unknown key: %s\n", key);
    }

    // Print final values for debug
    printf("IP: %s\nMask: %s\nGateway: %s\nDNS: %s\n", net_ip, net_mask, net_gw, net_dns);

    FILE *fout = fopen(output_file, "w");
    if (fout == NULL)
    {
        printf("Error: failed to create file '%s'\n", output_file);
        return 1;
    }

    // Write configuration with values from file
    fprintf(fout, "auto eth0\n");
    fprintf(fout, "iface eth0 inet static\n");
    fprintf(fout, "address %s\n", net_ip);
    fprintf(fout, "netmask %s\n", net_mask);
    fprintf(fout, "gateway %s\n", net_gw);
    fprintf(fout, "dns-server %s\n", net_dns);

    fclose(fout);
    printf("File '%s' created succesfully!\n", output_file);

    return 0;
}
