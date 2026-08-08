#include <iostream>
#include <ixwebsocket/IXWebSocketServer.h>
#include <pybind11/pybind11.h>
#include <thread>

namespace py = pybind11;

static bool world_initialized = false;

void RunEchoServer()
{
    int port = 9001;
    std::string host("0.0.0.0");
    ix::WebSocketServer server(port, host);

    server.setOnClientMessageCallback(
        [](std::shared_ptr<ix::ConnectionState> connectionState,
           ix::WebSocket &webSocket,
           const ix::WebSocketMessagePtr &msg)
        {
            if (msg->type == ix::WebSocketMessageType::Open)
            {
                std::cout << "Client connected: "
                          << connectionState->getRemoteIp() << std::endl;
            }
            else if (msg->type == ix::WebSocketMessageType::Message)
            {
                std::cout << "Received: " << msg->str << std::endl;
                webSocket.send(msg->str, msg->binary);
            }
        });

    auto res = server.listen();
    if (!res.first)
    {
        std::cerr << "Failed to listen: " << res.second << std::endl;
        return;
    }

    std::cout << "Echo server listening on ws://" << host << ":" << port << std::endl;

    server.start();
    server.wait();
}

int InitWorld()
{
    if (world_initialized)
        return -1;

    world_initialized = true;

    std::thread echoThread(RunEchoServer);
    echoThread.detach();

    return 0;
}

void ShutdownWorld()
{
    world_initialized = false;
}

PYBIND11_MODULE(_core, m)
{
    m.def("world_init", &InitWorld, "Initialize world (starts echo server)");
    m.def("world_shutdown", &ShutdownWorld, "Shutdown world");
}
