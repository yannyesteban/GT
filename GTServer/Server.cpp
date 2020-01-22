#include <stdio.h>
#include "Server.h"

GT::Server::Server(SocketInfo pInfo):Socket(pInfo),debug(false) {
}

bool GT::Server::init(AppConfig pConfig) {
	//config = pConfig;
	pConfig.db.debug = pConfig.debug;
	db = new DB(pConfig.db);
	db->connect();
	db->loadProtocols();
	db->loadVersions();
	//db->loadClients();
	db->loadFormats();
	return true;
}

void GT::Server::onConnect(ConnInfo Info) {

}

void GT::Server::onMessage(ConnInfo Info) {
	printf("recibiendo: %s\n", Info.buffer);
}

void GT::Server::onClose(ConnInfo Info) {
}

using namespace std;
namespace GT {
	int Server::connectDB(InfoDB pInfo) {
		try {
			driver = get_driver_instance();

			char str_host[100] = "tcp://";
			strcat_s(str_host, pInfo.host);
			strcat_s(str_host, ":");
			strcat_s(str_host, pInfo.port);

			cn = driver->connect(str_host, pInfo.user, pInfo.pass);
			cout << "es correcta " << cn->isValid() << endl;
			/* Connect to the MySQL test database */
			cn->setSchema(pInfo.name);
			return 1;

		} catch (sql::SQLException & e) {
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			//cout << ", SQLState: " << e.getSQLState() << " )" << endl;
			return 0;
		}
	}
	bool Server::isSyncMsg(const char* buffer, char* id) {
		SyncMsg* sync_msg = (SyncMsg*)buffer;
		printf(ANSI_COLOR_CYAN "---> verification of sync (%lu)..." ANSI_COLOR_RESET, sync_msg->Keep_Alive_Device_ID);
		//puts(sync_msg->Keep_Alive_Device_ID));
		for (int i = 0; i < versions.n; i++) {
			if (sync_msg->Keep_Alive_Header == versions.e[i]) {
				sprintf(id, "%lu", sync_msg->Keep_Alive_Device_ID);
				return true;
			}
		}
		return false;
	}
}