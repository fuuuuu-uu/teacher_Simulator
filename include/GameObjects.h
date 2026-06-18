
#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H

#include <string>
#include <vector>

// 狀態結構
struct Stats
{
    int success = 50;
    int mood = 50;
    int prestige = 50;
};

// 選項結構
struct Option
{
    std::string description;  // 選項文字
    std::string result;       // 造成的結果
    int successChange;        // 授課成功度增減
    int moodChange;           // 教室氛圍度增減
    int prestigeChange;       // 教師威望增減
    std::string FailedResult; // 失敗的結果
};

class Event
{
private:
    int eventID;                  // 事件編號
    std::string eventleval;       // 事件等級
    std::string title;            // 標題
    std::string data_description; // 描述文字
    std::vector<Option> options;  // 儲存這個事件所有的選項

public:
    Event();
    Event(int i, std::string leval, std::string tit, std::string data);
    virtual ~Event(); // 虛擬解構子

    int EventShow();
    void optionsChooses(int i);
    void OptionsResult(int i, int change);
    int getID();
    void addOption(const Option &temp);
    Option getResult(int choice);
    virtual bool triggerChallenge(); // 虛擬函式以支援多型
};

// 子類別：緊急事件
class UrgentEvent : public Event
{
public:
    UrgentEvent(int i, std::string leval, std::string tit, std::string data);
    bool triggerChallenge() override; // 覆寫多型功能
};

// 宣告全域變數（使用 extern 避免多個檔案重複定義）
extern std::vector<Event *> eventPool;

#endif // GAMEOBJECTS_H
