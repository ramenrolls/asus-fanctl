# Asus Fan Control

Laptops are inherently limited by their compact design, and a single fan will struggle to effectively cool hardware running at high capacity

**The idea**

Program will monitor key temperature readings and CPU load:

* **NVMe SSD Temperature (hwmon3):** 
* **CPU Temperature (hwmon4):**  
* **CPU Load Average (1-minute):**  

Based on indications, it toggles the fan between two modes:

* **Automatic Mode:**  The default mode, typically reserved to 'low fan speed' for quiet operation. This struggles to maintain optimal cooling when it actually counts. Under heavy workloads leading to thermal throttling or reduced system performance. In essence, its function under stress is negligible and insufficient.
* **Maximum Speed Mode:**  This mode activates when temperature or load thresholds are exceeded, ramping up fan speed to its maximum to ensure cooling and fight thermal throttling 

## Installation

### One-Line Installation:

For quick and easy installation:

```bash
git clone https://github.com/ramenrolls/asus_fanctl fanctl && cd fanctl && sudo bash configure.sh
```
### Manual Compilation

1. **Compile the Source Code:**
   ```bash
   gcc -o asus_fanctl asus_fanctl.c 
   ```

2. **Systemd Service Setup:**
   * **Move the Service File:** (Assuming you are in the repository directory)
      ```bash
      sudo mv .install/fanctl.service /etc/systemd/system/
      ```

   * **Enable and Start the Service:**
      ```bash
      sudo systemctl daemon-reload
      sudo systemctl enable fanctl.service
      sudo systemctl start fanctl.service
      ```
---

## Configuration

Adjust temperature and load thresholds directly in the `fanctl.c` source

#### Updating

If you made local changes to the `fanctl.c` source, re-run `configure.sh`:

```bash
sudo bash configure.sh
```

The script will detect the existing binary, stop the `fanctl` service, remove the old binary, recompile the program from the updated source code, and restart the service.

### Disclaimer

This script is a crude solution to thermal management, not a replacement for Pulse-Width Modulation fan control. It utilizes the available `pwm1_enable` sys-fs interface to interact with the hwmon5 ('cpu_fan') 

hwmon: (pwm-fan) add fan pwm1_enable attribute (https://patchwork.kernel.org/project/linux-hwmon/patch/20201125163242.GA1264232@paju/)

**Limitations**
-  Only automatic and maximum mode are available through hwmon/sys-fs interface and incomplete PWM capabilities

TODO 
====
Explore and refactor code to ACPI, as laptops will not have proper circuitry on motherboard to communicate with lm-sensors and PWM. ACPI remains the viable low level alternative, barring the crude pwm1_enable sysfs interaction.


What is ACPI? 

The standard that allows the OS to discover and configure hardware, manage power and monitors system status.

Thermal Management:

ACPI defines thermal zones, which are logical collections of interfaces to temperature sensors, trip points, thermal property information, and cooling controls
The operating system can make cooling decisions based on the system’s application load, user preferences, and thermal heuristics
ACPI allows for proactive thermal management, including graceful shutdown of devices or the entire system at critical heat levels

Fan Speed Control:

ACPI fan drivers create attributes in the sysfs directory of the ACPI device, listing properties of fan performance states
These attributes include control percent, trip point index, speed in RPM, noise level, and power draw
Users should be able to adjust fan speed using thermal sysfs cooling devices, either by setting it to predefined performance states

