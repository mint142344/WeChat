const Redis = require("ioredis");
const config = require("./config");

const keyPrefix = "email_verify_code:";

class RedisClient {
    constructor() {
        this.client = new Redis({
            host: config.redis_host,
            port: config.redis_port,
            password: config.redis_passwd,

            // 重连策略
            retryStrategy(times) {
                const delay = Math.min(times * 50, 2000);
                if (times > 5) {
                    console.error("Redis connection retry failed");
                    return null;
                }

                return delay;
            }
        });

        this.client.on("error", function (error) {
            console.error(error);
        });

        this.client.on("connect", function () {
            console.log("Redis connected");
        });

    }

    async setEamilVerifyCode(email, code, expireSeconds = 60 * 3) {
        return await this.client.set(keyPrefix + email, code, "EX", expireSeconds);
    }

    async getEmailVerifyCode(email) {
        return await this.client.get(keyPrefix + email);
    }

    async delEmailVerifyCode(email) {
        return await this.client.del(keyPrefix + email);
    }

    async close() {
        return await this.client.quit();
    }
}

const redisClient = new RedisClient();

module.exports = { redisClient };