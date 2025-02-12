#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 1024

void highlight_differences(const char *line1, const char *line2) {
    int len1 = strlen(line1);
    int len2 = strlen(line2);
    int min_len = len1 < len2 ? len1 : len2;

    printf("      ");
    for (int i = 0; i < min_len; i++) {
        if (line1[i] != line2[i]) {
            putchar('^');
        } else {
            putchar(' ');
        }
    }

    if (len1 > len2) {
        for (int i = min_len; i < len1; i++) putchar('^');
    } else if (len2 > len1) {
        for (int i = min_len; i < len2; i++) putchar('^');
    }

    putchar('\n');
}

void compare_outputs(const char *prof_program, const char *my_program, char *args) {
    char command1[MAX_LINE_LENGTH], command2[MAX_LINE_LENGTH];
    snprintf(command1, sizeof(command1), "%s %s", prof_program, args);
    snprintf(command2, sizeof(command2), "%s %s", my_program, args);

    FILE *prof_output = popen(command1, "r");
    FILE *my_output = popen(command2, "r");

    if (!prof_output || !my_output) {
        fprintf(stderr, "Error running programs.\n");
        exit(1);
    }

    char prof_line[MAX_LINE_LENGTH], my_line[MAX_LINE_LENGTH];
    int line_num = 1;
    int prof_read, my_read;

    while (1) {
        prof_read = (fgets(prof_line, MAX_LINE_LENGTH, prof_output) != NULL);
        my_read = (fgets(my_line, MAX_LINE_LENGTH, my_output) != NULL);

        if (!prof_read && !my_read) {
            break;
        }

        if (!prof_read) strcpy(prof_line, "(EOF)");
        if (!my_read) strcpy(my_line, "(EOF)");

        prof_line[strcspn(prof_line, "\n")] = '\0';
        my_line[strcspn(my_line, "\n")] = '\0';

        printf("Line %d:\n", line_num);
        printf("Prof: %s\n", prof_line);
        printf("Mine: %s\n", my_line);
        if (strcmp(prof_line, my_line) != 0) {
            highlight_differences(prof_line, my_line);
        }
        printf("\n");

        line_num++;
    }

    pclose(prof_output);
    pclose(my_output);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <professor_program> <my_program> [args...]\n", argv[0]);
        return 1;
    }

    char args[MAX_LINE_LENGTH] = "";
    for (int i = 3; i < argc; i++) {
        strcat(args, argv[i]);
        if (i < argc - 1) {
            strcat(args, " ");
        }
    }

    for (int i = 3; i < argc; i++) {
        if (access(argv[i], F_OK) == -1) {
            fprintf(stderr, "Error: Cannot access file %s\n", argv[i]);
            return 1;
        }
    }

    compare_outputs(argv[1], argv[2], args);
    return 0;
}
