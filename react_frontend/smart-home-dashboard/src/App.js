import React, { useState, useEffect } from "react";
import "./App.css";

function App() {
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
        <div>
          <p>🧍 Intruder Distance: {data.distance} cm</p>
          <p>💨 Gas Level: {data.gasLevel}</p>
          <p>🚶 Motion: {data.motionDetected === 0 ? "Detected" : "None"}</p>
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
      <div style={{ border: "2px solid #555", width: "80%", maxWidth: 600 }}>
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
