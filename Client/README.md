# 即时通讯客户端

## LoginDialog类
登录对话框从上往下：
1. 圆形label显示用户头像，没有历史用户显示一个默认头像
2. 用户名（下拉框），历史登录账号           -> leading icon
3. 密码                                 -> leading icon     
4. 登录按钮                             -> 页面跳转
5. 注册账号和忘记密码                     -> 页面跳转

## RegisterDialog类
- UI
注册对话框从上到下：
1. label 显示欢迎注册用户
2. 伙伴(label+lineEdit):输入用户名
3. 伙伴(label+lineEdit):输入邮箱
4. 伙伴(label+lineEdit):输入密码        -> 查看/隐藏密码
5. 伙伴(label+lineEdit):再次确认密码    -> 查看/隐藏密码
6. 伙伴(label+lineEdit):接收验证码      -> 重写QPushButton
7. 确认和取消                           -> 页面跳转
- 网络：获取验证码，确认注册

注册回调 -> `HttpManager` -> 分发Signal -> slot拿到response -> 回调处理响应
1. 构造函数注册 http 请求的各种处理回调
2. 连接`HttpManager`单例类的信号，槽函数分别对不同请求处理

ui对象命名规范：类名_功能名，比如登录按钮button_login

## HttpManager类
1. 单例类，发送http get/post请求
2. `HttpManager`类将信号分发到各模块(登录,注册, ...), 不同模块细分处理
3. 注册，忘记密码使用http