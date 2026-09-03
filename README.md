# EEG Band Separation & Brainwave Visualizer

## About the Project
This is an end-to-end embedded and desktop visualization system built to simulate real-time biomedical signal processing. 
I created this project to bridge hardware signal conditioning with desktop software, practice multi-layer system architecture, and gain hands-on experience in processing multi-channel analog signals, microcontroller firmware development, and real-time serial telemetry in C#.

The system processes incoming signal sources, filters them into distinct EEG frequency bands via Op-Amp circuits, determines the dominant brain state using an ATmega32 microcontroller, and streams live telemetry to a C# WinForms application for real-time visualization.

---

## Features
- **Analog Signal Conditioning:** Active Op-Amp filtering circuits designed to isolate key EEG frequency bands (Delta, Theta, Alpha, Beta).
- **Microcontroller Signal Processing:** Real-time ADC sampling, dynamic power ratio calculation, and threshold-based brain state classification (`Deep Sleep`, `Drowsy`, `Relaxed`, `Focused`).
- **Hardware Telemetry Display:** On-board 16x2 character LCD updating live system status and band percentages.
- **Serial Telemetry Integration:** Formatted UART data transmission at 9600 Baud to external software interfaces.
- **Real-Time Data Visualization:** C# WinForms UI displaying live percentage column charts and dynamic brain state status indicators.
- **Thread-Safe UI Operations:** Robust async thread-marshaling using `BeginInvoke` and Regex-based stream parsing for dynamic updates.

---

## Architecture & Structure
The project is structured into three clear domains to separate hardware design, embedded firmware, and desktop presentation layers:

- **hardware (Schematic & PCB Layer)**  
  Contains the Proteus simulation files, circuit schematics, and PCB/3D layouts. It models the signal sources and the active operational amplifier filtering stages that isolate the EEG frequency bands.

- **firmware (Embedded Layer)**  
  Contains the CodeVisionAVR C code for the ATmega32 microcontroller. It handles analog-to-digital conversion (ADC), calculates power percentages, manages local LCD output, and streams raw telemetry tokens over UART.

- **software (Presentation Layer)**  
  A C# .NET WinForms application that connects to the serial port (via real or virtual COM ports). It parses incoming data streams with Regular Expressions and updates the visualization chart safely without blocking the main UI thread.

---

## Concepts Used
- **Biomedical Signal Processing:** Active bandpass filtering and EEG frequency isolation (Delta, Theta, Alpha, Beta).
- **Embedded C Development:** ATmega32 ADC configuration, UART communication, and LCD interfacing using CodeVisionAVR.
- **Hardware Simulation & PCB Design:** Circuit modeling, schematic capture, and 3D PCB design in Proteus.
- **Asynchronous UI Updating:** Thread-safe operations with `BeginInvoke` in WinForms.
- **Serial Communication Protocol:** Telemetry parsing with Regex over virtual/physical COM ports.

---

## What I Learned
Through this project, I significantly improved my understanding of:
- Translating biomedical signal conditioning concepts into physical schematic and PCB layouts.
- Developing embedded firmware to bridge analog inputs with digital serial streams.
- Interfacing hardware simulators (Proteus) with desktop applications using virtual serial port bridges (`com0com`).
- Decoupling hardware data acquisition from software presentation layers in a multi-disciplinary workflow.

---

## Screenshots

### Hardware Schematic
<img width="1000" alt="Hardware Schematic" src="docs/Shematic_Brain%20Wave%20Band%20Seperation.jpg" />

### 3D PCB View
<img width="1000" alt="3D PCB View" src="docs/3D_Brain%20Wave%20Band%20Seperation.png" />

### PCB Layout
<img width="1000" alt="PCB Layout" src="docs/PCB_Brain%20Wave%20Band%20Seperation.png" />

### Main Dashboard (WinForms GUI)
<img width="800" alt="Main Dashboard GUI" src="docs/gui.PNG" />

---

## How to Run
1. **Virtual Ports Setup:** Configure `com0com` to pair two virtual serial ports (e.g., `COM1` $\leftrightarrow$ `COM2`).
2. **Hardware Simulation:**
   - Compile the firmware in **CodeVisionAVR** to generate the `.hex` or `.cof` output file.
   - Open `hardware/Brain Wave Band Seperation.pdsprj` in Proteus.
   - Double-click the **ATmega32** microcontroller, click on **Program File**, and select the generated `.hex` (or `.cof`) file from the `firmware/` folder.
   - Double-click the **COMPIM** component and configure it to `COM1` at `9600 Baud`.
   - Run the simulation.
3. **Desktop Application:**  
   - Open `software/EEG Simulator.sln` in Visual Studio.  
   - Build and run the project.  
   - Select `COM2` from the dropdown menu and click **Connect Serial Port**.

---

## Notes
This project was created for learning purposes and to showcase my skills in embedded systems, biomedical signal conditioning, and C# desktop application development.

