# Week 4：基础通信 + 启动与参数 · 学习日志

> 日期：YYYY-MM-DD ~ YYYY-MM-DD

## 本周目标

-

## 学到了什么

-## Day 1
- 创建 Publisher 的三项必要信息
消息类型（Message Type）：C++ 模板参数，决定序列化格式（本日用 std_msgs::msg::String）
Topic 名称：发布者与订阅者在 DDS 网络中互相发现、对齐的唯一凭证
QoS / 队列深度：可以是整数（语法糖，自动转换为"深度=N + 默认策略"的 QoS 对象），也可以是显式 rclcpp::QoS 对象（如 SystemDefaultsQoS()）；本日先用整数写法，QoS 细节留到 Day 2
- 为什么发布逻辑必须放在定时器回调里，而不是构造函数里
生命周期错位：构造函数执行时代码还未进入 spin()，事件调度器尚未接管节点
DDS discovery 时延：create_publisher 后，DDS 实体的发现报文（PDP/EDP）才刚开始交换，发布者与订阅者的握手、QoS 匹配需要几十到上百毫秒；此时发布的消息会直接丢失
实测验证：ros2 topic hz 刚启动时打印 WARNING: topic [...] does not appear to be published yet，正是这个 discovery 窗口期的直接证据，几十毫秒后 warning 自动消失
通过 create_wall_timer + spin()，等 discovery 完成后才持续周期性发布，避开了这个窗口期
（连接 Day 2 预告）该问题的另一种解法是 QoS 层面的 transient_local + keep_all：发布端为晚连接的订阅端缓存并补发历史消息
- executor.add_node() 传参：node vs node->get_node_base_interface()
两种写法都能编译通过，但语义不同
executor 内部只关心调度相关的最小契约，即 NodeBaseInterface（这是接口隔离原则的体现）
直接传 shared_ptr<Node> 是语法便利（内部依然会取 get_node_base_interface()）
显式传 get_node_base_interface() 更贴近底层设计意图：即便未来出现不继承 rclcpp::Node、但实现了 NodeBaseInterface 的类（如生命周期节点），也能被同一个 executor 无缝纳管
- const 成员函数与 const correctness
订阅回调写成 void topic_callback(...) const，将隐式 this 指针变为 const StatusSubNode*
函数体内所有非 mutable 成员被视为只读，若尝试修改成员变量（如 message_count_++）会编译报错
当前回调只读取 msg->data 并打日志，不修改节点状态，const 标注是对这份契约的显式声明，比教材原文更严谨

-## Day 2
- ros2 topic info 的分层输出
默认输出：Type / Publisher count / Subscription count
--verbose：额外展开 Node name、Node namespace、Topic type hash（Jazzy 新增的类型强校验字段，教材未覆盖）、GID、完整 QoS profile
- QoS 策略分两类，理解这一点是判断兼容性的前提
RxO（Request vs Offered）策略：会写进 DDS 发现协议、两端互相交换、参与兼容性判断——Reliability、Durability、Deadline、Liveliness
本地策略：只在进程内部生效，从不通过网络广播——History、Depth

实测验证：远程查询 ros2 topic info --verbose 时，History (Depth) 永远显示 UNKNOWN，因为这项从未被广播出去；只有节点自己知道自己配的深度。判断两端能不能连上，只看 Reliability 和 Durability，History/Depth 不参与这个判断。

- RxO 兼容性原则

发布者承诺的强度 ≥ 订阅者要求的强度，才能建立连接。强度排序：

Reliability：RELIABLE > BEST_EFFORT
Durability：TRANSIENT_LOCAL > VOLATILE

四种组合：

Publisher	Subscriber	结果
RELIABLE / TRANSIENT_LOCAL	同级或更低	兼容
BEST_EFFORT / VOLATILE	要求更高	不兼容，拒绝建立数据通路
- Reliability 与 Durability 的选型依据（结合机械臂场景推理）
Reliability：高频、可被新数据覆盖的连续流（如 /camera/image_raw）用 BEST_EFFORT——丢包时重传只会让订阅端拿到过时帧，没有意义；决定安全/逻辑判定的状态流（如 /joint_states）用 RELIABLE——丢包会导致轨迹跟踪异常或到位判定失效
Durability：高频连续覆盖型数据用 VOLATILE；低频/单次但后续所有节点都必须知道的全局状态（/map、/robot_description、/tf_static，以及讨论到的 /arm/emergency_stop）用 TRANSIENT_LOCAL
- "发现"与"匹配"是两个独立阶段

ros2 topic info 里的 Publisher/Subscription count 只反映 DDS 发现层面"端点存在"，不代表两端已经 QoS 匹配、能交换数据。真正决定能否通信的是匹配阶段的 QoS 兼容性判断——这也是本日排障过程中厘清的一个关键认识。

