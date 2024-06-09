
#ifndef WEBSOCKET_HANDLER_H
#define WEBSOCKET_HANDLER_H

#define TIMEOUT_READ_SECONDS 20
#define DIGITS_UINT16_T 5

#define SAMPLE_ERROR_RESPONSE "{\"action\":\"error\",\"requestId\":\"xVB_1716157277.932128\",\"body\":\"Lorem ipsum dolor sit amet, consectetur adipiscing elit.Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\"}"
#define SAMPLE_ACK_RESPONSE "{\"action\":\"ack\",\"requestId\":\"xVB_1716157277.932128\"}"
#define SAMPLE_FIXED_READ_RESPONSE "{\"action\":\"ack\",\"requestId\":\"xVB_1716157277.932128\",\"bufferSize\":\"255\",\"rawBuffer\":\"\"}"

#define ERROR_RESPONSE_LENGTH sizeof(SAMPLE_ERROR_RESPONSE)
#define ACK_RESPONSE_LENGTH sizeof(SAMPLE_ACK_RESPONSE)
#define FIXED_READ_RESPONSE_LENGTH sizeof(SAMPLE_FIXED_READ_RESPONSE)

#include <string>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>
#include "websocket_handler/utils/validators.h"
#include "utils/utils.h"
#include "ir_remote/ir_remote.h"


/**
 * @brief The WebSocketHandler class handles the communication with a WebSocket server.
 * 
 * This class provides methods for establishing a WebSocket connection, sending and receiving messages,
 * and handling different types of WebSocket events, primarily to handle incoming IR remote commands.
 */
class WebSocketHandler {
public:
    /**
     * @brief Constructs a WebSocketHandler object.
     * 
     * @param irRemote A pointer to the address of a IRremote object used for handling IR remote commands.
     */
    WebSocketHandler(IRremote** irRemote);

    /**
     * @brief Handles the command to execute an IR remote code received from the WebSocket server and send an ACK response.
     * 
     * @param requestId The unique identifier of the request.
     * @param bufferlen The size of the command.
     * @param rawBuffer The raw buffer containing the command to be executed.
     */
    void handleExecuteCommand(const char* requestId, const char* bufferlen, const char* rawBuffer);

    /**
     * @brief Handles the command to read an IR remote code received from the WebSocket server and send an ACK response
     * containing the result.
     * 
     * @param requestId The unique identifier of the request.
     */
    void handleReadCommand(const char* requestId);

    /**
     * @brief Sends an error message to the WebSocket server.
     * 
     * @param requestId The unique identifier of the request.
     * @param error The error message to be sent.
     */
    void sendErrorMessage(const char* requestId, const char* error);

    /**
     * @brief Callback function for WebSocket events.
     * 
     * @param type The type of the WebSocket event.
     * @param payload The payload data received from the WebSocket server.
     * @param length The length of the payload data.
     */
    void wsEventCallback(WStype_t type, uint8_t* payload, size_t length);

    /**
     * @brief Handles a WebSocket message received from the server, validates it and routes it to the appropriate method.
     * 
     * @param payload The payload data of the message.
     */
    void handleMessage(uint8_t* payload);

    /**
     * @brief Starts the WebSocket connection to the specified server.
     * 
     * @param ws_host The hostname or IP address of the WebSocket server.
     * @param ws_port The port number of the WebSocket server.
     * @param ws_url The URL of the WebSocket server.
     * @param ws_fingerprint The fingerprint of the WebSocket server's SSL certificate.
     * @param ws_protocol The protocol to be used for the WebSocket connection.
     */
    void startConnection(const char* ws_host, uint16_t ws_port, const char* ws_url, const char* ws_fingerprint, const char* ws_protocol);

    /**
     * @brief Checks if the WebSocket connection is currently established.
     * 
     * @return true if the WebSocket connection is established, false otherwise.
     */
    bool isConnected();

    /**
     * @brief Performs the necessary operations in the WebSocketHandler loop.
     * 
     * This method should be called periodically to handle WebSocket events and maintain the connection.
     */
    void loop();

    /**
     * @brief Disconnects the WebSocket connection.
     */
    void disconnect();

    /**
     * @brief Sends a text message to the WebSocket server.
     * 
     * @param payload The text message to be sent.
     */
    void sendTXT(char* payload);

    /**
     * @brief Enables the WebSocket heartbeat functionality.
     * 
     * @param pingInterval The interval between ping messages in milliseconds.
     * @param pongTimeout The timeout for receiving a pong message in milliseconds.
     * @param disconnectTimeoutCount The number of consecutive pong message timeouts before disconnecting.
     */
    void enableHeartbeat(uint32_t pingInterval, uint32_t pongTimeout, uint8_t disconnectTimeoutCount);

private:
    WebSocketsClient wsClient; /**< The underlying WebSocketsClient object for handling the WebSocket connection. */
    IRremote** irRemote; /**< A pointer to the address of a IRremote object used for handling IR remote commands. */
};


#endif