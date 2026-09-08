#include <iostream>
#include <functional>

class Button {
public:
    // 注册一个回调,当按钮被点击时触发
    void setOnClick(std::function<void()> callback) {
        onClickCallback = callback;
    }
    
    void click() {  // 模拟按钮被点击
        // TODO: 触发回调
        if (onClickCallback) {
            onClickCallback();
        }
    }

private:
    // TODO: 需要一个成员变量存住回调,因为 setOnClick 和 click 是分开调用的
    std::function<void()> onClickCallback;
};

int main() {
    Button btn;
    int clickCount = 0;
    
    btn.setOnClick([&clickCount]() {
        clickCount++;
        std::cout << "Clicked! Count: " << clickCount << std::endl;
    });
    
    btn.click();
    btn.click();
}