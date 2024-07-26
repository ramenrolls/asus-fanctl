# Asus Fan Control

This C-based fan control solution provides a duct-tape and gum fix for Asus laptops (and maybe others) inadequate fan management, particularly during high-load compile time.

Laptops are inherently limited by their compact design, and a single fan will struggle to effectively cool hardware running at high capacity

**The idea**

Program will monitor key temperature readings and CPU load:

* **NVMe SSD Temperature (hwmon3):** 
* **CPU Temperature (hwmon4):**  
* **CPU Load Average (1-minute):**  

Based on indications, it toggles the fan between two modes:

* **Automatic Mode:**  The default mode, typically reserved to 'low fan speed' for quiet operation. This struggles to maintain optimal cooling when it actually counts. Under heavy workloads leading to thermal throttling or reduced system performance. In essence, its function under stress is negligible and insufficient.
* **Maximum Speed Mode:**  This mode activates when temperature or load thresholds are exceeded, ramping up fan speed to its maximum to ensure sufficient cooling and combat thermal throttling 

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

This script is a workaround, not a replacement for Pulse-Width Modulation fan control. It utilizes the available `pwm1_enable` sys-fs interface to interact with the hwmon5 ('cpu_fan') 

hwmon: (pwm-fan) add fan pwm1_enable attribute (https://patchwork.kernel.org/project/linux-hwmon/patch/20201125163242.GA1264232@paju/)

**Limitations**

* **Two Fan Modes:**  Only automatic and maximum mode are available through hwmon/sys-fs interface
* **Not Full PWM Capability:** Limited by physical hardware

TODO 
====

* **Threshold Fine-tuning:**  Refine default thresholds and give guidelines for customization based on hardware and use cases
* **Hysteresis:**  Add delay before switching back to automatic mode to prevent fan speed fluctuations
* **Logging and Debugging:**  Incorporate logging to track temperature, load, and fan mode changes for debugging
* **Error Handling:**  Expand error handling to cover a wider range of scenarios

**Longer-Term Goals**

* **Multiple Fan Modes:**  Explore ways to achieve more granular fan control, potentially through kernel modules?
* **GPU Monitoring:**  Integrate GPU temperature and load monitoring for comprehensive thermal management
* **Adaptive Algorithm:**  Implement algorithm that learns from usage patterns and dynamically adjusts thresholds for performance and efficiency
* **System Integration:**  Potentially integrate with system management tools or dashboards for streamlined experience

