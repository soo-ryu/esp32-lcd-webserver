# ESP32 LCD Web Server

This project implements a web server on the ESP32 that allows users to change the text displayed on an LCD screen. The server provides a simple interface for updating the LCD text via HTTP requests.

## Features

- Initialize and control an LCD display using the LiquidCrystal_I2C library.
- A web server that listens for incoming requests to update the LCD text.
- Simple HTML interface for user interaction.

## Project Structure

```
esp32-lcd-webserver
├── src
│   ├── main.cpp          // Entry point of the application
│   ├── lcd_display.cpp   // Implementation of the LCDDisplay class
│   ├── lcd_display.h     // Definition of the LCDDisplay class
│   ├── web_server.cpp     // Implementation of the WebServer class
│   └── web_server.h      // Definition of the WebServer class
├── include
│   └── README.md         // Documentation for the include directory
├── platformio.ini        // PlatformIO configuration file
└── README.md             // Project documentation
```

## Setup Instructions

1. Clone the repository or download the project files.
2. Open the project in your preferred IDE.
3. Install the required libraries, including `LiquidCrystal_I2C`.
4. Connect the ESP32 to your computer.
5. Upload the code to the ESP32 board.

## Usage

1. After uploading the code, open the Serial Monitor to find the IP address assigned to the ESP32.
2. Open a web browser and navigate to the ESP32's IP address.
3. Use the provided interface to change the text displayed on the LCD.

## License

This project is licensed under the MIT License.