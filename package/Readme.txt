WinKeyHook
==========

[概要]
・Windows のキーイベントをシステムレベルで取得して横取りするアプリケーションです。
・Windows ホットキーでは設定できないキーイベントにショートカットキーを割り当てることができます。
・扱いを間違えるとシステムを壊しかねないので自己責任で利用してください。※開発者推奨

[機能]
・起動するタスクトレイに格納されます。
・アプリケーションを終了する場合はタスクトレイからアイコンを選択 → ウィンドウメニューの"アプリケーションの終了" を選択してください。
・以下のキーイベント変換が実装されています。
	1, "Ctrl + Q" → "Alt + F4"
	2, "CapsLock" → "ALT" x 2
	3, "Ctrl + CapsLock" → "Shift" x 2

	※ 2と3は Clibor(https://forest.watch.impress.co.jp/library/software/clibor/)の画面呼出用

・タスク スケジューラにタスクを設定しておけば自動起動可能です。※"タスクスケジューラ登録方法"フォルダを参照


[開発動機]
・Macの "command + C" に比べて "Alt + F4" が不便だったので自分用に作成しました。

[ファイル詳細]
1, VC_redist.x64.exe	
	Visual Studio 2019 の Microsoft Visual C++ 再頒布可能パッケージ
2, WinKeyHook.exe		
	実行ファイル
3, KeyHook.dll
	システムキーイベントのフック用DLL

[開発環境]
・Visual Studio 2019/Visual C++
