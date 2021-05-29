#include "Router.h"

#include "WebServer.h"

//register a new route on the router
bool Router::route(String method, String path, rest_callback_t cb) {
    if(_routeIndex < ROUTER_MAX_ROUTES) {
        routes[_routeIndex] = Route { method, path, cb};
        _routeIndex++;
    }
    return false;
};

//Helper to register new GET route
bool Router::get(String path, rest_callback_t cb) {
    return route("GET", path, cb);
};

//Helper to register new POST route
bool Router::post(String path, rest_callback_t cb) {
    return route("POST", path, cb);
};

// Handle an incoming WebRequest by calling the correct handler for the path
void Router::handle(WebRequest& req) {
    // find matching route
    for(int i = 0; i < _routeIndex; i++) {
        Route r = routes[i];
        if(r.method == req.method && r.path == req.path) {
            WebResponse res = req.getResponse();
            r.callback(req, res);
            return;
        }
    }

    Serial.println("No matching routes for request");
    WebResponse res = req.getResponse();
    res.status = HTTP_NOT_FOUND;
    res.send();
};