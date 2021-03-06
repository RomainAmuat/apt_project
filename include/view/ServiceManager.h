#ifndef SERVICE_MANAGER_H
#define SERVICE_MANAGER_H

#include "model/MySQLLink.h"

#include "controler/Player/PlayerControler.h"
#include "controler/Game/GameControler.h"

#include "view/Generic.h"

#include "ignored/infos.h"

#include <algorithm>

#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>

#include "json/json.h"
#include <json/reader.h>
#include <json/writer.h>

using namespace std;
using namespace Pistache;

class ServiceManager {
public:
    ServiceManager(Address addr, ConMySQL * msql);
    ~ServiceManager();

    void init(size_t thr = 2);
    void start();

    void shutdown();

private:
    void setupRoutes();

    void post_new_player(const Rest::Request& request, Http::ResponseWriter response);
    void get_all_players(const Rest::Request& request, Http::ResponseWriter response);
    void get_player_from_id(const Rest::Request& request, Http::ResponseWriter response);
    void delete_player_from_id(const Rest::Request& request, Http::ResponseWriter response);

    void post_info_player(const Rest::Request& request, Http::ResponseWriter response);

    void post_new_game(const Rest::Request& request, Http::ResponseWriter response);
    void get_game_from_id(const Rest::Request& request, Http::ResponseWriter response);
    void get_all_games(const Rest::Request& request, Http::ResponseWriter response);
    void delete_game_from_id(const Rest::Request& request, Http::ResponseWriter response);
    
    PlayerControler * player_controler;
    GameControler * game_controler;
    ConMySQL * msql;

    std::shared_ptr<Http::Endpoint> httpEndpoint;
    Rest::Router router;

};

#endif