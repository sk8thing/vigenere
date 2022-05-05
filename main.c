#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#define EOOR 255

typedef struct key_value_pair{
    int key, value;
} Pair;

void remove_spacing(char *str){
    int i = 0, j = 0;
    while(str[i]){
        if(str[i] != ' ' && strchr("abcdefghijklmnopqrstuvwxyz", str[i])) str[j++] = str[i];
        i++;
    }
    str[j] = '\0';
}

void decrypt(char *str, char *key){
    int i, j;
    char l;
    unsigned len = strlen(key);
    for(i = 0, j = 0; i < strlen(str); i++){
        if(!strchr("abcdefghijklmnopqrstuvwxyz", str[i])) continue;
        l = key[j % len];
        str[i] -= l - 'a';
        if(str[i] < 'a') str[i] = 'z' - ('a' - str[i]) + 1;
        j++;
    }
}

char* get_key(char *str, int key_len){
    const char letters[26] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
    const float values[26] = { 0.098f, 0.01f, 0.05f, 0.035f, 0.12f, 0.01f, 0.008f, 0.005f, 0.1f,  0.002f, 0.001f, 0.043f, 0.038f, 0.07f, 0.041f, 0.038f, 0.0f, 0.072f, 0.043f, 0.058f, 0.059f, 0.01f, 0.0f, 0.001f, 0.001f, 0.008f};
    int i, j, k, n, bin_len, flag;
    unsigned int len;
    float *scores[key_len], max;
    char *bins[key_len], *temp, *key;
    char *model = (char*) malloc(sizeof(char) * strlen(str) + 1);
    strcpy(model, str);
    remove_spacing(model);
    len = strlen(model);
    bin_len = (int) ceil((float) len / (float) key_len);
    for(i = 0; i < key_len; i++) bins[i] = (char*) calloc(bin_len, sizeof(char));
    for(i = 0; i < len; i++){
        for(j = 0; j < bin_len; j++){
            if(bins[i % key_len][j] == 0){
                bins[i % key_len][j] = model[i];
                break;
            }
        }
    }
    free(model);
    for(i = 0; i < key_len; i++){
        for(j = 0; j < bin_len; j++){
            if(bins[i][j] == 0){
                bins[i][j] = '\0';
                break;
            }
        }
    }
    temp = (char*) malloc(bin_len * sizeof(char));
    for(i = 0; i < key_len; i++){
        scores[i] = (float*) calloc(26, sizeof(float));
        for(j = 0; j < 26; j++){
            strcpy(temp, bins[i]);
            for(k = 0; k < strlen(temp); k++){
                temp[k] -= j;
                if(temp[k] < 'a') temp[k] = 'z' - ('a' - temp[k]) + 1;
                for(n = 0; n < 26; n++) if(temp[k] == letters[n]) scores[i][j] += values[n];
            }
        }
    }
    key = (char*) malloc(sizeof(char) * key_len + 1);
    key[key_len] = '\0';
    for(i = 0; i < key_len; i++){
        free(bins[i]);
        max = 0.0f;
        flag = 0;
        for(j = 0; j < 26; j++){
            if(scores[i][j] > max){
                max = scores[i][j];
                flag = j;
            }
        }
        key[i] = flag + 'a';
        free(scores[i]);
    }
    return key;
}

int get_key_length(char *str, float ratio){
    int i, j, k, max_val = 0, max_flag, *scores, *temp, *spikes, *spike_subs, sum = 0, avg, ctr;
    unsigned int len;
    char *model, *shifted;
    Pair *freqs;
    model = (char*) malloc(sizeof(char) * strlen(str) + 1);
    shifted = (char*) malloc(sizeof(char) * strlen(str));
    strcpy(model, str);
    remove_spacing(model);
    len = strlen(model);
    scores = (int*) calloc(len, sizeof(int));
    for(k = 1; k < len; k++){
        strcpy(shifted, model);
        shifted[len - k] = '\0';
        for(i = 0, j = k; i < len - k; i++, j++){
            if(model[j] == shifted[i]){
                scores[k]++;
            }
        }
    }
    free(model);
    free(shifted);
    spikes = (int*) malloc(sizeof(int));
    spike_subs = (int*) malloc(sizeof(int));
    for(i = 1; i < len; i++) sum += scores[i];
    avg = (int) floor((float) sum / (float) len);
    for(i = 2, j = 1; i < len - 1; i++){
        if(scores[i] > (int) (ratio * avg) && scores[i] > scores[i - 1] && scores[i] > scores[i + 1]){
            spikes[j - 1] = i;
            j++;
            spikes = (int*) realloc(spikes, sizeof(int) * j);
        }
    }
    free(scores);
    for(i = 0, k = 1; i < j - 2; i++){
        spike_subs[k - 1] = abs(spikes[i] - spikes[i + 1]);
        k++;
        spike_subs = (int*) realloc(spike_subs, sizeof(int) * k);
    }
    free(spikes);
    temp = (int*) malloc(sizeof(int) * k - 1);
    for(i = 0; i < k - 1; i++) temp[i] = -1;
    for(i = 0; i < k - 1; i++){
        ctr = 1;
        for(j = i + 1; j < k - 1; j++){
            if(spike_subs[i] == spike_subs[j]){
                ctr++;
                temp[j] = 0;
            }
        }
        if(temp[i] != 0){
            temp[i] = ctr;
        }
    }
    freqs = malloc(sizeof(Pair));
    for(i = 0, j = 1; i < k - 1; i++){
        if(temp[i] != 0){
            freqs[j - 1].key = spike_subs[i];
            freqs[j - 1].value = temp[i];
            j++;
            freqs = realloc(freqs, sizeof(Pair) * j);
        }
    }
    free(temp);
    free(spike_subs);
    for(i = 0; i < j - 1; i++){
        if(freqs[i].value > max_val){
            max_val = freqs[i].value;
            max_flag = freqs[i].key;
        }
    }
    ctr = 0;
    for(i = 0; i < j - 1; i++){
        if(freqs[i].key % max_flag == 0) ctr++;
    }
    free(freqs);
    if(ctr >= 2){
        return max_flag;
    }
    return 0;
}

size_t get_fsize(FILE *file){
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);
    return size;
}

char *file_to_buffer(FILE *file){
    size_t f_size = get_fsize(file);
    char *buf = (char*) malloc(f_size * sizeof(char) + 1);
    buf[f_size] = '\0';
    fread(buf, sizeof(char), f_size, file);
    strlwr(buf);
    return buf;
}

int main(int argc, char* argv[]) {
    FILE *file;
    int key_length;
    char *text, *key;
    if(argc < 2){
        printf("Example: %s [input file path] [optional key]", argv[0]);
        exit(EINVAL);
    }
    file = fopen(argv[1], "rb");
    if(!file){
        printf("Error: cannot open file.");
        exit(ENOENT);
    }
    text = file_to_buffer(file);
    fclose(file);
    if(argv[2]){
        decrypt(text, argv[2]);
        printf("%s", text);
        return 0;
    }
    key_length = (get_key_length(text, 3.0f)) ? get_key_length(text, 3.0f) : get_key_length(text, 2.5f);
    if(!key_length){
        printf("COULDN\'T FIND KEY LENGTH");
        exit(EOOR);
    }
    key = get_key(text, key_length);
    printf("KEY: %s\n", key);
    decrypt(text, key);
    free(key);
    printf("%s", text);
    return 0;
}
