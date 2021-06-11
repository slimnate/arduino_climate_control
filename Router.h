#ifndef ROUTER_H
#define ROUTER_H

#include "WebServer.h"

typedef void(*rest_callback_t)(WebRequest&, WebResponse&); // typedef for rest route callback function

const byte ROUTER_MAX_ROUTES = 20; // max number of routes that can be held by the router

// represents a REST route in the router
struct Route {
    String method; // request method
    String path; // request path
    rest_callback_t callback;
};

// REST server router class
class Router {
    private:
        int _routeIndex; // current route index, used for adding new routes to the correct location.
    public:
        WebServer server; // the underlying WebServer instance
        Route routes[ROUTER_MAX_ROUTES]; // list of routes currently set up

        bool route(String, String, rest_callback_t); // add new route to the router (method, path, callback);
        bool get(String, rest_callback_t); // helper to add new GET route
        bool post(String, rest_callback_t); // helper to add new POST route

        void handle(WebRequest&);

};

#endif