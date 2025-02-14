const fs = require("fs");


let config = JSON.parse(fs.readFileSync("config.json", "utf-8"));

let email_user = config.email.user
let email_passwd = config.email.passwd

let redis_host = config.redis.host
let redis_port = config.redis.port
let redis_passwd = config.redis.passwd

let mysql_host = config.mysql.host
let mysql_port = config.mysql.port
let mysql_user = config.mysql.user
let mysql_passwd = config.mysql.passwd


module.exports = {
    email_user, email_passwd,
    redis_host, redis_port, redis_passwd,
    mysql_host, mysql_port, mysql_user, mysql_passwd
};