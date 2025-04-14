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
    </div>
  );
}

export default App;
