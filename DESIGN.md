# RK3568 人脸识别系统 — 详细设计方案 v2.0

> 人脸识别：摄像头实时取帧 → NPU 推理检测人脸 → 框出人脸 → 特征向量比对识别身份 → 报警联动

---

## 0. 硬件规格 (设计约束)

| 项目 | 规格 | 说明 |
|------|------|------|
| **开发板** | RK3568 ARM Cortex-A55 × 4 | NPU 1TOPS |
| **显示屏** | **1024 × 600** | FBDEV 单缓冲, 60Hz |
| **摄像头** | USB UVC (349c:0411) | YUYV 4:2:2 |
| **摄像头分辨率** | **640 × 480** @ 30fps | 位于 `/dev/video9` |
| **摄像头像素格式** | `V4L2_PIX_FMT_YUYV` | 单平面 API |
| **显示格式** | RGB565 (lv_canvas) | YUYV → RGB565 转换 |

### 0.1 布局适配原则

```
屏幕: 1024 × 600
  ├─ 所有页面: 全屏 1024×600, bg_color=浅粉 #fce4ec
  ├─ 摄像头页面:
  │   ├─ lv_canvas: 640×480, 居中定位 x=(1024-640)/2=192, y=52
  │   ├─ 按钮行: canvas 下方, y=480+52+12=544
  │   └─ 标题栏: 顶部居中 y=12
  ├─ 主菜单: 6 图标 3×2 网格, 每个 240×180, gap=28
  └─ 其他页面: 内容居中, 左上角 ← Back 按钮 (16,12)
```

### 0.2 人脸识别模型选择

| 方案 | 模型 | 推理速度 | 精度 | 可用性 |
|------|------|---------|------|--------|
| **当前最优** | `yolov8n.rknn` (已转换, 7.7MB) | ~80ms | COCO 80类含 person | ✅ 本地已有 |
| 后续升级 | `retinaface_mobilenet.rknn` | ~60ms | 专用人脸检测 | 需从 rknn_model_zoo 拉取 |
| 最终方案 | `yolov8n-face.rknn` | ~80ms | 人脸检测+关键点 | 需自行训练+转换 |

**第一阶段用 `yolov8n.rknn`**：检测 COCO "person" 类 (class_id=0)，效果等价于人脸检测。后续替换为专用 face 模型。

