/---------------------------------------------------------------
| VERSION 1.0.0.1 第一版框架设计
| VERSION 1.0.0.2 增加第一和第二版UI界面设计
| VERSION 1.0.0.3 增加第三版UI设计和第一版模拟器设计
| VERSION 1.0.0.4 增加第二版模拟器设计
---------------------------------------------------------------/

1、js文件中：
    LineEdit:.text
    CheckBox:.checked
    Combobox:.currentText
2、  /*QDir::currentPath()*/
3、FloorBtn参数：13(按钮总个数);5x3(行列);2(按钮行间隔);1(按钮列间隔);1000(特效时间);
4、windows配置：只要在GlobalFun.h中修改WINDOWS宏定义即可。
   UBUNTU配置：1、修改宏定义
              2、主要大小写问题，比如《Phonon/Meobject》在ubunut下需要写成《phonon/meobject》
              3、去掉USB.pri
              4、
5、如果程序运行不成功，则可能是dll文件没有放在system32中的原因：在usb文件夹中。
6、增加新组件的方法：
    1、新建effectxxx类，并在effectinc中include
    2、在globalfun.h文件中的COM_TYPE中增加新的组件类型,在getComponetType、getComPath中添加此控件代码
    3、在boot.xml文件中增加此新组件（最好在subui中增加）
    4、在global.h文件AddComDom函数中添加
    5、在MainWindow.h-134行中添加新控件类型，在MainWindow.cpp中的AddTools/CreateRightFrame函数中添加代码
    6、ManageWidgetItem::start中添加代码
    7、在DiagramScene::OperateComDiagram中的update处增加代码
    8、在simulatorform代码InitUI()中添加代码
    9、在digramcom中的ComBaseWidget::InitEffect中增加代码
7、增加主题设置参数的方法：
    1、在diagramsetting.cpp中的EffectSetting::accept()中添加
    2、在diagramsetting.cpp中的EffectSetting::CreateUi()中添加
8、烧录工具设计说明
   打开软件后检查服务器（服务器端有个后台软件，使用与近场调试相同的网络协议，主要是执行客户机要求的查询和文件传输操作）是否连接，连接后进行下载相关镜像。
   点击连接按钮，并给测试地板上电，正常情况下可以连接uboot，开始正常的烧录内核操作，超时10s后没有建立连接请求，则给出提示。
9、Log4QT使用：
    首先配置IDE_Log.conf文件，修改log4j.appender.E.File项为运行路径下的RuntimeRc/Log/BST_IDE.log。
    注意，在Windows下运行路径一般为.../IDE/BST_IDE/，而不是.../IDE/BST_IDE/debug，要在“项目”->“运行”中修改。
    然后使用GetLoggerInfo（）初始化Log路径以及全局类，最后就可以使用Logger写日志了。

    IDE_Log.conf的查找从运行路径算起，而其中有关.log文件的路径从程序路径开始。

10、待修改
     Text区域根据HV参数设定缩放比例（宽高等比例缩放），然后进行特效操作
     clock数字时钟区域增加几种固定位置特效，模拟时钟解决旋转角度偏差270问题
     为treewidget继承新类
11、建立Demon和Display的环境：
    windows：
        1.在D:\MyProject\IDE\BST_DAEMON\debug\RuntimeRc目录建立“FSRC”、“TFlash0”、“USB0”三个文件夹，并在TFlash0中放入可用主题
        2.
12、新建一个用于QHash中KEY的类时，要添加==的重载运算以及在类外添加
    uint qHash(const NEWCLASS &key)
    {
        return (key.mPInterface << 16) + key.mLDevice; //>@只要可以获得唯一的数据即可。
    }
13、NFD文件没有更新。
    Global关于Mdp的没有更新。

1、log增加
2、

#SUBDIRS +=  libQtAV
#BST_IDE.depends += libQtAV
#libQtAV.file = QtAV/QtAV.pro
#include(QtAV/root.pri)
