#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "format_type.h"

struct statistic_t
{
    int min;
    int max;
    float avg;
    float standard_deviation;
    int range;
};

int load(const char *filename, int ***ptr, enum save_format_t format) {
    if(filename == NULL || ptr == NULL || format > 1) return 1;

    FILE *file;

    int count_rows = 0, count_nums, i, value, c;

    if(format == fmt_text) {
        file = fopen(filename, "rt");
        if(file == NULL) return 2;

        fseek(file, 0, SEEK_END);
        if(ftell(file) == 0) {
            fclose(file);
            return 3;
        }
        fseek(file, 0, SEEK_SET);

        if(fscanf(file, "%d", &value) != 1) {
            fclose(file);
            return 3;
        }
        else fseek(file, 0, SEEK_SET);

        while(!feof(file)) {
            c = fgetc(file);
            if(c >= 'A' && c <= 'z') {
                fclose(file);
                return 3;
            }
        }

        fseek(file, 0, SEEK_SET);

        while(1) {
            if(feof(file)) break;
            while(1) {
                if(fscanf(file, "%d", &value) != 1) break;
                if(value == -1) {
                    count_rows++;
                    break;
                }
            }
        }

        *ptr = (int **)malloc((count_rows + 1) * sizeof(int *));
        if(*ptr == NULL) {
            fclose(file);
            return 4;
        }

        fseek(file, 0, SEEK_SET);

        int rows = count_rows;
        count_rows = 0;

        while(1) {
            count_nums = 0;
            if(feof(file)) break;
            while(1) {
                if(fscanf(file, "%d", &value) != 1) break;
                if(value == -1) {
                    count_nums++;
                    if(count_rows < rows) *(*ptr + count_rows) = (int *)malloc(count_nums * sizeof(int));
                    if(*(*ptr + count_rows) == NULL) {
                        for(i = 0; i < count_rows; i++) {
                            free(*(*ptr + i));
                        }
                        free(*ptr);
                        *ptr = NULL;
                        fclose(file);
                        return 4;
                    }
                    count_rows++;
                    break;
                }
                else count_nums++;
            }
        }

        *(*ptr + rows) = NULL;
        fseek(file, 0, SEEK_SET);

        count_rows = 0;

        while(1) {
            count_nums = 0;
            if(feof(file)) break;
            while(1) {
                if(fscanf(file, "%d", &value) != 1) break;
                if(value == -1) {
                    *(*(*ptr + count_rows) + count_nums) = -1;
                    count_rows++;
                    break;
                }
                else {
                    *(*(*ptr + count_rows) + count_nums) = value;
                    count_nums++;
                }
            }
        }

        fclose(file);
    }
    else if(format == fmt_binary) {
        file = fopen(filename, "rb");
        if(file == NULL) return 2;

        fseek(file, 0, SEEK_END);
        if(ftell(file) == 0) {
            fclose(file);
            return 3;
        }
        fseek(file, 0, SEEK_SET);

        if(fread(&value, sizeof(int), 1, file) != 1) {
            fclose(file);
            return 3;
        }
        else fseek(file, 0, SEEK_SET);

        while(1) {
            if(feof(file)) break;
            while(1) {
                if(fread(&value, sizeof(int), 1, file) != 1) break;
                if(value == -1) {
                    count_rows++;
                    break;
                }
            }
        }

        *ptr = (int **)malloc((count_rows + 1) * sizeof(int *));
        if(*ptr == NULL) {
            fclose(file);
            return 4;
        }

        fseek(file, 0, SEEK_SET);

        int rows = count_rows;
        count_rows = 0;

        while(1) {
            count_nums = 0;
            if(feof(file)) break;
            while(1) {
                if(fread(&value, sizeof(int), 1, file) != 1) break;
                if(value == -1) {
                    count_nums++;
                    if(count_rows < rows) *(*ptr + count_rows) = (int *)malloc(count_nums * sizeof(int));
                    if(*(*ptr + count_rows) == NULL) {
                        for(i = 0; i < count_rows; i++) {
                            free(*(*ptr + i));
                        }
                        free(*ptr);
                        *ptr = NULL;
                        fclose(file);
                        return 4;
                    }
                    count_rows++;
                    break;
                }
                else count_nums++;
            }
        }

        *(*ptr + rows) = NULL;
        fseek(file, 0, SEEK_SET);

        count_rows = 0;

        while(1) {
            count_nums = 0;
            if(feof(file)) break;
            while(1) {
                if(fread(&value, sizeof(int), 1, file) != 1) break;
                if(value == -1) {
                    *(*(*ptr + count_rows) + count_nums) = -1;
                    count_rows++;
                    break;
                }
                else {
                    *(*(*ptr + count_rows) + count_nums) = value;
                    count_nums++;
                }
            }
        }

        fclose(file);
    }

    return 0;
}

