#!/bin/bash

# Constants
BINARY_NAME="asus_fanctl"
SERVICE_FILE="fanctl.service"
SERVICE_DIR="/etc/systemd/system"
CONFIG_DIR="$HOME/.config/fanctl"

# Functions

function check_dependencies() {
  command -v gcc >/dev/null 2>&1 || { echo "Error: gcc is not installed. Please install it."; exit 1; }
  command -v systemctl >/dev/null 2>&1 || { echo "Error: systemd is not detected. This script is designed for systems using systemd."; exit 1; }
}

function compile_source() {
  sudo gcc -o "/usr/local/bin/$BINARY_NAME" asus_fanctl.c || { echo "Error: Compilation failed."; exit 1; }
  sudo chmod +x "/usr/local/bin/$BINARY_NAME" # we want exec perms
  } 

function setup_service() {
    echo "This action requires elevated privileges:"

    sudo -v  # This will prompt for authentication and cache it
    sudo cp ".install/$SERVICE_FILE" "$SERVICE_DIR/$SERVICE_FILE" || { echo "Error: Failed to copy service file."; exit 1; }

    sudo systemctl daemon-reload
    sudo systemctl enable "$SERVICE_FILE"
    sudo systemctl start "$SERVICE_FILE" || { echo "Error: Failed to start service."; exit 1; }
}

function create_config_dir() {
  mkdir -p "$CONFIG_DIR"
}

# Main script

check_dependencies
compile_source
setup_service
create_config_dir

echo "Installation complete!"
# echo "You can configure fan curve and temp1_values by editing the file in $CONFIG_DIR"

