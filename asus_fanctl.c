#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define PWM1_ENABLE "/sys/devices/platform/asus-nb-wmi/hwmon/hwmon5/pwm1_enable"
#define NVME_TEMP_PATH "/sys/class/hwmon/hwmon3/temp1_input"
#define NVME_TEMP_THRESHOLD 72.0
#define HYST_NVME 3

#define CPU_TEMP_PATH "/sys/class/hwmon/hwmon4/temp1_input"
#define CPU_TEMP_THRESHOLD 70.0
#define HYST_CPU 3 

#define CPU_LOAD_PATH "/proc/loadavg"
#define CPU_LOAD_THRESHOLD 0.8
#define LOAD_HYST 0.09 // 9 pct

#define SLEEP_DURATION 3
#define LOADAVG_WINDOW 5 // Sample average
#define LOADAVG_SMOOTHING_WINDOW 30 

struct FanConfig {
    float tempThreshold;
    float hyst;
    const char *tempPath;
};
float loadavg_history[LOADAVG_WINDOW] = {0.0};
int loadavg_index = 0;
float smoothed_loadavg = 0.0;

float read_temperature(const char* path) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) { 
        fprintf(stderr, "Error reading temp %s: %s\n", path, strerror(errno));
        exit (1);
        }

    char buf[16];
    ssize_t num_bytes = read(fd, buf, sizeof(buf) - 1);
    if (num_bytes < 0) { 
        fprintf(stderr, "Error reading temp %s: %s\n", path, strerror(errno));
        }
        close(fd);
        
        buf[num_bytes] = '\0';
        return atof(buf) / 1000.0;
}

float read_loadavg(const char* path) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror("Error opening loadavg file");
        exit(1);
    }

    char buf[16];
    ssize_t num_bytes = read(fd, buf, sizeof(buf));
    if (num_bytes < 0) {
        perror("Error reading loadavg file");
        exit(1);
    }

    close(fd);

    // Extract the first field (1-minute load average)
    char *space = strchr(buf, ' ');
    if (space) *space = '\0'; // Null-terminate to get the first float

    return atof(buf);
}
    // calc average
float get_averaged_loadavg() {
    float current_loadavg = read_loadavg(CPU_LOAD_PATH);
    loadavg_history[loadavg_index] = current_loadavg;
    loadavg_index = (loadavg_index + 1) % LOADAVG_WINDOW;
    
    float total_load = 0.0;
    for (int i = 0; i < LOADAVG_WINDOW; i++) {
        total_load += loadavg_history[i];
    }

    float avg_load = total_load / LOADAVG_WINDOW;
    smoothed_loadavg = (smoothed_loadavg * (LOADAVG_SMOOTHING_WINDOW - 1) + avg_load) / LOADAVG_SMOOTHING_WINDOW;
    
    return avg_load;
    }

int main() {
    int fan_control_fd;
    float cpu_load;
    int fan_mode;
    int previous_fan_mode = -1;

fan_control_fd = open(PWM1_ENABLE, O_WRONLY);
    if (fan_control_fd < 0) {
        fprintf(stderr, "Error opening pwm1_enable: %s\n", strerror(errno));
        exit(1);
    }

struct FanConfig fanConfigs[] = {
        {NVME_TEMP_THRESHOLD, HYST_NVME, NVME_TEMP_PATH},
        {CPU_TEMP_THRESHOLD, HYST_CPU, CPU_TEMP_PATH}
    };

    while (1) {
        float temps[2] = {
            read_temperature(fanConfigs[0].tempPath), 
            read_temperature(fanConfigs[1].tempPath)
        };
        cpu_load = get_averaged_loadavg();
        
        // Analyze load average trend 
        float load_diff = cpu_load - loadavg_history[(loadavg_index - 1 + LOADAVG_WINDOW) % LOADAVG_WINDOW];
        int increasing_trend = load_diff > 0.3;
        
        // Fan control logic
        fan_mode = 2; // Default to auto/quiet mode
        for (int i = 0; i < 2; i++) { 
            if (temps[i] >= fanConfigs[i].tempThreshold + fanConfigs[i].hyst ||
               (increasing_trend && cpu_load > CPU_LOAD_THRESHOLD + LOAD_HYST)) {
                fan_mode = 0; // Switch to full speed
                break;
            }
        }

            
        // setting fan mode, and write if necessary
        if (fan_mode != previous_fan_mode) {
            ssize_t bytes_written = write(fan_control_fd, &fan_mode, sizeof(fan_mode));
            if (bytes_written != sizeof(fan_mode)) {
                perror("Error writing to pwm1_enable");
                close(fan_control_fd); 
                return 1;
            }

            previous_fan_mode = fan_mode;

        sleep(SLEEP_DURATION); 
    }

    close(fan_control_fd);
    return 0; 
    }
}
