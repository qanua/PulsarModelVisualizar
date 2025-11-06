# Pulsar Model Visualizar
## 概要
このプログラムは、パルサーという星の放射モデルを可視化するために制作しました。
計算部分は C++ で実装し、表示部分は C# を用いて OpenTK による3Dレンダリング処理を実装しています。
マウス操作によるカメラ移動や、GUI操作によるパラメータ変更などができます。

### 開発背景
このプログラムは、大学院での研究内容を再現するために個人的に開発したものです。
当時は計算部分を Fortran で実装し、その結果を gnuplot を用いて表示していました。
しかし将来的に、表示部分を実装することでより直感的に計算結果を眺められるようにしたいという思いがありました。
そこで、数年前から少しずつ開発を行ってきました。


## 開発環境
* [開発言語]　C#, C++
* [構成]
  * C#
    * WPF（GUI構築）
    * OpenTK（3D描画）
  * C++ 
    * Native DLL（数値計算）
* [IDE]　Visual Studio 2022
* [OS]　Windows 11 Home 24H2
* [GPU]　NVIDIA GeForce RTX 4060 Ti
* [OpenGL]　4.6 (ドライバ依存) 


## 実行方法
### 実行ファイルから
1. `dev/build/x64/Release/net8.0-windows` 内の `WpfApp.exe` を実行

### Visual Studio から
1. Visual Studio 2022 を起動
2. `PulsarModelVisualizar.sln` を開く
3. ソリューション構成を **Debug / Release** に設定
4. プラットフォームを **x64** に設定
5. **ソリューションのビルド**（Ctrl + Shift + B）
6. **WpfApp** をスタートアッププロジェクトに設定して実行（F5）


## ディレクトリ構成
```
PulsarModelVisualizar/
 ├─ dev/
 │   ├─ build/
 │   │   └─ x64/
 │   │        ├─ Release/
 │   │        │   ├─ WpfApp.exe       # 実行ファイル
 │   │        │   ├─ CalcLib.dll      # Native DLL
 │   │        │   └─ その他依存 DLL
 │   │        └─ Debug/
 │   ├─ WpfApp/                       # GUI部ソースコード（C#）
 │   ├─ CalcLib/                      # DLL部ソースコード（C++）
 │   └─ PulsarModelVisualizar.sln
 ├─ doc/
 │   ├─ images/                       # UML図
 │   └─ PulsarModelVisualizar.asta    # 設計ファイル
 ├─ DESIGN.md                         # UML図一覧
 └─ README.md
```


## 基本設計
設計の詳細は [DESIGN.md](DESIGN.md) を参照ください。

<img alt="01_クラス図_基本設計" src="doc/images/class_diagram_01.png" width=60%>


* WpfApp : 表示部分のモジュール（C#）
* CalcLib : 計算部分のモジュール（C++）

### クラス構成
<img alt="02_クラス図_全体" src="doc/images/class_diagram_02.png" width=100%>

* WpfApp
  * MainWindow : WPF を用いたGUI構築、OpenTK による3D描画を行う
    * Camera : カメラ設定を保持する
    * Object : 頂点データなど、描画情報を保持する
* CalcLib
  * CalcManager : 計算結果を返すなど、計算処理を管理する
  * PulsarAsset : パルサーの各種データ
    * LCFL : 放射領域を定義する磁力線（LastClosedFieldLine）の点群データ
    * Pulse : 放射により観測されるパルス波形の点群データ
    * SkyMap : 放射の位相マッピングの点群データ
  * ModelCalculator : モデル計算を行う
    * CalculationAssets : モデル計算に必要なデータ
      * PolarCapDirection : 磁力線の描画始点の方角
      * MagneticLineState : 磁力線の開閉状況
    * MagneticIntegrationParams : 磁場の積分計算に必要なパラメータ
* Vector3D\<T> : 3次元ベクトルのクラステンプレート
* Vector2D\<T> : 2次元ベクトルのクラステンプレート


## 画面仕様


## 機能仕様


## 工夫した点


## 今後の展望


