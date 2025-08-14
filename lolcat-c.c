#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_COLORS 30

const char *colors[NUM_COLORS] = {
    "\033[38;5;39m", "\033[38;5;38m", "\033[38;5;44m", "\033[38;5;43m",
    "\033[38;5;49m", "\033[38;5;48m", "\033[38;5;84m", "\033[38;5;83m",
    "\033[38;5;119m", "\033[38;5;118m", "\033[38;5;154m", "\033[38;5;148m",
    "\033[38;5;184m", "\033[38;5;178m", "\033[38;5;214m", "\033[38;5;208m",
    "\033[38;5;209m", "\033[38;5;203m", "\033[38;5;204m", "\033[38;5;198m",
    "\033[38;5;199m", "\033[38;5;163m", "\033[38;5;164m", "\033[38;5;128m",
    "\033[38;5;129m", "\033[38;5;93m",  "\033[38;5;99m",  "\033[38;5;63m",
    "\033[38;5;69m",  "\033[38;5;33m"
};

const char *reset_color = "\033[0m";

static inline int utf8_charlen(unsigned char c) {
    if (c < 0x80) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return 1;
}

static inline const unsigned char *skip_ansi_escape(const unsigned char *p) {
    p += 2; 
    while (*p && !(*p >= 0x40 && *p <= 0x7E)) ++p;
    if (*p) ++p;
    return p;
}

void print_colored_line_strip_esc(const char *line, float gradient, int line_offset) {
    const unsigned char *p = (const unsigned char *)line;
    size_t char_index = 0;
    float base_offset = line_offset * gradient;

    while (*p) {
        if (*p == 0x1B && *(p + 1) == '[') {
            p = skip_ansi_escape(p);
            continue;
        }

        int len = utf8_charlen(*p);
        int color_index = (int)(base_offset + char_index * gradient) % NUM_COLORS;
        if (color_index < 0) color_index += NUM_COLORS;

        printf("%s", colors[color_index]);
        fwrite(p, 1, len, stdout);

        p += len;
        char_index++;
    }

    printf("%s\n", reset_color);
}

void process_file(FILE *f, float gradient) {
    char buffer[4096];
    int line_num = 0;
    int line_offset = rand() % NUM_COLORS;

    while (fgets(buffer, sizeof(buffer), f)) {
        buffer[strcspn(buffer, "\n")] = '\0';
        print_colored_line_strip_esc(buffer, gradient, line_offset + line_num);
        line_num++;
    }
}

int main(int argc, char *argv[]) {
    srand((unsigned int)time(NULL));

    float gradient = 0.6f;
    const char *filename = NULL;

    for (int i = 1; i < argc; ++i) {
        if (strncmp(argv[i], "-g=", 3) == 0) {
            gradient = (float)atof(argv[i] + 3);
        }
        else {
            filename = argv[i];
        }
    }

    FILE *f = stdin;
    if (filename) {
        f = fopen(filename, "r");
        if (!f) {
            perror("Error opening file");
            return EXIT_FAILURE;
        }
    }

    process_file(f, gradient);

    if (f != stdin) fclose(f);

    return EXIT_SUCCESS;
}
