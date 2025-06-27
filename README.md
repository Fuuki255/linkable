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

int main(int argc, char** argv) {
	IterableArray<DataRow> array;
	
	array.Add(new DataRow(0, "fuuki", 36000));
	array.Add(new DataRow(1, "sakura", 42000));
	array.Add(new DataRow(2, "ren", 39000));
    
	printf("dataset:\n");
	for (DataRow* row : array) {
		printf("ID: %d, Name: %s, Salary: %d\n", row->id, row->name.c_str(), row->salary);
	}

	return 0;
}
```

推奨用途

- UIレイアウトツリーやエンティティ階層
- 動的な削除と再配置が頻繁なゲーム内オブジェクトの管理
- ツリー/リスト構造を持つ管理機構の基盤実装

---

自由にプロジェクトに組み込んで、あなたのオブジェクトライフサイクルに力を貸します。
