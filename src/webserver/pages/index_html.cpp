#include "webserver/pages/index_html.h"

char index_html[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>AirRemote Setup</title>
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
            width: 100wh;
            overflow: hidden;
        }
        .container {
            background: rgba(30, 30, 30, 0.7);
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.5);
            width: 85%;
            max-width: 400px;
            text-align: center;
            backdrop-filter: blur(10px);
            border: 1px solid rgba(255, 255, 255, 0.2);
        }
        h1 {
            font-size: 24px;
            margin-bottom: 20px;
        }
        .options {
            margin: 20px 0;
        }
        .options input {
            display: none;
        }
        .options label {
            display: block;
            padding: 10px;
            margin: 10px 0;
            background-color: #333;
            border-radius: 4px;
            cursor: pointer;
            transition: background-color 0.3s;
        }
        .options input:checked + label {
            background: linear-gradient(30deg, #3e00b3, #a230ed);
        }
        .submit-button {
            background: linear-gradient(30deg, #3e00b3, #a230ed);
            color: #ffffff;
            padding: 10px 20px;
			width: 120px;
            border: none;
            border-radius: 4px;
            cursor: pointer;
            font-size: 16px;
            transition: background 0.3s, transform 0.3s;
            box-shadow: 0 4px 15px rgba(72, 61, 139, 0.4);
        }
        .submit-button:hover {
            background: linear-gradient(45deg, #9370db, #1e90ff);
            transform: scale(1.05);
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>AirRemote</h1>
        <form action="/redirect-setup" method="POST">
            <div class="options">
                <input type="radio" id="wifi-setup" name="setup-option" value="wifi-setup" checked>
                <label for="wifi-setup">WiFi Setup Only</label>
                <input type="radio" id="complete-setup" name="setup-option" value="complete-setup">
                <label for="complete-setup">Complete Setup</label>
            </div>
            <button type="submit" class="submit-button">Submit</button>
        </form>
    </div>
</body>
</html>
)=====";