1. [系统架构](#1-系统架构)
2. [页面流程](#2-页面流程)
3. [目录结构](#3-目录结构)
4. [模块职责](#4-模块职责)
5. [数据存储](#5-数据存储)
6. [页面详细设计](#6-页面详细设计)
7. [人脸识别方案](#7-人脸识别方案)
8. [预加载管理](#8-预加载管理)
9. [报警联动](#9-报警联动)
10. [物理按键映射](#10-物理按键映射)
11. [建议与优化](#11-建议与优化)
12. [构建计划](#12-构建计划)
13. [当前状态确认](#13-当前状态确认)

---

## 1. 系统架构

```
┌──────────────────────────────────────────────────────────────────┐
│                    UI Layer (手写 C + LVGL v9)                    │
│  Splash │ Login │ Register │ MainMenu │ Camera │ Album │ Game    │
│  GameMenu │ Controls │ Recognition │ Settings │ FaceViewer       │
├──────────────────────────────────────────────────────────────────┤
│                     Logic Layer (C modules)                       │
│  auth.c │ camera.c │ hw_ctrl.c │ face_detect.c │ preload.c        │
│  game_snake.c │ game_2048.c │ album_mgr.c │ config.c             │
├──────────────────────────────────────────────────────────────────┤
│                   Hardware Abstraction                            │
│  V4L2 │ GPIO │ PWM │ Backlight │ NPU (RKNN) │ FBDEV │ EVDEV      │
├──────────────────────────────────────────────────────────────────┤
│                   Linux Kernel (RK3568)                           │
└──────────────────────────────────────────────────────────────────┘
```

---

## 2. 页面流程

```
设备上电 → 自动启动 lvglsim
  │
  ▼
Splash (GIF + 进度条 + 预加载)
  │ [自动完成 或 点击跳过]
  ▼
Login ──→ [忘记密码] → ResetPassword
  │         │
  │         ▼
  │       Register
  │         │ [注册成功自动填入账号]
  │         └→ Login
  │
  ▼ [登录成功]
MainMenu (动态图标 + 状态栏 + 齿轮)
  │
  ├─ Camera ──→ 实时预览 + 拍照 + 闪光灯
  ├─ Album  ──→ 缩略图浏览 + 视频播放 + 幻灯片
  ├─ Game   ──→ GameMenu → Snake / 2048
  ├─ Controls ─→ LED/蜂鸣器/背光/传感器
  ├─ Recognition ─→ 实时识别 + 人脸比对 + 报警
  └─ [齿轮] ──→ Settings → Controls + 模型切换 + 语言 + OTA

退出登录 → 确认弹窗 → Login
长按物理键 → 直接回 MainMenu
双击物理键 → 截图
```

---

## 3. 目录结构

```
src/
├── main.c                         # 入口: 杀系统 demo, 启动 preload, init LVGL
├── ui/
│   ├── ui.h                       # 所有页面 extern 声明 + 导航函数
│   ├── ui.c                       # 全局导航: destroy_all + LOAD 宏
│   ├── ui_helpers.c/h             # 工具: 消息框、键盘、确认弹窗
│   ├── ui_config.h                # 主菜单图标启用/禁用配置
│   └── screens/
│       ├── ui_Splash.c/h          # 开机动画 + 进度条 + 预加载回调
│       ├── ui_Login.c/h           # 登录 (完整功能)
│       ├── ui_Register.c/h        # 注册 (文件存储)
│       ├── ui_ResetPassword.c/h   # 忘记密码/修改密码
│       ├── ui_MainMenu.c/h        # 主菜单 (动态图标 + 状态栏)
│       ├── ui_Camera.c/h          # 摄像头页面
│       ├── ui_Album.c/h           # 相册 (缩略图 + 视频 + 幻灯片)
│       ├── ui_GameMenu.c/h        # 游戏选择菜单
│       ├── ui_Snake.c/h           # 贪吃蛇
│       ├── ui_2048.c/h            # 2048
│       ├── ui_Controls.c/h        # 硬件控制 (LED/蜂鸣器/背光)
│       ├── ui_Recognition.c/h     # 人脸识别主页面
│       ├── ui_FaceViewer.c/h      # 人脸注册/查看器
│       └── ui_Settings.c/h        # 设置 (齿轮进入)
│   └── assets/                    # 图片/GIF 素材 (不变)
├── lib/
│   ├── camera.h/c                 # V4L2 采集 + 拍照 (已有，保留)
│   ├── hw_ctrl.h/c                # 硬件控制 (已有，保留)
│   ├── auth.h/c                   # 账号密码文件存储
│   ├── face_detect.h/c            # RKNN 人脸检测推理
│   ├── preload.h/c                # 资源预加载管理器
│   ├── album_mgr.h/c              # 相册文件管理 (扫描/排序/缩略图)
│   ├── config.h/c                 # 配置文件读写 (启用/禁用功能)
│   ├── key_monitor.h/c            # 物理按键监听 (短按/长按/双击)
│   └── status_bar.h/c             # 状态栏 (时间/CPU/NPU)
├── models/
│   └── yolov8n_face.rknn          # 人脸检测模型
└── data/
    ├── accounts.dat               # 账号密码文件
    ├── scores.dat                 # 游戏最高分
    ├── config.ini                 # 配置文件
    └── alarm.log                  # 报警日志
```

---

## 4. 模块职责

| 模块 | 职责 | 依赖 |
|------|------|------|
| `auth.c` | 账号密码文件读写、验证、注册、修改密码、记住密码加密 | 无 LVGL |
| `camera.c` | V4L2 打开/启动/取帧(YUYV→RGB565)/拍照BMP/闪光灯 GPIO | 无 LVGL |
| `hw_ctrl.c` | GPIO 120-124 LED、GPIO 111 蜂鸣器、backlight sysfs | 无 LVGL |
| `face_detect.c` | 加载 RKNN→推理→人脸框+关键点→特征向量→比对 | 无 LVGL |
| `preload.c` | 后台预加载资源、进度回调、超时处理(5s)、日志 | 无 LVGL |
| `album_mgr.c` | 扫描目录→排序→缩略图缓存→文件删除 | 无 LVGL |
| `config.c` | 读/写 config.ini 控制主菜单图标启用/语言/亮度等 | 无 LVGL |
| `key_monitor.c` | EVDEV 按键监听: 短按返回、长按主页、双击截图 | EVDEV |
| `status_bar.c` | 定时读 /proc/stat (CPU) + NPU 状态 + 系统时间 | LVGL timer |

---

## 5. 数据存储

### 5.1 账号密码 (`/root/ljr/data/accounts.dat`)

```
格式: username:hash_salt\n
哈希: 简单 XOR+salt 混淆 (非明文)
上限: 50 个账号
```

```c
int  auth_init(void);                           // 初始化, 创建目录
int  auth_login(const char *u, const char *p);   // 返回 0 成功, -1 失败
int  auth_register(const char *u, const char *p); // 注册 (检查重复/长度)
int  auth_user_exists(const char *u);            // 检查存在
int  auth_change_password(const char *u, const char *old, const char *new);
char *auth_get_remembered(void);                 // 读记住的账号 (加密存储)
void  auth_save_remembered(const char *u, const char *p);
```

### 5.2 相册 (`/root/ljr/album/`)

```
snap_XXXX.bmp — 摄像头抓拍
face_XXXX.bmp — 人脸识别结果 (带水印)
rec_XXXX.mp4  — 录像 (拓展)
```

### 5.3 游戏评分 (`/root/ljr/data/scores.dat`)

```
snake_high=120
snake_easy=300
snake_hard=50
2048_high=4096
```

### 5.4 配置文件 (`/root/ljr/data/config.ini`)

```ini
[icons]
camera=1
game=1
controls=1
album=1
recognition=1

[display]
brightness=80
language=zh

[recognition]
model=face        ; face / security / vehicle
alarm_sound=1
alarm_led=1

[system]
auto_start=1
```

### 5.5 已注册人脸特征 (`/root/ljr/data/faces/`)

```
zhangsan.feat   — 128 维特征向量 (JSON 格式)
lisi.feat
```

---

## 6. 页面详细设计

### 6.1 Splash (开机动画) — `ui_Splash.c`

**布局**: 全屏 GIF + 底部进度条 + 百分比文字

```
┌────────────────────────────────────────────────────────────┐
│                                                            │
│                   [ GIF 动画 全屏 ]                         │
│                  (lv_animimg, 9秒)                          │
│                                                            │
│  ██████████████████████████████████░░░░░░░░ 78%            │
│             正在加载资源...                                  │
│                                            「点击跳过 →」    │
└────────────────────────────────────────────────────────────┘
```

**逻辑**:

```
1. 创建 GIF (lv_animimg, 从 gif_anim 素材加载)
2. 创建进度条 (lv_bar, range 0-100)
3. 启动预加载管理器 (preload.c):
   - 图片解码 (0-30%)
   - 模型加载 (30-70%)
   - 缩略图缓存 (70-90%)
   - 字体缓存 (90-100%)
4. 预加载回调更新进度条:
   - 正常推进 → bar.set_value(pct)
   - GIF 播完但未完成 → 停在99%, 显示 "正在准备..."
   - 任一资源超时5s → 跳过, 打日志
5. 完成或用户点击 → ui_load_login_screen()
```

**过渡动画**: `LV_SCR_LOAD_ANIM_FADE_ON` (500ms 淡入)

---

### 6.2 Login (登录) — `ui_Login.c`

**布局** (已审核通过 HTML):

```
┌────────────────────────────────────────────────────────────┐
│ ← Back                                                     │
│                    🔐 人脸识别系统                           │
│                                                            │
│    ┌──────────────────────────────────────────────┐        │
│    │  登录                                         │        │
│    │                                              │        │
│    │  Username   [________________________]        │        │
│    │  Password   [________________________] 👁     │        │
│    │                                              │        │
│    │  □ Remember Me   □ Auto Login                 │        │
│    │                                              │        │
│    │  [🔓 LOGIN]    [Register →]                   │        │
│    └──────────────────────────────────────────────┘        │
│                    忘记密码？                                │
└────────────────────────────────────────────────────────────┘
```

**完整功能**:

| 功能 | 实现 |
|------|------|
| 账号密码输入 | 2 个 `lv_textarea`, one_line=true |
| 密码可见性 | "👁" 按钮切换 `lv_textarea_set_password_mode` |
| 记住密码 | checkbox → 登录成功保存到 `accounts.dat`，下次自动填充 |
| 自动登录 | checkbox + 记住密码勾选时 → 启动直接跳转 MainMenu |
| 错误限制 | 连续3次失败 → 禁用登录按钮30秒 + 倒计时显示 |
| 输入校验 | 账号/密码为空时登录按钮灰色 (LV_STATE_DISABLED) |
| Tab/Enter | `LV_KEY_NEXT`→光标切换, `LV_KEY_ENTER`→触发登录 |
| 登录验证 | `auth_login()` 比对文件中的账号密码 |
| 忘记密码 | 跳转 `ui_ResetPassword` — 回答预设问题验证后修改 |
| 注册跳转 | "Register →" 按钮 → `ui_load_register_screen()` |

---

### 6.3 Register (注册) — `ui_Register.c`

**完整功能**:

| 功能 | 实现 |
|------|------|
| 输入框 | Username + Password + Confirm Password |
| 密码强度 | 检测: 长度<6→弱, 有数字+字母→中, 含特殊字符→强 |
| 重复检查 | `auth_user_exists()` 查询, 存在→弹窗+清空 |
| 两次一致 | Confirm 失焦时校验, 不一致→红色提示 |
| 注册存储 | `auth_register()` 写入文件 |
| 注册成功 | 弹窗 "注册成功" → 自动跳回 Login + 填入账号 |
| 退出确认 | 输入框有内容时点 Back → 弹窗 "放弃注册?" |
| 账号上限 | 超50个 → 弹窗 "账号已满" |

---

### 6.4 MainMenu (主菜单) — `ui_MainMenu.c`

**布局**: 6 图标网格 + 顶栏状态 + 左上齿轮

```
┌────────────────────────────────────────────────────────────┐
│ ⚙           Main Menu                  🕐14:30 CPU:23% ⚙ │
│                                                            │
│  ┌────────┐ ┌────────┐ ┌────────┐                        │
│  │   📷   │ │   🎮   │ │   💡   │                        │
│  │ Camera │ │  Game  │ │Controls│                        │
│  └────────┘ └────────┘ └────────┘                        │
│                                                            │
│  ┌────────┐ ┌────────┐ ┌────────┐                        │
│  │   🖼   │ │   👤   │ │   🚪   │                        │
│  │ Album  │ │Recog.  │ │ Logout │                        │
│  └────────┘ └────────┘ └────────┘                        │
└────────────────────────────────────────────────────────────┘
```

**动态图标**: 从 `config.ini` 读取启用/禁用，自动排列 (支持翻页)

**状态栏**: `lv_timer` 每秒刷新: 系统时间 + CPU 使用率 + NPU 使用率

**齿轮菜单**: 点击 → `ui_load_settings()` → Controls + 语言 + 模型选择 + OTA

**退出确认**: 长按 Logout 图标或点 Logout → 弹窗 "确认退出?"

---

### 6.5 Camera (摄像头) — `ui_Camera.c`

**基于现有 camera.c，优化 UI**:

```
┌────────────────────────────────────────────────────────────┐
│ ← Back                           📷 Video Player   ⚡Flash │
├────────────────────────────────────────────────────────────┤
│                                                            │
│         ┌──────────────────────────────┐                   │
│         │                              │                   │
│         │    640×480 lv_canvas         │                   │
│         │    (RGB565 实时预览 + 人脸框)  │                   │
│         │                              │                   │
│         └──────────────────────────────┘                   │
│                                                            │
│          [📸 Snap]  [▶ Start]  [⏹ Stop]                    │
│                  Swipe to switch                            │
└────────────────────────────────────────────────────────────┘
```

**闪光灯**: 独立 GPIO 控制，摄像头预览区域外围 LED 补光

---

### 6.6 Album (相册) — `ui_Album.c`

**完整功能**:

| 功能 | 实现 |
|------|------|
| 缩略图网格 | 首次进目录→后台生成缩略图缓存到 `/tmp/thumb/` |
| 文件浏览 | 按文件名排列，上一页/下一页 |
| 视频播放 | 检测 `.mp4` → 调用 `mplayer -fs` 或 gstreamer 全屏播放 |
| 幻灯片 | 按钮切换自动播放模式，3s/5s/10s 可设间隔 |
| 删除 | 长按→弹菜单→删除 (重命名为 `.deleted` 待确认) |
| 空目录 | 显示 "暂无内容，去拍照吧!" + 跳转按钮 |
| 排序 | 按时间/按名称切换 |

---

### 6.7 Game (游戏) — `ui_GameMenu.c` / `ui_Snake.c` / `ui_2048.c`

**游戏菜单**:

```
┌────────────────────────────────────────────────────────────┐
│ ← Back                          🎮 Game Center             │
│                                                            │
│       ┌─────────────────┐   ┌─────────────────┐            │
│       │      🐍         │   │      🔢         │            │
│       │     Snake       │   │     2048        │            │
│       │  最高分: 120    │   │  最高分: 4096    │            │
│       │  [Easy][Hard]   │   │  [Easy][Hard]   │            │
│       └─────────────────┘   └─────────────────┘            │
└────────────────────────────────────────────────────────────┘
```

**贪吃蛇**:
- lv_canvas 绘制网格 (20×15)
- 支持划屏控制方向 (LV_EVENT_GESTURE)
- 吃食物加分 + 蜂鸣器短响
- 撞墙/撞自己 → 蜂鸣器长响 + 显示 Game Over + 最高分

**2048**:
- lv_canvas 绘制 4×4 网格 + 数字方块 (颜色随数值变)
- 手势检测 LV_EVENT_GESTURE (上下左右滑动)
- 达到 2048 → 弹窗 "You Win!"
- 无可用移动 → Game Over

**共同功能**: 开始前选难度、暂停/继续、记录最高分到文件

---

### 6.8 Controls (控制面板) — `ui_Controls.c`

**与现有 hw_ctrl.c 配合**:

```
┌────────────────────────────────────────────────────────────┐
│ ← Back                            💡 Controls              │
│                                                            │
│  LED 0 (GPIO120)           [Switch]   ON                   │
│  LED 1 (GPIO121)           [Switch]   OFF                  │
│  LED 2 (GPIO123)           [Switch]   OFF                  │
│  LED 3 (GPIO124)           [Switch]   ON                   │
│  Buzzer (GPIO111)          [Switch]   OFF                  │
│                                                            │
│  Backlight    🌙暗 ████████████░░░ ☀亮   65%               │
│                                                            │
│  [Light Sensor Auto] □ 启用自动调节                          │
└────────────────────────────────────────────────────────────┘
```

**背光反转**: 滑条左=暗(0%)，右=亮(100%) — 正常方向

**自动调节**: 读取 `/sys/bus/iio/devices/iio:device0/in_illuminance_input`（如果有亮度传感器），根据环境自动调背光

---

### 6.9 Image Recognition (人脸识别) — `ui_Recognition.c`

```
┌────────────────────────────────────────────────────────────┐
│ ← Back                       👤 Image Recognition          │
├────────────────────────────────────────────────────────────┤
│                                                            │
│       ┌──────────────────────────────────────┐             │
│       │                                      │             │
│       │    640×420 lv_canvas (实时预览)       │             │
│       │    ┌─────────┐  ┌─────────┐          │             │
│       │    │👤 张三  │  │👤 李四  │          │             │
│       │    │  94%    │  │  87%    │          │             │
│       │    └─────────┘  └─────────┘          │             │
│       │                                      │             │
│       └──────────────────────────────────────┘             │
│                                                            │
│  ○ Real-Time    ● Capture (拍照后识别)                      │
│  [📸 Capture & Recognize]   [⚙ ROI Set]   [👤 Register Face]│
│                                                            │
│  ⚡ Alarm: □ Sound  □ LED Flash  — on detection            │
└────────────────────────────────────────────────────────────┘
```

**两种模式**:

| 模式 | 行为 |
|------|------|
| Real-Time | 定时器连续取帧→RKNN 推理→结果叠加到 canvas |
| Capture | 点按钮→拍照→推理→结果标注→保存到 album→显示 2s 后渐隐 |

**人脸比对**:
1. 检测到人脸→提取 128 维特征向量 (FaceNet/ArcFace 轻量模型)
2. 与 `/root/ljr/data/faces/*.feat` 中已注册人脸比对
3. 余弦相似度 > 0.7 → 匹配成功 → 显示姓名+置信度
4. < 0.7 → 显示 "Unknown"

**报警联动**:
- 匹配到特定黑名单人脸 → 蜂鸣器响 500ms × 3 + LED 闪烁
- 记录日志: `时间|类别|姓名|置信度`

**ROI 区域**: 在预览上拖动两个角设置检测区域，仅该区域触发报警

**模型热切换**: Settings 中选择 (人脸/安防/车辆) → 后台异步加载对应 RKNN 模型

**水印**: 保存图片前叠加: `"2024-01-15 14:30:25 | Model: yolov8n-face | RK3568"`

---

### 6.10 Settings (设置) — `ui_Settings.c`

齿轮进入:

```
┌────────────────────────────────────────────────────────────┐
│ ← Back                            ⚙ Settings               │
│                                                            │
│  💡 Controls (LED/蜂鸣器/背光)    →                        │
│  🧠 Model: [Face ▼]              →                        │
│  🌐 Language: [中文 ▼]           →                        │
│  📡 OTA Update                   → (预留)                   │
│  ℹ About                         →                        │
└────────────────────────────────────────────────────────────┘
```

---

## 7. 人脸识别方案

### 7.1 模型选择: YOLOv8n-face

| 项目 | 值 |
|------|-----|
| 基础模型 | YOLOv8n (Nano, 6.2MB) |
| 训练数据 | WiderFace |
| 输出 | 人脸框 + 5 关键点 (眼/鼻/嘴角) + 置信度 |
| NPU 推理速度 | ~80ms/帧 |
| 精度 | mAP 92% @ WiderFace Easy |

### 7.2 转换流程

```
yolov8n.pt → ultralytics 训练 face → export ONNX (RKNN opt)
→ RKNN-Toolkit2 量化 → yolov8n_face.rknn
```

### 7.3 推理管线

```
YUYV 帧 (640×480×2)
  → YUYV→RGB888 (640×480×3)
  → 缩放 letterbox → (640×640×3)
  → rknn_run() NPU 推理
  → 后处理: 解码框 + NMS
  → 框映射回 640×480 坐标
  → 人脸对齐 (用关键点仿射变换)
  → 特征提取 (可选 FaceNet 轻量模型)
  → 特征向量与注册库比对
  → 在 lv_canvas 上绘制框 + 标签
```

### 7.4 人脸比对特征存储

```
/root/ljr/data/faces/
  张三.feat    ← [128个float32小端序]
  李四.feat
```

比对算法: 余弦相似度

```c
float cosine_similarity(float *a, float *b, int dim) {
    float dot = 0, na = 0, nb = 0;
    for (int i = 0; i < dim; i++) {
        dot += a[i] * b[i];
        na += a[i] * a[i];
        nb += b[i] * b[i];
    }
    return dot / (sqrtf(na) * sqrtf(nb));
}
```

---

## 8. 预加载管理

```
preload.c 任务队列:

  Task 1: GIF 帧解码          (0-30%, 优先级 HIGH)
  Task 2: RKNN 模型加载        (30-70%, 优先级 HIGH)
  Task 3: 相册缩略图缓存       (70-90%, 优先级 MED)
  Task 4: 字体/图标缓存        (90-100%, 优先级 LOW)

超时: 单任务超过 5s → skip + 日志
99% 卡住: 显示 "正在准备..."，用户可点击跳过
```

```c
void preload_init(void);
int  preload_get_progress(void);  // 返回 0-100
const char *preload_get_status(void); // "加载模型中..."
int  preload_is_done(void);
void preload_force_finish(void);  // 强制跳过
```

---

## 9. 报警联动

| 触发条件 | 动作 |
|------|------|
| 人脸匹配到黑名单 | 蜂鸣器 500ms×3, LED 闪烁 1s |
| 连续3帧未授权 | 蜂鸣器连续响 + LED 常亮 |
| 报警日志 | 写入 `/root/ljr/data/alarm.log`: `时间|类型|姓名|置信度` |

```c
void alarm_trigger(alarm_type_t type, const char *name, float conf);
```

---

## 10. 物理按键映射

| 按键 | 动作 | 实现 |
|------|------|------|
| 短按 (<500ms) | 返回上一页 | `ui_load_prev()` 或 back_cb |
| 长按 (>1s) | 直接回 MainMenu | `ui_load_main_menu()` |
| 双击 (<300ms间隔) | 截图保存 | `Camera_Get_Pic()` |

通过 EVDEV 监听 `/dev/input/event*` 的 KEY 事件，在 `lv_indev_read_cb` 回调中处理。

---

## 11. 建议与优化

### 11.1 性能优化

1. **NPU 推理异步化**: 人脸推理在独立线程中运行 (pthread)，避免阻塞 UI
2. **Canvas 双缓冲**: 摄像头和识别结果分别渲染到两个 buffer，原子交换
3. **缩略图异步生成**: 相册首次加载时，后台线程生成缩略图，主线程先显示占位图标
4. **模型预加载**: Settings 切换模型时，后台异步加载，主界面不卡顿

### 11.2 用户体验

1. **全部浅粉配色**: 统一视觉风格，识别度更高
2. **按键防抖**: 所有按钮加 200ms 冷却，防止误触多次触发
3. **加载状态感知**: 任何耗时操作 (模型切换/预加载) 都显示 spinner 或进度条
4. **错误恢复**: 摄像头断连自动重试、模型加载失败回退 CPU 推理

### 11.3 安全性

1. **密码存储**: XOR + 随机 salt 混淆，虽非真正的加密但阻止明文泄露
2. **登录限制**: 连续错误禁用 + 日志记录尝试次数和时间
3. **配置文件权限**: `config.ini` 设为只读 (chmod 444)，防止运行时误改

### 11.4 可维护性

1. **模块零 LVGL 依赖**: camera.c、hw_ctrl.c、auth.c、face_detect.c 都是纯 C，可单独测试
2. **统一 LOAD 宏**: 所有页面导航集中管理
3. **配置文件驱动**: 新增功能只需改 config.ini + 添加页面文件，不改主菜单代码

---

## 12. 构建计划

| 阶段 | 内容 | 文件数 | 预计时间 |
|------|------|--------|---------|
| **零** | 清理项目: 删除旧 ui_Video/ui_Recognition 等，保留 camera.c/hw_ctrl.c + 素材 | - | 1h |
| **一** | Splash (preload) + Login + Register + ResetPassword + auth.c | ~10 | 3h |
| **二** | MainMenu (动态图标) + config.c + status_bar.c | ~5 | 2h |
| **三** | Camera (UI翻新) + Album (缩略图+视频+幻灯片) + album_mgr.c | ~6 | 3h |
| **四** | GameMenu + Snake + 2048 + scores.dat | ~5 | 3h |
| **五** | Controls (UI翻新) + Settings + key_monitor.c | ~5 | 2h |
| **六** | Recognition + face_detect.c + 人脸特征存储 + 报警 | ~5 | 4h |
| **七** | 多语言 + OTA 预留 + 自启动 + 端到端测试 | ~4 | 3h |

---

## 13. 当前状态确认

- [x] 不保留扫雷 → 只做 Snake + 2048
- [x] 人脸识别用 YOLOv8n-face + 人脸比对
- [x] camera.c / hw_ctrl.c 保留复用，UI 翻新
- [x] 账号密码存文件 (max 50)
- [x] Controls 从齿轮进入
- [x] 所有子页面: 浅粉背景 + 左上 ← Back 按钮
- [x] Login: 卡片完整包裹 + 忘记密码在卡片外
- [x] Register: 无忘记密码
- [x] 背光滑块: 左暗右亮 (正常方向)
- [x] 确认模型: 第一阶段用已有的 `yolov8n.rknn` (COCO person类)，后续升级 face 专用模型
- [ ] 确认视频播放方案: mplayer / gst-launch / gstreamer
- [ ] 确认是否有亮度传感器 (检查 `/sys/bus/iio/`)
