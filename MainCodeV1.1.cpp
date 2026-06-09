#include <iostream>
#include <string>
#include <vector>            //一個可以調整大小的陣列,用來存選項
#include <stdlib.h> 
#include <ctime>            // 隨機數
#include <fstream>          //檔案
#include <sstream>           // 增加這個，處理字串切分超方便
#include <algorithm>         //給getli增強用
#include <direct.h> // Windows 專用 ->_getcwd
#include <chrono>  //校準的時間判斷

using namespace std;

struct Stats {
    int success = 50;
    int mood = 50;
    int prestige = 50;
};

struct Option {
    string description;      //選項文字
    string result;           //造成的結果
    int successChange;  //授課成功度增減
    int moodChange;     //教室氛圍度增減 
    int prestigeChange; //教師威望增減
};

class Event{
public:
    int eventID;             //事件編號
    string eventleval ;      //事件等級
    string title;            //標題
    string data_description; //描述文字
    vector<Option> options;  //建立一個Option類別的vector陣列，這裡存了這個事件所有的選項+選項帶來的增減值
public:
    Event():eventID(0){}
    Event(int i,string leval,string tit,string data):
    eventID(i),eventleval(leval),title(tit),data_description(data){}

    Event(int i,string leval,string tit,string data,vector<Option> optin): 
    eventID(i),eventleval(leval),title(tit),data_description(data),options(optin){}
    int EventShow(){
        cout << "--------* "<< "Title : "<<title<<" *--------"<<endl;
        cout << "Eventleval : "<<eventleval<<endl;
        cout << data_description << endl;

        for(size_t  i=0; i < options.size(); i++) {
            cout << i+1 << ". " << options[i].description << endl;
        }return options.size();
    }
    void optionsChooses(int i){ //將使用者選項對應到最終增減、輸出"造成結果"和"三項數值的增減"
        if(i == 4){
            cout << "----------------------------------"<<endl;
            cout << "你選擇了 : 4 -> 立刻辭職 欸 pwp"<< endl;
            cout << "結果 : 恭喜辭職，既然辭職了就別在意結果吧 :)"  << endl;
        }else{
            i = i -1;
            cout << "----------------------------------"<<endl;
            cout << "你選擇了 : " << (i+1) << " -> "<< options[i].description << endl;
        }
    }
    void OptionsResult(int i,int change){
            cout << "結果 : " << options[i].result << endl << "造成 : " << endl;
            cout <<"授課成功度 : "<<(options[i].successChange > 0 ? "+":"")<< options[i].successChange*change << endl;
            cout <<"教室氛圍度 : "<<(options[i].moodChange > 0 ? "+":"")<< options[i].moodChange*change << endl;
            cout <<"教師威望值 : "<<(options[i].prestigeChange > 0 ? "+":"")<< options[i].prestigeChange*change << endl;
    }
    Option getResult(int choice) {
        return options[choice - 1]; //索引值從 0 開始
    }
    virtual ~Event() {}     //確保父類別 Event 的解構子也是虛擬的，否則在刪除指標時會發生記憶體洩漏
    virtual bool triggerChallenge(){
        cout << "觸發隨機判定 ! -> 你在這件事中 : " ;
        //一般事件是擲硬幣
        return rand() % 2 == 0 ;  //== 0:ture
    }
};

/*宣告一個名為UrgentEvent的 class 類別 繼承自Event*/
class UrgentEvent : public Event {       //使用冒號 : 指定繼承方式與父類
public:     //緊急事件是觸發校準
    UrgentEvent(const Event& e) : Event(e) {}
    
