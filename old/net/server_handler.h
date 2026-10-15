#pragma once

#include <iostream>
#include <ixwebsocket/IXWebSocketServer.h>

class ServerHandler
{
public:
    void operator()(std::shared_ptr<ix::ConnectionState> connectionState, ix::WebSocket &webSocket, const ix::WebSocketMessagePtr &msg) const
    {
        switch (msg->type)
        {
        case ix::WebSocketMessageType::Message:
            OnMessage(connectionState, webSocket, msg);
            break;

        case ix::WebSocketMessageType::Open:
            OnOpen(connectionState, webSocket, msg);
            break;

        case ix::WebSocketMessageType::Close:
            OnClose(connectionState, webSocket, msg);
            break;

        case ix::WebSocketMessageType::Error:
            OnError(connectionState, webSocket, msg);
            break;

        default:
            break;
        }
    }

private:
    void OnMessage(std::shared_ptr<ix::ConnectionState> state, ix::WebSocket &ws, const ix::WebSocketMessagePtr &msg) const
    {
        std::cout << "Received: " << msg->str << std::endl;
        ws.send(msg->str, msg->binary);
    }

    void OnOpen(std::shared_ptr<ix::ConnectionState> state, ix::WebSocket &ws, const ix::WebSocketMessagePtr &msg) const
    {
        std::cout << "Client connected: " << state->getRemoteIp() << std::endl;
    }

    void OnClose(std::shared_ptr<ix::ConnectionState> state, ix::WebSocket &ws, const ix::WebSocketMessagePtr &msg) const
    {
        std::cout << "Client disconnected: " << state->getRemoteIp() << std::endl;
    }

    void OnError(std::shared_ptr<ix::ConnectionState> state, ix::WebSocket &ws, const ix::WebSocketMessagePtr &msg) const
    {
        std::cerr << "WebSocket error: " << msg->errorInfo.reason << std::endl;
    }
};
