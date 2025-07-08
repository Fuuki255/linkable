#include "linkable.hpp"
#include <string>

class DataRow : public TLinkable {
public:
	int id;
	std::string name;
	int salary;
	
	DataRow(int _id, const char*  _name, int _salary) {
		id = _id;
		name = _name;
		salary = _salary;
	}

    bool operator==(const char* find) {
        return name.compare(find) == 0;
    }
};

void printDataRow(TLinkableList<DataRow>& mylist) {
    for (DataRow* row : mylist) {
        printf("ID: %d, Name: %s, Salary: %d\n", row->id, row->name.c_str(), row->salary);
    }
}

int main() {
    DataRow* fuuki, *sakura, *ren, *nanami, *toma;

    // --- 初期化（合計5人）---
    
    // 配列初期化
    TLinkableList<DataRow> mylist;

    // データ追加
    fuuki = mylist.tlAdd(new DataRow(0, "fuuki", 36000));
    sakura = mylist.tlAdd(new DataRow(1, "sakura", 42000));
    ren = mylist.tlAdd(new DataRow(2, "ren", 39000));
    nanami = mylist.tlAdd(new DataRow(3, "nanami", 41000));
    toma = mylist.tlAdd(new DataRow(4, "toma", 38500));

    // 表示
    printf("Before edits:\n");
    printDataRow(mylist);


    // --- 削除処理 ---

    delete sakura;      // sakura を削除
    delete mylist[-1];  // 最後にいる toma を削除
    toma = NULL;


    // --- 移動処理 ---

    // 新しいデータの挿入
    mylist.tlInsertBefore(ren, new DataRow(5, "mei", 39500));   // 新しいデータを ren の前に挿入
    mylist.tlInsertAfter(3,  new DataRow(6, "kai", 37000));     // 3番目の nanami のあとに挿入
    
    // 内部移動
    mylist.tlInsertAfter(-1, fuuki);    // fuuki を一番後ろに移動する
    
    // 外部移動、
    // 同じオブジェクトは一つの配列だけ属するため、移動すると元の配列の最高がなくなる
    TLinkableList<DataRow> mylist2;
    mylist2.tlAdd(fuuki);
    
    mylist.tlInsertBefore(0, fuuki);    // mylist2 から mylist に戻る

    // オブジェクトを外す
    mylist.tlPop(0);        // fuuki は配列から外す
    delete fuuki;           // 自動解放されなくなるため、delete が必要

	// 編集後の表示
    printf("\nAfter edits:\n");
    printDataRow(mylist);

    // --- その他 ---

    // オブジェクト数を取得
    printf("\nArray Length: %d\n", mylist.tlLength());

    // オブジェクトが含まれているかのチェック、クラスに設定すれば任意のタイプでチェックすることができる
    printf("Is nanami in mylist? %s\n", mylist.tlIsContains("nanami") ? "true" : "false");

    // 自動解放があるが、tlClear で手動解放もできる
    mylist.tlClear();

    printf("\nAfter clear: Length = %d\n", mylist.tlLength());      // 解放されたら空き配列になる、続いて使用可能

    return 0;
}