    bool triggerChallenge() override {
        int randomNum = rand() % 10; 

        cout << "!!! 緊急校準 !!!" << endl;
        cout << "請在倒數 "<<randomNum<<" 秒結束後 "<<(double)randomNum + 1.5 <<" 秒前，輸入數字 "<< randomNum <<" 並按 Enter ..." << endl;
        //這裡寫一個簡單的計時邏輯或是純粹的機率比拼 
        // 檢查：1.數字要打對 2.時間不能超過 randomNum 秒 
        auto start = std::chrono::steady_clock::now();
        int guess;
        cin >> guess;
        auto end = std::chrono::steady_clock::now();
        auto diff = end - start;
        // 把 chrono 的時間轉成秒數
        double seconds = std::chrono::duration<double>(diff).count();
        return (guess == randomNum) && (seconds >= (double)randomNum && seconds <= (double)randomNum + 1.5); 
    }
};

vector <Event> eventPool;

void finalShow(int success,int mood,int prestige){              //結算畫面

    cout << "-------------* 結算 time !! *-------------"<<endl;
    cout << "基於你本節課的表現 ，我方給予此評價... : "<<endl ;
    cout << "總授課成功度 : "<< success << endl;
    cout << "總教室氛圍度 : "<< mood << endl;
    cout << "總教師威望值 : "<< prestige << endl;
    cout << "根據董事會決議，你本節課，總評級為... : ";

    int finalcount = success+mood+prestige;
    switch (finalcount /30){
    case 10 :
    case 9 :
        cout << " A+ ！！"<<endl;
        break;
    case 8 :
    case 7 :
        cout << " A ！"<<endl;
        break;
    case 6 :
    case 5 :
        cout << " B "<<endl;
        break;
    case 4 :
    case 3 :
        cout << " C "<<endl;
        break;
    default:
        cout << " D-  :( "<<endl;
        break;
    }
}

int stoiExamine(string ss){     //避免stoi函式讀取時讀到非數字字串
    string val = ss;
        if ((!val.empty() && isdigit(val[0])) || (val.size() > 1 && val[0] == '-')) {   //empty():檢查字串是否為空
            return stoi(val);
        } else {
            return 0; // 給予預設值，避免崩潰
        }
        return stoi(ss);
}

string getli(stringstream &ss){ //把從檔案讀取的值分割成字串
    string cell;
    if(!getline(ss, cell, ',')) return ""; // 讀取失敗回傳空

    // 移除字串前後的空白與不可見字元（如 \r, \n, tab）
    cell.erase(cell.find_last_not_of(" \n\r\t") + 1);
    cell.erase(0, cell.find_first_not_of(" \n\r\t"));
    
    return cell;
}

void loadAllEvents(){
    ifstream file("events.csv");
    string line;
    int id;
    getline(file, line);        //去除標題欄位

    while(getline(file, line)){// 先讀取一整行
        stringstream ss(line);    // 把這一行變成一個「小流」，方便再切分
        string cell ,leval, titleFromCSV , descFromCSV;
        Option temp;

        id = stoiExamine(getli(ss));             //抓出編號
        leval = getli(ss);
        titleFromCSV = getli(ss);                //標題
        descFromCSV = getli(ss);                 //內文

        temp.description = getli(ss);            //選項
        temp.result = getli(ss);
        temp.successChange = stoiExamine(getli(ss));
        temp.moodChange = stoiExamine(getli(ss));
        temp.prestigeChange = stoiExamine(getli(ss));

        bool found = false;
        for (auto& e : eventPool){             //對於 eventPool 裡面的每一個元素 e，都執行一次大括號裡的事
            if (e.eventID == id) {             //如果原本就在事件池
                e.options.push_back(temp);     //把剛剛讀的選項與值加在後面
                found = true;
                break; 
            }
        }
        if (!found) {                   //沒有的話
            Event e1(id,leval,titleFromCSV,descFromCSV);               //建一個新事件 id = 剛剛抓的
            e1.options.push_back(temp);        //把剛剛讀的選項與值加在裡面
            eventPool.push_back(e1);
        }
    }
    file.close();
}