-## Day 3
- service 的主从式架构模型
service 是基于 Client-Server（C/S）架构的一次性请求-应答通信，与 topic 的多对多广播模型本质不同
同一 namespace + 同一 DDS 域下，一个 service 名字只允许有一个服务端，客户端可以有多个——这个限制的根本原因：
应答歧义：客户端的 Future/回调机制只支持"一次性交付"语义，第二个响应包到达时没有地方存放，会被静默丢弃（不是报错）
非幂等操作的重复执行风险：service 通常用于触发状态变更（如本次的"清零计数"，或实际系统中的"机械臂开合夹爪"），多服务端会导致同一操作被重复执行
topic 面向数据（Data-Centric，不关心听众数量），service 面向过程/控制（Function-Centric，必须权责明确、结果唯一）
建立可通信的 service 需要满足 5 项匹配条件：相同 service 名字、相同 service 类型、兼容的 QoS、相同 namespace、相同 DDS 域
service 默认 QoS 是 rmw_qos_profile_services_default（Reliable + Volatile）——不用历史补发，因为"补发一个过时的请求"在语义上没有意义（机械臂三秒前的移动指令现在收到，物理状态早已改变）
- service 服务端实现
create_service<ServiceT>(name, callback)，回调签名固定两个参数：
cpp
  void callback(
    const std::shared_ptr<ServiceT::Request> request,   // 只读，const
    std::shared_ptr<ServiceT::Response> response          // 待填写，非 const
  )
请求指针只读用 const，应答指针需要写入用非 const——与 Day 1 建立的"只读用 const"规则一致
用 std_srvs::srv::Trigger（内建、无请求字段）作为最简服务类型，比教材例子的 rcl_interfaces::srv::GetParameters 更贴合"单一动作触发"场景
- const correctness 的再权衡
topic_callback 原本标注 const（只读打日志），今天需要在其中给心跳计数自增，讨论了两种方案：
mutable 关键字：突破 const 成员函数的只读限制，但语义上更适合"逻辑常量性"场景（互斥锁、缓存），不适合业务状态
去掉 const（采用）：函数签名应如实反映"是否修改对象状态"，业务状态变更不应该用 mutable 硬凑
- 调试工具 ros2service（4.2.2）
ros2 service list -t：列出所有 service 及类型，验证了 /reset_heartbeat_count [std_srvs/srv/Trigger] 被正确发现
ros2 service call <name> <type> "<request_json>"：手动触发请求，Trigger 的空请求体用 "{}" 即可（因为 Request 部分不含字段）
ros2 service call 每次执行都会先打印 "waiting for service to become available..."——因为该命令会临时创建一个全新客户端节点，即便目标服务端已运行多时，这个新节点仍需完整走一遍 DDS discovery（PDP/EDP）握手，才能调用 wait_for_service() 成功返回
- 客户端异步应答处理（教材未讲全的部分）
教材原文 C++ 例子（代码 4-31）只调用 async_send_request 却完全不处理返回值，应答内容全靠服务端自己打日志"剧透"；真正的应答处理教材放在了 Python 的 future + spin_until_future_complete 里讲
rclcpp 提供的 C++ 对应写法：async_send_request(request, callback)，callback 参数类型是 rclcpp::Client<ServiceT>::SharedFuture（类似 std::shared_future 的包装类型），可以直接用 [this] 捕获的 lambda 实现，回调触发时 future.get() 必然已经 ready，不会阻塞
一次性工具型节点的生命周期设计（区别于 status_pub/status_sub 的"常驻型"节点）：
构造函数只做 create_client（纯粹的通信句柄初始化）
wait_for_service() 不依赖 spin——它直接轮询 DDS 图事件判断服务端是否上线，可以在 rclcpp::spin() 启动前调用并阻塞主线程
async_send_request 的应答回调必须等 spin() 启动后才能被调度触发
退出时机：回调函数内部处理完应答后调用 rclcpp::shutdown()，让 main() 里阻塞的 rclcpp::spin(node) 感知退出信号后自然返回，形成确定性闭环

