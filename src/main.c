#include <mayak/pwm.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>

#define TEMP_HIGH 70
#define SPEED_HIGH 70
#define TEMP_OK 50
#define SPEED_OK 50

const mayak_pwm_file pwm = {
    .chip = 0,
    .channel = 5
};

struct timespec ts = {
    .tv_nsec = 1 * 1000 * 1000,
    .tv_sec = 0
};

bool get_temp(float* temp) {
    FILE* thermal = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    if (!thermal) {
        perror("fopen");
        return false;
    }
    if (fscanf(thermal, "%f", temp) != 1) {
        fprintf(stderr, "failed to scan temperature!\n");
        fclose(thermal);
        return false;
    }
    fclose(thermal);
    return true;
}

int main() {
    if (!mayak_pwm_export(&pwm))
        return 1;

    while (access("/sys/class/pwm/pwmchip0/pwm5/polarity", F_OK) != 0){
        printf("waiting for pwm to appear\n");
        nanosleep(&ts, NULL);
    }

    mayak_pwm_set_freq(&pwm, MAYAK_PWM_KHZ, 25);
    mayak_pwm_set_duty(&pwm, SPEED_OK);

    mayak_pwm_set_enabled(&pwm, true);

    mayak_pwm_set_inversed(&pwm, false);

    ts.tv_nsec = 500 * 1000 * 1000; // 500 ms

    float temp;
    bool high = false;

    while (1) {

        if (!get_temp(&temp)) {
            fprintf(stderr, "failed to get temp!\n");
            nanosleep(&ts, NULL);
            continue;
        }
        temp /= 1000;

        printf("temp: %f\n", temp);

        if (temp >= TEMP_HIGH && high == false) {
            printf("temperature is high!\n");
            high = true;
            mayak_pwm_set_duty(&pwm, SPEED_HIGH);
        }
        if (high == true && temp <= TEMP_OK) {
            printf("temperature normalized!\n");
            high = false;
            mayak_pwm_set_duty(&pwm, SPEED_OK);
        }

        nanosleep(&ts, NULL);
    }

    return 0;
}
