import React, { useEffect, useState } from "react";
import "./App.css";

function App() {
  const [data, setData] = useState(null);

  useEffect(() => {
    const fetchStatus = () => {
      fetch("http://192.168.214.68/status")
        .then((res) => res.json())
        .then((json) => setData(json))
        .catch((err) => console.error("Error fetching data", err));
    };

    fetchStatus();
    const interval = setInterval(fetchStatus, 3000); // update every 3 seconds

    return () => clearInterval(interval);
  }, []);

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

      <DoorControl />
      
    </div>
  );

  function DoorControl() {
    const openDoor = () => {
      fetch("http://192.168.214.68/open-door") // replace with ESP IP
        .then((res) => res.text())
        .then((msg) => alert(msg))
        .catch((err) => console.error("Error opening door:", err));
    };
  
    return (
      <div style={{ marginTop: "20px" }}>
        <button
          onClick={openDoor}
          style={{
            padding: "10px 20px",
            backgroundColor: "#4CAF50",
            color: "white",
            fontSize: "16px",
            border: "none",
            borderRadius: "8px",
            cursor: "pointer",
          }}
        >
          🚪 Open Door
        </button>
      </div>
    );
  }
  
}

export default App;