-## Day 4
- launch 脚本的本质（3.2.1–3.2.2）
launch 系统在逻辑层面是进程编排器：把"手动开多个终端敲 ros2 run"翻译成"声明式拓扑描述 + 统一父进程托管"——launch 引擎作为根进程统一 fork/exec 子进程、聚合日志、级联下发 SIGINT
launch 脚本永远用 Python 写，跟被启动节点用什么语言实现完全无关——这是刻意的架构分层：节点实现层（C++/Python）决定"做什么"，编排层（永远 Python）决定"启动谁、传什么参数"。launch 引擎不关心也不需要知道自己拉起的是二进制还是脚本
install(DIRECTORY launch DESTINATION share/${PROJECT_NAME}/) 与 install(TARGETS ...) 的本质差异：前者是纯文件拷贝（Deployment），不依赖编译；后者是注册编译产物，必须先有二进制才能装。两个目录（launch、param）目标路径相同时可以合并成一条 install(DIRECTORY) 语句，遵循"单点维护"原则
同时 fork ≠ 同时构造完成：launch 把三个节点几乎同时拉起，但 status_client 依然能安全调用 status_sub 的服务，因为 wait_for_service(1s) 在客户端内部提供了时序解耦——launch 只保证"统一管理"，不保证"同步就绪"
- rclcpp 参数系统（3.3.1）
this->declare_parameter<double>("name", default) 是 rclcpp 特有的合并写法：一步完成"声明白名单"+"读取当前值（外部传了用外部的，没传用默认值）"，不像 rclpy 需要 declare_parameter + get_parameter 分两步。但如果要在运行时动态响应参数变化（add_on_set_parameters_callback），仍需显式读取，因为合并写法只在构造那一刻生效一次
类型转换坑：declare_parameter<double> 返回纯数字，create_wall_timer 需要带单位语义的 std::chrono::duration 类型，用 std::chrono::duration<double>(period_sec) 显式构造（duration 是模板类，不是只有 1s/500ms 这几个预定义字面量）
create_wall_timer 本身是模板函数，能同时接受 duration<double, ratio<1>> 和 duration<int64_t, milli> 等不同实例化，语法透明——跟 Week 2 学的模板"编译期按类型生成代码实例"是同一原理
- YAML 参数文件（3.3.2）
语法三条硬规则：缩进只能用空格（不能 Tab）、同层级左对齐、# 开头是注释
YAML 最外层的 key 必须是目标节点的名字，因为参数系统底层是靠节点自带的隐式 service（/<node_name>/set_parameters 等）实现的，YAML 本质是"投递清单"，靠节点名路由
关键坑——静默失效：--params-file 是节点在自己的 rclcpp::init 阶段读取的，如果节点当前名字（可能被 launch/命令行重命名过）在 YAML 里找不到匹配的顶层 key，不会报错，只会安静退回 declare_parameter 写的默认值。这类 bug 不会在日志里报警，只能靠"结果跟预期不符"倒推排查
- launch 中集成参数（3.3.3）
Node(..., parameters=[param_file]) 把 YAML 路径喂给节点，路径通过 get_package_share_directory('arm_basics') 获取，与 install(DIRECTORY ... DESTINATION share/${PROJECT_NAME}) 的安装路径一一对应
DeclareLaunchArgument('params_file', default_value=...) + LaunchConfiguration('params_file') 组合，把原本写死在 launch 文件里的 YAML 路径变成命令行可覆盖的启动参数，验证了 ros2 launch ... params_file:=xxx.yaml 能动态切换配置文件

## 卡在哪 / 怎么解决的

- transient_local + keep_all 晚到补发
修改 publisher QoS 为 rclcpp::QoS(0).keep_all().transient_local().reliable()
中途排障：status_pub 完全无输出，Ctrl+C 立即响应但无日志——定位到构造函数中 create_wall_timer 整行缺失（历史代码改动时误删），修复后恢复正常
Pub=TRANSIENT_LOCAL / Sub=VOLATILE：连接建立（Subscription count: 1），但订阅端只收新消息，不补发历史——验证"能连上 ≠ 能拿到全部数据"
Pub=TRANSIENT_LOCAL / Sub=TRANSIENT_LOCAL：订阅端启动瞬间收到 19 条历史消息（发布端已独立运行 19 秒），全部集中在约 1.2 毫秒内到达——用到达模式（而非时间戳，因 std_msgs::String 不携带时间信息）证明这是历史补发而非实时接收

- ament_target_dependencies(status_sub ...) 重复调用：两次声明同一 target 的依赖，合并为一次，讨论了"单点事实原则被破坏"的维护风险
- install(TARGETS ...) 合并方向反了：三段渐进式的 install(TARGETS)（分别装 1/2/3 个可执行文件）误删了内容最全的两段，只留最初最不完整的一段，导致 status_pub/status_sub 编译成功但 ros2 run 报 "No executable found"——编译（build）成功 ≠ 安装（install）完整，两者是独立步骤
- 改代码后忘记重新 colcon build：status_client.cpp 和 CMakeLists.txt 都改对了，但没有重新构建就直接 ros2 run，导致仍然找不到可执行文件
- 跨天遗留的 QoS 警告清理：rclcpp::QoS(0) 构造时先天产生"深度=0 但历史策略是 KEEP_LAST"的无意义中间态，即使后续链式调用 .keep_all() 也无法消除该警告；改用 rclcpp::QoS(rclcpp::KeepAll()) 从构造起点就直接是 KEEP_ALL 语义，警告消失

- 两次"忘记保存"：一次是 status_pub.cpp 改了 create_wall_timer 没保存导致行为没变；另一次更隐蔽——粘贴新版 bringup.launch.py 内容时误粘进了 pub_config.yaml，导致两个文件互相污染，colcon build 因为是纯资源拷贝（不涉及编译），即使内容错了也不会报错拦截，只会在耗时上出现"0.2 秒秒退"这种间接信号
- 修复方式：改用 cat > file << 'EOF' ... EOF 直接在终端里整体覆写文件内容，避免编辑器多窗口粘贴错位的风险
有趣的时序观察：发布周期压到 0.2s 后，status_client 的 reset 请求第一次与心跳消息的到达顺序产生了可观察的竞争（Reset count from 1 to 0，而不是 Day 3 时 1 秒周期下必然的 0 to 0）——印证了"多个独立事件的到达顺序不存在全局保证"这一分布式系统的基本特性

## 检查点是否通过

- [ ]

## 代码/产出链接

-

## 下周计划微调

-
