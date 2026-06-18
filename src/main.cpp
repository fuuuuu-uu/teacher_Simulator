
#include "../include/GameObjects.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm> // 包含 std::shuffle & 給getli增強用
#include <random>    // 包含 std::default_random_engine
#include <ctime>
#include <stdlib.h>
#include <direct.h> // Windows 專用 ->_getcwd
#include <limits>   //使用無上限清除的數值巨集 ->清除緩衝區殘值

using namespace std;
void finalShow(int success, int mood, int prestige)
{ // 結算畫面
    cout << "-------------* 結算 time !! *-------------" << endl;
    cout << "基於你本節課的表現 ，我方給予此評價... : " << endl;
    cout << "總授課成功度 : " << success << endl;
    cout << "總教室氛圍度 : " << mood << endl;
    cout << "總教師威望值 : " << prestige << endl;
    cout << "根據董事會決議，你本節課，總評級為... : ";

    int finalcount = success + mood + prestige;
    switch (finalcount / 30)
    {
    case 10:
    case 9:
        cout << " A+ ！！" << endl;
        break;
    case 8:
    case 7:
        cout << " A ！" << endl;
        break;
    case 6:
    case 5:
        cout << " B " << endl;
        break;
    case 4:
    case 3:
        cout << " C " << endl;
        break;
    default:
        cout << " D-  :( " << endl;
        break;
    }
}

// 實作 try-catch 的轉換防呆
int stoiExamine(string ss)
{ // 避免stoi函式讀取時讀到非數字字串
    try
    {
        if (ss.empty())
            return 0;
        return stoi(ss);
    }
    catch (...)
    {
        return 0;
    }
}

// 分割檔案字串
string getli(stringstream &ss)
{ // 把從檔案讀取的值分割成字串
    string cell;
    if (!getline(ss, cell, ','))
        return "";

    cell.erase(cell.find_last_not_of(" \n\r\t") + 1);
    cell.erase(0, cell.find_first_not_of(" \n\r\t"));
    return cell;
}

// 讀取 CSV 檔案並建立多型物件 分"緊急" 和 "一般"
void loadAllEvents()
{
    try
    {
        ifstream file("event/events.csv");
        if (!file.is_open())
        {
            throw runtime_error("無法開啟 events.csv 檔案，請檢查路徑是否與執行檔在相同目錄！");
        }
        string line;
        int id;
        getline(file, line); // 去除標題欄位

        while (getline(file, line))
        {                          // 先讀取一整行
            stringstream ss(line); // 把這一行變成一個「小流」，方便再切分
            string leval, titleFromCSV, descFromCSV;
            Option temp;

            id = stoiExamine(getli(ss)); // 抓出編號
            leval = getli(ss);           // 事件等級
            titleFromCSV = getli(ss);    // 標題
            descFromCSV = getli(ss);     // 內文

            temp.description = getli(ss); // 選項
            temp.result = getli(ss);
            temp.successChange = stoiExamine(getli(ss));
            temp.moodChange = stoiExamine(getli(ss));
            temp.prestigeChange = stoiExamine(getli(ss));
            temp.FailedResult = getli(ss);

            bool found = false;
            for (auto &e : eventPool)
            { // 對於 eventPool 裡面的每一個元素 e，都執行一次大括號裡的事
                if (e->getID() == id)
                {                       // 如果原本就在事件池
                    e->addOption(temp); // 把剛剛讀的選項與值加在後面
                    found = true;
                    break;
                }
            }
            if (!found)
            {                              // 沒有的話
                Event *newEvent = nullptr; // 建一個新事件 id = 剛剛抓的
                                           //  在讀取時，就根據等級決定建立一般事件還是緊急事件的實體（核心多型點！）
                if (leval == "緊急")
                { // new 動態記憶體配置
                    newEvent = new UrgentEvent(id, leval, titleFromCSV, descFromCSV);
                }
                else
                {
                    newEvent = new Event(id, leval, titleFromCSV, descFromCSV);
                }
                newEvent->addOption(temp);
                eventPool.push_back(newEvent);
            }
        }
        file.close();
    }
    catch (const runtime_error &e)
    {
        cout << "檔案讀取錯誤: " << e.what() << endl;
        exit(1); // 結束程式
    }
}

