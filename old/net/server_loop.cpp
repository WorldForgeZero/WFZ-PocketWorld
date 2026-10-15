#include <iostream>
#include <ixwebsocket/IXWebSocketServer.h>

#include "server_handler.h"
#include "server_loop.h"

// Пока это всё можно назвать чисто бесполезной хренью. Мне нужно по хорошему разделить все системы, а не пихать всё в 1 блок
// Но кто мне что скажет, я в другом городе

void ServerRun(std::string host, int port)
{
    ix::WebSocketServer server(port, host);

    server.setOnClientMessageCallback(ServerHandler{});

    auto [ok, err] = server.listen();
    if (!ok)
    {
        throw std::runtime_error("WebSocket server failed to listen: " + err);
    }

    std::cout << "Echo server listening on ws://" << host << ":" << port << std::endl;
    server.start();
    server.wait();
}
