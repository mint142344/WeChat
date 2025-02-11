const fs = require("fs");


let config = JSON.parse(fs.readFileSync("config.json", "utf-8"));

let email_user = config.email.user
let email_passwd = config.email.passwd


module.exports = { email_user, email_passwd };