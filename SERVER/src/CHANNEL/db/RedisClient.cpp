#include "RedisClient.h"


#define REDIS_HOST "100.114.42.54"
#define REDIS_PORT 6379

RedisClient *RedisClient::m_instance =nullptr;

RedisClient::RedisClient() : m_ctx(nullptr)
{

}

RedisClient::~RedisClient()
{
 
    if(m_ctx)
    {
        redisFree(m_ctx);
        m_ctx = nullptr;
    }

}

int RedisClient::Init()
{
    if (m_ctx != nullptr)
    {
        return EXIT_SUCCESS;
    }

    m_ctx = redisConnect(REDIS_HOST, REDIS_PORT);

    if (m_ctx == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] Redis Connect error: ctx is null", __FUNCTION__, __LINE__);
        return EXIT_FAILURE;
    }

    if (m_ctx->err)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] Redis Connect error(%d): %s", __FUNCTION__, __LINE__, m_ctx->err, m_ctx->errstr);
        redisFree(m_ctx);
        m_ctx = nullptr;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

bool RedisClient::Connect()
{
    return Init() == EXIT_SUCCESS;
}

RedisClient *RedisClient::GetInstance()
{
    if(m_instance == nullptr)
    {
        m_instance = new RedisClient();
        if(m_instance->Init() != EXIT_SUCCESS)
        {
            delete m_instance;
            m_instance = nullptr;
        }
    }
    return m_instance;
}

int RedisClient::HSet(const std::string key, const std::string& field, const std::string& value, const int expire)
{
    redisReply* reply = nullptr;

    if (m_ctx == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] redis context is null", __FUNCTION__, __LINE__);
        return EXIT_FAILURE;
    }

    reply = (redisReply*)redisCommand(m_ctx, "HSET %s %s %s", key.c_str(), field.c_str(), value.c_str());
    if (reply == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] HSET command failed for key: %s", __FUNCTION__, __LINE__, key.c_str());
        return EXIT_FAILURE;
    }

    freeReplyObject(reply);
    reply = nullptr;

    reply = (redisReply*)redisCommand(m_ctx, "EXPIRE %s %d", key.c_str(), expire);
    if (reply == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] EXPIRE command failed for key: %s, expire: %d", __FUNCTION__, __LINE__, key.c_str(), expire);
        return EXIT_FAILURE;
    }

    freeReplyObject(reply);
    return EXIT_SUCCESS;
}

int RedisClient::HSetAll(const std::string& key, std::map<std::string, std::string> redis_map, const int expire)
{
    int rc = EXIT_SUCCESS;
    std::vector<const char*> value;
    std::vector<size_t> valueLen;
    redisReply* reply = nullptr;
    std::string cmd = "HSET";

    auto push = [&](const std::string& s)
    {
        value.push_back(s.c_str());
        valueLen.push_back(s.size());
    };

    if (m_ctx == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] redis context is null", __FUNCTION__, __LINE__);
        return EXIT_FAILURE;
    }

    if (redis_map.empty())
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] redis_map is empty", __FUNCTION__, __LINE__);
        return EXIT_FAILURE;
    }

    value.reserve(2 + redis_map.size() * 2);
    valueLen.reserve(2 + redis_map.size() * 2);

    push(cmd);
    push(key);

    for (const auto& [field, fieldValue] : redis_map)
    {
        push(field);
        push(fieldValue);
    }

    reply = (redisReply*)redisCommandArgv(m_ctx, (int)value.size(), value.data(), valueLen.data());
    if (reply == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] redisCommandArgv is failed", __FUNCTION__, __LINE__);
        return EXIT_FAILURE;
    }

    freeReplyObject(reply);
    reply = nullptr;

    reply = (redisReply*)redisCommand(m_ctx, "EXPIRE %s %d", key.c_str(), expire);
    if (reply == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] redisCommand EXPIRE is failed", __FUNCTION__, __LINE__);
        return EXIT_FAILURE;
    }

    freeReplyObject(reply);
    return rc;
}

std::optional<std::map<std::string, std::string>> RedisClient::HGetAll(const std::string key)
{
    std::map<std::string, std::string> result;
    redisReply* reply = nullptr;
    if(m_ctx == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] redis context is null", __FUNCTION__, __LINE__);
        return std::nullopt;
    }

    reply = (redisReply*)redisCommand(m_ctx, "HGETALL %s", key.c_str());
    if(reply == nullptr || reply->type != REDIS_REPLY_ARRAY)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] HGETALL command failed for key: %s", __FUNCTION__, __LINE__, key.c_str());
        if(reply)
        {
            freeReplyObject(reply);
        }
        return std::nullopt;
    }

    for(size_t i=0; i < reply->elements; i+=2)
    {
        std::string field = reply->element[i]->str;
        std::string value = reply->element[i+1]->str;
        result[field] = value;
    }

    freeReplyObject(reply);
    return result;
}
