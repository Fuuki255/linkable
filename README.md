# Iterable - 自己リンク式イテレータベース

`iterable.hpp` は、単一のオブジェクトを配列化し管理するライブラリです、`std::list` と比べて整合性が高くパフォーマンスに強い配列を構成できる。

クラス `Iterable` は継承でイテレーション構造をオブジェクト内に組み込むことで完璧な整合ができ、オブジェクトを `delete` するだけで配列のポインタが消え、オブジェクトから上位構造をアクセス、そしてパフォーマンスの上昇

主にゲームエンジンやUIレイアウトなど、動的なオブジェクト階層管理・削除処理・イテレーションに最適です。

**std::list と比べて Iterator のメリットは**

- 完璧な整合性：
  - `Iterable` を継承したクラスそのものがイテレータのため、オブジェクトの移動や削除にオブジェクトをそのまま渡していい
  - 親へのアクセスがあるゆえ、オブジェクトだけで上位構造を操作できる（IterableArray.owner）
  - オブジェクトが削除すると配列のアクセスも一緒に削除され、手間を減る

- パフォーマンスアップ：
  - `std::list` が二回のメモリ割り当てに比べ、`Iterable` は一回（ブロックのサイズよりも割り当ての回数が重要）
  - 配列をループしてオブジェクトを操作する必要ない

- メモリ安全性：
  - オブジェクトと配列の整合性により、オブジェクトが削除されて間違ったポインタにアクセスすることがない
  - `std::list` とは違い、配列が削除すると中のポインタも解放される


**デメリット**

- 各オブジェクトは 同時に1つの ObjectArray にしか所属できません (std::list, std::vector, ... に影響しない)
- Iterable 継承オブジェクトのシングルポインタだけが使える



## How to Install

Just download the iterable.hpp and include that.


**サンプル**

```cpp
#include "Includes/iterable.hpp"
#include <string>

class DataRow : public Iterable<DataRow> {
public:
	int id;
	std::string name;
	int salary;
	
	DataRow(int _id, const char*  _name, int _salary) {
		id = _id;
		name = _name;
		salary = _salary;
	}
};

int main() {
    IterableArray<DataRow> array;
    DataRow* fuuki, *sakura, *ren, *nanami, *toma;

    // --- 初期データ（合計5人）---
    fuuki = array.Add(new DataRow(0, "fuuki", 36000));
    sakura = array.Add(new DataRow(1, "sakura", 42000));
    ren = array.Add(new DataRow(2, "ren", 39000));
    nanami = array.Add(new DataRow(3, "nanami", 41000));
    toma = array.Add(new DataRow(4, "toma", 38500));

	// 配列プリント
    printf("Before edits:\n");
    for (DataRow* row : array) {
        printf("ID: %d, Name: %s, Salary: %d\n", row->id, row->name.c_str(), row->salary);
    }
    
    // --- 配列編集 ---

    // sakura を削除
    delete sakura;
    
    // 最後 (toma) を削除
    delete array[-1];
    toma = NULL;


    // ren の前に挿入
    array.InsertBefore(ren, new DataRow(5, "mei", 39500));
    // インデックス 3 (nanami) のあとに挿入
    array.InsertAfter(3,  new DataRow(6, "kai", 37000));
    
    // 内部移動
    array.InsertAfter(-1, fuuki);
    
    // 外部移動、
    // 設計的に同じオブジェクトは一つの配列だけ属するため、複数参考は起こりません
    IterableArray<DataRow> array2;
    array2.Add(fuuki);
    
    // 戻る
    array.InsertBefore(0, fuuki);


	// --- 編集後に表示 ---
    printf("\nAfter edits:\n");
    for (DataRow* row : array) {
        printf("ID: %d, Name: %s, Salary: %d\n", row->id, row->name.c_str(), row->salary);
    }

    printf("\nArray Length: %d\n", array.Length());

    // 自動解放前提 → Clear のみ
    array.Clear();

    printf("\nAfter clear: Length = %d\n", array.Length());

    return 0;
}
```

推奨用途

- UIレイアウトツリーやエンティティ階層
- 動的な削除と再配置が頻繁なゲーム内オブジェクトの管理
- ツリー/リスト構造を持つ管理機構の基盤実装

---

自由にプロジェクトに組み込んで、あなたのオブジェクトライフサイクルに力を貸します。
