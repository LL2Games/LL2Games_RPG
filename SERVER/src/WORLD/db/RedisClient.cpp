#include "RedisClient.h"
#include "K_slog.h"

#define REDIS_HOST "100.124.14.8"
#define REDIS_PORT 6379


RedisClient *RedisClient::m_instance = nullptr;

RedisClient::RedisClient() : m_ctx(nullptr)
{
}

RedisClient::~RedisClient()
{
    if (m_instance)
    {
        if (m_ctx)
        {
            redisFree(m_ctx);
            m_ctx = nullptr;
        }
        delete m_instance;
        m_instance = nullptr;
    }
}

int RedisClient::Init()
{
   m_ctx = redisConnect(REDIS_HOST, REDIS_PORT);
   if (m_ctx == nullptr || m_ctx->err)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] Redis connection error: %s", __FUNCTION__, __LINE__, m_ctx ? m_ctx->errstr : "Can't allocate redis context");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

RedisClient* RedisClient::GetInstance()
{
    if (m_instance == nullptr)
    {
        m_instance = new RedisClient();
        if (m_instance->Init() != EXIT_SUCCESS)
        {
            delete m_instance;
            m_instance = nullptr;
        }
    }

    return m_instance;
}

int RedisClient::HSet(const std::string key, const std::string& field, const std::string& value, const int expire)
{
    int rc = EXIT_SUCCESS;
    redisReply* reply = nullptr;
    if (m_ctx == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] redis context is null", __FUNCTION__, __LINE__);
        rc = EXIT_FAILURE;
        goto err;
    }

    //HSET
    K_slog_trace(K_SLOG_DEBUG, "[%s][%d] HSET %s %s %s", __FUNCTION__, __LINE__, key.c_str(), field.c_str(), value.c_str());
    reply = (redisReply*)redisCommand(m_ctx, "HSET %s %s %s", key.c_str(), field.c_str(), value.c_str());
    if (reply == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] HSET command failed for key: %s", __FUNCTION__, __LINE__, key.c_str());
        rc = EXIT_FAILURE;
        goto err;
    }
    
    //유효기간 설정
    K_slog_trace(K_SLOG_DEBUG, "[%s][%d] EXPIRE %s %d", __FUNCTION__, __LINE__, key.c_str(), expire);
    reply = (redisReply*)redisCommand(m_ctx, "EXPIRE %s %d", key.c_str(), expire);
    if (reply == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] EXPIRE command failed for key: %s, expire: %d", __FUNCTION__, __LINE__, key.c_str(), expire);
        rc = EXIT_FAILURE;
        goto err;
    }

err:
    if (reply)
        freeReplyObject(reply);
    return rc;
}

std::optional<std::map<std::string, std::string>> RedisClient::HGetAll(const std::string key)
{
    std::map<std::string, std::string> result;
    redisReply* reply = nullptr;
    if (m_ctx == nullptr)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] redis context is null", __FUNCTION__, __LINE__);
        return std::nullopt;
    }

    reply = (redisReply*)redisCommand(m_ctx, "HGETALL %s", key.c_str());
    if (reply == nullptr || reply->type != REDIS_REPLY_ARRAY)
    {
        K_slog_trace(K_SLOG_ERROR, "[%s][%d] HGETALL command failed for key: %s", __FUNCTION__, __LINE__, key.c_str());
        if (reply)
            freeReplyObject(reply);
        return std::nullopt;
    }

    for (size_t i = 0; i < reply->elements; i += 2)
    {
        std::string field = reply->element[i]->str;
        std::string value = reply->element[i + 1]->str;
        result[field] = value;
    }

    freeReplyObject(reply);
    return result;
}
