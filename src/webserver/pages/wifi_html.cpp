#include "webserver/pages/wifi_html.h"

char wifi_html[] = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>AirRemote WiFi Setup</title>
<script>
    const wifiTimeout = 10000; 
    const wpsTimeout = 30000;
    const pollingInterval = 1000;
    const wifiSearchTimeout = 2000;
    function startPollingWifiStatus(timeoutMs) {
        const intervalId = setInterval(() => {
            checkWifiStatus(intervalId);
        }, pollingInterval);

        setTimeout(() => {
            clearInterval(intervalId);
            console.log('Polling stopped after 10 seconds');
            window.location.href = '/error';
        }, timeoutMs); 
    }

    function updateWifi() {
        fetch('/wifi-list')
        .then(response => response.text())
        .then(data => {
            console.log(data)
            const select = document.getElementById('ssid');
            select.innerHTML = data;
        }).catch(error => {
            console.error('Error fetching Wi-Fi:', error);
        });
    }
    function wifiScan() {
        fetch('/wifi-scan')
        .then(response => response.json())
        .then(data => {
            console.log(data);
            if (data.status === 'connecting') {
                setTimeout(() => updateWifi(), wifiSearchTimeout);
            } else {
                console.log("ERROR: Couldn't create task.")
            }
        });
        
    }   

    function checkWifiStatus(intervalId) {
        fetch('/wifi-status')
        .then(response => response.json())
        .then(data => {
            if (data.status === 'connected') {
                clearInterval(intervalId);
                const params = new URLSearchParams(window.location.search);
                const param = params.get('setup');
                if (param === 'complete-setup') {
                    window.location.href = '/api-page';
                } else {
                    window.location.href = '/success';
                }
            }
        });
    } 

    function validateWifiPassword(password) {
        const regex = /^.{8,63}$/;
        return regex.test(password);
    }

    function submitFormWifi(event) {
        event.preventDefault();
        const formData = new FormData(event.target);
        const password = formData.get('password');
        if (!validateWifiPassword(password)) {
            alert('Password must be between 8 and 63 characters');
            return;
        }
        fetch('/wifi-connect', {
            method: 'POST',
            body: formData
        }).then(response => response.text())
        .then(result => {
            console.log(result);
            startPollingWifiStatus(wifiTimeout);
        });
    }

    function submitFormWPS(event) {
        event.preventDefault();
        fetch('/wps-connect', {
            method: 'POST'
        }).then(response => response.text())
        .then(result => {
            console.log(result);
            startPollingWifiStatus(wpsTimeout);
        });
    }
    
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
    }
    h2 {
        font-size: 30px;
        margin-bottom: 20px;
    }
    label {
        display: block;
        text-align: left;
    }
    select, input[type="password"] {
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
    select:focus, input[type="password"]:focus {
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
    .separator {
        color: #aaaaaa;
        margin: 10px 0;
    }
    .wps-button {
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
    .wps-button:hover {
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
    .loader-wps {
        width: 48px;
        height: 48px;
        background: #FFF;
        display: inline-block;
        border-radius: 50%;
        box-sizing: border-box;
        animation: animloader 1s ease-in infinite;
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
    @keyframes rotation {
        0% {
            transform: rotate(0deg);
        }
        100% {
            transform: rotate(360deg);
        }
    } 
    .idle .loader-wifi,
    .idle .loader-wps,
    .wifi .loader-wps,
    .wifi .submit-button,
    .wifi .wps-button,
    .wifi .separator,
    .wifi .form-group,
    .wps .loader-wifi,
    .wps .submit-button,
    .wps .wps-button,
    .wps .form-group,
    .wps .separator {
        display: none;
    }
</style>
</head>
<body>
<div class="container">
    <h2>WiFi Setup</h2>
    <div class="idle">    
        <form onsubmit="submitFormWifi(event)">
            <div class="form-group">
                <label for="ssid">WiFi SSID</label>
                <select id="ssid" name="ssid">
                &                                            
                                                                
                                                                
                                                                
                                                                
                                                                
                                                                
                                                                
                                                                
                                                                
                                                                
                                                                
                                                                
                                                                
                                                                &
                </select>
                <label for="password">WiFi Password</label>
                <input type="password" id="password" name="password">
            </div>
            <div class="buttons-container">            
                <span class="loader-wifi"></span>
                <span class="loader-wps"></span>
                <button type="submit" class="submit-button">Submit</button>
                <div class="separator">- OR -</div>
            </div>
        </form>
        <form onsubmit="submitFormWPS(event)">
            <button type="submit" class="wps-button">WPS Connect</button>
        </form>
    </div>
</div>
<script>
        document.querySelector('.submit-button').addEventListener('click', () => {
            document.querySelector('.container > div').className = 'wifi';
        });
        document.querySelector('.wps-button').addEventListener('click', () => {
            document.querySelector('.container > div').className = 'wps';
        });
</script>
</body>
</html>
)=====";