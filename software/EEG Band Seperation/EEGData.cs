using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace EEG_Band_Seperation
{
    /// <summary>
    /// Represents EEG frequency band power levels and the corresponding brain state.
    /// </summary>
    public class EEGData
    {
        // EEG Band Relative Power Values (%)
        public int Delta { get; set; }
        public int Theta { get; set; }
        public int Alpha { get; set; }
        public int Beta { get; set; }

        /// <summary>
        /// Numeric identifier for the current brain state (1: Delta, 2: Theta, 3: Alpha, 4: Beta).
        /// </summary>
        public int State { get; set; }


        /// <summary>
        /// Gets a human-readable description of the brain state based on the State property.
        /// </summary>
        public string StateText 
        {
            get 
            {
                switch (State)
                {
                    case 1: return "Deep Sleep (Delta)";
                    case 2: return "Drowsy (Theta)";
                    case 3: return "Relaxed (Alpha)";
                    case 4: return "Focused (Beta)";
                    default: return "No Signal!";
                }
            }
        }

        /// <summary>
        /// Parses a raw string containing EEG band values and converts it into an <see cref="EEGData"/> object.
        /// </summary>
        /// <param name="rawData">The raw string data formatted from the sensor/serial stream.</param>
        /// <returns>An instance of <see cref="EEGData"/> if matching succeeds; otherwise, null.</returns>
        public static EEGData parse(string rawData)
        {

            // Regex pattern to extract band percentages and state from the raw input string
            Match match = Regex.Match(rawData, @"Delta=(\d+)%\s+Theta=(\d+)%\s+Alpha=(\d+)%\s+Beta=(\d+)%\s+State=(\d+)");
            if (match.Success) 
            {
                return new EEGData 
                {
                    Delta = int.Parse(match.Groups[1].Value),
                    Theta = int.Parse(match.Groups[2].Value),
                    Alpha = int.Parse(match.Groups[3].Value),
                    Beta = int.Parse(match.Groups[4].Value),
                    State = int.Parse(match.Groups[5].Value)
                };
            }

            // Return null if the raw data format does not match expected criteria
            return null;
        }
    }
}
