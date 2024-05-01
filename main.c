#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define MAX_FILENAME_LENGTH 256
#define MAX_TAG_LENGTH 128
#define MAX_CONTENT_LENGTH 128

// Function to process XML file
void process_xml_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    FILE *temp_file = tmpfile();
    if (temp_file == NULL) {
        perror("Error creating temporary file");
        fclose(file);
        return;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file) != NULL) {
        char *start_tag = strstr(line, "<name>");
        if (start_tag != NULL) {
            char *end_tag = strstr(start_tag, "</name>");
            if (end_tag != NULL) {
                char content[MAX_CONTENT_LENGTH];
                strncpy(content, start_tag + 6, end_tag - start_tag - 6);
                content[end_tag - start_tag - 6] = '\0';

                if (strcmp(content, "0") == 0) {
                    fprintf(temp_file, "%.*s<name>fire</name>%.*s", (int)(start_tag - line), line, (int)(strlen(end_tag + 7)), end_tag + 7);
                } else if (strcmp(content, "1") == 0) {
                    fprintf(temp_file, "%.*s<name>smoke</name>%.*s", (int)(start_tag - line), line, (int)(strlen(end_tag + 7)), end_tag + 7);
                } else {
                    fprintf(temp_file, "%s", line);
                }
            }
        } else {
            fprintf(temp_file, "%s", line);
        }
    }

    fclose(file);

    // Reset file pointer to the beginning of the temp file
    rewind(temp_file);

    // Open the original file for writing
    file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        fclose(temp_file);
        return;
    }

    // Copy the contents of the temp file to the original file
    int c;
    while ((c = fgetc(temp_file)) != EOF) {
        fputc(c, file);
    }

    fclose(file);
    fclose(temp_file);
}

// Function to traverse directory and process XML files
void traverse_directory(const char *dir_path) {
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        perror("Error opening directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { // Regular file
            char filepath[MAX_FILENAME_LENGTH];
            snprintf(filepath, MAX_FILENAME_LENGTH, "%s/%s", dir_path, entry->d_name);
            if (strstr(entry->d_name, ".xml") != NULL) {
                process_xml_file(filepath);
            }
        }
    }

    closedir(dir);
}

int main() {
    const char *directory = "."; // Current directory
    traverse_directory(directory);
    return 0;
}