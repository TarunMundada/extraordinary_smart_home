// import React, { useState, useEffect } from "react";
// import "./App.css";

// function App() {
//   const [data, setData] = useState(null);
//   const [password, setPassword] = useState("");

//   useEffect(() => {
//     const fetchStatus = () => {
//       fetch("http://192.168.214.68/status")
//         .then((res) => res.json())
//         .then((json) => setData(json))
//         .catch((err) => console.error("Error fetching data", err));
//     };

//     fetchStatus();
//     const interval = setInterval(fetchStatus, 3000);
//     return () => clearInterval(interval);
//   }, []);

//   const sendPassword = () => {
//     fetch("http://192.168.214.68/password", {
//       method: "POST",
//       headers: { "Content-Type": "application/x-www-form-urlencoded" },
//       body: `password=${encodeURIComponent(password)}`,
//     })
//       .then((res) => res.text())
//       .then((text) => alert(text))
//       .catch((err) => alert("Failed to send password"));
//   };

//   return (
//     <div className="App">
//       <h1>🏠 ESP8266 Home Automation Dashboard</h1>
//       {data ? (
//         <div>
//           <p>🧍 Intruder Distance: {data.distance} cm</p>
//           <p>💨 Gas Level: {data.gasLevel}</p>
//           <p>🚶 Motion: {data.motionDetected === 0 ? "Detected" : "None"}</p>
//         </div>
//       ) : (
//         <p>Loading sensor data...</p>
//       )}
//       <div>
//         <h2>🔐 Enter Password to Unlock Door</h2>
//         <input
//           type="password"
//           value={password}
//           onChange={(e) => setPassword(e.target.value)}
//         />
//         <button onClick={sendPassword}>Submit</button>
//       </div>
//       <h2>📷 Live Surveillance Feed</h2>
//       <div style={{ border: "2px solid #555", width: "80%", maxWidth: 600 }}>
//         <img
//           src="http://192.168.214.11:8080/video"
//           alt="Live Stream"
//           style={{ width: "100%" }}
//         />
//       </div>
//     </div>
//   );
// }

// export default App;

import React, { useState, useEffect } from "react";
import "./App.css";

function App() {
  const GAS_THRESHOLD = 950;
  const [data, setData] = useState(null);
  const [password, setPassword] = useState("");

  useEffect(() => {
    const fetchStatus = () => {
      fetch("http://192.168.214.68/status")
        .then((res) => res.json())
        .then((json) => setData(json))
        .catch((err) => console.error("Error fetching data", err));
    };

    fetchStatus();
    const interval = setInterval(fetchStatus, 3000);
    return () => clearInterval(interval);
  }, []);

  const sendPassword = () => {
    fetch("http://192.168.214.68/password", {
      method: "POST",
      headers: { "Content-Type": "application/x-www-form-urlencoded" },
      body: `password=${encodeURIComponent(password)}`,
    })
      .then((res) => res.text())
      .then((text) => alert(text))
      .catch((err) => alert("Failed to send password"));
  };

  return (
    <div className="App">
      <h1>🏠 ESP8266 Home Automation Dashboard</h1>

      {data ? (
  <div className="card-container">
    <div className="card">
      <h3>💨 Gas Level</h3>
      <p>{data.gasLevel ?? "N/A"}</p>
      {data.gasLevel > GAS_THRESHOLD && (
        <p style={{ color: "red", fontWeight: "bold" }}>
          ⚠️ Gas Detected! Level is high!
        </p>
      )}
    </div>
    <div className="card">
      <h3>🚶 Motion</h3>
      <p>{data.motionDetected === 0 ? "Detected: Light ON" : "None: Light OFF"}</p>
    </div>
    <div className="card">
      <h3>🔆 Light Intensity (LDR)</h3>
      <p>{data.ldrValue}</p>
    </div>
    <div className="card">
      <h3>🌧️ Rain Sensor</h3>
      <p>{data.rainValue == 0 ? "Pump OFF" : "Pump On"}</p>
    </div>
  </div>
) : (
  <p>Loading sensor data...</p>
)}


      <div>
        <h2>🔐 Enter Password to Unlock Door</h2>
        <input
          type="password"
          value={password}
          onChange={(e) => setPassword(e.target.value)}
        />
        <button onClick={sendPassword}>Submit</button>
      </div>

      <h2>📷 Live Surveillance Feed</h2>
      <div className="video-section">
        <img
          src="http://192.168.214.11:8080/video"
          alt="Live Stream"
          style={{ width: "100%" }}
        />
      </div>

    </div>
  );
}

export default App;
