# 即时通讯客户端

## LoginDialog类
登录对话框从上往下：
1. 圆形label显示用户头像，没有历史用户显示一个默认头像
2. 用户名（下拉框），历史登录账号
3. 密码
4. 登录按钮
5. 注册账号和忘记密码

## RegisterDialog类
注册对话框从上到下：
1. label 显示欢迎注册用户
2. 伙伴(label+lineEdit):输入用户名
3. 伙伴(label+lineEdit):输入邮箱
4. 伙伴(label+lineEdit):输入密码
5. 伙伴(label+lineEdit):再次确认密码
6. 伙伴(label+lineEdit):接收验证码
7. 确认和取消

ui对象命名规范：类名_功能名，比如登录按钮button_login

## HttpManager类
1. 单例类，发送接受http请求
2. 使用信号通知其他界面 消息处理情况