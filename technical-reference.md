Technical Reference
===========

Overview
___________

Addressing the inadequate fan control on laptops that lack dedicated hardware for granular adjustment. It utilizes existing Linux interfaces and mechanisms to provide proactive cooling solution explicitly targeting sustained high-load scenarios.

## Hardware Limitations and Setup

### PWM (Pulse Width Modulation) Absence

Many laptops, including ASUS models, often lack dedicated PWM circuitry for precise fan speed control. This is a design choice to reduce cost and complexity, but it results in limited control options.

### ACPI-Based Fan Control

Instead of PWM, fan speed is often likely managed through the ACPI (Advanced Configuration and Power Interface) system. ACPI defines various methods and interfaces for controlling system components, including fans. In such setups, the fan speed is typically adjusted by setting predefined levels or relying on firmware-controlled logic.

### `pwm-fan` Kernel Module

Despite the absence of hardware PWM, the kernel provides the `pwm-fan` module. This module emulates PWM behavior by manipulating the power supply to the fan, effectively controlling its speed. However, this is usually limited to a few discrete levels (e.g., off, low, medium, high) rather than the continuous range offered by true PWM.

## Software Interfaces and APIs

### sysfs Interface

`asus_fanctl` primarily interacts with the `hwmon` (Hardware Monitoring) interface exposed through the sysfs virtual filesystem. The relevant files for fan control are usually found under `/sys/devices/platform/asus-nb-wmi/hwmon/hwmon5/`.

The key file used by `asus_fanctl` is:

- `/sys/devices/platform/asus-nb-wmi/hwmon/hwmon5/pwm1_enable`: Writing `0` to this file sets the fan to maximum speed, while `2` sets it to automatic mode (controlled by the system's firmware).

### `Hardware Monitor` API 

While the `hwmon` API provides a way for kernel modules to expose monitor reading via sys-fs interface, `asus_fanctl` doesn't completely utilize it.  Instead, it reads the available `pwm1_enable`, which is pseudo-defined through asus-wmi 

c `/* Based on standard hwmon pwmX_enable values */ `

`#define ASUS_FAN_CTRL_FULLSPEED 0 `

`#define ASUS_FAN_CTRL_MANUAL 1 `

`#define ASUS_FAN_CTRL_AUTO 2 `


## Mitigations and Limits

### Limited Control:

The lack of true PWM control and reliance on limted hwmon/sysfs interaction limit the granularity of fan speed adjustments. The control logic aims to switch between the automatic (quiet) and maximum speed modes based on temperature and load conditions

### False Positives:

Transient spikes in temperature or load can trigger the fan unnecessarily. The logic uses techniques like hysteresis and averaging CPU load to reduce false positive reading of exceeding threshold values, making sure the fan only activates during sustained high-load scenario

### System-Specific Behavior:

ACPI fan control implementations can vary significantly between laptop models and BIOS versions.  `asus_fanctl`'s thresholds and logic might need to be adjusted for different systems.

#### Documentation Reference:
Docs are sourced from [Kernel.org](https://www.kernel.org).

- [Userspace Tools](https://www.kernel.org/doc/Documentation/hwmon/userspace-tools.rst)
- [HWMon Kernel API](https://www.kernel.org/doc/Documentation/hwmon/hwmon-kernel-api.rst)
- [ASUS WMI Sensors](https://www.kernel.org/doc/Documentation/hwmon/asus_wmi_sensors.rst)
- [ASUS WMI EC Sensors](https://www.kernel.org/doc/Documentation/hwmon/asus_wmi_ec_sensors.rst)
- [ASUS EC Sensors](https://www.kernel.org/doc/Documentation/hwmon/asus_ec_sensors.rst)
- [ACPI Power Meter](https://www.kernel.org/doc/Documentation/hwmon/acpi_power_meter.rst)

## Future Considerations

* **Expanding Device Support:**  Investigate ACPI fan control mechanisms for laptop models and potentially extend the project to support a wider range of devices.
* **Alternative Control Methods:** Explore methods of interacting with the fan, such as directly calling ACPI methods (not inherently safe)

