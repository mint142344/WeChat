const grpc = require('@grpc/grpc-js');
const protoLoader = require('@grpc/proto-loader');
const path = require('path');
const sendEmail = require('./email').sendEmail;
const redisClient = require('./redis').redisClient;


// Load protobuf file
const PROTO_PATH = path.join(__dirname, '../message.proto');
const packageDefinition = protoLoader.loadSync(PROTO_PATH, {
    keepCase: true,
    longs: String,
    enums: String,
    defaults: true,
    oneofs: true
});

const emailVerifyProto = grpc.loadPackageDefinition(packageDefinition).message;

// Implement the verifyEmail RPC method
async function getEmailVerifyCode(call, callback) {
    const email = call.request.email;

    try {
        // 随机生成一个6位数的验证码
        const verificationCode = String(Math.floor(100000 + Math.random() * 900000));

        // 发送邮件
        await sendEmail(email, verificationCode);
        // 存储验证码到 Redis
        await redisClient.setEamilVerifyCode(email, verificationCode);

        callback(null, {
            status: "ok",
            code: verificationCode,
            error: ""
        });

        console.log(`Verification code sent to ${email}: ${verificationCode}`);
    } catch (error) {

        callback(null, {
            status: "error",
            code: "",
            error: error
        });
        console.log(`Error sending verification code to ${email}: ${error}`);
    }
}

function main() {
    const server = new grpc.Server();
    server.addService(emailVerifyProto.EmailVerifyService.service, { getEmailVerifyCode });

    const endpoint = "localhost:5001";
    server.bindAsync(endpoint, grpc.ServerCredentials.createInsecure(), (err, port) => {
        if (err) {
            console.error(err);
            return;
        }
        console.log(`Listening on ${port}`);
    });
}

main();