# TLinkable - リンクオブジェクトテンプレート

`linkable.hpp` の `TLinkable` はオブジェクト自身がリストノートになるテンプレートで、 `std::list` に比べてメリットは:

- 性能上昇: オブジェクト自身がリストノートなので、対応のリストノートを探す手間が減り、メモリ割り当て一回で済む
- 自由な削除: 
- 上位構造にアクセス: `(T)TLinkable::tlGetOwner()` や `TLinkableList::tlGetOwner()`

クラス `Iterable` は継承でイテレーション構造をオブジェクト内に組み込むことで完璧な整合ができ、オブジェクトを `delete` するだけで配列のポインタが消え、オブジェクトから上位構造をアクセス、そしてパフォーマンスの上昇

主にゲームエンジンやUIレイアウトなど、動的なオブジェクト階層管理・削除処理・イテレーションに最適です。

**std::list と比べて Iterator のメリットは**

- 整合性：
  - `Iterable` を継承したクラスそのものがイテレータノードであり、配列のオブジェクト操作はオブジェクトをそのまま渡していい
  - 上位構造のポインターを配列に設定 (SetOwner) することで下のオブジェクトが上位構造にアクセス (GetOwner) ことができる
  - オブジェクトの削除に配列のポインターがいらない
  - 外部ライブラリいらない

- パフォーマンス：
  - `std::list` が二回のメモリ割り当てに比べ、`Iterable` は一回だけ
  - オブジェクトの移動や削除は std::vector のように後ろのポインターを書き直す必要ないし、std::list が先にオブジェクトのノードを探す必要もない

- メモリ安全性：
  - オブジェクトと配列は同期しているため、間違ったポインタにアクセスすることがない (std::vector や std::list と違う)


**デメリット**

- 各オブジェクトは 同時に1つの ObjectArray にしか所属できません (std::list, std::vector, ... に影響しない)
- Iterable 継承オブジェクトのシングルポインタだけが使える



## インストール

iterable.hpp をダウンロードし、`#include "iterable.hpp"` で導入すれば使用可能、コンパイルは以前のままでいい


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

## 予定アップグレード (2.0)

- Iterable から TwoLinkable に名前付け
- LeftLinkable と RightLinkable を追加する
- 内部名先が tl (TwoLinkable), ll (LeftLinkable と rl (RightLinkable) に

---

自由にプロジェクトに組み込んで、あなたのオブジェクトライフサイクルに力を貸します。
