-- 用户表
CREATE TABLE users (
    id          INT             AUTO_INCREMENT PRIMARY KEY,
    username    VARCHAR(50)     UNIQUE NOT NULL,
    password    VARCHAR(100)     NOT NULL,
    email       VARCHAR(100)    NOT NULL,
    avatar_url  VARCHAR(250)    DEFAULT '',
    create_time TIMESTAMP       DEFAULT CURRENT_TIMESTAMP,
    INDEX   idx_email(email)
);