#include "webserver/pages/wifi_html.h"


char success_html[] = R"=====(

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>AirRemote WiFi Connected</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #121212;
            color: #ffffff;
            margin: 0;
            padding: 0;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            height: 100vh;
        }
        .container {
            background: rgba(30, 30, 30, 0.7);
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.5);
            width: 90%;
            max-width: 400px;
            text-align: center;
            backdrop-filter: blur(10px);
            border: 1px solid rgba(255, 255, 255, 0.2);
        }
        h1 {
            font-size: 24px;
            /* margin-bottom: 0px; */
        }
        h2 {
            font-size: 30px;
            margin-bottom: 20px;
        }
        .success-message {
            font-size: 18px;
            margin-bottom: 20px;
        }
        .success-button {
            background: linear-gradient(30deg, #3e00b3, #a230ed);
            color: #ffffff;
            padding: 10px 20px;
            border: none;
            border-radius: 4px;
            cursor: pointer;
            font-size: 16px;
            transition: background 0.3s, transform 0.3s;
            box-shadow: 0 4px 15px rgba(72, 61, 139, 0.4);
        }
        .success-button:hover {
            background: linear-gradient(45deg, #9370db, #1e90ff);
            transform: scale(1.05);
        }
    </style>
</head>
<body>
    <div class="container">
        <h2>WiFi Connected</h2>
        <p class="success-message">Your device is now connected to the WiFi network successfully.</p>
        <button class="success-button" onclick="window.location.href='/'">Go to Homepage</button>
    </div>
</body>
</html>
)=====";