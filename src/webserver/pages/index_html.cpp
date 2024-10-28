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
        .logoContainer {
            display: flex;
            flex-direction: row;
            align-items: center;
            justify-content: center;
            color: #ffffff;
            fill: #ffffff;
        }
        .logoTextContainer {
            height: 100%;
            margin-left: 5px;
            font-size: 15px;
            padding-top: 3px;
        }
        .logoText {
            font-size: 23px;
            font-weight: 600;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="logoContainer">
            <svg xmlns="http://www.w3.org/2000/svg" width="40" height="40" viewBox="0 0 164 171">
                <defs>
                <style>
                    .cls-1 {
                    fill-rule: evenodd;
                    }
                </style>
                </defs>
                <path class="cls-1" d="M92,0h71L82,85H9L92,0h0Z"/>
                <path id="Rectangle_1_copy" data-name="Rectangle 1 copy" class="cls-1" d="M83,85.5h71l-81,85H0l83-85h0Z"/>
            </svg>
            <div class="logoTextContainer">
                <span class="logoText">AirRemote</span>
            </div>
        </div>

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