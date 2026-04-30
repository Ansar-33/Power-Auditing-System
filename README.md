⚡ Power Auditing System for Industries.

📊 An IoT-based smart energy monitoring and auditing solution for industrial applications.

📌 Overview:

The Power Auditing System for Industries is an advanced IoT-based project designed to monitor, analyze, and optimize electrical energy consumption in industrial environments. This system continuously measures key electrical parameters such as voltage, current, power, energy, and power factor across a three-phase supply system.

By integrating sensors, microcontrollers, and cloud platforms, the system enables real-time monitoring and data visualization, helping industries reduce energy wastage, improve efficiency, and maintain power quality.


🎯 Objectives:


⚡ Monitor real-time electrical parameters

📉 Reduce energy losses and improve efficiency

📊 Provide accurate energy consumption data

🌐 Enable remote monitoring using IoT

🏭 Support industrial power auditing and analysis



🧠 System Architecture:

3-Phase Supply → Sensors → ESP32 → Blynk Cloud → User Dashboard



🔧 Hardware Components:

🔌 ESP32 Microcontroller

⚡ ZMPT101B Voltage Sensor

🔄 ACS712 Current Sensor

📟 LCD Display (I2C)

🔌 CT Coil (for high current measurement)

🔋 Power Supply Unit



💻 Software & Technologies:

🧾 Embedded C / Arduino IDE

📱 Blynk IoT Platform

📡 WiFi Communication (ESP32)

📊 Real-time Data Visualization



⚙️ Parameters Measured:

🔋 Voltage (V)

🔄 Current (A)

⚡ Power (W)

🔌 Energy (kWh)

📉 Power Factor (PF)



📊 Working Principle:

The system uses voltage and current sensors to capture real-time electrical data from a three-phase supply. The ESP32 processes the analog signals, calculates electrical parameters using embedded algorithms, and sends the data to the Blynk cloud platform via WiFi.

Users can monitor live readings through a mobile dashboard, while the LCD displays phase-wise values locally. The system also helps identify inefficiencies and abnormal power usage.



📱 Features:

📡 Real-time monitoring

📊 IoT dashboard visualization

🔍 Accurate power auditing

📟 LCD display for local readings

⚙️ Multi-phase monitoring

🌐 Remote access via mobile



🚀 Applications:

🏭 Industrial Power Monitoring

🏢 Commercial Energy Management

⚡ Electrical Load Analysis

🔍 Energy Auditing Systems

📉 Power Optimization Solutions
