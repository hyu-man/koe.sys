# koe.sys

こえも、規則がきめる。

ボイスチェンジャー。つまみは「たかさ」「かたち」「mix」の三つ、遅延は実測表示。
Windows スタンドアロン。配布パッケージは [BOOTH](https://vicugna.booth.pm/) にあります。

![koe.sys](Assets/icon.png)

## ビルド

```
git clone https://github.com/juce-framework/JUCE ../JUCE
cmake -S . -B build
cmake --build build --config Release --target KoeSys_Standalone
```

JUCE をこのリポジトリの**隣**（`../JUCE`）に置いてください。
ピッチ／フォルマントのエンジンは [Signalsmith Stretch](https://github.com/Signalsmith-Audio/signalsmith-stretch)（MIT、同梱）。
フォントは DotGothic16（OFL、同梱）。

## ライセンス

GPLv3。詳細は [LICENSE](LICENSE) と [THIRD_PARTY.txt](THIRD_PARTY.txt)。

---

ひゅーまん / ω （vicugna） — 規則の部屋: https://hyu-man.com/
