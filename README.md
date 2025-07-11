# TLinkable - リンクオブジェクトテンプレート

オブジェクト自身がリストノートになるテンプレートで、より速い配列操作ができる、ダイナミックなオブジェクトに適している


| バージョン   | 3.0.0      |
| ------------ | ---------- |
| プログラマー | ふうき255  |
| 日付         | 2025/07/12 |


## インストール

iterable.hpp をダウンロードし、`#include "iterable.hpp"` で導入すれば使用可能、コンパイルパラメーターが追加しない


## メリット

- 性能： オブジェクトから直接操作でき、リストノートを探す手間が減る、メモリ割り当て数が一回で済む
- 自由： 配列側に気にせず、自由に移動や削除できる
- 上位構造にアクセス： オブジェクトから上位構造にアクセスでき。アクセスには `(T)yourLinkable.tlGetOwner()` や `Owner* yourList.tlGetOwner()`。設定は1回だけ、`Owner* yourList.tlSetOwner(Owner* owner)`

ゲームエンジン、データベースやUIレイアウトなど、動的なオブジェクト階層管理・削除処理・イテレーションに最適です。


## デメリット

- 各オブジェクトは 同時に1つの ObjectArray にしか所属できません (std::list, std::vector, ... に影響しない)
- Iterable 継承オブジェクトのシングルポインタだけが使える


---

## サンプル

```cpp
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
    printf("Is nanami in mylist? %s\n", mylist.tlGetEqual("nanami") ? "true" : "false");

    // 自動解放があるが、tlClear で手動解放もできる
    mylist.tlClear();

    printf("\nAfter clear: Length = %d\n", mylist.tlLength());      // 解放されたら空き配列になる、続いて使用可能

    return 0;
}
```

---

## オブジェクト操作

### 作成や削除

`new YourLinkable`
`delete YourLinkable`

### 独立化

`YourLinkable* tlUnlink()`

解放するとこのメソッドが自動的に実行する

### 前/後オブジェクト

`YourLinkable* tlPrev()`
`YourLinkable* tlNext()`

### 上位構造アクセス

`void* tlGetOwner()`


## 配列操作

### 作成と削除：

`LinkableList<YourLinkable, OwnerType> yourList`

自動解放、または `TLinkable& tlClear()`

### 追加と挿入：

`YourLinkable* ltAdd(YourLinkable* object)`
`YourLinkable* tlInsertBefore(YourLinkable* pos, YourLinkable* object)`
`YourLinkable* tlInsertAfter(YourLinkable* pos, YourLinkable* object)`

オブジェクトごと一つのリストだけに属する、リスト入れたオブジェクトがほかのリストに移動するとそのリストから消えます

### ループ：

`for (YourLinkable* object : yourList) {}`

途中にオブジェクトが削除されても影響しません

### 全てのオブジェクトを削除

`TLinkable& tlClear()`

### 数える

`int tlLength()`

### 同じものをゲット

`YourLinkable* tlGetEqual(AnyType value)`
`YourLinkable* tlGetEqual(YourLinkable* object)`

`YourLinkable::operator==()` を編集することで望むをゲットできる

---

## 予定アップグレード

なし

## ログ

### linkable.hpp (3.0.0)

- 一方向リンク式テンプレート `RLinkable` を追加、機能が限定されたが，より高いスピードが得られる
- `TLinkableList<>` や `RLinkableQueue<>` が C++ 初期化配列で構成できるようになった
- `bool IsContains(T)` から `YourLinkable* GetEqual(T)` になる
- `TLinkable` と `RLinkable` のベース拡張として、`IntLinkable`, `FLoatLinkable`, `StringLinkable` などを追加

### linkable.hpp (2.0.0)

- `Iterable<T>` から `TLinkable`, `IterableArray<T>` から `TLinkableList<Object, Owner = void>`
- `TLinkable` が多重継承することができるようになった
- メソッド名先 tl 付き、継承クラスからテンプレートメソッドが分けやすくなる
- `bool TLinkableList::tlIsContains(T value)` で複数チェックメソッドを追加

### iterable.hpp (1.0.0)

- `Iterable<T>`、`IterableArray<T>` 作成
- 基本メソッド追加

---

自由にプロジェクトに組み込んで、あなたのオブジェクトライフサイクルに力を貸します。
