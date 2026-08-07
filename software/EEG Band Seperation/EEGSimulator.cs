using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;
using System.Drawing;

namespace EEG_Band_Seperation
{
    /// <summary>
    /// Represents the main user interface form for the EEG Data Simulator and Visualizer.
    /// Manages serial communication and updates UI chart components in real time.
    /// </summary>
    public partial class EEGSimulator : Form
    {

        // Global serial port instance for micro-controller data transmission
        private SerialPort serial = new SerialPort();

        /// <summary>
        /// Initializes a new instance of the <see cref="EEGSimulator"/> class.
        /// </summary>
        public EEGSimulator()
        {
            InitializeComponent();
        }


        /// <summary>
        /// Handles form loading event. Populates available COM ports and configures chart visual settings.
        /// </summary>
        private void Form1_Load(object sender, EventArgs e)
        {

            // Populate ComboBox with available system serial ports
            cmbPort.Items.Clear();
            cmbPort.Items.AddRange(SerialPort.GetPortNames());

            // Select the first available port by default
            if (cmbPort.Items.Count > 0)
                cmbPort.SelectedIndex = 0;

            // Initialize chart configuration
            SetupChart();
        }

        /// <summary>
        /// Configures initial styling, axes limits, and default data points for the EEG chart.
        /// </summary>
        private void SetupChart()
        {
            chartEEG.Series.Clear();
            chartEEG.ChartAreas[0].AxisY.Maximum = 100;
            chartEEG.ChartAreas[0].AxisY.Minimum = 0;
            chartEEG.ChartAreas[0].AxisY.Title = "Percentage (%)";

            // Create column series for displaying relative band power levels
            Series series = new Series("EEG Bands")
            {
                ChartType = SeriesChartType.Column,
                IsValueShownAsLabel = true,
                Font = new Font("Segeo UI", 9f, FontStyle.Bold),
                LabelForeColor = Color.White
            };

            // Add default initial points for each frequency band
            series.Points.AddXY("Delta", 0);
            series.Points.AddXY("Theta", 0);
            series.Points.AddXY("Alpha", 0);
            series.Points.AddXY("Beta", 0);

            // Assign distinct custom colors to each frequency band
            series.Points[0].Color = Color.FromArgb(157, 78, 221);
            series.Points[1].Color = Color.FromArgb(0, 180, 216);
            series.Points[2].Color = Color.FromArgb(46, 204, 113);
            series.Points[3].Color = Color.FromArgb(255, 107, 107);

            chartEEG.Series.Add(series);
        }

        /// <summary>
        /// Handles connect/disconnect toggle actions for the selected serial port.
        /// </summary>
        private void btnConnect_Click(object sender, EventArgs e)
        {
            if (!serial.IsOpen)
            {
                try
                {
                    // Configure serial port settings
                    serial.PortName = cmbPort.Text;
                    serial.BaudRate = 9600;
                    serial.DataBits = 8;
                    serial.Parity = Parity.None;
                    serial.StopBits = StopBits.One;
                    serial.NewLine = "\r\n";

                    // Subscribe to incoming data handler
                    serial.DataReceived += Serial_DataReceived;

                    serial.Open();
                    btnConnect.Text = "Disconnect Serial Port";
                    cmbPort.Enabled = false;
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Problem in Port Opening!" + ex.Message);
                }
            }
            else
            {
                try
                {
                    // Unsubscribe event and close active port connection
                    serial.DataReceived -= Serial_DataReceived;
                    serial.Close();
                    btnConnect.Text = "Connect Serial Port";
                    cmbPort.Enabled = true;
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Problem in Port Opening! " + ex.Message);
                }
            }
        }

        /// <summary>
        /// Asynchronous event handler triggered when new serial data is received.
        /// Safely marshals data processing onto the UI thread using BeginInvoke.
        /// </summary>
        private void Serial_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            try
            {
                string line = serial.ReadLine();

                // Marshal thread execution to UI thread to safely update WinForms controls
                BeginInvoke(new Action(() =>
                {
                   EEGData eeg = EEGData.parse(line);
                    if (eeg != null) 
                    {
                        UpdateUI(eeg); 
                    }
                }));
            }
            catch
            {
                // Silently ignore transient read/stream errors
            }
        }

        // <summary>
        /// Updates UI elements (chart series points and state label) with newly parsed EEG values.
        /// </summary>
        /// <param name="eeg">Parsed EEG object containing updated values.</param>
        private void UpdateUI(EEGData eeg)
        {
            // Update Y-values for each corresponding EEG frequency band
            chartEEG.Series["EEG Bands"].Points[0].YValues[0] = eeg.Delta;
            chartEEG.Series["EEG Bands"].Points[1].YValues[0] = eeg.Theta;
            chartEEG.Series["EEG Bands"].Points[2].YValues[0] = eeg.Alpha;
            chartEEG.Series["EEG Bands"].Points[3].YValues[0] = eeg.Beta;

            // Redraw chart to reflect visual updates
            chartEEG.Refresh();

            // Display human-readable current brain state
            lblState.Text = $"Brain State: {eeg.StateText}";
        }

        /// <summary>
        /// Form closing event handler. Ensures active serial connections are safely terminated.
        /// </summary>
        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (serial.IsOpen)
                serial.Close();
        }
    }
}
