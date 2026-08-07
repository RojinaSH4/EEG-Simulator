# EEG Band Separation & Brainwave Visualizer
An end-to-end embedded and desktop application system for separating EEG frequency bands, determining brain states, and visualizing power distribution in real-time.
---
## 📌 Project Overview
This project simulates real-time EEG signal processing by taking raw band signals (Delta, Theta, Alpha, Beta) through operational amplifier circuits, processing them using an **ATmega32** microcontroller, and transmitting parsed power percentages over Serial/UART communication to a **C# WinForms** desktop interface for live data visualization.
---
## 🛠️ System Architecture & Workflow
```text
[Signal Sources] -> [Op-Amp Conditioning] -> [ATmega32 ADC] -> [UART Transmission] -> [com0com Virtual Port] -> [C# WinForms GUI]
```
1. **Hardware / Conditioning Stage:**  
   - Filters and amplifies EEG signals across four major frequency bands (Delta, Theta, Alpha, Beta).
2. **Firmware Stage (CodeVisionAVR):**  
   - Reads analog signals via ADC channels.
   - Computes dynamic power ratios and determines dominant brain states (`Deep Sleep`, `Drowsy`, `Relaxed`, `Focused`).
   - Displays real-time status on a $16 \times 2$ LCD screen.
   - Formats and sends raw telemetry strings over UART at 9600 Baud.
3. **Software Stage (C# WinForms):**  
   - Receives serial streams via real or virtual COM ports (`com0com`).
   - Uses Regular Expressions (Regex) to parse raw stream tokens into structured data objects.
   - Safely updates UI controls via asynchronous thread-marshaling (`BeginInvoke`).
   - Renders live percentage column charts and state indicators.
---
## 🧠 Brain State Logic

| Band | Threshold / Condition | State Output |
| :--- | :--- | :--- |
| **Delta** | Percentage $\ge 50\%$ | Deep Sleep |
| **Theta** | Percentage $\ge 40\%$ | Drowsy |
| **Alpha** | Percentage $\ge 40\%$ | Relaxed |
| **Beta** | Percentage $\ge 45\%$ | Focused |
| **None** | Low/Zero Signal Amplitude | No Signal |

---
## 📁 Repository Structure
```text
├── docs/        # Project diagrams, PCB, schematic, and GUI screenshots
├── firmware/    # CodeVisionAVR source code (.c) and ATmega32 configuration
├── hardware/    # Proteus circuit design (.pdsprj)
└── software/    # C# .NET WinForms desktop application
```
---
## 🚀 Getting Started
### Prerequisites
- **Proteus Design Suite** (for hardware simulation)
- **CodeVisionAVR** or **Microchip Studio** (for compiling AVR firmware)
- **Visual Studio 2019/2022** with .NET Framework (for WinForms application)
- **com0com** (Null-modem emulator for serial bridging between Proteus and C#)
### Running the Simulation
1. **Bridge Virtual COM Ports:**  
   Configure `com0com` to pair two virtual ports (e.g., `COM1` $\leftrightarrow$ `COM2`).
2. **Launch Proteus:**  
   Open `hardware/Brain Wave Band Seperation.pdsprj`, configure the `COMPIM` component to use `COM1` at `9600 Baud`, and start the simulation.
3. **Launch Desktop App:**  
   Open `software/EEG Simulator.sln` in Visual Studio, build and run the application. Select `COM2` from the connection panel and click **Connect Serial Port**.
---
## 📄 License
This project is open-source and available under the [MIT License](LICENSE).