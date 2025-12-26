#include "COMMON/common.h"
#include "CHANNEL/core/ChannelServer.h"

#if 1 /*DB 연결 테스트*/
#include "CHANNEL/db/MySqlConnectionPool.h"
#endif

#define DAEMON_NAME "CHANNEL_SERVER"
#define PORT 1234
#define LOG_PATH "../logs/channel"


int main()
{
    //log
    K_slog_init(LOG_PATH, DAEMON_NAME);
    K_slog_trace(K_SLOG_TRACE, "[%s]==============START==============", DAEMON_NAME);
   
#if 1 /*DB 연결*/
    {
        int result = 0;
        int num_fields = 0;
        MySqlConnectionPool* test= nullptr;
        test = MySqlConnectionPool::GetInstance();
        
        std::string query;

        query=std::string("SELECT * FROM channel");

        MYSQL* conn;
        conn = test->GetConnection();

        result = mysql_query(conn,query.c_str());
        if(result != 0)
        {
            K_slog_trace(K_SLOG_ERROR, "%s DB QUERY ERROR [%s]", DAEMON_NAME, mysql_error(conn));
            return -1;
        }

        MYSQL_RES *res = mysql_store_result(conn);

        if(!res){
            K_slog_trace(K_SLOG_ERROR, "%s DB QUERY ERROR [%s]", DAEMON_NAME, mysql_error(conn));
            return -1;
        }

        num_fields = mysql_num_fields(res);
        MYSQL_ROW row;

       while((row = mysql_fetch_row(res)) != nullptr)
       {
            for(int i = 0; i< num_fields; i++)
            {
                K_slog_trace(K_SLOG_TRACE, "%s value[%s] ", DAEMON_NAME, row[i]);
            }
       }

        mysql_free_result(res);
    }
#endif
    bool Start = false;
    ChannelServer channelServer;

    Start = channelServer.Init(PORT);

    if(Start == false)
    {
        return -1;
    }

    channelServer.Run();


    K_slog_trace(K_SLOG_TRACE, "[%s]..................the End..............", DAEMON_NAME);
    K_slog_close();
    return 0;

}