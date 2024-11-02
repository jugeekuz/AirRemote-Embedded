#include "webserver/pages/api_html.h"

const char api_html[] = R"=====(
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>AirRemote API Credentials</title>
<script>
    const webSocketTimeout = 20000; 
    const pollingInterval = 1000;
    function startPollingWebSocketStatus() {
        const intervalId = setInterval(() => {
            checkWebSocketStatus(intervalId);
        }, pollingInterval);

        setTimeout(() => {
            clearInterval(intervalId);
            console.log('Polling stopped after 10 seconds');
            window.location.href = '/error';
        }, webSocketTimeout); 
    }
    function checkWebSocketStatus(intervalId) {
        fetch('/websocket-status')
        .then(response => response.json())
        .then(data => {
            if (data.status === 'connected') {
                clearInterval(intervalId);
                
                window.location.href = '/success';
            }
        });
    } 
    function validateWifiPassword(password) {
        const regex = /^.{8,63}$/;
        return regex.test(password);
    }
    function submitForm(event) {
        event.preventDefault();
        const formData = new FormData(event.target);
        const password = formData.get('password');
        
        fetch('/websocket-connect', {
            method: 'POST',
            body: formData
        }).then(response => response.text())
        .then(result => {
            console.log(result);
            setTimeout(startPollingWebSocketStatus, 2000);
        });
    }
    document.addEventListener('DOMContentLoaded', () => {
        document.querySelector('.submit-button').addEventListener('click', () => {
            document.querySelector('#loader-container').className = 'loading';
        });
    });
</script>
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
        overflow-x: hidden;
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
        margin-top: 10px;
        margin-bottom: 10px;
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
    .loader-wifi {
        width: 48px;
        height: 48px;
        border: 3px solid #FFF;
        border-radius: 50%;
        display: inline-block;
        position: relative;
        box-sizing: border-box;
        animation: rotation 1s linear infinite;
    }
    .loader-wifi::after {
        content: '';  
        box-sizing: border-box;
        position: absolute;
        left: 50%;
        top: 50%;
        transform: translate(-50%, -50%);
        width: 56px;
        height: 56px;
        border-radius: 50%;
        border: 3px solid;
        border-color: #FF3D00 transparent;
    }
    @keyframes animloader {
        0% {
            transform: scale(0);
            opacity: 1;
        }
        100% {
            transform: scale(1);
            opacity: 0;
        }
    } 
    .idle .loader-wifi {
        display: none;
    }
    .loading .submit-button {
        display: none;
    }
    @keyframes rotation {
        0% {
            transform: rotate(0deg);
        }
        100% {
            transform: rotate(360deg);
        }
    } 
</style>
</head>
<body>
    <div class="container">
        <h2>API Setup</h2>
        <form onsubmit="submitForm(event)">
            <div class="form-group-1">
                <label for="WS_HOST">WSS HOST</label>
                <input type="text" id="WS_HOST" name="WS_HOST" placeholder="ex. fa8d3.execute-api.eu.amazonaws.com">
            </div>
            <div class="form-group-2">
                <label for="WS_URL">WSS URL</label>
                <input type="text" id="WS_URL" name="WS_URL" placeholder="ex. /dev">
            </div>
            <div class="form-group-2">
                <label for="WS_PORT">WSS Port</label>
                <input type="number" id="WS_PORT" name="WS_PORT" value="443">
            </div>
            <div class="form-group-3">
                <label for="auth-key">Authentication token</label>
                <input type="password" id="AUTH_TOKEN" name="AUTH_TOKEN" placeholder="Enter your key">
            </div>
            <div id="loader-container" class="idle">
                <span class="loader-wifi"></span>
                <button type="submit" class="submit-button">Submit</button>
            </div>
        </form>
    </div>
</body>
</html>
)=====";
