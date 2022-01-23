# IC_Layout_Visual
![](https://i.imgur.com/svNeeuD.png)

Demo Video: https://youtu.be/UOOK9NKUdD0

## 編譯
### 環境:
1.	使用cmake幫忙編譯整個專案
2.	OS: Windows，使用的編譯器為msvc (裝Visual Studio 2019 就有了)
3.	使用git裡面的git submodule做套件管理
4.	C++版本: 17

因此在編譯專案之前要先裝好三個東西: cmake, Visual Studio 2019和Git.

### 如何編譯
1. 下載套件原始碼

此專案所使用的第三方套件都放在vendors的檔案夾裡，如果到vendors裡去點選一些套件的檔案夾會發現有些套件的檔案是空的。回到專案的根目錄有一個叫做”.gitsubmodules”的檔案，裡面的內容有各個套件的github網址。在根目錄執行:
```
git submodule update --init --recursive
```
git就會根據.gitsubmodules幫我們把各個套件的原始碼從github下載到vendors裡各個套件的資料夾中。

2. 建置整個專案

建議可以使用Visual Studio 2019當作IDE，裡面有支援cmake的功能可以幫忙編譯以及除錯。我個人是使用Visual Studio Code當作我的IDE，並且使用裡面cmake的套件，但是如果是第一次接觸Visual Studio Code可能會比較難上手。建議可以上網搜尋如何使用這些IDE幫忙開發cmake的專案，如果學會使用會方便很多。那這邊我就只示範不用IDE而是使用終端機指令的方法編譯(在這裡我預設大家是在windows環境執行，在windows執行cmake會預設選msvc當作編譯器。如果不是在windows上執行的話，請上網搜尋如何選擇使用msvc編譯器進行建置):

(1)	在根目錄建立一個新的資料夾”build”。我們所有編譯的結果都會放到build資料夾裡面。

(2)	到剛剛建立的build資料夾裡面執行:
```
cmake ..
```
這行指令會根據根資料夾裡的CMakeList.txt去生成一些建置時會用到的一些組態檔案。

(3)	一樣在build資料夾裡面執行:
```
cmake –-build . --config Release
```
這行指令會建置整個專案。如果是第一次建置，它一開始會先去建置所有的第三方套件，所以會比較久一點。在這邊我是使用Release組態去編譯的，所以產生出來的執行檔沒有除錯的功能，效能會是最好的。如果想要除錯的話指令就不用加—config Release，他預設會是Debug組態。

(4)	建置完成後build資料夾裡會有一個Release的資料夾，進去Release資料夾裡就會有編譯完成的ic_layout_visual.exe的執行檔。執行此檔案就可以開始使用軟體。(一開始執行的時候，各個視窗c會散亂的放在軟體各處，請自行拖拉視窗的大小，並擺放到想要的位置上面，之後執行會記憶視窗的擺放位置)。
