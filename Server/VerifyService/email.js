const nodemailer = require('nodemailer');
const config = require('./config');


// 创建一个SMTP客户端对象
const transporter = nodemailer.createTransport({
    host: 'smtp.126.com',
    port: 465,
    secure: true,
    auth: {
        user: config.email_user,
        pass: config.email_passwd
    }
});

// 发送邮件
function sendEmail(email, code) {
    const mailOptions = {
        from: config.email_user,
        to: email,
        subject: 'Verification Code',
        text: `Your verification code is ${code}`
    };

    return new Promise((resolve, reject) => {

        transporter.sendMail(mailOptions, (error, info) => {
            if (error) {
                reject(error);
            } else {
                resolve(info.response);
            }
        });


    });

}

module.exports = { sendEmail };