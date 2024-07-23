#include "webserver/pages/api_html.h"

const char api_html[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>AirRemote API Credentials</title>
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
            /* margin-bottom: 0px; */
        }
        h2 {
            font-size: 30px;
            margin-bottom: 20px;
        }
        label {
            display: block;
            text-align: left;
        }
        input[type="text"], input[type="number"], input[type="password"] {
            width: 100%;
			height: 40px;
            padding: 10px;
            margin-top: 10px;
            margin-bottom: 15px;
            border-radius: 4px;
            border: 1px solid #333;
            background-color: #333;
            color: #fff;
            font-size: 16px;
            box-sizing: border-box;
        }
        input[type="text"]:focus, input[type="number"]:focus, input[type="password"]:focus {
            border: 2px solid;
            border-radius: 4px;
            border-image-slice: 1;
            border-image-source: linear-gradient(30deg, #3e00b3, #a230ed);
            outline: none;
        }
        .submit-button {
            background: linear-gradient(30deg, #3e00b3, #a230ed);
            color: #ffffff;
            padding: 10px 20px;
            width: 160px;
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
        <h2>API Credentials Setup</h2>
        <form>
            <div class="form-group-1">
                <label for="wss-url">WSS URL</label>
                <input type="text" id="wss-url" name="wss-url" placeholder="wss://example.com">
            </div>
            <div class="form-group-2">
                <label for="wss-port">WSS Port</label>
                <input type="number" id="wss-port" name="wss-port" value="443">
            </div>
            <div class="form-group-3">
                <label for="auth-key">Authentication Key</label>
                <input type="password" id="auth-key" name="auth-key" placeholder="Enter your key">
            </div>
            <button type="submit" class="submit-button">Submit</button>
        </form>
    </div>
</body>
</html>

)=====";