int main(){
    char buff[255];
    _getcwd(buff, 255);
    cout << "目前工作目錄: " << buff << endl;
    
    loadAllEvents();   //載入事件庫 
    srand(time(NULL)); // 啟動隨機數種子
    Stats myStats;     // 玩家的總分

    if (eventPool.empty()){
        cout << "找不到檔案";
        return 0;
    }

    int star;                  //魔法小咒語:1234
    int surpriseCount = 1;      //賣萌 :)
    cout << "輸入 1234 後,開始模擬 : ";
    cin >> star;
    if (star != 1234) {
        cout << "重啟後可執行";
        return 0;
    }
    
    int OptionChoose;                        //使用者事件選擇的選項
    for(int i=0 ;i<4;i++){                   //4個事件
        
        int randomIndex = rand() % eventPool.size();     //隨機數 除 事件池 後的餘數 -> 確保一定在範圍內

        Event& nowEvent = eventPool[randomIndex];
        
        int NumOfOption = nowEvent.EventShow();   //讀取選項總數
        /* 可以引發和捕捉任何類型的異常（如基本類型、類型物件、指標等） */
        while (true){
            try {
                if (!(cin >> OptionChoose)) {
                    cin.clear(); // 清除錯誤狀態
                    cin.ignore(1000, '\n'); // 忽略掉那串亂打的字
                    throw "輸入格式錯誤！請輸入數字。";    //throw丟給msg 透過引發一個 字串類型(const char*)的異常
                }   
                if (OptionChoose > NumOfOption || OptionChoose < 1) {
                    throw "選項超出範圍！";
                }
                break; // 輸入正確，跳出迴圈
            } catch (const char* msg) {     //catch 捕捉和處理異常 catch在這裡捕捉到類型為 const char* 的異常，並輸出異常資訊。
                cout << "發生錯誤: " << msg << endl;
                if(surpriseCount > 3){
                    cout << "這樣真的很好玩嗎 0.o ？是的，很好玩 0.<"<<endl<<"但還是要";
                }
                cout << "重新選擇，範圍: 1 ~ "<<NumOfOption<<endl;
                surpriseCount++;
            
        }
    }
        nowEvent.optionsChooses(OptionChoose); //顯示選項的結果
        if (OptionChoose == 4) return 0;

        Event* currentEvent; // 使用指標來達成多型

        if (eventPool[randomIndex].eventleval == "緊急") {  //緊急事件 = 去UrgentEvent這個子類別
            currentEvent = new UrgentEvent(eventPool[randomIndex]);
        } else {
            currentEvent = &eventPool[randomIndex];
        }
        Option chosen = nowEvent.getResult(OptionChoose);  //從Event 裡抓這個事件整體到主程式
        bool isSuccess = currentEvent->triggerChallenge(); // 這裡就會自動跳出擲硬幣或校準！

        if (isSuccess) {
            cout << "反應極快！教學效果翻倍！" << endl;
            // 將 chosen.successChange * 2 等等
            nowEvent.OptionsResult(OptionChoose,2);
            myStats.success += chosen.successChange *2;   //分數
            myStats.mood += chosen.moodChange*2; 
            myStats.prestige += chosen.prestigeChange*2; 
        } else {
            cout << "手忙腳亂...狀況變得更糟了。" << endl;
            // 將數值 * 1
            nowEvent.OptionsResult(OptionChoose,1);
            myStats.success += chosen.successChange*1;   //分數
            myStats.mood += chosen.moodChange*1; 
            myStats.prestige += chosen.prestigeChange*1; 
        }
        if (eventPool[randomIndex].eventleval == "緊急") {
            delete currentEvent; // 釋放 new 出來的記憶體
        }
        cout << "\n--- 按 Enter 鍵繼續授課 ---" << endl;
        cin.ignore(1000, '\n'); // 清除之前輸入留下的換行符
        cin.get();              // 等待使用者按下 Enter
        system("cls");          //清空畫面
    }
    finalShow(myStats.success,myStats.mood,myStats.prestige);
}