int statistics_row(int **ptr, struct statistic_t **stats) {
    if(ptr == NULL || stats == NULL) return -1;

    int counter_elems, counter_rows = 0, i = 0, j;

    while(*(ptr + i) != NULL) {
        counter_rows++;
        i++;
    }

    i = 0;
    *stats = malloc(counter_rows * sizeof(struct statistic_t));
    if(*stats == NULL) return -2;

    int temp_min, temp_max, sum, first_min, first_max;
    float std_dev;

    while(1) {
        if(*(ptr + i) == NULL) {
            if(i == 0) {
                free(*stats);
                *stats = NULL;
                return -1;
            }
            return counter_rows;
        }
        else {
            j = 0;
            counter_elems = 0;
            temp_min = 0;
            temp_max = 0;
            sum = 0;
            first_min = 0;
            first_max = 0;
            std_dev = 0;

            while(1) {
                if(*(*(ptr + i) + j) == -1) {
                    if(j == 0) {
                        (*stats + i)->max = -1;
                        (*stats + i)->min = -1;
                        (*stats + i)->avg = -1;
                        (*stats + i)->range = -1;
                        (*stats + i)->standard_deviation = -1.0f;
                        break;
                    }
                    break;
                }
                else {
                    counter_elems++;
                    sum += *(*(ptr + i) + j);

                    if(first_min == 0) {
                        temp_min = *(*(ptr + i) + j);
                        first_min = 1;
                    }
                    else if(*(*(ptr + i) + j) < temp_min) temp_min = *(*(ptr + i) + j);

                    if(first_max == 0) {
                        temp_max = *(*(ptr + i) + j);
                        first_max = 1;
                    }
                    else if(*(*(ptr + i) + j) > temp_max) temp_max = *(*(ptr + i) + j);
                    j++;
                }
            }

            if((*stats + i)->standard_deviation == -1) {
                i++;
                continue;
            }

            (*stats + i)->max = temp_max;
            (*stats + i)->min = temp_min;
            (*stats + i)->avg = (float)sum / (float)counter_elems;
            (*stats + i)->range = abs(temp_min - temp_max);

            j = 0;

            while (1) {
                if (*(*(ptr + i) + j) == -1) {
                    if(j == 0) {
                        (*stats + i)->max = -1;
                        (*stats + i)->min = -1;
                        (*stats + i)->avg = -1;
                        (*stats + i)->range = -1;
                        (*stats + i)->standard_deviation = -1.0f;
                        break;
                    }
                    break;
                }
                else {
                    std_dev += powf((float) *(*(ptr + i) + j) - (*stats + i)->avg, 2);
                    j++;
                }
            }

            if((*stats + i)->standard_deviation == -1) {
                i++;
                continue;
            }

            std_dev /= (counter_elems * 1.0f);
            std_dev = sqrtf(std_dev);
            (*stats + i)->standard_deviation = std_dev;
            i++;
        }
    }
}

void destroy(int ***ptr) {
    if(ptr == NULL) return;

    int i = 0;

    while(*(*ptr + i) != NULL) {
        free(*(*ptr + i));
        i++;
    }
    free(*ptr);
    *ptr = NULL;
}

void display(int **ptr) {
    if(ptr == NULL) return;

    int i = 0, j;

    while(1) {
        if(*(ptr + i) == NULL) return;
        else {
            j = 0;
            while(1) {
                if(*(*(ptr + i) + j) == -1) break;
                else {
                    printf("%d ", *(*(ptr + i) + j));
                    j++;
                }
            }
            printf("\n");
            i++;
        }
    }
}

int main(void) {
    char *filename, *ext;
    int res, **ptr, i, c;
    filename = malloc(40 * sizeof(char));

    if(filename == NULL) {
        printf("Failed to allocate memory");
        return 8;
    }

    printf("Please input filename:");
    scanf("%39s", filename);
    while((c = getchar()) && c != EOF && c != '\n');
    ext = strchr(filename, '.');

    if(ext == NULL) {
        printf("Unsupported file format");
        free(filename);
        return 7;
    }

    struct statistic_t *statistics;

    if(strcmp(ext, ".txt") == 0) {
        res = load(filename, &ptr, fmt_text);
        if(res == 2) {
            free(filename);
            printf("Couldn't open file");
            return 4;
        }
        else if(res == 3) {
            free(filename);
            printf("File corrupted");
            return 6;
        }
        else if(res == 4) {
            free(filename);
            printf("Failed to allocate memory");
            return 8;
        }
        else if(res == 0) {
            res = statistics_row(ptr, &statistics);
            if(res == -2) {
                free(filename);
                destroy(&ptr);
                printf("Failed to allocate memory");
                return 8;
            }
            else if(res > 0) {
                for(i = 0; i < res; i++) {
                    printf("%d %d %d %.2f %.2f\n", (statistics + i)->min, (statistics + i)->max,
                           (statistics + i)->range, (statistics + i)->avg, (statistics + i)->standard_deviation);
                }
                free(statistics);
                free(filename);
                destroy(&ptr);
                return 0;
            }
        }
    }
    else if(strcmp(ext, ".bin") == 0) {
        res = load(filename, &ptr, fmt_binary);
        if(res == 2) {
            printf("Couldn't open file");
            free(filename);
            return 4;
        }
        else if(res == 3) {
            printf("File corrupted");
            free(filename);
            return 6;
        }
        else if(res == 4) {
            printf("Failed to allocate memory");
            free(filename);
            return 8;
        }
        else if(res == 0) {
            res = statistics_row(ptr, &statistics);
            if(res == -2) {
                printf("Failed to allocate memory");
                free(filename);
                destroy(&ptr);
                return 8;
            }
            else if(res > 0) {
                for(i = 0; i < res; i++) {
                    printf("%d %d %d %.2f %.2f\n", (statistics + i)->min, (statistics + i)->max,
                           (statistics + i)->range, (statistics + i)->avg, (statistics + i)->standard_deviation);
                }
                free(statistics);
                free(filename);
                destroy(&ptr);
                return 0;
            }
        }
    }
    else {
        printf("Unsupported file format");
        free(filename);
        return 7;
    }
}
