#!/bin/bash

# Constants
BINARY_NAME="asus_fanctl"
SERVICE_FILE="fanctl.service"
SERVICE_DIR="/etc/systemd/system"
CONFIG_DIR="$HOME/.config/fanctl"

# Functions

function check_dependencies() {
  command -v gcc >/dev/null 2>&1 || { echo "Error: gcc is not installed. Please install it."; exit 1; }
  command -v systemctl >/dev/null 2>&1 || { echo "Error: non-systemd distribution, unsupported."; exit 1; }
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

function update_fanctl() {
    echo "Updating asus_fanctl..."

    # Stop the service
    sudo systemctl stop "$SERVICE_FILE"

    # Remove the old binary (if it exists)
    if [ -f "/usr/local/bin/$BINARY_NAME" ]; then
        echo "Removing old binary..."
        sudo rm "/usr/local/bin/$BINARY_NAME"
    fi

    # Compile the new source
    compile_source

    # Restart the service
    sudo systemctl daemon-reload
    sudo systemctl restart "$SERVICE_FILE"
}

function create_config_dir() {
  mkdir -p "$CONFIG_DIR"
}

# Main script

check_dependencies
# Check if binary exists to determine if it's an update
if [ -f "/usr/local/bin/$BINARY_NAME" ]; then
    update_fanctl
else
    compile_source
    setup_service
    create_config_dir
fi

echo "Installation complete!"
# echo "You can configure fan curve and temp1_values by editing the file in $CONFIG_DIR"

