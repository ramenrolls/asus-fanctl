#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define FAN_CONTROL_PATH "/sys/devices/platform/asus-nb-wmi/hwmon/hwmon5/pwm1_enable"
#define NVME_TEMP_PATH "/sys/class/hwmon/hwmon3/temp1_input"
#define CPU_TEMP_PATH "/sys/class/hwmon/hwmon4/temp1_input"
#define CPU_LOAD_PATH "/proc/loadavg"

#define NVME_TEMP_THRESHOLD 72.0
#define CPU_TEMP_THRESHOLD 70.0
#define CPU_LOAD_THRESHOLD 0.8
#define SLEEP_DURATION 3

float read_temperature(const char* path) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) { perror("Error opening temp file"); exit(1); }
    char buf[16];
    ssize_t num_bytes = read(fd, buf, sizeof(buf));
    if (num_bytes < 0) { perror("Error reading temp file"); exit(1); }
    close(fd);
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

int main() {
    int fan_control_fd;
    float nvme_temp, cpu_temp, cpu_load;
    int fan_mode;

    while (1) {
        // Read NVMe temperature
        nvme_temp = read_temperature(NVME_TEMP_PATH);

        // Read CPU temperature
        cpu_temp = read_temperature(CPU_TEMP_PATH);

        // Read CPU load
        cpu_load = read_loadavg(CPU_LOAD_PATH);
        
// NVMe temp1_max=76850 // temp readings in milidegree celsius (temp°C * 1000) / 
// NVMe temp1_crit=79850 
// NVMe temp1_min= 65000 
        
        // Fan control logic
        if (nvme_temp >= NVME_TEMP_THRESHOLD || cpu_temp >= CPU_TEMP_THRESHOLD || cpu_load > CPU_LOAD_THRESHOLD) {
                fan_mode = 0; // max speed
            } else {
                fan_mode = 2; // Automatic (default) 
            }
        
        // Set fan mode
        fan_control_fd = open(FAN_CONTROL_PATH, O_WRONLY);
        if (fan_control_fd < 0) { perror("Error opening fan control file"); exit(1); }
        dprintf(fan_control_fd, "%d", fan_mode);
        close(fan_control_fd);

        sleep(SLEEP_DURATION); // Lets not hardcode this 
    }

    return 0;
}
