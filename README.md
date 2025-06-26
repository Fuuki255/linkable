# Iterable - 自己リンク式イテレータベース

`iterable.h` は、単一のオブジェクトを配列化し管理するライブラリです、`std::list` と比べて整合性が高くパフォーマンスに強い配列を構成できる。

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



サンプル

```cpp
#include "iterable.h"

class IntIterable : public Iterable<IntIterable> {
public:
  int val;

  IntIterable(int _val) {
    val = _val;
  }
};

int main(int argc, char** argv) {
  // 配列とデータを作成, * がないだけどポインタだけが使える
  ObjectArray<IntIterable> array;
  IntIterable* num;

  num = array.Add(new IntIterable(1));

  for (int i = 2; i < 11; i++) {
    array.Add(new IntIterable(i));
  }

  // 配列をプリント
  printf("test1: ");
  for (IntIterable* val : array) {
    printf("%d, ", val->val);
  }
  putchar('\n');
  
  // カウント
  printf("length: %d\n", array.Length());

  // 3番目と最後のデータを取る
  printf("array[2] = %d\n", array.IndexOf(2)->val);
  printf("array[-1] = %d\n", array.IndexOf(-1)->val);

  // 要素を削除
  delete num; // 配列から 1 を削除
  array.DeleteIndex(7);      // 8番目を削除
  array.DeleteIndexes(3, 5); // 4番目から6番目を削除

  // データを挿入
  num = array.IndexOf(2);  // 提示用: 3番目をゲット

  array.InsertAfter(num /* オブジェクトそのままが使える */, new IntIterable(255));
  array.InsertBefore(0 /* 0 や NULL で最初に挿入する */, new IntIterable(100));
  array.InsertAfter(-1 /* -1 や NULL で最後に挿入する */, new IntIterable(100));


  // 変更をプリント
  for (IntIterable* obj : array) {
      printf("%d, ", val->val);
  }

  // 含む
  printd("array contains 0? %s\n", array.Contains(0) ? "true" : "false");
  printd("array contains 100? %s\n", array.Contains(100) ? "true" : "false");

  // 配列クリア（そうしなくとも自動解放される）
  array.Clear();

  return 0;
}
```

推奨用途

- UIレイアウトツリーやエンティティ階層
- 動的な削除と再配置が頻繁なゲーム内オブジェクトの管理
- ツリー/リスト構造を持つ管理機構の基盤実装

---

自由にプロジェクトに組み込んで、あなたのオブジェクトライフサイクルに力を貸します。