int main()
{
    // 強制將 Windows 終端機編碼改為 UTF-8 (65001)，並隱藏命令輸出
    system("chcp 65001 > nul");

    char buff[255];
    _getcwd(buff, 255);
    cout << "目前工作目錄: " << buff << endl;

    loadAllEvents(); // 載入事件庫

    unsigned int seed = time(NULL);
    srand(seed);   // 啟動傳統隨機數種子
    Stats myStats; // 玩家的總分

    if (eventPool.empty())
    {
        cout << "資料庫中沒有事件，程式結束。";
        return 0;
    }

    int star;              // 魔法小咒語:1234
    int surpriseCount = 1; // 賣萌 :)
    cout << "輸入 1234 後,開始模擬 : ";
    cin >> star;
    if (star != 1234)
    {
        cout << "重啟後可執行";
        // 提早退出前也要記得釋放記憶體
        for (auto e : eventPool)
            delete e;
        return 0;
    }
    // 核心改動：利用 std::shuffle 將整個事件池隨機打亂（洗牌）
    // 使用 random_engine 配合時間種子確保每次洗牌結果不同
    auto rng = std::default_random_engine(seed);
    std::shuffle(eventPool.begin(), eventPool.end(), rng);

    // int randomIndex = rand() % eventPool.size();
    // 原 V1.1 版本：隨機數除事件池的餘數->確保一定在範圍內

    // 決定要執行的事件數量，若池子事件不夠，則以池子大小為準
    int totalRounds = (eventPool.size() < 4) ? eventPool.size() : 4;

    for (int i = 0; i < totalRounds; i++)
    { // 4個事件

        Event *currentEvent = eventPool[i]; // 直接取得多型指標

        int OptionChoose = 0; // 使用者事件選擇的選項

        int NumofOption = currentEvent->EventShow();
        /* 可以引發和捕捉任何類型的異常（如基本類型、類型物件、指標等） */
        while (true)
        {
            try
            {
                if (!(cin >> OptionChoose))
                {
                    cin.clear();                        // 清除錯誤狀態
                    cin.ignore(1000, '\n');             // 忽略掉那串亂打的字
                    throw "輸入格式錯誤！請輸入數字。"; // throw丟給msg 透過引發一個 字串類型(const char*)的異常
                }
                if (OptionChoose > NumofOption || OptionChoose < 1)
                {
                    throw "選項超出範圍！";
                }
                break; // 輸入正確，跳出迴圈
            }
            catch (const char *msg)
            { // catch 捕捉和處理異常 catch在這裡捕捉到類型為 const char* 的異常，並輸出異常資訊。
                cout << "發生錯誤: " << msg << endl;
                if (surpriseCount > 3)
                {
                    cout << "這樣真的很好玩嗎 0.o ？是的，很好玩 0.<" << endl
                         << "但還是要";
                }
                cout << "重新選擇，範圍: 1 ~ " << NumofOption << endl;
                surpriseCount++;
            }
        }

        currentEvent->optionsChooses(OptionChoose); // 顯示選項的結果

        Option chosen = currentEvent->getResult(OptionChoose); // 從Event 裡抓這個事件整體到主程式

        if (chosen.description == "立刻辭職")
        {
            // 提早退出前也要記得釋放記憶體
            for (auto e : eventPool)
                delete e;
            return 0;
        }

        bool isSuccess = currentEvent->triggerChallenge(); // 這裡就會自動跳出擲硬幣或校準！

        if (isSuccess)
        {
            cout << "反應極快！教學效果翻倍！" << endl;
            // 將 chosen.successChange * 2 等等
            currentEvent->OptionsResult(OptionChoose, 2);
            myStats.success += chosen.successChange * 2;
            myStats.mood += chosen.moodChange * 2;
            myStats.prestige += chosen.prestigeChange * 2;
        }
        else
        {
            cout << "手忙腳亂...狀況變得更糟了。" << endl;
            // 數值不變
            currentEvent->OptionsResult(OptionChoose, 1); // 存取
            myStats.success += chosen.successChange * 1;
            myStats.mood += chosen.moodChange * 1;
            myStats.prestige += chosen.prestigeChange * 1;
        }

        cout << "\n--- 按 Enter 鍵繼續授課 ---" << endl;
        // 🎯 1. 先用一個 ignore 把上一步 (cin >>) 留在緩衝區最前面的那個舊 \n 吃掉
        cin.ignore(1000, '\n');

        // 🎯 2. 宣告一個暫存字串，並用 getline 強迫程式停下來，等待使用者按下 Enter
        // 不管使用者此時是單純按 Enter，還是調皮輸入了 "42" 再按 Enter
        // getline 都會把整行字（包含 "42\n"）全部讀進 dummy 變數裡，讓緩衝區維持 1000% 乾淨！
        string dummy;
        getline(cin, dummy);

        system("cls"); // 清空畫面
    }

    finalShow(myStats.success, myStats.mood, myStats.prestige);

    // 釋放動態記憶體
    // 專案結束，釋放 eventPool 中所有 new 出來的動態記憶體（防止 Memory Leak）
    for (auto e : eventPool)
    {
        delete e;
    }
    eventPool.clear();

    return 0;
}
