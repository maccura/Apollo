打印模板设计工具

Features
* Pure Qt4/Qt5 compatible code
* report designer
* report preview
* Various types for a report of any complexity
* Page header and footer
* Report elements: Text, Geometric (Line, Ellipsis, Rectangle), Picture
* An Automatic height adjustment
* Smart split moving data to the next page
* PDF output
	 
编译环境：
	 Windows平台 编译环境: Qt5.9.7 ,编译器: mingw53_32. 编译前需要先执行`pre_build.bat`. 

版本：1.1.04.240722
1.修复page只存在页眉或页脚渲染异常的问题。
2.优化报告渲染速度。
3.更新多语言资源。

版本：1.1.03.240621
1.解决表格空数据时导致的崩溃问题。

版本：1.1.02.240611
1.解决表格控件数据为1行时，导致的无限递归问题.

版本：1.1.01.240605
1.解决了部分编译警告。
2.qSort替换为std::sort

版本：1.1.01.240530
修改内容：
1.增加组功能
（1）增加元件编组。
（2）增加repeat属性、组变量。
（3）增加段前分页功能。
2.增加列表头控件
3.增加元件边界辅助线显示、元件边框属性。
4.表格控件选择自动填充后默认为两行，第一行显示变量，第二行用于拉伸间距。
5.多语言显示
（1）文本控件增加提示信息，用于解决显示多语言导致的内容显示不完整问题。
（2）增加文本元件显示最大长度。
6.优化报告渲染速度
7.增加了表格行高最大值限定（不超过单页显示区域）。

版本：1.0.00.240301
修改内容：
1.增加表格控件变长属性（行高自适应、行数自适应）、page布局自适应、page自动翻页、页眉页脚功能。
2.软件多语言增加英语、法语、俄语、西班牙语、意大利语。
3.pro文件增加QMAKE_CXXFLAGS_WARN_ON宏开关减少不必要警告。

版本：0.8.00.231229
修改内容：
1.支持打印模板设计。
2.软件语言目前支持中文。
	 
文件夹说明
|--target: exe生成文件夹，其中Release.rar为软件压缩包，可直接解压使用
|
|--doc：文档文件夹, 包含各种文档，软件使用说明文档为《打印模板设计工具使用说明书》
|			
|--src：源代码文件夹
|
|--config：配置文件夹，包含软件运行所需要的配置文件
|
|--rc：资源文件夹，包含软件中用到的图片、样式表等资源文件