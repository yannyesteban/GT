#include "GTComm.h"

GT::GTComm::GTComm(GTAppConfig pConfig) : config(pConfig)
{
}

GT::GTComm::~GTComm()
{
}

void GT::GTComm::start(std::string configFile)
{
    
    
    std::cout << "Hello: el archivo de configuracion es: " << configFile << "\n";
    rapidjson::Document d = loadConfig(configFile.c_str());

    std::cout << "App " << d["appname"].GetString() << "\n\n";

    const rapidjson::Value& a = d["servers"];
    std::vector<std::thread*> tasks;
    for (rapidjson::SizeType i = 0; i < a.Size(); i++) {
        rapidjson::Value& pp = d["servers"][i];
        auto infoConfig = getConfig(pp);

        
        std::cout << "App " << infoConfig.appname << " STATUS: " << infoConfig.status << "\n\n";
       
        if (infoConfig.status == 1) {
            tasks.push_back(new std::thread(runServer, &infoConfig));
            Sleep(100);
        }
        
        

    }
    //std::cout << "...Bye" << "\n";
    for (std::vector<std::thread*>::iterator it = tasks.begin(); it != tasks.end(); ++it) {
        (*it)->join();
    }
    
    
    //std::cout << "...Bye 2" << "\n";
    
}

rapidjson::Document loadConfig(const char* path)
{
    
    	
	FILE* fp = fopen(path, "rb"); // non-Windowsyannyesteban@ho use "r"

	if (fp == NULL)
	{
		perror("Error while opening the file.\n");
		exit(EXIT_FAILURE);
	}

	char* readBuffer;

	readBuffer = (char*)malloc(1500);
	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	rapidjson::Document d;

	d.ParseStream(is);
	fclose(fp);

	
	if (readBuffer != NULL) {
		readBuffer[0] = '\0';
		free(readBuffer);
	}
    return d;
}

GT::AppConfig getConfig(rapidjson::Value& d)
{
    GT::AppConfig info;
    info.appname = d["appname"].GetString();
    info.port = d["port"].GetInt();
    info.max_clients = d["max_clients"].GetInt();
    info.version = d["version"].GetString();
    info.debug = d["debug"].GetBool();
    info.show_cache = d["show_cache"].GetBool();
    info.keep_alive = d["keep_alive"].GetInt();
    info.waitTime = d["wait_time"].GetInt();
    info.status = d["status"].GetInt();

    info.db.host = d["db"]["host"].GetString();
    info.db.port = d["db"]["port"].GetString();
    info.db.user = d["db"]["user"].GetString();
    info.db.pass = d["db"]["pass"].GetString();
    info.db.name = d["db"]["name"].GetString();
    
    return info;
}

void runServer(GT::AppConfig* config)
{

    auto appInfo = config;
    std::cout << Color::_yellow() << "NAME: " << appInfo->appname << Color::_reset() << std::endl;
    printf("Version: %s \n", appInfo->version);
    printf("DB Name: %s\n", appInfo->db.name);
    printf("Socket Port: %d\n\n", appInfo->port);
    printf("Max Clients: %d\n\n", appInfo->max_clients);
    //printf("Time: %s\n\n", XT::Time::now());

    GT::SocketInfo Info;
    Info.host = (char*)"127.0.0.1";
    Info.port = appInfo->port;
    Info.maxClients = appInfo->max_clients;

    GT::Server* S = new GT::Server(Info);
    S->init(*appInfo);
    S->start();
}
