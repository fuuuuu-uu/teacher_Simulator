
#include "../include/GameObjects.h"
#include <iostream>
#include <stdlib.h>
#include <ctime>
#include <chrono>
#include <limits>

using namespace std;

// 事件池
std::vector<Event *> eventPool;

Event::Event() : eventID(0) {}

Event::Event(int i, string leval, string tit, string data) : eventID(i), eventleval(leval), title(tit), data_description(data) {}

Event::~Event() {}

int Event::EventShow()
{
    cout << "--------* " << "Title : " << title << " *--------" << endl;
    cout << "Eventleval : " << eventleval << endl;
    cout << data_description << endl;

    for (size_t i = 0; i < options.size(); i++)
    {
        cout << i + 1 << ". " << options[i].description << endl;
    }
    return options.size();
}

void Event::optionsChooses(int i)
{ // 將使用者選項對應到最終增減、輸出"造成結果"和"三項數值的增減"
    if (options[i - 1].description == "立刻辭職")
    {
        cout << "----------------------------------" << endl;
        cout << "你選擇了 : 4 -> 立刻辭職 欸 pwp" << endl;
        cout << "結果 : 恭喜辭職，既然辭職了就別在意結果吧 :)" << endl;
    }
    else
    {
        i = i - 1;
        cout << "----------------------------------" << endl;
        cout << "你選擇了 : " << (i + 1) << " -> " << options[i].description << endl;
    }
}

void Event::OptionsResult(int i, int change)
{
    i = i - 1; // 修正索引值
    if (change == 1 && eventleval == "緊急")
        cout << "結果 : " << options[i].FailedResult << endl
             << "造成 : " << endl;
    else
        cout << "結果 : " << options[i].result << endl
             << "造成 : " << endl;
    cout << "授課成功度 : " << (options[i].successChange > 0 ? "+" : "") << options[i].successChange * change << endl;
    cout << "教室氛圍度 : " << (options[i].moodChange > 0 ? "+" : "") << options[i].moodChange * change << endl;
    cout << "教師威望值 : " << (options[i].prestigeChange > 0 ? "+" : "") << options[i].prestigeChange * change << endl;
}
int Event::getID()
{
    return eventID;
}
void Event::addOption(const Option &temp)
{

    options.push_back(temp);
}
Option Event::getResult(int choice)
{
    return options[choice - 1];
}

bool Event::triggerChallenge()
{
    // 一般事件是擲硬幣
    cout << "觸發隨機判定 ! -> 你在這件事中 : ";
    return (rand() % 2 == 0); //== 0:ture
}

UrgentEvent::UrgentEvent(int i, string leval, string tit, string data) : Event(i, leval, tit, data) {}

bool UrgentEvent::triggerChallenge()
{
    int randomNum = rand() % 5 + 2; // 隨機 2~6 秒

    cout << "!!! 緊急校準 !!!" << endl;
    cout << "請在倒數 " << randomNum << " 秒結束後 " << (double)randomNum + 1.5 << " 秒前，輸入數字 " << randomNum << " 並按 Enter ..." << endl;
    // 這裡寫一個簡單的計時邏輯
    //  檢查：1.數字要打對 2.時間不能超過 randomNum 秒
    auto start = std::chrono::steady_clock::now();
    int guess;
    cin >> guess;
    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;
    // 把 chrono 的時間轉成秒數

    // 🎯 【關鍵修正】QTE 輸入完數字後，立刻強行拔除它留在緩衝區的 \n
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    double seconds = std::chrono::duration<double>(diff).count();
    return (guess == randomNum) && (seconds >= (double)randomNum && seconds <= (double)randomNum + 1.5);
}
